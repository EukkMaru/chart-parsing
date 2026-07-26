# Session 2026-07-20: parser-derived adjacent judgement windows

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`, `judgement.windows`, `note.tap`, `note.hold`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close the open TAP adjacent-window adjustment from chart-load state through
runtime classification, including directionality, lane overlap, bounds, reset
timing, and reuse by other note families.

## Findings

- `research/claims/tap-adjacent-window-adjustment.md`

Successful-load postprocessing attaches per-lane preceding/following distances
to selected nearby records with overlapping lane spans. Shared checker
initialization tightens all five early or late interval sides within configured
caps, recomputes overall eligibility, and supplies the adjusted endpoints to
TAP classification. HOLD start-checker construction uses the same initializer.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Temporary function
creation and decompilation occurred only in `/tmp/chart-readonly-20260720`, a
disposable clone of the project; source artifacts were not changed.

## Validation

- Binary identity and workspace health were checked at session start.
- CMake build completed.
- CTest passed 10/10 tests, including directional trimming, caps/pivot,
  classification after adjustment, and disabled-lane behavior.
- `harness.py validate` is pending the final coverage synchronization.

## Unresolved and contradictions

- Initial provisional checker/adjustment addresses were invalid function
  anchors. They were removed; the closed path uses the anchors below.
- Runtime record-group selectors, pair thresholds, common offset, caps, and
  player-facing feature identity are externally loaded and remain parameters.
- Three additional note-family load callers reuse the checker initializer; their
  family identities and downstream behavior remain to be closed vertically.
- GhidraMCP is not exposed and its configured HTTP endpoint is not listening,
  so justified names/comments could not be persisted to the live project.

## Handoff

Start at successful-load setup `RAM:00b2a8c0`, pairwise postprocessor
`RAM:011bccc0`, directional writers `RAM:011cb410`/`RAM:011cb4c0`, shared
checker initializer `RAM:00c18800`, aggregate-bound recomputation
`RAM:00c19880`, TAP load `RAM:00c1da90`, HOLD load `RAM:00c2adc0`, and
classifier `RAM:00c183a0`. Select the simplest unblocked remaining caller of
the shared checker and close it through input/result handling. Active ownership
remains with codex-root for `pipeline.boundaries`, `judgement.types`,
`note.tap`, and `note.hold`.
