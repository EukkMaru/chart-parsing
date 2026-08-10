#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

using namespace chart::reconstruction;

namespace {

std::vector<std::uint32_t> sequences(
    const std::vector<PresentationSubmission>& submissions) {
    std::vector<std::uint32_t> result;
    for (const auto& submission : submissions) {
        result.push_back(submission.source_sequence);
    }
    return result;
}

PresentationSubmission submission(
    std::uint32_t sequence,
    std::uint16_t layer,
    float depth,
    std::uint32_t material_sort_high = 0,
    std::uint32_t material_sort_low = 0) {
    PresentationSubmission result;
    result.layer = layer;
    result.depth = depth;
    result.material_sort_high = material_sort_high;
    result.material_sort_low = material_sort_low;
    result.source_sequence = sequence;
    return result;
}

}  // namespace

int main() {
    const std::vector<PresentationPassConfiguration> configurations{
        {2,
         true,
         PresentationPassType::opaque,
         0,
         PresentationPassSortMode::depth_upper,
         PresentationPassRange::all,
         0.0F,
         0},
        {0,
         true,
         PresentationPassType::all,
         0,
         PresentationPassSortMode::layer_all_lower,
         PresentationPassRange::all,
         0.0F,
         0},
        {2,
         true,
         PresentationPassType::opaque,
         0,
         PresentationPassSortMode::depth_lower,
         PresentationPassRange::all,
         0.0F,
         0},
        {1,
         false,
         PresentationPassType::opaque,
         0,
         PresentationPassSortMode::shader_upper,
         PresentationPassRange::all,
         0.0F,
         0},
    };
    const auto plan = build_presentation_pass_plan(configurations);
    assert(plan.valid_external_configuration);
    assert(plan.entries.size() == 3);
    assert(plan.entries[0].source_index == 1);
    assert(plan.entries[1].source_index == 0);
    assert(plan.entries[2].source_index == 2);

    PresentationSubmission opaque;
    opaque.type = PresentationPassType::opaque;
    opaque.user = 0;
    opaque.layer = 7;
    const auto route = route_presentation_submission(plan, opaque, 12.5F);
    assert(route.valid_external_configuration);
    assert(route.plan_index == 0U);  // first sorted match is Type=All
    assert(route.submission.depth == 12.5F);

    assert(presentation_pass_type_matches(
        PresentationPassType::opaque_punch,
        PresentationPassType::opaque));
    assert(presentation_pass_type_matches(
        PresentationPassType::opaque_punch,
        PresentationPassType::punch));
    assert(!presentation_pass_type_matches(
        PresentationPassType::opaque_punch,
        PresentationPassType::trans));
    assert(presentation_pass_type_matches(
        PresentationPassType::three_d_all,
        PresentationPassType::trans));
    assert(!presentation_pass_type_matches(
        PresentationPassType::three_d_all,
        PresentationPassType::two_d));

    PresentationPassConfiguration ranged;
    ranged.range = PresentationPassRange::depth_front;
    ranged.range_value_f32 = 10.0F;
    assert(presentation_pass_range_matches(ranged, 9.0F, 0));
    assert(!presentation_pass_range_matches(ranged, 10.0F, 0));
    ranged.range = PresentationPassRange::depth_back;
    assert(presentation_pass_range_matches(ranged, 10.0F, 0));
    assert(!presentation_pass_range_matches(ranged, 9.0F, 0));
    ranged.range = PresentationPassRange::layer_front;
    ranged.range_value_u32 = 5;
    assert(presentation_pass_range_matches(ranged, 0.0F, 5));
    assert(!presentation_pass_range_matches(ranged, 0.0F, 4));
    ranged.range = PresentationPassRange::layer_back;
    assert(presentation_pass_range_matches(ranged, 0.0F, 4));
    assert(!presentation_pass_range_matches(ranged, 0.0F, 5));

    PresentationPassConfiguration malformed;
    malformed.pass_index = 32;
    const auto malformed_plan =
        build_presentation_pass_plan(std::span{&malformed, 1U});
    assert(!malformed_plan.valid_external_configuration);

    std::vector<PresentationSubmission> shader{
        submission(0, 0, 0.0F, 1, 9),
        submission(1, 0, 0.0F, 2, 1),
        submission(2, 0, 0.0F, 2, 3),
        submission(3, 0, 0.0F, 2, 3),
    };
    sort_presentation_submissions(
        shader, PresentationPassSortMode::shader_upper);
    assert((sequences(shader) == std::vector<std::uint32_t>{2, 3, 1, 0}));
    auto shader_lower = std::vector<PresentationSubmission>{
        submission(0, 0, 0.0F, 1, 9),
        submission(1, 0, 0.0F, 2, 1),
        submission(2, 0, 0.0F, 2, 3),
        submission(3, 0, 0.0F, 2, 3),
    };
    sort_presentation_submissions(
        shader_lower, PresentationPassSortMode::shader_lower);
    assert(sequences(shader_lower) == sequences(shader));

    const std::vector<PresentationSubmission> keyed{
        submission(0, 1, 1.0F),
        submission(1, 3, 3.0F),
        submission(2, 2, 2.0F),
        submission(3, 2, 2.0F),
    };
    auto ordered = keyed;
    sort_presentation_submissions(
        ordered, PresentationPassSortMode::depth_upper);
    assert((sequences(ordered) == std::vector<std::uint32_t>{1, 2, 3, 0}));
    ordered = keyed;
    sort_presentation_submissions(
        ordered, PresentationPassSortMode::depth_lower);
    assert((sequences(ordered) == std::vector<std::uint32_t>{0, 2, 3, 1}));
    ordered = keyed;
    sort_presentation_submissions(
        ordered, PresentationPassSortMode::layer_all_upper);
    assert((sequences(ordered) == std::vector<std::uint32_t>{1, 2, 3, 0}));
    ordered = keyed;
    sort_presentation_submissions(
        ordered, PresentationPassSortMode::layer_all_lower);
    assert((sequences(ordered) == std::vector<std::uint32_t>{0, 2, 3, 1}));

    const std::vector<PresentationSubmission> compound{
        submission(0, 1, 1.0F),
        submission(1, 0, 2.0F),
        submission(2, 1, 3.0F),
        submission(3, 1, 3.0F),
    };
    ordered = compound;
    sort_presentation_submissions(
        ordered, PresentationPassSortMode::layer_lower_depth_upper);
    assert((sequences(ordered) == std::vector<std::uint32_t>{1, 2, 3, 0}));
    ordered = compound;
    sort_presentation_submissions(
        ordered, PresentationPassSortMode::layer_lower_depth_lower);
    assert((sequences(ordered) == std::vector<std::uint32_t>{1, 0, 2, 3}));

    // The source switches from insertion leaves to adaptive merging at 33
    // records. COMISS unordered flags make a NaN stay in place at 32, while a
    // NaN at the start of the buffered right half crosses that left half at 33.
    std::vector<PresentationSubmission> thirty_two;
    for (std::uint32_t index = 0; index < 32; ++index) {
        thirty_two.push_back(submission(index, 0, 1.0F));
    }
    thirty_two[16].depth = std::numeric_limits<float>::quiet_NaN();
    sort_presentation_submissions(
        thirty_two, PresentationPassSortMode::depth_upper);
    assert(thirty_two[16].source_sequence == 16);

    std::vector<PresentationSubmission> thirty_three;
    for (std::uint32_t index = 0; index < 33; ++index) {
        thirty_three.push_back(submission(index, 0, 1.0F));
    }
    thirty_three[17].depth = std::numeric_limits<float>::quiet_NaN();
    sort_presentation_submissions(
        thirty_three, PresentationPassSortMode::depth_upper);
    assert(thirty_three.front().source_sequence == 17);
}
