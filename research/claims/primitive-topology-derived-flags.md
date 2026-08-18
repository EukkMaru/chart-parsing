# Claim: chart primitive topology modes have exact derived flags

- ID: `claim.presentation.primitive-topology-derived-flags`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer row `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

The common topology/mode setter has exactly 28 direct thunk calls. Joint and
Sprite are its only chart-side owners. Modes 2, 3, and 4 select fixed embedded
table properties and deterministically update the stored mode, extended-mode,
two payload-property, and inverse auxiliary-property bits before finalization.
No external resource row supplies these chart-mode flags.

## Anchors

- `game.exe @ RAM:0042a89c -> RAM:006caa80, SetPrimitiveTopologyMode, complete 28-call set, hash 0dbcf3433c95bd7ed38ff2729755cb5cc185fb4da1e7242ae63e96e2b16ac0c9`
- `game.exe @ RAM:0044b2e5 -> RAM:006ca9f0, UpdatePrimitiveTopologyDerivedFlags, sole helper thunk, hash f9db3ba1d4ddf86ceb6763f5a511884bd8cbdddc462b82fdc57a468db8ce7c91`
- `game.exe @ RAM:0186b900, 62 fixed 0x20-byte topology-property rows; chart row fields at +0x08/+0x18 are mode 2 (1,1), mode 3 (1,0), mode 4 (1,0)`
- `game.exe @ RAM:006b5af0, InitializePrimitiveSubmissionPayload called on DynamicPrimitiveEntryHelper +0x20, hash dae812ae2b821e9c7bc3f6449e5a4cd642f95f8a871de316a63be8336c18e0e0`
- `game.exe @ RAM:00d7e350, BuildAndSubmitJointDynamicPrimitive, chart modes 2/3/4 copied from complete Joint descriptors`
- `game.exe @ RAM:0102e190 / RAM:0102e3f0, InitializeAirSprite and BuildAndSubmitAirSpriteQuad, chart-system mode 3 path`

## Observations

- The setter replaces only the low six bits at payload `+0x00`. A signed input
  below 33 clears payload flag `0x800`; any other input sets it.
- The derived helper indexes the stored low-six-bit mode, clamps values above
  61 to row 61, replaces payload bits `0x20` and `0x40` from row bytes
  `+0x08/+0x18`, and sets auxiliary bit `0x08` exactly when the first property
  is zero. After that helper returns, a signed input above 32 forces payload
  bit `0x20`.
- The embedded rows for all chart-reachable modes are:

| Mode | Row `+0x08` | Row `+0x18` | Payload `0x20` | Payload `0x40` | Payload `0x800` | Auxiliary `0x08` |
| ---: | ---: | ---: | --- | --- | --- | --- |
| 2 | 1 | 1 | set | set | clear | clear |
| 3 | 1 | 0 | set | clear | clear | clear |
| 4 | 1 | 0 | set | clear | clear | clear |

- Joint submission obtains the mode from its closed descriptor set: Hold,
  Slide, AirSlide/AirLadder, AirSolid, and HeavenHold use only 2/3/4. Sprite
  initializes and reapplies mode 3. These are the only chart-side functions in
  the setter's complete 28-call set.
- The other 25 calls map to the already assigned default-debug, GuiWindow/
  GuiPanel, generic `air::Primitive`, font, Surfride, SPK, SGL, Wind, Aura, and
  engine helper families. No additional runtime-note or projView owner appears.
- Finalization operates on the same payload flag word at utility `+0x80`
  (payload `+0x60`), ORing bit 0 and replacing bit 7. It preserves all topology
  bits established here.

## Reasoning

Complete setter-thunk xrefs establish ownership, while the embedded table and
helper body establish flag selection without guessing enum semantics. The
Joint descriptor inventory and Sprite call sites independently bound the
chart-mode domain to 2/3/4. Following the same flag word into finalization
proves that these properties survive submission rather than being temporary
builder state.

## Alternatives and falsifiers

- Competing explanation: topology affects only geometry assembly, or chart
  resource rows override these payload flags.
- Evidence that would disprove this claim: another chart-side setter caller,
  a chart mode outside 2/3/4, a write that replaces the listed bits between
  setter and finalizer, or an external load into the embedded property table.

## Unknowns

- Human-readable engine enum/property names are unavailable; numeric modes and
  bit effects are normative.
- Non-chart modes and their properties are outside the gameplay reconstruction
  despite residing in the same executable-owned table.
- Backend interpretation of the property bits beyond their preserved
  submission values is not assigned a guessed semantic label.

## Consequences

- Ghidra mutations: supported names/comments for submission-payload
  initialization, topology/mode selection, and derived-flag application.
- Spec sections: `spec/presentation.md`, common dynamic-primitive boundary.
- Reconstruction code: `PrimitiveTopologyModeState`, exact generic bit update,
  and chart-mode property selection in `include/chart/reconstruction.hpp`.
- Tests: modes 2/3/4, preservation/clearing of exact bits, auxiliary inverse,
  and the above-32 forced-bit boundary in
  `tests/scene_camera_presentation_test.cpp`.

## Verification

The setter's 28 calls were mapped to containing functions and reconciled with
the complete constructor/helper owner inventories. The property helper's sole
thunk, fixed table rows, clamp, threshold comparisons, and finalizer consumer
were independently inspected. Focused tests cover every chart row and the
extended-mode threshold behavior.
