# Gameplay presentation

This specification covers binary-owned presentation behavior that begins at a
runtime note and can change the faithful asset-free gameplay view. It does not
make external game meshes, textures, materials, audio, fonts, or animation
payloads normative. Per-family shape, transform, phase, and lifetime rules are
specified in `spec/notes/`; this document owns their common result-feedback and
submission boundary.

## Scene and camera boundary

The gameplay presentation constructs `BgScene` at signed priority 9900 and
`MainScene` at priority 10000. Scene insertion is ascending and traversal is
forward, so background scene work precedes main scene work. `BgScene` uses draw
index 16 and non-immediate presentation; `MainScene` uses draw index 0 and the
immediate path. Both own an `air::BasicCamera`. The direct scene-camera field
write closure contains only construction, replacement during `air::Scene`
construction, and destruction. `ForesterPlayer` declares target-scene, mask,
draw-index, and 2D-layer parameters but does not replace the active camera
pointer.

The `BasicCamera` constructor defaults are near 1, far 30000, vertical FOV 45
degrees, position `(0,0,30)`, target `(0,0,0)`, up `(0,1,0)`, perspective on,
and zero projection offsets. Scene binding replaces its initial aspect with
`viewport_width / max(viewport_height, 1)`. These defaults are a fallback, not
a recovered final gameplay pose: with that view, changing chart depth changes
perspective scale but leaves a point at Y=0 on the viewport's vertical center.

The executable has a generic `SbUvcFileLoader` path capable of configuring a
`BasicCamera` mixer from external SVO/UVC graph nodes. A target graph ID is
resolved and RTTI-checked as `BasicCamera`; its `SbUvcFileLoader` children
replace mixer slots with normal or offset camera motions, copy external motion
resource handles, bound a `Frame` parameter by the loaded motion length, and
apply `Blend`, `Link`, `Play`, and `Loop` controls. Missing targets, wrong RTTI,
or unresolved motion resources leave the fallback/no-op path. The loader
defaults are mode `Normal`, blend 1, link 8, frame 0, play true, loop false,
and delete false.

The generic table loader composes an externally selected database directory,
`\\`, the table-record name, and `.bin`. Gameplay field setup reads rows 43,
47, and 46 from `AcroartsTableRecord.bin`; each 0x18-byte row contributes the
string handle at +0x0c, while a negative or out-of-range index yields the
binary's empty-string fallback. Every selected string is prefixed with
`acroarts/`, including row 43 through the first-player helper, and the result
is passed to the Forester graph loader. A missing resource or graph leaves the
runtime graph/player path unset.

The same setup assigns the Forester `2DLayer` parameter from rows 6, 29, and
29 of `LayerTableRecord.bin`. That 0x10-byte row accessor reads +0x0c and
returns zero on an invalid index. These are layer values, not target-scene
names. The constructor's `TargetScene` default is empty and field setup does
not directly replace it; any nonempty value is external graph/parameter data.

The selected strings, graph relationships, camera motion payloads, and
resulting pose are not embedded in the executable. The executable side is
closed at this named loader/selector/consumer/fallback boundary. Therefore a
clean-room viewer must expose the camera pose/configuration as an external or
owner-calibrated parameter and must not label a fitted pose canonical.
Evidence: `claim.presentation.common-scene-camera`.

## Projection and viewport conversion

Matrices are row-major and transform column vectors. The perspective matrix
uses `q = 1 / tan(FovY / 2)`, `m00 = q / aspect`, `m11 = q`,
`m22 = far / (near - far)`, `m23 = near*far / (near - far)`, and `m32 = -1`.
Before construction, far is raised to `near + 0.0001` when necessary. The view
matrix uses normalized `position - target`, normalized `cross(up, z)`, and
their cross product, with negative position dot products in the translation
column. A zero up vector falls back to `(0,1,0)`; a position/target separation
below float epsilon shifts target Z by `0.0001`.

The combined matrix is `projection * view`. For a world point, the executable
divides clip X/Y/Z by clip W and maps X to
`width/2 * (ndc_x + 1)`, Y to `height/2 * (1 - ndc_y)`, and retains NDC Z as
depth. This algorithm is normative even when the resource-selected camera pose
and runtime framebuffer dimensions remain external. Reconstruction:
`build_presentation_projection_view` and `project_presentation_point`; focused
test: `tests/scene_camera_presentation_test.cpp`.

## Chart mirror and chart-space lateral coordinates

Chart mirror is a parser operation, not a scene/camera operation. After width
is clamped to 1..16, each root or independently authored control span stores:

```text
mirrored_lane = wrapped_i32(16 - lane - width)
```

The common presentation transforms are:

```text
span_center_x = 4*lane + 2*width - 32
lane_coordinate_x = (coordinate - 8)*4
```

Consequently every valid mirrored span has the negative chart-space center of
the original, and its left/right edges are the negated original right/left
edges. TAP/CHR/FLK/MNE and attached roots use the common center/edge path;
HOLD uses the two edges; Slide, AirSlide, AirLadder, AirSolid, and HeavenHold
consume individually mirrored controls and retain reflection through affine
lateral interpolation/clipping. Malformed lane subtraction retains signed
32-bit wrapping and is not renormalized.

AIR and ADW direction codes remain unchanged. AUR/AUL and ADR/ADL swap; each
pair shares its external resource row, with AUL and ADR applying negative
lateral resource scale and AUR and ADL positive scale. HeavenHold's authored
selector mirror/mode and the source-category-15 feedback X flip are separate
local effects, not a second chart mirror. The external camera can prevent a
chart-space reflection from being a pixel-perfect screen reflection.
Evidence: `claim.presentation.chart-mirror-transform`; reconstruction:
`mirror_c2s_lane_origin`, `presentation_lane_coordinate_lateral`, and existing
family geometry helpers; focused test: `tests/mirroring_geometry_test.cpp`.

## Shared static vertex colors

Three startup initializers, registered in the executable's initializer table,
construct exact packed colors through one four-channel byte writer:

| Role | Packed value | Consumers |
| --- | ---: | --- |
| base white | `0xffffffff` | Hold, HeavenHold, Slide, AirSolid, AirLadder/AirSlide streams 0/1 |
| low-alpha white | `0x40ffffff` | AirLadder/AirSlide stream 2 |
| alternate gray | `0xff666666` | Hold, HeavenHold, Slide, AirSlide mode 2 |

Hold, HeavenHold, and Slide modes 0/1 use base white and mode 2 uses alternate
gray. AirSolid always uses base white. The shared Air path builder uses base
white for primary streams in modes 0/1, alternate gray in mode 2, and
low-alpha white for stream 2 in every mode. Slide's mode-1 overlay separately
uses immediate `0x20ffffff`.

These packed values are binary constants, not external material parameters.
Textures, shaders, blend/depth behavior, and final pixels remain resource and
renderer inputs. Evidence: `claim.presentation.shared-static-colors`;
reconstruction: the `presentation_static_*_color` constants and exact family
mode selectors; focused coverage spans the six affected presentation tests.
One additional direct read occurs in the RTTI-identified `projView::Field`
mesh, but its sole recovered allocation root has no caller or data reference;
that class is excluded from the reachable gameplay presentation graph unless
a concrete indirect producer is found.

## Scene pass configuration and routing

External `BasePass` records provide `PassIndex` in 0..31, `Entry`, `Type` in
0..7, `User` in 0..15, `Sort` in 0..7, `Range` in 0..4, and one float and one
unsigned range value. Active records are stably ordered by ascending
`PassIndex`; disabled records are omitted. The resulting internal pass order
is registered both with the model submission lists and with the first/last
range for each external draw index.

A submission is assigned to the first planned pass satisfying all of these
tests:

- Type is equal, or the pass is All, or it is OpaquePunch for type 0/1, or it
  is 3DAll for type 0/1/2.
- User equals the submission's four-bit user field.
- Range is All, `depth < F32` for DepthFront, `F32 <= depth` for DepthBack,
  `U32 <= layer` for LayerFront, or `layer < U32` for LayerBack.

The supplied depth probe is retained in an accepted submission. Later matching
passes are not considered, and a submission with no match is not appended by
this path. The concrete `BasePass` rows are external resource-graph values and
must remain configurable. Evidence:
`claim.presentation.common-scene-pass-ordering`; reconstruction:
`build_presentation_pass_plan` and `route_presentation_submission`; focused
test: `tests/scene_pass_ordering_test.cpp`.

## Per-pass sorting and draw traversal

The eight sort modes order the 0x38-byte logical submission records as follows:

| Sort | Primary | Secondary |
| ---: | --- | --- |
| 0 ShaderUpper | material-sort high word descending | low word descending |
| 1 ShaderLower | material-sort high word descending | low word descending |
| 2 DepthUpper | depth descending | none |
| 3 DepthLower | depth ascending | none |
| 4 LayerAllUpper | layer descending | none |
| 5 LayerAllLower | layer ascending | none |
| 6 LayerLower+DepthUpper | layer ascending | depth descending |
| 7 LayerLower+DepthLower | layer ascending | depth ascending |

Sort 1 deliberately has the same implementation as sort 0 in this executable;
the label does not justify inventing an inverse. Ranges of at most 32 records
use insertion sorting. Larger ranges split at `(count + 1) / 2` and use the
binary's adaptive merge direction. Equal finite keys retain source order. The
reconstruction also preserves the executable's unordered floating comparisons
rather than substituting a library sort with unspecified NaN behavior.

The scene filter traverses external draw indices 0 through 31 and submits each
configured inclusive internal-pass range. Each pass is walked in sorted order;
after the immediate draw cycle, every submission end is reset to its begin
without releasing capacity. This closes record routing and ordering, but not
the external materials' selected depth-test, blend, shader, texture, or final
pixel behavior. Evidence: `claim.presentation.common-scene-pass-ordering`;
reconstruction: `sort_presentation_submissions`; focused test:
`tests/scene_pass_ordering_test.cpp`.

## Material pass classification and offscreen requests

Generic scene submissions carry backend kind 0, 1, or 2. Kinds 0 and 1 expose
external material flags; kind 2 receives the process default flag word zero.
The pass `User` value is bits 25..28 of that word.

For kind 0/1, Type starts as Opaque, bit `0x40` selects Punch, and bit `0x20`
selects Trans with precedence over Punch. Kind 0 bit `0x80` then selects 2D.
Kind 1 selects 2D for the sign bit or its separate payload bit `0x10`. Finally,
bit `0x2000` selects Reduce for either kind and overrides all earlier choices.
Kind 2 instead selects Trans, Punch, or Opaque from its two nested payload
bytes, then its wrapper bit `0x08` overrides the result with 2D.

For Sort 0/1, a backend callback refreshes the two-word material sort key
before ordering. The words are consumed only as high-then-low unsigned values;
no independent semantic meaning may be assigned without resource/backend
evidence. Kind 2 and unavailable kind-1 callback paths provide zero.

Four external material bits request offscreen targets:

| Bit | Scope | Effect |
| ---: | --- | --- |
| `0x200` | PassIndex | add pass to color mask; request color target |
| `0x400` | PassIndex | add pass to depth mask; request depth target |
| `0x800` | submission | request color target; select before this draw |
| `0x1000` | submission | request depth target; select before this draw |

Any scoped request raises the corresponding scene-wide byte, which is
propagated to BasicScene parameters 14 `RequestColorOffscreen` and 15
`RequestDepthOffscreen`. The filter consumes pass-mask selections during
draw-index traversal and submission selections immediately before backend
draw. This specifies routing, not the absent external material values or final
hardware pixel state. Evidence: `claim.presentation.material-pass-flags`;
reconstruction: `presentation_pass_type_from_material` and the
`presentation_*_offscreen_*` helpers; focused test:
`tests/material_pass_flags_test.cpp`.

## Cross-family update and composition order

The active manager owns separate primary and attached-secondary vectors. In
each manager substep it completes candidate work over primaries, updates every
primary in forward storage order, removes terminal primaries, updates every
attached secondary in its separate forward order, and then removes terminal
secondaries. It never interleaves one attachment immediately after its root.
For a compound chart with primary order `P0, P1, P2` and secondary order
`S0, S1`, the update order is always:

```text
P0, P1, P2, S0, S1
```

Each family performs gameplay/result/lifetime work before its presentation
half. Shared feedback calls are synchronous, so feedback creation serials and
lane-overlay overwrites follow the same complete-primary then complete-
secondary order. The ordinary path can perform more than one catch-up substep,
but the ten effect lists update only once after all substeps, in indices 0
through 9. Pending record materialization occurs after that effect pass, and
the post-manager scene-resource exports occur later still. A newly materialized
note cannot update or emit feedback in that outer call.

This update order is not an unconditional draw-layer rule. Local multi-stream
Joint order remains family-owned, `BgScene` traverses before `MainScene`, and
the first matching external pass sorts its submissions by material, depth, or
layer keys. Equal finite keys retain producer submission order, while unequal
keys can reorder it. Concrete external pass/material/graph values therefore
remain required to determine a particular final overlap. Evidence:
`claim.presentation.cross-family-update-composition`; reconstruction:
`RuntimeActiveVectors`, `runtime_manager_update_order`, and
`build_cross_family_outer_update_schedule`; focused test:
`tests/cross_family_presentation_order_test.cpp`.

## Shared result-feedback order

Every runtime note class reaches one common feedback consumer. The common
one-position wrapper supplies the same position to its three ordinary effect
slots. Ten note classes use that wrapper directly. Mine and Flick use
opcode-identical wrappers that first apply the loaded active result-control
rule to the incoming result byte, then forward the remapped byte and unchanged
position to the common wrapper. Mine's held-lane average/span-center rule is
only for its separate zero-result success effect. The following order is
normative:

1. sample the currently selected ordinary/alternate terminal predicate;
2. adjust the transient feedback result and variant;
3. attempt effect-list kinds 2, 0, and 1, in that order;
4. update the lane-feedback overlay when its source-category gate admits;
5. dispatch the result to the authoritative result owner;
6. sample the selected terminal predicate again and conditionally emit the
   fixed post-result cue;
7. for Slide source category 1 with nonzero result, attempt the two extended
   result effects in kind order 6 then 7.

Thus the three ordinary effects observe terminal state from before the current
result dispatch, while the fixed cue observes terminal state after it. Lane
feedback is not controlled by this terminal predicate. Evidence:
`claim.presentation.shared-result-feedback`; focused test:
`tests/shared_feedback_presentation_test.cpp`.

## Transient result and variant adjustment

Results 3 and 4 share one externally selected mask index; results 2 and 1 use
two other externally selected indices. Each index is `min(selected,
loaded_count - 1)`. The executable assumes nonzero counts and indices within
the following four-entry masks; clean-room callers must reject a zero count or
an effective index at least four as malformed external configuration.

| Result | Suppress requested variant | Remap result to 5 |
| ---: | --- | --- |
| 4 | always | `0, 0, 1, 1` |
| 3 | `1, 0, 0, 1` | `0, 0, 1, 1` |
| 2 | `1, 0, 0, 1` | `0, 0, 1, 1` |
| 1 | `1, 0, 0, 1` | `0, 0, 1, 1` |

Results 0 and values above 4 retain both inputs. The adjusted pair selects an
external effect resource mode:

| Result | Default variant | Variant 1 | Variant 2 |
| ---: | ---: | ---: | ---: |
| 0 | 7 | 7 | 7 |
| 1 | 4 | 5 | 6 |
| 2 | 1 | 2 | 3 |
| 3 | 0 | 8 | 9 |
| 4 | 0 or 13 by the kind-2 special flag | same | same |
| 5 | 12 | 10 | 11 |

Other results map to `-1`. Evidence:
`claim.presentation.shared-result-feedback`; reconstruction:
`adjust_feedback_result_variant` and `map_feedback_resource_mode`.

## Ordinary effect selectors and scenes

All three ordinary effect attempts require the pre-dispatch selected terminal
predicate to be false. Kind 2 can additionally be restricted to source
categories 2 through 6 by one manager byte. Kind 1 is always disabled for
those five categories. Kind 0 has no source-category restriction.

Kind 2 reads external source-row field `+0x3c`. Kind 1 reads `+0x38`; when the
controlling manager byte is clear, resource IDs `0x18` and `0x1c` are replaced
by `0x3e` and `0x3f`. Kind 0 selects its external field and bucket width by
span:

| Span | Row byte offset | Bucket width |
| --- | ---: | ---: |
| 1 | `0x34` | 1 |
| 2 | `0x30` | 2 |
| 3 | `0x2c` | 3 |
| 4 | `0x28` | 4 |
| 5..6 | `0x20` | 6 |
| 7..8 | `0x1c` | 8 |
| 9..16 | `0x0c` | 16 |
| otherwise | none | 0 |

For source categories 7 through 9, kind 0 uses X scale `span / bucket_width`;
all other categories use 1. Kind 0 and kind 1 flip X and bind to `BgScene`
only for source category 15. Kind 2 always binds to `MainScene`. A negative,
zero, or resource-table-out-of-range selected ID suppresses submission.
External resource contents remain parameters. Evidence:
`claim.presentation.shared-result-feedback`.

## Lane-feedback overlay

The lane overlay is requested for source categories 0, 1, 14, and 15. Source
category 17 requests it only when result is zero and supplies the sole
zero-result override. Other categories do not update it.

The 32-bit signed start clamps to at least zero. The 32-bit wrapped sum
`start + span` clamps to at most 16; a nonpositive end or empty interval does
nothing. A normal result zero also does nothing. On an admitted call, one
32-bit serial is incremented with wrap. For each selected lane in ascending
order, every one of the sixteen lanes carrying that lane's prior nonzero
serial is first reset to serial/result zero. The selected lane then receives
the new serial and result byte. Consequently overwriting one member of an old
multi-lane group clears the full old group before constructing the new group.
Evidence: `claim.presentation.shared-result-feedback`; reconstruction:
`apply_lane_feedback`.

## Post-result cue and Slide extended feedback

The fixed cue accepts only configured mode 1, 2, or 3 and result byte strictly
below that mode. It also requires the selected terminal predicate sampled
after authoritative dispatch to be false. Its hardcoded identifiers select an
external payload and do not establish a player-facing effect or sound name.

Slide source category 1 with nonzero result uses its unsigned trailing selector
only for transient feedback. Values 0 through 7 index embedded rows
`0,1,2,4,3,6,5,7` when the external mode equals 1; other modes select a
writable eight-row runtime table. Values at least 8 produce `-1`. A valid
external result-effect record independently gates kind 6 and kind 7, in that
order. The selector does not change the persistent Slide path mesh. Evidence:
`claim.presentation.shared-result-feedback` and
`claim.note.slide-path-presentation-geometry`.

## Effect-list ownership, lifetime, and submission

The effect manager owns ten fixed lists in this update/lifetime order:

| Index | List | Capacity | Cooldown count |
| ---: | --- | ---: | ---: |
| 0 | Bomb | 24 | 0 |
| 1 | Reaction | 24 | 0 |
| 2 | Text | 24 | 0 |
| 3 | Continue | 32 | 0 |
| 4 | AirRing | 16 | 0 |
| 5 | SonicBoom | 16 | 0 |
| 6 | CharaNote | 16 | 0 |
| 7 | CharaBG | 4 | 5 |
| 8 | Mine | 48 | 0 |
| 9 | MineBG | 4 | 5 |

Normal gameplay submission requires active count below capacity and an expired
list cooldown. Cooldown count converts with frame rate 60 or 120 as rounded
`(1000 / frame_rate) * count`, so count 5 becomes 83 or 42 milliseconds. The
staged note-view preload temporarily enables tuple-deduplicating force mode;
normal mode is restored when preload completes.

Each accepted external player is assigned the current process-global 32-bit
sequence key and the key increments with wrap. The executable binds the player
to the selected `MainScene` or `BgScene`, starts it with that sequence and its
transform parameters, and appends it to the fixed list. List updates use the
table order and remove an object only when its own state becomes 3. This proves
trigger, ownership, update, and creation-key order. It does not prove that list
index is final draw order: scene sorting, camera, depth/blend state, external
player geometry, materials, textures, and animation payloads remain downstream
inputs. Evidence: `claim.presentation.shared-result-feedback`.
