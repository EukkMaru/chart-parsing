# Session 2026-07-21: ordinary AirLadder judgement

- Investigator: codex-root
- Ghidra writer: none (MCP unavailable)
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`,
  `note.other_variants`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Trace corpus-dominant ALD/type 9 through parser chains and its concrete runtime
object, while preserving any class-selection exception as a separate target.

## Findings

- Added reconstructed/high-confidence `claim.note.air-ladder-judgement` and
  `spec/notes/air_ladder.md`.
- Ordinary ALD constructs RTTI-identified AirLadderNote, creates one fixed
  profile-7 retained checker per point, exposes no candidate, and can submit
  multiple category-18 results in one substep.
- Completion waits for all checkers plus an inclusive external threshold.
- Factory inspection found the exact selector-zero/style-15 (`NON`) exception,
  which constructs HeavenHoldNote shared with parsed type 13.

## Ghidra mutations

None in the live project. GhidraMCP remained unavailable. Missing thunk
definitions were disassembled only in the temporary project clone to recover
vtable targets; no workspace copy of raw output was made.

## Validation

- Binary identity and workspace doctor rechecked; only MCP was unavailable.
- Local aggregate corpus check found 195,469 ALD records and six current-schema
  lines with the visible zero/`NON` selector/style pair.
- Workspace rebuilt successfully; all 18 CTest targets passed, including
  `chart.air_ladder_judgement`.
- Harness validation is required after the final coverage update.

## Unresolved and contradictions

- The initial handoff incorrectly called ALD parsed type 13. The established
  event-family map and factory confirm ALD is type 9; only its zero/`NON`
  variant selects the same concrete class as type 13. Durable state is corrected.
- HeavenHold start/sustain/generated behavior and its parsed `+0x158` producer
  remain active.
- Legacy ALD records with one fewer token need compatibility/default tracing.
- External timing/profile values remain parameterized.

## Handoff

Continue HeavenHoldNote at vtable `RAM:018d93e8`, load `RAM:00c15cd0`, start
`RAM:00c147e0`, sustain/generated update `RAM:00c13f90`, and terminal predicate
`RAM:00c158f0`. Trace parsed vector `+0x158` back through postprocessor
`RAM:011bda60`; then close HHD/HHX parser flags even though the corpus has none.
