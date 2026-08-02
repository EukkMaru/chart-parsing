#include "chart/reconstruction.hpp"

#include <cassert>
#include <cstdint>

int main() {
    using chart::reconstruction::forced_result_mode_has_snapshot_producer;
    using chart::reconstruction::select_reachable_forced_result_companion;
    using chart::reconstruction::select_forced_result_byte;
    using chart::reconstruction::tutorial_forced_result_mode;

    assert(tutorial_forced_result_mode(false) == 0);
    assert(tutorial_forced_result_mode(true) == 2);
    assert(forced_result_mode_has_snapshot_producer(0));
    assert(forced_result_mode_has_snapshot_producer(2));
    for (const int unreachable : {-1, 1, 3, 4, 5, 6, 7}) {
        assert(!forced_result_mode_has_snapshot_producer(unreachable));
    }
    assert(select_reachable_forced_result_companion(false) == 0);
    assert(select_reachable_forced_result_companion(true) == 1);

    std::uint32_t counter = 0;
    assert(select_forced_result_byte(false, 1, counter, 0) == 0);
    assert(counter == 0);

    assert(select_forced_result_byte(true, 1, counter, 0) == 4);
    assert(select_forced_result_byte(true, 2, counter, 0) == 3);
    assert(select_forced_result_byte(true, 3, counter, 0) == 2);
    assert(select_forced_result_byte(true, 4, counter, 0) == 1);
    assert(select_forced_result_byte(true, 0, counter, 0) == 0);
    assert(select_forced_result_byte(true, 7, counter, 0) == 0);
    assert(counter == 0);

    assert(select_forced_result_byte(true, 5, counter, 0) == 4);
    assert(select_forced_result_byte(true, 5, counter, 0) == 3);
    assert(select_forced_result_byte(true, 5, counter, 0) == 2);
    assert(select_forced_result_byte(true, 5, counter, 0) == 1);
    assert(select_forced_result_byte(true, 5, counter, 0) == 4);
    assert(counter == 5);

    assert(select_forced_result_byte(true, 6, counter, 0) == 4);
    assert(select_forced_result_byte(true, 6, counter, 1) == 3);
    assert(select_forced_result_byte(true, 6, counter, 2) == 2);
    assert(select_forced_result_byte(true, 6, counter, 3) == 1);
    assert(select_forced_result_byte(true, 6, counter, 4) == 0);
    assert(select_forced_result_byte(true, 6, counter, 9) == 0);
    assert(counter == 5);

    // The recovered tutorial producer therefore selects anonymous byte 3 when
    // the separately initialized manager enable is active. This is the only
    // reachable nonzero-mode result in the exact snapshot.
    assert(select_forced_result_byte(
               true, tutorial_forced_result_mode(true), counter, 0) == 3);
}
