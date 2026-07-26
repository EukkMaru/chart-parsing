# Claim: gameplay input framing is two TouchSlider banks plus six photo sensors

- ID: `claim.input.touch-photo-input-framing`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `input.transport`, `input.logical_state`, `input.buffering`
- Last reviewed: 2026-07-26

## Statement

Each gameplay input substep builds one zero-initialized 40-byte source record
containing a scalar time value, two ordered 16-byte TouchSlider banks, and a
six-bit photo-sensor mask. Ordinary catch-up selects TouchSlider sample offset
`-1` or `0`; the alternate path reads the current filtered sample. Both paths
pass the same record to snapshot synthesis.

## Anchors

- `game.exe @ RAM:00da8730, FUN_00da8730, ordinary source-record builder, hash ed2d269bf3ba4bfcefd8300792569c3c467773fc2b2a543636e5db55b918712a`
- `game.exe @ RAM:00da70c0, FUN_00da70c0, alternate source-record builder, hash 0959c2ccedac3b54b30f05483aab971b3b4f2c214ba70001388c319a7259880b`
- `game.exe @ RAM:00c2d7e0, FUN_00c2d7e0, 40-byte zero initialization, hash 868d2b186beefab63094e5868e1bf03999f8e0d718e474e7755f95f20cf24963`
- `game.exe @ RAM:00a7b730, FUN_00a7b730, bounded historical TouchSlider getter, hash dd5f1a5b31e99fbaf4f16a91df5d2d4452d99301bf5322305e01d3c80c056290`
- `game.exe @ RAM:00a7fdc0, FUN_00a7fdc0, historical sample indexing, hash 730d0ba9f7076e3ebf6a4344be60bcb21cd5f0deffaaf8c9918bbe1e34ee596e`
- `game.exe @ RAM:00a7b840 and RAM:00a7fe50, current filtered TouchSlider getter, hash d17f2e044964389d07c12c29d0bee8820932341a948098162b386e18129e792f / ccb4fb2a0b268bcc1e06f83594c8ee7dc2767d5cf87f70ed850d98015ccde54e`
- `game.exe @ RAM:00a80240, FUN_00a80240, TouchSlider sample filtering and history ownership, hash 21334e295d3606f56aeceff0ff7a8c80c6767915b51a937ade61d1735cbd0436`
- `game.exe @ RAM:00a7d350, FUN_00a7d350, photo-sensor resource selection, hash 2b816a6bee369224bde317e2aa73d42389ddb793548cc91b5f2043db883957c6`
- `game.exe @ RAM:00a7d170, FUN_00a7d170, photo-sensor state query, hash c80f6adc0c973410b4662fcf2357a2ee7b563741ab7cb8dc4bc98cc47f59598f`

## Observations

- Both gameplay builders clear ten dwords, store the substep scalar in the
  first dword, fill bytes `+0x04..+0x13` from TouchSlider Y selector 0 and
  bytes `+0x14..+0x23` from selector 1, then populate bits 0 through 5 of the
  final dword before calling the sole snapshot synthesizer.
- TouchSlider X selectors run from 0 through 15 in gameplay order. The
  interface storage maps them through `(15 - x) * 2 + y`; this reversal is
  internal to the getter and does not reverse the gameplay record's lane
  order. A returned byte is later interpreted only by its zero/nonzero state.
- The ordinary getter accepts only history offsets `-1` and `0` and selects
  adjacent 32-byte filtered samples. The ordinary catch-up loop supplies
  exactly those values. The alternate getter selects the current filtered
  32-byte state without a history offset.
- The TouchSlider updater applies per-source byte thresholds before storing
  the current and adjacent historical samples used by these getters.
- The six mask queries select the locally named resources `photo_sensor_6`
  through `photo_sensor_1` in that order. Therefore source bit 0 corresponds
  to `photo_sensor_6` and bit 5 to `photo_sensor_1`. Their result is normalized
  to the mask's asserted/clear distinction.

## Reasoning

The builders' fixed offsets establish the framing independently of the later
bitset code. The interface assertion identity, X/Y bounds, and storage formula
connect both 16-byte regions to one TouchSlider surface with two Y selectors.
The six resource-selection cases and fixed builder loop establish the photo
mask order. Both complete builders converge on `FUN_00c2dfa0`, so there is no
second gameplay framing interpretation.

## Alternatives and falsifiers

- Competing explanation: the two banks are unrelated controllers, or the six
  high bits are another TouchSlider region.
- Evidence that would disprove this claim: another writer to the record before
  synthesis, a gameplay path passing a TouchSlider history offset outside
  `-1/0`, or a photo-mask bit sourced from a different resource family.

## Unknowns

- The hardware/backend protocol beneath the named TouchSlider and photo-sensor
  interfaces is outside the gameplay-facing boundary and remains unresolved.
- Per-source threshold values and device-resource bindings are externally
  loaded and unavailable. The executable's selection and framing logic is
  recovered; those values remain parameters.

## Consequences

- Ghidra mutations: concise comments at the two source builders and interface
  getters.
- Spec sections: `spec/input.md`, `spec/configuration.md`.
- Reconstruction code: `InputSynthesisSnapshot` input fields and
  `synthesize_input_snapshot` in `include/chart/reconstruction.hpp`.
- Tests: `tests/input_profile_test.cpp`.

## Verification

The ordinary and alternate builders were checked separately through their
TouchSlider and photo-sensor callees and then forward to their common
synthesizer. Focused tests cover the two-bank fold and nonzero-byte behavior.
