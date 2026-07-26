# Claim: normal TAP judgement consumes a derived logical rising edge

- ID: `claim.input.tap-rising-edge-snapshot`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `input.logical_state`, `input.buffering`, `matching.priority`, `note.tap`
- Last reviewed: 2026-07-26

## Statement

The normal TAP gate reads the newest processed snapshot's 16-lane rising-edge
mask, formed by detecting edges independently in two 16-source banks and then
OR-folding corresponding sources into logical lanes.

## Anchors

- `game.exe @ RAM:00c2dfa0, FUN_00c2dfa0, source processing and snapshot append`
- `game.exe @ RAM:00c2f020, FUN_00c2f020, 0x58-byte snapshot history append`
- `game.exe @ RAM:00c2db20, FUN_00c2db20, history selector lookup`
- `game.exe @ RAM:00c2de10, FUN_00c2de10, logical level at snapshot +0x30`
- `game.exe @ RAM:00c2de70, FUN_00c2de70, logical rising edge at snapshot +0x34`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, TAP consumer with selector 0`
- `game.exe @ RAM:00da8730, FUN_00da8730, input-before-note update ordering`

## Observations

- The input processor receives 32 source bytes organized as two 16-source
  banks. A source byte is asserted when nonzero.
- It stores the 32 current source levels at snapshot `+0x28`. For each source,
  the bit at snapshot `+0x2c` is set only when the current source is asserted and
  the prior snapshot's corresponding `+0x28` bit was clear.
- It ORs the two banks into a 16-bit logical level at `+0x30` and independently
  ORs their source-edge masks into a 16-bit logical rising mask at `+0x34`.
- The completed `0x58`-byte snapshot is appended to bounded history. Selector 0
  resolves the newest snapshot when history is nonempty.
- The TAP lane helper tests `+0x34`, not `+0x30`, and the ordinary gameplay
  update builds the input snapshot before updating active notes.

## Reasoning

The prior/current source comparison proves edge semantics. Separate source-bank
edge construction before logical folding also means a newly asserted source can
produce a logical edge while its counterpart in the other bank is already held.
The selector and update caller connect that derived edge to the TAP gate in the
same gameplay substep.

## Alternatives and falsifiers

- Competing explanation: snapshot `+0x34` is a held-level mask.
- Evidence that would disprove this claim: a path populating `+0x34` without the
  prior-clear condition, or a TAP caller selecting a different snapshot field.

## Unknowns

- `claim.input.touch-photo-input-framing` closes both banks as the two
  TouchSlider Y selectors and `claim.input.snapshot-profile-synthesis` closes
  the remaining snapshot/history fields.
- The manager forced-result branch can bypass ordinary rising-edge acceptance;
  its selector is covered by `claim.judgement.forced-result-mode`, while its
  external activation identity remains open.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/input.md`, `spec/notes/tap.md`, `spec/matching.md`.
- Reconstruction code: `derive_tap_lane_input` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/tap_input_test.cpp`.

## Verification

Focused tests cover both banks, held input, release, non-boolean source bytes,
lane endpoints, and the per-source-before-folding edge invariant. The ordinary
update caller independently confirms input processing precedes note judgement.
