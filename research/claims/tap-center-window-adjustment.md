# Claim: TAP widens its center window after candidate exposure

- ID: `claim.judgement.tap-center-window-adjustment`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `matching.candidates`, `judgement.windows`, `note.tap`, `config.external`
- Last reviewed: 2026-07-20

## Statement

On each TAP manager substep, candidate exposure precedes an enabled-lane
mutation that independently widens either center endpoint to the corresponding
inner endpoint when the inner and middle endpoints differ by less than
`0.00001F`; later input classification in that substep observes the mutation,
while candidate eligibility does not.

## Anchors

- `game.exe @ RAM:00b2b690, FUN_00b2b690, candidate/mutation/update virtual-call order`
- `game.exe @ RAM:00c1d4e0, FUN_00c1d4e0, TAP candidate wrapper`
- `game.exe @ RAM:00c1aa90, FUN_00c1aa90, outermost-window candidate exposure`
- `game.exe @ RAM:00c1d500, FUN_00c1d500, TAP checker-mutation thunk`
- `game.exe @ RAM:00c18130, shared checker mutation body`
- `game.exe @ RAM:00c183a0, FUN_00c183a0, later center-first classifier`
- `game.exe @ RAM:018690e4, initialized comparison epsilon`

## Observations

- The active-note manager invokes each primary note's candidate slot, reduces
  lane candidates, then invokes its checker-mutation slot before the note
  update that can classify input.
- TAP's candidate wrapper clears its slots and delegates to a builder that
  tests aggregate and per-lane outermost bounds. It does not read the center,
  inner, or middle endpoint pairs.
- TAP's mutation slot delegates to the embedded checker. For every enabled
  `0x38`-byte lane record, the checker compares inner lower with middle lower
  and inner upper with middle upper. Each absolute difference strictly below
  the initialized `0.00001F` global copies that inner endpoint to center.
- The lower and upper tests are independent. Disabled records are skipped.
- The input path later submits current-minus-scheduled timing to the same
  lane record's center-first classifier.
- The mutation writes the lane record in place. Repetition is idempotent after
  an endpoint has been copied.

## Reasoning

The manager call order closes the same-substep data flow: candidate selection
uses only the wider outermost bounds, the mutation then changes center, and
input classification consumes the changed record. Because no candidate field
depends on center, widening can change an accepted fine-result index to center
without changing whether or when the TAP was exposed for lane reduction.

## Alternatives and falsifiers

- Competing explanation: the mutation is initialization-only or visual state.
- Evidence that would disprove this claim: an update path classifying a copy of
  the pre-mutation checker, or a candidate path reading the center endpoints.

## Unknowns

- The compiler/runtime purpose of the shared global outside gameplay is not
  assigned; only its initialized value and these gameplay reads are normative.
- Player-facing names for the result indices changed by this widening remain
  unresolved.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Spec sections: `spec/judgement.md`, `spec/configuration.md`,
  `spec/notes/tap.md`.
- Reconstruction code: `tap_center_endpoint_epsilon` and
  `apply_tap_center_window_adjustment` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/tap_window_test.cpp` covers both endpoints, independent
  mutation, strict epsilon equality, disabled lanes, changed classification,
  and unchanged outermost bounds.

## Verification

TAP vtable-slot resolution, the manager's indirect-call order, direct checker
field accesses, and the later classifier were audited independently. The FLK
wrapper reaches the same checker mutation only in its awaiting-edge phase,
consistent with `claim.note.flick-motion-judgement`.
