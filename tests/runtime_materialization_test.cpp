#include <chart/reconstruction.hpp>

#include <cassert>
#include <cmath>
#include <limits>

int main() {
    using chart::reconstruction::MaterializationSkillBeforeUnit;
    using chart::reconstruction::PlayOptionSpeedTableRecord;
    using chart::reconstruction::RuntimeMaterializationProbe;
    using chart::reconstruction::resolve_materialization_projection_base_offset;
    using chart::reconstruction::resolve_materialization_runtime_speed;
    using chart::reconstruction::runtime_factory_constructs_primary;
    using chart::reconstruction::runtime_materialization_probe_is_eligible;
    using chart::reconstruction::runtime_materialization_projected_position;
    using chart::reconstruction::runtime_materialization_scan_is_bypassed;
    using chart::reconstruction::select_materialization_speed_id;
    using chart::reconstruction::should_materialize_runtime_record;

    static_assert(!runtime_materialization_scan_is_bypassed);

    const PlayOptionSpeedTableRecord speed_table[]{
        {0.5},
        {1.25},
        {2.0},
    };
    const MaterializationSkillBeforeUnit no_speed_override[]{
        {4, 2},
        {5, 1},
    };
    assert(select_materialization_speed_id(
               1, no_speed_override, std::size(speed_table)) == 1);
    assert(resolve_materialization_runtime_speed(
               1, no_speed_override, speed_table) == 1.25F);

    // The first type-6 unit is decisive. A valid ID replaces the current one.
    const MaterializationSkillBeforeUnit valid_speed_override[]{
        {4, 0},
        {6, 2},
        {6, 0},
    };
    assert(select_materialization_speed_id(
               1, valid_speed_override, std::size(speed_table)) == 2);
    assert(resolve_materialization_runtime_speed(
               1, valid_speed_override, speed_table) == 2.0F);

    // An invalid first type-6 ID leaves the current ID in place and blocks a
    // later valid unit.
    const MaterializationSkillBeforeUnit invalid_first_override[]{
        {6, 99},
        {6, 2},
    };
    assert(select_materialization_speed_id(
               1, invalid_first_override, std::size(speed_table)) == 1);
    assert(resolve_materialization_runtime_speed(
               1, invalid_first_override, speed_table) == 1.25F);

    // Invalid IDs and nonpositive/NaN table values resolve through the exact
    // float conversion and 0.1F setup clamp.
    assert(resolve_materialization_runtime_speed(
               99, no_speed_override, speed_table) == 0.1F);
    const PlayOptionSpeedTableRecord clamp_table[]{
        {-1.0},
        {std::numeric_limits<double>::quiet_NaN()},
        {std::numeric_limits<double>::infinity()},
    };
    assert(resolve_materialization_runtime_speed(0, {}, clamp_table) == 0.1F);
    assert(resolve_materialization_runtime_speed(1, {}, clamp_table) == 0.1F);
    assert(std::isinf(
        resolve_materialization_runtime_speed(2, {}, clamp_table)));

    assert(resolve_materialization_projection_base_offset(false, 99.0F) ==
           0.0F);
    assert(resolve_materialization_projection_base_offset(true, -12.5F) ==
           -12.5F);

    // The raw shortcut is strict and precedes the adjusted projection path.
    const RuntimeMaterializationProbe near{29.999F, 1000.0F, 1000.0F};
    assert(runtime_materialization_probe_is_eligible(near, 1000.0F, 0.0F));

    const RuntimeMaterializationProbe at_threshold{30.0F, 30.0F, 1.0F};
    assert(!runtime_materialization_probe_is_eligible(
        at_threshold, 20.0F, 0.0F));

    // The executable branches only when 30 <= raw and rejects only when one
    // of two ordered projected-bound comparisons succeeds. Unordered NaN
    // therefore passes both the shortcut guard and the far-path bounds.
    const float nan = std::numeric_limits<float>::quiet_NaN();
    assert(runtime_materialization_probe_is_eligible(
        {nan, 1000.0F, 1.0F}, 1000.0F, 1000.0F));
    assert(runtime_materialization_probe_is_eligible(
        {30.0F, nan, 1.0F}, 1.0F, 0.0F));
    assert(runtime_materialization_probe_is_eligible(
        {30.0F, 1000.0F, nan}, 1.0F, 0.0F));
    assert(runtime_materialization_probe_is_eligible(
        {30.0F, 1000.0F, 1.0F}, 1.0F, nan));

    // Both projected bounds are inclusive.
    const RuntimeMaterializationProbe lower_bound{30.0F, 30.0F, 1.0F};
    assert(runtime_materialization_projected_position(
               lower_bound, 1.0F, 455.0F, 1.0F) == -550.0F);
    assert(runtime_materialization_probe_is_eligible(
        lower_bound, 1.0F, 455.0F, 1.0F));

    const RuntimeMaterializationProbe upper_bound{30.0F, -615.0F, 99.0F};
    assert(runtime_materialization_projected_position(
               upper_bound, 1.0F, 0.0F, 1.0F) == 550.0F);
    assert(runtime_materialization_probe_is_eligible(
        upper_bound, 1.0F, 0.0F, 1.0F));

    // A failed start probe can be rescued only by a supported endpoint.
    const RuntimeMaterializationProbe outside{30.0F, 1000.0F, 1.0F};
    assert(!should_materialize_runtime_record(
        outside, false, near, 1.0F, 0.0F));
    assert(should_materialize_runtime_record(
        outside, true, near, 1.0F, 0.0F));

    // Positive adjusted deltas use the supplied projection factor; negative
    // deltas ignore it and use 1.0.
    const RuntimeMaterializationProbe positive{30.0F, 10.0F, 2.0F};
    assert(runtime_materialization_projected_position(
               positive, 1.0F, 0.0F, 1.0F) == -85.0F);
    const RuntimeMaterializationProbe negative{30.0F, -10.0F, 99.0F};
    assert(runtime_materialization_projected_position(
               negative, 1.0F, 0.0F, 1.0F) == -55.0F);

    for (const int type : {0, 1, 2, 4, 6, 9, 10, 11, 13}) {
        assert(runtime_factory_constructs_primary(type));
    }
    for (const int type : {-1, 3, 5, 7, 8, 12, 14}) {
        assert(!runtime_factory_constructs_primary(type));
    }
}
