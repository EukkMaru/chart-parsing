# Claim: a completed TAP can seed one following candidate reduction

- ID: `claim.matching.tap-deferred-terminal-candidate`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `matching.priority`, `judgement.miss`, `note.tap`, `state.ownership`, `interactions.cross_note`
- Last reviewed: 2026-07-20

## Statement

On the ordinary active-note path, a TAP result requests terminal state without
changing current state immediately. If that TAP remains timing-eligible in the
next manager substep, its scheduled value participates in that substep's lane
candidate reduction before the pending transition is committed; the note then
enters terminal state and is removed after the update pass.

## Anchors

- `game.exe @ RAM:00c1d510, FUN_00c1d510, TAP result-wrapper arguments`
- `game.exe @ RAM:00c1cce0, FUN_00c1cce0, shared-result argument forwarding`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, state-2 request on active state`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, candidate-update-removal ordering`
- `game.exe @ RAM:00c1d4e0, FUN_00c1d4e0, unconditional TAP candidate preparation`
- `game.exe @ RAM:00c1aa90, FUN_00c1aa90, timing-eligible candidate exposure`
- `game.exe @ RAM:00c10f40, FUN_00c10f40, per-note tick ordering`
- `game.exe @ RAM:00c0dd10, FUN_00c0dd10, pending-state commit`
- `game.exe @ RAM:00c1a650, FUN_00c1a650, current-state terminal query`
- `game.exe @ RAM:00c17210, FUN_00c17210, embedded checker construction/reset`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, stored detailed-result write`
- `game.exe @ RAM:00c1d9a0, FUN_00c1d9a0, stored-result terminal guard`
- `game.exe @ RAM:00c19af0, FUN_00c19af0, base tick-guard initialization`
- `game.exe @ RAM:00c19960, FUN_00c19960, base callback registration`

## Observations

- TAP passes a true finalize argument through its `+0x48` wrapper to the
  shared `+0x44` result handler. Instruction-level stack recovery is required
  here because the wrapper duplicates one argument and the decompiler omits
  one of the callee's nine stack arguments.
- When current state is 1, the shared handler writes 2 to requested state
  `+0x14`; it does not write current state `+0x10`.
- The manager's same-pass removal phase recognizes only current state 2, so the
  newly completed TAP remains in the active vector.
- On the next manager substep, candidate preparation and minimum reduction run
  before the note tick. TAP preparation clears and rebuilds candidates from
  time and lane windows without checking current state, requested state, or
  the stored detailed result.
- The embedded checker initializes and reloads its stored detailed result to
  zero. Successful normal, expiration, and forced paths write the converted
  detailed result before returning completion.
- Before running the judgement gate, TAP tests whether that stored signed value
  is greater than 1. If so, the callback skips judgement. Candidate preparation
  does not read this value.
- The subsequent note tick commits requested state to current state before
  state callbacks. State 2 has no update work, and the manager's removal phase
  then destroys the note.
- The generic note base initializes bytes `+0x64/+0x65` to zero. Byte `+0x65`
  gates the whole tick wrapper, but all resolved callbacks registered by the
  note base leave it untouched. A program-wide direct-offset writer audit found
  no writer applicable to the note hierarchy or active-note manager.

## Reasoning

The separate requested/current fields create a one-substep ordering effect.
Candidate selection observes the still-active object before the state machine
commits its request. This can retain the completed TAP's earlier scheduled
value as the selected lane candidate and delay a later unequal candidate for
that substep. It does not allow the completed TAP to judge again: normally the
pending base transition is committed before the state-1 callback, and the
embedded stored-result guard independently rejects an already resolved TAP.
The generic tick-guard branch is inert on the recovered note construction and
manager path because its byte remains at the constructor's zero value.

## Alternatives and falsifiers

- Competing explanation: result handling disables TAP lane candidates before
  the following reduction.
- Evidence that would disprove this claim: a result-path write clearing lane
  enable flags/candidate eligibility, a state/result guard in `FUN_00c1d4e0`
  or `FUN_00c1aa90`, or transition commit before manager candidate reduction.

## Unknowns

- The effect is conditional on the next substep still falling inside both the
  overall and per-lane candidate windows. External endpoint values remain
  unavailable.
- A computed-address or untraced external framework write to tick-guard byte
  `+0x65` would falsify the recovered-path absence result; no such producer is
  statically supported in the resolved note callback graph.
- The different candidate/update lifecycles for HOLD, Slide/HeavenHold, FLK,
  Mine, and attached Air families are closed by their family claims and
  `claim.interactions.cross-family-candidate-result-order`.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/matching.md`, `spec/notes/tap.md`.
- Reconstruction code: lane reduction, requested/current transition, and
  `tap_has_terminal_detailed_result` helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/tap_lifecycle_test.cpp`.

## Verification

The TAP result call's stack arguments were checked against the wrapper and
callee prologues, then the stored result, state request, next-substep candidate
phase, state commit, and removal query were traced as separate stages. Base
construction and every registered callback were independently inspected, and
all direct `+0x65` writers in the program were reviewed for applicability.
Focused tests preserve the phase boundary, exact stored-result predicate, and
reduction rule.
