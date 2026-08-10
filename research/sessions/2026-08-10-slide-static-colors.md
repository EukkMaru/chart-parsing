# Session 2026-08-10: shared static color correction

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `render.hold`, `render.slide`, `render.heaven_hold`,
  `render.air_slide`, `render.air_ladder`, `render.air_solid`,
  `config.external_presentation`, `audit.binary_saturation`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Resolve the source, values, and complete consumer set of the shared sustain and
path packed colors previously described as external runtime configuration.

## Findings

- A full-program byte-reference search found three tiny static initializer
  functions that Ghidra had not created. Their initializer-table entries and
  shared four-byte writer prove exact packed values `0xffffffff`, `0x40ffffff`,
  and `0xff666666` at globals `01c7abf0/+4/+8`.
- Slide, Hold, HeavenHold, AirSolid, and the shared AirSlide/AirLadder builder
  consume these globals. Direct xrefs close the read set, including a separate
  Field-mesh base-white reader. The Field reader belongs to a class whose sole
  recovered allocation root has no incoming caller/data reference, so it is a
  documented unreachable exclusion rather than a gameplay factory addition.
  Family/mode selectors are now exact in claims, specs, reconstruction, and
  tests.
- External materials/textures/shaders and final pixels remain external data;
  the color selector and values do not.
- The same raw-xref audit found a second missed initializer at `004be800` for
  Mine's 17-entry success-effect width table. It proves exact selector
  `max(clamp(width,0,16)-1,0)` and disproves the former external-value claim.

## Ghidra mutations

After representative disassembly and successful dry-runs, created default-
named functions at `004cf1d0`, `004cf1f0`, and `004cf220`. Added supported
plate comments recording their destinations, channel arguments, packing, and
initializer-table registration. Added exclusion comments to `00b28c10`,
`00c2c6d0`, and `00c07690` after tracing the Field allocation, setup, and mesh
path. No symbols were renamed and no types applied.
Created the default-named `004be800` initializer after dry-run and documented
it and consumer `00b29920`.

## Validation

- Focused Slide build/test: pending final run.
- Full CTest and `python3 scripts/harness.py validate`: pending final run.
- Ghidra project save: pending final run.

## Unresolved and contradictions

- Disproved: `01c7abf0` and `01c7abf8` were external runtime colors with no
  recoverable writer. The missed startup functions are their direct writers.
- Product phase simulation and external material/pixel substitution remain;
  neither is hidden executable color logic.

## Handoff

Continue the remaining partial viewer-row audit. Search raw address bytes when
a fixed global appears writerless; Ghidra's ordinary xref map omitted these
previously undefined initializer functions.
