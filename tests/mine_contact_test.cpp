#include "chart/reconstruction.hpp"

#include <cassert>

int main() {
    using chart::reconstruction::MineContactState;
    using chart::reconstruction::MinePhase;
    using chart::reconstruction::SharedResultRoute;
    using chart::reconstruction::mine_exposes_candidate;
    using chart::reconstruction::mine_is_terminal;
    using chart::reconstruction::mine_phase_from_forced_result;
    using chart::reconstruction::mine_source_category;
    using chart::reconstruction::mine_terminal_result_byte;
    using chart::reconstruction::map_dispatch_aggregate_category;
    using chart::reconstruction::map_dispatch_progress_category;
    using chart::reconstruction::map_shared_result_category;
    using chart::reconstruction::route_shared_result;
    using chart::reconstruction::update_mine_contact;

    assert(!mine_exposes_candidate(MinePhase::accumulating));

    MineContactState state{};
    assert(update_mine_contact(state, -3.0F, -4.0F, true, -2.0F, 2.0F,
                               3.0F) == MinePhase::accumulating);
    assert(state.accumulated == 0.0F);

    // Held contact credits the entire clipped substep interval, not a count of
    // held lanes. Resolution is deferred while the current delta is negative.
    assert(update_mine_contact(state, -1.0F, -3.0F, true, -2.0F, 2.0F,
                               3.0F) == MinePhase::accumulating);
    assert(state.accumulated == 1.0F);
    assert(update_mine_contact(state, 0.5F, -1.0F, true, -2.0F, 2.0F,
                               3.0F) == MinePhase::accumulating);
    assert(state.accumulated == 2.5F);
    assert(update_mine_contact(state, 1.0F, 0.5F, false, -2.0F, 2.0F,
                               3.0F) == MinePhase::accumulating);
    assert(state.accumulated == 2.5F);
    assert(update_mine_contact(state, 1.5F, 1.0F, true, -2.0F, 2.0F,
                               3.0F) == MinePhase::threshold_reached);
    assert(state.accumulated == 3.0F);
    assert(mine_is_terminal(state.phase));
    assert(mine_terminal_result_byte(state.phase) == 0);

    // Terminal state is stable even if a later held interval is supplied.
    assert(update_mine_contact(state, 2.0F, 1.5F, true, -2.0F, 2.0F,
                               3.0F) == MinePhase::threshold_reached);
    assert(state.accumulated == 3.0F);

    MineContactState impossible{};
    assert(update_mine_contact(impossible, 0.0F, -1.0F, false, -2.0F, 1.0F,
                               2.0F) == MinePhase::threshold_unreachable);
    assert(mine_terminal_result_byte(impossible.phase) == 4);

    // Equality with the best possible total is not an early failure, and an
    // exact accumulated threshold is a success.
    MineContactState exact_possible{};
    assert(update_mine_contact(exact_possible, 0.0F, -1.0F, false, -2.0F,
                               1.0F, 1.0F) == MinePhase::accumulating);
    exact_possible.accumulated = 1.0F;
    assert(update_mine_contact(exact_possible, 0.0F, 0.0F, false, -2.0F,
                               1.0F, 1.0F) == MinePhase::threshold_reached);

    MineContactState clipped{};
    update_mine_contact(clipped, 3.0F, -4.0F, true, -2.0F, 2.0F, 100.0F);
    assert(clipped.accumulated == 4.0F);
    assert(clipped.phase == MinePhase::threshold_unreachable);

    assert(mine_phase_from_forced_result(0) ==
           MinePhase::threshold_reached);
    assert(mine_phase_from_forced_result(1) ==
           MinePhase::threshold_unreachable);
    assert(mine_phase_from_forced_result(4) ==
           MinePhase::threshold_unreachable);

    // MNE's fixed source category selects dispatch record 11. The record's
    // independent fields select progress category 0 and aggregate category 8.
    assert(mine_source_category == 17);
    assert(map_shared_result_category(mine_source_category) == 11);
    assert(map_dispatch_progress_category(11) == 0);
    assert(map_dispatch_aggregate_category(11) == 8);
    assert(route_shared_result(mine_source_category, 19, 0, 12, false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(route_shared_result(mine_source_category, 19, 4, 12, true) ==
           SharedResultRoute::observer_only);
}
