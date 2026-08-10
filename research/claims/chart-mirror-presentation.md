# Claim: chart mirror is a parser-owned lane reflection

- ID: `claim.presentation.chart-mirror-transform`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.mirroring_geometry`; `parser.events`
- Last reviewed: 2026-08-10

## Statement

Chart mirror is applied once while group-2 records are ingested: every root
and every independently authored lateral endpoint becomes
`16 - lane - clamped_width` with signed-32-bit wrapping. Runtime presentation
consumes those stored lanes through a centered four-units-per-lane transform;
it does not apply a second chart-mirror flag. AIR direction codes use a separate
exact left/right swap whose shared external resource is reflected by scale sign.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, complete group-2 mirror consumer, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:00b28820, FUN_00b28820, lane-span center transform, hash 48582c5e1559c4da78c278131bf51e208fa9ff87db46f1a3b98492aa42954b00`
- `game.exe @ RAM:00b28870, FUN_00b28870, lane-coordinate edge transform, hash 1cc795d4bdfb7e1e4f971672fd21b7eaa2a3ef012b82b2bc753e726b14909e68`
- `game.exe @ RAM:00c0a3d0, FUN_00c0a3d0, Slide affine lateral interpolation/clipping, hash a637a8905ec6a6f5ebbee5b1abb8f4f6f020542a4c422e801be58f7965e14d2d`
- `game.exe @ RAM:00c09d60, FUN_00c09d60, Hold lane-edge body geometry, hash ad838a784bee0660e680d4583c1e66a545faeb7ee6b8413bdcb00e1464573712`
- `game.exe @ RAM:00c03c00, FUN_00c03c00, shared AirSlide/AirLadder path geometry, hash d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`
- `game.exe @ RAM:00c05ac0, FUN_00c05ac0, AirSolid affine geometry and clipping, hash 0ef38695a6ee89607db46366d486fece940ff86a63b81e29c1793d530fe096bb`
- `game.exe @ RAM:00c1e730, FUN_00c1e730, AIR direction resource/sign selection, hash f7aff9f14a26a21480adb1d435ecdf141f1b3c2c9f58add1e1537ac3d12d62f6`
- `game.exe @ RAM:011cbbe0 and RAM:011cbbb0, HeavenHold local selector mirror/mode tables, hash fe1286ea8b693ad23514a92dce718aec8fc0ba5231146e99a8704f6649215e6f`

## Observations

- Before the family switch, the event handler reads root lane field 2, clamps
  root width field 3 to 1..16, and conditionally stores wrapped
  `16 - lane - width`. That common result covers TAP, HLD/HXD, all Slide roots,
  AIR attachments, CHR, AHD/AHX, FLK, ASD/ASC, ALD, ASO, MNE, SLA, and HHD/HHX.
- Independently authored endpoints are mirrored before matching/append:
  Slide type 2 uses lane field 6 and its endpoint width; AirSlide, AirLadder,
  and AirSolid types 8/9/10 use lane field 8 and width field 9; HeavenHold type
  13 uses lane field 7 and width field 8. Hold and SLA have no independent
  lateral endpoint, while AirHold's attached data retains its mirrored root.
- The mirror parameter is used for these lane rewrites and the AIR direction
  lookup; it is not copied into the parsed record for later model/path code.
- The shared center function is `4*lane + 2*width - 32`; the shared edge
  function is `(coordinate - 8)*4`. For a valid field span, replacing lane by
  `16-lane-width` negates its center, and left/right edges become the negated
  right/left pair. Width and width-indexed resource selection do not change.
- Tap/CharaTap, Flick, Mine, and attached root models consume the common
  center/edge transforms. Hold builds its constant-width body from the two
  edges. Slide, AirSlide/AirLadder, AirSolid, and HeavenHold path builders
  consume already-normalized centers/edges and use affine lateral mixing when
  clipping or interpolating, so chart-space reflection is preserved.
- AIR and ADW direction codes are unchanged by mirror. AUR/AUL and ADR/ADL
  swap. Each left/right pair selects the same width-indexed external resource;
  codes AUL (2) and ADR (4) use negative lateral scale while AUR (1) and ADL
  (5) use positive scale.
- HeavenHold's authored `+0xb0` endpoint-mirror/geometry-mode selectors are a
  separate presentation-local transformation and compose after chart parsing.
  The source-category-15 feedback X flip is also independent of chart mirror.

## Reasoning

The complete group-2 switch establishes every family and every distinct lane
field while the common pre-switch path establishes the root transform. The
parsed record retains the transformed lane rather than the mirror flag. Shared
center/edge helpers and the family geometry builders then close the path to
chart-space vertices. Their lateral clipping operations are affine, so no
family reintroduces an unmirrored authored coordinate.

## Alternatives and falsifiers

- Competing explanation: the renderer mirrors the complete scene or camera.
  The only chart-mirror consumer is the parser path plus AIR direction lookup;
  downstream geometry reads the rewritten lanes.
- Competing explanation: mirrored AIR spellings select unrelated assets. Each
  left/right pair shares a resource row and changes only the executable-owned
  lateral scale sign.
- Evidence that would disprove this claim: a live family-specific path that
  reads an original lane after the rewrite, a stored chart-mirror flag consumed
  by presentation, or a non-affine lateral operation driven by chart mirror.

## Unknowns

- The external camera pose can make a chart-space reflection appear
  non-symmetric in final screen pixels. Chart-space mirror behavior is exact;
  pixel-space symmetry is not asserted.
- External model geometry determines the visible shape around a reflected
  origin. Width-row identity and transform sign are known, but the asset is not.
- Malformed lanes retain wrapped signed-32-bit parser behavior. The simple
  geometric reflection statement is limited to valid field spans after width
  clamp; wrapped extremes are preserved as exact values, not normalized.

## Consequences

- Ghidra mutation: expanded the evidence comment at `011c8870`; existing
  comments at the shared lateral and geometry builders remain applicable.
- Spec: chart mirror subsection in `spec/presentation.md`.
- Reconstruction: `mirror_c2s_lane_origin`,
  `presentation_lane_coordinate_lateral`, and
  `mirror_presentation_lane_center`; existing parsers use the shared wrapped
  helper.
- Tests: `tests/mirroring_geometry_test.cpp`.

## Verification

The focused test exhausts every valid integer lane/width span, pins malformed
wrapped arithmetic, exercises Hold/SLA/AirSolid parsing, all six AIR direction
outcomes/signs, Hold edge swapping, Slide reflected interpolation, AirSolid
edge swapping, and the independent HeavenHold selector boundary. Full tests
and harness validation are recorded in the session note.
