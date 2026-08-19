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

    assert((air_sprite_dynamic_primitive_setup ==
            std::array<std::uint32_t, 4>{4, 3, 6, 1}));
    assert((dynamic_primitive_line_setup ==
            std::array<std::uint32_t, 4>{3, 1, 2, 1}));

    const auto no_texture_rows =
        presentation_joint_texture_source_rows(0);
    for (const auto row : no_texture_rows) {
        assert(row == -1);
    }
    const auto one_texture_row =
        presentation_joint_texture_source_rows(1);
    for (const auto row : one_texture_row) {
        assert(row == 0);
    }
    const auto four_texture_rows =
        presentation_joint_texture_source_rows(4);
    assert(four_texture_rows[0] == 0);
    assert(four_texture_rows[1] == 1);
    assert(four_texture_rows[2] == 2);
    assert(four_texture_rows[3] == 3);
    assert(four_texture_rows[4] == 3);
    assert(four_texture_rows[15] == 3);
    const auto full_texture_rows =
        presentation_joint_texture_source_rows(16);
    assert(full_texture_rows[0] == 0);
    assert(full_texture_rows[15] == 15);
    const auto oversized_texture_rows =
        presentation_joint_texture_source_rows(20);
    assert(oversized_texture_rows[15] == 15);
    assert(!presentation_joint_texture_index_admitted(-1));
    assert(presentation_joint_texture_index_admitted(0));
    assert(presentation_joint_texture_index_admitted(15));
    assert(!presentation_joint_texture_index_admitted(16));

    DynamicPrimitiveFinalizeState finalize_state{
        .layout_selector_08 = 0x10203040U,
        .topology_mode_0c = 0x50607080U,
        .vertex_count_10 = 6U,
        .vertex_stride_14 = 24U,
        .vertex_byte_count_18 = 144U,
        .flag_1c = true,
        .submission_flags_80 = 0x13579bdfU & ~0x80U,
        .submitted_90 = 1U,
        .submitted_94 = 2U,
        .payload_flags_a4 = 0x20U,
        .pending_150 = 0x1234U,
    };
    const auto preserved_submission_bits =
        finalize_state.submission_flags_80 & ~0x80U;
    finalize_dynamic_primitive_state(finalize_state);
    assert(finalize_state.payload_flags_a4 == 0x21U);
    assert((finalize_state.submission_flags_80 & ~0x80U) ==
           preserved_submission_bits);
    assert((finalize_state.submission_flags_80 & 0x80U) != 0U);
    assert(finalize_state.submitted_90 == 0x10203040U);
    assert(finalize_state.submitted_94 == 0x50607080U);
    assert(finalize_state.pending_150 == 0U);

    finalize_state.flag_1c = false;
    finalize_state.submission_flags_80 |= 0x80U;
    finalize_state.pending_150 = 0x5678U;
    finalize_dynamic_primitive_state(finalize_state);
    assert((finalize_state.submission_flags_80 & 0x80U) == 0U);
    assert(finalize_state.pending_150 == 0U);

    set_dynamic_primitive_entry_configuration(
        finalize_state, 4U, 3U, 6U, true, 24U, 0x9abcU);
    assert(finalize_state.layout_selector_08 == 4U);
    assert(finalize_state.topology_mode_0c == 3U);
    assert(finalize_state.vertex_count_10 == 6U);
    assert(finalize_state.vertex_stride_14 == 24U);
    assert(finalize_state.vertex_byte_count_18 == 144U);
    assert(finalize_state.flag_1c);
    assert(finalize_state.submitted_90 == 4U);
    assert(finalize_state.submitted_94 == 3U);
    assert(finalize_state.pending_150 == 0x9abcU);

    // The executable uses 32-bit multiplication for count * stride.
    set_dynamic_primitive_entry_configuration(
        finalize_state, 3U, 1U, 0xffffffffU, false, 16U, 0U);
    assert(finalize_state.vertex_byte_count_18 == 0xfffffff0U);
    assert(!finalize_state.flag_1c);

    PrimitiveTopologyModeState topology_state{
        .packed_mode_word = 0xffffffffU,
        .auxiliary_flags_58 = 0xffffffffU,
        .payload_flags_60 = 0xffffffffU,
    };
    const auto mode2 = chart_primitive_topology_mode_properties(2U);
    assert(mode2.has_value());
    set_primitive_topology_mode(topology_state, 2, *mode2);
    assert((topology_state.packed_mode_word & 0x3fU) == 2U);
    assert((topology_state.payload_flags_60 & 0x20U) != 0U);
    assert((topology_state.payload_flags_60 & 0x40U) != 0U);
    assert((topology_state.payload_flags_60 & 0x800U) == 0U);
    assert((topology_state.auxiliary_flags_58 & 0x08U) == 0U);

    const auto mode3 = chart_primitive_topology_mode_properties(3U);
    const auto mode4 = chart_primitive_topology_mode_properties(4U);
    assert(mode3.has_value() && mode4.has_value());
    set_primitive_topology_mode(topology_state, 3, *mode3);
    assert((topology_state.payload_flags_60 & 0x20U) != 0U);
    assert((topology_state.payload_flags_60 & 0x40U) == 0U);
    set_primitive_topology_mode(topology_state, 4, *mode4);
    assert((topology_state.packed_mode_word & 0x3fU) == 4U);
    assert((topology_state.payload_flags_60 & 0x60U) == 0x20U);

    // Modes above 32 always force payload bit 0x20 even when the selected
    // table row's first property is false.
    set_primitive_topology_mode(topology_state, 33, {false, false});
    assert((topology_state.payload_flags_60 & 0x820U) == 0x820U);
    assert((topology_state.auxiliary_flags_58 & 0x08U) != 0U);
    assert(!chart_primitive_topology_mode_properties(1U).has_value());

    const auto default_sprite = build_air_sprite_quad({});
    assert((default_sprite[0].position ==
            PresentationVec3{0.0F, 0.0F, 0.0F}));
    assert((default_sprite[1].position ==
            PresentationVec3{0.0F, 16.0F, 0.0F}));
    assert((default_sprite[2].position ==
            PresentationVec3{16.0F, 0.0F, 0.0F}));
    assert((default_sprite[3].position == default_sprite[1].position));
    assert((default_sprite[4].position ==
            PresentationVec3{16.0F, 16.0F, 0.0F}));
    assert((default_sprite[5].position == default_sprite[2].position));
    assert(default_sprite[0].u == 0.0F && default_sprite[0].v == 0.0F);
    assert(default_sprite[1].u == 0.0F && default_sprite[1].v == 1.0F);
    assert(default_sprite[2].u == 1.0F && default_sprite[2].v == 0.0F);
    for (const auto& vertex : default_sprite) {
        assert(vertex.color == 0xffffffffU);
    }

    AirSpriteQuadParameters centered_sprite{};
    centered_sprite.width = 4.0F;
    centered_sprite.height = 2.0F;
    centered_sprite.anchor_mode = 4;
    centered_sprite.color = 0x12345678U;
    const auto centered_quad = build_air_sprite_quad(centered_sprite);
    assert((centered_quad[0].position ==
            PresentationVec3{-2.0F, -1.0F, 0.0F}));
    assert((centered_quad[4].position ==
            PresentationVec3{2.0F, 1.0F, 0.0F}));
    assert(centered_quad[4].color == 0x12345678U);

    AirSpriteQuadParameters transformed_sprite{};
    transformed_sprite.width = 2.0F;
    transformed_sprite.height = 1.0F;
    transformed_sprite.anchor_mode = 8;
    transformed_sprite.scale_x = 2.0F;
    transformed_sprite.scale_y = 3.0F;
    transformed_sprite.rotation_radians = 1.57079632679F;
    transformed_sprite.x = 10.0F;
    transformed_sprite.y = 20.0F;
    transformed_sprite.u0 = 0.25F;
    transformed_sprite.v0 = 0.5F;
    PresentationMat4 uv_transform{};
    uv_transform.at(0, 0) = 2.0F;
    uv_transform.at(0, 3) = 1.0F;
    uv_transform.at(1, 1) = 3.0F;
    uv_transform.at(1, 3) = -1.0F;
    transformed_sprite.uv_transform = uv_transform;
    const auto transformed_quad = build_air_sprite_quad(transformed_sprite);
    assert(near(transformed_quad[0].position.x, 13.0F));
    assert(near(transformed_quad[0].position.y, 16.0F));
    assert(near(transformed_quad[4].position.x, 10.0F));
    assert(near(transformed_quad[4].position.y, 20.0F));
    assert(near(transformed_quad[0].u, 1.5F));
    assert(near(transformed_quad[0].v, 0.5F));

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
