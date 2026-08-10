#include "chart/reconstruction.hpp"

#include <cassert>
#include <cstdint>

using namespace chart::reconstruction;

int main() {
    PresentationPassTypeInputs input;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::opaque);

    input.material_flags = 0x40U;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::punch);
    input.material_flags = 0x20U | 0x40U;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::trans);
    input.material_flags |= 0x80U;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::two_d);
    input.material_flags |= 0x2000U;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::reduce);

    input = {};
    input.backend = PresentationSubmissionBackendKind::kind_1;
    input.material_flags = 0x40U;
    input.kind_1_secondary_force_2d = true;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::two_d);
    input.kind_1_secondary_force_2d = false;
    input.material_flags = 0x80000000U;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::two_d);
    input.material_flags |= 0x2000U;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::reduce);

    input = {};
    input.backend = PresentationSubmissionBackendKind::kind_2;
    input.kind_2_punch = true;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::punch);
    input.kind_2_trans = true;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::trans);
    input.kind_2_force_2d = true;
    assert(presentation_pass_type_from_material(input) ==
           PresentationPassType::two_d);

    assert(presentation_pass_user_from_material_flags(0x0aU << 25U) == 0x0aU);
    assert(presentation_pass_user_from_material_flags(0xffU << 24U) == 0x0fU);

    PresentationOffscreenRequests requests;
    assert(accumulate_presentation_offscreen_requests(requests, 3, 0x200U));
    assert(requests.color_pass_mask == (1U << 3U));
    assert(requests.request_color);
    assert(!requests.request_depth);

    assert(accumulate_presentation_offscreen_requests(
        requests, 5, 0x400U | 0x800U | 0x1000U));
    assert(requests.depth_pass_mask == (1U << 5U));
    assert(requests.request_color);
    assert(requests.request_depth);

    const auto pass_three = presentation_pass_offscreen_selection(requests, 3);
    assert(pass_three.color && !pass_three.depth);
    const auto pass_five = presentation_pass_offscreen_selection(requests, 5);
    assert(!pass_five.color && pass_five.depth);

    const auto submission =
        presentation_submission_offscreen_selection(0x800U | 0x1000U);
    assert(submission.color && submission.depth);
    assert(!accumulate_presentation_offscreen_requests(requests, 32, 0xffffffffU));
}
