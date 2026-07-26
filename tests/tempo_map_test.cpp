#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <vector>

namespace {

bool near(float left, float right, float tolerance = 0.001F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using chart::reconstruction::BpmScheduleRecord;
    using chart::reconstruction::ChartPosition;
    using chart::reconstruction::air_hold_sample_step;
    using chart::reconstruction::bpm_at_scheduled_position;
    using chart::reconstruction::canonicalize_c2s_position;
    using chart::reconstruction::chart_position_scalar;
    using chart::reconstruction::finalize_bpm_schedule;
    using chart::reconstruction::schedule_at_chart_position;

    const ChartPosition normalized = canonicalize_c2s_position(2, 96);
    assert(normalized.major == 2.0F);
    assert(normalized.minor == 1.0F);
    assert(chart_position_scalar(normalized) == 9.0F);
    assert(chart_position_scalar(canonicalize_c2s_position(0, 1)) ==
           4.0F / 384.0F);
    assert(chart_position_scalar(canonicalize_c2s_position(7, 3, 0)) ==
           0.0F);

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

    // The executable's default PROGJUDGE_BPM is 240. At 120 BPM the adaptive
    // loop doubles three times before reaching the four-times-reference gate.
    assert(air_hold_sample_step(
               bpm_at_scheduled_position(0.0F, bpm_records), 240.0F) == 48);
}
