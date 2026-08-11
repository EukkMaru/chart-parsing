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

    assert(near(tap_model_lateral_center(0, 1.0F), -30.0F));
    assert(near(tap_model_lateral_center(7, 2.0F), 0.0F));
    assert(near(tap_model_lateral_center(15, 1.0F), 30.0F));

    assert(tap_model_resource_row(
               1, TapModelResourceFamily::ordinary) == 81);
    assert(tap_model_resource_row(
               16, TapModelResourceFamily::ordinary) == 66);
    assert(tap_model_resource_row(
               0, TapModelResourceFamily::ordinary) == 81);
    assert(tap_model_resource_row(
               99, TapModelResourceFamily::ordinary) == 66);
    assert(tap_model_resource_row(
               1, TapModelResourceFamily::extended_primary) == 97);
    assert(tap_model_resource_row(
               16, TapModelResourceFamily::extended_primary) == 82);
    assert(tap_model_resource_row(
               1, TapModelResourceFamily::extended_alternate) == 113);
    assert(tap_model_resource_row(
               16, TapModelResourceFamily::extended_alternate) == 98);

    assert(near(tap_model_lateral_scale(4.0F, 8), 0.5F));
    assert(near(tap_model_lateral_scale(4.0F, 0), 1.0F));
    assert(near(tap_model_lateral_scale(4.0F, -1), 1.0F));
    assert(near(tap_model_depth_scale(false), 1.0F));
    assert(near(tap_model_depth_scale(true), 1.3F));
    assert(near(tap_model_parked_depth, -10000.0F));

    // Offset and DCM are both active for the normal note-model update.
    assert(near(active_note_projected_depth(
                    10.0F, 2.0F, 3.0F, 0.5F, true, true),
                -84.5F));
    // Nonpositive deltas do not select DCM.
    assert(near(active_note_projected_depth(
                    -10.0F, 2.0F, 3.0F, 0.5F, true, true),
                -44.0F));
    // Disabling base-offset mode also disables the nested DCM query.
    assert(near(active_note_projected_depth(
                    10.0F, 2.0F, 3.0F, 0.5F, false, true),
                -95.0F));
}
