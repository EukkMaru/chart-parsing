# Claim: TAP lane windows use center-first half-open classification

- ID: `claim.judgement.tap-window-classification`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `judgement.types`, `judgement.windows`, `config.external`, `note.tap`
- Last reviewed: 2026-07-20

## Statement

An enabled TAP lane checker classifies a timing delta by testing five configured
half-open intervals from center to outermost, then splits non-center bands at a
configured pivot into distinct early- and late-side internal results.

## Anchors

- `game.exe @ RAM:00c183a0, FUN_00c183a0, five-band classifier`
- `game.exe @ RAM:00c17e00, FUN_00c17e00, lane-record selection`
- `game.exe @ RAM:01bdd66c, static fine-result metadata table`
- `game.exe @ RAM:00c194d0, FUN_00c194d0, acceptance-byte consumer`
- `game.exe @ RAM:00c196f0, FUN_00c196f0, coarse-to-detailed result mapping`
- `game.exe @ RAM:00c18800, FUN_00c18800, external window selection and initialization`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, JudgeTiming.ini load path`

## Observations

- A lane record is `0x38` bytes and begins with an enabled byte. It contains five
  lower/upper endpoint pairs and a side pivot.
- Each containment test is lower-inclusive and upper-exclusive. Test order is
  center, inner, middle, outer, then outermost.
- Disabled lanes return internal index 0. Center returns 6. From inner through
  outermost, deltas below the pivot return 5, 4, 3, 2 respectively, while other
  deltas return 7, 8, 9, 10. Outside every interval returns 0 below the pivot
  and 11 otherwise.
- Internal indices address a 12-entry static table. Indices 2 through 10 have
  acceptance byte 1; index 11 has acceptance byte 2; indices 0 and 1 have zero.
- The table's coarse tiers for indices 2 through 10 are
  `0,1,2,3,4,3,2,1,0`; its side codes are
  `1,1,1,1,0,2,2,2,2`.
- Coarse-to-detailed mapping returns code 11 for tier 0; codes 3/9, 4/8, and
  5/7 for early/other sides of tiers 1, 2, and 3; and code 6 for tier 4.
- Window endpoints are filled from an external configuration record selected
  by the exact parsed-type/extended-form table reconstructed in
  `claim.note.hold-extended-profile-selection`: TAP selects record 0, FLK
  selects 6, CHR and extended HOLD/Slide/HeavenHold select 4, MNE selects 11,
  and other supported ordinary forms select 0. The executable contains a
  `JudgeTiming.ini` load path, but this workspace does not contain the record
  values.
- Before runtime classification, chart postprocessing can trim the early and
  late sides per lane using distances to nearby overlapping records. This
  initialization-time dependency is reconstructed separately by
  `claim.judgement.tap-adjacent-window-adjustment`.
- During each manager substep, TAP also widens either center endpoint to the
  corresponding inner endpoint when inner and middle differ by less than the
  executable-owned `0.00001F` epsilon. Candidate exposure precedes this
  mutation and input classification follows it. This runtime dependency is
  reconstructed separately by
  `claim.judgement.tap-center-window-adjustment`.

## Reasoning

The classifier, table lookup, acceptance consumer, and detailed-result mapper
close the boundary semantics without requiring names for the result codes. The
configuration initializer proves the endpoints are parameters rather than
constants that should be guessed from common rhythm-game behavior.

## Alternatives and falsifiers

- Competing explanation: the endpoint pairs are scores or animation ranges,
  not timing windows.
- Evidence that would disprove this claim: a caller passing a quantity other
  than current-minus-scheduled time, or a configuration consumer swapping the
  endpoint order before classification.

## Unknowns

- Player-facing names for internal, coarse, detailed, and table codes remain
  unassigned.
- Exact endpoint, pivot, adjacent-cap/threshold values, units, and any resource
  version variation remain unavailable. Record selection itself is closed.
- NaN/non-finite configuration behavior is not specified as a supported input.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `classify_tap_delta`, `tap_fine_results`, and
  `tap_detailed_result_code` in `include/chart/reconstruction.hpp`; runtime
  center mutation is reconstructed by `apply_tap_center_window_adjustment`.
- Tests: `tests/tap_window_test.cpp` covers inclusivity, precedence, result
  metadata, detailed-code mapping, and the center-window mutation.

## Verification

Focused clean-room tests cover every interval transition, disabled behavior,
center-first overlap precedence, both sides, the static metadata table, and the
coarse-to-detailed mapping. The focused mutation tests cover strict epsilon,
independent endpoints, disabled lanes, and changed center classification.
Runtime configuration values remain externally unavailable.
