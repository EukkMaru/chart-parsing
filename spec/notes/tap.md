# TAP note

## Construction and lane extent

Parsed record type 0 constructs a runtime `projView::TapNote`. The record's
start-lane field is copied directly. Authored width is clamped to 1 through 16
and stored as `width - 1`; runtime loading decodes indices 0 through 15 back
to widths 1 through 16. Runtime lane coverage is:

```text
start = max(lane, 0)
count = max(min(lane + width, 16) - start, 0)
```

The involved tables are fixed and have no compatibility writer in this
snapshot. Reconstruction: `parse_c2s_common_lane_geometry`,
`decode_c2s_note_width`, and `bounded_note_lane_extent`; focused tests:
`tests/common_lane_width_test.cpp`; evidence:
`claim.parser.common-lane-width-encoding`.
The note also owns an embedded checker with one `0x38`-byte window record per
logical lane. Evidence: `claim.parser.event-family-type-map` and
`claim.note.tap-construction`.

The record's float at `+0x08` is multiplied directly by the executable's
initialized `0.06F` scale and stored as the note's scheduled chart position.
There is no integer conversion or explicit rounding on this field path.
Reconstruction: `make_note_scheduled_position`; evidence:
`claim.note.tap-construction`.

## Root presentation model

Tap and CharaTap share one active root-model transform. The decoded width
selects `clamp(width - 1, 0, 15)`. Ordinary Tap uses the executable's embedded
descending resource-row table `81..66`; CharaTap fixes the extended-primary
variant and uses `97..82`. The same loader also supports an
extended-alternate table `113..98`. These are exact resource-table indices;
the referenced proprietary model/material data remains external.

The initial model translation is:

```text
x = 4 * start_lane + 2 * decoded_width - 32
y = 0
z = -10000
```

Thus one lane is four chart-space units and the center of the complete
sixteen-lane field is zero. Initial scale is
`(decoded_width / external_native_width, 1, platform_depth_scale)`, with a
fallback lateral scale of `1` when the external native width is below one.
The executable selects depth scale `1.3` in its alternate platform/mode branch
and `1.0` otherwise.

While the family phase remains at most one, the active update replaces the
parked depth with the shared projection of
`scheduled_time - manager_time`, using the configured base offset and the
positive-delta DCM factor, writes the scale/translation matrix, and forces the
model visible. The family maintenance/reset slot hides it. The result/effect
half of the update, final camera conversion, scene layering, and external
model pixels are separate presentation boundaries.

Evidence: `claim.presentation.tap-chara-model-transform`; reconstruction:
`active_note_projected_depth`, `tap_model_lateral_center`,
`tap_model_resource_row`, `tap_model_lateral_scale`, and
`tap_model_depth_scale`; focused tests: `tests/tap_presentation_test.cpp`.

An ordinary TAP root reserves one source-ordered primary result identifier.
Its middle/end and secondary identifiers remain negative unless a compatible
attached component supplies the latter. The shared result path's exact slot
selection is normative in `spec/judgement.md`. Evidence:
`claim.judgement.result-component-identifier-flow`.

## Candidate phase

Before input evaluation, the note clears its 16 candidate slots. For each lane
within its bounded extent, it can expose its scheduled value only while
`current gameplay time - scheduled time` lies within the configured eligibility
bounds. The shared manager reduces all exposed values to the smallest
nonnegative candidate for each lane. Evidence:
`claim.note.tap-candidate-judgement-gate`.

## Input and completion

The normal TAP path examines covered lanes in ascending extent order. A lane is
submitted to its checker only when:

1. the newest input snapshot has that lane's derived rising-edge bit set;
2. the lane is in the 0-through-15 domain; and
3. the manager's selected candidate equals this TAP's candidate for that lane.

The first checker result with a nonzero acceptance byte completes the lane loop.
If no lane is accepted and the timing delta remains below the overall late
bound, the TAP stays incomplete. At or beyond that bound, it chooses a terminal
result and completes without an accepted input lane. The result then crosses a
shared downstream handler, which requests state 2 while current state remains
1 for the rest of that manager pass.

Completion also stores the detailed result in the embedded checker. At the
start of a TAP state callback, a stored signed value greater than 1 suppresses
another judgement attempt. Construction and checker reload initialize it to
zero. This guard is distinct from the deferred base-state transition and is not
consulted by candidate preparation. Reconstruction:
`tap_has_terminal_detailed_result`; evidence:
`claim.matching.tap-deferred-terminal-candidate`.

If the completed TAP is still timing-eligible on the following normal substep,
it participates in candidate reduction once more because candidate preparation
precedes the pending-state commit. The note then commits state 2 before its
state callback and is removed after the full update pass. It cannot judge again
in that substep, but its earlier scheduled value can temporarily hold priority
over a later unequal note. Evidence:
`claim.matching.tap-deferred-terminal-candidate`.

The generic note tick wrapper also has a byte guard at base offset `+0x65`.
The note base initializes it to zero, its registered callbacks do not set it,
and no applicable direct writer was found in the active-note path. It is
therefore inert for the reconstructed TAP lifetime in this snapshot; no
speculative pause/suspension behavior is assigned to it.

Equal selected TAP candidates share the same unconsumed rising edge during the
full active-note update pass; each can independently accept it. Evidence:
`claim.matching.tap-equal-candidate-fanout`. Results are category-mapped and
reach the authoritative aggregate under the validation and terminal-route rules
in `spec/judgement.md`. A prior result can activate terminal observer routing
for a later tied TAP without preventing that TAP's own completion. Evidence:
`claim.judgement.shared-result-two-stage-routing` and
`claim.interactions.result-terminal-short-circuit`. Cross-family ties, dynamic
creation order, equal-edge fanout, and same-pass result order are normative in
`claim.interactions.cross-family-candidate-result-order`. Player-facing result
labels are intentionally unassigned.

When manager forced-result state is active, the shared gate can complete after
its configured timing point without a rising edge or selected-candidate equality
test. The selector's fixed, cycling, and random cases are specified in
`spec/judgement.md`, but exact-snapshot producer closure makes tutorial mode 2
and byte 3 the only reachable nonzero case. Evidence:
`claim.judgement.forced-result-mode`.

## Window classification

Each lane uses the parameterized classifier in `spec/judgement.md` and
`chart::reconstruction::classify_tap_delta`. Exact endpoint values remain
external configuration. Focused boundary coverage is in
`tests/tap_window_test.cpp`.

Before classification, successful chart-load postprocessing can associate a
lane with preceding/following distances from nearby records whose lane spans
overlap. At checker initialization, a preceding distance raises all five lower
endpoints while a following distance lowers all five upper endpoints. External
internal caps and the side pivot bound those limits. This trims judgement
eligibility toward the note without changing center-first or half-open
classification. Evidence: `claim.judgement.tap-adjacent-window-adjustment`;
reconstruction: `apply_tap_adjacent_window_adjustment`.

On every manager substep, candidate exposure occurs before TAP's checker
mutation. For each enabled lane, the lower and upper center endpoints are
handled independently: when the absolute difference between the corresponding
inner and middle endpoints is strictly less than `0.00001F`, center is assigned
the inner endpoint. Input classification later in the same substep observes
the widened center. Candidate exposure is unchanged because it uses the
outermost interval. The mutation persists in the checker and is idempotent.
Evidence: `claim.judgement.tap-center-window-adjustment`; reconstruction:
`apply_tap_center_window_adjustment`.

After the classifier produces its provisional tier, active result-control state
can replace that tier with 0 before detailed-code conversion. This override is
owned by a `SkillChangeJudgeResultData` control unit and is part of TAP outcome
behavior. Evidence: `claim.judgement.active-tier-zeroing`.

## CHR / CharaTap variant

The `CHR` command maps to parsed type 4 and constructs RTTI-identified
`projView::CharaTapNote`. Its gameplay slots reuse the TAP record loader,
candidate builder/reduction, rising-edge equality gate, window classifier,
forced branch, result wrapper, terminal predicate, deferred transition, and
manager removal described above. Type-4 external configuration and adjacent-
record participation can still supply different parameter values.

The common finalizer selects source category 1 for CharaTapNote's fixed variant
flags, while ordinary TapNote selects source category 0. The shared map converts
these to authoritative categories 4 and 0 respectively. Both categories can
aggregate and notify the result observer, but remain distinct buckets for
downstream rules. Exact selection is
`chart::reconstruction::tap_variant_source_category`. Evidence:
`claim.note.chara-tap-result-category`; tests:
`tests/shared_result_test.cpp` and the TAP suites.
