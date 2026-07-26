#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <span>

int main() {
    using namespace chart::reconstruction;

    assert(select_ald_runtime(0, ald_non_style_code) ==
           AldRuntimeKind::heaven_hold);
    assert(select_ald_runtime(1, ald_non_style_code) ==
           AldRuntimeKind::air_ladder);
    assert(select_ald_runtime(0, 0) == AldRuntimeKind::air_ladder);

    assert(air_ladder_input_profile == 7);
    assert(!air_ladder_exposes_candidate());
    assert(air_ladder_source_category == 18);
    assert(map_shared_result_category(air_ladder_source_category) == 7);

    const std::array<std::int32_t, 4> before{0, 1, 2, 0};
    const std::array<std::int32_t, 4> after{2, 3, 3, 1};
    assert(air_ladder_new_resolution_count(
               std::span<const std::int32_t>{before},
               std::span<const std::int32_t>{after}) == 2);

    assert(!air_ladder_is_terminal(9.9F, 10.0F, 2, 2));
    assert(!air_ladder_is_terminal(10.0F, 10.0F, 1, 2));
    assert(air_ladder_is_terminal(10.0F, 10.0F, 2, 2));
    assert(air_ladder_is_terminal(11.0F, 10.0F, 2, 2));
}
