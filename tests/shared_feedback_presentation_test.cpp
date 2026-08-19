#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>

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

    FeedbackEffectPlayerState effect_player;
    assert(effect_player.current_phase == -1);
    assert(effect_player.pending_phase == -1);
    assert(effect_player.visible);
    assert(!effect_player.stopped);
    start_feedback_effect_player_state(effect_player);
    assert(effect_player.pending_phase == 1);

    const auto starting_update =
        update_feedback_effect_player_state(effect_player, true);
    assert(starting_update.applied_pending_transition);
    assert(starting_update.queued_live);
    assert(!starting_update.queued_terminal);
    assert(!starting_update.terminal_after_update);
    assert(effect_player.current_phase == 1);
    assert(effect_player.pending_phase == 2);
    assert(effect_player.ticks_in_phase == 0);

    const auto live_update =
        update_feedback_effect_player_state(effect_player, true);
    assert(live_update.applied_pending_transition);
    assert(!live_update.queued_live);
    assert(!live_update.queued_terminal);
    assert(effect_player.current_phase == 2);
    assert(effect_player.pending_phase == -1);

    // External disappearance only queues state 3. EffList's terminal check
    // still observes current state 2 after this update.
    const auto disappearance_update =
        update_feedback_effect_player_state(effect_player, false);
    assert(disappearance_update.queued_terminal);
    assert(!disappearance_update.terminal_after_update);
    assert(effect_player.current_phase == 2);
    assert(effect_player.pending_phase == 3);
    const auto terminal_update =
        update_feedback_effect_player_state(effect_player, false);
    assert(terminal_update.applied_pending_transition);
    assert(terminal_update.terminal_after_update);
    assert(feedback_effect_player_is_terminal(effect_player));

    FeedbackEffectPlayerState stopped_effect;
    start_feedback_effect_player_state(stopped_effect);
    assert(stop_feedback_effect_player_state(stopped_effect));
    assert(!stop_feedback_effect_player_state(stopped_effect));
    assert(stopped_effect.stopped);
    assert(!feedback_effect_player_is_terminal(stopped_effect));
    const auto stopped_update =
        update_feedback_effect_player_state(stopped_effect, true);
    assert(stopped_update.terminal_after_update);

    FeedbackEffectPlayerState paused_effect;
    start_feedback_effect_player_state(paused_effect);
    const auto paused_update =
        update_feedback_effect_player_state(paused_effect, true, true);
    assert(!paused_update.applied_pending_transition);
    assert(paused_effect.current_phase == -1);
    assert(paused_effect.pending_phase == 1);
    assert(paused_effect.ticks_in_phase == 0);

    assert(apply_feedback_effect_visibility_flag(0xffffffffU, true) ==
           0xfffffffbU);
    assert(apply_feedback_effect_visibility_flag(0U, false) == 0x4U);
    const auto effect_position =
        feedback_effect_translation_matrix(1.0F, 2.0F, 3.0F);
    assert(effect_position[0] == 1.0F && effect_position[5] == 1.0F &&
           effect_position[10] == 1.0F && effect_position[15] == 1.0F);
    assert(effect_position[12] == 1.0F && effect_position[13] == 2.0F &&
           effect_position[14] == 3.0F);

    SlideRetainedFeedbackControlState retained_slide_effect;
    const auto absent_slide_effect = update_slide_retained_feedback_control(
        retained_slide_effect, false, 2);
    assert(!absent_slide_effect.controls_handle);
    assert(retained_slide_effect.pending_resource_entry_latch == -1);

    const auto hidden_slide_effect = update_slide_retained_feedback_control(
        retained_slide_effect, true, 1);
    assert(hidden_slide_effect.controls_handle);
    assert(!hidden_slide_effect.visible);
    assert(!hidden_slide_effect.updates_position);
    assert(!hidden_slide_effect.selected_resource_entry.has_value());
    assert(retained_slide_effect.pending_resource_entry_latch == -1);

    const auto first_live_slide_effect =
        update_slide_retained_feedback_control(
            retained_slide_effect, true, 2);
    assert(first_live_slide_effect.visible);
    assert(first_live_slide_effect.updates_position);
    assert(first_live_slide_effect.selected_resource_entry == 0);
    assert(retained_slide_effect.pending_resource_entry_latch == 0);
    const auto later_live_slide_effect =
        update_slide_retained_feedback_control(
            retained_slide_effect, true, 2);
    assert(!later_live_slide_effect.selected_resource_entry.has_value());

    const auto preload_step_10 = slide_preload_feedback_control(10);
    const auto preload_step_11 = slide_preload_feedback_control(11);
    const auto preload_step_12 = slide_preload_feedback_control(12);
    const auto preload_step_13 = slide_preload_feedback_control(13);
    const auto preload_step_14 = slide_preload_feedback_control(14);
    assert(!preload_step_10.controls_each_handle);
    assert(preload_step_11.controls_each_handle && preload_step_11.visible &&
           preload_step_11.selected_resource_entry == 0);
    assert(preload_step_12.controls_each_handle && preload_step_12.visible &&
           preload_step_12.selected_resource_entry == 1);
    assert(preload_step_13.controls_each_handle && !preload_step_13.visible &&
           preload_step_13.selected_resource_entry == 1);
    assert(!preload_step_14.controls_each_handle);

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

    FeedbackEffectListState list_state{
        .active_count = 4,
        .submissions_since_update = 0,
        .maximum_occupancy = 2,
        .accepted_total = 10,
        .cooldown_ready = true,
    };
    // Admission uses the per-update submission counter, not active occupancy.
    assert(feedback_effect_list_accepts_normal_submission(
        feedback_effect_lists[7],
        list_state.submissions_since_update,
        list_state.cooldown_ready));
    const auto normal_append = append_feedback_effect_list_state(
        list_state, feedback_effect_lists[7], false);
    assert(normal_append.evicted_oldest);
    assert(normal_append.restarted_cooldown);
    assert(list_state.active_count == 4);
    assert(list_state.submissions_since_update == 1);
    assert(list_state.accepted_total == 11);
    assert(!list_state.cooldown_ready);

    list_state.active_count = 0;
    list_state.submissions_since_update = 4;
    list_state.cooldown_ready = true;
    assert(!feedback_effect_list_accepts_normal_submission(
        feedback_effect_lists[7],
        list_state.submissions_since_update,
        list_state.cooldown_ready));

    list_state.active_count = 4;
    const auto forced_append = append_feedback_effect_list_state(
        list_state, feedback_effect_lists[7], true);
    assert(!forced_append.evicted_oldest);
    assert(!forced_append.restarted_cooldown);
    assert(list_state.active_count == 5);

    update_feedback_effect_list_state(list_state, 3);
    assert(list_state.active_count == 3);
    assert(list_state.maximum_occupancy == 3);
    assert(list_state.submissions_since_update == 0);
    list_state.submissions_since_update = 2;
    clear_feedback_effect_list_state(list_state);
    assert(list_state.active_count == 0);
    assert(list_state.maximum_occupancy == 0);
    assert(list_state.accepted_total == 0);
    assert(list_state.cooldown_ready);
    // Clear does not zero the per-update counter; update owns that reset.
    assert(list_state.submissions_since_update == 2);
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

    assert(slide_chara_effect_lane_footprints ==
           (std::array<std::int32_t, 16>{
               2, 4, 6, 8, 2, 12, 2, 16,
               2, 2, 2, 2, 2, 2, 2, 32}));
    for (const float duration : slide_chara_effect_lane_durations) {
        assert(duration == 1.0F);
    }
    assert(slide_chara_effect_global_duration == 7.0F);

    SlideCharaEffectAdmissionState chara_admission;
    assert(check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 2, 1, 0.0F));
    assert(chara_admission.lane_expiry[4] == 1.0F);
    assert(chara_admission.lane_expiry[5] == 1.0F);
    assert(!check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 2, 1, 0.5F));
    assert(check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 2, 1, 1.0F));

    // The binary scans [first,last) but writes [first,last]. A reservation in
    // only the candidate's last cell therefore does not block admission.
    reset_slide_chara_effect_admission(chara_admission);
    chara_admission.lane_expiry[5] = 10.0F;
    assert(check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 2, 1, 0.0F));

    reset_slide_chara_effect_admission(chara_admission);
    assert(check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 0, 16, 3.0F));
    for (const float expiry : chara_admission.lane_expiry) {
        assert(expiry == 4.0F);
    }
    assert(!check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, -1, 1, 4.0F));
    assert(!check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 1, 16, 4.0F));
    assert(!check_and_reserve_slide_chara_effect_lane_overlap(
        chara_admission, 0, 0, 4.0F));

    reset_slide_chara_effect_admission(chara_admission);
    assert(check_and_reserve_slide_chara_effect_global_cooldown(
        chara_admission, 0.0F));
    assert(chara_admission.global_next_allowed == 7.0F);
    assert(!check_and_reserve_slide_chara_effect_global_cooldown(
        chara_admission, 6.999F));
    assert(check_and_reserve_slide_chara_effect_global_cooldown(
        chara_admission, 7.0F));
    assert(chara_admission.global_next_allowed == 14.0F);

    const float nan = std::numeric_limits<float>::quiet_NaN();
    reset_slide_chara_effect_admission(chara_admission);
    assert(check_and_reserve_slide_chara_effect_global_cooldown(
        chara_admission, nan));
    assert(check_and_reserve_slide_chara_effect_global_cooldown(
        chara_admission, nan));
    reset_slide_chara_effect_admission(chara_admission);
    for (const float expiry : chara_admission.lane_expiry) {
        assert(expiry == 0.0F);
    }
    assert(chara_admission.global_next_allowed == 0.0F);
}
