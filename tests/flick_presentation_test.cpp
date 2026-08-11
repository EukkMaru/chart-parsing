#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(flick_model_resource_row(1) == 129);
    assert(flick_model_resource_row(16) == 114);
    assert(flick_model_resource_row(0) == 129);
    assert(flick_model_resource_row(99) == 114);

    assert(!flick_phase_is_terminal(0));
    assert(!flick_phase_is_terminal(1));
    assert(flick_phase_is_terminal(2));
    assert(flick_phase_is_terminal(3));
    assert(flick_phase_is_terminal(4));
    assert(flick_phase_is_terminal(5));
    assert(flick_root_model_visible(0));
    assert(flick_root_model_visible(1));
    assert(!flick_root_model_visible(2));
    assert(!flick_root_model_visible(5));

    // Flick uses the same exact root transform helpers as the Tap family.
    assert(near(tap_model_lateral_center(3, 4.0F), -12.0F));
    assert(near(tap_model_lateral_scale(4.0F, 8), 0.5F));
    assert(near(tap_model_depth_scale(false), 1.0F));
    assert(near(tap_model_depth_scale(true), 1.3F));
    assert(near(tap_model_parked_depth, -10000.0F));
    assert(near(active_note_projected_depth(
                    10.0F, 2.0F, 3.0F, 0.5F, true, true),
                -84.5F));
}
