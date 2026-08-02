#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <vector>

int main() {
    using chart::reconstruction::LaneCandidateGate;
    using chart::reconstruction::LaneCandidateParticipant;
    using chart::reconstruction::RuntimeAppendEvent;
    using chart::reconstruction::RuntimeAppendRole;
    using chart::reconstruction::SharedResultRoute;
    using chart::reconstruction::SharedResultRoutingState;
    using chart::reconstruction::append_runtime_factory_events;
    using chart::reconstruction::dispatch_shared_result;
    using chart::reconstruction::evaluate_lane_candidate_fanout;
    using chart::reconstruction::lane_candidate_gate_allows_start;
    using chart::reconstruction::reduce_lane_candidate;

    // An earlier FLK becomes the lane minimum. Its own edge path remains
    // eligible, while a later TAP/HOLD/Slide-style start is candidate-gated.
    std::int32_t selected = reduce_lane_candidate(-1, 100);
    selected = reduce_lane_candidate(selected, 120);
    assert(selected == 100);
    assert(lane_candidate_gate_allows_start(
        LaneCandidateGate::independent, selected, 100));
    assert(!lane_candidate_gate_allows_start(
        LaneCandidateGate::selected_equality, selected, 120));

    // An earlier gated note remains selected, but a later FLK still ignores
    // that inequality. Neither candidate is consumed by either note update.
    selected = reduce_lane_candidate(-1, 100);
    selected = reduce_lane_candidate(selected, 120);
    assert(lane_candidate_gate_allows_start(
        LaneCandidateGate::selected_equality, selected, 100));
    assert(lane_candidate_gate_allows_start(
        LaneCandidateGate::independent, selected, 120));

    // Equal candidates admit both policies; reduction is not a winner choice.
    selected = reduce_lane_candidate(-1, 100);
    selected = reduce_lane_candidate(selected, 100);
    assert(lane_candidate_gate_allows_start(
        LaneCandidateGate::selected_equality, selected, 100));
    assert(lane_candidate_gate_allows_start(
        LaneCandidateGate::independent, selected, 100));

    // The equality-gated families reject an absent manager candidate. FLK's
    // local timing/input path does not consult that sentinel.
    assert(!lane_candidate_gate_allows_start(
        LaneCandidateGate::selected_equality, -1, -1));
    assert(lane_candidate_gate_allows_start(
        LaneCandidateGate::independent, -1, -1));

    // TAP, CHR, unresolved HOLD, unresolved Slide, and unresolved HeavenHold
    // all use the same equality gate. Equal candidates share one unconsumed
    // edge across those families; FLK independently accepts the same edge.
    const std::array<LaneCandidateParticipant, 6> equal_cross_family{{
        {100, LaneCandidateGate::selected_equality, true},  // TAP
        {100, LaneCandidateGate::selected_equality, true},  // CHR
        {100, LaneCandidateGate::selected_equality, true},  // HOLD
        {100, LaneCandidateGate::selected_equality, true},  // Slide
        {100, LaneCandidateGate::selected_equality, true},  // HeavenHold
        {100, LaneCandidateGate::independent, true},        // FLK
    }};
    const auto equal_decision =
        evaluate_lane_candidate_fanout(equal_cross_family);
    assert(equal_decision.selected == 100);
    for (const bool accepted : equal_decision.accepts) {
        assert(accepted);
    }

    // Local timing/input rejection remains per note and does not consume the
    // shared edge before later equal participants.
    auto one_local_rejection = equal_cross_family;
    one_local_rejection[1].local_input_accepts = false;
    const auto local_decision =
        evaluate_lane_candidate_fanout(one_local_rejection);
    assert(!local_decision.accepts[1]);
    assert(local_decision.accepts[2]);

    // Runtime storage order is dynamic construction order. Existing objects
    // remain first; a root is immediately followed by its attached secondary;
    // a factory-default parsed record contributes no object.
    std::vector<RuntimeAppendEvent> active_order{
        {2, RuntimeAppendRole::primary},
    };
    append_runtime_factory_events(active_order, 5, true, true);
    append_runtime_factory_events(active_order, 6, false, true);
    append_runtime_factory_events(active_order, 7, true, false);
    assert(active_order.size() == 4);
    assert(active_order[0].parsed_index == 2);
    assert(active_order[1].parsed_index == 5 &&
           active_order[1].role == RuntimeAppendRole::primary);
    assert(active_order[2].parsed_index == 5 &&
           active_order[2].role == RuntimeAppendRole::attached_secondary);
    assert(active_order[3].parsed_index == 7);

    // The later full-vector update preserves that order for simultaneous
    // results. Once the first event activates terminal routing, the next
    // same-pass event still runs but becomes observer-only.
    SharedResultRoutingState route_state{};
    assert(dispatch_shared_result(
               route_state, 0, 19, 4, 12, true) ==
           SharedResultRoute::authoritative_aggregate);
    assert(dispatch_shared_result(
               route_state, 0, 19, 4, 12, false) ==
           SharedResultRoute::observer_only);
}
