# Claim: the ALD authored/generated-vector contradiction is corrected

- ID: `claim.audit.primary-ald-checkpoint-correction`
- State: superseded
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `audit.closure`
- Last reviewed: 2026-08-03

## Statement

The primary investigator reproduced and corrected the exact contradiction that
invalidated the former ALD checkpoint claim: type-9 parsing owns separate
authored-control and generated-checkpoint vectors, and ordinary AirLadder
judges the generated vector. Dependent specs, clean-room code, focused tests,
status, and viewer planning now use the corrected producer. Overall independent
closure is deliberately not reasserted by this same-investigator record.

This record is superseded by
`claim.audit.primary-ald-precalc-correction`, whose owning behavior evidence is
`claim.note.air-ladder-precalc-presentation`: this record's judgement-
cardinality correction remains valid, but it incorrectly said generated
samples fed the main type-9 geometry and retained an external terminal-
threshold premise.

## Anchors

- `game.exe @ RAM:011c8870 -> RAM:011c6a50, type-9 generated-vector production`
- `game.exe @ RAM:011caf70 and RAM:011ca7a0, distinct 0x24/0x20 append widths`
- `game.exe @ RAM:00c132f0 -> RAM:00c19180, generated-record profile-7 load`
- `game.exe @ RAM:00c11ca0 -> RAM:00c12d80, update and completion consumers`
- `game.exe @ RAM:00c12350 -> RAM:00c03c00, generated presentation consumers`

## Observations

- The former active ALD claim and post-fix closure verdict are preserved as
  superseded rather than silently rewritten.
- `claim.note.air-ladder-generated-checkpoints` records the complete corrected
  producer, cardinality, interpolation, scheduling, judgement, and
  presentation-input path with stable function hashes.
- The normative C2S, input, matching, judgement, configuration, AirLadder, and
  HeavenHold specs now cite the replacement claim.
- The reconstruction generates root-cadence samples, exact-end/overshoot
  behavior, interpolated lane/vertical/decoded-width values, and explicit
  malformed interval/span/wrap dispositions. Focused tests exercise each rule.
- Presentation stream classification remains open in the separate viewer
  ledger and does not weaken the corrected gameplay checkpoint producer.

## Reasoning

Distinct append widths and owners rule out the old single-vector explanation.
The type-9 parser regeneration call and schedule pass connect the authored
chain to generated records; AirLadder's fixed-width load loop connects those
records to checkers. Updating every dependent artifact removes the known stale
premise without claiming that the rest of the binary has received a fresh
independent contradiction audit.

## Alternatives and falsifiers

- Competing explanation: the correction only changes terminology while
  runtime cardinality remains authored-control-based.
- Evidence that would disprove this claim: any active normative artifact still
  assigning ordinary AirLadder checkers per authored control, or focused tests
  whose expected cardinality does not follow the recovered interval producer.

## Unknowns

- `audit.closure` remains investigating until a fresh independent review tests
  the corrected workspace and all completion gates. The superseded audit cannot
  be reused as that verdict.
- The later correction closes neutral stream layouts, clipping, extents,
  winding, diagnostic counter categories, endpoint SLA-key projection, embedded style
  coordinate, effect expiry, TextureTable handle selection, Joint traversal,
  and default command submission. Viewer result-state integration remains
  stage-two work under `render.air_ladder`; resource/material roles and final
  pixel compositing are unavailable external-data semantics.

## Consequences

- Ghidra mutations: none; the contradiction trace was read-only.
- Spec sections: `spec/c2s.md`, `spec/input.md`, `spec/matching.md`,
  `spec/judgement.md`, `spec/configuration.md`,
  `spec/notes/air_ladder.md`, and `spec/notes/heaven_hold.md`.
- Reconstruction code: `AirLadderPathPoint`,
  `generate_air_ladder_checkpoints`, and existing lifecycle helpers.
- Tests: `tests/air_ladder_judgement_test.cpp` plus the full suite.

## Verification

`python3 scripts/harness.py validate` passes. A full normal build and all 34
CTest targets pass after the dependent claim/spec/reconstruction/test/viewer
edits. A content-free pass over all 7,752 local charts exercises both ALD field
shapes, 18 interval values, exact final landing and overshoot, and chains up to
459 authored controls. A new independent closure verdict remains pending and
is not implied here.
