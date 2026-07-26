# Session 2026-07-20: ordinary terminal end threshold

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: continuing `pipeline.boundaries` and
  `judgement.types`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not
  listening

## Goal

Resolve the ordinary terminal evaluator's end-selector virtual call, its
concrete option ownership, external value paths, fallbacks, and final comparison
without importing unavailable runtime table values.

## Findings

- Added reconstructed claim
  `research/claims/ordinary-terminal-end-threshold.md`.
- Instruction-level caller recovery shows the evaluator passes fixed selector
  `0x20` to a statically constructed 35-entry play-option object table.
- Entry 32's RTTI identifies
  `projView::PlayOptionOpe<projDB::PlayOptionTrackSkipID>`. Its virtual
  `+0x10` target returns the live option value from the stored value pointer.
- Track-skip zero disables the producer. ID 7 selects a current-track-derived
  value with zero on missing lookup. Other nonzero IDs map through external
  `PlayOptionTrackSkipTableRecord` field `+0x0c`, then
  `ScoreRankTableRecord` field `+0x0c`; invalid lookups return zero.
- The exact comparison is unsigned
  `aggregate_metric >= (1010000U - selected_value) + 1U`.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Missing vtable-entry thunk
functions were created only in `/tmp/chart-readonly-20260720` to resolve their
existing jump targets; source artifacts were not modified.

## Validation

- `python3 scripts/harness.py validate` passed with 28 coverage rows.
- CMake build succeeded.
- CTest passed all 15 tests. `chart.shared_result` covers disabled, ordinary
  mapped, special-present, and special-absent boundaries.
- Caller registers/stack, fixed selector, option-table construction, RTTI,
  vtable target, both external-table RTTI types, current-track path, fallbacks,
  and final instructions were inspected independently.

## Unresolved and contradictions

- External track-skip, score-rank, and current-track values and player-facing
  labels are unavailable and remain parameters.
- The aggregate metric and special current-track field keep structural names.
- Separate mode/state gates around this producer are recovered as exclusions
  but their player-facing identities are not yet assigned.
- The prior arbitrary-selector hypothesis was contradicted by fixed selector
  `0x20` and concrete entry-32 RTTI.

## Handoff

The indirect selector is closed. Resume its evidence at caller
`RAM:00b92a62`, dispatch `RAM:00b61ad0`, table construction `RAM:00b659b0`,
entry 32 at `RAM:00b6670a`, vtable `RAM:018d6810`, getter `RAM:00b61ac0`,
track-skip lookup `RAM:01147f10`, and score-rank lookup `RAM:01084650`. The
next high-value ordinary-terminal question is the configured type-3 rule
evaluator at `RAM:00b92ff0` and its population path at `RAM:00b94ac0`.
Continue ownership of `pipeline.boundaries` and `judgement.types`.
