# Session: cross-family chart mirror presentation

- Date: 2026-08-10
- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage: `render.mirroring_geometry`; `parser.events`
- Binary identity and MCP health: confirmed for the continuing session

## Goal

Close chart mirroring from the group-2 parser through every presentation family
and distinguish it from presentation-local mirroring, feedback flips, and the
external camera.

## Result

`claim.presentation.chart-mirror-transform` establishes one parser-owned,
wrapped lane-span reflection for every root and independently authored control
field; exact AIR direction swaps/resource signs; shared centered lateral
transforms; and preservation through Hold, Slide, Air-family, AirSolid, and
HeavenHold geometry. There is no stored chart-mirror flag for a later scene
reflection.

## Ghidra mutations

Expanded the plate comment at `011c8870` with the common root rewrite, each
distinct endpoint field, AIR lookup behavior, and the absence of a downstream
mirror flag. No tentative symbol was renamed.

## Verification

- CMake configure/build: passed.
- Focused `chart.mirroring_geometry` plus adjacent pass/material tests: passed.
- Full CTest: all 49 tests passed.
- `python3 scripts/harness.py validate`: passed with 28 coverage rows and 15
  required files.
- Ghidra project save: passed.

## Unresolved and handoff

- Viewer mirror selection is product integration; the binary rule is closed.
- External camera and model resources can prevent pixel-space symmetry even
  though chart-space reflection is exact. Keep those external values explicit.
- Continue the binary-side ledger audit with the open cross-family visual
  interaction row; do not conflate resource absence with an executable-rule
  ambiguity.
- The independent contradiction audit remains a separate completion gate and
  must not be self-certified by this investigator.
