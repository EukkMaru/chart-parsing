#include "chart/reconstruction.hpp"

#include <cassert>
#include <limits>
#include <span>
#include <vector>

int main() {
    using namespace chart::reconstruction;

    assert(air_slide_is_asd_marker(AirSlideCommand::asd));
    assert(!air_slide_is_asd_marker(AirSlideCommand::asc));
    assert(air_slide_continuation_reference_matches(AirSlideCommand::asd,
                                                    AirSlideCommand::asd));
    assert(!air_slide_continuation_reference_matches(AirSlideCommand::asd,
                                                     AirSlideCommand::asc));

    const auto constant_tempo = [](float) { return 4.0F; };
    const auto point_at_tick = [](std::int32_t tick) {
        const auto major = tick / 384;
        const auto minor_ticks = tick - major * 384;
        return AirSlidePathPoint{
            static_cast<float>(major),
            static_cast<float>(minor_ticks) / 96.0F,
            0.0F,
            AirSlideCommand::asc,
        };
    };
    const auto schedule_at_point = [](const AirSlidePathPoint& point) {
        return static_cast<float>(air_slide_grid_tick(point)) / 38.4F;
    };
    const AirSlidePathPoint root{0.0F, 0.0F, 0.0F,
                                 AirSlideCommand::asc};
    assert(air_slide_grid_tick({
               std::numeric_limits<float>::infinity(), 0.0F, 0.0F,
               AirSlideCommand::asc}) ==
           std::numeric_limits<std::int32_t>::min());
    assert(evaluate_air_slide_segment_generation(0, 600, 384) ==
           AirSlideSegmentGenerationDisposition::generated);
    assert(evaluate_air_slide_segment_generation(0, -383, 384) ==
           AirSlideSegmentGenerationDisposition::no_interior_samples);
    assert(evaluate_air_slide_segment_generation(0, 600, 0) ==
           AirSlideSegmentGenerationDisposition::
               source_path_cursor_does_not_advance);
    assert(evaluate_air_slide_segment_generation(
               2147483520, 2147483520, 384) ==
           AirSlideSegmentGenerationDisposition::
               source_cursor_wrap_expansion);
    assert(evaluate_air_slide_cursor_advance(
               2147483520, 2147483600, 384) ==
           AirSlideSegmentGenerationDisposition::
               source_cursor_wrap_expansion);

    // ASC carries the cadence across its control point. The first sample after
    // the root is disabled, but the carried samples are enabled.
    const std::vector<AirSlidePathPoint> continuous_controls{
        {1.0F, 2.25F, 6.0F, AirSlideCommand::asc},  // tick 600
        {4.0F, 0.0F, 40.0F, AirSlideCommand::asc}, // tick 1536
    };
    auto continuous = generate_air_slide_path_records(
        root, std::span<const AirSlidePathPoint>{continuous_controls}, 1.0F,
        constant_tempo, point_at_tick, schedule_at_point);
    assert(continuous.size() == 4);
    assert(air_slide_grid_tick(continuous[0].point) == 384);
    assert(!continuous[0].emission_enabled);
    assert(air_slide_grid_tick(continuous[1].point) == 768);
    assert(continuous[1].emission_enabled);
    assert(continuous[2].kind == AirSlideGeneratedRecordKind::sample);
    assert(air_slide_grid_tick(continuous[2].point) == 1152);
    assert(continuous.back().kind == AirSlideGeneratedRecordKind::path_end);

    // Unlike AirHold/HeavenHold's unsigned span comparison, a decreasing
    // AirSlide control simply admits no interior samples and appends its end.
    const std::vector<AirSlidePathPoint> decreasing_controls{
        {-1.0F, 0.0F, -10.0F, AirSlideCommand::asc},
    };
    const auto decreasing = generate_air_slide_path_records(
        root, std::span<const AirSlidePathPoint>{decreasing_controls}, 1.0F,
        constant_tempo, point_at_tick, schedule_at_point);
    assert(decreasing.size() == 1);
    assert(decreasing.front().kind ==
           AirSlideGeneratedRecordKind::path_end);
    assert(air_slide_grid_tick(decreasing.front().point) == -383);

    const AirSlidePathPoint indefinite_root{
        std::numeric_limits<float>::infinity(), 0.0F, 0.0F,
        AirSlideCommand::asc};
    const std::vector<AirSlidePathPoint> indefinite_controls{{
        std::numeric_limits<float>::infinity(), 0.0F, 1.0F,
        AirSlideCommand::asc,
    }};
    const auto indefinite = generate_air_slide_path_records(
        indefinite_root,
        std::span<const AirSlidePathPoint>{indefinite_controls}, 1.0F,
        constant_tempo, point_at_tick, schedule_at_point);
    assert(indefinite.size() == 1);
    assert(indefinite.front().kind == AirSlideGeneratedRecordKind::path_end);

    // ASD inserts a boundary and restarts one step after the marked point.
    const std::vector<AirSlidePathPoint> restarted_controls{
        {1.0F, 2.25F, 6.0F, AirSlideCommand::asd},  // tick 600
        {3.0F, 3.0F, 37.5F, AirSlideCommand::asc}, // tick 1440
    };
    auto restarted = generate_air_slide_path_records(
        root, std::span<const AirSlidePathPoint>{restarted_controls}, 1.0F,
        constant_tempo, point_at_tick, schedule_at_point);
    assert(restarted.size() == 5);
    assert(restarted[1].kind ==
           AirSlideGeneratedRecordKind::segment_boundary);
    assert(air_slide_grid_tick(restarted[1].point) == 600);
    assert(!restarted[1].emission_enabled);
    assert(air_slide_grid_tick(restarted[2].point) == 984);
    assert(!restarted[2].emission_enabled);
    assert(air_slide_grid_tick(restarted[3].point) == 1368);
    assert(restarted[3].emission_enabled);

    assert(air_slide_authored_checkpoint_count(
               std::span<const AirSlidePathPoint>{restarted_controls}) == 1);
    assert(air_slide_authored_source_category(0, 2) == 12);
    assert(air_slide_authored_source_category(1, 2) == 13);

    // Final ASC enables the end-margin filter. Final ASD bypasses it.
    continuous[0].emission_enabled = true;
    continuous[1].emission_enabled = true;
    continuous[2].emission_enabled = true;
    filter_air_slide_path_emissions(
        std::span{continuous}, continuous_controls.back(), 1.0F, 1.0F,
        false, std::span<const AirHoldExclusionInterval>{}, constant_tempo);
    assert(continuous[0].emission_enabled);
    assert(continuous[1].emission_enabled);
    assert(!continuous[2].emission_enabled);

    continuous[0].emission_enabled = true;
    continuous[1].emission_enabled = true;
    continuous[2].emission_enabled = true;
    filter_air_slide_path_emissions(
        std::span{continuous}, continuous_controls.back(), 1.0F,
        std::numeric_limits<float>::infinity(), false,
        std::span<const AirHoldExclusionInterval>{}, constant_tempo);
    assert(continuous[0].emission_enabled);
    assert(continuous[1].emission_enabled);
    assert(continuous[2].emission_enabled);

    restarted[3].emission_enabled = true;
    AirSlidePathPoint final_asd = restarted_controls.back();
    final_asd.command = AirSlideCommand::asd;
    filter_air_slide_path_emissions(
        std::span{restarted}, final_asd, 1.0F, 10.0F, false,
        std::span<const AirHoldExclusionInterval>{}, constant_tempo);
    assert(restarted[3].emission_enabled);

    const std::vector<AirHoldExclusionInterval> intervals{
        {19.0F, 21.0F, 0},
    };
    continuous[1].emission_enabled = true;
    filter_air_slide_path_emissions(
        std::span{continuous}, continuous_controls.back(), 1.0F, -1.0F,
        true, std::span<const AirHoldExclusionInterval>{intervals},
        constant_tempo);
    assert(!continuous[1].emission_enabled);

    assert(air_slide_start_profile(1) == 0);
    assert(air_slide_start_profile(0) == 2);
    assert(air_slide_start_profile(6) == 4);

    AirSlideContactState contact{};
    assert(!update_air_slide_contact(contact, true, false, true));
    assert(!update_air_slide_contact(contact, true, false, false));
    assert(update_air_slide_contact(contact, true, false, true));

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

    // A disabled due type-8 record still performs the ordinary reset.
    auto checkpoint = update_air_slide_generated_checkpoint(
        gap, true, false, false);
    assert(checkpoint.consumed && !checkpoint.emit);
    assert(checkpoint.ordinary_result_valid);
    assert(checkpoint.ordinary_result_byte == 1);
    assert(checkpoint.reset_maximum_gap && gap.maximum_gap == 3.0F);

    gap.maximum_gap = 3.0F;
    checkpoint = update_air_slide_generated_checkpoint(gap, true, true, true);
    assert(checkpoint.emit && !checkpoint.ordinary_result_valid);
    assert(!checkpoint.reset_maximum_gap && gap.maximum_gap == 3.0F);

    assert(air_slide_active_path_phase(0) ==
           AirSlidePathPhase::best_current_gap);
    assert(air_slide_active_path_phase(1) ==
           AirSlidePathPhase::other_current_gap);
    assert(air_slide_path_complete(0, 2, 2));
    assert(!air_slide_path_complete(1, 2, 2));
    assert(air_slide_is_terminal(AirSlideStartPhase::complete,
                                 AirSlidePathPhase::complete));
    assert(!air_slide_exposes_candidate());

    assert(air_slide_start_source_category == 9);
    assert(air_slide_generated_source_category == 11);
    assert(map_shared_result_category(air_slide_start_source_category) == 3);
    assert(map_shared_result_category(air_slide_generated_source_category) ==
           5);
    assert(map_shared_result_category(12) == 7);
    assert(map_shared_result_category(13) == 7);
}
