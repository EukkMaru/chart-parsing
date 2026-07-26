#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>

int main() {
    using chart::reconstruction::AirCommand;
    using chart::reconstruction::AirDirectionCode;
    using chart::reconstruction::AirPhase;
    using chart::reconstruction::AirProfileSnapshot;
    using chart::reconstruction::AirTimingState;
    using chart::reconstruction::SharedResultRoute;
    using chart::reconstruction::TapInterval;
    using chart::reconstruction::TapLaneWindows;
    using chart::reconstruction::air_direction_code;
    using chart::reconstruction::air_exposes_candidate;
    using chart::reconstruction::air_input_profile;
    using chart::reconstruction::air_is_terminal;
    using chart::reconstruction::air_source_category;
    using chart::reconstruction::route_shared_result;
    using chart::reconstruction::sample_air_profile;
    using chart::reconstruction::update_air_timing;

    assert(air_direction_code(AirCommand::air, false) ==
           AirDirectionCode::air);
    assert(air_direction_code(AirCommand::air, true) ==
           AirDirectionCode::air);
    assert(air_direction_code(AirCommand::aur, false) ==
           AirDirectionCode::aur);
    assert(air_direction_code(AirCommand::aur, true) ==
           AirDirectionCode::aul);
    assert(air_direction_code(AirCommand::aul, true) ==
           AirDirectionCode::aur);
    assert(air_direction_code(AirCommand::adw, true) ==
           AirDirectionCode::adw);
    assert(air_direction_code(AirCommand::adr, true) ==
           AirDirectionCode::adl);
    assert(air_direction_code(AirCommand::adl, true) ==
           AirDirectionCode::adr);

    assert(air_input_profile(1, AirDirectionCode::air) == 0);
    assert(air_input_profile(1, AirDirectionCode::adw) == 1);
    assert(air_input_profile(0, AirDirectionCode::aur) == 2);
    assert(air_input_profile(11, AirDirectionCode::adr) == 3);
    assert(air_input_profile(6, AirDirectionCode::aul) == 4);
    assert(air_input_profile(6, AirDirectionCode::adl) == 5);
    assert(air_input_profile(9, AirDirectionCode::air) == -1);

    AirProfileSnapshot snapshot{};
    snapshot.profile_levels[2] = 7;
    assert(sample_air_profile(snapshot, 2));
    assert(snapshot.sample_marker);
    assert(snapshot.profile_levels[2] == 7);

    const TapLaneWindows windows{
        true,
        std::array<TapInterval, 5>{{
            {-5.0F, 5.0F},
            {-4.0F, 4.0F},
            {-3.0F, 3.0F},
            {-2.0F, 2.0F},
            {-1.0F, 1.0F},
        }},
        0.0F,
    };

    AirTimingState retained{};
    // The outer lower endpoint is strict at the AIR gate.
    auto decision = update_air_timing(retained, windows, -5.0F, true);
    assert(decision.fine_index == 0);
    assert(!decision.completes);
    assert(retained.retained_fine_index == 0);

    // Pre-pivot input improves retained timing but still returns the internal
    // nonaccepting waiting index.
    decision = update_air_timing(retained, windows, -2.5F, true);
    assert(decision.fine_index == 1);
    assert(!decision.completes);
    assert(retained.retained_fine_index == 4);

    // At the pivot, a better current location with no input does not replace
    // or emit the retained non-center result.
    decision = update_air_timing(retained, windows, 0.0F, false);
    assert(decision.fine_index == 1);
    assert(!decision.completes);

    // Once current timing becomes worse than the retained sample, the retained
    // early result is emitted without requiring a new profile level.
    decision = update_air_timing(retained, windows, 2.5F, false);
    assert(decision.fine_index == 4);
    assert(decision.coarse_tier == 2);
    assert(decision.side_code == 1);
    assert(decision.completes);

    // A center sample retained before the pivot completes at the pivot even
    // when the current profile byte is clear.
    AirTimingState center{};
    decision = update_air_timing(center, windows, -0.5F, true);
    assert(decision.fine_index == 1);
    assert(center.retained_fine_index == 6);
    decision = update_air_timing(center, windows, 0.0F, false);
    assert(decision.fine_index == 6);
    assert(decision.coarse_tier == 4);
    assert(decision.side_code == 0);
    assert(decision.completes);

    // Without a retained sample, the outer upper endpoint resolves to the
    // fixed late fine result.
    AirTimingState timeout{};
    decision = update_air_timing(timeout, windows, 5.0F, false);
    assert(decision.fine_index == 11);
    assert(decision.coarse_tier == 0);
    assert(decision.side_code == 2);
    assert(decision.completes);

    assert(!air_exposes_candidate(AirPhase::awaiting_result));
    assert(!air_exposes_candidate(AirPhase::resolved));
    assert(!air_is_terminal(AirPhase::awaiting_result));
    assert(air_is_terminal(AirPhase::resolved));

    assert(air_source_category(AirDirectionCode::air) == 7);
    assert(air_source_category(AirDirectionCode::aul) == 7);
    assert(air_source_category(AirDirectionCode::adw) == 8);
    assert(air_source_category(AirDirectionCode::adl) == 8);
    assert(route_shared_result(air_source_category(AirDirectionCode::air), 19,
                               4, 12, false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(route_shared_result(air_source_category(AirDirectionCode::adw), 19,
                               4, 12, true) ==
           SharedResultRoute::observer_only);
}
