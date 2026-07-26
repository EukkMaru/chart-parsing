#include "chart/reconstruction.hpp"

#include <cassert>

using chart::reconstruction::NoteLifecycleState;
using chart::reconstruction::commit_requested_transition;
using chart::reconstruction::is_terminal;
using chart::reconstruction::reduce_lane_candidate;
using chart::reconstruction::request_terminal_transition;
using chart::reconstruction::tap_has_terminal_detailed_result;

int main() {
    // The manager's reduction copies the first exposed value, then retains the
    // smallest nonnegative value. Equal candidates remain equal and therefore
    // do not create a single-winner tie break.
    assert(reduce_lane_candidate(-1, -1) == -1);
    assert(reduce_lane_candidate(-1, 200) == 200);
    assert(reduce_lane_candidate(200, 240) == 200);
    assert(reduce_lane_candidate(200, 200) == 200);
    assert(reduce_lane_candidate(200, 180) == 180);
    assert(reduce_lane_candidate(200, -1) == 200);

    NoteLifecycleState tap{1, -1};
    request_terminal_transition(tap);

    // A result requests state 2 but does not alter current state immediately,
    // so the same manager pass cannot remove the TAP.
    assert(tap.current == 1);
    assert(tap.requested == 2);
    assert(!is_terminal(tap));

    // On the following substep candidate reduction precedes the note tick.
    // If the TAP is still timing-eligible, its precomputed candidate therefore
    // participates once more before the transition is committed.
    assert(reduce_lane_candidate(-1, 100) == 100);
    assert(commit_requested_transition(tap));
    assert(tap.current == 2);
    assert(tap.requested == -1);
    assert(is_terminal(tap));

    // Terminal requests are conditional on the active state, and an absent
    // request is a no-op.
    NoteLifecycleState inactive{0, -1};
    request_terminal_transition(inactive);
    assert(inactive.requested == -1);
    assert(!commit_requested_transition(inactive));

    // The embedded checker starts unresolved. Detailed result values above one
    // suppress another judgement callback independently of the deferred base
    // state transition; the source comparison is strictly signed greater-than.
    assert(!tap_has_terminal_detailed_result(0));
    assert(!tap_has_terminal_detailed_result(1));
    assert(tap_has_terminal_detailed_result(2));
    assert(tap_has_terminal_detailed_result(11));
}
