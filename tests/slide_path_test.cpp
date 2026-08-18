#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <map>
#include <stdexcept>

int main() {
    using chart::reconstruction::HoldGapState;
    using chart::reconstruction::HoldLaneSourceState;
    using chart::reconstruction::SlideCheckpointProgress;
    using chart::reconstruction::SlideEndpointProfile;
    using chart::reconstruction::SlideGeneratedPath;
    using chart::reconstruction::SlideGeometryVertex;
    using chart::reconstruction::SlidePathPoint;
    using chart::reconstruction::SlidePathPhase;
    using chart::reconstruction::SlidePresentationMode;
    using chart::reconstruction::SlidePresentationPoint;
    using chart::reconstruction::SlidePresentationSegment;
    using chart::reconstruction::SlideStartPhase;
    using chart::reconstruction::SlideWindowPhase;
    using chart::reconstruction::classify_slide_window;
    using chart::reconstruction::build_slide_generated_path;
    using chart::reconstruction::build_slide_center_stream_vertices;
    using chart::reconstruction::build_slide_main_stream_vertices;
    using chart::reconstruction::build_slide_overlay_stream_vertices;
    using chart::reconstruction::build_slide_presentation_geometry;
    using chart::reconstruction::c2s_slide_endpoint_width;
    using chart::reconstruction::clip_slide_presentation_segment;
    using chart::reconstruction::combine_slide_window_phases;
    using chart::reconstruction::slide_command_sets_path_marker;
    using chart::reconstruction::SlideCommandForm;
    using chart::reconstruction::select_slide_feedback_resource;
    using chart::reconstruction::slide_bounded_style_resource_index;
    using chart::reconstruction::slide_endpoint_profile_index;
    using chart::reconstruction::slide_feedback_code;
    using chart::reconstruction::slide_generated_endpoint_resource_present;
    using chart::reconstruction::slide_generated_endpoint_decoded_width;
    using chart::reconstruction::slide_generated_endpoint_resource_visible;
    using chart::reconstruction::slide_generated_segment_result_table_index;
    using chart::reconstruction::slide_checkpoint_source_category;
    using chart::reconstruction::slide_exposes_candidate;
    using chart::reconstruction::slide_gap_active;
    using chart::reconstruction::slide_is_terminal;
    using chart::reconstruction::slide_joint_submission_order;
    using chart::reconstruction::slide_base_main_stream_color;
    using chart::reconstruction::slide_center_stream_color;
    using chart::reconstruction::slide_alternate_main_stream_color;
    using chart::reconstruction::slide_shared_low_alpha_color;
    using chart::reconstruction::slide_main_stream_color;
    using chart::reconstruction::slide_mode_one_intensity;
    using chart::reconstruction::slide_overlay_stream_color;
    using chart::reconstruction::slide_presentation_mode;
    using chart::reconstruction::slide_primitive_counter_categories;
    using chart::reconstruction::slide_stream_topology_modes;
    using chart::reconstruction::slide_root_uses_extended_resource;
    using chart::reconstruction::slide_unresolved_result_table_index;
    using chart::reconstruction::require_slide_generated_path;
    using chart::reconstruction::update_hold_gap;
    using chart::reconstruction::update_slide_checkpoints;
    using chart::reconstruction::update_slide_lane_sources;

    constexpr std::array<float, 4> edges{1.0F, 2.0F, 3.0F, 4.0F};
    assert(classify_slide_window(false, 2.5F, edges) ==
           SlideWindowPhase::disabled);
    assert(classify_slide_window(true, 0.999F, edges) ==
           SlideWindowPhase::before);
    assert(classify_slide_window(true, 1.0F, edges) ==
           SlideWindowPhase::early);
    assert(classify_slide_window(true, 1.999F, edges) ==
           SlideWindowPhase::early);
    assert(classify_slide_window(true, 2.0F, edges) ==
           SlideWindowPhase::center);
    assert(classify_slide_window(true, 3.0F, edges) ==
           SlideWindowPhase::center);
    assert(classify_slide_window(true, 3.001F, edges) ==
           SlideWindowPhase::late);
    assert(classify_slide_window(true, 4.0F, edges) ==
           SlideWindowPhase::late);
    assert(classify_slide_window(true, 4.001F, edges) ==
           SlideWindowPhase::after);

    assert(combine_slide_window_phases(SlideWindowPhase::early,
                                       SlideWindowPhase::late) ==
           SlideWindowPhase::late);
    assert(combine_slide_window_phases(SlideWindowPhase::late,
                                       SlideWindowPhase::center) ==
           SlideWindowPhase::center);

    assert(slide_command_sets_path_marker(SlideCommandForm::sld));
    assert(slide_command_sets_path_marker(SlideCommandForm::sxd));
    assert(!slide_command_sets_path_marker(SlideCommandForm::slc));
    assert(!slide_command_sets_path_marker(SlideCommandForm::sxc));
    assert(!slide_root_uses_extended_resource(SlideCommandForm::sld));
    assert(slide_root_uses_extended_resource(SlideCommandForm::sxd));
    assert(!slide_root_uses_extended_resource(SlideCommandForm::slc));
    assert(slide_root_uses_extended_resource(SlideCommandForm::sxc));
    assert(slide_feedback_code("UP") == 0);
    assert(slide_feedback_code("DW") == 1);
    assert(slide_feedback_code("BS") == 7);
    assert(slide_feedback_code("unknown") == 0);
    assert(slide_bounded_style_resource_index(-1) == 0);
    assert(slide_bounded_style_resource_index(0) == 0);
    assert(slide_bounded_style_resource_index(2) == 2);
    assert(slide_bounded_style_resource_index(9) == 2);
    assert(slide_generated_endpoint_resource_present(true));
    assert(!slide_generated_endpoint_resource_present(false));
    assert(slide_generated_endpoint_decoded_width(8, 2) == 2);
    assert(slide_generated_endpoint_decoded_width(2, 8) == 8);
    assert(c2s_slide_endpoint_width(6, 4, 0) == 4);
    assert(c2s_slide_endpoint_width(7, 4, 12) == 12);
    assert(c2s_slide_endpoint_width(8, 4, 12) == 12);
    assert(c2s_slide_endpoint_width(8, 4, 0) == 1);
    assert(slide_unresolved_result_table_index == 0xff);
    assert(slide_generated_segment_result_table_index(true, 2) == 2);
    assert(slide_generated_segment_result_table_index(false, 2) == 4);
    assert(slide_generated_endpoint_resource_visible(
        slide_unresolved_result_table_index, 5));
    assert(!slide_generated_endpoint_resource_visible(0, 5));
    assert(!slide_generated_endpoint_resource_visible(4, 5));
    assert(slide_generated_endpoint_resource_visible(5, 5));
    assert(slide_generated_endpoint_resource_visible(0, 256));
    assert(!slide_generated_endpoint_resource_visible(0, 257));
    constexpr std::array<int, 8> primary{10, 11, 12, 13, 14, 15, 16, 17};
    constexpr std::array<int, 8> alternate{20, 21, 22, 23, 24, 25, 26, 27};
    assert(select_slide_feedback_resource(3, false, primary, alternate, -1) ==
           13);
    assert(select_slide_feedback_resource(3, true, primary, alternate, -1) ==
           23);
    assert(select_slide_feedback_resource(-1, false, primary, alternate, -1) ==
           -1);
    assert(select_slide_feedback_resource(8, false, primary, alternate, -1) ==
           -1);

    assert(slide_presentation_mode(SlideStartPhase::awaiting_result,
                                   SlidePathPhase::before_start) ==
           SlidePresentationMode::base);
    assert(slide_presentation_mode(SlideStartPhase::awaiting_result,
                                   SlidePathPhase::best_current_gap) ==
           SlidePresentationMode::hide_past_with_overlay);
    assert(slide_presentation_mode(SlideStartPhase::resolved,
                                   SlidePathPhase::other_current_gap) ==
           SlidePresentationMode::alternate_color);
    assert(slide_presentation_mode(SlideStartPhase::resolved,
                                   SlidePathPhase::complete) ==
           SlidePresentationMode::hide_past_with_overlay);
    assert(slide_presentation_mode(SlideStartPhase::awaiting_result,
                                   SlidePathPhase::complete) ==
           SlidePresentationMode::base);
    assert(slide_primitive_counter_categories ==
           (std::array<std::int32_t, 3>{1, 2, 3}));
    assert(slide_stream_topology_modes ==
           (std::array<std::int32_t, 3>{4, 3, 3}));
    assert(slide_joint_submission_order ==
           (std::array<std::int32_t, 3>{0, 1, 2}));
    assert(slide_base_main_stream_color == 0xffffffffU);
    assert(slide_center_stream_color == 0xffffffffU);
    assert(slide_alternate_main_stream_color == 0xff666666U);
    assert(slide_shared_low_alpha_color == 0x40ffffffU);
    assert(slide_main_stream_color(SlidePresentationMode::base) ==
           slide_base_main_stream_color);
    assert(slide_main_stream_color(SlidePresentationMode::alternate_color) ==
           slide_alternate_main_stream_color);
    assert(slide_main_stream_color(
               SlidePresentationMode::hide_past_with_overlay) ==
           slide_base_main_stream_color);
    assert(std::fabs(slide_mode_one_intensity(0.0F) - 1.5F) < 0.0001F);
    assert(std::fabs(slide_mode_one_intensity(5.0F) - 1.75F) < 0.0001F);
    assert(std::fabs(slide_mode_one_intensity(10.0F) - 1.5F) < 0.0001F);

    const std::array presentation_points{
        SlidePresentationPoint{2.0F, 2.0F, false},
        SlidePresentationPoint{2.0F, 4.0F, false},
        SlidePresentationPoint{4.0F, 6.0F, true},
    };
    const std::array raw_positions{10.0F, 20.0F, 50.0F};
    const std::array projected_positions{-100.0F, -50.0F, 0.0F};
    auto presentation = build_slide_presentation_geometry(
        presentation_points, raw_positions, projected_positions,
        SlidePresentationMode::base, -65.0F);
    assert(presentation.cardinality_valid);
    assert(presentation.segments.size() == 2);
    assert(presentation.segments[0].start_marker);
    assert(!presentation.segments[0].end_marker);
    assert(presentation.segments[1].end_marker);
    assert(std::fabs(presentation.segments[0].coordinate_start - 0.0F) <
           0.0001F);
    assert(std::fabs(presentation.segments[0].coordinate_end - 0.25F) <
           0.0001F);
    assert(std::fabs(presentation.segments[1].coordinate_start - 0.25F) <
           0.0001F);
    assert(std::fabs(presentation.segments[1].coordinate_end - 1.0F) <
           0.0001F);
    const std::array grouped_points{
        SlidePresentationPoint{2.0F, 1.0F, false},
        SlidePresentationPoint{2.0F, 2.0F, true},
        SlidePresentationPoint{2.0F, 3.0F, false},
        SlidePresentationPoint{2.0F, 4.0F, false},
    };
    const std::array grouped_raw{10.0F, 20.0F, 40.0F, 60.0F};
    const std::array grouped_projected{-150.0F, -100.0F, -50.0F, 0.0F};
    const auto grouped = build_slide_presentation_geometry(
        grouped_points, grouped_raw, grouped_projected,
        SlidePresentationMode::base, -65.0F);
    assert(grouped.segments.size() == 3);
    assert(grouped.segments[0].end_marker);
    assert(grouped.segments[1].start_marker);
    assert(std::fabs(grouped.segments[0].coordinate_end - 1.0F) < 0.0001F);
    assert(std::fabs(grouped.segments[1].coordinate_start - 0.0F) < 0.0001F);
    assert(std::fabs(grouped.segments[1].coordinate_end - 0.5F) < 0.0001F);
    assert(std::fabs(grouped.segments[2].coordinate_end - 1.0F) < 0.0001F);
    const std::array invalid_projected{-100.0F, -50.0F};
    assert(!build_slide_presentation_geometry(
                presentation_points, raw_positions, invalid_projected,
                SlidePresentationMode::base, -65.0F)
                .cardinality_valid);

    const std::array crossing_points{
        SlidePresentationPoint{2.0F, 2.0F, false},
        SlidePresentationPoint{4.0F, 6.0F, true},
    };
    const std::array crossing_raw{-10.0F, 10.0F};
    const std::array crossing_projected{-100.0F, 0.0F};
    auto crossing = build_slide_presentation_geometry(
        crossing_points, crossing_raw, crossing_projected,
        SlidePresentationMode::base, -65.0F);
    assert(crossing.segments.size() == 2);
    assert(crossing.segments[0].raw_start == 0.0F);
    assert(crossing.segments[0].raw_end == 0.0F);
    assert(std::fabs(crossing.segments[0].projected_end + 65.0F) < 0.0001F);
    assert(std::fabs(crossing.segments[1].projected_start + 65.0F) < 0.0001F);
    assert(std::fabs(crossing.segments[0].coordinate_end - 0.5F) < 0.0001F);
    assert(std::fabs(crossing.segments[1].coordinate_start - 0.5F) <
           0.0001F);
    crossing = build_slide_presentation_geometry(
        crossing_points, crossing_raw, crossing_projected,
        SlidePresentationMode::hide_past_with_overlay, -65.0F);
    assert(!crossing.segments[0].visible);
    assert(crossing.segments[1].visible);

    SlidePresentationSegment clipped{
        .visible = true,
        .projected_start = 100.0F,
        .projected_end = -700.0F,
        .lateral_start = 0.0F,
        .lateral_end = 80.0F,
        .width_start = 2.0F,
        .width_end = 4.0F,
        .coordinate_start = 0.0F,
        .coordinate_end = 1.0F,
    };
    clip_slide_presentation_segment(clipped);
    assert(clipped.visible);
    assert(std::fabs(clipped.projected_start - 50.0F) < 0.0001F);
    assert(std::fabs(clipped.projected_end + 600.0F) < 0.0001F);
    assert(std::fabs(clipped.lateral_start - 5.0F) < 0.0001F);
    assert(std::fabs(clipped.lateral_end - 70.0F) < 0.0001F);
    assert(std::fabs(clipped.coordinate_start - 0.0625F) < 0.0001F);
    assert(std::fabs(clipped.coordinate_end - 0.875F) < 0.0001F);
    clipped = {
        .visible = true,
        .projected_start = -700.0F,
        .projected_end = 100.0F,
        .lateral_start = 0.0F,
        .lateral_end = 80.0F,
        .width_start = 2.0F,
        .width_end = 4.0F,
        .coordinate_start = 0.0F,
        .coordinate_end = 1.0F,
    };
    clip_slide_presentation_segment(clipped);
    assert(clipped.visible);
    assert(std::fabs(clipped.projected_start + 600.0F) < 0.0001F);
    assert(std::fabs(clipped.projected_end - 50.0F) < 0.0001F);
    assert(std::fabs(clipped.lateral_start - 10.0F) < 0.0001F);
    assert(std::fabs(clipped.lateral_end - 75.0F) < 0.0001F);

    SlidePresentationSegment render_segment{
        .visible = true,
        .projected_start = 0.0F,
        .projected_end = 10.0F,
        .lateral_start = 0.0F,
        .lateral_end = 4.0F,
        .width_start = 2.0F,
        .width_end = 2.0F,
        .coordinate_start = 0.0F,
        .coordinate_end = 1.0F,
    };
    auto main_vertices =
        build_slide_main_stream_vertices(render_segment, 0x12345678U);
    assert(main_vertices.size() == 6);
    assert(std::fabs(main_vertices[0].lateral + 4.0F) < 0.0001F);
    assert(main_vertices[0].color == 0x12345678U);
    assert(main_vertices[0].coordinate_u == 0.0F);
    assert(main_vertices[2].coordinate_u == 1.0F);
    render_segment.width_end = 4.0F;
    main_vertices =
        build_slide_main_stream_vertices(render_segment, 0x12345678U);
    assert(main_vertices.size() == 18);
    assert(std::fabs(main_vertices[0].coordinate_u - 0.15F) < 0.0001F);
    assert(std::fabs(main_vertices[2].coordinate_u - 0.85F) < 0.0001F);
    render_segment.width_end = 2.0F;
    render_segment.projected_start = 10.0F;
    render_segment.projected_end = 0.0F;
    main_vertices =
        build_slide_main_stream_vertices(render_segment, 0x12345678U);
    assert(main_vertices.size() == 6);
    assert(std::fabs(main_vertices[1].lateral - 8.0F) < 0.0001F);
    assert(std::fabs(main_vertices[2].lateral - 0.0F) < 0.0001F);
    const auto center_vertices =
        build_slide_center_stream_vertices(render_segment);
    assert(center_vertices.size() == 6);
    assert(std::fabs(center_vertices[0].lateral + 2.0F) < 0.0001F);
    for (const SlideGeometryVertex& vertex : center_vertices) {
        assert(vertex.color == slide_center_stream_color);
    }
    assert(build_slide_overlay_stream_vertices(
               render_segment, SlidePresentationMode::base)
               .empty());
    const auto overlay_vertices = build_slide_overlay_stream_vertices(
        render_segment, SlidePresentationMode::hide_past_with_overlay);
    assert(overlay_vertices.size() == 6);
    for (const SlideGeometryVertex& vertex : overlay_vertices) {
        assert(vertex.color == slide_overlay_stream_color);
    }
    assert(slide_endpoint_profile_index(-10) == 15);
    assert(slide_endpoint_profile_index(1) == 15);
    assert(slide_endpoint_profile_index(16) == 0);
    assert(slide_endpoint_profile_index(99) == 0);

    std::array<SlideEndpointProfile, 16> profiles{};
    for (auto& profile : profiles) {
        profile.lane_reference = 2.0F;
        profile.edge_offsets = {-2.0F, -1.0F, 1.0F, 2.0F};
    }
    const SlidePathPoint root{
        .scheduled_position = 10.0F,
        .lane = 0,
        .width = 2,
        .marker = false,
    };
    const std::array controls{
        SlidePathPoint{
            .scheduled_position = 20.0F,
            .lane = 2,
            .width = 2,
            .marker = false,
        },
        SlidePathPoint{
            .scheduled_position = 30.0F,
            .lane = 2,
            .width = 2,
            .marker = false,
        },
    };
    const auto generated =
        build_slide_generated_path(root, controls, profiles, 0.5F);
    assert(generated.enabled);
    assert(generated.edges ==
           (std::array<float, 4>{8.5F, 9.5F, 31.5F, 32.5F}));
    assert(generated.segments.size() == 2);
    assert(generated.segments[0].start_marker);
    assert(!generated.segments[0].end_marker);
    assert(!generated.segments[0].final_segment);
    assert(!generated.segments[1].start_marker);
    assert(generated.segments[1].end_marker);
    assert(generated.segments[1].final_segment);

    const std::map<std::uint32_t, SlideGeneratedPath> paths{{7U, generated}};
    assert(&require_slide_generated_path(paths, 7U) == &paths.at(7U));
    bool missing_key_threw = false;
    try {
        (void)require_slide_generated_path(paths, 8U);
    } catch (const std::out_of_range&) {
        missing_key_threw = true;
    }
    assert(missing_key_threw);

    // A move from lanes [0,2) to [2,4) generates the full swept corridor.
    const auto& first = generated.segments[0];
    assert(first.lane_windows[0].enabled);
    assert(first.lane_windows[1].enabled);
    assert(first.lane_windows[2].enabled);
    assert(first.lane_windows[3].enabled);
    assert(!first.lane_windows[4].enabled);
    assert(first.lane_windows[0].edges[0] == 8.5F);
    assert(first.lane_windows[3].edges[3] == 22.5F);
    assert(first.lane_windows[2].edges[0] == 11.0F);
    assert(19.0F < first.lane_windows[1].edges[2]);
    assert(first.lane_windows[1].edges[2] < 21.0F);

    HoldLaneSourceState sources;
    std::array<bool, 2> marker{};

    // Participating edge phases arm released banks, but do not sustain.
    assert(!update_slide_lane_sources(sources, {false, false}, {false, false},
                                      SlideWindowPhase::early, marker));
    assert(sources.armed[0] && sources.armed[1]);
    assert(!update_slide_lane_sources(sources, {true, false}, {false, false},
                                      SlideWindowPhase::late, marker));
    assert(!sources.active[0] && !marker[0]);

    // The center phase activates an armed held bank and writes its marker.
    assert(update_slide_lane_sources(sources, {true, false}, {false, false},
                                     SlideWindowPhase::center, marker));
    assert(sources.active[0] && marker[0]);

    // Leaving phases 3..5 clears arming and activity.
    assert(!update_slide_lane_sources(sources, {true, true}, {true, true},
                                      SlideWindowPhase::after, marker));
    assert(!sources.armed[0] && !sources.armed[1]);
    assert(!sources.active[0] && !sources.active[1]);

    assert(!slide_gap_active(false, false));
    assert(slide_gap_active(true, false));
    assert(slide_gap_active(false, true));

    assert(slide_checkpoint_source_category(2) == 4);
    assert(slide_checkpoint_source_category(3) == 5);
    assert(slide_checkpoint_source_category(4) == 6);
    assert(slide_checkpoint_source_category(1) == -1);

    HoldGapState gap{
        .update_gate = 0.0F,
        .participation_floor = 0.0F,
        .end = 20.0F,
        .thresholds = {0.5F, 1.0F, 2.0F, 4.0F},
    };
    assert(update_hold_gap(gap, 0.0F, true) == 0);
    assert(update_hold_gap(gap, 1.5F, false) == 2);

    SlideCheckpointProgress progress{.remaining = 2};
    auto decision = update_slide_checkpoints(
        progress, gap, false, 2, true, true, 2);
    assert(!decision.consumed && progress.remaining == 2);
    assert(progress.phase == SlidePathPhase::before_start);

    decision = update_slide_checkpoints(progress, gap, true, 2, true, false, 3);
    assert(decision.consumed && !decision.emit);
    assert(decision.ordinary_result_byte == 2);
    assert(decision.source_category == 5);
    assert(progress.remaining == 1);
    assert(progress.phase == SlidePathPhase::other_current_gap);
    assert(gap.maximum_gap == 1.5F);

    // Even with another overdue entry, one call removes only one front.
    decision = update_slide_checkpoints(progress, gap, true, 0, true, true, 4);
    assert(decision.consumed && decision.emit);
    assert(progress.remaining == 0);
    assert(progress.phase == SlidePathPhase::complete);

    assert(slide_exposes_candidate(SlideStartPhase::awaiting_result));
    assert(!slide_exposes_candidate(SlideStartPhase::resolved));
    assert(!slide_is_terminal(SlideStartPhase::awaiting_result,
                              SlidePathPhase::complete));
    assert(slide_is_terminal(SlideStartPhase::resolved,
                             SlidePathPhase::complete));
}
