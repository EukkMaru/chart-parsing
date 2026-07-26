#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

int main() {
    using chart::reconstruction::OrdinaryConfiguredTerminalContext;
    using chart::reconstruction::OrdinaryConfiguredTerminalResult;
    using chart::reconstruction::OrdinaryConfiguredTerminalRule;
    using chart::reconstruction::OrdinaryAggregateSnapshotKind;
    using chart::reconstruction::OrdinaryTerminalSummary;
    using chart::reconstruction::SharedResultRoute;
    using chart::reconstruction::SharedResultRoutingState;
    using chart::reconstruction::capped_result_observer_increment;
    using chart::reconstruction::dispatch_shared_result;
    using chart::reconstruction::evaluate_ordinary_configured_terminal_rule;
    using chart::reconstruction::apply_ordinary_periodic_aggregate;
    using chart::reconstruction::map_dispatch_aggregate_category;
    using chart::reconstruction::map_dispatch_progress_category;
    using chart::reconstruction::map_shared_result_category;
    using chart::reconstruction::ordinary_configured_terminal_accepts;
    using chart::reconstruction::ordinary_negative_adjustment_accepts;
    using chart::reconstruction::ordinary_periodic_progress_accepts;
    using chart::reconstruction::ordinary_terminal_end_threshold_reached;
    using chart::reconstruction::periodic_aggregate_position_tick;
    using chart::reconstruction::produce_ordinary_terminal_summary;
    using chart::reconstruction::route_shared_result;
    using chart::reconstruction::saturating_result_count_increment;
    using chart::reconstruction::tap_variant_source_category;

    assert(tap_variant_source_category(false, false) == 0);
    assert(tap_variant_source_category(true, false) == 1);
    assert(tap_variant_source_category(true, true) == 0);

    assert(map_shared_result_category(0) == 0);
    assert(map_shared_result_category(1) == 4);
    assert(map_shared_result_category(2) == 1);
    assert(map_shared_result_category(3) == 1);
    assert(map_shared_result_category(4) == 2);
    assert(map_shared_result_category(5) == 2);
    assert(map_shared_result_category(6) == 2);
    assert(map_shared_result_category(7) == 3);
    assert(map_shared_result_category(8) == 3);
    assert(map_shared_result_category(9) == 3);
    assert(map_shared_result_category(10) == 5);
    assert(map_shared_result_category(11) == 5);
    assert(map_shared_result_category(12) == 7);
    assert(map_shared_result_category(13) == 7);
    assert(map_shared_result_category(14) == 6);
    assert(map_shared_result_category(15) == 6);
    assert(map_shared_result_category(16) == 6);
    assert(map_shared_result_category(17) == 11);
    assert(map_shared_result_category(18) == 7);
    assert(map_shared_result_category(-1) == -1);
    assert(map_shared_result_category(19) == -1);

    constexpr std::int32_t expected_progress_categories[14]{
        0, 1, 2, 3, 0, 3, 4, 3, 3, 3, 3, 0, 3, 3};
    constexpr std::int32_t expected_aggregate_categories[14]{
        0, 1, 2, 3, 4, 5, 6, 7, 5, 7, 7, 8, 7, 7};
    for (std::int32_t dispatch_category = 0; dispatch_category < 14;
         ++dispatch_category) {
        assert(map_dispatch_progress_category(dispatch_category) ==
               expected_progress_categories[dispatch_category]);
        assert(map_dispatch_aggregate_category(dispatch_category) ==
               expected_aggregate_categories[dispatch_category]);
    }
    assert(map_dispatch_progress_category(-1) == -1);
    assert(map_dispatch_progress_category(14) == -1);
    assert(map_dispatch_aggregate_category(-1) == -1);
    assert(map_dispatch_aggregate_category(14) == -1);

    constexpr std::uint32_t source_count = 19;
    constexpr std::uint32_t result_count = 12;
    assert(route_shared_result(-1, source_count, 4, result_count, false) ==
           SharedResultRoute::ignored);
    assert(route_shared_result(19, source_count, 4, result_count, false) ==
           SharedResultRoute::ignored);
    assert(route_shared_result(17, source_count, 4, result_count, false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(route_shared_result(0, source_count, 12, result_count, false) ==
           SharedResultRoute::dispatched_without_aggregate);
    assert(route_shared_result(0, source_count, 4, result_count, false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(route_shared_result(0, source_count, 4, result_count, true) ==
           SharedResultRoute::observer_only);

    SharedResultRoutingState state{};
    assert(dispatch_shared_result(state, 0, source_count, 4, result_count,
                                  true) ==
           SharedResultRoute::authoritative_aggregate);
    assert(state.aggregate_count == 1);
    assert(state.observer_notification_count == 1);
    assert(state.terminal_route_active);

    // The next same-pass event is observer-routed instead of accumulated.
    assert(dispatch_shared_result(state, 0, source_count, 4, result_count,
                                  false) ==
           SharedResultRoute::observer_only);
    assert(state.aggregate_count == 1);
    assert(state.observer_notification_count == 2);

    SharedResultRoutingState wider_category{};
    assert(dispatch_shared_result(wider_category, 12, source_count, 4,
                                  result_count, false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(wider_category.aggregate_count == 1);
    assert(wider_category.observer_notification_count == 1);

    SharedResultRoutingState mine_category{};
    assert(dispatch_shared_result(mine_category, 17, source_count, 4,
                                  result_count, false) ==
           SharedResultRoute::authoritative_aggregate);
    assert(mine_category.aggregate_count == 1);
    assert(mine_category.observer_notification_count == 1);

    assert(capped_result_observer_increment(0, 3) == 1);
    assert(capped_result_observer_increment(2, 3) == 3);
    assert(capped_result_observer_increment(3, 3) == 3);
    assert(capped_result_observer_increment(9, 0) == 0);
    assert(capped_result_observer_increment(
               std::numeric_limits<std::uint32_t>::max(), 3) == 0);

    assert(saturating_result_count_increment(0) == 1);
    assert(saturating_result_count_increment(
               std::numeric_limits<std::uint32_t>::max()) ==
           std::numeric_limits<std::uint32_t>::max());

    const OrdinaryTerminalSummary ordinary =
        produce_ordinary_terminal_summary(12.5, false, false);
    assert(ordinary.value == 12.5);
    assert(!ordinary.primary);
    assert(!ordinary.secondary);

    const OrdinaryTerminalSummary configured_rule =
        produce_ordinary_terminal_summary(12.5, true, false);
    assert(configured_rule.value == 0.0);
    assert(configured_rule.primary);
    assert(!configured_rule.secondary);

    const OrdinaryTerminalSummary end_threshold =
        produce_ordinary_terminal_summary(12.5, false, true);
    assert(end_threshold.value == 0.0);
    assert(end_threshold.primary);
    assert(end_threshold.secondary);

    // The configured-rule path sets primary first, so the end-threshold branch
    // is skipped when both predicates would otherwise be true.
    const OrdinaryTerminalSummary configured_rule_precedence =
        produce_ordinary_terminal_summary(12.5, true, true);
    assert(configured_rule_precedence.value == 0.0);
    assert(configured_rule_precedence.primary);
    assert(!configured_rule_precedence.secondary);

    // The periodic conversion uses the separately stored approximate inverse,
    // single-precision multiplication, and floor. It is observably not exact
    // inversion of the forward 0.06F scale at these boundaries.
    assert(periodic_aggregate_position_tick(0.06F) == 0);
    assert(periodic_aggregate_position_tick(0.12F) == 1);
    assert(periodic_aggregate_position_tick(60.0F) == 999);
    assert(periodic_aggregate_position_tick(-0.06F) == -1);

    assert(ordinary_negative_adjustment_accepts(
        OrdinaryAggregateSnapshotKind::result));
    assert(!ordinary_negative_adjustment_accepts(
        OrdinaryAggregateSnapshotKind::periodic));
    assert(ordinary_configured_terminal_accepts(
        OrdinaryAggregateSnapshotKind::result));
    assert(!ordinary_configured_terminal_accepts(
        OrdinaryAggregateSnapshotKind::periodic));
    assert(!ordinary_periodic_progress_accepts(
        OrdinaryAggregateSnapshotKind::result, true));
    assert(!ordinary_periodic_progress_accepts(
        OrdinaryAggregateSnapshotKind::periodic, false));
    assert(ordinary_periodic_progress_accepts(
        OrdinaryAggregateSnapshotKind::periodic, true));

    const OrdinaryTerminalSummary periodic_contribution =
        apply_ordinary_periodic_aggregate(10.0, 2.5, false, 0.0, false);
    assert(periodic_contribution.value == 12.5);
    assert(!periodic_contribution.primary);
    assert(!periodic_contribution.secondary);

    const OrdinaryTerminalSummary periodic_promotion =
        apply_ordinary_periodic_aggregate(10.0, 2.5, true, 15.0, false);
    assert(periodic_promotion.value == 15.0);
    const OrdinaryTerminalSummary lower_promotion =
        apply_ordinary_periodic_aggregate(10.0, 2.5, true, 11.0, false);
    assert(lower_promotion.value == 12.5);

    // Kind 2 cannot request configured primary-only termination, but the
    // later common end threshold still zeros the result and latches both.
    const OrdinaryTerminalSummary periodic_end =
        apply_ordinary_periodic_aggregate(10.0, 2.5, true, 15.0, true);
    assert(periodic_end.value == 0.0);
    assert(periodic_end.primary);
    assert(periodic_end.secondary);

    // Track-skip option zero disables the end-threshold producer regardless of
    // the supplied external value and aggregate metric.
    assert(!ordinary_terminal_end_threshold_reached(
        0, std::numeric_limits<std::uint32_t>::max(), 1000000, true, 1000000));

    // Ordinary nonzero options use the value mapped through TrackSkip and
    // ScoreRank tables: 1,010,000 - 10,000 + 1 == 1,000,001.
    assert(!ordinary_terminal_end_threshold_reached(
        1, 1000000, 10000, false, 0));
    assert(ordinary_terminal_end_threshold_reached(
        1, 1000001, 10000, false, 0));

    // Option 7 selects current-track data when available, ignoring the
    // ordinary mapped value.
    assert(!ordinary_terminal_end_threshold_reached(
        7, 990000, 1, true, 20000));
    assert(ordinary_terminal_end_threshold_reached(
        7, 990001, 1, true, 20000));

    // A missing option-7 current-track record selects zero, so only the
    // executable score ceiling plus one reaches the comparison.
    assert(!ordinary_terminal_end_threshold_reached(
        7, 1010000, 10000, false, 20000));
    assert(ordinary_terminal_end_threshold_reached(
        7, 1010001, 10000, false, 20000));

    OrdinaryConfiguredTerminalRule configured{};
    configured.value_floor_enabled = true;
    OrdinaryConfiguredTerminalContext configured_context{};
    configured_context.event_type = 2;
    configured_context.computed_value = 0.0;
    OrdinaryConfiguredTerminalResult configured_result =
        evaluate_ordinary_configured_terminal_rule(
            configured, configured_context);
    assert(!configured_result.matched);
    assert(!configured_result.terminal_requested);

    configured_context.event_type = 1;
    configured_context.computed_value = 0.1;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(!configured_result.matched);
    configured_context.computed_value = 0.0;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.matched);
    assert(configured_result.terminal_requested);

    // A nonzero scale has a strict lower comparison; equality does not match.
    configured_context.event_scale_units = 5;
    configured_context.computed_value = 0.05;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(!configured_result.matched);
    configured_context.computed_value = 0.049;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.terminal_requested);
    configured_context.computed_value =
        std::numeric_limits<double>::quiet_NaN();
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.terminal_requested);

    configured = {};
    configured.first_bucket_limit_enabled = true;
    configured.first_bucket_count_limit = 3;
    configured.aggregate_metric_limit_enabled = true;
    configured.aggregate_metric_threshold = 10;
    configured_context = {};
    configured_context.event_type = 1;
    configured_context.first_result_bucket_nonzero = true;
    configured_context.event_count = 3;
    configured_context.aggregate_metric = 9;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(!configured_result.matched);
    configured_context.event_count = 2;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.terminal_requested);
    configured_context.first_result_bucket_nonzero = false;
    configured_context.event_count = 3;
    configured_context.aggregate_metric = 10;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.terminal_requested);

    // A result-band match can consume the rule without requesting terminal,
    // and it decrements the mode-2 gauge before the zero-gauge condition.
    configured = {};
    configured.result_ceiling = 2;
    configured.cumulative_result_weight_threshold = 5;
    configured_context = {};
    configured_context.event_type = 1;
    configured_context.result_type_count = 5;
    configured_context.result_byte = 2;
    configured_context.cumulative_result_weight = 4;
    configured_context.gauge_update_allowed = true;
    configured_context.gauge_enabled = true;
    configured_context.gauge_mode = 2;
    configured_context.gauge_current = 1;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.matched);
    assert(!configured_result.terminal_requested);
    assert(configured_result.gauge_current == 0);

    configured.zero_gauge_enabled = true;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.matched);
    assert(configured_result.terminal_requested);
    assert(configured_result.gauge_current == 0);

    configured.zero_gauge_enabled = false;
    configured_context.cumulative_result_weight = 5;
    configured_context.gauge_current = 2;
    configured_result = evaluate_ordinary_configured_terminal_rule(
        configured, configured_context);
    assert(configured_result.terminal_requested);
    assert(configured_result.gauge_current == 1);
}
