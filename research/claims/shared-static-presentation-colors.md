# Claim: Shared presentation vertex colors are startup constants

- ID: `claim.presentation.shared-static-colors`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.hold`, `render.slide`, `render.heaven_hold`,
  `render.air_slide`, `render.air_ladder`, `render.air_solid`,
  `config.external_presentation`, `audit.binary_saturation`
- Last reviewed: 2026-08-10

## Statement

The packed colors read by the shared sustain/path geometry are not runtime
configuration. Three startup initializers use one four-byte channel writer to
construct exact constants: base white `0xffffffff`, low-alpha white
`0x40ffffff`, and alternate gray `0xff666666`.

## Anchors

- `game.exe @ RAM:004cf1f0, FUN_004cf1f0, base-white initializer, hash e2ed6bf85587c181e62d17809e5028c1da3dfb9826b2f2aa7f406610c819d70f`
- `game.exe @ RAM:004cf220, FUN_004cf220, low-alpha-white initializer, hash e37a0573f190a418faec35cf09652e0a1a90efc21a39316e14cdab2808c8d520`
- `game.exe @ RAM:004cf1d0, FUN_004cf1d0, alternate-gray initializer, hash 822a70948ead3fb5d5cfc994812681591bbb277d0186b8dfa7db94bfec99b817`
- `game.exe @ RAM:005fb980, FUN_005fb980, four-channel byte writer, hash ce95e7278600f32356052a9221beb32ac757111e9566bd33e9036e374ccc1f94`
- `game.exe @ RAM:016f1bf8..016f1c00, initializer pointer-table entries`
- `game.exe @ RAM:01c7abf0/+4/+8, initialized packed-color globals`
- `game.exe @ RAM:00c03c00, shared AirSlide/AirLadder geometry consumer, hash d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`
- `game.exe @ RAM:00c05ac0, AirSolid consumer, hash 0ef38695a6ee89607db46366d486fece940ff86a63b81e29c1793d530fe096bb`
- `game.exe @ RAM:00c08420 / 00c09d60 / 00c0a3d0, HeavenHold, Hold, and Slide consumers`

## Observations

- The writer stores its third, second, first, and fourth byte arguments at
  destination offsets zero through three. The three initializer calls pass
  `(255,255,255,255)`, `(255,255,255,64)`, and `(102,102,102,255)` and write
  globals `01c7abf0`, `01c7abf4`, and `01c7abf8`, respectively.
- Direct xrefs close every read of those globals. The base value is read by the
  shared AirSlide/AirLadder builder, AirSolid, the Field mesh, HeavenHold,
  Hold, and Slide. Only the shared builder reads low-alpha white. The shared
  builder, HeavenHold, Hold, and Slide read alternate gray.
- The Field read does not establish runtime reachability. Its sole recovered
  allocation root `00b28c10` constructs the RTTI-identified
  `projView::Field`, calls its vtable setup, and has no caller, data xref, or
  raw absolute-pointer reference. The reachable note families remain the
  normative consumers; Field is a documented exclusion unless a concrete
  indirect producer is recovered.
- Shared Air path streams zero and one use base white in modes zero and one
  and alternate gray in mode two. Stream two always uses low-alpha white.
  AirLadder calls the builder in mode zero. AirSlide selects modes one and two
  for path phases three and four, otherwise mode zero.
- Hold and HeavenHold use base white in modes zero and one and alternate gray
  in mode two. AirSolid always uses base white. Slide modes zero and one use
  base white and mode two uses alternate gray; its mode-one overlay separately
  uses the immediate constant `0x20ffffff`.

## Reasoning

The initializer table proves startup reachability, the initializer arguments
and byte writer prove exact packed values, and the complete data-xref set ties
each value to its family and mode consumer. No asset or footage inference is
needed.

## Unknowns

- External resources still determine texture/material/shader behavior and
  final pixels. They do not determine these vertex colors.
- The unreferenced Field class is recorded to close the data-xref set, not as a
  factory-reachable gameplay component.

## Consequences

- Ghidra mutations: after successful dry-runs, created the previously missed
  default-named functions at `004cf1d0`, `004cf1f0`, and `004cf220`; added
  supported plate comments to those initializers and `005fb980`. Added
  exclusion comments at the Field allocation root `00b28c10`, setup
  `00c2c6d0`, and mesh builder `00c07690`. No symbols were renamed and no types
  were applied.
- Reconstruction: shared static-color constants and family/mode selectors in
  `include/chart/reconstruction.hpp`.
- Tests: Hold, HeavenHold, Slide, AirSolid, AirSlide, and AirLadder focused
  presentation tests assert the exact values and selectors.

## Verification

Raw address-byte search found the initializer references missed by the prior
ordinary xref search. All reads of `01c7abf0`, `01c7abf4`, and `01c7abf8`
were enumerated and mapped to their containing functions. The exact values,
mode selection, and family call-site modes are represented in focused tests.
