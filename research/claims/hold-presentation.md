# Claim: Hold combines two width-indexed markers with one clipped constant-width body quad

- ID: `claim.presentation.hold-root-body-transform`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.hold`, `render.playfield_projection`,
  `render.feedback_layering`, `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

Hold presentation owns two width-indexed `InstancingModel` markers and one
`JointHold` dynamic body. The markers share the Tap-family lane-center and
model-scale rules but select Hold-specific root/far resource tables. The body
is one constant-width, single-sided six-vertex quad whose independently
SLA-adjusted endpoints are clipped to projected depth `[-600, 50]`; raw
deltas separately control judgement-plane trimming and resource coordinates.

## Anchors

- `game.exe @ RAM:00c2adc0, FUN_00c2adc0, parsed load and resource setup, hash ad3bfbdad8b83dd0ea012891f9ed314f5f4f8adde6c9494d39be4f78a30bd489`
- `game.exe @ RAM:00c2b480, FUN_00c2b480, active wrapper, hash 32cbb89793b8071b60b93064cd0837b5fafb94a1cd4d6b477e8a84b97e48940a`
- `game.exe @ RAM:00c2a440, FUN_00c2a440, marker/body update, hash e97bd9b835a5ab2977b05632b91a12f393cf2f154ea039167ec4e510d36761ec`
- `game.exe @ RAM:00c09d60, FUN_00c09d60, JointHold geometry builder, hash ad838a784bee0660e680d4583c1e66a545faeb7ee6b8413bdcb00e1464573712`
- `game.exe @ RAM:00b28870, FUN_00b28870, lane-edge transform, hash 1cc795d4bdfb7e1e4f971672fd21b7eaa2a3ef012b82b2bc753e726b14909e68`
- `game.exe @ RAM:00c03710, FUN_00c03710, JointHold initialization, hash e23892449da86f81dbd8bc23be6389b612f39952e23374db85285232b5adbba8`
- `game.exe @ RAM:00c2a9c0, FUN_00c2a9c0, marker/body reset, hash 3ddcaddb34938205414bd61715d8012ef991492b6524d4c924488fd520259ef3`
- `game.exe @ RAM:00c2ab00, FUN_00c2ab00, Hold preload step, hash 5eda6d2686dfdeb9bc893c2440e42075563c5f825e43b50b7c18902b1f66f7f9`

## Observations

- Load independently selects the root and parsed-end SLA keys and retains both
  schedules. The active update projects the root and end separately through
  those keys before the common depth transform.
- Width lookup uses `clamp(decoded_width - 1, 0, 15)`. An ordinary Hold root
  selects embedded rows `49..34`. An extended HXD root reuses the shared
  extended-primary `97..82` or extended-alternate `113..98` table according to
  its parsed variant byte.
- The far marker selects rows `65..50` when no AIR-family secondary is attached
  and `289..274` when parsed attachment type 3, 5, or 8 set runtime byte
  `+0xb5`. This is an exact attachment-dependent resource selector, not a
  chart-color heuristic.
- Both markers use lateral center `4*lane + 2*width - 32`, initial depth
  `-10000`, lateral scale `width/external_native_width` with the below-one
  fallback `1`, and the executable-selected `1.0/1.3` depth scale already
  closed by the Tap-family claim. Their active visibility input is exactly
  `start_phase != 4`; the reset slot hides both.
- `JointHold` stores the constant lane center and decoded width. The builder
  converts its two edges by `(lane_coordinate - 8)*4`, so its half extent is
  `2*decoded_width` and its full width is the authored lane span at four units
  per lane.
- The body input preserves both adjusted and raw root/end deltas. Adjusted
  deltas feed the shared projection. `raw_start * raw_end < 0.000001` marks a
  possible judgement-plane crossing. When the projected end reaches the
  projected zero plane within the same epsilon, the drawn start is moved to
  that plane. Animated mode also forces that start-plane placement.
- A segment is rejected only when both projected ends are below `-600`, both
  are above `50`, or the absolute surviving depth span is below `0.000001`.
  Each surviving end is clamped independently to `[-600, 50]`. Start/end
  resource coordinates retain the clipped fraction; a valid raw crossing
  replaces the start coordinate with
  `raw_end / (raw_end - raw_start)`.
- Each surviving body emits exactly six `0x18` vertices: lateral, zero
  vertical, projected depth, an executable-owned static packed color, and two
  resource coordinates. Winding depends only on projected endpoint order.
  One dynamic primitive is submitted; a rejected body clears its prior vertex
  count.
- Presentation mode is exact: path phase 2 selects mode 1; otherwise, after
  start phase 4, path phase 3 selects mode 2 and every other path phase selects
  mode 1; before start completion mode 0 is used. Modes 0 and 1 use exact
  base white `0xffffffff`; mode 1 sets resource scale to
  `sin(fmod(counter*0.05,1)*2pi)*0.25 + 1.5`. Mode 2 uses scale one and the
  exact alternate gray `0xff666666`. The full active update stops only when both
  start and path phases equal 4.
- Hold preload stages the three sixteen-row model tables at steps zero through
  two, feedback resources at steps three through five, and becomes ready at
  step twelve. Destruction releases the two models, the dynamic primitive, and
  gameplay-owned vectors independently.

## Reasoning

The load path closes resource-family and transform initialization. The active
wrapper reaches one marker/body updater; its raw and adjusted inputs can be
followed independently into `JointHold`. The geometry builder fixes culling,
clipping, coordinate generation, vertex cardinality/winding, color mode, and
submission. Reset, preload, and destruction close lifetime without assigning
meaning to the external resource contents.

## Alternatives and falsifiers

- Competing explanation: Hold is rendered as a generic interpolated sustain
  shared with Slide.
- Evidence that would disprove this claim: a changing lateral/width input in
  `JointHold`, a Slide path-builder call, or more than one submitted body span.
- Competing explanation: the endpoint inherits the root's SLA transform.
- Evidence that would disprove this claim: the active update passing the root
  key with the parsed end schedule.

## Unknowns

- The model/material contents, external native widths, and final pixels are
  external resource data. Their selectors, fallbacks, and
  units-by-use are closed; their unavailable values remain parameters.
- Result/start/end feedback resources and final cross-family layer/occlusion
  order remain in the shared feedback/layering row.
- Final camera/viewport conversion after chart-space depth remains open.

## Consequences

- Ghidra mutations: plate comments at `00c2adc0`, `00c2a440`, `00c09d60`,
  `00b28870`, `00c03710`, `00c2a9c0`, and `00c2ab00`.
- Spec sections: `spec/notes/hold.md`.
- Reconstruction code: Hold resource-row selectors, phase visibility/mode,
  animated scale, body clipping/coordinates, and six-vertex builder in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/hold_presentation_test.cpp`.

## Verification

Constructor, parsed load, both SLA producers, active wrapper, marker updater,
body initializer/builder/submission, all three embedded row tables, the shared
static-color initializers and consumers, reset,
preload, and destruction were checked independently. Focused tests cover every
resource table edge, phase mode, visibility/terminal combination, raw crossing,
animated crossing override, near/far clipping, resource coordinates, both
colors, winding, off-range rejection, and degenerate spans.
