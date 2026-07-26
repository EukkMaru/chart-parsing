# Claim: HOLD sustain sources require arming or a preceding sustain marker

- ID: `claim.input.hold-source-continuation`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `input.logical_state`, `input.buffering`, `note.hold`, `state.ownership`, `interactions.cross_note`
- Last reviewed: 2026-07-26

## Statement

For each covered HOLD lane and each of its two physical source banks, sustain is
active only when the current source is held and has either previously been
observed released by that HOLD or is admitted by a preceding snapshot's shared
sustain marker; active sources write the corresponding marker into the current
snapshot.

## Anchors

- `game.exe @ RAM:00c2adc0, FUN_00c2adc0, per-HOLD source-state reset`
- `game.exe @ RAM:00c29c50, FUN_00c29c50, HOLD source qualification loop`
- `game.exe @ RAM:00c2db20, FUN_00c2db20, input-history selector`
- `game.exe @ RAM:00c2ded0, FUN_00c2ded0, physical source-level read`
- `game.exe @ RAM:00c2df50, FUN_00c2df50, shared sustain-marker read`
- `game.exe @ RAM:00c2f250, FUN_00c2f250, current sustain-marker write`

## Observations

- HOLD loading clears two 32-byte arrays. Runtime indexing treats each as 16
  logical lanes times two physical source banks.
- For every covered lane, the sustain update reads current source-level bits
  from snapshot field `+0x28` and the preceding selected snapshot's bits from
  field `+0x38`.
- If any currently held bank also has a preceding `+0x38` marker, the HOLD ORs
  both banks' current held levels into their persistent armed bytes. Otherwise,
  it ORs each bank's released state into its armed byte.
- Before adjusted start, both active bytes are cleared. At and after adjusted
  start, each active byte is exactly `armed && current-held`.
- Every active source sets its own bit in the current snapshot's `+0x38` field.
  Activity from any covered source feeds the sustain gap tracker; manager
  forced-result state can also force this aggregate activity input.

## Reasoning

The persistent bytes prevent a source that was already held from becoming
ordinary sustain merely by crossing the note start. Observed release arms a
later press. The shared history bit provides continuity between substeps and,
because it lives in the input snapshot rather than the note object, can also
bridge overlapping HOLD consumers. The exact cross-bank rule is broader than
same-bank continuity: one held-and-marked bank arms every currently held bank
for that logical lane.

## Alternatives and falsifiers

- Competing explanation: snapshot `+0x38` is a passive visualization flag.
- Evidence that would disprove this claim: a current-snapshot writer that
  clears or replaces `+0x38` before the following HOLD update, or a source that
  reaches the gap tracker's active input without the reconstructed predicates
  outside the explicit forced-result branch.

## Unknowns

- `claim.input.touch-photo-input-framing` closes bank 0 and bank 1 as the two
  TouchSlider Y selectors. `claim.input.snapshot-profile-synthesis` closes
  Slide and HeavenHold as the other gameplay marker consumers and finds no
  additional writer.
- Forced-result selection is covered by
  `claim.judgement.forced-result-mode`; its player-facing identity and lifetime
  remain unresolved.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/input.md`, `spec/notes/hold.md`.
- Reconstruction code: `HoldLaneSourceState` and
  `update_hold_lane_sources` in `include/chart/reconstruction.hpp`.
- Tests: `tests/hold_sustain_test.cpp`.

## Verification

Instruction-level reads distinguish source field `+0x28` from marker field
`+0x38`; the current writer targets `+0x38`. Focused tests cover unarmed held
input, release-then-press arming, and cross-bank marker hand-off.
