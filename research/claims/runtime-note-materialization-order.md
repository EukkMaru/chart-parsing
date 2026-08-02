# Claim: parsed records are materialized after the current note-manager pass

- ID: `claim.pipeline.runtime-note-materialization-order`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.discovery`, `parser.events`, `timing.clock`, `state.ownership`, `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-27

## Statement

Gameplay setup queues every accepted parsed-record index but constructs no
runtime notes. Once per later outer update, after all input/note-manager
substeps have completed, it scans that queue in index order. An eligible record
is removed from the queue and sent through the primary factory. A new object
cannot participate until a later outer update: its first manager substep only
activates the shared state machine, and its type-specific update is first
possible on the second later manager substep. An eligible factory-default
record is consumed without creating an object.

## Anchors

- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, destructive gameplay load entry`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, parsed-count queue initialization`
- `game.exe @ RAM:00da6c40, FUN_00da6c40, pending-index append helper`
- `game.exe @ RAM:00da3a10, FUN_00da3a10, scene construction and adjacent two-byte state initialization`
- `game.exe @ RAM:00da2160, FUN_00da2160, gameplay-state entry and bypass-byte reset`
- `game.exe @ RAM:00da2f30, FUN_00da2f30, outer update and scene-exit reader`
- `game.exe @ RAM:00da9820, FUN_00da9820, outer gameplay operation order`
- `game.exe @ RAM:00da8730, FUN_00da8730, ordinary input/manager substeps`
- `game.exe @ RAM:00da70c0, FUN_00da70c0, alternate input substep`
- `game.exe @ RAM:00da62c0, FUN_00da62c0, pending-record scan, factory call, and erase/retain paths`
- `game.exe @ RAM:011c6160, FUN_011c6160, bounded parsed-record copy`
- `game.exe @ RAM:00b29c90, FUN_00b29c90, materialization eligibility predicate`
- `game.exe @ RAM:011c69a0, FUN_011c69a0, endpoint-support predicate`
- `game.exe @ RAM:011c5c50, FUN_011c5c50, endpoint selection`
- `game.exe @ RAM:011c6720, FUN_011c6720, keyed projected-position adjustment`
- `game.exe @ RAM:011c58f0, FUN_011c58f0, positive-delta projection-factor lookup`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, primary/secondary runtime factory`
- `game.exe @ RAM:018672f8, initialized 0.06F chart-unit scale`
- `game.exe @ RAM:018d3f24, initialized 30.0F near-delta threshold`
- `game.exe @ RAM:018d3f54, initialized -65.0F projection origin`
- `game.exe @ RAM:018d3f5c and RAM:018d3f3c, initialized inclusive -550.0F/550.0F bounds`

## Observations

- The gameplay loader clears parsed state before file checking. Precheck or
  tokenizer failure leaves the vector empty; successful tokenization performs
  the ordered parser/postprocessor chain and makes record rejection skip-only.
  The state-0 caller does not branch on load success.
- State 0 computes the final parsed-vector count, appends indices zero through
  count-minus-one to a scene-owned pending queue, and does not call the runtime
  factory. An empty failed load therefore creates an empty queue, not an
  immediate zero-iteration factory pass.
- The ordinary outer path performs its complete one- or two-substep
  input/manager loop before `FUN_00da62c0`; the alternate path derives input,
  calls the manager once, and reaches the same later scan. Materialization runs
  once after those substeps, not between catch-up substeps.
- The scan copies the record by pending index through a bounds-checked helper.
  An invalid index is erased. An ineligible record advances the iterator and
  remains pending. An eligible record is copied again, sent to the factory,
  and erased before scanning continues. Multiple eligible records can be
  consumed in one pass, in ascending pending-index order.
- Let raw delta be `record_start_milliseconds * 0.06F - manager_position`.
  A raw delta strictly below `30.0F` is immediately eligible. Otherwise, a
  nonnegative keyed-region index can replace the scheduled position through
  `FUN_011c6720`; this adjustment occurs after the strict raw-delta test. The
  index is the SLA-selected root or endpoint tag, and its STP/SFL/SLP producer
  is reconstructed by `claim.timing.projection-schedule-materialization`.
- For the far path, a positive adjusted delta selects a chart-owned projection
  factor from the source-order DCM schedule at the adjusted target position; a
  zero or negative adjusted delta uses `1.0F`. In source multiplication order,
  the tested position is:

  ```text
  projected = -65.0F
            - 1.5F * (projection_base_offset + adjusted_delta)
                   * runtime_speed * projection_factor
  ```

  The source enters this far path only when the ordered comparison
  `30.0F <= raw_delta` succeeds and rejects only when
  `projected < -550.0F || 550.0F < projected`. The probe is therefore eligible
  at either inclusive bound, between them, and whenever raw or projected is
  unordered by NaN. Runtime speed resets to `1.0F` and has a direct setter; the
  fixed multiplier resets to `1.5F`; the base offset is copied from caller-
  owned setup state.
- If the start probe fails, primary parsed types 1, 2, 9, 10, 12, and 13 repeat
  the same predicate using the record endpoint and its separate keyed-region
  index. Other primary types have no endpoint fallback in this gate.
- Factory primary cases are exactly 0, 1, 2, 4, 6, 9, 10, 11, and 13. Types 3,
  5, and 8 are attachment families constructed only as secondaries after a
  supported root. Region-only type 12 and every other default value create no
  primary object. The pending entry is still gone because erasure belongs to
  the caller after the factory returns.
- A factory-created primary is appended to the first active vector, followed
  immediately by an attached type-3, type-5, or type-8 secondary when present.
  Since the manager call has already finished, neither object is a candidate,
  input consumer, or result producer during the construction update. On the
  first later manager substep, primaries already enter candidate reduction but
  both primary and secondary common ticks only commit state 0 and request state
  1. Their type-specific state-1 updates are first called on the second later
  manager substep. Thus a two-substep catch-up can activate and update them in
  the next outer call, whereas a one-substep path cannot update them until a
  still later call.
- The scan is guarded by scene byte `+0x511`, but its complete static reference
  set makes the guard dormant in this snapshot. Scene construction writes the
  adjacent word at `+0x510` as value one, producing bytes `{1, 0}`; gameplay-
  state entry explicitly writes zero to `+0x511` again. The only other accesses
  are the materialization guard and a post-update scene-exit predicate. No
  instruction takes the field's address or writes it nonzero. Materialization
  therefore cannot be suppressed, nor the scene exit forced, through this
  byte on the recovered path.

## Reasoning

The state-0 queue writes and the later factory call are in separate functions.
Their order in both outer-update branches disproves the earlier immediate-build
interpretation. The independently closed common state dispatch fixes the first
possible type-specific update to the second manager substep after construction,
not merely to an unspecified point in the next outer call. The queue's erase
placement also separates eligibility from factory support: SLA can be consumed
without a runtime note, while supported roots append objects. Independent
constant reads and helper traces close the strict threshold, SLA-selected
keyed adjustment, DCM factor, endpoint fallback, projection bounds, and factory
type set. The
guard's full initializer/read/write set rules out a hidden session mode that
would invalidate this once-per-outer-update ordering.

## Alternatives and falsifiers

- Competing explanation: state 0 constructs notes immediately and the later
  queue only prepares presentation resources.
- Evidence that would disprove this claim: a state-0 call to `FUN_00b28cc0`, or
  a manager update that consumes an object before the later queue scan appends
  it.
- Competing explanation: factory-default records stay queued for a later
  supported mode.
- Evidence that would disprove this claim: an erase branch conditional on a
  nonnull factory product rather than unconditional after the factory return.
- Competing explanation: the `30.0F` comparison uses the region-adjusted delta.
- Evidence that would disprove this claim: data flow applying `FUN_011c6720`
  before the first comparison.

## Unknowns

- External `PlayOptionSpeedTable` rows and deployed `[OFFSET] DRAW` values are
  unavailable. Their complete setup producers are closed by
  `claim.configuration.runtime-materialization-input-producers`.
- Allocation failure is outside the successful finite-allocation path.
  Nonfinite arithmetic through the materialization comparisons and keyed
  schedule sort is explicitly reconstructed by the linked projection claim
  and focused tests.

## Consequences

- Ghidra mutations: the independent follow-up audit created the missed
  comparator function boundary at `RAM:011c3c60`; no semantic rename was made.
- Superseded claim: `claim.pipeline.c2s-load-failure-lifecycle`; its reset and
  teardown evidence is retained here, while its immediate-factory statement is
  withdrawn.
- Spec sections: `spec/c2s.md`, `spec/timing.md`, `spec/configuration.md`.
- Related dispatch claim: `claim.pipeline.runtime-note-dispatch`.
- Schedule producer claims:
  `claim.timing.projection-schedule-materialization` and
  `claim.parser.sla-materialization-selection`.
- Runtime-input producer claim:
  `claim.configuration.runtime-materialization-input-producers`.
- Reconstruction code: `GameplayChartLoadAttempt`,
  `runtime_materialization_probe_is_eligible`,
  `runtime_materialization_probe_from_schedule`,
  `should_materialize_runtime_record`, and
  `runtime_factory_constructs_primary` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/chart_load_lifecycle_test.cpp`,
  `tests/runtime_materialization_test.cpp`,
  `tests/projection_schedule_test.cpp`, and
  `tests/runtime_note_dispatch_test.cpp`.

## Verification

The queue initializer, both update branches, bounded record copy, retain/erase
paths, projection helper chain, initialized scalar values, complete factory
switch, active-vector append order, and full `+0x511` reference set were checked
independently. The parser
postprocessor sequence was rewalked for a record-vector reorder; it mutates
records and owned child containers but leaves the top-level record order
intact. Focused tests cover the strict near threshold, inclusive projection
bounds, endpoint fallback, positive-factor selection, primary factory set, and
the corrected empty-versus-queued load state.
