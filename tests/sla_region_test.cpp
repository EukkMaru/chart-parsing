#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <string_view>

int main() {
    using chart::reconstruction::C2sSlaRegion;
    using chart::reconstruction::ChartPosition;
    using chart::reconstruction::parse_c2s_sla_record;
    using chart::reconstruction::select_c2s_sla_tag;
    using chart::reconstruction::select_c2s_sla_tag_for_float_span;

    const std::string_view complete[]{"1", "0", "3", "4", "96", "7"};
    const C2sSlaRegion parsed = parse_c2s_sla_record(complete);
    assert(parsed.start.major == 1.0F && parsed.start.minor == 0.0F);
    assert(parsed.end.major == 1.0F && parsed.end.minor == 1.0F);
    assert(parsed.lane == 3);
    assert(parsed.width == 4);
    assert(parsed.tag == 7);

    const C2sSlaRegion mirrored = parse_c2s_sla_record(complete, true);
    assert(mirrored.lane == 9);

    const std::string_view narrow[]{"0", "0", "2", "0", "1", "3",
                                    "ignored"};
    assert(parse_c2s_sla_record(narrow).width == 1);
    const std::string_view wide[]{"0", "0", "2", "99", "1", "3"};
    assert(parse_c2s_sla_record(wide).width == 16);

    const std::span<const std::string_view> missing;
    const C2sSlaRegion defaulted = parse_c2s_sla_record(missing);
    assert(defaulted.start.major == 0.0F && defaulted.start.minor == 0.0F);
    assert(defaulted.end.major == 0.0F && defaulted.end.minor == 0.0F);
    assert(defaulted.lane == 0 && defaulted.width == 1 && defaulted.tag == 0);

    bool malformed_threw = false;
    try {
        const std::string_view malformed[]{"bad"};
        (void)parse_c2s_sla_record(malformed);
    } catch (const std::invalid_argument&) {
        malformed_threw = true;
    }
    assert(malformed_threw);

    const std::array<C2sSlaRegion, 4> regions{{
        {{1.0F, 0.0F}, {1.0F, 1.0F}, 3, 4, 7},
        {{1.0F, 0.0F}, {1.0F, 1.0F}, 4, 2, 12},
        {{1.0F, 0.0F}, {1.0F, 1.0F}, 3, 4, 0},
        {{1.0F, 0.0F}, {1.0F, 1.0F}, 3, 4, -4},
    }};

    assert(select_c2s_sla_tag({1.0F, 0.0F}, 4, 2, regions) == 12);
    assert(select_c2s_sla_tag({1.0F, 0.5F}, 3, 4, regions) == 7);
    assert(select_c2s_sla_tag({1.0F, 0.5F}, 2, 4, regions) == 0);
    assert(select_c2s_sla_tag({1.0F, 0.5F}, 5, 3, regions) == 0);
    assert(select_c2s_sla_tag({1.0F, 1.0F}, 4, 2, regions) == 0);

    const float half_grid = 1.0F / 192.0F;
    assert(select_c2s_sla_tag({0.0F, 4.0F - half_grid}, 4, 2, regions) ==
           12);
    assert(select_c2s_sla_tag({1.0F, 1.0F - half_grid}, 4, 2, regions) ==
           0);

    assert(select_c2s_sla_tag_for_float_span({1.0F, 0.5F},
                                              3.0F - 0.000009F,
                                              4.0F + 0.000018F,
                                              regions) == 7);
    assert(select_c2s_sla_tag_for_float_span({1.0F, 0.5F},
                                              3.0F - 0.00002F,
                                              4.0F,
                                              regions) == 0);
}
