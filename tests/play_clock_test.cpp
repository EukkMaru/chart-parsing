#include "chart/reconstruction.hpp"

#include <cassert>
#include <array>
#include <cmath>

namespace {

bool near(float left, float right, float tolerance = 0.00001F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using chart::reconstruction::PlayClockMode;
    using chart::reconstruction::PlayTimingOffsetRecord;
    using chart::reconstruction::ViewTimingRateState;
    using chart::reconstruction::advance_smoothed_play_position;
    using chart::reconstruction::correct_play_position_discrepancy;
    using chart::reconstruction::elapsed_milliseconds_modulo;
    using chart::reconstruction::first_catch_up_offset;
    using chart::reconstruction::integrate_play_position;
    using chart::reconstruction::invalid_music_cursor_milliseconds;
    using chart::reconstruction::make_manager_judgement_position;
    using chart::reconstruction::make_note_scheduled_position;
    using chart::reconstruction::make_raw_play_position;
    using chart::reconstruction::music_cursor_base_time_value;
    using chart::reconstruction::music_cursor_milliseconds;
    using chart::reconstruction::select_play_clock_rate;
    using chart::reconstruction::select_play_timing_offset;

    assert(near(make_note_scheduled_position(100.0F), 6.0F));
    assert(near(make_note_scheduled_position(-25.0F), -1.5F));
    assert(music_cursor_milliseconds(true, 24000U, 48000) == 500U);
    assert(music_cursor_milliseconds(true, 0U, 48000) == 0U);
    assert(music_cursor_milliseconds(false, 24000U, 48000) ==
           invalid_music_cursor_milliseconds);
    assert(music_cursor_milliseconds(true, 24000U, 0) ==
           invalid_music_cursor_milliseconds);
    assert(music_cursor_milliseconds(
               true, 0x8000000000000000ULL, 48000) ==
           invalid_music_cursor_milliseconds);
    assert(music_cursor_base_time_value(500U) == 500.0F);
    assert(near(make_raw_play_position(10.0F, 100U), 6.6F));
    assert(elapsed_milliseconds_modulo(3U, 0xfffffffeU) == 5U);

    assert(advance_smoothed_play_position(1.0F, 0.0F,
                                          PlayClockMode::ordinary) == 1.0F);
    assert(near(advance_smoothed_play_position(
                    1.2F, 0.0F, PlayClockMode::ordinary),
                1.01F));
    assert(advance_smoothed_play_position(0.9375F, 0.0F,
                                          PlayClockMode::ordinary) == 1.0F);
    assert(near(advance_smoothed_play_position(
                    3.0F, 0.0F, PlayClockMode::ordinary),
                2.01F));
    assert(near(advance_smoothed_play_position(
                    -1.0F, 0.0F, PlayClockMode::ordinary),
                -0.01F));

    // Equality with the executable's threshold does not trigger the slew.
    assert(correct_play_position_discrepancy(0.0F, 0.1F) == 0.1F);
    assert(near(correct_play_position_discrepancy(0.0F, 0.125F), 0.115F));

    assert(advance_smoothed_play_position(123.0F, 4.0F,
                                          PlayClockMode::ordinary, true) ==
           5.0F);
    assert(advance_smoothed_play_position(123.0F, 4.0F,
                                          PlayClockMode::alternate, true) ==
           4.5F);

    assert(near(advance_smoothed_play_position(
                    1.0F, 0.0F, PlayClockMode::alternate),
                0.81F));
    assert(advance_smoothed_play_position(0.5F, 0.0F,
                                          PlayClockMode::alternate) == 0.5F);
    assert(near(advance_smoothed_play_position(
                    2.0F, 0.0F, PlayClockMode::alternate),
                1.51F));
    assert(near(advance_smoothed_play_position(
                    -0.5F, 0.0F, PlayClockMode::alternate),
                -0.01F));

    assert(near(integrate_play_position(10.0F, 2.0F, 2.5F, 1.5F),
                10.75F));

    assert(select_play_clock_rate(ViewTimingRateState{}) == 1.0F);
    assert(select_play_clock_rate(ViewTimingRateState{false, 2.0F, 3.0F}) ==
           1.0F);
    assert(select_play_clock_rate(ViewTimingRateState{true, 2.0F, 3.0F}) ==
           6.0F);

    constexpr std::array timing_offsets{
        PlayTimingOffsetRecord{0.125},
        PlayTimingOffsetRecord{-0.5},
    };
    static_assert(select_play_timing_offset(timing_offsets, 0) == 0.125F);
    static_assert(select_play_timing_offset(timing_offsets, 1) == -0.5F);
    static_assert(select_play_timing_offset(timing_offsets, 2) == 0.0F);
    static_assert(select_play_timing_offset(timing_offsets, 0xffffffffU) ==
                  0.0F);

    assert(near(make_manager_judgement_position(10.0F, -0.5F, 2.0F,
                                                select_play_timing_offset(
                                                    timing_offsets, 0)),
                11.375F));

    assert(first_catch_up_offset(10, 10) == 0);
    assert(first_catch_up_offset(9, 10) == 0);
    assert(first_catch_up_offset(8, 10) == -1);
    assert(first_catch_up_offset(0, 10) == -1);
    assert(first_catch_up_offset(0xffffffffU, 0U) == 0);
    assert(first_catch_up_offset(0U, 0xffffffffU) == 0);
    // The source first wraps in 32 bits and only then interprets the result as
    // signed for its clamp. Mathematical 64-bit subtraction would differ.
    assert(first_catch_up_offset(0U, 0x80000000U) == -1);
}
