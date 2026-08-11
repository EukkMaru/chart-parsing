#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>

namespace {

bool near(float left, float right, float tolerance = 0.0001F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(presentation_scene_traversal[0].name == "BgScene");
    assert(presentation_scene_traversal[0].render_priority == 9900);
    assert(presentation_scene_traversal[0].draw_index == 16);
    assert(!presentation_scene_traversal[0].immediate);
    assert(presentation_scene_traversal[1].name == "MainScene");
    assert(presentation_scene_traversal[1].render_priority == 10000);
    assert(presentation_scene_traversal[1].draw_index == 0);
    assert(presentation_scene_traversal[1].immediate);

    const BasicCameraParameters defaults{};
    assert(defaults.near_plane == 1.0F);
    assert(defaults.far_plane == 30000.0F);
    assert(defaults.vertical_fov_degrees == 45.0F);
    assert((defaults.position == PresentationVec3{0.0F, 0.0F, 30.0F}));
    assert((defaults.target == PresentationVec3{}));
    assert((defaults.up == PresentationVec3{0.0F, 1.0F, 0.0F}));
    assert(resolve_camera_aspect(1920, 1080) == 1920.0F / 1080.0F);
    assert(resolve_camera_aspect(320, 0) == 320.0F);

    const auto default_view = make_presentation_view(
        defaults.position, defaults.target, defaults.up);
    assert(near(default_view.at(0, 0), 1.0F));
    assert(near(default_view.at(1, 1), 1.0F));
    assert(near(default_view.at(2, 2), 1.0F));
    assert(near(default_view.at(2, 3), -30.0F));
    assert(near(default_view.at(3, 3), 1.0F));

    BasicCameraParameters camera = defaults;
    camera.aspect = resolve_camera_aspect(1920, 1080);
    const auto projection = make_presentation_perspective(
        camera.vertical_fov_degrees,
        camera.aspect,
        camera.near_plane,
        camera.far_plane);
    assert(near(projection.at(3, 2), -1.0F));
    assert(projection.at(0, 0) < projection.at(1, 1));
    assert(projection.at(2, 2) < -1.0F);

    const auto default_projection_view =
        build_presentation_projection_view(camera);
    const auto center = project_presentation_point(
        default_projection_view, {0.0F, 0.0F, 0.0F}, 1920, 1080);
    assert(near(center.x, 960.0F));
    assert(near(center.y, 540.0F));

    // Constructor defaults do not turn chart depth into screen-space Y.
    const auto distant_center = project_presentation_point(
        default_projection_view, {0.0F, 0.0F, -100.0F}, 1920, 1080);
    assert(near(distant_center.y, center.y));

    // A resource-supplied pitch is therefore an explicit input to the same
    // recovered matrix/viewport path, not a canonical fitted constant.
    camera.position = {0.0F, 40.0F, 30.0F};
    camera.target = {0.0F, 0.0F, -70.0F};
    const auto pitched_projection_view =
        build_presentation_projection_view(camera);
    const auto pitched_near = project_presentation_point(
        pitched_projection_view, {0.0F, 0.0F, 0.0F}, 1920, 1080);
    const auto pitched_far = project_presentation_point(
        pitched_projection_view, {0.0F, 0.0F, -100.0F}, 1920, 1080);
    assert(!near(pitched_near.y, pitched_far.y));

    const ExternalCameraMotionControl external_control{};
    assert(external_control.mode == ExternalCameraMotionMode::normal);
    assert(external_control.blend == 1.0F);
    assert(external_control.link == 8.0F);
    assert(external_control.frame == 0.0F);
    assert(external_control.play);
    assert(!external_control.loop);
    assert(!external_control.delete_after_playback);
    assert(gameplay_forester_external_resource_indices[0] == 43);
    assert(gameplay_forester_external_resource_indices[1] == 47);
    assert(gameplay_forester_external_resource_indices[2] == 46);
    assert(gameplay_forester_resource_table_filename ==
           "AcroartsTableRecord.bin");
    assert(gameplay_forester_resource_root == "acroarts/");
    assert(gameplay_forester_invalid_row_string.empty());
    assert(gameplay_forester_layer_table_filename ==
           "LayerTableRecord.bin");
    assert(gameplay_forester_external_layer_indices[0] == 6);
    assert(gameplay_forester_external_layer_indices[1] == 29);
    assert(gameplay_forester_external_layer_indices[2] == 29);
    assert(gameplay_forester_invalid_layer_fallback == 0);
}
