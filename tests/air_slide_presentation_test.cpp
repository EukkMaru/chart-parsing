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

    assert(air_slide_resource_width_index(-4) == 0);
    assert(air_slide_resource_width_index(1) == 0);
    assert(air_slide_resource_width_index(16) == 15);
    assert(air_slide_resource_width_index(30) == 15);

    assert(air_slide_presentation_update_enabled(
        AirSlideStartPhase::awaiting_result,
        AirSlidePathPhase::complete));
    assert(!air_slide_presentation_update_enabled(
        AirSlideStartPhase::complete,
        AirSlidePathPhase::complete));
    assert(air_slide_root_resource_visible(
        AirSlideStartPhase::awaiting_result));
    assert(!air_slide_root_resource_visible(
        AirSlideStartPhase::complete));
    assert(air_slide_action_resources_visible(false));
    assert(!air_slide_action_resources_visible(true));

    assert(air_slide_path_presentation_variant(
               AirSlidePathPhase::before_start) ==
           AirSlidePathPresentationVariant::default_state);
    assert(air_slide_path_presentation_variant(
               AirSlidePathPhase::best_current_gap) ==
           AirSlidePathPresentationVariant::best_current_gap);
    assert(air_slide_path_presentation_variant(
               AirSlidePathPhase::other_current_gap) ==
           AirSlidePathPresentationVariant::other_current_gap);
    assert(air_slide_shared_geometry_mode(
               AirSlidePathPhase::before_start) ==
           SharedAirPathPresentationMode::base);
    assert(air_slide_shared_geometry_mode(
               AirSlidePathPhase::best_current_gap) ==
           SharedAirPathPresentationMode::animated);
    assert(air_slide_shared_geometry_mode(
               AirSlidePathPhase::other_current_gap) ==
           SharedAirPathPresentationMode::alternate);
    assert(shared_air_path_primary_color(
               air_slide_shared_geometry_mode(
                   AirSlidePathPhase::other_current_gap)) ==
           presentation_static_alternate_color);
    assert(air_slide_emits_field_feedback(
        AirSlidePathPhase::best_current_gap));
    assert(!air_slide_emits_field_feedback(
        AirSlidePathPhase::other_current_gap));

    assert(air_slide_resource_style_row(0) == 2);
    assert(air_slide_resource_style_row(1) == 0);
    assert(air_slide_resource_style_row(2) == 1);
    assert(air_slide_resource_style_row(99) == 0);
    assert(air_slide_resource_style_offsets(0) ==
           (std::array<std::int32_t, 3>{2, 5, 8}));
    assert(air_slide_resource_style_offsets(1) ==
           (std::array<std::int32_t, 3>{0, 3, 6}));
    assert(air_slide_primitive_topologies ==
           (std::array<std::int32_t, 3>{3, 3, 2}));
    assert(air_slide_primitive_counter_categories ==
           (std::array<std::int32_t, 3>{4, 6, 5}));
    assert(air_slide_action_resource_count == 2);
    assert(air_slide_preload_ready_after_step == 12);

    assert(near(air_slide_initial_root_vertical_value(false, 9.0F),
                1.0F));
    assert(near(air_slide_initial_root_vertical_value(true, 9.0F),
                9.0F));

    const auto root = build_air_slide_root_transform(
        AirSlideStartPhase::awaiting_result,
        10.0F,
        2.0F,
        3.0F,
        -50.0F,
        1.0F,
        2.0F,
        3.0F,
        4,
        2);
    assert(root.visible);
    assert(near(root.lateral, 11.0F));
    assert(near(root.vertical,
                common_air_render_vertical(2.0F) + 2.0F));
    assert(near(root.projected, -47.0F));
    assert(near(root.lateral_scale, 2.0F));
    assert(near(root.vertical_scale,
                common_air_render_vertical(3.0F) *
                    air_slide_root_vertical_scale_factor));
    assert(near(root.projected_scale, 1.0F));

    const auto actions = build_air_slide_action_transforms(
        false,
        12.0F,
        3.0F,
        -100.0F,
        1.0F,
        2.0F,
        3.0F,
        4,
        8);
    assert(actions[0].visible && actions[1].visible);
    assert(near(actions[0].lateral, 13.0F));
    assert(near(actions[0].vertical,
                common_air_action_render_vertical(3.0F) + 2.0F));
    assert(near(actions[0].projected, -97.0F));
    assert(near(actions[0].lateral_scale, 0.5F));
    assert(near(actions[0].vertical_scale, 1.0F));
    assert(near(actions[1].vertical_scale,
                common_air_action_render_vertical(3.0F) *
                    air_slide_action_vertical_scale_factor));

    const AirSlidePresentationPoint presentation_root{
        110.0F, 100.0F, 2.0F, 4.0F, 1.0F};
    const std::array<AirSlidePresentationPoint, 2> controls{
        AirSlidePresentationPoint{120.0F, 105.0F, 4.0F, 2.0F, 3.0F},
        AirSlidePresentationPoint{140.0F, 130.0F, 7.0F, 6.0F, 5.0F},
    };
    const auto schedules = build_air_slide_geometry_schedules(
        100.0F, presentation_root,
        std::span<const AirSlidePresentationPoint>{controls});
    assert(schedules.size() == 3);
    assert(near(schedules[0].adjusted_relative, 10.0F));
    assert(near(schedules[0].raw_relative, 0.0F));
    assert(near(schedules[2].adjusted_relative, 40.0F));
    assert(near(schedules[2].raw_relative, 30.0F));

    const auto path = build_air_slide_authored_geometry_path(
        presentation_root,
        std::span<const AirSlidePresentationPoint>{controls});
    assert(path.size() == 3);
    assert(near(path[0].lane_center, 4.0F));
    assert(near(path[1].lane_center, 5.0F));
    assert(near(path[2].lane_center, 10.0F));
    assert(!path[0].final_endpoint);
    assert(!path[1].final_endpoint);
    assert(path[2].final_endpoint);
    assert(build_air_slide_authored_geometry_path(
               presentation_root,
               std::span<const AirSlidePresentationPoint>{})
               .empty());
}
