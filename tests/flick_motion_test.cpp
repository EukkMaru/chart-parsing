#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>

int main() {
    using chart::reconstruction::FlickMotionOutcome;
    using chart::reconstruction::FlickMotionTracker;
    using chart::reconstruction::FlickPhase;
    using chart::reconstruction::TapInterval;
    using chart::reconstruction::TapLaneWindows;
    using chart::reconstruction::apply_flick_center_window_adjustment;
    using chart::reconstruction::advance_flick_phase;
    using chart::reconstruction::compose_flick_direction_result;
    using chart::reconstruction::flick_exposes_start_candidate;
    using chart::reconstruction::flick_is_terminal;
    using chart::reconstruction::flick_terminal_metadata;
    using chart::reconstruction::select_forced_flick_direction;
    using chart::reconstruction::update_flick_motion;

    std::array<bool, 16> bank_zero{};
    std::array<bool, 16> bank_one{};
    FlickMotionTracker tracker;

    TapLaneWindows windows{
        true,
        std::array<TapInterval, 5>{{
            {-5.0F, 5.0F},
            {-4.0F, 4.0F},
            {-2.0F, 2.0F},
            {-2.01F, 2.01F},
            {-0.5F, 0.5F},
        }},
        0.0F,
    };
    apply_flick_center_window_adjustment(windows, 0.02F);
    assert(windows.bands_outer_to_center[4].lower == -2.01F);
    assert(windows.bands_outer_to_center[4].upper == 2.01F);

    // First activity initializes all groups it belongs to without producing
    // motion. A later held-source centroid shift can complete positively.
    bank_zero[4] = true;
    assert(update_flick_motion(tracker, bank_zero, bank_one, 4, 2, 6, 2.0F) ==
           FlickMotionOutcome::tracking);
    bank_zero[4] = false;
    bank_zero[7] = true;
    assert(update_flick_motion(tracker, bank_zero, bank_one, 4, 2, 6, 2.0F) ==
           FlickMotionOutcome::positive);

    // The union group can detect motion split across physical source banks.
    FlickMotionTracker union_tracker;
    bank_zero = {};
    bank_one = {};
    bank_zero[6] = true;
    assert(update_flick_motion(union_tracker, bank_zero, bank_one, 4, 4, 6,
                               2.0F) == FlickMotionOutcome::tracking);
    bank_zero = {};
    bank_one[3] = true;
    assert(update_flick_motion(union_tracker, bank_zero, bank_one, 4, 4, 6,
                               2.0F) == FlickMotionOutcome::negative);

    // Leaving a chart-edge lane contributes the binary's virtual half-lane
    // sentinel for one sample.
    FlickMotionTracker edge_tracker;
    bank_zero = {};
    bank_one = {};
    bank_zero[0] = true;
    assert(update_flick_motion(edge_tracker, bank_zero, bank_one, 0, 2, 4,
                               0.75F) == FlickMotionOutcome::tracking);
    bank_zero = {};
    assert(update_flick_motion(edge_tracker, bank_zero, bank_one, 0, 2, 4,
                               0.75F) == FlickMotionOutcome::negative);
    assert(edge_tracker.groups[0].current == -0.5F);

    // The start and motion stages can both transition on one update.
    assert(advance_flick_phase(FlickPhase::awaiting_edge, 0.0F, -2.0F, 3.0F,
                               4.0F, true,
                               FlickMotionOutcome::positive) ==
           FlickPhase::positive_motion);
    assert(advance_flick_phase(FlickPhase::awaiting_edge, 3.0F, -2.0F, 3.0F,
                               4.0F, false,
                               FlickMotionOutcome::tracking) ==
           FlickPhase::edge_timeout);
    assert(advance_flick_phase(FlickPhase::tracking_motion, 4.0F, -2.0F, 3.0F,
                               4.0F, false,
                               FlickMotionOutcome::positive) ==
           FlickPhase::motion_timeout);

    assert(flick_exposes_start_candidate(FlickPhase::awaiting_edge));
    assert(!flick_exposes_start_candidate(FlickPhase::tracking_motion));
    assert(flick_is_terminal(FlickPhase::negative_motion));
    assert(!flick_is_terminal(FlickPhase::tracking_motion));
    assert(flick_terminal_metadata(FlickPhase::edge_timeout) == 16);
    assert(flick_terminal_metadata(FlickPhase::negative_motion) == 14);
    assert(flick_terminal_metadata(FlickPhase::positive_motion) == 15);

    const auto low_combined = compose_flick_direction_result(3, 2);
    assert(low_combined.provisional_result == 1);
    assert(low_combined.side_code == 2);
    assert(low_combined.control_code == 2);

    const auto high_combined = compose_flick_direction_result(6, 6);
    assert(high_combined.provisional_result == 4);
    assert(high_combined.side_code == 0);
    assert(high_combined.control_code == 4);

    bool shared_forced_direction = true;
    assert(select_forced_flick_direction(shared_forced_direction) ==
           FlickPhase::negative_motion);
    assert(!shared_forced_direction);
    assert(select_forced_flick_direction(shared_forced_direction) ==
           FlickPhase::positive_motion);
    assert(shared_forced_direction);
}
