# Claim: configured ordinary terminal rules have five ordered conditions

- ID: `claim.judgement.ordinary-configured-terminal-rule`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `judgement.miss`,
  `state.ownership`, `interactions.cross_note`, `config.external`
- Last reviewed: 2026-07-20

## Statement

A configured type-3 terminal rule is evaluated only for aggregate event type
1. It returns separate `matched` and `terminal_requested` values. Five ordered
conditions can match:

1. When enabled, a value-floor condition requests terminal if the event's
   computed double is not above its event-derived floor. A zero floor matches
   values less than or equal to zero; a nonzero floor is
   `event_scale_units * 0.01` and matches values strictly below it. Unordered
   floating-point comparisons also match.
2. When enabled and the first result-weight bucket is nonzero, an unsigned
   event count strictly below the configured limit requests terminal.
3. When enabled, an unsigned aggregate metric greater than or equal to the
   configured threshold requests terminal.
4. A valid result band matches when both configured and event result bytes are
   below the runtime result-type count and
   `event_result <= configured_ceiling`. It requests terminal only when the
   cumulative weight for result buckets zero through the configured ceiling is
   at least its configured threshold. The band is therefore the one condition
   that can match and consume a rule without requesting termination.
5. When enabled, the owner's enabled mode-2 gauge being zero requests
   terminal.

On a result-band match, the evaluator applies the ordinary unit gauge update
before testing condition 5. On an update-eligible enabled mode-2 gauge, this
decrements a nonzero current value by one. A value of one can therefore become
zero and satisfy condition 5 in the same evaluation.

Any immediate condition sets both outputs; conditions are OR-combined and do
not clear earlier results. A matched record is later marked consumed and
counted even when `terminal_requested` is false.

## Anchors

- `game.exe @ RAM:00b92640, FUN_00b92640, type-3 loop and output consumption`
- `game.exe @ RAM:00b92ff0, FUN_00b92ff0, five-condition evaluator`
- `game.exe @ RAM:018ae278, initialized double 0.01`
- `game.exe @ RAM:00b8dbb0, FUN_00b8dbb0, event scale-unit derivation`
- `game.exe @ RAM:00c408e0, FUN_00c408e0, first result-weight bucket getter`
- `game.exe @ RAM:00c409d0, FUN_00c409d0, cumulative result-weight getter`
- `game.exe @ RAM:0108e060, FUN_0108e060, runtime result-type count`
- `game.exe @ RAM:00b95170, FUN_00b95170, matched-band gauge update`
- `game.exe @ RAM:00b94ac0, FUN_00b94ac0, type-3 population branch`
- `game.exe @ RAM:00b93d90, FUN_00b93d90, type-3 temporary construction`
- `game.exe @ RAM:00b946f0, FUN_00b946f0, type-3 vector append`
- `game.exe @ RAM:00b90c30, FUN_00b90c30, type-3 record copy`
- `game.exe @ RAM:011f8c30, FUN_011f8c30, matched-record consumed bit`

## Observations

- The caller initializes the evaluator's terminal-request byte to zero. It
  marks a record consumed and increments its applied count when the evaluator's
  return value is one, independently of that byte.
- `FUN_00b92ff0` first clears both outputs and rejects an event whose leading
  type is not one.
- The floor comparison uses initialized double `0.01`. Its zero-unit branch is
  a strict `0.0 < value` escape; its nonzero branch escapes on either
  `floor < value` or equality. Consequently NaN reaches the match path in both
  branches.
- The first-bucket condition requires its enabled byte, a nonzero value from
  the first result-weight slot, and unsigned `event_count < configured_limit`.
- The aggregate-metric condition uses inclusive unsigned
  `configured_threshold <= aggregate_metric`.
- The result band bounds-checks both bytes against the same
  `NotesJudgeResultTableRecord` count already used by shared result routing.
  `FUN_00c409d0` sums weight slots starting at zero through the configured
  ceiling before the inclusive threshold comparison.
- Every band match calls `FUN_00b95170` with amount and multiplier one. Its
  applicable mode-2 branch subtracts one from nonzero current gauge state.
- The final condition then reads the updated owner gauge and requires its own
  enabled byte, owner enable, mode exactly two, and current value zero.
- Population copies the source's type-3 subrecord into a manager-owned
  `0x60`-byte vector record. Reset destroys and clears this vector separately
  from the per-evaluation output bytes.

## Reasoning

The separate return and output byte explain why a result-band record can be
consumed without activating terminal routing. Direct call order from the band
through the gauge updater into the final condition establishes the one-event
gauge dependency. The pure reconstruction preserves this ordering while
leaving unavailable configured thresholds as inputs.

## Alternatives and falsifiers

- Competing explanation: every matched type-3 record necessarily requests
  terminal routing, or the gauge-zero condition reads pre-event state.
- Evidence that would disprove this claim: the caller ignoring the separate
  terminal byte, `FUN_00b95170` running after the zero test, or a result-band
  path that bypasses record consumption when its cumulative threshold fails.

## Unknowns

- Source-field names, configured values, units, defaults, and player-facing
  rule labels are absent. The exact consumers and comparison directions are
  normative; descriptive field names remain structural.
- The semantic identities of event count, aggregate metric, result weights,
  and event-derived scale units are not assigned beyond their traced data flow.
- Owner mode/enable setup is bounded in the existing result-owner claims; its
  player-facing mode name remains unresolved.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Spec sections: `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `OrdinaryConfiguredTerminal*` and
  `evaluate_ordinary_configured_terminal_rule` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp` covers the event-type gate, floor
  boundaries including NaN, strict/inclusive count thresholds, band-only
  match, cumulative request, and same-evaluation gauge-zero transition.

## Verification

The type-3 population/copy/reset path, caller outputs and consumption, all five
condition branches, 0.01 data value, result-count source, bucket getters,
gauge updater, and final owner reads were inspected separately. Focused tests
preserve each boundary and the only intra-rule state mutation.
