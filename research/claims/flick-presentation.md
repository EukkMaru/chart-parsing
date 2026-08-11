# Claim: Flick uses one width-indexed root model until its edge or motion phase becomes terminal

- ID: `claim.presentation.flick-root-model`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.flick`, `render.playfield_projection`,
  `render.feedback_layering`, `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

Flick owns one root model whose executable resource row, transform, phase
visibility, reset, destruction, and preload behavior are closed. Widths 1
through 16 select rows 129 through 114. Nonterminal phases 0 and 1 place the
model at the common lane-span center and scheduled-time projected depth;
terminal phases 2 through 5 hide it. Directional result state does not select a
different root model.

## Anchors

- `game.exe @ RAM:00c200a0, FUN_00c200a0, parsed load and root setup, hash 92f5b333d1890f55c22aec7c3ff802858fae880b156c64b02fe43abfd3c44bd8`
- `game.exe @ RAM:00c20340, FUN_00c20340, active judgement/presentation update, hash 73082cff7fdfe6c66429320d370b7c615c5a345d0ca4261a86e629b17bb777a7`
- `game.exe @ RAM:00c1fee0, FUN_00c1fee0, terminal table query, hash 47c78518f05916a9c55ab80c06ca47d261434cdadf32a44673dbd559f517a1c3`
- `game.exe @ RAM:00c1fde0, FUN_00c1fde0, root hide/reset, hash db22e8f3a79e83220d28cc1023a8603d1b9267576b5a7fd29857345b5a0eebc8`
- `game.exe @ RAM:00c1ff30, FUN_00c1ff30, preload state machine, hash a5ede49e4b5a129500c2c349def32c34aeb27e8a763e871ceb2d02c61fd271e9`
- `game.exe @ RAM:00c1fe00, FUN_00c1fe00, preload reset/hide, hash b3d22ae27c3edc1db2a37d209a5c1688e959f31dc17db6dcef12adbb6ff1259d`
- `game.exe @ RAM:00c1fd20, FUN_00c1fd20, owned-state destruction, hash 5ca5a2f52d7ab577629b976f33d8362f1da8120c7eeefbfd217150e7d33a8ff0`
- `game.exe @ RAM:00b28820, FUN_00b28820, common lateral-center transform, hash 48582c5e1559c4da78c278131bf51e208fa9ff87db46f1a3b98492aa42954b00`
- `game.exe @ RAM:00b28890, FUN_00b28890, common projected depth, hash 19696db1083c59e7e5324f7e5dcc4866796038ba33244d08ddd4b650966ceae8`

## Observations

- Load clamps `decoded_width - 1` to `[0, 15]` and indexes the embedded
  descending row sequence `129..114`. The row resolves through an external
  model-resource table, so the binary closes selection while the proprietary
  model contents remain unavailable and replaceable.
- Initial translation is
  `(4*start_lane + 2*decoded_width - 32, 0, -10000)`. Scale is
  `(decoded_width / external_native_width, 1, executable_depth_scale)`;
  external native widths below one select lateral scale `1.0`, and the
  executable depth scale is the same platform-selected `1.0` or `1.3` used by
  Tap-family roots.
- Load clears field `+0x24` on each of two optional model-owned subobjects.
  Their semantic class is not needed to reproduce the observable root
  selection or transform, so this claim records the writes without assigning
  an unsupported animation name.
- The terminal query indexes a fixed byte table over the owned phase domain
  0..5: `{0,0,1,1,1,1}`. Thus only phases 0/1 enter the active model branch.
- The active branch computes the scheduled-time delta using the shared manager
  timing helper, invokes common projected depth with base-offset and
  positive-delta DCM enabled, writes the model matrix, and forces visibility.
  The terminal branch hides the root and enters common terminal handling.
- The large first half of the active function implements the already-closed
  edge/motion/result state machine. Its phase-4/5 direction distinction changes
  result metadata and feedback dispatch, not root resource selection or root
  geometry.
- Maintenance/reset hides the model. Destruction releases Flick/shared-note
  owned presentation state. Preload step zero stages all sixteen root rows;
  steps one and two stage feedback groups 16 and 14; readiness is set after
  step eight. Preload reset hides owned objects.

## Reasoning

The runtime vtable connects load, active update, terminal query, reset, and
destruction to the same Flick object. The embedded row sequence and common
transform consumers establish exact selection and placement. The terminal
table and the only model visibility calls establish lifetime. The preload
vtable independently accounts for every root row and the distinct feedback
groups.

## Alternatives and falsifiers

- Competing explanation: phase 4 and phase 5 choose left/right root models.
- Evidence that would disprove this claim: a phase-dependent write to the root
  resource handle, transform source, or model pointer after load.
- Competing explanation: terminal Flick remains visible until manager cleanup.
- Evidence that would disprove this claim: a terminal-table value of zero for
  phases 2..5 or a visible write after the terminal hide.

## Unknowns

- External model mesh, material, native-width records, and final pixels are not
  executable contents. A clean-room renderer must substitute original assets
  while preserving the exact row and scale protocol.
- Directional result effects, feedback lifetime, final scene layering, and
  camera/viewport conversion remain in their dedicated slices.

## Consequences

- Ghidra mutations: plate comments at `00c200a0`, `00c20340`, `00c1fee0`,
  `00c1fde0`, `00c1ff30`, and `00c1fd20`.
- Spec section: `spec/notes/flick.md`.
- Reconstruction code: `flick_model_resource_row`,
  `flick_phase_is_terminal`, and the common model-transform helpers.
- Tests: `tests/flick_presentation_test.cpp`.

## Verification

Runtime load/update/query/reset/destruction, preload/reset, the complete
16-entry row table, the complete valid phase table, common lateral/depth
helpers, and the absence of any post-load direction-specific root selector were
checked. Focused tests cover table endpoints/clamping, every valid phase, the
shared center/scale/depth rules, and active projection.
