#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

int main() {
    using chart::reconstruction::InputProfileSynthesisConfig;
    using chart::reconstruction::InputSynthesisSnapshot;
    using chart::reconstruction::append_input_snapshot;
    using chart::reconstruction::residual_photo_position;
    using chart::reconstruction::retained_input_snapshot_count;
    using chart::reconstruction::synthesize_input_snapshot;

    assert(residual_photo_position(0, 65) == 65);
    assert(residual_photo_position(0, 139) == 65);
    assert(residual_photo_position(0, 140) == 215);
    assert(residual_photo_position(0x01, 65) == 75);
    assert(residual_photo_position(0x03, 65) == 95);
    assert(residual_photo_position(0x20, 65) == 205);
    assert(residual_photo_position(0x30, 65) == 185);
    assert(residual_photo_position(0x06, 65) == 120);

    InputProfileSynthesisConfig config{};
    config.conditioning_history_count = 3;
    config.conditioning_required_count = 2;
    config.motion_history_counts.fill(1);
    config.absolute_motion_thresholds.fill(1000);
    config.positive_motion_thresholds.fill(1000);
    config.negative_motion_thresholds.fill(1000);
    config.profile_7_position_lower = 75;
    config.profile_7_position_upper = 205;

    std::array<std::uint8_t, 32> touch{};
    touch[3] = 1;
    touch[16 + 3] = 1;

    std::vector<InputSynthesisSnapshot> history(2);
    history[0].conditioned_photo_mask = 0x01;
    history[1].conditioned_photo_mask = 0x01;
    auto snapshot =
        synthesize_input_snapshot(config, touch, 0x01, history);
    assert(snapshot.raw_level == ((1U << 3U) | (1U << 19U)));
    assert(snapshot.logical_level == (1U << 3U));
    assert(snapshot.logical_rising == (1U << 3U));
    assert(snapshot.conditioned_photo_mask == 0x01);
    assert(snapshot.residual_photo_mask == 0);
    assert(snapshot.photo_position == 65);
    assert(snapshot.profile_levels[7] == 0);

    // Profile 7 ignores its stored motion-threshold record.
    config.absolute_motion_thresholds[7] = 0;
    snapshot = synthesize_input_snapshot(config, touch, 0x01, history);
    assert(snapshot.profile_levels[7] == 0);
    config.absolute_motion_thresholds[7] = 1000;

    // The preceding AIR marker blocks only a newly qualified conditioned bit.
    history[1].conditioned_photo_mask = 0;
    history[1].air_sample_marker = true;
    snapshot = synthesize_input_snapshot(config, touch, 0x01, history);
    assert(snapshot.conditioned_photo_mask == 0);
    assert(snapshot.residual_photo_mask == 0x01);
    assert(snapshot.photo_position == 75);
    assert(snapshot.photo_position_delta == 10);
    assert(snapshot.profile_levels[7] == 1);

    // A jump beyond the executable's inclusive +/-50 range stores zero motion.
    history.back().photo_position = 215;
    snapshot = synthesize_input_snapshot(config, touch, 0x01, history);
    assert(snapshot.photo_position == 75);
    assert(snapshot.photo_position_delta == 0);

    // Profile windows include the current delta and then newest older deltas.
    config.motion_history_counts[2] = 3;
    config.absolute_motion_thresholds[2] = 1000;
    config.positive_motion_thresholds[2] = 12;
    config.negative_motion_thresholds[2] = 1000;
    history[0].photo_position_delta = 50;
    history[1].photo_position = 65;
    history[1].photo_position_delta = -7;
    snapshot = synthesize_input_snapshot(config, touch, 0x01, history);
    // Current +10, newest -7, then older +50: positive motion is 60.
    assert(snapshot.profile_levels[2] == 1);

    config.motion_history_counts[2] = 2;
    config.positive_motion_thresholds[2] = 12;
    snapshot = synthesize_input_snapshot(config, touch, 0x01, history);
    // Current +10 and newest -7 do not reach the positive threshold.
    assert(snapshot.profile_levels[2] == 0);

    std::vector<InputSynthesisSnapshot> bounded(
        retained_input_snapshot_count);
    append_input_snapshot(bounded, {});
    assert(bounded.size() == retained_input_snapshot_count);
}
