# Session 2026-07-20: FLK edge and lateral-motion judgement

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`, `note.flick`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Select the simplest remaining shared-checker caller and close one vertical note
slice from parsed type through construction, candidate/input rules, direction
tracking, result submission, terminal state, forced mode, and cleanup.

## Findings

- `research/claims/flick-motion-judgement.md`

Parsed type 6 was connected through factory and RTTI to `projView::FlickNote`.
Its ordinary path uses an adjusted TAP-style checker for the initiating edge,
a second checker for its motion deadline/completion classification, and
union/per-bank held-source centroids for lateral distance. The exact phase,
boundary continuation, timeout, result-composition, forced alternation, and
cleanup rules are reconstructed. A cross-family asymmetry was recorded: FLK
contributes candidates but its own edge path does not compare the manager's
selected candidate.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Missing thunk functions,
computed phase targets, and decompilation were created only in
`/tmp/chart-readonly-20260720`, a disposable clone of the project; source
artifacts were not changed.

## Validation

- Factory case, RTTI, all vtable targets, manager call order, both checker
  initializers, input readers, result wrapper, terminal table, and cleanup were
  independently traced.
- The shared forced-direction flag was searched across all program functions;
  only FlickNote construction and its forced branch access it.
- CMake build completed.
- CTest passed 11/11 tests, including the new FLK center-window, motion,
  boundary, phase, result, metadata, and forced-direction cases.
- `harness.py validate` is pending final coverage synchronization.

## Unresolved and contradictions

- The initial decompiler rendering of the left boundary sentinel was wrong;
  direct instruction inspection established float `-0.5`, paired with `16.5`.
- Exact primary/secondary windows, observation span, distance, forced timing,
  and player-facing names are externally loaded and remain parameters.
- FLK's candidate contribution without selected-candidate equality needs a
  cross-family order/tie audit against TAP and HOLD.
- Active result-control behavior beyond the reconstructed tier-zeroing unit is
  not closed for the FLK composition context.
- GhidraMCP is not exposed and its configured HTTP endpoint is not listening,
  so justified names/comments could not be persisted to the live project.

## Handoff

Use factory `RAM:00b28cc0`, FlickNote constructor `RAM:00c1fae0`, vtable
`RAM:018d98f0`, RTTI descriptor `RAM:01c32648`, load `RAM:00c200a0`, primary
checker `RAM:00c18800`, secondary checker `RAM:00c19190`, candidate functions
`RAM:00c1fd80`/`RAM:00c1fdb0`, state machine `RAM:00c20340`, terminal lookup
`RAM:00c1fee0`, and result wrapper `RAM:00c20050`. The next likely-simple
factory case is parsed type 4 at constructor `RAM:00c2b4f0`. Active ownership
remains with codex-root for `pipeline.boundaries`, `judgement.types`,
`note.tap`, and `note.hold`; `note.flick` is released as mapped.
