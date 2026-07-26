# Session 2026-07-20: MNE held-contact judgement

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`, `note.other_variants`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close parsed type 11 (`MNE`) through runtime construction, candidate behavior,
input, timing integration, terminal result, forced mode, shared outcome routing,
and object lifetime.

## Findings

- `research/claims/mine-contact-judgement.md`

Type 11 constructs RTTI-identified `projView::MineNote`. It exposes no lane
candidate, reads folded logical held levels, and credits a clipped timing
interval when any covered lane is held. Exact strict/equality boundaries decide
whether its external requirement has been reached or has become unreachable.
Both terminal paths use source category 17, which maps to category 11 and
cannot enter the authoritative aggregate.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Function creation,
instruction inspection, and decompilation occurred only in
`/tmp/chart-readonly-20260720`, a disposable project clone; source artifacts
were not changed.

## Validation

- The family lookup, factory, constructor, RTTI, complete vtable, load/reset,
  ordinary and forced updates, direct floating-point branches, logical input
  accessor, result/category path, generic state request/commit, manager removal,
  destructor, and resource siblings were checked independently.
- `MinePhase`, `MineContactState`, and the Mine helpers reconstruct the finite
  gameplay state machine in `include/chart/reconstruction.hpp`.
- `tests/mine_contact_test.cpp` covers interval clipping, gates, strict
  impossibility, equality, forced resolution, candidate absence, and routing.
- CMake build and all 12 CTest cases pass.
- `harness.py validate` passes with 28 coverage rows and 10 required files.

## Unresolved and contradictions

- External timing values and the selected runtime correction remain parameters;
  no absent constants were guessed.
- The chart postprocessor can include parsed type 11 in nearby-record work, but
  Mine's runtime path has no TAP checker, emits no candidates, and does not read
  those distances for its held-contact result.
- Player-facing names for phases, result bytes, and categories remain open.
- GhidraMCP is not exposed and its configured HTTP endpoint is not listening,
  so justified names/comments could not be persisted to the live project.

## Handoff

Use family lookup `RAM:011cc970`, factory `RAM:00b28cc0`, MineNote constructor
`RAM:00c1ec90`, vtable `RAM:018d9878`, RTTI descriptor `RAM:01c32620`, load
`RAM:00c1f940`, update `RAM:00c1ee00`, terminal lookup `RAM:00c1f690`, Mine
result wrapper `RAM:00c1f8f0`, shared handler `RAM:00c1c340`, and manager update
`RAM:00b2b690`. Type 3 has no standalone main-factory case; next trace its
parser ownership into secondary constructors `RAM:00c1dc50`, `RAM:00c21020`,
and `RAM:00c24d40`. `note.other_variants` is released mapped; `note.air` is now
owned by codex-root. Existing ownership of `pipeline.boundaries`,
`judgement.types`, `note.tap`, and `note.hold` remains active.
