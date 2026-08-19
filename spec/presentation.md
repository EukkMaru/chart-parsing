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

## Background SpriteNode and common Sprite quad

`projView::System` embeds an `air::Sprite` in a registered
`air::SpriteNode`. Gameplay-scene construction assigns an externally obtained
render-target handle to that Sprite and names the graph node `BG SpriteNode`.
Its graph callback submits the Sprite through the default/null collector path.
System teardown unregisters the graph node, releases the Sprite resource and
optional UV transform, and destroys its embedded dynamic-primitive utility.

An `air::Sprite` starts with null resource, translation `(0,0)`, dimensions
`16x16`, scale `(1,1)`, rotation 0, UV rectangle `(0,0)-(1,1)`, packed color
`0xffffffff`, no UV matrix, anchor mode 0, and topology selector 3. A successful
resource load replaces the handle and dimensions with the loaded resource's
integer width and height; failure retains a zero handle. Existing-handle
assignment applies the same release/retain and dimension-copy lifecycle.

Sprite draw requests the observed dynamic-primitive setup tuple `(4,3,6,1)`
and emits one six-vertex triangle list. The anchor rectangle is:

| Anchor modes | X range | Y range |
| --- | --- | --- |
| 0 | `[0,w]` | `[0,h]` |
| 1 | `[-w/2,w/2]` | `[0,h]` |
| 2 | `[-w,0]` | `[0,h]` |
| 3 | `[0,w]` | `[-h/2,h/2]` |
| 4 | `[-w/2,w/2]` | `[-h/2,h/2]` |
| 5 | `[-w,0]` | `[-h/2,h/2]` |
| 6 | `[0,w]` | `[-h,0]` |
| 7 | `[-w/2,w/2]` | `[-h,0]` |
| 8 and other values | `[-w,0]` | `[-h,0]` |

Local XY is scaled independently, rotated, and translated in that order.
Positions are emitted as `(x0,y0),(x0,y1),(x1,y0),(x0,y1),(x1,y1),(x1,y0)`;
UVs use the corresponding
`(u0,v0),(u0,v1),(u1,v0),(u0,v1),(u1,v1),(u1,v0)` sequence. A present UV
matrix transforms every pair before submission, and the packed color is copied
to all six vertices.

The direct Sprite draw caller set is complete: the gameplay background node,
font/ruby text, `air::LedObject`, and `EmoteControl`. The presentation owner's
16 checked texture-row wrappers are also Sprite-backed, but they supply
resource handles to Joint descriptors and are not a fifth direct Sprite draw
owner. WindManager, AuraScene, and `star::SglVTFWaterLine` direct dynamic-
primitive paths have separate RTTI/vtable owners outside chart-note
presentation. The render-target pixels, loaded dimensions, texture data, and
final material/pass composition remain external inputs. Evidence:
`claim.presentation.air-sprite-dynamic-primitive-closure`.

## Dynamic-primitive construction-root inventory

The common `DynamicPrimitiveUtil` constructor has exactly 17 direct function
roots in this snapshot. Two chart-side uses survive complete owner
classification: `projView::JointBase` through one of four `air::Primitive`
constructor overloads, and the background `air::Sprite`. Their reachable
geometry, resource admission, callbacks, submission, and teardown are
specified above and in the Joint section below.

The remaining roots are assigned as follows:

| Root family | Recovered owner boundary |
| --- | --- |
| two anonymous owners | sole allocator creates literal `DefaultDebugScene` |
| extended utility constructor | only the default-debug owners, `air::GuiWindow`, and engine `Debug` system window; its default twin is unreferenced |
| `air::GuiWindow` | generic window allocation, `air::GuiPanel`, and `air::GuiMenuBar` |
| non-Joint `air::Primitive` overload calls | RTTI/vtable `air` GUI, graph/camera/light/filter/model/physical/IK debug and editor classes |
| `font::TextBoxObject` | sole owner `font::FontManager` |
| `surfride::SrRenderer` | sole direct owner `surfride::SrPlayer::Impl` |
| `SpkDynamicPrimitive` | SPK debug/font/SGL and temporary particle-drawing functions |
| direct specialized roots | WindManager, AuraScene, registered `star::SglMask`, and `star::SglVTFWaterLine` |

The four generic `air::Primitive` overloads have complete caller counts
2/5/5/5. `projView::JointBase` is the only chart class among those 17 callers;
the other 16 are assigned debug/editor owners, including two helpers reached
only from RTTI `air::ModelDebugWindow`. Thus a generic helper appearance is not
evidence for another note primitive.

This inventory closes construction ownership, not external payload contents.
Surfride animation data, SPK particles, SGL masks, Wind/Aura/water resources,
and final material/pass pixels remain named external rendering boundaries and
may be reopened only from a traced gameplay consumer. Evidence:
`claim.presentation.dynamic-primitive-util-owner-inventory`.

## Dynamic-primitive finalization and teardown

The two dynamic vtables share a configuration slot, pending-write-pointer
accessor, backend-count reset slot, and finalizer. Configuration stores the
layout selector at `+0x08`, topology/mode at `+0x0c`, vertex count at `+0x10`,
and the one-byte submission flag at `+0x1c`. It resolves the layout selector's
external byte stride into `+0x14`, computes the 32-bit product
`vertex_count * stride` at `+0x18`, mirrors selector/mode into `+0x90/+0x94`,
and stores the acquired write pointer at `+0x150`. The accessor returns the
address of that pointer field, which geometry builders dereference before
writing vertices.

The shared renderer resolves selectors through a 17-entry external layout
table and returns zero when the backend is absent. Its allocator records the
selector, topology/mode, and count, rounds the backend's current byte offset up
to a stride boundary, adds two stride units only when topology/mode is 4,
allocates `count*stride` after that prefix, and returns the write address after
the prefix. Concrete layout entries and allocation storage remain external.
The reset virtual clears only the backend's recorded vertex count; it neither
clears owner-local submission state nor releases memory. Evidence:
`claim.presentation.dynamic-primitive-entry-setup-reset-closure`.

The common finalizer is installed in both the RTTI
`sea::DynamicPrimitiveUtil` and `sea::DynamicPrimitiveEntryHelper` vtables.
Its complete non-vtable direct caller set contains only the specialized
WindManager and AuraScene producers; Joint and Sprite reach the same function
through the installed virtual slot. No additional finalizer target is hidden
behind the common construction roots.

For one pending entry, finalization performs this exact ordered state update:

1. OR bit 0 into the payload flags at utility offset `+0xa4`.
2. Replace bit 7 of the submission flags at `+0x80` from the boolean byte at
   `+0x1c`, preserving every other bit.
3. Copy `+0x0c` to `+0x94` and `+0x08` to `+0x90`.
4. Submit the payload at `+0x20` to the optional collector/default command
   path.
5. Clear the pending pointer at `+0x150` after submission returns.

The common two-vertex line helper requests setup tuple `(3,1,2,1)`, emits two
vertices with stride `0x10`, and reaches this same virtual finalizer.

The common teardown function does only one thing: it restores the
`sea::BasePrimitiveModule` vtable. It does not free a buffer, clear a
container, or reset the finalizer fields. The complete non-unwind destructor
caller set maps back to every construction-root family: default-debug and
extended utilities, GUI window, generic `air::Primitive`, font text box,
Surfride renderer, SPK primitive, SGL mask, Sprite, Wind, Aura, and water line.
Compiler unwind handlers cover the same families' partial-construction paths.
Class-local storage is released by the surrounding owner destructor, not by
the shared helper. The Surfride cleanup call is anchored at the direct call
instruction because that small cleanup chunk is not safely represented as a
standalone Ghidra function in this project.

Field semantics beyond their observed offsets, the optional collector's
external command payload, and backend rendering remain unnamed boundaries.
Evidence:
`claim.presentation.dynamic-primitive-finalizer-teardown-closure`.

The common standalone convenience builders are also exhaustively assigned.
They comprise flagged/unflagged 3D two-vertex lines, one flagged 2D triangle,
and flagged/unflagged 3D triangles. Three variants have no live reference; all
live engine calls collapse to a literal RAM/VRAM/draw-call/vertex performance
debug overlay and its private helpers, except the separately owned
`star::SglVTFWaterLine` line producer. No runtime note, projView Joint, or
gameplay-scene owner reaches these builders. Evidence:
`claim.presentation.dynamic-primitive-convenience-helper-owner-closure`.

Primitive topology/mode selection also updates submission flags; it is not
only a geometry label. The common setter stores the low six mode bits, clears
the extended-mode bit for signed modes below 33 and sets it otherwise, clamps
the table lookup to row 61, and applies two executable-owned table properties
to payload bits `0x20/0x40` plus the inverse first property to auxiliary bit
`0x08`. Modes above 32 force payload bit `0x20` after table application.

All chart-reachable values are closed: mode 2 has table properties `(1,1)`,
while modes 3 and 4 have `(1,0)`. Thus mode 2 sets payload bits `0x20` and
`0x40`; modes 3/4 set `0x20` and clear `0x40`; all three clear the extended
`0x800` and auxiliary `0x08` bits. The common finalizer later adds payload bit
0 and replaces bit 7 from its per-entry flag. The setter's 28-call inventory
contains only the closed Sprite and Joint chart paths plus already assigned
GUI/debug/font/Surfride/SPK/SGL/Wind/Aura owners. Evidence:
`claim.presentation.primitive-topology-derived-flags`.

## External presentation-table provenance

The common table loader constructs each path as:

```text
selected_database_directory + "\\" + registered_table_record_name + ".bin"
```

The executable registers and consumes these six presentation-relevant table
families:

| Effective basename | Row stride | Checked invalid-row result | Recovered first consumer |
| --- | ---: | --- | --- |
| `NotesEffectTableRecord.bin` | `0x44` | resource ID `-1` | ordinary result-feedback span/effect selection |
| `NotesCharaEffectTableRecord.bin` | `0x28` | resource IDs `-1`; associated parameters `0` | Slide extended-feedback kinds 6/7 |
| `ModelTableRecord.bin` | `0x14` | shared empty path; auxiliary value `0` | checked model path/resource resolution |
| `ModelSetTableRecord.bin` | `0x30` | checked signed IDs `-1` | model/model-set preload and load selection |
| `TextureTableRecord.bin` | `0x10` | shared empty path | checked texture resource-wrapper loading, including AirLadder |
| `FieldLineFileTableRecord.bin` | `0x20` | missing key or checked signed ID `-1` | field-line selection followed by checked ModelTable lookup |

The basename, row stride, field location, invalid fallback, and executable
consumer are normative. The selected database directory, concrete row values,
resource paths, models, textures, and final presentation payloads are external
and must remain supplied parameters. A clean-room implementation must not
replace a missing row with a visually plausible constant. Evidence:
`claim.configuration.external-presentation-table-provenance`.

## External presentation resource pools

One presentation owner supplies both runtime note-model instances and the
texture wrappers installed by the six recovered Joint initializers. During
power-on population it clears prior contents, checks ModelSet IDs 0 through
407, and creates a keyed pool entry only when the checked
`ModelSetTableRecord` field at `+0x1c` is nonzero. It then creates exactly 16
`air::Sprite`-backed texture wrappers. Wrapper slot `i` selects checked
`TextureTableRecord` row `i` while `i < row_count`; later slots repeat row
`row_count - 1`. A zero-row table selects `-1` for every slot and leaves every
wrapper invalid. Tables longer than 16 still populate only rows 0 through 15.
Each wrapper loads through the Sprite resource path and exposes the resulting
handle to Joint descriptors; the wrappers are not separate direct Sprite draw
owners. A Joint handle lookup accepts only an unsigned index below the current
wrapper count, so negative and out-of-range indices return zero.

Model-set ID `-1` does not produce a pool entry. Other IDs find or append a
0x2c-byte keyed entry. Runtime acquisition reuses the first free instance or
lazily creates and loads one `InstancingModel`; release deactivates the
matching occupied instance and makes it reusable. A tracked ID/handle pair is
cleared only after successful release, and replacement releases the old pair
before acquiring the requested ID.

The separate RTTI `CacheManager` singleton has no direct runtime-note or Joint
resource consumer. It must not be substituted for this presentation owner.
This distinction does not make the external table rows, model paths, texture
paths, meshes, materials, or resource payloads executable-owned. They remain
external inputs, and the asset-free viewer reconstructs their original
primitive roles rather than cloning the proprietary cache. Evidence:
`claim.presentation.model-resource-pool-boundary`.

## Cross-family precompute ownership

Fresh chart setup clears `NotesPreCalcManager` and creates precomputes for only
four accepted parsed types:

| Parsed type | Family | Independent manager map | Object size |
| ---: | --- | ---: | ---: |
| 2 | Slide | `+0x04` | `0x78` |
| 9 | AirLadder | `+0x0c` | `0x78` |
| 10 | AirSolid | `+0x1c` | `0x5c` |
| 13 | HeavenHold | `+0x14` | `0x54` |

Each entry is keyed by the accepted-record identity at parsed `+0x84`. Every
other note type bypasses this manager. The corresponding runtime family uses
an exact unsigned-key lookup in only its own map; a missing key takes the
binary's out-of-range failure path and never selects another entry or a
fallback precompute.

The common clear routine runs before each fresh chart population and from the
recovered scene/gameplay reset paths. It destroys every family-owned object,
releases all map nodes, restores all four sentinels, and zeros all four counts.
Family-local fields and their observable geometry/lifetime consumers remain in
the four note specifications. Evidence:
`claim.presentation.notes-precalc-manager-map-closure`.

## Joint dynamic-primitive ownership

The executable has one common `projView::Joint` graph node. Construction
registers it with the scene graph, initializes topology 4 and a zero resource
handle, and reserves 256 vertices of 0x18 bytes. Its graph callback submits
only when the runtime resource handle is nonzero and the vertex vector is
nonempty with cardinality divisible by three. Submission copies the descriptor
and vertices into dynamic-primitive state; a null optional collector reaches
the adjacent-compatible batch check and default command queue. Destruction
releases the vertex vector and unregisters the graph node.

The complete constructor-owner set is:

| Wrapper | Reachable owner | Child count | Topology/mode |
| --- | --- | ---: | --- |
| `JointSlide` | Slide precompute | 3 | `[4,3,3]` |
| `JointAirSlide` | AirLadder precompute; AirSlide runtime | 3 | `[3,3,2]` |
| `JointAirSolid` | AirSolid precompute | 1 | `3` for a submitted primitive |
| `JointHeavenHold` | HeavenHold precompute | 1 | `4` |
| `JointHold` | Hold runtime | 1 | `4` |
| `JointField` | no reachable owner | 1 | `0`, excluded |

For the shared Air-path initializer, the first topology is 3 for selector 8 or
9 and 4 otherwise; the remaining topologies are 3 and 2. AirLadder supplies
literal 9 and AirSlide supplies literal 8. Thus both exact reachable triples
are `[3,3,2]`; the former AirLadder `[4,3,2]` transcription is superseded.

The Field wrapper is retained as a negative path: its sole allocation helper
has no incoming reference in this snapshot. External texture rows, materials,
and final pixels remain unavailable resource inputs, but every Joint-backed
chart producer, callback, topology, submission boundary, and teardown is
assigned. Evidence:
`claim.presentation.joint-dynamic-primitive-producer-closure`.

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

Hold and HeavenHold modes 0/1 use base white and mode 2 uses alternate gray.
Slide's main stream follows that selector, but its fixed-width center stream
uses base white in every mode. AirSolid always uses base white. The shared Air path builder uses base
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

Before those two submissions, `CharaEffectManager` computes two independent
mutating admission flags against current NotesManager presentation time. Kind
6 receives the lane-overlap flag. It owns 32 half-lane subcell expiries and the
following footprint table for decoded widths 1 through 16:

```text
2, 4, 6, 8, 2, 12, 2, 16, 2, 2, 2, 2, 2, 2, 2, 32
```

Every width uses duration `1.0`. For lane `l`, width `w`, and footprint `p`,
the reserved endpoints are `2*l + w - p/2` and
`2*l + w - 1 + p/2`, each clamped to 0..31. Invalid lanes or `l+w > 16` are
rejected. The comparison loop checks expiry strictly greater than current time
over `[first,last)`, but acceptance writes `current+1.0` over
`[first,last]`. This last-cell asymmetry is normative.

Kind 7 receives a separate global flag. It rejects while current time is
strictly below the retained next-admission value; otherwise it stores
`current+7.0` and accepts. The two gates are both evaluated and reserve state
independently, so failure of one does not prevent mutation by the other.
Gameplay reset and the final preparation transition clear all 32 expiries and
the global value. Evidence:
`claim.presentation.slide-extended-feedback-admission`; reconstruction:
`check_and_reserve_slide_chara_effect_lane_overlap` and
`check_and_reserve_slide_chara_effect_global_cooldown`.

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

Normal gameplay admission requires the list's submissions-since-update counter
to be below capacity and its cooldown to be expired. This is not an
active-occupancy comparison. On a non-forced append, active occupancy already
at capacity causes the oldest effect to be stopped, marked terminal, and
removed immediately before the new effect is appended. The cooldown then
restarts. A forced/preload append skips both eviction and cooldown restart, so
forced active occupancy can exceed capacity. Cooldown count converts with frame
rate 60 or 120 as rounded `(1000 / frame_rate) * count`, so count 5 becomes 83
or 42 milliseconds. The staged note-view preload temporarily enables
tuple-deduplicating force mode; normal mode is restored when preload completes.

Each accepted external player is assigned the current process-global 32-bit
sequence key and the key increments with wrap. The executable binds the player
to the selected `MainScene` or `BgScene`, starts it with that sequence and its
transform parameters, and appends it to the fixed list. List updates use the
table order, erase objects whose state reaches 3, record maximum post-erase
occupancy, and reset the submissions-since-update counter. Normal reset and the
final shader-preparation transition clear all ten lists, their cooldowns,
maximum occupancy, and accepted-total counters. The clear slot does not itself
zero submissions-since-update; the shader path updates first, and ordinary
updates own that counter reset. This proves trigger, ownership, update,
capacity eviction, reset, and creation-key order. It does not prove that list
index is final draw order: scene sorting, camera, depth/blend state, external
player geometry, materials, textures, and animation payloads remain downstream
inputs. Evidence: `claim.presentation.shared-result-feedback` and
`claim.presentation.effect-list-lifecycle-closure`.

Each appended handle is a four-state executable wrapper around that external
player. Construction begins with current and pending state `-1`, visibility
true, and stopped false. Start queues state 1. On each unpaused update, a
pending transition is applied first; state 1 update queues state 2, and state 2
update queues state 3 when the external player lookup no longer returns an
instance. State 3 is terminal. All enter/exit callbacks and the state 0/state 3
update callbacks are no-ops.

The list tests current state after updating, not pending state. Natural
external disappearance therefore takes two list updates to remove: the first
queues state 3 while current remains 2, and the second applies 3 before the
terminal check. An explicit stop queues state 3 idempotently; because note work
precedes the ordinary all-list update, that later update can apply and remove
the stopped object in the same outer gameplay pass. Capacity eviction removes
the oldest list handle immediately instead.

The wrapper retains a visibility byte. Visible clears bit `0x4` on the
external instance, hidden sets it, and the wrapper reapplies the bit after
every update. The Slide-retained effect position setter submits an identity
4x4 matrix whose translation is the supplied `(x,y,z)` in elements 12..14.
Slide construction and parsed-record loading also arm a one-shot latch with
`-1`. The first retained-handle update in Slide presentation phase 2 shows and
repositions the player, selects external resource entry 0, and clears the
latch; non-phase-2 updates hide it without consuming the latch. No reachable
writer rearms it until another construction/load.

The loading-only Slide object independently walks every retained handle at
resource steps 11, 12, and 13, applying visible plus entry 0, visible plus
entry 1, then hidden plus entry 1. Steps 14..19 do not control those handles,
and step 20 declares the Slide preload ready. Entry selection uses the loaded
external player table and fallback, so the request IDs and timing are exact
while their mesh/animation payload remains external. External payload also
determines geometry and natural instance lifetime. Evidence:
`claim.presentation.effect-player-state-machine-closure`; reconstruction:
`FeedbackEffectPlayerState`, `update_feedback_effect_player_state`,
`stop_feedback_effect_player_state`, `apply_feedback_effect_visibility_flag`,
`feedback_effect_translation_matrix`,
`update_slide_retained_feedback_control`, and
`slide_preload_feedback_control`.
