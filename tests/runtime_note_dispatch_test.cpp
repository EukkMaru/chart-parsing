#include <chart/reconstruction.hpp>

#include <cassert>

int main() {
    using chart::reconstruction::RuntimeNoteScheduledCallback;
    using chart::reconstruction::dispatch_runtime_note_substep;
    using chart::reconstruction::make_runtime_note_lifecycle;
    using chart::reconstruction::runtime_note_has_state_action_child;
    using chart::reconstruction::runtime_note_tick_is_suppressed;

    static_assert(!runtime_note_has_state_action_child);
    static_assert(!runtime_note_tick_is_suppressed);

    auto note = make_runtime_note_lifecycle();
    assert(note.current == -1);
    assert(note.requested == 0);

    // The first manager substep after materialization commits state 0. Every
    // concrete factory class has a true state-0 predicate, which requests
    // state 1 but does not run the gameplay update in this substep.
    const auto activation = dispatch_runtime_note_substep(note);
    assert(activation.transition_committed);
    assert(activation.callback ==
           RuntimeNoteScheduledCallback::state_0_activation);
    assert(note.current == 0);
    assert(note.requested == 1);
    assert(!activation.removable_after_substep);

    // The second later manager substep commits state 1 and executes the
    // note-specific update. A terminal request remains pending.
    const auto gameplay = dispatch_runtime_note_substep(note, true);
    assert(gameplay.transition_committed);
    assert(gameplay.callback == RuntimeNoteScheduledCallback::state_1_update);
    assert(note.current == 1);
    assert(note.requested == 2);
    assert(!gameplay.removable_after_substep);

    // The next substep commits state 2, selects the no-op callback, and lets
    // the manager's same-pass removal phase erase the object.
    const auto terminal = dispatch_runtime_note_substep(note);
    assert(terminal.transition_committed);
    assert(terminal.callback == RuntimeNoteScheduledCallback::none);
    assert(note.current == 2);
    assert(note.requested == -1);
    assert(terminal.removable_after_substep);

    // An active update can remain in state 1 when its family-specific logic
    // does not request completion.
    note = {1, -1};
    const auto continuing = dispatch_runtime_note_substep(note, false);
    assert(!continuing.transition_committed);
    assert(continuing.callback == RuntimeNoteScheduledCallback::state_1_update);
    assert(note.current == 1);
    assert(note.requested == -1);
    assert(!continuing.removable_after_substep);
}
