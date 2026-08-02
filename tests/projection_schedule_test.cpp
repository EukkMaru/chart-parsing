#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

bool near(float left, float right, float tolerance = 0.01F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using chart::reconstruction::BpmScheduleRecord;
    using chart::reconstruction::C2sProjectionCommandDisposition;
    using chart::reconstruction::C2sProjectionSchedule;
    using chart::reconstruction::ProjectionScheduleInterval;
    using chart::reconstruction::adjust_projection_target_milliseconds;
    using chart::reconstruction::apply_c2s_projection_command;
    using chart::reconstruction::finalize_bpm_schedule;
    using chart::reconstruction::finalize_c2s_projection_schedule;
    using chart::reconstruction::projection_factor_at_milliseconds;
    using chart::reconstruction::runtime_materialization_probe_from_schedule;
    using chart::reconstruction::runtime_materialization_probe_is_eligible;

    std::vector<BpmScheduleRecord> bpm_records{
        {{0.0F, 0.0F}, 0.0F, 0, 60.0F},
    };
    finalize_bpm_schedule(bpm_records);

    C2sProjectionSchedule schedule;
    const std::string_view sfl[]{"0", "96", "96", "2"};
    const std::string_view stp[]{"0", "0", "96"};
    const std::string_view slp[]{"0", "0", "192", "0.5", "7"};
    const std::string_view dcm[]{"0", "0", "192", "3"};
    const std::string_view clk[]{"0", "48"};

    assert(apply_c2s_projection_command(
               schedule, "SFL", sfl, bpm_records, 2) ==
           C2sProjectionCommandDisposition::keyed_interval);
    assert(apply_c2s_projection_command(
               schedule, "STP", stp, bpm_records, 1) ==
           C2sProjectionCommandDisposition::keyed_interval);
    assert(apply_c2s_projection_command(
               schedule, "SLP", slp, bpm_records, 3) ==
           C2sProjectionCommandDisposition::keyed_interval);
    assert(apply_c2s_projection_command(
               schedule, "DCM", dcm, bpm_records, 4) ==
           C2sProjectionCommandDisposition::factor_interval);
    assert(apply_c2s_projection_command(
               schedule, "CLK", clk, bpm_records, 5) ==
           C2sProjectionCommandDisposition::presentation_click);
    assert(apply_c2s_projection_command(
               schedule, "UNKNOWN", {}, bpm_records, 6) ==
           C2sProjectionCommandDisposition::unrecognized);

    C2sProjectionSchedule wrapped_schedule;
    const std::string_view wrapped_duration[]{
        "0", "2147483647", "1", "2"};
    assert(apply_c2s_projection_command(
               wrapped_schedule,
               "SFL",
               wrapped_duration,
               bpm_records,
               0) ==
           C2sProjectionCommandDisposition::keyed_interval);
    const auto expected_wrapped_end =
        chart::reconstruction::canonicalize_c2s_position(
            0, std::numeric_limits<std::int32_t>::min());
    assert(wrapped_schedule.keyed_intervals.at(0).front().end.major ==
           expected_wrapped_end.major);
    assert(wrapped_schedule.keyed_intervals.at(0).front().end.minor ==
           expected_wrapped_end.minor);

    // SFE is registered but has no group-1 handler case. Its fields are not
    // accessed, so malformed text cannot turn the rejection into an error.
    const std::string_view malformed_sfe[]{"bad"};
    assert(apply_c2s_projection_command(
               schedule, "SFE", malformed_sfe, bpm_records, 7) ==
           C2sProjectionCommandDisposition::recognized_but_rejected);

    finalize_c2s_projection_schedule(schedule);
    assert(schedule.keyed_intervals.at(0).size() == 2);
    assert(schedule.keyed_intervals.at(0)[0].factor == 0.0F);
    assert(schedule.keyed_intervals.at(0)[1].factor == 2.0F);
    assert(schedule.keyed_intervals.at(7).size() == 1);
    assert(schedule.factor_intervals.size() == 1);
    assert(schedule.presentation_clicks.size() == 1);
    assert(near(schedule.presentation_clicks.front().scheduled_milliseconds,
                500.0F));

    // Keyed vectors use the same compiled MSVC three-way introsort as BPM
    // records, not host std::sort. This remains deterministic for equivalent
    // starts and NaN, where the comparator is unordered.
    C2sProjectionSchedule nonfinite_schedule;
    std::vector<BpmScheduleRecord> homologous_bpm;
    auto& nonfinite = nonfinite_schedule.keyed_intervals[23];
    for (std::uint32_t sequence = 0; sequence < 48; ++sequence) {
        const float major =
            sequence == 5
                ? std::numeric_limits<float>::quiet_NaN()
                : static_cast<float>(47U - sequence);
        nonfinite.push_back({
            {major, 0.0F},
            0.0F,
            sequence,
            static_cast<float>(sequence),
            {major, 1.0F},
            1.0F,
        });
        homologous_bpm.push_back({
            {major, 0.0F},
            0.0F,
            sequence,
            static_cast<float>(sequence),
        });
    }
    chart::reconstruction::snapshot_bpm_sort(homologous_bpm);
    finalize_c2s_projection_schedule(nonfinite_schedule);
    for (std::size_t index = 0; index < nonfinite.size(); ++index) {
        assert(nonfinite[index].source_sequence ==
               homologous_bpm[index].source_sequence);
    }

    // Every overlapping keyed interval contributes. STP removes the first
    // second, then SFL doubles the following half second.
    assert(near(adjust_projection_target_milliseconds(
                    schedule, 0, 0.0F, 1500.0F),
                1000.0F));
    assert(near(adjust_projection_target_milliseconds(
                    schedule, 7, 0.0F, 1500.0F),
                750.0F));
    assert(near(adjust_projection_target_milliseconds(
                    schedule, 99, 0.0F, 1500.0F),
                1500.0F));
    assert(near(adjust_projection_target_milliseconds(
                    schedule, 0, 2000.0F, 1000.0F),
                1000.0F));

    assert(projection_factor_at_milliseconds(
               -1.0F, schedule.factor_intervals) == 3.0F);
    assert(projection_factor_at_milliseconds(
               1998.999F, schedule.factor_intervals) == 3.0F);
    assert(projection_factor_at_milliseconds(
               1999.0F, schedule.factor_intervals) == 1.0F);

    // Zero factors are skipped, and source order is normative: a future start
    // stops the scan before any later interval can be considered.
    const ProjectionScheduleInterval zero{
        {}, 0.0F, 0, 0.0F, {}, 100.0F};
    const ProjectionScheduleInterval nonzero{
        {}, 0.0F, 1, 4.0F, {}, 100.0F};
    const ProjectionScheduleInterval future{
        {}, 100.0F, 2, 8.0F, {}, 200.0F};
    const ProjectionScheduleInterval later_cover{
        {}, 0.0F, 3, 9.0F, {}, 200.0F};
    const ProjectionScheduleInterval zero_then_nonzero[]{zero, nonzero};
    assert(projection_factor_at_milliseconds(
               10.0F, zero_then_nonzero) == 4.0F);
    const ProjectionScheduleInterval future_then_cover[]{
        future, later_cover};
    assert(projection_factor_at_milliseconds(
               10.0F, future_then_cover) == 1.0F);

    // A nonidentity SLP selected by an SLA tag can move a far note inside the
    // materialization projection. The same note with a missing key stays out.
    C2sProjectionSchedule tagged_schedule;
    const std::string_view full_stop[]{"0", "0", "960", "0", "7"};
    assert(apply_c2s_projection_command(
               tagged_schedule, "SLP", full_stop, bpm_records, 0) ==
           C2sProjectionCommandDisposition::keyed_interval);
    finalize_c2s_projection_schedule(tagged_schedule);

    const auto untagged = runtime_materialization_probe_from_schedule(
        10000.0F, 0, 0.0F, tagged_schedule);
    const auto tagged = runtime_materialization_probe_from_schedule(
        10000.0F, 7, 0.0F, tagged_schedule);
    assert(!runtime_materialization_probe_is_eligible(
        untagged, 1.0F, 0.0F));
    assert(runtime_materialization_probe_is_eligible(
        tagged, 1.0F, 0.0F));

    // The raw <30 shortcut still wins even if a schedule would otherwise
    // produce a far-path value.
    const auto near_probe = runtime_materialization_probe_from_schedule(
        100.0F, 99, 0.0F, tagged_schedule);
    assert(near_probe.raw_delta < 30.0F);
    assert(runtime_materialization_probe_is_eligible(
        near_probe, 1000.0F, 1000.0F));

    bool malformed_threw = false;
    try {
        const std::string_view malformed[]{"bad"};
        (void)apply_c2s_projection_command(
            schedule, "STP", malformed, bpm_records, 8);
    } catch (const std::invalid_argument&) {
        malformed_threw = true;
    }
    assert(malformed_threw);

    schedule.reset();
    assert(schedule.keyed_intervals.empty());
    assert(schedule.factor_intervals.empty());
    assert(schedule.presentation_clicks.empty());
}
