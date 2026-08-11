# Claim: Slide path presentation is a clipped three-stream segment mesh

- ID: `claim.note.slide-path-presentation-geometry`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `note.slide`, `state.ownership`; viewer rows
  `render.slide`, `render.playfield_projection`,
  `render.feedback_layering`, and `config.external_presentation`
- Last reviewed: 2026-08-03

## Statement

Ordinary type-2 Slide presentation converts the generated path into adjacent
segments with marker-delimited longitudinal coordinates, splits the first
segment crossing raw position zero, applies mode-dependent past culling, clips
all carried values to projected range `[-600, 50]`, and fills three ordered
resource-independent vertex streams: a full-width main mesh, a fixed
one-lane-wide center mesh, and a mode-1-only full-width overlay.

## Anchors

- `game.exe @ RAM:00b25510, FUN_00b25510, presentation-point and descriptor producer, hash a3a0ba09ade07133f23b8d78260d159511afc37d2d821c8ab595c7ef382f025d`
- `game.exe @ RAM:00bff510, FUN_00bff510, RTTI-identified JointSlide construction, hash 66de92b2c17671d434d84f79cf92c3147711170071ac91788739b46f5786db32`
- `game.exe @ RAM:00c0ee40, FUN_00c0ee40, position-array and presentation-mode producer, hash 081634ec8e24c1d970ce8ee43104eaef0940b64500bd6703aeb622b8df0e673d`
- `game.exe @ RAM:00c0a3d0, FUN_00c0a3d0, segment preparation and three-stream vertex producer, hash a637a8905ec6a6f5ebbee5b1abb8f4f6f020542a4c422e801be58f7965e14d2d`
- `game.exe @ RAM:00c02260, FUN_00c02260, 0x30-record insertion used by zero-crossing split, hash a6890d31bbe05def3daf7a0ec961eac3cf89b06e302436c4334b982a6eb5e836`
- `game.exe @ RAM:00b28890, FUN_00b28890, keyed schedule-to-projected-position transform, hash 19696d4583b614a4b5281702aa92150391ee2431993647be73915c71f0625cea`
- `game.exe @ RAM:00bfebb0, FUN_00bfebb0, Joint construction and graph registration, hash f3f9eed732002e4f91d8f62528b3deccd7c18d15cd1f128d9aec558bce59132f`
- `game.exe @ RAM:004cf1f0, FUN_004cf1f0, static white-color initializer, hash e2ed6bf85587c181e62d17809e5028c1da3dfb9826b2f2aa7f406610c819d70f`
- `game.exe @ RAM:004cf1d0, FUN_004cf1d0, static gray-color initializer, hash 822a70948ead3fb5d5cfc994812681591bbb277d0186b8dfa7db94bfec99b817`
- `game.exe @ RAM:004cf220, FUN_004cf220, static low-alpha white initializer, hash e37a0573f190a418faec35cf09652e0a1a90efc21a39316e14cdab2808c8d520`
- `game.exe @ RAM:005fb980, FUN_005fb980, four-channel byte packing, hash ce95e7278600f32356052a9221beb32ac757111e9566bd33e9036e374ccc1f94`
- `game.exe @ RAM:016f1bf8/016f1c00, static initializer-table entries for FUN_004cf1f0/FUN_004cf1d0`
- `game.exe @ RAM:01c7abf0/01c7abf4/01c7abf8, initialized packed-color globals`

## Observations

- Slide precompute appends one presentation point for every generated segment
  start and one forced final endpoint. Each point stores decoded width, lane
  center, and its preceding boundary marker. The same builder selects three
  style-indexed external resource descriptors with topology/mode values 4, 3,
  and 3.
- `projView::JointSlide` constructs exactly three `projView::Joint` children in
  stream-index order 0, 1, 2. Joint construction registers each graph node in
  that order. It also reserves 256 entries for a `0x30`-byte segment vector.
- The Slide update appends one raw and one projected position for the root and
  every generated endpoint. Projected positions use the endpoint's keyed
  schedule path and `FUN_00b28890`; raw positions retain the corresponding
  schedule delta. A cardinality mismatch among points, raw positions, and
  projected positions clears all three streams.
- Presentation mode is exactly: path phase 2 selects mode 1; otherwise a
  resolved start with path phase 3 selects mode 2; a resolved start with any
  other path phase selects mode 1; all remaining states select mode 0.
- Adjacent points become `0x30`-byte segments carrying raw and projected
  endpoints, render lateral centers `(lane_center - 8) * 4`, decoded widths,
  boundary markers, and longitudinal coordinates. The first start marker and
  final end marker are forced.
- Within each marker-delimited group, coordinates accumulate absolute raw
  span and divide by the absolute difference between the group's first raw
  start and last raw end. The denominator is floored at `0.00001`; every
  coordinate is clamped to `[0, 1]`.
- The first segment with `raw_start < 0.000001` and
  `raw_end > -0.000001` is duplicated and divided at the projected judgement
  plane. Lateral center, width, and longitudinal coordinate are interpolated
  at the split. The past copy has both raw values zero and clears its end
  marker; the future copy clears its start marker.
- Mode 1 discards segments whose raw end is below `0.000001`. Every mode first
  discards a segment when both projected endpoints lie below -600 or both lie
  above 50, then clips either crossed endpoint to the exact bound while
  interpolating lateral center, decoded width, and longitudinal coordinate
  from the original endpoints.
- Stream 0 uses full decoded width and vertical zero. Equal-width segments
  emit six vertices. A width delta at least the binary float epsilon
  `1.1920929e-7` emits 18: a center quad at width scale `0.7` and texture
  coordinates `0.15`/`0.85`, plus two side strips reaching coordinates 0/1.
  Winding is selected from the projected-end versus projected-start order.
- Stream 1 always emits a six-vertex vertical-zero quad with render-space half
  extent 2, which is exactly one chart lane in total width. Stream 2 emits a
  six-vertex full-width quad only in mode 1 and uses packed color
  `0x20ffffff`.
- Three formerly missed static initializers construct the neighboring packed
  colors through one four-byte writer. The writer stores the third, second,
  first, and fourth arguments at byte offsets 0, 1, 2, and 3. The initializer
  pointer table registers base white `(255,255,255,255)`, low-alpha white
  `(255,255,255,64)`, and alternate gray `(102,102,102,255)`, producing exact
  little-endian packed values `0xffffffff`, `0x40ffffff`, and `0xff666666`.
- Mode 0 reads exact base `0xffffffff` for stream 0. Mode 1 uses that same
  color and applies the scalar
  `sin(fmod(counter * 0.05, 1) * 2*pi) * 0.25 + 1.5`. Mode 2 reads the
  exact alternate `0xff666666` and emits no stream 2. The neighboring
  `0x40ffffff` global is also a binary-owned shared presentation color; it is
  not an external configuration value.
- Diagnostic calls in categories 1, 2, and 3 count stream triangles; they do
  not establish rendering order. The three Joint graph callbacks are reached
  in registered order 0, 1, 2. Final pixels still depend on the externally
  selected resources and shared renderer state.

## Reasoning

The precompute point vector and update-side position arrays converge at the
single cardinality-checked geometry builder. That builder owns every split,
clip, vertex append, and diagnostic count. The three stored Joint children
consume the corresponding stream containers in their construction order.
This closes the executable's resource-independent Slide silhouette and stream
structure while keeping material-dependent appearance outside the claim.

## Alternatives and falsifiers

- Competing explanation: the path is a sampled spline, every control produces
  a separate checkpoint quad, or the three topology values are visual layer
  numbers.
- Evidence that would disprove this claim: a live ordinary Slide path that
  bypasses `FUN_00c0a3d0`, another writer that curves or retessellates the
  segment endpoints before Joint submission, a fourth path stream, or a
  submission traversal that reorders the three registered Joint children.

## Unknowns

- Resource/material/texture identities, shaders, blend/depth state, and final
  pixel compositing are external-data or shared-renderer semantics. Their
  executable selection structure is known, but asset contents are excluded.
- `FUN_00b28890` closes schedule-to-projected-position arithmetic and the
  geometry builder closes its local projected clipping. The shared binary
  camera/viewport algorithm and exact external graph boundary are closed by
  `claim.presentation.common-scene-camera`.
- A chart-only preview does not yet simulate the start/path phases needed to
  select modes 1 and 2. The exact mode selector is reconstructed; runtime
  state integration remains open.
- HeavenHold leaves ordinary type-2 Slide before this presentation path and
  requires a separate presentation trace.

## Consequences

- Ghidra mutations: supported plate comments at `00bff510`, `00c0a3d0`, and
  `00c02260`; extended supported comments at `00b25510` and `00c0ee40`; created
  the missed static initializer functions at `004cf1d0`, `004cf1f0`, and
  `004cf220` after dry-run and documented their exact destinations/values.
  Default symbols and types were retained.
- Spec sections: `spec/notes/slide.md` and `docs/VIEWER_ROADMAP.md`.
- Reconstruction code: Slide presentation mode, point/segment preparation,
  coordinate grouping, zero split, projected clipping, three vertex-stream
  builders, mode color/pulse selection, and stream metadata in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/slide_path_test.cpp`.

## Verification

The point producer, position/mode producer, geometry builder, insertion helper,
Joint owner, graph registration, and keyed projection helper were traced as
separate paths. Focused tests cover all presentation-mode branches,
cardinality rejection, marker-delimited coordinates, zero splitting, mode-1
past culling, simultaneous near/far clipping with interpolation, exact stream
counts and extents, width-change tessellation coordinates, overlay gating and
color, Joint order, topology values, exact static colors, main-color choice,
and pulse extrema.
The full CTest and harness results are recorded in the session handoff.
