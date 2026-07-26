# Claim: equal TAP candidates share one unconsumed edge

- ID: `claim.matching.tap-equal-candidate-fanout`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `matching.priority`, `interactions.cross_note`, `note.tap`
- Last reviewed: 2026-07-20

## Statement

Within an ordinary active-note update, a logical rising edge and the selected
per-lane candidate are not consumed after one TAP result, so every still-active
TAP whose candidate equals the same selected value can independently observe
that edge in the substep.

## Anchors

- `game.exe @ RAM:00b2b690, FUN_00b2b690, prepare-all then update-all ordering`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, read-only candidate and edge checks`
- `game.exe @ RAM:00c2de70, FUN_00c2de70, read-only snapshot bit test`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared result side effects`
- `game.exe @ RAM:00c1a650, FUN_00c1a650, terminal-state query after update loop`

## Observations

- The manager first prepares candidates for every active note and completes its
  16-lane minimum reduction. It then invokes every active note's update in a
  separate full-vector pass.
- TAP reads the manager candidate and the newest snapshot's rising bit. Its gate
  performs no write to either location.
- The shared TAP result path has no call into the input history and does not
  rewrite the manager's candidate array.
- Finished objects are detected, removed, and destroyed only after the
  full-vector note-update pass.

## Reasoning

Equal candidate values all satisfy the same equality gate. Because input and
candidate state survive the first note's result and later tied objects remain
in the update vector, the matching mechanism has fan-out rather than a
single-consumer tie break. Each note still applies its own lane extent, checker,
mode branches, and terminal state.

## Alternatives and falsifiers

- Competing explanation: result propagation consumes input through an indirect
  callback not represented in its direct callees.
- Evidence that would disprove this claim: a resolved indirect result callback
  clearing snapshot `+0x34`, changing the manager candidate, or skipping later
  equal candidates in the active-vector pass.

## Unknowns

- Cross-family equal-time behavior can differ through virtual update paths and
  remains open.
- A result-owner terminal flag does not stop later TAP input acceptance or note
  finalization, but it observer-routes later result events instead of aggregating
  them. That separate order effect is covered by
  `claim.interactions.result-terminal-short-circuit`.
- Stable ordering among unequal notes with identical storage order is
  irrelevant to this input fan-out rule but matters for result routing.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/matching.md`, `spec/notes/tap.md`.
- Reconstruction code: none; no consumption primitive is required for TAP.
- Tests: result-route ordering is covered by `tests/shared_result_test.cpp`; a
  full reconstructed manager fan-out harness remains pending.

## Verification

The candidate reduction, full update pass, input helper, TAP gate, result
handler callees, terminal observer branch, and deferred removal pass were
audited independently. Other note-family virtual paths remain a required
contradiction check.
