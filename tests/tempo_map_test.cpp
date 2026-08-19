#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

namespace {

bool near(float left, float right, float tolerance = 0.001F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using chart::reconstruction::BpmScheduleRecord;
    using chart::reconstruction::ChartPosition;
    using chart::reconstruction::AdaptiveAirStepDisposition;
    using chart::reconstruction::ScheduledBpmSelectionDisposition;
    using chart::reconstruction::air_hold_sample_step;
    using chart::reconstruction::bpm_at_scheduled_position;
    using chart::reconstruction::canonicalize_c2s_position;
    using chart::reconstruction::chart_position_scalar;
    using chart::reconstruction::evaluate_air_hold_sample_step;
    using chart::reconstruction::evaluate_bpm_at_scheduled_position;
    using chart::reconstruction::finalize_bpm_schedule;
    using chart::reconstruction::c2s_meter_grid_steps;
    using chart::reconstruction::schedule_at_chart_position;
    using chart::reconstruction::snapshot_bpm_sort;

    const ChartPosition normalized = canonicalize_c2s_position(2, 96);
    assert(normalized.major == 2.0F);
    assert(normalized.minor == 1.0F);
    assert(chart_position_scalar(normalized) == 9.0F);
    assert(chart_position_scalar(canonicalize_c2s_position(0, 1)) ==
           4.0F / 384.0F);
    assert(chart_position_scalar(canonicalize_c2s_position(7, 3, 0)) ==
           0.0F);

    const auto three_four = c2s_meter_grid_steps(4, 3);
    assert(three_four.components_nonzero);
    assert(three_four.beat_ticks == 96);
    assert(three_four.bar_ticks == 288);
    const auto six_eight = c2s_meter_grid_steps(8, 6);
    assert(six_eight.beat_ticks == 48);
    assert(six_eight.bar_ticks == 288);
    assert(!c2s_meter_grid_steps(0, 4).components_nonzero);
    assert(!c2s_meter_grid_steps(4, 0).components_nonzero);
    assert(c2s_meter_grid_steps(0, 4).current_anchor_retained);
    assert(c2s_meter_grid_steps(4, 0).current_anchor_retained);
    assert(three_four.current_anchor_retained);
    assert(c2s_meter_grid_steps(-4, 3).beat_ticks == 0);

    // Input order is deliberately scrambled. Four scalar beats from major 0
    // to major 1 take 2000 ms at 120 BPM; the next four take 1000 ms at 240.
    std::vector<BpmScheduleRecord> bpm_records{
        {{2.0F, 0.0F}, 0.0F, 2, 60.0F},
        {{0.0F, 0.0F}, 0.0F, 0, 120.0F},
        {{1.0F, 0.0F}, 0.0F, 1, 240.0F},
    };
    finalize_bpm_schedule(bpm_records);

    assert(bpm_records[0].position.major == 0.0F);
    assert(bpm_records[1].position.major == 1.0F);
    assert(bpm_records[2].position.major == 2.0F);
    assert(near(bpm_records[0].scheduled_milliseconds, 0.0F));
    assert(near(bpm_records[1].scheduled_milliseconds, 2000.0F));
    assert(near(bpm_records[2].scheduled_milliseconds, 3000.0F));

    assert(near(schedule_at_chart_position({0.0F, 2.0F}, bpm_records),
                1000.0F));
    assert(near(schedule_at_chart_position({1.0F, 2.0F}, bpm_records),
                2500.0F));
    assert(near(schedule_at_chart_position({2.0F, 1.0F}, bpm_records),
                4000.0F));

    assert(bpm_at_scheduled_position(-1.0F, bpm_records) == 120.0F);
    assert(bpm_at_scheduled_position(1999.0F, bpm_records) == 120.0F);
    assert(bpm_at_scheduled_position(2000.0F, bpm_records) == 240.0F);
    assert(bpm_at_scheduled_position(3000.0F, bpm_records) == 60.0F);
    assert(evaluate_bpm_at_scheduled_position(
               0.0F, std::span<const BpmScheduleRecord>{})
               .disposition ==
           ScheduledBpmSelectionDisposition::source_empty_map_dereference);

    // The executable's default PROGJUDGE_BPM is 240. At 120 BPM the adaptive
    // loop doubles three times before reaching the four-times-reference gate.
    assert(air_hold_sample_step(
               bpm_at_scheduled_position(0.0F, bpm_records), 240.0F) == 48);

    // Comparator-equivalent BPM positions do not use source_sequence as a
    // tie-breaker. The snapshot insertion path preserves these two records,
    // so the reverse lookup sees the later source record at the same position.
    std::vector<BpmScheduleRecord> duplicate_position{
        {{0.0F, 0.0F}, 0.0F, 10, 120.0F},
        {{0.0F, 0.0F}, 0.0F, 11, 240.0F},
    };
    finalize_bpm_schedule(duplicate_position);
    assert(duplicate_position[0].source_sequence == 10);
    assert(duplicate_position[1].source_sequence == 11);
    assert(bpm_at_scheduled_position(
               0.0F, duplicate_position) == 240.0F);

    // The large-range three-way partition keeps one wholly equivalent region
    // intact; this also crosses the snapshot's median-of-nine threshold.
    std::vector<BpmScheduleRecord> equivalent_region;
    for (std::uint32_t index = 0; index < 42; ++index) {
        equivalent_region.push_back(
            {{3.0F, 0.0F}, 0.0F, index, 100.0F + index});
    }
    snapshot_bpm_sort(equivalent_region);
    for (std::uint32_t index = 0; index < 42; ++index) {
        assert(equivalent_region[index].source_sequence == index);
    }

    std::vector<BpmScheduleRecord> large_reverse;
    for (std::uint32_t index = 0; index < 100; ++index) {
        large_reverse.push_back({
            {static_cast<float>(99U - index), 0.0F},
            0.0F,
            index,
            120.0F,
        });
    }
    snapshot_bpm_sort(large_reverse);
    for (std::size_t index = 1; index < large_reverse.size(); ++index) {
        assert(!chart::reconstruction::bpm_record_precedes(
            large_reverse[index], large_reverse[index - 1U]));
    }

    std::vector<BpmScheduleRecord> heap_fallback;
    for (std::uint32_t index = 0; index < 64; ++index) {
        heap_fallback.push_back({
            {static_cast<float>(63U - index), 0.0F},
            0.0F,
            index,
            120.0F,
        });
    }
    chart::reconstruction::snapshot_bpm_heap_sort(
        heap_fallback, 0U, heap_fallback.size());
    for (std::size_t index = 1; index < heap_fallback.size(); ++index) {
        assert(!chart::reconstruction::bpm_record_precedes(
            heap_fallback[index], heap_fallback[index - 1U]));
    }

    const auto zero_rate = evaluate_air_hold_sample_step(0.0F, 240.0F);
    assert(zero_rate.disposition ==
           AdaptiveAirStepDisposition::source_loop_does_not_terminate);
    const auto negative_rate =
        evaluate_air_hold_sample_step(-1.0F, 240.0F);
    assert(negative_rate.disposition ==
           AdaptiveAirStepDisposition::source_loop_does_not_terminate);
    const auto nan_rate = evaluate_air_hold_sample_step(
        std::numeric_limits<float>::quiet_NaN(), 240.0F);
    assert(nan_rate.disposition == AdaptiveAirStepDisposition::produced);
    assert(nan_rate.step == 384);
    const auto infinite_rate = evaluate_air_hold_sample_step(
        std::numeric_limits<float>::infinity(), 240.0F);
    assert(infinite_rate.disposition == AdaptiveAirStepDisposition::produced);
    const auto tiny_positive =
        evaluate_air_hold_sample_step(1.0F, 240.0F);
    assert(tiny_positive.disposition ==
           AdaptiveAirStepDisposition::source_path_cursor_does_not_advance);
    assert(tiny_positive.step == 0);
    const auto infinite_reference = evaluate_air_hold_sample_step(
        240.0F, std::numeric_limits<float>::infinity());
    assert(infinite_reference.disposition ==
           AdaptiveAirStepDisposition::source_path_cursor_does_not_advance);
    assert(infinite_reference.step == 0);
    assert(infinite_rate.step == 384);
}
