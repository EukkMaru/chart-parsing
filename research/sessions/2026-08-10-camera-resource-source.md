# Session 2026-08-10: camera resource source closure

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `render.playfield_projection`,
  `config.external_presentation`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Close the exact executable-side source, selectors, consumers, and fallbacks for
the external Forester graphs that can supply gameplay camera motion.

## Findings

- `claim.presentation.common-scene-camera` now names
  `AcroartsTableRecord.bin`, its 0x18-byte row/string layout, rows 43/47/46,
  and the empty invalid-row fallback.
- All three paths use `acroarts/`; the earlier documentation incorrectly
  limited that prefix to rows 47/46 because row 43 performs concatenation in a
  helper.
- `LayerTableRecord.bin` rows 6/29/29 supply Forester `2DLayer`, with zero on an
  invalid index. They are not TargetScene selectors.
- Forester's load path and UVC consumer establish the graph relationship and
  camera motion as external file contents. No executable-side selector,
  consumer, or fallback remains unresolved in this slice.

## Ghidra mutations

Updated supported plate comments at `00b2e990` and added compact comments at
`00b2f000`, `010e4660`, `010e6570`, `010f1070`, and `01271ac0`. No symbols
were renamed and no types were applied.

## Validation

- Focused scene/camera build and test: pending final run.
- Full CTest and `python3 scripts/harness.py validate`: pending final run.
- Ghidra project save: pending final run.

## Unresolved and contradictions

- The earlier two-of-three prefix statement is disproved and corrected.
- The Acroarts row strings, resource graph relationship, motion samples,
  runtime framebuffer, and final material/pixel state are external data or
  runtime environment. They cannot be recovered from `game.exe` and remain
  explicit product inputs.

## Handoff

Continue the binary-saturation ledger by auditing every remaining partial
viewer row. Close rows whose only residual is this named external boundary;
reopen or create a claim wherever an executable selector, consumer, state
owner, reset, or indirect path remains untraced.
