#include "chart/reconstruction.hpp"

#include <cassert>
#include <cstdint>
#include <limits>

int main() {
    using chart::reconstruction::ActiveGameplayExitContext;
    using chart::reconstruction::OuterGameplayUpdateMode;
    using chart::reconstruction::PostActiveDrainGateContext;
    using chart::reconstruction::active_gameplay_next_state_default;
    using chart::reconstruction::active_gameplay_next_state_variant;
    using chart::reconstruction::decide_active_gameplay_exit;
    using chart::reconstruction::final_scene_report_state;
    using chart::reconstruction::first_pre_active_priming_state;
    using chart::reconstruction::last_pre_active_priming_state;
    using chart::reconstruction::outer_gameplay_update_mode_for_scene_state;
    using chart::reconstruction::post_active_drain_ready_for_teardown;
    using chart::reconstruction::post_active_drain_state;
    using chart::reconstruction::post_active_outer_update_required;
    using chart::reconstruction::result_count_completion_enabled_for_setup;
    using chart::reconstruction::result_count_completion_reached;
    using chart::reconstruction::runtime_note_teardown_state;
    using chart::reconstruction::structural_active_gameplay_state;

    assert(!result_count_completion_enabled_for_setup(0));
    assert(result_count_completion_enabled_for_setup(1));
    assert(!result_count_completion_reached(false, 0, 0));
    assert(!result_count_completion_reached(true, 4, 3));
    assert(result_count_completion_reached(true, 4, 4));
    assert(result_count_completion_reached(true, 4, 5));
    assert(result_count_completion_reached(
        true, std::numeric_limits<std::uint32_t>::max(),
        std::numeric_limits<std::uint32_t>::max()));

    ActiveGameplayExitContext ordinary{};
    auto decision = decide_active_gameplay_exit(ordinary);
    assert(!decision.requested);
    assert(decision.next_state == 0);
    assert(!decision.transition_timestamp_requested);
    assert(!decision.final_report_requested);

    ordinary.selected_terminal = true;
    decision = decide_active_gameplay_exit(ordinary);
    assert(decision.requested);
    assert(decision.next_state == active_gameplay_next_state_default);
    assert(decision.transition_timestamp_requested);
    assert(decision.final_report_requested);

    ordinary = {};
    ordinary.result_count_complete = true;
    ordinary.scene_variant = true;
    decision = decide_active_gameplay_exit(ordinary);
    assert(decision.requested);
    assert(decision.next_state == active_gameplay_next_state_variant);
    assert(decision.transition_timestamp_requested);
    assert(!decision.final_report_requested);

    // The alternate controller ignores the ordinary selected-terminal input.
    ActiveGameplayExitContext alternate{};
    alternate.alternate_controller = true;
    alternate.selected_terminal = true;
    decision = decide_active_gameplay_exit(alternate);
    assert(!decision.requested);

    alternate.result_count_complete = true;
    decision = decide_active_gameplay_exit(alternate);
    assert(!decision.requested);
    alternate.alternate_ready = true;
    decision = decide_active_gameplay_exit(alternate);
    assert(decision.requested);
    assert(decision.next_state == active_gameplay_next_state_default);

    alternate = {};
    alternate.alternate_controller = true;
    alternate.alternate_state_three = true;
    alternate.scene_variant = true;
    decision = decide_active_gameplay_exit(alternate);
    assert(decision.requested);
    assert(decision.next_state == active_gameplay_next_state_variant);
    assert(!decision.final_report_requested);

    assert(post_active_outer_update_required(
        active_gameplay_next_state_variant));
    assert(post_active_outer_update_required(
        active_gameplay_next_state_default));
    assert(post_active_outer_update_required(post_active_drain_state));
    assert(!post_active_outer_update_required(runtime_note_teardown_state));
    assert(!post_active_outer_update_required(final_scene_report_state));

    assert(outer_gameplay_update_mode_for_scene_state(
               first_pre_active_priming_state - 1) ==
           OuterGameplayUpdateMode::none);
    for (std::uint32_t state = first_pre_active_priming_state;
         state <= last_pre_active_priming_state; ++state) {
        assert(outer_gameplay_update_mode_for_scene_state(state) ==
               OuterGameplayUpdateMode::zero_base_priming);
    }
    assert(outer_gameplay_update_mode_for_scene_state(
               structural_active_gameplay_state) ==
           OuterGameplayUpdateMode::live);
    assert(outer_gameplay_update_mode_for_scene_state(
               active_gameplay_next_state_variant) ==
           OuterGameplayUpdateMode::live);
    assert(outer_gameplay_update_mode_for_scene_state(
               active_gameplay_next_state_default) ==
           OuterGameplayUpdateMode::live);
    assert(outer_gameplay_update_mode_for_scene_state(
               post_active_drain_state) ==
           OuterGameplayUpdateMode::live);
    assert(outer_gameplay_update_mode_for_scene_state(
               runtime_note_teardown_state) ==
           OuterGameplayUpdateMode::none);
    assert(outer_gameplay_update_mode_for_scene_state(
               final_scene_report_state) ==
           OuterGameplayUpdateMode::none);

    PostActiveDrainGateContext drain{};
    assert(!post_active_drain_ready_for_teardown(drain));
    drain.ordinary_release_signal = true;
    assert(post_active_drain_ready_for_teardown(drain));
    drain.ordinary_wait_active = true;
    assert(!post_active_drain_ready_for_teardown(drain));
    drain.local_bypass = true;
    assert(post_active_drain_ready_for_teardown(drain));

    drain = {};
    drain.alternate_controller = true;
    drain.local_bypass = true;
    drain.ordinary_release_signal = true;
    assert(!post_active_drain_ready_for_teardown(drain));
    drain.alternate_ready = true;
    assert(post_active_drain_ready_for_teardown(drain));
    drain.alternate_ready = false;
    drain.alternate_state_three = true;
    assert(post_active_drain_ready_for_teardown(drain));
}
