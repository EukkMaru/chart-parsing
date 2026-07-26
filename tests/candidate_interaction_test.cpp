#include "chart/reconstruction.hpp"

#include <cassert>

int main() {
    using chart::reconstruction::LaneCandidateGate;
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
}
