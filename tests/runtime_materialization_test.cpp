#include <chart/reconstruction.hpp>

#include <cassert>

int main() {
    using chart::reconstruction::RuntimeMaterializationProbe;
    using chart::reconstruction::runtime_factory_constructs_primary;
    using chart::reconstruction::runtime_materialization_probe_is_eligible;
    using chart::reconstruction::runtime_materialization_projected_position;
    using chart::reconstruction::runtime_materialization_scan_is_bypassed;
    using chart::reconstruction::should_materialize_runtime_record;

    static_assert(!runtime_materialization_scan_is_bypassed);

    // The raw shortcut is strict and precedes the adjusted projection path.
    const RuntimeMaterializationProbe near{29.999F, 1000.0F, 1000.0F};
    assert(runtime_materialization_probe_is_eligible(near, 1000.0F, 0.0F));

    const RuntimeMaterializationProbe at_threshold{30.0F, 30.0F, 1.0F};
    assert(!runtime_materialization_probe_is_eligible(
        at_threshold, 20.0F, 0.0F));

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
