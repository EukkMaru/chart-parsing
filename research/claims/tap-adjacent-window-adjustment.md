# Claim: nearby overlapping records trim TAP timing-window sides

- ID: `claim.judgement.tap-adjacent-window-adjustment`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`, `matching.candidates`, `judgement.windows`, `note.tap`, `note.hold`, `interactions.cross_note`, `config.external`
- Last reviewed: 2026-07-20

## Statement

After successful chart parsing, a pairwise postprocessor can attach per-lane
preceding/following distances to nearby records with overlapping lane spans.
When the TAP checker is initialized, a preceding distance raises every early-
side lower endpoint and a following distance lowers every late-side upper
endpoint. Internal configured caps prevent either limit from crossing too far
through the checker's central region.

## Anchors

- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, successful-load postprocessing setup`
- `game.exe @ RAM:011bccc0, FUN_011bccc0, record selection, pairwise proximity/lane overlap, and per-lane propagation`
- `game.exe @ RAM:011cb410, FUN_011cb410, earlier-record following-distance writes`
- `game.exe @ RAM:011cb4c0, FUN_011cb4c0, later-record preceding-distance writes`
- `game.exe @ RAM:00c18800, FUN_00c18800, checker initialization and endpoint trimming`
- `game.exe @ RAM:00c19880, FUN_00c19880, aggregate eligibility-bound recomputation`
- `game.exe @ RAM:00c1da90, FUN_00c1da90, TAP load path into shared checker initialization`
- `game.exe @ RAM:00c2adc0, FUN_00c2adc0, HOLD load path into the same checker initialization`
- `game.exe @ RAM:00c183a0, FUN_00c183a0, adjusted endpoint consumer`

## Observations

- After a successful load, `FUN_00b2a8c0` builds several five-field runtime
  configuration groups and passes the parsed `0x174`-byte record vector to
  `FUN_011bccc0`. The enabled group selected by parsed type determines whether
  that record participates and supplies adjustment bounds.
- The postprocessor considers selected types 0, 1, 2, 4, 6, 11, and 13 (with a
  variant selection for types 1, 2, and 13). It rejects near-equal/nonordered
  pairs, requires their bounded lane extents to overlap, computes the positive
  separation between their scheduled positions, and applies only separations
  below the configured pair threshold.
- For every lane in the overlap, the earlier record receives a following flag
  at `+0xc4+lane` and distance at `+0x114+4*lane`; the later record receives a
  preceding flag at `+0xb4+lane` and distance at `+0xd4+4*lane`. Repeated writes
  retain a bounded selected value according to the record's runtime group. An
  optional group flag propagates the most restrictive observed adjustment
  across the record's lanes.
- TAP initialization reaches `FUN_00c18800` through `FUN_00c1da90`. The shared
  initializer first loads five lower/upper pairs plus two internal cap values
  and the side pivot from external configuration, adding a common base offset.
  HOLD record loading at `FUN_00c2adc0` reaches the same initializer for its
  start checker.
- For an enabled lane with a preceding flag, it computes
  `lower_limit = min(base - preceding_distance, early_cap, side_pivot)` and
  replaces each of the five lower endpoints with
  `max(existing_lower, lower_limit)`.
- For an enabled lane with a following flag, it computes
  `upper_limit = max(base + following_distance, late_cap, side_pivot)` and
  replaces each of the five upper endpoints with
  `min(existing_upper, upper_limit)`.
- Disabled lanes are not modified. After either side is processed, the checker
  recomputes its overall minimum lower and maximum upper eligibility bounds.
  The adjusted intervals are then consumed unchanged by the center-first,
  half-open classifier.
- The adjustment is chart-load/checker-initialization state. No per-tick TAP
  update mutates these endpoints.

## Reasoning

The parser-owned flags/distances, pair ordering, overlap range, runtime checker
copy, exact min/max instruction sequence, and classifier reads form a closed
path from cross-record proximity to judgement eligibility. Runtime configuration
selects participation and caps; unavailable values remain parameters.

## Alternatives and falsifiers

- Competing explanation: the per-lane arrays affect only rendering overlap.
- Evidence that would disprove this claim: checker construction ignoring the
  arrays, adjusted fields not being classifier endpoints, or a later reset
  restoring the base endpoints before TAP input evaluation.

## Unknowns

- Player-facing names for eligible record groups and the adjustment feature are
  not established.
- Runtime group values, pair thresholds, base offset, caps, and exact authored
  resource-version variation are externally loaded.
- Non-finite configuration/record inputs are not specified as supported.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/judgement.md`, `spec/notes/tap.md`,
  `spec/configuration.md`, `spec/matching.md`.
- Reconstruction code: `TapAdjacentWindowAdjustment` and
  `apply_tap_adjacent_window_adjustment` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/tap_window_test.cpp`.

## Verification

The successful-load caller, record filter, pair ordering, lane-overlap range,
both directional writers, cross-lane propagation, checker constructor caller,
both endpoint-trimming arms, aggregate-bound recomputation, and classifier were
traced independently. Focused tests cover directional trimming, cap/pivot
limits, classification after trimming, and disabled lanes.
