#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

chart::reconstruction::AirSolidGeometryEndpoint endpoint(
    float raw,
    float projected,
    float lateral) {
    return {
        raw,
        projected,
        lateral,
        lateral + 10.0F,
        lateral + 20.0F,
        lateral + 2.0F,
        0.5F,
        lateral + 30.0F,
    };
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(near(air_solid_style_vertex_coordinate(0), 0.1561999917F));
    assert(near(air_solid_style_vertex_coordinate(15), 0.0F));
    assert(near(air_solid_style_vertex_coordinate(99),
                air_solid_style_vertex_coordinate(0)));
    assert(air_solid_style_has_external_resource(0));
    assert(!air_solid_style_has_external_resource(15));
    assert(air_solid_style_has_external_resource(99));

    assert(!air_solid_is_terminal(9.0F, 10.0F));
    assert(air_solid_is_terminal(10.0F, 10.0F));
    assert(air_solid_is_terminal(11.0F, 10.0F));
    const float nan = std::numeric_limits<float>::quiet_NaN();
    assert(!air_solid_is_terminal(nan, 10.0F));
    assert(!air_solid_is_terminal(10.0F, nan));

    AirSolidGeometrySegment crossing{};
    crossing.start = endpoint(-2.0F, 10.0F, 0.0F);
    crossing.end = endpoint(6.0F, 0.0F, 8.0F);
    assert(air_solid_crosses_raw_origin(crossing));
    assert(near(air_solid_raw_origin_fraction(crossing), 0.25F));
    const auto seam = air_solid_mix_endpoint(
        crossing.start, crossing.end,
        air_solid_raw_origin_fraction(crossing));
    assert(near(seam.raw_relative, 0.0F));
    assert(near(seam.projected, 7.5F));
    assert(near(seam.lateral, 2.0F));
    crossing.start.raw_relative = 2.0F;
    assert(!air_solid_crosses_raw_origin(crossing));

    AirSolidGeometrySegment high{};
    high.start = endpoint(100.0F, 100.0F, 0.0F);
    high.end = endpoint(0.0F, 0.0F, 10.0F);
    high = clip_air_solid_geometry_segment(high);
    assert(high.enabled);
    assert(near(high.start.projected, 50.0F));
    assert(near(high.start.lateral, 5.0F));
    assert(near(high.start.vertical_a, 15.0F));
    assert(near(high.start.vertical_b, 25.0F));
    assert(near(high.start.decoded_width, 7.0F));
    assert(near(high.start.coordinate_v, 35.0F));
    // Projected clipping does not rewrite the separate raw-relative value.
    assert(near(high.start.raw_relative, 100.0F));

    AirSolidGeometrySegment far{};
    far.start = endpoint(-700.0F, -700.0F, 0.0F);
    far.end = endpoint(0.0F, 0.0F, 10.0F);
    far = clip_air_solid_geometry_segment(far);
    assert(far.enabled);
    assert(near(far.start.projected, -600.0F));
    assert(near(far.start.lateral, 10.0F / 7.0F));

    AirSolidGeometrySegment rejected{};
    rejected.start = endpoint(0.0F, 51.0F, 0.0F);
    rejected.end = endpoint(1.0F, 60.0F, 10.0F);
    rejected = clip_air_solid_geometry_segment(rejected);
    assert(!rejected.enabled);
    assert(build_air_solid_vertices(rejected).empty());

    AirSolidGeometrySegment forward{};
    forward.start = endpoint(-1.0F, 0.0F, 4.0F);
    forward.end = endpoint(1.0F, 10.0F, 8.0F);
    forward.start.decoded_width = 2.0F;
    forward.end.decoded_width = 4.0F;
    forward.start.coordinate_v = 0.25F;
    forward.end.coordinate_v = 0.75F;
    assert(!air_solid_uses_reverse_winding(forward));
    assert(near(air_solid_half_extent(2.0F), 4.0F));
    const auto forward_corners = air_solid_geometry_corners(forward);
    assert(near(forward_corners.start_left_a.lateral, 0.0F));
    assert(near(forward_corners.start_right_a.lateral, 8.0F));
    assert(near(forward_corners.end_left_a.lateral, 0.0F));
    assert(near(forward_corners.end_right_a.lateral, 16.0F));
    assert(forward_corners.start_left_a.color ==
           presentation_static_base_color);
    assert(near(forward_corners.start_left_a.coordinate_u, 0.5F));
    assert(near(forward_corners.start_left_a.coordinate_v, 0.25F));

    const auto forward_vertices = build_air_solid_vertices(forward);
    assert(forward_vertices.size() == 30);
    // Forward bridge: ELA, SLB, ERA, ERA, SLB, SRB.
    assert(near(forward_vertices[0].lateral,
                forward_corners.end_left_a.lateral));
    assert(near(forward_vertices[1].vertical,
                forward_corners.start_left_b.vertical));
    assert(near(forward_vertices[2].lateral,
                forward_corners.end_right_a.lateral));

    AirSolidGeometrySegment reverse = forward;
    reverse.start.projected = 10.0F;
    reverse.end.projected = 0.0F;
    assert(air_solid_uses_reverse_winding(reverse));
    const auto reverse_vertices = build_air_solid_vertices(reverse);
    assert(reverse_vertices.size() == 30);
    const auto reverse_corners = air_solid_geometry_corners(reverse);
    // Reverse bridge: ELA, ERA, SLB, SLB, ERA, SRB.
    assert(near(reverse_vertices[0].lateral,
                reverse_corners.end_left_a.lateral));
    assert(near(reverse_vertices[1].lateral,
                reverse_corners.end_right_a.lateral));
    assert(near(reverse_vertices[2].vertical,
                reverse_corners.start_left_b.vertical));

    reverse.emit_bridge = false;
    assert(build_air_solid_vertices(reverse).size() == 24);
    reverse.emit_bridge = true;
    reverse.emit_shell = false;
    assert(build_air_solid_vertices(reverse).size() == 6);

    forward.end.projected = forward.start.projected;
    assert(!air_solid_uses_reverse_winding(forward));
    forward.end.projected = nan;
    assert(!air_solid_uses_reverse_winding(forward));
}
