# Claim: ordinary end termination is selected by the track-skip option

- ID: `claim.judgement.ordinary-terminal-end-threshold`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `config.external`,
  `audit.indirect_calls`
- Last reviewed: 2026-07-20

## Statement

The ordinary end-threshold producer is disabled when the live
`PlayOptionTrackSkipID` value is zero. For any other value, after its separate
runtime mode/state gates pass, it selects an external integer and evaluates
the following 32-bit unsigned rule:

`aggregate_metric >= (1010000U - selected_value) + 1U`

Track-skip ID 7 selects a current-track-derived value when the current track is
present in the runtime lookup, or zero when it is absent. Every other nonzero
ID maps through `PlayOptionTrackSkipTable` field `+0x0c` to a score-rank ID,
then uses `ScoreRankTable` field `+0x0c`; an invalid lookup supplies zero. A
true comparison feeds the secondary terminal-summary producer bounded by
`claim.judgement.ordinary-terminal-summary-producers`.

## Anchors

- `game.exe @ RAM:00b92640, FUN_00b92640, fixed option selector and end comparison`
- `game.exe @ RAM:00af8200, FUN_00af8200, live play-option subobject accessor`
- `game.exe @ RAM:00b61ad0, FUN_00b61ad0, indexed option virtual dispatch`
- `game.exe @ RAM:00b659b0, FUN_00b659b0, 35-entry option-object construction`
- `game.exe @ RAM:018d6810, RTTI-backed track-skip option vtable`
- `game.exe @ RAM:00b61ac0, FUN_00b61ac0, live track-skip value getter`
- `game.exe @ RAM:01147f10, FUN_01147f10, track-skip row mapping`
- `game.exe @ RAM:01084650, FUN_01084650, score-rank row value lookup`
- `game.exe @ RAM:01964848, score-rank table vtable`
- `game.exe @ RAM:01965ce4, track-skip table vtable`
- `game.exe @ RAM:00af9610, FUN_00af9610, current-track subobject accessor`
- `game.exe @ RAM:00aff0b0, FUN_00aff0b0, current-track lookup membership`
- `game.exe @ RAM:00afaba0, FUN_00afaba0, selected current-track data lookup`
- `game.exe @ RAM:00b4e780, FUN_00b4e780, current-track profile selector`
- `game.exe @ RAM:0106e1b0, FUN_0106e1b0, current-track profile row selection`

## Observations

- Immediately before the option dispatch, the evaluator writes literal
  `0x20` to the selector object. `FUN_00af8200` supplies the live play-option
  owner, and `FUN_00b61ad0` indexes its `0x23`-entry object table by that fixed
  value before calling virtual slot `+0x10`.
- Table entry 32 is constructed with vtable `0x018d6810`. Its MSVC RTTI type is
  `projView::PlayOptionOpe<projDB::PlayOptionTrackSkipID>`. Slot `+0x10`
  resolves through a thunk to `FUN_00b61ac0`, which returns the integer at the
  object's stored live-value pointer.
- The result must be nonzero before any end-threshold work. Value 7 chooses the
  current-track branch; all other nonzero values choose the external table
  branch.
- The ordinary branch bounds-checks the track-skip ID against records of stride
  `0x24`, reads row field `+0x0c`, bounds-checks that result against records of
  stride `0x10`, and reads their field `+0x0c`. The table vtable RTTI identifies
  the sources as `PlayOptionTrackSkipTableRecord` and `ScoreRankTableRecord`.
- The special branch tests the current-track key in a runtime map. Absence
  selects zero. Presence chooses a profile-dependent field from a current-
  track data object. The exact external row value remains data, not executable
  code.
- The evaluator subtracts the selected value from literal 1,010,000, adds one,
  and compares unsigned against aggregate snapshot field `+0xf8`.
- Several independent runtime mode predicates, a prior-primary-summary test,
  and evaluator state bytes gate this calculation. Those gates do not alter
  option/value selection or the final comparison.

## Reasoning

Fixed selector recovery plus the constructed table's RTTI resolves the prior
indirect call without relying on a conventional interpretation. The two table
RTTI identities and their exact field reads establish the ordinary mapping;
the instruction-level subtract/add/unsigned-compare sequence establishes the
clean-room arithmetic even though external record values are unavailable.

## Alternatives and falsifiers

- Competing explanation: selector `0x20` denotes a runtime player index or an
  arbitrary option chosen by table data.
- Evidence that would disprove this claim: entry 32 being replaceable after
  construction, virtual slot `+0x10` resolving to a different getter, or the
  evaluator reaching the comparison when the returned option value is zero.

## Unknowns

- External track-skip rows, score-rank rows, current-track values, defaults,
  and player-facing choice labels are unavailable and remain parameters.
- The semantic name of aggregate snapshot field `+0xf8` and the special
  current-track value is not assigned. Only the exact selection and comparison
  are normative.
- The player-facing reasons for the separate runtime mode exclusions remain
  unresolved.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Spec sections: `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `ordinary_terminal_end_threshold_reached` and its
  constants in `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp` covers disabled, ordinary mapped,
  special-present, and special-absent selection.

## Verification

The caller's stack/register setup, fixed selector, option-table construction,
entry-32 RTTI, vtable target, live-value pointer, both external-table RTTI
types, current-track presence path, fallbacks, and final instructions were
inspected independently. The focused test preserves the unsigned threshold
boundary and option-7 precedence.
