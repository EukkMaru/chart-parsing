#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cstdint>

int main() {
    using namespace chart::reconstruction;

    const std::array<ActiveResultControlUnit, 1> result_controls{{
        {.source_record_present = true, .threshold = 2},
    }};
    const LoadedActiveResultControls loaded_result_controls{
        .skill_profile_id = 7,
        .units = result_controls,
    };
    constexpr std::array<RuntimeFeedbackFamily, 12> feedback_families{
        RuntimeFeedbackFamily::slide,
        RuntimeFeedbackFamily::air_ladder,
        RuntimeFeedbackFamily::heaven_hold,
        RuntimeFeedbackFamily::air_solid,
        RuntimeFeedbackFamily::tap,
        RuntimeFeedbackFamily::air,
        RuntimeFeedbackFamily::mine,
        RuntimeFeedbackFamily::flick,
        RuntimeFeedbackFamily::air_hold,
        RuntimeFeedbackFamily::air_slide,
        RuntimeFeedbackFamily::hold,
        RuntimeFeedbackFamily::chara_tap,
    };
    for (const auto family : feedback_families) {
        const bool special = family == RuntimeFeedbackFamily::mine ||
                             family == RuntimeFeedbackFamily::flick;
        assert(feedback_wrapper_applies_active_result_control(family) ==
               special);
        assert(apply_feedback_wrapper_result_control(
                   family, 2, loaded_result_controls, 5) ==
               (special ? 0 : 2));
    }
    // Applying the Mine/Flick wrapper after a producer-side remap is
    // idempotent, and an unloaded controller preserves the input.
    assert(apply_feedback_wrapper_result_control(
               RuntimeFeedbackFamily::mine, 0, loaded_result_controls, 5) ==
           0);
    assert(apply_feedback_wrapper_result_control(
               RuntimeFeedbackFamily::flick, 2,
               {.skill_profile_id = -1, .units = result_controls}, 5) == 2);

    const FeedbackVariantSelectors selectors{
        {3, 4},
        {1, 4},
        {2, 4},
    };
    const auto result_4 =
        adjust_feedback_result_variant(4, 2, selectors);
    assert(result_4.valid_external_configuration);
    assert(result_4.result == 5);
    assert(result_4.variant == 0);
    assert(result_4.selected_mask_index == 3);

    const auto result_3 =
        adjust_feedback_result_variant(3, 2, selectors);
    assert(result_3.result == 5);
    assert(result_3.variant == 0);

    const auto result_2 =
        adjust_feedback_result_variant(2, 2, selectors);
    assert(result_2.result == 2);
    assert(result_2.variant == 2);

    const auto result_1 =
        adjust_feedback_result_variant(1, 2, selectors);
    assert(result_1.result == 5);
    assert(result_1.variant == 2);

    FeedbackVariantSelectors malformed = selectors;
    malformed.result_1.loaded_count = 0;
    assert(!adjust_feedback_result_variant(1, 1, malformed)
                .valid_external_configuration);
    malformed.result_1 = {7, 8};
    assert(!adjust_feedback_result_variant(1, 1, malformed)
                .valid_external_configuration);

    assert(map_feedback_resource_mode(0, 99, false) == 7);
    assert(map_feedback_resource_mode(1, 0, false) == 4);
    assert(map_feedback_resource_mode(1, 1, false) == 5);
    assert(map_feedback_resource_mode(1, 2, false) == 6);
    assert(map_feedback_resource_mode(2, 0, false) == 1);
    assert(map_feedback_resource_mode(2, 1, false) == 2);
    assert(map_feedback_resource_mode(2, 2, false) == 3);
    assert(map_feedback_resource_mode(3, 0, false) == 0);
    assert(map_feedback_resource_mode(3, 1, false) == 8);
    assert(map_feedback_resource_mode(3, 2, false) == 9);
    assert(map_feedback_resource_mode(4, 2, false) == 0);
    assert(map_feedback_resource_mode(4, 2, true) == 13);
    assert(map_feedback_resource_mode(5, 0, false) == 12);
    assert(map_feedback_resource_mode(5, 1, false) == 10);
    assert(map_feedback_resource_mode(5, 2, false) == 11);
    assert(map_feedback_resource_mode(6, 0, false) == -1);

    assert(feedback_kind_2_enabled(false, false, 0));
    assert(!feedback_kind_2_enabled(true, false, 3));
    assert(feedback_kind_2_enabled(false, true, 2));
    assert(feedback_kind_2_enabled(false, true, 6));
    assert(!feedback_kind_2_enabled(false, true, 1));
    assert(!feedback_kind_2_enabled(false, true, 7));
    assert(feedback_kind_1_enabled(false, 1));
    assert(!feedback_kind_1_enabled(false, 2));
    assert(!feedback_kind_1_enabled(false, 6));
    assert(feedback_kind_1_enabled(false, 7));

    assert(select_feedback_span_resource(0) ==
           FeedbackSpanResourceSelection{});
    assert(select_feedback_span_resource(1).row_byte_offset == 0x34);
    assert(select_feedback_span_resource(4).row_byte_offset == 0x28);
    assert(select_feedback_span_resource(5).bucket_width == 6);
    assert(select_feedback_span_resource(8).row_byte_offset == 0x1c);
    assert(select_feedback_span_resource(16).row_byte_offset == 0x0c);
    assert(!select_feedback_span_resource(17).has_resource_field);
    assert(feedback_span_lateral_scale(
               7, 5, select_feedback_span_resource(5)) == 5.0F / 6.0F);
    assert(feedback_span_lateral_scale(
               6, 5, select_feedback_span_resource(5)) == 1.0F);

    assert(ordinary_feedback_scene(0, 15) == FeedbackScene::background);
    assert(ordinary_feedback_scene(1, 15) == FeedbackScene::background);
    assert(ordinary_feedback_scene(2, 15) == FeedbackScene::main);
    assert(ordinary_feedback_scene(0, 14) == FeedbackScene::main);

    assert(select_lane_feedback_request(0, 3).updates_lanes);
    assert(select_lane_feedback_request(15, 0).updates_lanes);
    assert(!select_lane_feedback_request(17, 1).updates_lanes);
    assert(select_lane_feedback_request(17, 0).zero_result_override);
    assert(!select_lane_feedback_request(2, 3).updates_lanes);

    LaneFeedbackState lanes;
    assert(apply_lane_feedback(lanes, 2, 3, 4, false));
    const std::uint32_t first_serial = lanes.next_serial;
    assert(lanes.result[2] == 4 && lanes.result[4] == 4);
    assert(lanes.serial[2] == first_serial &&
           lanes.serial[4] == first_serial);

    // Overwriting one lane clears every lane in its old serial group first.
    assert(apply_lane_feedback(lanes, 3, 1, 2, false));
    assert(lanes.result[2] == 0);
    assert(lanes.result[3] == 2);
    assert(lanes.result[4] == 0);
    assert(lanes.serial[2] == 0 && lanes.serial[4] == 0);

    assert(!apply_lane_feedback(lanes, 0, 1, 0, false));
    assert(apply_lane_feedback(lanes, -2, 4, 0, true));
    assert(lanes.result[0] == 0 && lanes.result[1] == 0);
    assert(lanes.serial[0] == lanes.next_serial);
    assert(!apply_lane_feedback(lanes, 16, 1, 1, false));

    assert(fixed_post_result_cue_enabled(1, 0, false));
    assert(!fixed_post_result_cue_enabled(1, 1, false));
    assert(fixed_post_result_cue_enabled(3, 2, false));
    assert(!fixed_post_result_cue_enabled(3, 2, true));
    assert(!fixed_post_result_cue_enabled(4, 0, false));

    assert(feedback_effect_lists[0].list == FeedbackEffectList::bomb);
    assert(feedback_effect_lists[0].capacity == 24);
    assert(feedback_effect_lists[7].list ==
           FeedbackEffectList::chara_background);
    assert(feedback_effect_lists[7].capacity == 4);
    assert(feedback_effect_lists[7].cooldown_frame_count == 5);
    assert(feedback_effect_lists[8].capacity == 48);
    assert(feedback_effect_lists[9].cooldown_frame_count == 5);
    assert(ordinary_feedback_submit_order ==
           (std::array<FeedbackEffectList, 3>{
               FeedbackEffectList::text,
               FeedbackEffectList::bomb,
               FeedbackEffectList::reaction,
           }));
    assert(slide_extended_feedback_submit_order ==
           (std::array<FeedbackEffectList, 2>{
               FeedbackEffectList::chara_note,
               FeedbackEffectList::chara_background,
           }));
    assert(feedback_effect_list_accepts_normal_submission(
        feedback_effect_lists[0], 23, true));
    assert(!feedback_effect_list_accepts_normal_submission(
        feedback_effect_lists[0], 24, true));
    assert(!feedback_effect_list_accepts_normal_submission(
        feedback_effect_lists[0], 0, false));
    assert(feedback_effect_cooldown_milliseconds(5, 60.0) == 83);
    assert(feedback_effect_cooldown_milliseconds(5, 120.0) == 42);
    assert(feedback_effect_cooldown_milliseconds(5, 0.0) == 0);

    std::uint32_t sequence = 0xffffffffU;
    assert(allocate_feedback_sequence(sequence) == 0xffffffffU);
    assert(sequence == 0U);
    assert(allocate_feedback_sequence(sequence) == 0U);
    assert(sequence == 1U);

    const std::array<std::int32_t, 8> runtime_rows{
        8, 9, 10, 11, 12, 13, 14, 15};
    assert(select_slide_extended_feedback_row(3, true, runtime_rows) == 4);
    assert(select_slide_extended_feedback_row(3, false, runtime_rows) == 11);
    assert(select_slide_extended_feedback_row(8, true, runtime_rows) == -1);
}
