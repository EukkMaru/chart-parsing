#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <span>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(!heaven_hold_mirrors_authored_endpoints(0));
    assert(heaven_hold_mirrors_authored_endpoints(1));
    assert(!heaven_hold_mirrors_authored_endpoints(2));
    assert(heaven_hold_mirrors_authored_endpoints(3));
    assert(!heaven_hold_mirrors_authored_endpoints(4));
    assert(!heaven_hold_mirrors_authored_endpoints(10));
    assert(heaven_hold_geometry_mode(0) == 0);
    assert(heaven_hold_geometry_mode(2) == 1);
    assert(heaven_hold_geometry_mode(4) == 0x83);
    assert(heaven_hold_geometry_mode(10) == 0x83);
    assert(near(heaven_hold_retyped_hld_path_scalar(), 1.0F));

    const HeavenHoldAuthoredPresentationPoint root{
        0.0F, 2.0F, 4.0F, 1.0F, 3};
    const std::array<HeavenHoldAuthoredPresentationPoint, 2> controls{{
        {1.0F, 8.0F, 2.0F, 3.0F, 5},
        {2.0F, 10.0F, 4.0F, 5.0F, 7},
    }};
    auto authored = build_heaven_hold_authored_presentation_path(
        root, std::span<const HeavenHoldAuthoredPresentationPoint>{controls},
        0);
    assert(authored.size() == 3);
    assert(near(authored[1].lane, 8.0F));
    assert(authored[2].sla_key == 7);
    authored = build_heaven_hold_authored_presentation_path(
        root, std::span<const HeavenHoldAuthoredPresentationPoint>{controls},
        1);
    // Root center is 4: reflect [8,10] to [-2,0] and [10,14] to [-6,-2].
    assert(near(authored[1].lane, -2.0F));
    assert(near(authored[2].lane, -6.0F));
    assert(build_heaven_hold_authored_presentation_path(
               root,
               std::span<const HeavenHoldAuthoredPresentationPoint>{}, 0)
               .empty());

    const std::array<HeavenHoldGeometryEndpoint, 3> crossing{{
        {-2.0F, 20.0F, 4.0F, 0.0F, 2.0F, 0.0F},
        {2.0F, 10.0F, 8.0F, 4.0F, 4.0F, 0.5F},
        {6.0F, 0.0F, 12.0F, 8.0F, 6.0F, 1.0F},
    }};
    const auto trimmed = trim_heaven_hold_nonzero_mode_at_origin(
        std::span<const HeavenHoldGeometryEndpoint>{crossing});
    assert(trimmed.size() == 3);
    assert(near(trimmed.front().raw_relative, 0.0F));
    assert(near(trimmed.front().projected, 15.0F));
    assert(near(trimmed.front().lateral, 6.0F));
    assert(near(trimmed.front().decoded_width, 3.0F));

    const auto reflected = reflect_heaven_hold_nonzero_mode_geometry(
        std::span<const HeavenHoldGeometryEndpoint>{trimmed});
    assert(reflected.size() == 3);
    // Endpoints remain fixed; the middle point is reverse-reflected.
    assert(near(reflected.front().projected, trimmed.front().projected));
    assert(near(reflected.front().lateral, trimmed.front().lateral));
    assert(near(reflected.back().projected, trimmed.back().projected));
    assert(near(reflected.back().lateral, trimmed.back().lateral));
    assert(near(reflected[1].projected, 5.0F));
    assert(near(reflected[1].lateral, 10.0F));

    HeavenHoldGeometrySegment high{};
    high.start = {0.0F, 100.0F, 0.0F, 1.0F, 2.0F, 0.0F};
    high.end = {1.0F, 0.0F, 10.0F, 3.0F, 4.0F, 1.0F};
    high = clip_heaven_hold_geometry_segment(high);
    assert(high.enabled);
    assert(near(high.start.projected, 50.0F));
    assert(near(high.start.lateral, 5.0F));
    assert(near(high.start.decoded_width, 3.0F));
    // Raw-relative schedule is not changed by projected clipping.
    assert(near(high.start.raw_relative, 0.0F));

    HeavenHoldGeometrySegment rejected{};
    rejected.start.projected = 51.0F;
    rejected.end.projected = 60.0F;
    assert(!clip_heaven_hold_geometry_segment(rejected).enabled);

    HeavenHoldGeometrySegment primitive{};
    primitive.start = {0.0F, 10.0F, 4.0F, 2.0F, 2.0F, 0.25F};
    primitive.end = {1.0F, 0.0F, 8.0F, 6.0F, 4.0F, 0.75F};
    const auto vertices = build_heaven_hold_geometry_vertices(
        primitive, HeavenHoldPresentationMode::base);
    assert(vertices.size() == 6);
    assert(near(heaven_hold_geometry_half_extent(2.0F), 4.0F));
    assert(near(vertices[0].lateral, 0.0F));
    assert(near(vertices[1].lateral, 16.0F));
    assert(near(vertices[2].lateral, 0.0F));
    assert(vertices[0].color == presentation_static_base_color);
    const auto alternate_vertices = build_heaven_hold_geometry_vertices(
        primitive, HeavenHoldPresentationMode::alternate);
    assert(alternate_vertices[0].color ==
           presentation_static_alternate_color);
    assert(near(vertices[0].coordinate_u, 0.0F));
    assert(near(vertices[4].coordinate_u, 1.0F));
    assert(near(vertices[0].coordinate_v, 0.25F));

    assert(heaven_hold_start_resource_visible(
        HeavenHoldStartPhase::awaiting_result));
    assert(!heaven_hold_start_resource_visible(
        HeavenHoldStartPhase::complete));
    assert(heaven_hold_body_resource_visible(
        HeavenHoldPathPhase::before_start));
    assert(!heaven_hold_body_resource_visible(
        HeavenHoldPathPhase::complete));
    assert(heaven_hold_presentation_mode(
               HeavenHoldStartPhase::awaiting_result,
               HeavenHoldPathPhase::before_start) ==
           HeavenHoldPresentationMode::base);
    assert(heaven_hold_presentation_mode(
               HeavenHoldStartPhase::awaiting_result,
               HeavenHoldPathPhase::best_current_gap) ==
           HeavenHoldPresentationMode::animated);
    assert(heaven_hold_presentation_mode(
               HeavenHoldStartPhase::complete,
               HeavenHoldPathPhase::other_current_gap) ==
           HeavenHoldPresentationMode::alternate);
    assert(near(heaven_hold_animated_resource_scale(0), 1.5F));

    assert(!heaven_hold_uses_secondary_body_resource(-1));
    assert(heaven_hold_uses_secondary_body_resource(3));
    assert(heaven_hold_uses_secondary_body_resource(5));
    assert(heaven_hold_uses_secondary_body_resource(8));
    assert(!heaven_hold_uses_secondary_body_resource(13));
}
