# Session 2026-08-10: common scene/camera presentation

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `render.playfield_projection`; `render.feedback_layering`; `config.external_presentation`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Close the asset-independent scene order, active-camera ownership, projection,
viewport, and external camera-configuration boundary without fitting an absent
resource value.

## Findings

`claim.presentation.common-scene-camera` records the closed scene/camera math
and the UVC/Forester external boundary. The constructor defaults are proven
fallbacks but are not promoted to the final gameplay pose.

## Ghidra mutations

Compact plate comments were added to the scene constructor, BasicCamera
world-to-viewport conversion, UVC mixer loader, and gameplay Forester loader.

## Validation

- CMake configure and full build passed.
- Focused `chart.scene_camera_presentation` passed.
- Full CTest passed: 46/46.
- `python3 scripts/harness.py validate` passed: 28 coverage rows and 15
  required files.

## Unresolved and contradictions

The external table strings, UVC graph relationship to MainScene, motion
samples, and final active pose are unavailable at the executable-only boundary.
The unreferenced field-anchor helper is excluded from the live-path proof.

## Handoff

Use the recovered camera math with an explicit external pose. Do not hardcode
the constructor pose or a footage fit as canonical. Next run the independent
closure/contradiction audit selected by the stage-one harness, then resume the
stage-two cross-family and binary-saturation ledgers.
