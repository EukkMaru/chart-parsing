# Session: material pass flags and offscreen requests

- Date: 2026-08-10
- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage: `render.feedback_layering`; `config.external_presentation`
- Binary identity and MCP health: confirmed for the continuing session

## Goal

Trace the material-state consumer downstream of the sorted scene draw loop,
closing executable-owned selection and request behavior without inventing the
external resource's material or final hardware state.

## Result

`claim.presentation.material-pass-flags` closes the three backend wrapper
kinds, material-attribute/default selection, pass Type and User precedence,
Sort 0/1 key refresh/defaults, four offscreen request bits, scene-wide request
propagation, pass-mask selection, and submission selection immediately before
draw dispatch.

The earlier `shader_secondary` reconstruction name was too specific: the
binary proves only that the backend callback returns a high/low material sort
key. The code, spec, and claim now use that evidence-bounded terminology.

## Ghidra mutations

Added compact evidence comments at `0063b650`, `0063b740`, `0063bbb0`,
`00649080`, `00601bd0`, and `00646b00`. No tentative function was renamed and
no proprietary output was copied into the workspace.

## Verification

- CMake configure/build: passed.
- Focused `chart.scene_pass_ordering` and `chart.material_pass_flags`: passed.
- Full CTest: all 48 tests passed.
- `python3 scripts/harness.py validate`: passed with 28 coverage rows and 15
  required files.
- Ghidra program save: passed after the recorded comments.

## Unresolved and handoff

- Concrete BasePass, material, and filter-source graph rows are external.
- The executable's generic backend calls can be traced farther, but without
  the selected resource payload they cannot yield the gameplay material's
  exact depth-test, blend, shader, texture, or pixel values. Continue with
  static paths that can still close selector/fallback behavior; parameterize
  unavailable values.
- The independent contradiction audit remains a separate completion gate and
  must not be self-certified by this investigator.
