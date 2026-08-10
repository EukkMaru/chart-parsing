#include <chart/reconstruction.hpp>

#include <cassert>
#include <cstdint>
#include <vector>

using namespace chart::reconstruction;

int main() {
    RuntimeActiveVectors active{
        .primaries = {2},
        .attached_secondaries = {2},
    };
    append_runtime_factory_events(active, 5, true, true);
    append_runtime_factory_events(active, 7, true, false);

    const auto schedule =
        build_cross_family_outer_update_schedule(active, 2);
    const std::vector<CrossFamilyOuterEvent> one_substep{
        {CrossFamilyOuterEventKind::primary_update, 2},
        {CrossFamilyOuterEventKind::primary_update, 5},
        {CrossFamilyOuterEventKind::primary_update, 7},
        {CrossFamilyOuterEventKind::attached_secondary_update, 2},
        {CrossFamilyOuterEventKind::attached_secondary_update, 5},
    };
    assert(std::vector<CrossFamilyOuterEvent>(
               schedule.begin(), schedule.begin() + 5) == one_substep);
    assert(std::vector<CrossFamilyOuterEvent>(
               schedule.begin() + 5, schedule.begin() + 10) == one_substep);

    // Catch-up does not update transient effects between note substeps. The
    // ten fixed lists run once, in index order, after both substeps.
    for (std::int32_t list = 0; list < 10; ++list) {
        assert((schedule[10U + static_cast<std::size_t>(list)] ==
                CrossFamilyOuterEvent{
                    CrossFamilyOuterEventKind::effect_list_update, list}));
    }
    assert((schedule[20] == CrossFamilyOuterEvent{
                                CrossFamilyOuterEventKind::pending_materialization,
                                0}));
    assert((schedule[21] == CrossFamilyOuterEvent{
                                CrossFamilyOuterEventKind::post_manager_scene_export,
                                0}));

    // Source order is only the equal-key fallback inside one external pass.
    // Different external keys can and do override the primary/secondary
    // update order before draw submission.
    std::vector<PresentationSubmission> equal_keys;
    for (std::uint32_t sequence = 0; sequence < one_substep.size();
         ++sequence) {
        PresentationSubmission submission;
        submission.layer = 4;
        submission.depth = 12.0F;
        submission.source_sequence = sequence;
        equal_keys.push_back(submission);
    }
    sort_presentation_submissions(
        equal_keys, PresentationPassSortMode::layer_lower_depth_upper);
    for (std::uint32_t sequence = 0; sequence < equal_keys.size();
         ++sequence) {
        assert(equal_keys[sequence].source_sequence == sequence);
    }

    equal_keys[0].depth = 1.0F;
    equal_keys[4].depth = 20.0F;
    sort_presentation_submissions(
        equal_keys, PresentationPassSortMode::depth_upper);
    assert(equal_keys.front().source_sequence == 4);
    assert(equal_keys.back().source_sequence == 0);
}
