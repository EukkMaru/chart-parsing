# HOLD note

## Construction and schedule

Parsed record type 1 constructs a runtime `projView::HoldNote`. Shared note
loading supplies start lane, table-decoded width, bounded coverage in logical
lanes 0 through 15, and start time scaled by `0.06`. HOLD-specific loading also
scales record field `+0x3c` as end time, applies the same selected runtime offset
to both endpoints, and copies a vector of `0x20`-byte checkpoint records.
Evidence: `claim.note.hold-construction-start-gate`.

The object has separate start and checkpoint phases, both initialized to 0.
Phase value 4 means that component is complete. Player-facing names for other
phase values are not normative.

## Start component

Only an unresolved start component exposes candidates. Its candidate timing,
per-lane minimum selection, newest rising-edge requirement, equality with the
manager-selected lane candidate, configured classification, and late expiration
are the shared TAP rules in `spec/notes/tap.md`. Completion forwards the start
result and changes the start phase to 4. Evidence:
`claim.note.hold-construction-start-gate`.

The shared checker initializer also consumes chart-postprocessed distances to
nearby records with overlapping lanes. These can trim the early and/or late
sides of HOLD's start windows exactly as described in `spec/judgement.md`.
Evidence: `claim.judgement.tap-adjacent-window-adjustment`.

## Physical-source qualification

For each covered logical lane, maintain `armed[2]` and `active[2]`, initially
false. Let `held[b]` be current physical source-bank level and `previous[b]` be
the shared sustain marker in the selected preceding snapshot.

1. If any bank has `held[b] && previous[b]`, OR every bank's `held[b]` into its
   `armed[b]`.
2. Otherwise, OR every bank's `!held[b]` into its `armed[b]`.
3. Before adjusted start, clear both active values.
4. At or after adjusted start, set `active[b] = armed[b] && held[b]`.
5. Every active bank writes its bit to the current snapshot's shared sustain
   marker. The aggregate tracker input is true when any covered source is
   active, or when manager forced-result state forces activity.

The exact reconstruction is
`chart::reconstruction::update_hold_lane_sources`. This permits an ordinary
source only after the HOLD has observed it released, while a preceding marker
can hand an already-held source into the current substep. Evidence:
`claim.input.hold-source-continuation`; tests:
`tests/hold_sustain_test.cpp`.

## Inactive-gap tracker

All values in this section are single-precision floats. The four thresholds and
the participation floor are runtime configuration parameters; do not substitute
guessed values.

For elapsed position relative to adjusted start:

- If elapsed is strictly less than `update_gate`, return -1 without changing
  state. For this HOLD loader, `update_gate` equals the externally configured
  participation floor because the added start-checker field is explicitly
  zero.
- Otherwise clamp elapsed to `end`, latch whether activity has ever occurred,
  store the clamped current time, and replace `last_active` when activity is
  true.
- Compute `gap = current - max(last_active, participation_floor)` and retain
  `maximum_gap = max(maximum_gap, gap)`.
- If activity has ever occurred, the current index is the first configured
  threshold greater than or equal to `gap`; return 4 when no threshold matches.
  A never-active tracker also returns 4.

Checkpoint classification applies the same threshold search to `maximum_gap`.
It returns -1 while current is strictly below the participation floor. After a
checkpoint, replace `maximum_gap` with the current gap. Thus an ongoing release
spans checkpoints, but an older completed release gap does not.

The exact implementation is `chart::reconstruction::HoldGapState` and its HOLD
helpers. Evidence: `claim.note.hold-sustain-gap-lifecycle`; tests:
`tests/hold_sustain_test.cpp`.

## Checkpoints and terminal transition

Each manager substep inspects only the first checkpoint. It is due when
`checkpoint_time * 0.06 + runtime_offset <= manager_time`. A checkpoint whose
emission flag is nonzero forwards an anonymous result byte and category 3 when
its type field equals 1, otherwise category 2. Whether emitted or not, exactly
one due front record is removed, so overdue records cannot be drained in one
substep.

Threshold indices map to anonymous result bytes as follows:

| Threshold index | Result byte |
| ---: | ---: |
| 0 | 4 |
| 1 | 3 |
| 2 | 2 |
| 3 | 1 |
| -1, 4, or other | 0 |

While checkpoints remain, the checkpoint phase is 2 only when the current-gap
index is 0 and is 3 otherwise. An empty checkpoint vector sets phase 4. After
the state-1 update runs both start and sustain/checkpoint components, it requests
terminal state 2 only when both phases equal 4. Requested/current separation,
next-substep commit, and manager removal follow the base lifecycle documented
for TAP. Evidence: `claim.note.hold-sustain-gap-lifecycle` and
`claim.matching.tap-deferred-terminal-candidate`.

Before adjusted start, checkpoint phase remains 0. At or after adjusted start,
an already-empty checkpoint vector immediately reaches phase 4 without waiting
for adjusted end. If start also resolves in that update, the note can request
terminal state at that point. No clean-room validation rule may silently add a
synthetic end checkpoint.

An additional once-only path runs after adjusted end plus an external runtime
offset. Its anonymous gap grade selects an indexed feedback resource and common
resource-control parameters. It does not call the HOLD result wrapper or shared
result owner, and it does not alter either HOLD phase, the checkpoint vector, or
requested note state. The gameplay reconstruction must therefore not synthesize
an end checkpoint or result from this branch. Exact resource identity is
non-normative. Evidence: `claim.note.hold-end-feedback-boundary`.

Actual HOLD start and scheduled-checkpoint results follow the category mapping,
aggregate validation, and terminal observer routing in `spec/judgement.md`.
Terminal routing can suppress aggregation of a later checkpoint event without
changing the HOLD's local checkpoint removal and phase updates. Evidence:
`claim.judgement.shared-result-two-stage-routing` and
`claim.interactions.result-terminal-short-circuit`.

When manager forced-result state is active, the shared start gate can complete
without physical input, aggregate sustain activity is forced true, and
checkpoints/end feedback use the forced byte selector instead of gap
classification. The mode table and shared cycling semantics are normative in
`spec/judgement.md`. Evidence: `claim.judgement.forced-result-mode`.
