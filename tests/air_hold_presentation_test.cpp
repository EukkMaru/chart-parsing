#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <span>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(air_hold_resource_width_index(-2) == 0);
    assert(air_hold_resource_width_index(1) == 0);
    assert(air_hold_resource_width_index(4) == 3);
    assert(air_hold_resource_width_index(20) == 15);

    assert(air_hold_presentation_update_enabled(
        AirHoldStartPhase::awaiting_result,
        AirHoldPathPhase::complete));
    assert(air_hold_presentation_update_enabled(
        AirHoldStartPhase::complete,
        AirHoldPathPhase::best_current_gap));
    assert(!air_hold_presentation_update_enabled(
        AirHoldStartPhase::complete,
        AirHoldPathPhase::complete));
    assert(air_hold_root_resource_visible(
        AirHoldStartPhase::awaiting_result));
    assert(!air_hold_root_resource_visible(AirHoldStartPhase::complete));
    assert(air_hold_checkpoint_resources_visible(false));
    assert(!air_hold_checkpoint_resources_visible(true));

    assert(air_hold_path_presentation_variant(
               AirHoldPathPhase::before_start) ==
           AirHoldPathPresentationVariant::default_state);
    assert(air_hold_path_presentation_variant(
               AirHoldPathPhase::best_current_gap) ==
           AirHoldPathPresentationVariant::best_current_gap);
    assert(air_hold_path_presentation_variant(
               AirHoldPathPhase::other_current_gap) ==
           AirHoldPathPresentationVariant::other_current_gap);
    assert(near(air_hold_primary_path_resource_scale(
                    AirHoldPathPhase::other_current_gap),
                0.5F));
    assert(near(air_hold_primary_path_resource_scale(
                    AirHoldPathPhase::best_current_gap),
                1.0F));
    assert(air_hold_emits_field_feedback(
        AirHoldPathPhase::best_current_gap));
    assert(!air_hold_emits_field_feedback(
        AirHoldPathPhase::other_current_gap));

    assert(air_hold_resource_attachment_verticals ==
           (std::array<float, 6>{
               0.0F, 15.574F, 15.574F, 0.0F, 15.724F, -15.574F}));

    const auto root = build_air_hold_root_transform(
        AirHoldStartPhase::awaiting_result,
        4.0F, 0.0F, -100.0F,
        1.0F, 2.0F, 3.0F,
        4, 2);
    assert(root.visible);
    assert(near(root.lateral, 5.0F));
    assert(near(root.vertical, 2.0F));
    assert(near(root.projected, -97.0F));
    assert(near(root.lateral_scale, 2.0F));
    assert(near(root.vertical_scale, 1.0F));
    assert(near(root.projected_scale, 1.0F));

    const auto checkpoint = build_air_hold_checkpoint_transform(
        false, 4.0F, -50.0F, 1.0F, 2.0F, 3.0F, 4, 2);
    assert(checkpoint.visible);
    assert(near(checkpoint.lateral, 5.0F));
    assert(near(checkpoint.vertical, 17.724F));
    assert(near(checkpoint.projected, -47.0F));
    assert(near(checkpoint.lateral_scale, 2.0F));

    const std::array<AirHoldPresentationCheckpoint, 3> checkpoints{
        AirHoldPresentationCheckpoint{false, 5.0F, -10.0F},
        AirHoldPresentationCheckpoint{true, 30.0F, -100.0F},
        AirHoldPresentationCheckpoint{false, 15.0F, 8.0F},
    };
    const auto envelope = build_air_hold_projection_envelope(
        10.0F, 5.0F, 20.0F, -5.0F,
        AirHoldStartPhase::complete, 20.0F,
        std::span<const AirHoldPresentationCheckpoint>{checkpoints});
    assert(envelope.includes_judgement_plane);
    assert(near(envelope.minimum, -10.0F));
    assert(near(envelope.maximum, 20.0F));

    const std::array<AirHoldPresentationCheckpoint, 1> past_checkpoint{
        AirHoldPresentationCheckpoint{false, 8.0F, 4.0F},
    };
    const auto no_future = build_air_hold_projection_envelope(
        10.0F, 5.0F, 8.0F, -5.0F,
        AirHoldStartPhase::complete, 20.0F,
        std::span<const AirHoldPresentationCheckpoint>{past_checkpoint});
    assert(!no_future.includes_judgement_plane);
    assert(near(no_future.minimum, -5.0F));
    assert(near(no_future.maximum, 5.0F));

    const auto start_pending = build_air_hold_projection_envelope(
        10.0F, 5.0F, 20.0F, -5.0F,
        AirHoldStartPhase::awaiting_result, 20.0F,
        std::span<const AirHoldPresentationCheckpoint>{});
    assert(!start_pending.includes_judgement_plane);

    const auto primary_path = build_air_hold_path_envelope_transform(
        envelope, 4.0F, 0.0F, 1.0F, 2.0F, 3.0F, 1.0F);
    assert(primary_path.visible);
    assert(near(primary_path.lateral, 5.0001F));
    assert(near(primary_path.vertical, 2.0F));
    assert(near(primary_path.projected, 23.0F));
    assert(near(primary_path.lateral_scale, 1.0F));
    assert(near(primary_path.projected_scale, 7.5F));

    const auto secondary_path = build_air_hold_path_envelope_transform(
        envelope, 4.0F, 0.0F, 1.0F, 2.0F, 3.0F,
        root.lateral_scale);
    assert(near(secondary_path.lateral_scale, 2.0F));
}
