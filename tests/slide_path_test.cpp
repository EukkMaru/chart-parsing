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
    using chart::reconstruction::SlidePathPoint;
    using chart::reconstruction::SlidePathPhase;
    using chart::reconstruction::SlideStartPhase;
    using chart::reconstruction::SlideWindowPhase;
    using chart::reconstruction::classify_slide_window;
    using chart::reconstruction::build_slide_generated_path;
    using chart::reconstruction::combine_slide_window_phases;
    using chart::reconstruction::slide_command_sets_path_marker;
    using chart::reconstruction::SlideCommandForm;
    using chart::reconstruction::slide_endpoint_profile_index;
    using chart::reconstruction::slide_checkpoint_source_category;
    using chart::reconstruction::slide_exposes_candidate;
    using chart::reconstruction::slide_gap_active;
    using chart::reconstruction::slide_is_terminal;
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
