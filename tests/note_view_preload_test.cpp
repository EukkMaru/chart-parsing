#include "chart/reconstruction.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>

int main() {
    using chart::reconstruction::NoteViewPreloadAction;
    using chart::reconstruction::advance_note_view_preload;
    using chart::reconstruction::note_view_first_ready_steps;
    using chart::reconstruction::note_view_manager_step_floor;
    using chart::reconstruction::note_view_preload_order;
    using chart::reconstruction::reset_note_view_preload;
    using chart::reconstruction::start_note_view_preload;

    auto state = start_note_view_preload();
    assert(state.stage == 1);
    assert(state.owned_object_count == 0);

    const auto first = advance_note_view_preload(state);
    assert(first.action == NoteViewPreloadAction::construct_first);
    assert(first.has_constructed_class);
    assert(first.constructed_class == note_view_preload_order[0]);
    assert(state.stage == 2);
    assert(state.owned_object_count == 1);

    std::uint32_t total_periodic_calls = 1;
    for (std::size_t latest = 0; latest < note_view_preload_order.size();
         ++latest) {
        const std::uint32_t expected_calls =
            std::max(note_view_manager_step_floor,
                     note_view_first_ready_steps[latest]) +
            1U;
        std::uint32_t calls = 0;
        while (true) {
            const auto step = advance_note_view_preload(state);
            ++calls;
            ++total_periodic_calls;
            if (step.action == NoteViewPreloadAction::none) {
                assert(!step.completed);
                continue;
            }

            assert(calls == expected_calls);
            assert(step.finalized_object_count == latest + 1U);
            if (latest + 1U < note_view_preload_order.size()) {
                assert(step.action ==
                       NoteViewPreloadAction::finalize_and_construct_next);
                assert(step.has_constructed_class);
                assert(step.constructed_class ==
                       note_view_preload_order[latest + 1U]);
                assert(!step.completed);
            } else {
                assert(step.action ==
                       NoteViewPreloadAction::finalize_and_complete);
                assert(!step.has_constructed_class);
                assert(step.completed);
            }
            break;
        }
    }

    assert(total_periodic_calls == 164);
    assert(state.stage == 13);
    assert(state.owned_object_count == note_view_preload_order.size());
    assert(advance_note_view_preload(state).completed);

    reset_note_view_preload(state);
    assert(state.stage == 0);
    assert(state.resource_step == 0);
    assert(state.owned_object_count == 0);
    assert(advance_note_view_preload(state).completed);
}
