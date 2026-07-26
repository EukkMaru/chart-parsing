# Session 2026-07-20: TAP closure audit

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: `note.tap` (released mapped); continuing ownership of
  `pipeline.boundaries` and `judgement.types`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not
  listening

## Goal

Contradiction-audit the remaining TAP construction, checker mutation, result
guard, base tick guard, and next-substep lifetime questions before releasing
the note-type slice.

## Findings

- Added reconstructed claim
  `research/claims/tap-center-window-adjustment.md`.
- Closed TAP scheduled-position conversion as a direct float multiplication by
  initialized `0.06F`; there is no intervening integer rounding step.
- Reconstructed the per-substep, per-endpoint center widening with exact
  executable-owned strict epsilon `0.00001F`. Candidate exposure precedes the
  mutation and reads only outermost bounds, while the later classifier observes
  the mutated center.
- Corrected the apparent note `+0x104` phase field to embedded checker `+0x1c`,
  which stores the detailed result. TAP skips judgement when that signed result
  is greater than one.
- Audited generic note byte `+0x65`, which gates the tick wrapper. Construction
  clears it, all registered callback targets leave it untouched, and a program-
  wide direct-writer review found no writer applicable to the note hierarchy.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Analysis used the
temporary project clone at `/tmp/chart-readonly-20260720`; source artifacts were
not modified.

## Validation

- `python3 scripts/harness.py validate` passed with 28 coverage rows.
- CMake build succeeded.
- CTest passed all 15 tests, including expanded TAP window, lifecycle, and
  gameplay-clock cases.
- Manager order, TAP vtable slots, checker field accesses, stored-result
  initialization/write/read, base callbacks, and direct tick-guard writers were
  inspected independently.

## Unresolved and contradictions

- Player-facing result names remain unsupported and were not guessed.
- External timing-window values remain unavailable and parameterized.
- A computed-address or untraced framework write to note byte `+0x65` would
  falsify the direct-writer absence result; none is statically supported.
- No contradiction remained in the recovered TAP path. The coverage row is
  released as mapped, not claimed as grand-goal closure.

## Handoff

Resume TAP mutation at manager `RAM:00b2b690`, candidate wrapper
`RAM:00c1d4e0`, checker mutation thunk `RAM:00c1d500`, shared body
`RAM:00c18130`, and classifier `RAM:00c183a0`. Resume stored-result lifetime at
checker construction `RAM:00c17210`, write `RAM:00c1af30`, guard
`RAM:00c1d9a0`, and base tick wrapper `RAM:00c10f40`. `note.tap` is mapped and
released. Continue only `pipeline.boundaries` and `judgement.types` ownership.
