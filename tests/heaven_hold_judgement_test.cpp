#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <limits>
#include <span>
#include <vector>

int main() {
    using namespace chart::reconstruction;

    assert(heaven_hold_parsed_type(HeavenHoldCommand::ald_zero_non) == 9);
    assert(heaven_hold_parsed_type(HeavenHoldCommand::hhd) == 13);
    assert(heaven_hold_parsed_type(
               HeavenHoldCommand::slide_hld_extended) == 13);
    assert(!heaven_hold_generates_path_records(
        HeavenHoldCommand::ald_zero_non));
    assert(heaven_hold_generates_path_records(HeavenHoldCommand::hhd));
    assert(heaven_hold_generates_path_records(HeavenHoldCommand::hhx));
    assert(heaven_hold_generates_path_records(
        HeavenHoldCommand::slide_hld_standard));

    assert(parse_slide_style_code("SLD") == slide_sld_style_code);
    assert(parse_slide_style_code("HLD") == slide_hld_style_code);
    assert(parse_slide_style_code("GRN") == slide_grn_style_code);
    assert(parse_slide_style_code("hld") == slide_sld_style_code);
    assert(parse_slide_style_code("") == slide_sld_style_code);
    assert(slide_style_retypes_to_heaven_hold(slide_hld_style_code));
    assert(!slide_style_retypes_to_heaven_hold(slide_sld_style_code));
    assert(slide_heaven_discriminator_code == 10);
    assert(!slide_command_uses_extended_profile(SlideCommandForm::sld));
    assert(slide_command_uses_extended_profile(SlideCommandForm::sxd));
    assert(!slide_command_uses_extended_profile(SlideCommandForm::slc));
    assert(slide_command_uses_extended_profile(SlideCommandForm::sxc));

    assert(heaven_hold_start_profile(HeavenHoldCommand::ald_zero_non, false) ==
           0);
    assert(heaven_hold_start_profile(HeavenHoldCommand::hhd, true) == 1);
    assert(heaven_hold_start_profile(HeavenHoldCommand::hhx, false) == 2);
    assert(heaven_hold_start_profile(HeavenHoldCommand::hhx, true) == 3);
    assert(heaven_hold_start_profile(
               HeavenHoldCommand::slide_hld_standard, false) == 0);
    assert(heaven_hold_start_profile(
               HeavenHoldCommand::slide_hld_standard, true) == 1);
    assert(heaven_hold_start_profile(
               HeavenHoldCommand::slide_hld_extended, false) == 2);
    assert(heaven_hold_start_profile(
               HeavenHoldCommand::slide_hld_extended, true) == 3);

    const HeavenHoldPathPoint root{0.0F, 0.0F, 0.0F};
    const HeavenHoldPathPoint end{2.0F, 0.0F, 20.0F};
    const HeavenHoldPathPoint decreasing_end{-1.0F, 0.0F, -10.0F};
    assert(evaluate_heaven_hold_path_generation(
               HeavenHoldCommand::ald_zero_non, root, decreasing_end) ==
           HeavenHoldPathGenerationDisposition::no_generated_path);
    assert(evaluate_heaven_hold_path_generation(
               HeavenHoldCommand::hhd, root, end) ==
           HeavenHoldPathGenerationDisposition::generated);
    assert(evaluate_heaven_hold_path_generation(
               HeavenHoldCommand::hhd, root, decreasing_end) ==
           HeavenHoldPathGenerationDisposition::
               source_large_unsigned_span_expansion);
    const HeavenHoldPathPoint near_tick_ceiling{
        5592405.0F, 0.0F, 0.0F};
    const HeavenHoldPathPoint indefinite_tick{
        std::numeric_limits<float>::infinity(), 0.0F, 1.0F};
    assert(evaluate_heaven_hold_path_generation(
               HeavenHoldCommand::hhd,
               near_tick_ceiling,
               indefinite_tick) ==
           HeavenHoldPathGenerationDisposition::generated);
    const HeavenHoldPathPoint near_tick_floor{
        -5592405.0F, 0.0F, 0.0F};
    assert(evaluate_heaven_hold_path_generation(
               HeavenHoldCommand::hhd,
               near_tick_floor,
               near_tick_ceiling) ==
           HeavenHoldPathGenerationDisposition::
               source_large_unsigned_span_expansion);

    const auto constant_tempo = [](float) { return 4.0F; };
    const auto point_at_tick = [](std::int32_t tick) {
        return HeavenHoldPathPoint{
            static_cast<float>(tick) / 384.0F, 0.0F, 0.0F};
    };
    const auto schedule_at_point = [](const HeavenHoldPathPoint& point) {
        return point.major * 10.0F;
    };

    auto ald_records = generate_heaven_hold_path_records(
        HeavenHoldCommand::ald_zero_non, root, end, 1.0F, false,
        std::span<const AirHoldExclusionInterval>{}, constant_tempo,
        point_at_tick, schedule_at_point);
    assert(ald_records.empty());

    auto records = generate_heaven_hold_path_records(
        HeavenHoldCommand::hhd, root, end, 1.0F, false,
        std::span<const AirHoldExclusionInterval>{}, constant_tempo,
        point_at_tick, schedule_at_point);
    assert(records.size() == 2);
    assert(air_hold_grid_tick(records[0].point) == 384);
    assert(records[0].kind == HeavenHoldGeneratedRecordKind::sample);
    assert(records[0].emission_enabled);
    assert(records[1].kind == HeavenHoldGeneratedRecordKind::path_end);
    assert(records[1].emission_enabled);

    const HeavenHoldPathPoint indefinite{
        std::numeric_limits<float>::infinity(), 0.0F, 0.0F};
    const auto indefinite_records = generate_heaven_hold_path_records(
        HeavenHoldCommand::hhd, indefinite, indefinite, 1.0F, false,
        std::span<const AirHoldExclusionInterval>{}, constant_tempo,
        point_at_tick, schedule_at_point);
    assert(air_hold_grid_tick(indefinite) ==
           std::numeric_limits<std::int32_t>::min());
    assert(indefinite_records.size() == 1);
    assert(indefinite_records.front().kind ==
           HeavenHoldGeneratedRecordKind::path_end);
    assert(indefinite_records.front().emission_enabled);

    const std::array<AirHoldExclusionInterval, 1> intervals{{
        {5.0F, 15.0F, 0},
    }};
    records = generate_heaven_hold_path_records(
        HeavenHoldCommand::hhx, root, end, 1.0F, true,
        std::span<const AirHoldExclusionInterval>{intervals}, constant_tempo,
        point_at_tick, schedule_at_point);
    assert(!records[0].emission_enabled);
    assert(records[1].emission_enabled);

    HeavenHoldLaneContactState contact{};
    auto contact_decision = update_heaven_hold_lane_contact(
        contact, false, {false, false}, {false, false}, false);
    assert(contact.admitted[0] && contact.admitted[1]);
    assert(!contact_decision.any_active);

    contact_decision = update_heaven_hold_lane_contact(
        contact, true, {true, false}, {false, false}, false);
    assert(contact_decision.admission_checked[0]);
    assert(contact_decision.active[0] && !contact_decision.active[1]);
    assert(contact_decision.any_active);

    HeavenHoldLaneContactState short_circuit{};
    contact_decision = update_heaven_hold_lane_contact(
        short_circuit, true, {true, true}, {true, true}, false);
    assert(contact_decision.admission_checked[0]);
    assert(!contact_decision.admission_checked[1]);
    assert(contact_decision.active[0] && contact_decision.active[1]);

    HeavenHoldLaneContactState forced{};
    contact_decision = update_heaven_hold_lane_contact(
        forced, true, {false, false}, {false, false}, true);
    assert(!contact_decision.active[0] && !contact_decision.active[1]);
    assert(contact_decision.any_active);

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
    auto checkpoint = update_heaven_hold_generated_checkpoint(
        gap, true, HeavenHoldGeneratedRecordKind::sample, false, false);
    assert(checkpoint.consumed && !checkpoint.emit);
    assert(checkpoint.ordinary_result_valid);
    assert(checkpoint.ordinary_result_byte == 1);
    assert(checkpoint.reset_maximum_gap);
    assert(checkpoint.source_category == 2);

    gap.maximum_gap = 3.0F;
    checkpoint = update_heaven_hold_generated_checkpoint(
        gap, true, HeavenHoldGeneratedRecordKind::path_end, true, true);
    assert(checkpoint.emit && !checkpoint.ordinary_result_valid);
    assert(!checkpoint.reset_maximum_gap && gap.maximum_gap == 3.0F);
    assert(checkpoint.source_category == 3);

    assert(heaven_hold_path_phase(0, 1) ==
           HeavenHoldPathPhase::best_current_gap);
    assert(heaven_hold_path_phase(1, 1) ==
           HeavenHoldPathPhase::other_current_gap);
    assert(heaven_hold_path_phase(0, 0) == HeavenHoldPathPhase::complete);
    assert(heaven_hold_exposes_candidate(
        HeavenHoldStartPhase::awaiting_result));
    assert(!heaven_hold_exposes_candidate(HeavenHoldStartPhase::complete));
    assert(heaven_hold_is_terminal(HeavenHoldStartPhase::complete,
                                   HeavenHoldPathPhase::complete));

    assert(heaven_hold_start_source_category(false, false) == 0);
    assert(heaven_hold_start_source_category(true, false) == 1);
    assert(map_shared_result_category(2) == 1);
    assert(map_shared_result_category(3) == 1);
    assert(!heaven_hold_end_feedback_submits_result());

    assert(select_ald_runtime(0, ald_missing_style_code) ==
           AldRuntimeKind::air_ladder);
}
