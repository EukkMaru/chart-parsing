# Judgement

## Parameterized TAP lane classifier

For a finite timing delta and an enabled lane record, classify five configured
intervals in this order: center, inner, middle, outer, outermost. Every interval
is half-open: its lower endpoint is included and its upper endpoint is excluded.
Center has precedence if configured intervals overlap.

Let the configured pivot divide early (`delta < pivot`) from the other side.
The resulting internal fine-result index is:

| Location | Early side | Other side |
| --- | ---: | ---: |
| Center | 6 | 6 |
| Inner | 5 | 7 |
| Middle | 4 | 8 |
| Outer | 3 | 9 |
| Outermost | 2 | 10 |
| Outside all intervals | 0 | 11 |

A disabled lane returns index 0. The snapshot's static result table marks
indices 2 through 10 with acceptance byte 1, index 11 with acceptance byte 2,
and indices 0 and 1 with zero. Consumers treat a nonzero byte as accepted.

The exact reconstruction is `chart::reconstruction::classify_tap_delta` and
its metadata is `chart::reconstruction::tap_fine_results`. Evidence:
`claim.judgement.tap-window-classification`; boundary tests:
`tests/tap_window_test.cpp`.

### Per-substep center widening

After candidate exposure but before TAP input classification, each enabled
lane independently compares its inner and middle lower endpoints and its inner
and middle upper endpoints. When an absolute difference is strictly less than
the executable-owned `0.00001F` epsilon, the corresponding center endpoint is
replaced by the inner endpoint. Equality with epsilon does not mutate. Disabled
lanes are unchanged.

This can convert an inner-band fine result to center in the same substep. It
cannot alter candidate exposure because that earlier phase reads the outermost
interval. The exact reconstruction is
`chart::reconstruction::apply_tap_center_window_adjustment`. Evidence:
`claim.judgement.tap-center-window-adjustment`; tests:
`tests/tap_window_test.cpp`.

### Nearby-record endpoint trimming

Successful chart-load postprocessing can attach a preceding and/or following
distance to each lane of a record when selected nearby records overlap that
lane. During TAP checker initialization, the preceding distance selects an
early lower limit and the following distance selects a late upper limit:

- `lower_limit = min(base - preceding_distance, early_cap, pivot)`
- `upper_limit = max(base + following_distance, late_cap, pivot)`

Every band lower endpoint becomes `max(lower, lower_limit)` when the preceding
flag is present. Every upper endpoint becomes `min(upper, upper_limit)` when
the following flag is present. Disabled lanes are unchanged. The adjusted
values then use the same center-first, half-open classifier above. All base,
cap, pair-threshold, and participation values are runtime configuration inputs.
Evidence: `claim.judgement.tap-adjacent-window-adjustment`; reconstruction:
`apply_tap_adjacent_window_adjustment`.

## Detailed anonymous result code

The static metadata assigns provisional coarse tiers 0 through 4 and side codes
1 or 2. Before detailed conversion, a loaded skill profile's change-result
control can replace the provisional tier with 0. The remap requires a
nonnegative loaded profile ID, a nonempty control vector, and a present source
record in the vector's first unit. Later units are ignored. Both tier and that
first threshold must be below the runtime result-type count, and
`tier <= threshold`.

This check does not call the separate temporary-effect lifetime predicate used
by other skill consumers. Its boundary is profile load/reset, not the
temporary-effect timer. Evidence: `claim.judgement.active-tier-zeroing`;
reconstruction:
`chart::reconstruction::apply_loaded_active_result_controls`.

The downstream conversion of the post-remap tier is:

| Coarse tier | Side 1 | Other side |
| --- | ---: | ---: |
| 0 | 11 | 11 |
| 1 | 3 | 9 |
| 2 | 4 | 8 |
| 3 | 5 | 7 |
| 4 | 6 | 6 |

These are deliberately anonymous internal codes. The code reaches the shared
result owner as event data, but no player-facing judgement name is normative.

## Shared result category and two-stage dispatch boundary

Before category mapping, the note selects its result-component identifier:

| Source result category | Identifier slot |
| ---: | --- |
| 0, 1 | primary |
| 2 through 6 | middle/end |
| 7 through 13 | attached secondary |
| other | primary default |

The parser assigns those slots from a pass-local source-order counter and
runtime loading preserves them unchanged. The selected identifier becomes
field zero of the retained result event. It does not control result-byte
validity, contribution, or category mapping. Exact reconstruction:
`allocate_c2s_root_result_identifiers`,
`attach_c2s_secondary_result_identifier`, and
`select_note_result_identifier`; evidence:
`claim.judgement.result-component-identifier-flow`; tests:
`tests/result_identifier_test.cpp`.

Before result-owner dispatch, the source category selects a dispatch record as
follows:

| Source category | Dispatch category |
| ---: | ---: |
| 0 | 0 |
| 1 | 4 |
| 2, 3 | 1 |
| 4, 5, 6 | 2 |
| 7, 8, 9 | 3 |
| 10, 11 | 5 |
| 12, 13, 18 | 7 |
| 14, 15, 16 | 6 |
| 17 | 11 |
| other | -1 |

Each dispatch record independently selects a five-way progress category and a
nine-way aggregate category:

| Dispatch category | Progress category | Aggregate category |
| ---: | ---: | ---: |
| 0 | 0 | 0 |
| 1 | 1 | 1 |
| 2 | 2 | 2 |
| 3 | 3 | 3 |
| 4 | 0 | 4 |
| 5 | 3 | 5 |
| 6 | 4 | 6 |
| 7 | 3 | 7 |
| 8 | 3 | 5 |
| 9 | 3 | 7 |
| 10 | 3 | 7 |
| 11 | 0 | 8 |
| 12 | 3 | 7 |
| 13 | 3 | 7 |

The source category must first be within the runtime source-category table.
Dispatch categories 0 through 13 can reach result dispatch. If the pre-dispatch
terminal predicate is already true, dispatch invokes the stored result observer
and skips normal result state. Otherwise an event is built. All fourteen fixed
dispatch records select a valid aggregate category 0 through 8; on this closed
path, only a result byte outside the runtime result-type range prevents an
ordinary event from entering the authoritative aggregate. A successful event
saturating-increments its 32-bit count and updates its accumulated value using
runtime-loaded contribution rules.

For an ordinary valid result, the per-result owner invokes the observer after
the independent mapping to progress category 0 through 4. Every fixed dispatch
record maps into that range. Its default callable is a no-op. The only resolved
replacement captures an optional progress object, increments its processed
count, and clamps the result to an expected count initialized from the existing
counts in those five progress categories. Terminal routing invokes that
observer directly before result validation. The observer does not mutate active
notes, input state, aggregate values, or the terminal-route flag.

The fixed category conversions, validation, count saturation, and route
ordering are reconstructed by `map_shared_result_category`,
`map_dispatch_progress_category`, `map_dispatch_aggregate_category`,
`route_shared_result`, and `dispatch_shared_result`. Runtime table counts and
contribution values remain parameters. Evidence:
`claim.judgement.shared-result-two-stage-routing`; tests:
`tests/shared_result_test.cpp`.

The TAP-derived CHR runtime object reaches this boundary with source category 1
rather than ordinary TAP's 0. It therefore maps to category 4 rather than 0,
while using the same timing/result byte. The exact local selector is
`tap_variant_source_category`. Evidence:
`claim.note.chara-tap-result-category`.

An aggregate rule evaluation can set the terminal-route flag immediately. A
later result in the same active-note update pass then takes the observer-only
route instead of the aggregate route, although that later note still performs
its own finalization. Evidence:
`claim.interactions.result-terminal-short-circuit`.

### Ordinary terminal-summary producers

Each ordinary valid aggregate evaluation begins with a clear primary and
secondary summary. A matched configured terminal rule can zero the computed
contribution and set primary. Only if primary is still clear can a later
runtime end-threshold producer zero the contribution and set both primary and
secondary. Thus a configured rule wins when both predicates are true and the
result is primary-only.

The result owner latches nonzero summaries in separate bytes; reset clears
both. The executable-owned suppression byte checked by the two producers is
initialized to zero and has no recovered nonzero writer. The configured-rule
conditions, scaled metric identity, and external threshold values remain
parameterized rather than guessed. Exact structural reconstruction is
`produce_ordinary_terminal_summary`. Evidence:
`claim.judgement.ordinary-terminal-summary-producers`; tests:
`tests/shared_result_test.cpp`.

### Periodic aggregate reevaluation

The ordinary result owner is also evaluated once at the end of every outer
gameplay update, after all current input/note substeps and post-note passes.
This path constructs snapshot kind 2 from retained owner state and a zero
per-event contribution. The same aggregate evaluator replaces the retained
computed value and latches the same terminal bytes used by kind-1 result
events.

Kind 2 has deliberately different rule eligibility:

- the common contribution vector remains active; external rule fields can add
  a value, update linked rule-record state, and enable two kind-2 progress
  checks; a marked one-shot unit succeeds only on its first valid indexed use;
- the event-only negative-adjustment vector cannot return an adjustment;
- the common promotion vector can still raise the computed value after its
  external gates pass;
- configured type-3 terminal rules reject kind 2; and
- the later track-skip/end-threshold producer remains eligible.

Consequently the ordinary terminal route can latch on an update that contains
no new note result. Since reevaluation follows the note pass, it affects later
result dispatches and the gameplay-state exit predicate, not a result already
processed in the same update. External rule values and the resulting periodic
contribution/promotion remain parameters; no random predicate participates.
Structural reconstruction is `apply_ordinary_periodic_aggregate`,
`ordinary_rule_one_shot_allows`, and the `OrdinaryAggregateSnapshotKind`
predicates. Evidence:
`claim.judgement.periodic-aggregate-reevaluation`; tests:
`tests/shared_result_test.cpp`.

### Result-count completion and active gameplay exit

Before active state `0x0f`, structural states `0x0a..0x0e` already run the
complete outer gameplay update with a zero clock base and zero physical-input
sample. Note state machines, lazy materialization, removal, and kind-2
aggregate evaluation remain enabled. This priming interval is normative for
zero- or negative-scheduled behavior and for externally configured periodic
rules; it is not an outcome-free presentation phase. Evidence:
`claim.pipeline.pre-active-zero-base-priming`; tests:
`tests/gameplay_exit_test.cpp`.

Gameplay setup distributes all fourteen dispatch-category counts through the
five-way progress mapping. It enables result-count completion only when the
32-bit sum of those five expected counts is nonzero. While enabled, completion
is the unsigned inclusive comparison:

```text
expected_total <= processed_total
```

The processed total is the sum of all runtime-valid result-byte counts across
the same five progress categories. Reset clears the enable and all counts, so
a zero-count setup cannot complete through this predicate.

After the full current outer update and ordinary report attempt, the ordinary
controller leaves structural active state `0x0f` when result-count completion
or its selected terminal predicate is true. The alternate controller instead
leaves it when its nested state is exactly 3, or when result-count completion
and an independent nested ready byte are both true. The selected terminal
predicate alone does not leave the alternate path. Reconstruction is
`result_count_completion_reached` and `decide_active_gameplay_exit`. Evidence:
`claim.pipeline.active-gameplay-exit-gate`; tests:
`tests/gameplay_exit_test.cpp`.

States `0x10`, `0x11`, and `0x12` continue running full gameplay updates before
state-`0x13` runtime-note teardown. A terminal-routed ordinary exit therefore
keeps later result submissions on the observer-only path during the drain. An
alternate nested-state-3 exit need not imply completion or terminal latching,
so later runtime-valid results can remain authoritative until teardown. Every
post-active update also performs the periodic aggregate reevaluation. Evidence:
`claim.pipeline.post-active-gameplay-drain`.

The outcome after the final drain update remains authoritative through
teardown. State `0x13` clears only two now-consumed configured-rule notification
masks, not the current aggregate, per-result counts, terminal bytes, or
alternate controller. State `0x14` resets an indexed destination and deep-copies
the complete current result object into it. Evidence:
`claim.pipeline.final-result-persistence-order`.

Configured type-3 rules run only for event type 1 and separately return
`matched` and `terminal_requested`. Their ordered conditions are:

- an enabled computed-value floor, with a zero-unit `value <= 0` boundary and
  otherwise strict `value < event_scale_units * 0.01` boundary;
- an enabled, nonzero first result-weight bucket plus unsigned event count
  strictly below a configured limit;
- an enabled aggregate metric at or above a configured threshold;
- a result band whose event result is at or below a configured ceiling, with
  both bytes below the runtime result count; and
- an enabled mode-2 gauge at zero.

The result band matches regardless of whether its cumulative bucket weight
reaches the configured terminal threshold. Thus it can consume the rule without
requesting terminal. Every band match also applies a unit gauge update before
the zero-gauge test; an update-eligible mode-2 gauge can fall from one to zero
and request terminal in the same evaluation. Exact reconstruction is
`evaluate_ordinary_configured_terminal_rule`. Evidence:
`claim.judgement.ordinary-configured-terminal-rule`; tests:
`tests/shared_result_test.cpp`.

The later producer is disabled for live `PlayOptionTrackSkipID` zero. For any
other ID, after independent mode/state gates pass, it compares the unsigned
aggregate metric against:

`(1010000U - selected_value) + 1U`

ID 7 selects a current-track-derived value when that runtime record exists, or
zero when absent. Every other nonzero ID maps through external
`PlayOptionTrackSkipTable` field `+0x0c` into `ScoreRankTable`, then selects
that row's field `+0x0c`; invalid lookups produce zero. Exact arithmetic and
selection are reconstructed by
`ordinary_terminal_end_threshold_reached`. Evidence:
`claim.judgement.ordinary-terminal-end-threshold`; tests:
`tests/shared_result_test.cpp`.

## Alternate terminal meter

When the bitwise OR of three runtime controller selector bytes is exactly one,
the pre-dispatch terminal predicate uses a separate configurable meter. In its
ordinary form, a nonzero maximum and zero current are terminal. A second form
instead tests whether a distinct setup-derived participant count is zero.

The auxiliary count is captured from a bounded prefix of runtime participant
records. Setup modes 1, 2, and 4 restrict the count to the local-identity
record; other modes can also include records carrying a shared-mode flag. The
local record contributes one unless the meter's ordinary zero latch was already
set. A selected non-local record contributes one only when its effective
current value is nonzero. The same capture stores the participant limit.

The initial gameplay setup requests a lock byte while capturing these values.
Later periodic setup attempts do not replace a locked capture. Construction
and external meter configuration clear the lock, count, and limit. Result
dispatch does not decrement the auxiliary count in this executable, so the
second predicate observes the captured/reset value, not the ordinary signed
meter-delta progression. Participant and mode names remain structural.

Selector byte 2 has a one-shot empty-participant fallback during that initial
capture. It runs only after a writable capture stored zero selected
participants and only while the currently selected terminal predicate is still
false. The fallback requests current zero, latches zero when the prior current
was nonzero, and also sets the ordinary result-owner terminal byte. The same
initial call has already requested the capture lock, so periodic gameplay calls
cannot reapply it. Gameplay setup is the only internal selector-2 writer and
the next setup/reset clears both selector and result bytes. Selector 2 therefore
continues selecting the alternate meter for the current session: the ordinary
byte write can affect direct controller data-source branches, but it does not
replace the selected pre-dispatch predicate while selector 2 remains active.

For a non-terminal dispatch, the current event performs normal per-result and
aggregate work before its anonymous result code updates the meter. Codes 0, 1,
and 2 select three external signed deltas; codes 3 and 4 share a fourth; other
codes select zero. The meter update is:

`current = clamp(current - selected_delta, 0, maximum)`

Thus positive configured values drain it and negative values restore it. The
zero state is latched; restoration is ignored while already full, and the
auxiliary form rejects further result deltas after zero has been latched. A
terminal transition affects the next dispatch, including a later note in the
same manager pass. External values remain parameters. Evidence:
`claim.judgement.alternate-terminal-meter`; reconstruction and tests:
`AlternateTerminalMeter`, `tests/alternate_meter_test.cpp`.

## Expiration

A TAP with no accepted lane remains active while its delta is below the overall
late bound. At or beyond the bound it supplies coarse tier 0 and side code 0 to
the detailed conversion, producing anonymous detailed result code 11, then
completes. Evidence: `claim.note.tap-candidate-judgement-gate` and
`claim.judgement.tap-window-classification`. Exact bound values and the
player-facing terminal name are unresolved.

## HOLD inactive-gap result

HOLD classifies inactivity, not accumulated held duration. Once its configured
participation floor is reached, it retains the maximum interval since the
greater of last active time and that floor. The first of four external
thresholds greater than or equal to the gap supplies index 0 through 3; no
match or never-active supplies index 4. Scheduled checkpoint emission maps
indices 0, 1, 2, and 3 to anonymous bytes 4, 3, 2, and 1 respectively, with all
other indices mapping to byte 0.

After each checkpoint, the retained maximum becomes the current open gap. Full
state and endpoint rules are in `spec/notes/hold.md`. Evidence:
`claim.note.hold-sustain-gap-lifecycle`; reconstruction:
`chart::reconstruction::HoldGapState`; tests:
`tests/hold_sustain_test.cpp`.

Type-2 slide path checkpoints use the same retained inactive-gap classifier and
byte conversion. They differ in source-category selection, front-record
schema, and the conjunction with a separate TAP-style start phase. Those exact
rules are normative in `spec/notes/slide.md`. Evidence:
`claim.note.slide-path-sustain-judgement`; reconstruction and tests:
`update_slide_checkpoints`, `tests/slide_path_test.cpp`.

## Forced-result selector

When its manager enable is clear, the selector returns anonymous byte 0. When
enabled, its integer mode maps as follows:

| Mode | Result |
| ---: | --- |
| 1 | byte 4 |
| 2 | byte 3 |
| 3 | byte 2 |
| 4 | byte 1 |
| 5 | bytes 4, 3, 2, 1 from shared `counter & 3`, then increment counter |
| 6 | bytes 4, 3, 2, 1, 0 from `random_value % 5` |
| other | byte 0 |

The active branch is `enable && mode != 0`. At its configured timing point,
the shared TAP/HOLD start gate completes from this selector without physical
lane input. HOLD also forces aggregate sustain activity true and uses the
selector instead of retained-gap classification at checkpoints and its
once-only end path. The selector's second source argument is ignored in this
snapshot.

The closed exact-snapshot producer set contains only reset/teardown mode 0 and
tutorial mode 2. The tutorial callback writes mode 2 from an indexed step flag,
so the only reachable active override selects byte 3. Modes 1 and 3 through 6
remain part of the selector implementation but are unreachable: the manager
field has no further writer or address escape. Therefore the cycling counter
and RNG cases describe dormant code, not gameplay variability in this
snapshot. The distinct companion field is fixed at zero, so its enabled
selector result is always 1.

Exact reconstruction: `chart::reconstruction::select_forced_result_byte`,
`forced_result_mode_has_snapshot_producer`, and
`select_reachable_forced_result_companion`.
Evidence: `claim.judgement.forced-result-mode`; tests:
`tests/forced_result_test.cpp`.

## FLK edge, motion, and result composition

FLK first applies the parameterized primary lane classifier to an initiating
rising edge, then uses a second parameterized checker to bound and classify
lateral motion completion. Its two-bank centroid tracking, terminal phases,
timeout bytes, coarse-tier composition, side normalization, and forced-direction
alternation are normative in `spec/notes/flick.md`. Reconstruction is provided
by the `Flick*` helpers; focused tests are in `tests/flick_motion_test.cpp`.
Evidence: `claim.note.flick-motion-judgement`.

## MNE held-contact threshold

MNE integrates clipped elapsed time whenever any covered logical lane is held.
At nonnegative adjusted time, it enters terminal phase 2 when the external
requirement is strictly greater than accumulated contact plus all remaining
time, or terminal phase 1 when accumulated contact is greater than or equal to
the requirement. Phase 1 supplies provisional byte 0 and phase 2 byte 4.

Both results use source category 17, which selects dispatch category 11. That
record selects progress category 0 and aggregate category 8, so a runtime-valid
ordinary MNE result does enter the authoritative aggregate. Full interval,
candidate, forced-mode, and lifecycle rules are normative in
`spec/notes/mine.md`. Reconstruction uses the `Mine*` helpers; focused tests are
in `tests/mine_contact_test.cpp`. Evidence:
`claim.note.mine-contact-aggregate-judgement`.

## AIR retained-profile timing result

AIR uses the shared five-interval geometry with a separate fixed rank and
result table. Input before the pivot can improve a retained fine index without
completing. At/after the pivot, retained center completes immediately; other
retained values complete once better-ranked than the current timing location,
or a current nonzero profile sample can complete at its equal/better location.
The outer lower gate is strict and the outer upper endpoint otherwise produces
accepted fine index 11. Exact ranks, result metadata, profile selection,
forced-mode interaction, and lifecycle are normative in `spec/notes/air.md`.

Direction codes 0 through 2 use source category 7 and codes 3 through 5 use
source category 8. Both map to shared category 3 and are eligible for the
authoritative aggregate. Reconstruction uses the `Air*` helpers; focused tests
are in `tests/air_judgement_test.cpp`. Evidence:
`claim.note.air-secondary-judgement`.

## AirHold start, path, and authored checkpoint results

AirHold's independent start checker uses the AIR retained-profile classifier
and submits source category 9. Its generated path records use HOLD's retained
maximum inactive-gap classifier, submit source category 10 when their emission
flag is enabled, and ordinarily reset the maximum only after emission. Each
authored AHX checker uses AIR profile 6 and submits category 12 except for final
category 13. These map to shared categories 3, 5, 7, and 7 and can all reach
the authoritative aggregate.

At most one generated record and one current AHX checker can resolve in a
substep. The secondary is terminal only when the start result is complete, the
generated vector is empty, and all AHX checkers are complete. Exact parser,
input, reset, and forced-result boundaries are normative in
`spec/notes/air_hold.md`; reconstruction/tests:
`AirHold*`, `tests/air_hold_judgement_test.cpp`. Evidence:
`claim.note.air-hold-secondary-judgement`.

## AirSlide start, path, and ASD checkpoint results

AirSlide submits start category 9, enabled generated-path category 11, and
authored ASD category 12 or 13 depending on whether that ASD is the final
control point. They map to shared categories 3, 5, 7, and 7. Unlike AirHold,
ordinary processing classifies and resets the retained maximum for every due
generated record before consulting its emission byte; a disabled record is
therefore stateful even though it submits no result.

Only one generated front and one current ASD checker can resolve per substep.
The secondary becomes terminal only after the start phase, generated vector,
and all ASD checkers complete. Exact generation, input, forced-result, and
marker rules are normative in `spec/notes/air_slide.md`; reconstruction/tests:
`AirSlide*`, `tests/air_slide_judgement_test.cpp`. Evidence:
`claim.note.air-slide-secondary-judgement`.

## AirLadder checkpoint results

Ordinary ALD/AirLadder builds one profile-7 retained checker per sampled
`0x20` generated record, not per `0x24` authored control. The parser starts at
the root and advances by ALD's exact positive interval, interpolating lane,
vertical value, and decoded width across the authored chain. AirLadder can
resolve multiple generated checkers in one substep because it iterates the
entire runtime vector. Every accepted checker submits source category 18,
mapped category 7. Completion additionally waits for the chart-derived end
with an inclusive `current >= final authored endpoint schedule` boundary for
ordinary finite values. That schedule is chart-derived by the type-9
precompute, not externally configured. The zero-interval/`NON` ALD
class-selection exception is HeavenHoldNote and is tracked separately. Exact
behavior is normative in `spec/notes/air_ladder.md`; reconstruction/tests:
`generate_air_ladder_checkpoints`, `air_ladder_*`, and
`tests/air_ladder_judgement_test.cpp`. Evidence:
`claim.note.air-ladder-generated-checkpoints` and
`claim.note.air-ladder-precalc-presentation`.

## HeavenHold start and path results

HHD/HHX type 13 and exceptional zero/`NON` ALD share HeavenHoldNote. Its
candidate-exposing TAP start submits source 0 or 1 and completes start phase 4.
Its independent gap path consumes at most one due generated front per substep:
ordinary kind 0 uses source 2 and final kind 1 uses source 3, both mapped to
shared category 1. Ordinary classification/reset happens even for disabled due
records; the emission flag gates submission only. Exceptional ALD has no
generated queue, so its path reaches phase 4 at the first eligible path update.
The adjusted-end grade is feedback-only. Both component phases must equal 4
before deferred termination. Exact input, generation, forced-result, and phase
rules are normative in `spec/notes/heaven_hold.md`; reconstruction/tests:
`HeavenHold*`, `tests/heaven_hold_judgement_test.cpp`. Evidence:
`claim.note.heaven-hold-judgement`.
