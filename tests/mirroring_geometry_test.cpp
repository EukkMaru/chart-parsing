#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string_view>

using namespace chart::reconstruction;

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.00001F;
}

}  // namespace

int main() {
    // Every valid integer lane span reflects about field center after width
    // clamping. This is the common root path for TAP/HLD/CHR/FLK/MNE and the
    // root/control path for SLD/AHD/ASD/ALD/ASO/SLA/HHD families.
    for (std::int32_t width = 1; width <= 16; ++width) {
        for (std::int32_t lane = 0; lane <= 16 - width; ++lane) {
            const auto mirrored = mirror_c2s_lane_origin(lane, width);
            const float original_lateral =
                tap_model_lateral_center(lane, static_cast<float>(width));
            const float mirrored_lateral = tap_model_lateral_center(
                mirrored, static_cast<float>(width));
            assert(near(mirrored_lateral, -original_lateral));
        }
    }

    // Malformed lane arithmetic follows the executable's wrapped i32
    // subtraction rather than C++ signed-overflow behavior.
    assert(mirror_c2s_lane_origin(
               std::numeric_limits<std::int32_t>::min(), 16) ==
           std::numeric_limits<std::int32_t>::min());

    const std::array<std::string_view, 5> hold_fields{
        "1", "2", "3", "4", "5"};
    assert(parse_c2s_hold_geometry(hold_fields, true).lane == 9);

    const std::array<std::string_view, 6> sla_fields{
        "1", "2", "3", "4", "5", "6"};
    assert(parse_c2s_sla_record(sla_fields, true).lane == 9);

    const std::array<std::string_view, 12> aso_fields{
        "1", "2", "1", "4", "0", "0",
        "5", "8", "3", "0", "0", "DEF"};
    const auto aso = parse_c2s_aso_record(aso_fields, true);
    assert(aso.start.lane == 11);
    assert(aso.end.lane == 5);

    constexpr std::array<AirCommand, 6> commands{
        AirCommand::air, AirCommand::aur, AirCommand::aul,
        AirCommand::adw, AirCommand::adr, AirCommand::adl};
    constexpr std::array<AirDirectionCode, 6> mirrored_directions{
        AirDirectionCode::air, AirDirectionCode::aul,
        AirDirectionCode::aur, AirDirectionCode::adw,
        AirDirectionCode::adl, AirDirectionCode::adr};
    for (std::size_t index = 0; index < commands.size(); ++index) {
        assert(air_direction_code(commands[index], true) ==
               mirrored_directions[index]);
    }
    assert(air_direction_resource_scale_sign(1) == 1.0F);
    assert(air_direction_resource_scale_sign(2) == -1.0F);
    assert(air_direction_resource_scale_sign(4) == -1.0F);
    assert(air_direction_resource_scale_sign(5) == 1.0F);

    const float center = 5.5F;
    const float reflected_center = mirror_presentation_lane_center(center);
    assert(near(presentation_lane_coordinate_lateral(reflected_center),
                -presentation_lane_coordinate_lateral(center)));
    assert(near(air_ladder_render_lateral(reflected_center),
                -air_ladder_render_lateral(center)));
    assert(near(slide_render_lateral(reflected_center),
                -slide_render_lateral(center)));

    const auto hold = prepare_hold_body_geometry(
        HoldPresentationMode::base, 1.0F, 10.0F, 2.0F, 0.0F, 0.0F,
        center, 3.0F);
    const auto mirrored_hold = prepare_hold_body_geometry(
        HoldPresentationMode::base, 1.0F, 10.0F, 2.0F, 0.0F, 0.0F,
        reflected_center, 3.0F);
    assert(hold.enabled && mirrored_hold.enabled);
    assert(near(mirrored_hold.left, -hold.right));
    assert(near(mirrored_hold.right, -hold.left));

    const std::array<SlidePresentationPoint, 2> slide_points{{
        {2.0F, 3.0F, true}, {4.0F, 10.0F, true}}};
    const std::array<SlidePresentationPoint, 2> mirrored_slide_points{{
        {2.0F, 13.0F, true}, {4.0F, 6.0F, true}}};
    const std::array<float, 2> raw{{-1.0F, 1.0F}};
    const std::array<float, 2> projected{{10.0F, -10.0F}};
    const auto slide = build_slide_presentation_geometry(
        slide_points, raw, projected, SlidePresentationMode::base, 0.0F);
    const auto mirrored_slide = build_slide_presentation_geometry(
        mirrored_slide_points, raw, projected,
        SlidePresentationMode::base, 0.0F);
    assert(slide.cardinality_valid && mirrored_slide.cardinality_valid);
    assert(slide.segments.size() == mirrored_slide.segments.size());
    for (std::size_t index = 0; index < slide.segments.size(); ++index) {
        assert(near(mirrored_slide.segments[index].lateral_start,
                    -slide.segments[index].lateral_start));
        assert(near(mirrored_slide.segments[index].lateral_end,
                    -slide.segments[index].lateral_end));
    }

    AirSolidGeometrySegment solid;
    solid.start.lateral = -12.0F;
    solid.start.decoded_width = 2.0F;
    solid.end.lateral = 20.0F;
    solid.end.decoded_width = 3.0F;
    auto mirrored_solid = solid;
    mirrored_solid.start.lateral = -solid.start.lateral;
    mirrored_solid.end.lateral = -solid.end.lateral;
    const auto corners = air_solid_geometry_corners(solid);
    const auto mirrored_corners =
        air_solid_geometry_corners(mirrored_solid);
    assert(near(mirrored_corners.start_left_a.lateral,
                -corners.start_right_a.lateral));
    assert(near(mirrored_corners.end_right_a.lateral,
                -corners.end_left_a.lateral));

    // HeavenHold's authored selector transform is independent from chart
    // mirror and therefore composes after these mirrored parsed coordinates.
    assert(!heaven_hold_mirrors_authored_endpoints(0));
    assert(heaven_hold_mirrors_authored_endpoints(1));
}
