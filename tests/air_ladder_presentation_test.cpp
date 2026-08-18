#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <span>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

chart::reconstruction::AirLadderGeometryEndpoint endpoint(
    float projected,
    float value) {
    return chart::reconstruction::AirLadderGeometryEndpoint{
        value + 100.0F,
        projected,
        value,
        value + 10.0F,
        value + 20.0F,
        value + 30.0F,
        value + 40.0F,
        value + 50.0F,
        value != 0.0F,
    };
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    const AirLadderPathPoint root{
        {}, 1.0F, 2.0F, 3.0F, 4.0F};
    const std::array<AirLadderPathPoint, 2> controls{
        AirLadderPathPoint{{}, 2.0F, 5.0F, 6.0F, 2.0F},
        AirLadderPathPoint{{}, 3.0F, 7.0F, 8.0F, 6.0F},
    };
    const auto path = build_air_ladder_authored_geometry_path(
        root, std::span<const AirLadderPathPoint>{controls});
    assert(path.size() == 3);
    assert(near(path[0].lane_center, 4.0F));
    assert(near(path[1].lane_center, 6.0F));
    assert(near(path[2].lane_center, 10.0F));
    assert(!path[0].final_endpoint);
    assert(!path[1].final_endpoint);
    assert(path[2].final_endpoint);
    assert(build_air_ladder_authored_geometry_path(
               root, std::span<const AirLadderPathPoint>{})
               .empty());

    assert(air_ladder_primitive_topologies ==
           (std::array<std::int32_t, 3>{3, 3, 2}));
    assert(air_ladder_primitive_counter_categories ==
           (std::array<std::int32_t, 3>{7, 9, 8}));
    assert(near(air_ladder_style_vertex_coordinate(0), 0.1561999917F));
    assert(near(air_ladder_style_vertex_coordinate(1), 0.9688000083F));
    assert(near(air_ladder_style_vertex_coordinate(15), 0.0F));
    assert(near(air_ladder_style_vertex_coordinate(99),
                air_ladder_style_vertex_coordinate(0)));
    assert(near(air_ladder_render_lateral(10.0F), 8.0F));
    assert(near(air_ladder_render_vertical(2.0F), 3.8934999F));
    assert(near(common_air_render_vertical(1.0F), 0.0F));
    assert(near(common_air_action_render_vertical(1.0F), 0.14999962F));
    assert(air_direction_resource_scale_sign(1) == 1.0F);
    assert(air_direction_resource_scale_sign(2) == -1.0F);
    assert(air_direction_resource_scale_sign(4) == -1.0F);
    assert(air_direction_resource_scale_sign(5) == 1.0F);
    assert(near(air_ladder_stream_zero_half_extent(2.0F), 3.0F));
    assert(near(air_ladder_stream_one_scale(0.0F), 0.75F));
    assert(near(air_ladder_stream_one_scale(15.574F), 0.65F));
    assert(near(air_ladder_stream_one_half_extent(2.0F, 15.574F),
                2.6F));
    assert(near(air_ladder_stream_two_half_extent(false), 1.96F));
    assert(near(air_ladder_stream_two_half_extent(true), 0.98F));

    assert(air_ladder_checkpoint_resource_slot(1.0F) == 0);
    assert(air_ladder_checkpoint_resource_slot(4.0F) == 3);
    assert(air_ladder_checkpoint_resource_slot(20.0F) == 15);
    assert(air_ladder_checkpoint_resource_slot(-2.0F) == 0);
    const auto checkpoint_transform = build_air_ladder_checkpoint_transform(
        2.0F, 4.0F, 3.0F, 2);
    assert(checkpoint_transform.resource_slot == 3);
    assert(near(checkpoint_transform.lateral, -16.0F));
    assert(near(checkpoint_transform.vertical,
                (3.0F - 1.0F) * 3.8934999F + 0.14999962F));
    assert(near(checkpoint_transform.projected, -10000.0F));
    assert(near(checkpoint_transform.lateral_scale, 2.0F));
    assert(near(checkpoint_transform.vertical_scale, 1.0F));
    assert(near(checkpoint_transform.projected_scale, 1.0F));
    assert(near(air_ladder_checkpoint_lateral_scale(4.0F, 0), 1.0F));

    assert(air_ladder_unresolved_result_table_index == 0xff);
    assert(air_ladder_result_table_indices ==
           (std::array<std::uint8_t, 12>{
               0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0}));
    for (std::uint8_t tier = 0;
         tier < air_ladder_result_table_indices.size(); ++tier) {
        assert(air_ladder_result_table_index(tier) ==
               air_ladder_result_table_indices[tier]);
    }
    bool invalid_tier_rejected = false;
    try {
        (void)air_ladder_result_table_index(12);
    } catch (const std::out_of_range&) {
        invalid_tier_rejected = true;
    }
    assert(invalid_tier_rejected);
    assert(air_ladder_checkpoint_effect_visible(
        air_ladder_unresolved_result_table_index, 5));
    assert(!air_ladder_checkpoint_effect_visible(0, 5));
    assert(!air_ladder_checkpoint_effect_visible(4, 5));
    assert(air_ladder_checkpoint_effect_visible(5, 5));
    // The caller stores the table count in a byte before comparing it.
    assert(air_ladder_checkpoint_effect_visible(0, 256));
    assert(!air_ladder_checkpoint_effect_visible(0, 257));

    AirLadderGeometrySegment high{};
    high.start = endpoint(100.0F, 0.0F);
    high.end = endpoint(0.0F, 10.0F);
    high = clip_air_ladder_geometry_segment(high);
    assert(high.enabled);
    assert(near(high.start.projected, 50.0F));
    assert(near(high.start.lateral, 5.0F));
    assert(near(high.start.normalized_right, 55.0F));
    // Raw-relative position and the discrete marker are not clipped.
    assert(near(high.start.raw_relative, 100.0F));
    assert(!high.start.path_marker);

    AirLadderGeometrySegment far{};
    far.start = endpoint(-700.0F, 0.0F);
    far.end = endpoint(0.0F, 10.0F);
    far = clip_air_ladder_geometry_segment(far);
    assert(far.enabled);
    assert(near(far.start.projected, -600.0F));
    assert(near(far.start.lateral, 10.0F / 7.0F));

    AirLadderGeometrySegment both{};
    both.start = endpoint(100.0F, 0.0F);
    both.end = endpoint(-700.0F, 10.0F);
    both = clip_air_ladder_geometry_segment(both);
    assert(both.enabled);
    assert(near(both.start.projected, 50.0F));
    assert(near(both.end.projected, -600.0F));
    assert(near(both.start.lateral, 0.625F));
    assert(near(both.end.lateral, 8.75F));

    AirLadderGeometrySegment rejected{};
    rejected.start = endpoint(51.0F, 0.0F);
    rejected.end = endpoint(60.0F, 10.0F);
    rejected = clip_air_ladder_geometry_segment(rejected);
    assert(!rejected.enabled);

    AirLadderGeometrySegment exact_boundary{};
    exact_boundary.start = endpoint(-601.0F, 0.0F);
    exact_boundary.end = endpoint(-600.0F, 10.0F);
    exact_boundary = clip_air_ladder_geometry_segment(exact_boundary);
    assert(exact_boundary.enabled);
    assert(near(exact_boundary.start.projected, -600.0F));

    AirLadderGeometrySegment primitive{};
    primitive.start = endpoint(10.0F, 4.0F);
    primitive.end = endpoint(0.0F, 8.0F);
    primitive.start.decoded_width = 2.0F;
    primitive.end.decoded_width = 4.0F;
    const auto stream_zero =
        build_air_ladder_stream_zero_vertices(
            primitive, SharedAirPathPresentationMode::base);
    assert(stream_zero.size() == 6);
    assert(near(stream_zero[0].lateral, 1.0F));
    assert(near(stream_zero[1].lateral, 14.0F));
    assert(near(stream_zero[2].lateral, 2.0F));
    assert(near(stream_zero[0].vertical, primitive.start.vertical));
    assert(stream_zero[0].color == presentation_static_base_color);
    assert(near(stream_zero[0].coordinate_u,
                primitive.start.normalized_left));
    assert(near(stream_zero[0].coordinate_v,
                primitive.start.style_coordinate));

    const auto stream_one =
        build_air_ladder_stream_one_vertices(
            primitive, SharedAirPathPresentationMode::alternate);
    assert(stream_one.size() == 6);
    for (const auto& vertex : stream_one) {
        assert(near(vertex.vertical, 0.0F));
        assert(vertex.color == presentation_static_alternate_color);
    }

    const auto stream_two =
        build_air_ladder_stream_two_vertices(primitive, false);
    assert(stream_two.size() == 12);
    assert(near(stream_two[0].lateral, 2.04F));
    assert(near(stream_two[1].lateral, 6.04F));
    assert(near(stream_two[2].lateral, 9.96F));
    assert(near(stream_two[5].lateral, 5.96F));
    // The second six vertices repeat the same quad with opposite winding.
    assert(near(stream_two[6].lateral, stream_two[0].lateral));
    assert(near(stream_two[7].lateral, stream_two[2].lateral));
    assert(near(stream_two[8].lateral, stream_two[1].lateral));
    for (const auto& vertex : stream_two) {
        assert(vertex.color == presentation_static_low_alpha_color);
    }

    const auto animated_stream =
        build_air_ladder_stream_zero_vertices(
            primitive, SharedAirPathPresentationMode::animated);
    assert(animated_stream[0].color == presentation_static_base_color);
}
