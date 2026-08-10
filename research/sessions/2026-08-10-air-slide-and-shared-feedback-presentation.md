# Session 2026-08-10: AirSlide and shared result-feedback presentation

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: viewer rows `render.air_slide`,
  `render.feedback_layering`, `config.external_presentation`, and
  `audit.cross_family_visual`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Close the remaining AirSlide family-local presentation path, then trace the
shared result-feedback consumer through lane ownership, effect admission,
lifetime, scene selection, and the boundary where unavailable external player
resources determine final pixels.

## Findings

- `claim.presentation.air-slide-model-path` closes AirSlide root/control/action
  ownership, exact transforms, dual schedules, authored geometry, phase modes,
  resource selectors, preload, maintenance, and destruction.
- `claim.presentation.shared-result-feedback` closes all twelve note-vtable
  entries into shared feedback, fixed result masks/resource modes, ordinary
  and Slide effect order, lane serial groups, the post-dispatch cue gate, ten
  effect lists, admission/cooldown/lifetime, scene selection, and monotonic
  submission keys.
- The stronger proposition that effect-list index is final draw order was
  rejected: the binary proves list update order and per-player sequence, while
  final scene sorting, camera, depth/blend state, and external materials remain
  downstream inputs requiring a separate trace.

## Ghidra mutations

- Added supported plate comments to the AirSlide constructor/destructor,
  gameplay/update/terminal/maintenance/reset/preload/load/wrapper path recorded
  by `claim.presentation.air-slide-model-path`.
- Added supported plate comments at `00c1c340`, `00c1cce0`, `00c1f8f0`,
  `00b1b370`, `00b1af90`, `00b1b210`, `00b2f830`, `00b1ccc0`, `00c1b6c0`,
  `00b1ab50`, `00b18370`, `00b1d1f0`, `00b1d6d0`, `00b1d880`, `00b1a900`,
  `00b1d750`, `00b1b680`, `00b1a660`, and `00bfdec0`.
- Saved `game.exe` in the local Ghidra project after both mutation groups.

## Validation

- Exact binary identity and MCP health passed at session start.
- `cmake -S . -B build -DBUILD_TESTING=ON` and the full build passed.
- All 45 CTests passed after adding the focused shared-feedback test.
- `python3 scripts/harness.py validate` passed: 28 coverage rows and 15
  required files.
- The shared consumer's decompiler types were independently checked against
  assembly for the lane helper's hidden owner and four pushed arguments.
- The embedded masks/table, effect-list descriptor records, vtable targets,
  preload-mode callers, scene-name branches, global sequence write, and
  effect-state removal predicate were followed separately.

## Unresolved and contradictions

- External resource IDs/rows, the writable Slide feedback table, effect-player
  meshes/materials/animations, and scene configuration are unavailable values;
  their executable selectors and consumer boundaries are closed.
- `MainScene`/`BgScene` selection and the monotonic sequence key are exact, but
  the downstream interpretation of sequence, scene traversal order, camera,
  depth/blend state, and cross-family occlusion is not yet closed.
- AirSlide's remaining viewer gap is state integration plus that same shared
  downstream composition, not another family-local binary branch.

## Handoff

Continue from `00bfdec0`'s scene lookup/player virtual calls and the known
family model/Joint registrations. Resolve scene traversal, camera/viewport,
sort/depth/blend inputs, and mirror propagation without assuming final pixels
from effect-list names. Then update `render.playfield_projection`,
`render.mirroring_geometry`, `render.feedback_layering`, and
`config.external_presentation`. Ghidra write ownership remains with codex-root
for the continuing downstream slice.
