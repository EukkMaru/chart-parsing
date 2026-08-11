# Session 2026-08-03: ordinary Slide presentation

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `parser.events`, `note.slide`, `state.ownership`;
  viewer rows `render.slide`, `render.playfield_projection`,
  `render.feedback_layering`, and `config.external_presentation`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Resolve ordinary type-2 Slide endpoint classes and shared path presentation
from exact command fields through generated ownership, geometry, clipping, and
submission, without assigning excluded resource imagery or material behavior.

## Findings

- `claim.note.slide-presentation-classes` is the normative endpoint/resource
  ownership claim. SLD/SXD ending markers own persistent generated endpoints;
  SLC/SXC nonfinal controls shape the path only; the final ending marker is
  forced. Root form, style, field-9 feedback, and endpoint lifetime have
  separate consumers.
- `claim.note.slide-path-presentation-geometry` is the normative shared-mesh
  claim. It closes the presentation-point vector, raw/projected cardinality,
  marker-delimited coordinates, raw-zero split, mode selection/culling,
  projected clipping, all three vertex streams, and Joint callback order.
- Optional field 9 has the exact extended-form decoder
  UP/DW/CE/RC/LC/RS/LS/BS and selects bounded result feedback only. It is not
  a path-direction, endpoint, or persistent-geometry selector.
- The viewer retains exact style/form/field-9/result-index state, places marked
  resources at the authored segment end, and represents the forced final
  endpoint with the same asset-free presence placeholder. Its former
  ten-sample Slide curve and screen-space gap were replaced by the recovered
  straight endpoint-segment silhouette and exact one-lane-wide center-stream
  silhouette.

## Ghidra mutations

Added supported plate comments at `00b25510`, `00c03790`, `00c10720`,
`00c0f8b0`, `00c0ee40`, `00c0de10`, `00c1d1d0`, `00c1b680`, `00c1b6c0`,
`00c1c340`, and `011c5630` for endpoint ownership, selectors, feedback, and
lifetime. Added supported path-geometry comments at `00bff510`, `00c0a3d0`,
and `00c02260`, and extended the comments at `00b25510` and `00c0ee40` for the
point/position/mode producers. Representative mutations were inspected and
dry-run before application. Default symbols and types were retained. The
program was saved after the final batch.

## Validation

- `python3 scripts/harness.py doctor`: exact binary identity, project, tools,
  and MCP health passed at session start.
- `python3 scripts/harness.py validate`: 28 coverage rows and all 13 required
  files passed after the final documentation/coverage update.
- Normal CMake build completed and CTest passed 35/35.
- Focused Slide tests cover all command marker/root forms, style and field-9
  decoding, endpoint allocation/visibility, presentation modes, cardinality
  rejection, coordinate grouping, zero split, mode culling, clipping,
  stream counts/extents/coordinates/colors, topology metadata, callback order,
  and the mode-1 pulse.
- A content-free aggregate over all 7,752 local charts found 3,004,267 Slide
  commands: 629,709 SLD, 1,955,244 SLC, 102,196 SXD, and 317,118 SXC. Exact
  field 8 covered empty, SLD, SXD-as-unrecognized/default, and two HLD cases;
  extended field 9 covered all eight exact strings plus empty legacy forms.

## Unresolved and contradictions

- The old viewer assumption that every Slide segment needs a ten-sample curved
  ribbon is contradicted by the recovered adjacent endpoint mesh. The viewer
  no longer uses that interpolation as a canonical Slide rule.
- Superseded 2026-08-10: raw address-byte search recovered the missed startup
  initializers. Base and alternate colors are exact `0xffffffff` and
  `0xff666666`; see `claim.presentation.shared-static-colors`. Resource
  identities, textures, materials, shaders, blend/depth
  behavior, and final pixel composition are excluded external semantics.
- The chart-only viewer does not simulate the start/path gameplay phases and
  therefore does not guess mode 1's overlay/pulse or mode 2's alternate color.
  Its remaining path colors and endpoint glyph are labeled asset-free
  substitutes, not binary values.
- Shared keyed projection arithmetic is closed, but final camera/viewport
  calibration remains under `render.playfield_projection`. Exact HLD style
  leaves ordinary Slide for HeavenHold, whose presentation remains separate.
- No JavaScript runtime is installed for a fresh syntax-only viewer execution,
  and the existing browser runner is known to stall in this environment. The
  HTML change was source-reviewed but is not represented as a fresh browser
  smoke pass.
- `harness.py next` selects only `audit.closure`. That gate requires a fresh
  independent investigator and cannot be self-certified by this session.

## Handoff

Give a different investigator the bounded contradiction audit selected by the
harness. For stage-two presentation work, the next exact Slide questions are
HeavenHold's separate view path and connecting the already reconstructed
ordinary Slide phase machine to viewer state. Do not reopen ordinary Slide
mesh geometry from footage unless a binary contradiction is found; do not
invent the unavailable colors or material semantics. No coverage row remains
owned by this investigator.
