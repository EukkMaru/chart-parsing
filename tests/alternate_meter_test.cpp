#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>

int main() {
    using chart::reconstruction::AlternateTerminalMeter;
    using chart::reconstruction::AlternateMeterParticipantSeed;
    using chart::reconstruction::alternate_auxiliary_restricts_to_local;
    using chart::reconstruction::alternate_terminal_meter_active;
    using chart::reconstruction::alternate_terminal_meter_selected;
    using chart::reconstruction::apply_alternate_meter_delta;
    using chart::reconstruction::apply_selector_2_empty_participant_setup_fallback;
    using chart::reconstruction::count_alternate_meter_participants;
    using chart::reconstruction::initialize_alternate_meter_auxiliary;
    using chart::reconstruction::select_alternate_meter_delta;
    using chart::reconstruction::selected_shared_result_terminal_route;

    assert(!alternate_terminal_meter_selected(0, 0, 0));
    assert(alternate_terminal_meter_selected(1, 0, 0));
    assert(alternate_terminal_meter_selected(0, 1, 1));
    assert(!alternate_terminal_meter_selected(2, 0, 0));

    constexpr std::array<std::int32_t, 4> deltas{1, 3, -2, 7};
    assert(select_alternate_meter_delta(0, deltas) == 1);
    assert(select_alternate_meter_delta(1, deltas) == 3);
    assert(select_alternate_meter_delta(2, deltas) == -2);
    assert(select_alternate_meter_delta(3, deltas) == 7);
    assert(select_alternate_meter_delta(4, deltas) == 7);
    assert(select_alternate_meter_delta(5, deltas) == 0);

    AlternateTerminalMeter meter{10, 10, false, false, 0};
    assert(!alternate_terminal_meter_active(meter));
    apply_alternate_meter_delta(meter, 3);
    assert(meter.current == 7);
    apply_alternate_meter_delta(meter, -20);
    assert(meter.current == 10);
    apply_alternate_meter_delta(meter, -1);
    assert(meter.current == 10);
    apply_alternate_meter_delta(meter, 12);
    assert(meter.current == 0);
    assert(meter.zero_latched);
    assert(alternate_terminal_meter_active(meter));

    AlternateTerminalMeter disabled{0, 0, false, false, 0};
    assert(!alternate_terminal_meter_active(disabled));

    AlternateTerminalMeter auxiliary{5, 10, true, false, 1};
    assert(!alternate_terminal_meter_active(auxiliary));
    apply_alternate_meter_delta(auxiliary, 10);
    assert(auxiliary.current == 0);
    assert(auxiliary.zero_latched);
    auxiliary.auxiliary_count = 0;
    assert(alternate_terminal_meter_active(auxiliary));
    apply_alternate_meter_delta(auxiliary, -3);
    assert(auxiliary.current == 0);

    assert(alternate_auxiliary_restricts_to_local(1));
    assert(alternate_auxiliary_restricts_to_local(2));
    assert(!alternate_auxiliary_restricts_to_local(3));
    assert(alternate_auxiliary_restricts_to_local(4));
    assert(!alternate_auxiliary_restricts_to_local(5));

    constexpr std::array<AlternateMeterParticipantSeed, 4> participants{{
        {true, false, 0},
        {false, true, 7},
        {false, true, 0},
        {false, false, 9},
    }};
    assert(count_alternate_meter_participants(participants, 4, 1, false) == 1);
    assert(count_alternate_meter_participants(participants, 4, 5, false) == 2);
    assert(count_alternate_meter_participants(participants, 4, 5, true) == 1);
    assert(count_alternate_meter_participants(participants, 1, 5, false) == 1);
    assert(count_alternate_meter_participants(participants, 99, 5, false) == 2);

    AlternateTerminalMeter captured{};
    assert(initialize_alternate_meter_auxiliary(captured, 2, 4, true));
    assert(captured.auxiliary_count == 2);
    assert(captured.auxiliary_participant_limit == 4);
    assert(captured.auxiliary_locked);
    assert(!initialize_alternate_meter_auxiliary(captured, 0, 1, false));
    assert(captured.auxiliary_count == 2);
    assert(captured.auxiliary_participant_limit == 4);

    AlternateTerminalMeter refreshable{};
    assert(initialize_alternate_meter_auxiliary(refreshable, 3, 4, false));
    assert(initialize_alternate_meter_auxiliary(refreshable, 1, 2, false));
    assert(refreshable.auxiliary_count == 1);
    assert(refreshable.auxiliary_participant_limit == 2);

    // Selector 2 is stable after setup, so an already-set ordinary byte is not
    // the selected terminal predicate while the alternate meter is nonterminal.
    AlternateTerminalMeter selected_meter{5, 10, false, false, 0};
    assert(!selected_shared_result_terminal_route(
        0, 0, 1, true, selected_meter));
    assert(selected_shared_result_terminal_route(
        0, 0, 0, true, selected_meter));

    // The one-shot zero-participant fallback runs after a successful capture,
    // forces the ordinary-form meter to zero, and records the ordinary latch.
    bool ordinary_terminal = false;
    AlternateTerminalMeter fallback_meter{5, 10, false, false, 0};
    const bool capture_applied = initialize_alternate_meter_auxiliary(
        fallback_meter, 0, 4, true);
    assert(apply_selector_2_empty_participant_setup_fallback(
        0, 0, 1, capture_applied, 0, ordinary_terminal, fallback_meter));
    assert(ordinary_terminal);
    assert(fallback_meter.current == 0);
    assert(fallback_meter.zero_latched);
    assert(selected_shared_result_terminal_route(
        0, 0, 1, ordinary_terminal, fallback_meter));

    // A later locked periodic capture cannot reapply the fallback.
    const bool recaptured = initialize_alternate_meter_auxiliary(
        fallback_meter, 0, 4, false);
    assert(!recaptured);
    assert(!apply_selector_2_empty_participant_setup_fallback(
        0, 0, 1, recaptured, 0, ordinary_terminal, fallback_meter));

    // An already-terminal selected meter and other selector modes also skip it.
    ordinary_terminal = false;
    AlternateTerminalMeter already_terminal{0, 10, false, true, 0};
    assert(!apply_selector_2_empty_participant_setup_fallback(
        0, 0, 1, true, 0, ordinary_terminal, already_terminal));
    assert(!ordinary_terminal);
    AlternateTerminalMeter selector_zero{5, 10, false, false, 0};
    assert(!apply_selector_2_empty_participant_setup_fallback(
        1, 0, 0, true, 0, ordinary_terminal, selector_zero));
}
