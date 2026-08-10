# Session: common scene pass routing and ordering

- Date: 2026-08-10
- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage: `render.feedback_layering`; `config.external_presentation`
- Binary identity and MCP health: confirmed before the slice

## Goal

Close the shared scene path between a model submission and its draw order,
including external pass fields, routing predicates, sort modes, traversal, and
reset, without guessing the external material payload.

## Result

`claim.presentation.common-scene-pass-ordering` records the exact `BasePass`
schema, stable active-pass plan, first-match Type/User/Range routing, all eight
record orders, insertion/adaptive-merge threshold including unordered-float
behavior, draw-index traversal, and post-draw reset. The spec, clean-room
helpers, and focused test were updated with the same behavior.

The concrete gameplay `BasePass` instances and material-selected hardware
state are external graph/resource data. Their binary-owned selectors and
consumers remain traceable, but absent resource values must not be fabricated.

## Ghidra mutations

- Created the previously missed default function at `00621c40`, reached by the
  BasePass parameter-controller callback thunk.
- Added compact evidence comments at `006212c0`, `00621c40`, `00648d80`,
  `00648a90`, `00646fd0`, `0064be60`, and `00600830`.
- Repaired `019c3b88` to undefined data after an accidental data disassembly;
  no executable instruction or source artifact was retained from that action.

## Verification

- CMake configure/build: passed.
- Focused `chart.scene_pass_ordering` test: passed.
- Full CTest: all 47 tests passed.
- `python3 scripts/harness.py validate`: passed with 28 coverage rows and 15
  required files.
- Ghidra program save: passed after the recorded mutations.

## Unresolved and handoff

- The `ShaderLower` label shares the exact sort-0 implementation in this
  snapshot; retain that fact unless a separate live path proves otherwise.
- Trace the material-state consumer reached from the sorted draw loop. Separate
  exact selector/fallback behavior owned by the executable from external
  depth, blend, shader, texture, and render-target values.
- The independent contradiction audit remains a separate completion gate and
  must not be self-certified by this investigator.
