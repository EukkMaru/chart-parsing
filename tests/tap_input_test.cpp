#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cstdint>

using chart::reconstruction::derive_tap_lane_input;

int main() {
    std::array<std::uint8_t, 32> sources{};

    auto snapshot = derive_tap_lane_input(sources, 0);
    assert(snapshot.raw_level == 0);
    assert(snapshot.logical_level == 0);
    assert(snapshot.logical_rising == 0);

    // Either physical/source bank folds to the same logical lane.
    sources[3] = 1;
    sources[16 + 9] = 7;  // Every nonzero byte is asserted.
    snapshot = derive_tap_lane_input(sources, 0);
    assert(snapshot.raw_level == ((1U << 3U) | (1U << 25U)));
    assert(snapshot.logical_level == ((1U << 3U) | (1U << 9U)));
    assert(snapshot.logical_rising == snapshot.logical_level);

    // A held source remains a level but no longer produces a rising edge.
    snapshot = derive_tap_lane_input(sources, snapshot.raw_level);
    assert(snapshot.logical_level == ((1U << 3U) | (1U << 9U)));
    assert(snapshot.logical_rising == 0);

    // Edges are detected before bank folding. A new source in bank 1 produces
    // a logical edge even when the same logical lane is held in bank 0.
    sources.fill(0);
    sources[5] = 1;
    const std::uint32_t prior_bank_zero = 1U << 5U;
    sources[16 + 5] = 1;
    snapshot = derive_tap_lane_input(sources, prior_bank_zero);
    assert(snapshot.logical_level == (1U << 5U));
    assert(snapshot.logical_rising == (1U << 5U));

    // Releasing both sources clears level and never synthesizes an edge.
    sources.fill(0);
    snapshot = derive_tap_lane_input(
        sources, (1U << 5U) | (1U << (16U + 5U)));
    assert(snapshot.logical_level == 0);
    assert(snapshot.logical_rising == 0);

    // Both ends of the 16-lane domain retain their positions after folding.
    sources[16] = 1;
    sources[31] = 1;
    snapshot = derive_tap_lane_input(sources, 0);
    assert(snapshot.logical_rising == 0x8001U);
}
