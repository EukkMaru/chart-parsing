#include "chart/reconstruction.hpp"

#include <cassert>
#include <limits>
#include <span>
#include <vector>

int main() {
    using chart::reconstruction::AirHoldCommand;
    using chart::reconstruction::AirHoldContactState;
    using chart::reconstruction::AirHoldPathPhase;
    using chart::reconstruction::AirHoldPathPoint;
    using chart::reconstruction::AirHoldStartPhase;
    using chart::reconstruction::AirHoldExclusionInterval;
    using chart::reconstruction::AirHoldGeneratedRecordKind;
    using chart::reconstruction::AirHoldPathGenerationDisposition;
    using chart::reconstruction::HoldGapState;
    using chart::reconstruction::SharedResultRoute;
    using chart::reconstruction::air_hold_active_path_phase;
    using chart::reconstruction::air_hold_authored_source_category;
    using chart::reconstruction::air_hold_exposes_candidate;
    using chart::reconstruction::air_hold_generated_source_category;
    using chart::reconstruction::air_hold_grid_tick;
    using chart::reconstruction::air_hold_in_exclusion_interval;
    using chart::reconstruction::air_hold_is_authored_checkpoint;
    using chart::reconstruction::air_hold_is_terminal;
    using chart::reconstruction::air_hold_path_complete;
    using chart::reconstruction::air_hold_runtime_checkpoint_count;
    using chart::reconstruction::air_hold_sample_step;
    using chart::reconstruction::air_hold_start_profile;
    using chart::reconstruction::air_hold_start_source_category;
    using chart::reconstruction::air_path_unsigned_tick_delta;
    using chart::reconstruction::filter_air_hold_path_emissions;
    using chart::reconstruction::evaluate_air_hold_path_generation;
    using chart::reconstruction::generate_air_hold_path_records;
    using chart::reconstruction::route_shared_result;
    using chart::reconstruction::update_air_hold_contact;
    using chart::reconstruction::update_air_hold_generated_checkpoint;

    assert(!air_hold_is_authored_checkpoint(AirHoldCommand::ahd));
    assert(air_hold_is_authored_checkpoint(AirHoldCommand::ahx));
    assert(air_hold_runtime_checkpoint_count(2, AirHoldCommand::ahd) == 2);
    assert(air_hold_runtime_checkpoint_count(2, AirHoldCommand::ahx) == 3);

    // Postprocessing uses a 384-tick major / 96-tick minor grid and halves its
    // cadence while the selected tempo-map value is below 4 * reference.
    assert(air_hold_grid_tick(AirHoldPathPoint{2.0F, 1.0F, 0.0F}) == 864);
    assert(air_hold_grid_tick(AirHoldPathPoint{
               std::numeric_limits<float>::infinity(), 0.0F, 0.0F}) ==
           std::numeric_limits<std::int32_t>::min());
    assert(air_hold_grid_tick(AirHoldPathPoint{
               std::numeric_limits<float>::quiet_NaN(), 0.0F, 0.0F}) ==
           std::numeric_limits<std::int32_t>::min());
    assert(air_hold_sample_step(4.0F, 1.0F) == 384);
    assert(air_hold_sample_step(1.0F, 1.0F) == 96);

    const std::vector<AirHoldPathPoint> anchors{
        {0.0F, 0.0F, 0.0F},
        {3.0F, 0.0F, 30.0F},
    };
    const std::vector<AirHoldPathPoint> decreasing_anchors{
        {0.0F, 0.0F, 0.0F},
        {-1.0F, 0.0F, -10.0F},
    };
    assert(evaluate_air_hold_path_generation({}) ==
           AirHoldPathGenerationDisposition::no_anchors);
    assert(evaluate_air_hold_path_generation(anchors) ==
           AirHoldPathGenerationDisposition::generated);
    assert(evaluate_air_hold_path_generation(decreasing_anchors) ==
           AirHoldPathGenerationDisposition::
               source_large_unsigned_span_expansion);
    const std::vector<AirHoldPathPoint> narrow_wrap_anchors{
        {5592405.0F, 0.0F, 0.0F},
        {std::numeric_limits<float>::infinity(), 0.0F, 1.0F},
    };
    assert(air_path_unsigned_tick_delta(
               air_hold_grid_tick(narrow_wrap_anchors[0]),
               air_hold_grid_tick(narrow_wrap_anchors[1])) == 128U);
    assert(evaluate_air_hold_path_generation(narrow_wrap_anchors) ==
           AirHoldPathGenerationDisposition::generated);
    const std::vector<AirHoldPathPoint> large_wrap_anchors{
        {-5592405.0F, 0.0F, 0.0F},
        {5592405.0F, 0.0F, 1.0F},
    };
    assert(evaluate_air_hold_path_generation(large_wrap_anchors) ==
           AirHoldPathGenerationDisposition::
               source_large_unsigned_span_expansion);
    const auto constant_tempo = [](float) { return 4.0F; };
    const auto point_at_tick = [](std::int32_t tick) {
        const auto major = tick / 384;
        const auto minor_ticks = tick - major * 384;
        return AirHoldPathPoint{static_cast<float>(major),
                                static_cast<float>(minor_ticks) / 96.0F,
                                0.0F};
    };
    const auto schedule_at_point = [](const AirHoldPathPoint& point) {
        return static_cast<float>(air_hold_grid_tick(point)) / 38.4F;
    };
    auto generated = generate_air_hold_path_records(
        std::span<const AirHoldPathPoint>{anchors}, 1.0F, constant_tempo,
        point_at_tick, schedule_at_point);
    assert(generated.size() == 3);
    assert(generated[0].kind == AirHoldGeneratedRecordKind::sample);
    assert(air_hold_grid_tick(generated[0].point) == 384);
    assert(!generated[0].emission_enabled);
    assert(generated[1].kind == AirHoldGeneratedRecordKind::sample);
    assert(air_hold_grid_tick(generated[1].point) == 768);
    assert(generated[1].emission_enabled);
    assert(generated[2].kind == AirHoldGeneratedRecordKind::path_end);
    assert(!generated[2].emission_enabled);

    const std::vector<AirHoldPathPoint> indefinite_anchors{
        {std::numeric_limits<float>::infinity(), 0.0F, 0.0F},
        {std::numeric_limits<float>::infinity(), 0.0F, 1.0F},
    };
    const auto indefinite_generated = generate_air_hold_path_records(
        std::span<const AirHoldPathPoint>{indefinite_anchors}, 1.0F,
        constant_tempo, point_at_tick, schedule_at_point);
    assert(indefinite_generated.size() == 1);
    assert(indefinite_generated.front().kind ==
           AirHoldGeneratedRecordKind::path_end);

    // Exclusion intervals are open on both ends and require selector zero.
    const std::vector<AirHoldExclusionInterval> intervals{
        {10.0F, 20.0F, 0},
        {30.0F, 40.0F, 1},
    };
    assert(!air_hold_in_exclusion_interval(
        10.0F, std::span<const AirHoldExclusionInterval>{intervals}));
    assert(air_hold_in_exclusion_interval(
        15.0F, std::span<const AirHoldExclusionInterval>{intervals}));
    assert(!air_hold_in_exclusion_interval(
        20.0F, std::span<const AirHoldExclusionInterval>{intervals}));
    assert(!air_hold_in_exclusion_interval(
        35.0F, std::span<const AirHoldExclusionInterval>{intervals}));

    // A nonnegative end margin applies only to final AHD. With one cadence of
    // margin, the second sample reaches the end and is suppressed.
    generated[0].emission_enabled = true;
    generated[1].emission_enabled = true;
    filter_air_hold_path_emissions(
        std::span{generated}, AirHoldCommand::ahd, anchors.back(), 1.0F, 1.0F,
        false, std::span<const AirHoldExclusionInterval>{}, constant_tempo);
    assert(generated[0].emission_enabled);
    assert(!generated[1].emission_enabled);

    generated[0].emission_enabled = true;
    generated[1].emission_enabled = true;
    filter_air_hold_path_emissions(
        std::span{generated}, AirHoldCommand::ahd, anchors.back(), 1.0F,
        std::numeric_limits<float>::infinity(), false,
        std::span<const AirHoldExclusionInterval>{}, constant_tempo);
    assert(generated[0].emission_enabled);
    assert(generated[1].emission_enabled);

    generated[0].emission_enabled = true;
    generated[1].emission_enabled = true;
    const std::vector<AirHoldExclusionInterval> filter_intervals{
        {9.0F, 11.0F, 0},
    };
    filter_air_hold_path_emissions(
        std::span{generated}, AirHoldCommand::ahx, anchors.back(), 1.0F, 1.0F,
        true, std::span<const AirHoldExclusionInterval>{filter_intervals},
        constant_tempo);
    assert(!generated[0].emission_enabled);
    assert(generated[1].emission_enabled);

    // AHD/AHX use the reset direction group of each supported root family.
    assert(air_hold_start_profile(1) == 0);
    assert(air_hold_start_profile(2) == 0);
    assert(air_hold_start_profile(13) == 0);
    assert(air_hold_start_profile(0) == 2);
    assert(air_hold_start_profile(4) == 2);
    assert(air_hold_start_profile(11) == 2);
    assert(air_hold_start_profile(6) == 4);
    assert(air_hold_start_profile(9) == -1);

    AirHoldContactState contact{};
    // Profile 7 alone cannot admit an initially held contact.
    assert(!update_air_hold_contact(contact, true, false, true));
    assert(!contact.admitted);
    // A profile-7 release latches admission, but is not itself active.
    assert(!update_air_hold_contact(contact, true, false, false));
    assert(contact.admitted);
    // Once admitted, a later profile-7 level is active without profile 6.
    assert(update_air_hold_contact(contact, true, false, true));
    // Profile 6 can admit profile 7 in the same update, but activity remains
    // suppressed before the scheduled start.
    AirHoldContactState profile_6_admission{};
    assert(!update_air_hold_contact(profile_6_admission, false, true, true));
    assert(profile_6_admission.admitted);
    assert(update_air_hold_contact(profile_6_admission, true, false, true));

    HoldGapState gap{
        .update_gate = 0.0F,
        .participation_floor = 0.0F,
        .end = 20.0F,
        .thresholds = {0.5F, 1.0F, 2.0F, 4.0F},
        .ever_active = true,
        .last_active = 1.0F,
        .current = 4.0F,
        .maximum_gap = 3.0F,
    };

    auto checkpoint = update_air_hold_generated_checkpoint(
        gap, false, true, false);
    assert(!checkpoint.consumed && gap.maximum_gap == 3.0F);

    // A disabled generated record is consumed without emitting or resetting.
    checkpoint = update_air_hold_generated_checkpoint(gap, true, false, false);
    assert(checkpoint.consumed && !checkpoint.emit);
    assert(!checkpoint.reset_maximum_gap && gap.maximum_gap == 3.0F);

    // An ordinary enabled record classifies the retained maximum, then keeps
    // only the currently open inactive streak.
    checkpoint = update_air_hold_generated_checkpoint(gap, true, true, false);
    assert(checkpoint.emit && checkpoint.ordinary_result_valid);
    assert(checkpoint.ordinary_result_byte == 1);
    assert(checkpoint.reset_maximum_gap && gap.maximum_gap == 3.0F);
    gap.last_active = 3.0F;
    gap.current = 4.0F;
    gap.maximum_gap = 3.0F;
    checkpoint = update_air_hold_generated_checkpoint(gap, true, true, false);
    assert(checkpoint.ordinary_result_byte == 1);
    assert(gap.maximum_gap == 1.0F);

    // Forced selection emits downstream but leaves ordinary gap state intact.
    gap.maximum_gap = 3.0F;
    checkpoint = update_air_hold_generated_checkpoint(gap, true, true, true);
    assert(checkpoint.emit && !checkpoint.ordinary_result_valid);
    assert(!checkpoint.reset_maximum_gap && gap.maximum_gap == 3.0F);

    assert(air_hold_active_path_phase(0) ==
           AirHoldPathPhase::best_current_gap);
    assert(air_hold_active_path_phase(-1) ==
           AirHoldPathPhase::other_current_gap);
    assert(air_hold_active_path_phase(2) ==
           AirHoldPathPhase::other_current_gap);
    assert(!air_hold_path_complete(1, 2, 2));
    assert(!air_hold_path_complete(0, 1, 2));
    assert(air_hold_path_complete(0, 2, 2));
    assert(!air_hold_is_terminal(AirHoldStartPhase::awaiting_result,
                                 AirHoldPathPhase::complete));
    assert(air_hold_is_terminal(AirHoldStartPhase::complete,
                                AirHoldPathPhase::complete));
    assert(!air_hold_exposes_candidate());

    assert(air_hold_start_source_category == 9);
    assert(air_hold_generated_source_category == 10);
    assert(air_hold_authored_source_category(0, 3) == 12);
    assert(air_hold_authored_source_category(1, 3) == 12);
    assert(air_hold_authored_source_category(2, 3) == 13);
    assert(route_shared_result(air_hold_start_source_category, 19, 4, 12,
                               false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(route_shared_result(air_hold_generated_source_category, 19, 4, 12,
                               false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(route_shared_result(air_hold_authored_source_category(0, 2), 19,
                               4, 12, false) ==
           SharedResultRoute::authoritative_aggregate);
}
    using chart::reconstruction::filter_air_hold_path_emissions;
    using chart::reconstruction::generate_air_hold_path_records;
