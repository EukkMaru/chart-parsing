#include "chart/reconstruction.hpp"

#include <cassert>
#include <cstdint>
#include <limits>

int main() {
    using chart::reconstruction::bounded_note_lane_extent;
    using chart::reconstruction::decode_c2s_note_width;
    using chart::reconstruction::encode_c2s_note_width;
    using chart::reconstruction::parse_c2s_common_lane_geometry;

    assert(encode_c2s_note_width(-1) == 0);
    assert(encode_c2s_note_width(0) == 0);
    assert(encode_c2s_note_width(1) == 0);
    assert(encode_c2s_note_width(2) == 1);
    assert(encode_c2s_note_width(16) == 15);
    assert(encode_c2s_note_width(17) == 15);

    assert(decode_c2s_note_width(-1) == 0);
    assert(decode_c2s_note_width(0) == 1);
    assert(decode_c2s_note_width(15) == 16);
    assert(decode_c2s_note_width(16) == 0);

    const auto defaulted = parse_c2s_common_lane_geometry(3, 0);
    assert(defaulted.lane == 3);
    assert(defaulted.width == 1);
    assert(defaulted.encoded_width == 0);

    const auto wide = parse_c2s_common_lane_geometry(-2, 99);
    assert(wide.lane == -2);
    assert(wide.width == 16);
    assert(wide.encoded_width == 15);

    const auto ordinary = bounded_note_lane_extent(3, 4);
    assert(ordinary.start == 3 && ordinary.count == 4);

    const auto clipped_left = bounded_note_lane_extent(-2, 5);
    assert(clipped_left.start == 0 && clipped_left.count == 3);

    const auto clipped_right = bounded_note_lane_extent(14, 4);
    assert(clipped_right.start == 14 && clipped_right.count == 2);

    const auto outside = bounded_note_lane_extent(20, 4);
    assert(outside.start == 20 && outside.count == 0);

    const auto invalid_width = bounded_note_lane_extent(4, 0);
    assert(invalid_width.start == 4 && invalid_width.count == 0);

    const auto wrapped = bounded_note_lane_extent(
        std::numeric_limits<std::int32_t>::max(), 16);
    assert(wrapped.start == std::numeric_limits<std::int32_t>::max());
    assert(wrapped.count == 16);
}
