#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cmath>

namespace {

bool near(float left, float right, float tolerance = 0.00001F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using chart::reconstruction::HoldGapState;
    using chart::reconstruction::HoldLaneSourceState;
    using chart::reconstruction::classify_hold_gap;
    using chart::reconstruction::hold_gap_result_byte;
    using chart::reconstruction::reset_hold_gap_after_checkpoint;
    using chart::reconstruction::update_hold_gap;
    using chart::reconstruction::update_hold_lane_sources;

    HoldLaneSourceState sources;
    std::array<bool, 2> marker{};

    // A source held before any observed release is not eligible by itself.
    assert(!update_hold_lane_sources(sources, {true, false}, {false, false},
                                     true, marker));
    assert(!sources.active[0]);

    // Observing the source released arms it for a later press.
    assert(!update_hold_lane_sources(sources, {false, false}, {false, false},
                                     false, marker));
    assert(sources.armed[0] && sources.armed[1]);
    assert(update_hold_lane_sources(sources, {true, false}, {false, false},
                                    true, marker));
    assert(marker[0] && !marker[1]);

    // A preceding sustain marker on either bank arms every currently held
    // source for this logical lane.
    HoldLaneSourceState handoff;
    marker = {};
    assert(update_hold_lane_sources(handoff, {true, true}, {false, true}, true,
                                    marker));
    assert(handoff.armed[0] && handoff.armed[1]);
    assert(marker[0] && marker[1]);

    HoldGapState gap{
        .update_gate = 1.0F,
        .participation_floor = 1.0F,
        .end = 20.0F,
        .thresholds = {0.5F, 1.0F, 2.0F, 4.0F},
    };

    // Updates before the gate do not mutate accumulated state.
    assert(update_hold_gap(gap, 0.5F, true) == -1);
    assert(!gap.ever_active && gap.current == 0.0F);

    assert(update_hold_gap(gap, 1.0F, true) == 0);
    assert(gap.ever_active && gap.last_active == 1.0F);
    assert(update_hold_gap(gap, 1.5F, false) == 0);
    assert(gap.maximum_gap == 0.5F);
    assert(update_hold_gap(gap, 2.1F, false) == 2);
    assert(near(gap.maximum_gap, 1.1F));

    // Renewed activity updates last-active time but does not erase the retained
    // worst gap used at checkpoints.
    assert(update_hold_gap(gap, 3.0F, true) == 0);
    assert(gap.last_active == 3.0F);
    assert(classify_hold_gap(gap) == 2);
    reset_hold_gap_after_checkpoint(gap);
    assert(gap.maximum_gap == 0.0F);

    // Elapsed time clamps at the configured end.
    assert(update_hold_gap(gap, 100.0F, false) == 4);
    assert(gap.current == 20.0F);
    assert(gap.maximum_gap == 17.0F);

    HoldGapState never_active{
        .update_gate = 0.0F,
        .participation_floor = 0.0F,
        .end = 10.0F,
        .thresholds = {0.5F, 1.0F, 2.0F, 4.0F},
    };
    assert(update_hold_gap(never_active, 0.0F, false) == 4);
    assert(classify_hold_gap(never_active) == 4);

    assert(hold_gap_result_byte(0) == 4);
    assert(hold_gap_result_byte(1) == 3);
    assert(hold_gap_result_byte(2) == 2);
    assert(hold_gap_result_byte(3) == 1);
    assert(hold_gap_result_byte(-1) == 0);
    assert(hold_gap_result_byte(4) == 0);
}
