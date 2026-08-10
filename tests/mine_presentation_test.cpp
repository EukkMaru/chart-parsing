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

    assert(mine_model_resource_row(1) == 145);
    assert(mine_model_resource_row(16) == 130);
    assert(mine_model_resource_row(0) == 145);
    assert(mine_model_resource_row(99) == 130);

    assert(mine_root_model_update_enabled(MinePhase::accumulating));
    assert(!mine_root_model_update_enabled(MinePhase::threshold_reached));
    assert(!mine_root_model_update_enabled(
        MinePhase::threshold_unreachable));

    // A normal success with held lanes follows their average lane index.
    assert(near(mine_success_effect_lane_coordinate(
                    4, 4, 15, 3, false),
                5.0F));
    assert(near(mine_success_effect_lateral(4, 4, 15, 3, false),
                -12.0F));
    // No held lane and forced mode both retain the chart-span center.
    assert(near(mine_success_effect_lane_coordinate(
                    4, 4, 0, 0, false),
                6.0F));
    assert(near(mine_success_effect_lane_coordinate(
                    4, 4, 15, 3, true),
                6.0F));
    assert(near(mine_success_effect_lateral(4, 4, 15, 3, true),
                -8.0F));

    assert(mine_success_effect_width_selector(-1) == 0);
    assert(mine_success_effect_width_selector(0) == 0);
    assert(mine_success_effect_width_selector(1) == 0);
    assert(mine_success_effect_width_selector(2) == 1);
    assert(mine_success_effect_width_selector(16) == 15);
    assert(mine_success_effect_width_selector(99) == 15);
    assert(mine_emits_success_effect(0));
    assert(!mine_emits_success_effect(1));
    assert(!mine_emits_success_effect(4));
    assert(near(mine_success_effect_depth, -65.0F));
    assert(mine_primary_effect_kind == 8);
    assert(mine_secondary_effect_kind == 9);
    assert(mine_primary_effect_lifetime == 100);
}
