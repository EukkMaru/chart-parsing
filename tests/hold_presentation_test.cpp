#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(hold_root_model_resource_row(1, false, false) == 49);
    assert(hold_root_model_resource_row(16, false, false) == 34);
    assert(hold_root_model_resource_row(1, true, false) == 97);
    assert(hold_root_model_resource_row(16, true, true) == 98);
    assert(hold_far_marker_resource_row(1, false) == 65);
    assert(hold_far_marker_resource_row(16, false) == 50);
    assert(hold_far_marker_resource_row(1, true) == 289);
    assert(hold_far_marker_resource_row(16, true) == 274);

    assert(hold_root_and_far_markers_visible(0));
    assert(!hold_root_and_far_markers_visible(4));
    assert(hold_body_update_enabled(4, 3));
    assert(!hold_body_update_enabled(4, 4));
    assert(hold_presentation_mode(0, 0) == HoldPresentationMode::base);
    assert(hold_presentation_mode(0, 2) == HoldPresentationMode::animated);
    assert(hold_presentation_mode(4, 3) == HoldPresentationMode::alternate);
    assert(hold_presentation_mode(4, 4) == HoldPresentationMode::animated);
    assert(near(hold_animated_resource_scale(0), 1.5F));

    auto crossing = prepare_hold_body_geometry(
        HoldPresentationMode::base,
        -10.0F, -100.0F, 10.0F, 0.0F, -65.0F,
        4.0F, 4.0F);
    assert(crossing.enabled);
    assert(near(crossing.left, -24.0F));
    assert(near(crossing.right, -8.0F));
    assert(near(crossing.start_projected, -65.0F));
    assert(near(crossing.end_projected, 0.0F));
    assert(near(crossing.start_resource_coordinate, 0.5F));
    assert(near(crossing.end_resource_coordinate, 0.0F));

    const auto crossing_vertices = build_hold_body_vertices(crossing);
    assert(crossing_vertices.size() == 6);
    assert(crossing_vertices[0].color == presentation_static_base_color);
    assert(near(crossing_vertices[0].lateral, -24.0F));
    assert(near(crossing_vertices[1].projected, 0.0F));
    assert(near(crossing_vertices[4].lateral, -8.0F));

    auto clipped = prepare_hold_body_geometry(
        HoldPresentationMode::alternate,
        10.0F, 100.0F, 20.0F, -700.0F, -65.0F,
        8.0F, 2.0F);
    assert(clipped.enabled);
    assert(near(clipped.start_projected, 50.0F));
    assert(near(clipped.end_projected, -600.0F));
    assert(near(clipped.start_resource_coordinate, 0.9375F));
    assert(near(clipped.end_resource_coordinate, 0.125F));
    const auto alternate_vertices = build_hold_body_vertices(clipped);
    assert(alternate_vertices[0].color ==
           presentation_static_alternate_color);

    auto animated_override = prepare_hold_body_geometry(
        HoldPresentationMode::animated,
        -20.0F, -100.0F, 10.0F, -200.0F, -65.0F,
        8.0F, 2.0F, 0);
    assert(animated_override.enabled);
    assert(near(animated_override.start_projected, -65.0F));
    assert(near(animated_override.start_resource_coordinate, 1.0F / 3.0F));
    assert(near(animated_override.resource_scale, 1.5F));

    const auto too_near = prepare_hold_body_geometry(
        HoldPresentationMode::base,
        1.0F, 51.0F, 2.0F, 60.0F, -65.0F,
        8.0F, 1.0F);
    assert(!too_near.enabled);
    const auto degenerate = prepare_hold_body_geometry(
        HoldPresentationMode::base,
        1.0F, 0.0F, 1.0F, 0.0F, -65.0F,
        8.0F, 1.0F);
    assert(!degenerate.enabled);
}
