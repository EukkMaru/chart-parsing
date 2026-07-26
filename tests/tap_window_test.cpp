#include "chart/reconstruction.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

using chart::reconstruction::TapInterval;
using chart::reconstruction::TapAdjacentWindowAdjustment;
using chart::reconstruction::TapLaneWindows;
using chart::reconstruction::apply_active_result_threshold;
using chart::reconstruction::apply_tap_adjacent_window_adjustment;
using chart::reconstruction::apply_tap_center_window_adjustment;
using chart::reconstruction::classify_tap_delta;
using chart::reconstruction::tap_center_endpoint_epsilon;
using chart::reconstruction::tap_detailed_result_code;
using chart::reconstruction::tap_fine_results;

namespace {

constexpr TapLaneWindows nested_windows{
    true,
    {{{-50.0F, 50.0F},
      {-40.0F, 40.0F},
      {-30.0F, 30.0F},
      {-20.0F, 20.0F},
      {-10.0F, 10.0F}}},
    0.0F,
};

}  // namespace

int main() {
    // Disabled entries never classify, even inside their center interval.
    auto disabled = nested_windows;
    disabled.enabled = false;
    assert(classify_tap_delta(disabled, 0.0F) == 0);

    // The search is center-first. Each interval is lower-inclusive and
    // upper-exclusive; exact upper boundaries therefore fall into the next
    // containing band.
    assert(classify_tap_delta(nested_windows, -10.0F) == 6);
    assert(classify_tap_delta(nested_windows, 0.0F) == 6);
    assert(classify_tap_delta(nested_windows, 9.999F) == 6);
    assert(classify_tap_delta(nested_windows, 10.0F) == 7);
    assert(classify_tap_delta(nested_windows, -20.0F) == 5);
    assert(classify_tap_delta(nested_windows, 20.0F) == 8);
    assert(classify_tap_delta(nested_windows, -30.0F) == 4);
    assert(classify_tap_delta(nested_windows, 30.0F) == 9);
    assert(classify_tap_delta(nested_windows, -40.0F) == 3);
    assert(classify_tap_delta(nested_windows, 40.0F) == 10);
    assert(classify_tap_delta(nested_windows, -50.0F) == 2);
    assert(classify_tap_delta(nested_windows, 50.0F) == 11);

    // Non-nested intervals demonstrate center-first precedence independently
    // of the configuration's normal containment invariant.
    TapLaneWindows overlapping{
        true,
        {{{-100.0F, 100.0F},
          {-90.0F, 90.0F},
          {-80.0F, 80.0F},
          {-70.0F, 70.0F},
          {-75.0F, 75.0F}}},
        0.0F,
    };
    assert(classify_tap_delta(overlapping, -72.0F) == 6);
    assert(classify_tap_delta(overlapping, 72.0F) == 6);

    // The same-substep TAP mutation widens center endpoints independently
    // when the corresponding inner and middle endpoints are strictly within
    // the executable-owned epsilon. The wider outermost candidate bounds are
    // unaffected.
    TapLaneWindows center_adjusted{
        true,
        {{{-5.0F, 5.0F},
          {-4.0F, 4.0F},
          {-2.0F, 2.0F},
          {-2.000005F, 2.000005F},
          {-0.5F, 0.5F}}},
        0.0F,
    };
    assert(classify_tap_delta(center_adjusted, -1.0F) == 5);
    assert(classify_tap_delta(center_adjusted, 1.0F) == 7);
    apply_tap_center_window_adjustment(center_adjusted);
    assert(center_adjusted.bands_outer_to_center[4].lower ==
           center_adjusted.bands_outer_to_center[3].lower);
    assert(center_adjusted.bands_outer_to_center[4].upper ==
           center_adjusted.bands_outer_to_center[3].upper);
    assert(center_adjusted.bands_outer_to_center[0].lower == -5.0F);
    assert(center_adjusted.bands_outer_to_center[0].upper == 5.0F);
    assert(classify_tap_delta(center_adjusted, -1.0F) == 6);
    assert(classify_tap_delta(center_adjusted, 1.0F) == 6);

    auto one_sided_center = center_adjusted;
    one_sided_center.bands_outer_to_center[2] = {-2.0F, 2.0F};
    one_sided_center.bands_outer_to_center[3] = {-2.5F, 2.000005F};
    one_sided_center.bands_outer_to_center[4] = {-0.5F, 0.5F};
    apply_tap_center_window_adjustment(one_sided_center);
    assert(one_sided_center.bands_outer_to_center[4].lower == -0.5F);
    assert(one_sided_center.bands_outer_to_center[4].upper == 2.000005F);

    // Equality with epsilon is excluded by the source's strict comparison.
    TapLaneWindows exact_epsilon = nested_windows;
    exact_epsilon.bands_outer_to_center[2] = {0.0F, 0.0F};
    exact_epsilon.bands_outer_to_center[3] = {
        -tap_center_endpoint_epsilon, tap_center_endpoint_epsilon};
    exact_epsilon.bands_outer_to_center[4] = {-0.25F, 0.25F};
    apply_tap_center_window_adjustment(exact_epsilon);
    assert(exact_epsilon.bands_outer_to_center[4].lower == -0.25F);
    assert(exact_epsilon.bands_outer_to_center[4].upper == 0.25F);

    auto disabled_center = center_adjusted;
    disabled_center.enabled = false;
    disabled_center.bands_outer_to_center[4] = {-0.5F, 0.5F};
    apply_tap_center_window_adjustment(disabled_center);
    assert(disabled_center.bands_outer_to_center[4].lower == -0.5F);
    assert(disabled_center.bands_outer_to_center[4].upper == 0.5F);

    // A preceding nearby record trims lower endpoints only. A following one
    // trims upper endpoints only. The center band remains narrower when its
    // existing endpoint is already inside the selected limit.
    auto adjusted = nested_windows;
    apply_tap_adjacent_window_adjustment(
        adjusted, {true, 15.0F, true, 15.0F, 0.0F, -5.0F, 5.0F});
    for (std::size_t i = 0; i < 4; ++i) {
        assert(adjusted.bands_outer_to_center[i].lower == -15.0F);
        assert(adjusted.bands_outer_to_center[i].upper == 15.0F);
    }
    assert(adjusted.bands_outer_to_center[4].lower == -10.0F);
    assert(adjusted.bands_outer_to_center[4].upper == 10.0F);
    assert(classify_tap_delta(adjusted, -45.0F) == 0);
    assert(classify_tap_delta(adjusted, 45.0F) == 11);

    // Internal caps and the side pivot bound how far a neighbor-derived limit
    // can cross into the checker. These values force both limits near center.
    auto capped = nested_windows;
    apply_tap_adjacent_window_adjustment(
        capped, {true, -100.0F, true, -100.0F, 0.0F, -4.0F, 4.0F});
    for (const TapInterval& band : capped.bands_outer_to_center) {
        assert(band.lower == -4.0F);
        assert(band.upper == 4.0F);
    }

    auto disabled_adjustment = nested_windows;
    disabled_adjustment.enabled = false;
    apply_tap_adjacent_window_adjustment(
        disabled_adjustment,
        {true, 1.0F, true, 1.0F, 0.0F, -1.0F, 1.0F});
    assert(disabled_adjustment.bands_outer_to_center[0].lower == -50.0F);
    assert(disabled_adjustment.bands_outer_to_center[0].upper == 50.0F);

    constexpr std::uint8_t acceptance[12]{0, 0, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 2};
    constexpr std::uint8_t tiers[12]{0, 0, 0, 1, 2, 3,
                                     4, 3, 2, 1, 0, 0};
    constexpr std::uint8_t sides[12]{0, 0, 1, 1, 1, 1,
                                     0, 2, 2, 2, 2, 0};
    constexpr std::uint8_t table_codes[12]{1, 4, 6, 8, 10, 11,
                                           9, 7, 5, 3, 1, 0};
    for (std::size_t i = 0; i < tap_fine_results.size(); ++i) {
        assert(tap_fine_results[i].acceptance_code == acceptance[i]);
        assert(tap_fine_results[i].coarse_tier == tiers[i]);
        assert(tap_fine_results[i].side_code == sides[i]);
        assert(tap_fine_results[i].table_code == table_codes[i]);
        assert(tap_fine_results[i].accepted() == (acceptance[i] != 0));
    }

    assert(tap_detailed_result_code(0, 0) == 11);
    assert(tap_detailed_result_code(1, 1) == 3);
    assert(tap_detailed_result_code(1, 2) == 9);
    assert(tap_detailed_result_code(2, 1) == 4);
    assert(tap_detailed_result_code(2, 2) == 8);
    assert(tap_detailed_result_code(3, 1) == 5);
    assert(tap_detailed_result_code(3, 2) == 7);
    assert(tap_detailed_result_code(4, 0) == 6);
    assert(tap_detailed_result_code(99, 0) == 0);

    // The active result-control threshold is applied after classification but
    // before the detailed code conversion.
    assert(apply_active_result_threshold(4, false, 4, 5) == 4);
    assert(apply_active_result_threshold(0, true, 2, 5) == 0);
    assert(apply_active_result_threshold(1, true, 2, 5) == 0);
    assert(apply_active_result_threshold(2, true, 2, 5) == 0);
    assert(apply_active_result_threshold(3, true, 2, 5) == 3);
    assert(apply_active_result_threshold(4, true, 4, 5) == 0);
    assert(apply_active_result_threshold(5, true, 4, 5) == 5);
    assert(apply_active_result_threshold(2, true, 5, 5) == 2);
    assert(tap_detailed_result_code(
               apply_active_result_threshold(4, true, 4, 5), 0) == 11);
}
