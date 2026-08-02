#include "chart/reconstruction.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>
#include <string_view>

int main() {
    using chart::reconstruction::C2sAsoSegment;
    using chart::reconstruction::c2s_aso_segments_connect;
    using chart::reconstruction::parse_c2s_aso_record;
    using chart::reconstruction::parse_c2s_color_style_code;
    using chart::reconstruction::quantize_c2s_aso_property;

    const std::string_view complete[]{
        "1", "0", "2", "3", "1.24", "-0.16",
        "96", "7", "4", "2.55", "0.04", "VLT",
    };
    const C2sAsoSegment parsed = parse_c2s_aso_record(complete);
    assert(parsed.start.position.major == 1.0F);
    assert(parsed.start.position.minor == 0.0F);
    assert(parsed.end.position.major == 1.0F);
    assert(parsed.end.position.minor == 1.0F);
    assert(parsed.start.lane == 2 && parsed.start.width == 3);
    assert(parsed.end.lane == 7 && parsed.end.width == 4);
    assert(parsed.start.property_a_tenths == 12);
    assert(parsed.start.property_b_tenths == -1);
    assert(parsed.end.property_a_tenths == 26);
    assert(parsed.end.property_b_tenths == 0);
    assert(parsed.color_style_code == 11);

    const C2sAsoSegment mirrored = parse_c2s_aso_record(complete, true);
    assert(mirrored.start.lane == 11);
    assert(mirrored.end.lane == 5);

    const std::string_view wrapped_fields[]{
        "0", "2147483647", "-2147483648", "16", "nan", "inf",
        "1", "-2147483648", "16", "-inf", "1e30", "DEF",
    };
    const C2sAsoSegment wrapped =
        parse_c2s_aso_record(wrapped_fields, true);
    assert(wrapped.start.lane == std::numeric_limits<std::int32_t>::min());
    assert(wrapped.end.lane == std::numeric_limits<std::int32_t>::min());
    const auto expected_wrapped_end =
        chart::reconstruction::canonicalize_c2s_position(
            0, std::numeric_limits<std::int32_t>::min());
    assert(wrapped.end.position.major == expected_wrapped_end.major);
    assert(wrapped.end.position.minor == expected_wrapped_end.minor);
    assert(wrapped.start.property_a_tenths ==
           std::numeric_limits<std::int32_t>::min());
    assert(wrapped.start.property_b_tenths ==
           std::numeric_limits<std::int32_t>::min());
    assert(wrapped.end.property_a_tenths ==
           std::numeric_limits<std::int32_t>::min());
    assert(wrapped.end.property_b_tenths ==
           std::numeric_limits<std::int32_t>::min());

    assert(quantize_c2s_aso_property(0.05F) == 1);
    assert(quantize_c2s_aso_property(-0.05F) == 0);
    assert(quantize_c2s_aso_property(
               std::numeric_limits<float>::quiet_NaN()) ==
           std::numeric_limits<std::int32_t>::min());
    assert(quantize_c2s_aso_property(
               std::numeric_limits<float>::infinity()) ==
           std::numeric_limits<std::int32_t>::min());
    assert(parse_c2s_color_style_code("DEF") == 0);
    assert(parse_c2s_color_style_code("NON") == 15);
    assert(parse_c2s_color_style_code("non") == 0);
    assert(parse_c2s_color_style_code("unknown") == 0);

    const std::string_view missing[]{"0", "0", "4", "0"};
    const C2sAsoSegment defaulted = parse_c2s_aso_record(missing);
    assert(defaulted.start.width == 1 && defaulted.end.width == 1);
    assert(defaulted.end.lane == 0);
    assert(defaulted.start.property_a_tenths == 0);
    assert(defaulted.end.property_b_tenths == 0);
    assert(defaulted.color_style_code == 0);

    const std::string_view prefixed[]{
        "0", "0", "1", "99", "1.2tail", "2", "96",
        "3", "-2", "3", "4", "RED", "ignored",
    };
    const C2sAsoSegment prefix_parsed = parse_c2s_aso_record(prefixed);
    assert(prefix_parsed.start.width == 16);
    assert(prefix_parsed.end.width == 1);
    assert(prefix_parsed.start.property_a_tenths == 12);
    assert(prefix_parsed.color_style_code == 1);

    bool malformed_threw = false;
    try {
        const std::string_view malformed[]{"0", "0", "0", "1", "bad"};
        (void)parse_c2s_aso_record(malformed);
    } catch (const std::invalid_argument&) {
        malformed_threw = true;
    }
    assert(malformed_threw);

    const std::string_view continuation[]{
        "1", "96", "7", "4", "2.55", "0.04",
        "96", "8", "2", "3", "4", "VLT",
    };
    const C2sAsoSegment next = parse_c2s_aso_record(continuation);
    assert(c2s_aso_segments_connect(parsed, next));

    C2sAsoSegment mismatch = next;
    mismatch.start.property_a_tenths += 1;
    assert(!c2s_aso_segments_connect(parsed, mismatch));
    mismatch = next;
    mismatch.color_style_code = 0;
    assert(!c2s_aso_segments_connect(parsed, mismatch));
    mismatch = next;
    mismatch.start.position.minor += 1.0F / 191.0F;
    assert(!c2s_aso_segments_connect(parsed, mismatch));
}
