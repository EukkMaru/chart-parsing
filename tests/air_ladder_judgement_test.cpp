#include "chart/reconstruction.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <span>
#include <vector>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(select_ald_runtime(0, ald_non_style_code) ==
           AldRuntimeKind::heaven_hold);
    assert(select_ald_runtime(1, ald_non_style_code) ==
           AldRuntimeKind::air_ladder);
    assert(select_ald_runtime(0, 0) == AldRuntimeKind::air_ladder);

    assert(air_ladder_input_profile == 7);
    assert(!air_ladder_exposes_candidate());
    assert(air_ladder_source_category == 18);
    assert(map_shared_result_category(air_ladder_source_category) == 7);

    const auto point = [](std::int32_t tick,
                          float lane,
                          float vertical,
                          float width) {
        return AirLadderPathPoint{
            air_ladder_position_from_grid_tick(tick),
            0.0F,
            lane,
            vertical,
            width,
        };
    };
    const auto schedule = [](const ChartPosition& position) {
        return chart_position_scalar(position) * 10.0F;
    };

    const AirLadderPathPoint root = point(0, 0.0F, 1.0F, 2.0F);
    const std::vector<AirLadderPathPoint> controls{
        point(100, 10.0F, 3.0F, 4.0F),
        point(200, 0.0F, 5.0F, 6.0F),
    };
    const auto exact = generate_air_ladder_checkpoints(
        root, std::span<const AirLadderPathPoint>{controls}, 50, schedule);
    assert(exact.disposition == AirLadderGenerationDisposition::generated);
    assert(exact.checkpoints.size() == 5);
    const std::array<std::int32_t, 5> exact_ticks{0, 50, 100, 150, 200};
    const std::array<float, 5> exact_lanes{0.0F, 5.0F, 10.0F, 5.0F, 0.0F};
    const std::array<float, 5> exact_verticals{1.0F, 2.0F, 3.0F, 4.0F,
                                               5.0F};
    const std::array<float, 5> exact_widths{2.0F, 3.0F, 4.0F, 5.0F, 6.0F};
    for (std::size_t index = 0; index < exact.checkpoints.size(); ++index) {
        const auto& checkpoint = exact.checkpoints[index];
        assert(checkpoint.enabled);
        assert(air_ladder_grid_tick(checkpoint.point) == exact_ticks[index]);
        assert(near(checkpoint.point.lane, exact_lanes[index]));
        assert(near(checkpoint.point.vertical, exact_verticals[index]));
        assert(near(checkpoint.point.decoded_width, exact_widths[index]));
        assert(near(checkpoint.point.scheduled,
                    schedule(checkpoint.point.position)));
    }

    const auto overshoot = generate_air_ladder_checkpoints(
        root, std::span<const AirLadderPathPoint>{controls}, 60, schedule);
    assert(overshoot.disposition ==
           AirLadderGenerationDisposition::generated);
    assert(overshoot.checkpoints.size() == 4);
    assert(air_ladder_grid_tick(overshoot.checkpoints.back().point) == 180);

    // A sample exactly at a duplicated control position takes the last
    // duplicate's properties as the next nonzero segment start.
    const std::vector<AirLadderPathPoint> duplicate_controls{
        point(100, 8.0F, 2.0F, 3.0F),
        point(100, 12.0F, 4.0F, 5.0F),
        point(200, 2.0F, 6.0F, 7.0F),
    };
    const auto duplicate = generate_air_ladder_checkpoints(
        root, std::span<const AirLadderPathPoint>{duplicate_controls}, 50,
        schedule);
    assert(duplicate.checkpoints.size() == 5);
    assert(near(duplicate.checkpoints[2].point.lane, 12.0F));
    assert(near(duplicate.checkpoints[2].point.vertical, 4.0F));
    assert(near(duplicate.checkpoints[2].point.decoded_width, 5.0F));

    const auto no_controls = generate_air_ladder_checkpoints(
        root, std::span<const AirLadderPathPoint>{}, 50, schedule);
    assert(no_controls.disposition ==
           AirLadderGenerationDisposition::no_authored_controls);
    const auto zero_interval = generate_air_ladder_checkpoints(
        root, std::span<const AirLadderPathPoint>{controls}, 0, schedule);
    assert(zero_interval.disposition ==
           AirLadderGenerationDisposition::nonpositive_interval);
    const std::array<AirLadderPathPoint, 1> backward{point(
        -1, 1.0F, 1.0F, 1.0F)};
    const auto no_forward = generate_air_ladder_checkpoints(
        root, std::span<const AirLadderPathPoint>{backward}, 1, schedule);
    assert(no_forward.disposition ==
           AirLadderGenerationDisposition::no_forward_span);

    const AirLadderPathPoint wrap_root = point(
        100, 0.0F, 0.0F, 1.0F);
    const std::array<AirLadderPathPoint, 1> wrapped_end{
        point(200, 1.0F, 1.0F, 2.0F)};
    const auto wrapped = generate_air_ladder_checkpoints(
        wrap_root, std::span<const AirLadderPathPoint>{wrapped_end},
        std::numeric_limits<std::int32_t>::max(), schedule);
    assert(wrapped.disposition ==
           AirLadderGenerationDisposition::source_cursor_wrap_expansion);
    assert(wrapped.checkpoints.size() == 1);

    const std::array<std::int32_t, 4> before{0, 1, 2, 0};
    const std::array<std::int32_t, 4> after{2, 3, 3, 1};
    assert(air_ladder_new_resolution_count(
               std::span<const std::int32_t>{before},
               std::span<const std::int32_t>{after}) == 2);

    assert(!air_ladder_is_terminal(9.9F, 10.0F, 2, 2));
    assert(!air_ladder_is_terminal(10.0F, 10.0F, 1, 2));
    assert(air_ladder_is_terminal(10.0F, 10.0F, 2, 2));
    assert(air_ladder_is_terminal(11.0F, 10.0F, 2, 2));

    // The binary does not implement the schedule guard as a simplified >=.
    // An unordered comparison bypasses that guard but never the checker-count
    // requirement.
    const float nan = std::numeric_limits<float>::quiet_NaN();
    assert(air_ladder_is_terminal(nan, 10.0F, 2, 2));
    assert(air_ladder_is_terminal(10.0F, nan, 2, 2));
    assert(!air_ladder_is_terminal(nan, 10.0F, 1, 2));
}
