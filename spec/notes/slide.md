# Slide notes

This section specifies parsed type 2, produced by `SLD`, `SXD`, `SLC`, and
`SXC`. Gameplay state and the recovered resource-independent presentation
classes are normative. External resource contents remain excluded.

## Parsing and path ownership

Each command supplies a `0x24`-byte control point. A command continues an
existing type-2 parsed record only when all of these match its last point:

- the parsed connection field;
- mirrored logical lane origin;
- bounded width;
- chart position under the parser's existing float-position equality helper.

The descriptor accepts the six-field legacy form. Control construction first
copies the root's encoded width. The tested vector count includes the command
token, so more than seven total tokens means seven or more data fields and
reads/clamps field 7 as a distinct endpoint width. Only the six-data-field
legacy form retains the root width. Missing endpoint width therefore means
“inherit root width,” not width 1. This field-count branch is independent of
the common root-width clamp.

No match creates a new `0x174`-byte parsed record and appends its first point.
The command-specific field pairs are:

| Command | First field | Second field |
| --- | ---: | ---: |
| `SLD` | 0 | 1 |
| `SXD` | 1 | 1 |
| `SLC` | 0 | 0 |
| `SXC` | 1 | 0 |

These names are structural; do not infer user-facing mode semantics from the
bits. The type-2 postprocessor converts the full control-point vector to a
manager-owned runtime path container and inserts it under parsed key `+0x84`.
`SlideNote` retains that key and performs checked lookup on every path use. It
copies checkpoints but not the generated path container. A missing key raises
the source's standard out-of-range exception; it does not construct a fallback
container or treat the path as disabled. Reconstruction:
`require_slide_generated_path`.

The second command-specific field is also a path-boundary marker:
`SLD`/`SXD` set it and `SLC`/`SXC` clear it. The generated root boundary and
final boundary are forced set; intermediate boundaries retain the authored
field. These markers control one-shot path feedback, not shared judgement
results. For ordinary Slide, an ending marker also exactly controls ownership
of the persistent generated endpoint resource described below.

One exact style is an exception to this type-2 path. Field 8 is decoded by an
exact three-string table: `SLD` is code 0, `HLD` is code 1, and `GRN` is code
2; empty, differently cased, or otherwise unrecognized values also become
zero. After all commands have been joined into chains, but before generated
paths are built, every type-2 chain with style code 1 is changed to type 13.
The pass writes path-scalar integer 10 to root `+0x30` and to `+0x18` of every
control point; HeavenHold precompute consumes those fields as scalar `1.0`.
It does not write the separate presentation selector at parsed `+0xb0`, which
retains zero. The pass preserves the command-form field: SLD/SLC keep zero,
while SXD/SXC keep one. These rewritten records bypass `SlideNote` and follow
the HeavenHold generation, factory, input, judgement, and presentation path
specified in `spec/notes/heaven_hold.md`.

Evidence: `claim.note.slide-path-sustain-judgement` and
`claim.note.slide-hld-heaven-retyping`.

## Presentation classes and selectors

The root/start resource, three shared path resources, and generated endpoint
resources are distinct owners.

- The root resource is selected from bounded width and decoded style. A root
  authored as SXD/SXC enters the extended resource branch; SLD/SLC enters the
  ordinary branch. The extended branch has a further runtime two-table
  selector. Actual resources and that selector's player-facing label are
  external or unresolved, so reconstruction retains the branch rather than
  assigning a guessed image.
- Style is bounded to codes 0 through 2 before selecting the three shared path
  resource descriptors. Their primitive topology/mode values are 4, 3, and 3.
  Exact `HLD` style has already left this path through the type-13 rewrite.
- Every generated segment stores its preceding marker, ending marker, and
  final-segment byte. The final ending marker is forced set. A generated
  endpoint resource is preloaded and lazily allocated if and only if the
  ending marker is set. The allocator decodes the ending control width stored
  at generated `+0x14`; the preceding/start width at `+0x10` is not its width
  selector. Thus a shrink-point resource uses the post-shrink width, nonfinal
  SLD/SXD endpoints own the visible class;
  nonfinal SLC/SXC controls are path-shaping only. The root start is separate,
  and the final endpoint is present regardless of the last command spelling.
- Each generated endpoint begins with result-table index `0xff`. Presentation
  narrows the loaded `NotesJudgeResultTable` row count to one byte and requests
  the endpoint visible exactly when the stored index is not below that count.
  A due marked endpoint receives the current mapped result index. A due
  unmarked segment stores hardcoded index 4 but owns no endpoint resource.
  External rows/count must be explicit inputs for any simulated resolved state.

Optional field 9 is decoded only for SXD/SXC roots by the exact string order
`UP`, `DW`, `CE`, `RC`, `LC`, `RS`, `LS`, `BS`; missing or unknown values
become zero. It is copied into runtime root state and selects an entry from one
of two eight-entry result-feedback tables. Values outside unsigned range 0..7
select no resource. It has no persistent path-geometry or endpoint-class
consumer.

Evidence: `claim.note.slide-presentation-classes`; reconstruction:
`slide_generated_endpoint_resource_present`,
`slide_generated_endpoint_decoded_width`,
`slide_generated_endpoint_resource_visible`, and related Slide presentation
helpers; focused coverage: `tests/slide_path_test.cpp`.

### Shared path mesh

The type-2 builder supplies the shared path owner with one presentation point
for every generated segment start and one forced final endpoint. Each point is
the decoded width, `lane + width / 2` center, and preceding boundary marker.
The runtime update independently supplies equal-length raw and projected
position arrays for the root and every generated endpoint. Projection follows
the endpoint's keyed schedule through the shared transform specified in
`spec/timing.md`. If any of the three cardinalities differ, all three path
streams are cleared.

Adjacent points form segments. Render lateral center is
`(lane_center - 8) * 4`; decoded width is retained separately. The first start
marker and final end marker are forced. Between two ending markers, the
longitudinal coordinate accumulates absolute raw segment length and divides by
the absolute difference from the group's first raw start to its last raw end.
The denominator is at least `0.00001`, and coordinates are clamped to `[0, 1]`.

The first segment satisfying both `raw_start < 0.000001` and
`raw_end > -0.000001` is split at the projected judgement plane. Lateral
center, width, and longitudinal coordinate are interpolated there. The past
copy stores raw start/end zero and clears its end marker; the future copy
clears its start marker.

Presentation mode is derived from the two Slide gameplay phases exactly:

| Start phase | Path phase | Mode |
| --- | --- | ---: |
| any | 2 | 1 |
| 4 | 3 | 2 |
| 4 | any other value | 1 |
| any other combination | any other value | 0 |

Mode 1 removes a segment when its raw end is below `0.000001`; modes 0 and 2
do not apply that removal. In every mode a segment is discarded if both
projected endpoints are below -600 or both are above 50. A segment crossing
either bound is clipped to exact range `[-600, 50]`, interpolating lateral
center, width, and longitudinal coordinate from the original endpoints.

The clipped segments fill three `0x18`-byte vertex streams in Joint construction
and callback order 0, 1, 2:

- Stream 0 is a vertical-zero, full-decoded-width trapezoid. Equal endpoint
  widths emit six vertices. An absolute width difference at least
  `1.1920929e-7` emits 18 vertices: a center quad at width scale `0.7` and
  horizontal coordinates `0.15`/`0.85`, plus two side strips reaching 0/1.
- Stream 1 is a vertical-zero six-vertex strip of fixed render-space half
  extent 2, exactly one chart lane in total width. It uses exact base white
  `0xffffffff` in every presentation mode; only stream 0 uses the mode-dependent
  white/gray selector.
- Stream 2 is a vertical-zero, full-width six-vertex overlay emitted only in
  mode 1, with packed color `0x20ffffff`.

Triangle winding depends on projected endpoint order. Diagnostic categories
1, 2, and 3 count triangles only; they are not layer identifiers. Static
initializer functions construct exact packed colors `0xffffffff` (base white),
`0x40ffffff` (shared low-alpha white), and `0xff666666` (alternate gray).
Mode 0 reads base `0xffffffff`. Mode 1 uses the same color, adds stream 2, and
applies intensity
`sin(fmod(counter * 0.05, 1) * 2*pi) * 0.25 + 1.5`. Mode 2 reads a separate
exact `0xff666666` and omits stream 2. Materials, textures, shaders, and final
pixel composition remain external presentation inputs; the three packed colors
do not.

Evidence: `claim.note.slide-path-presentation-geometry`; reconstruction:
`build_slide_presentation_geometry`, the three
`build_slide_*_stream_vertices` helpers, and related constants; focused
coverage: `tests/slide_path_test.cpp`.

## Construction and start judgement

The factory creates a `0x294`-byte `projView::SlideNote`. It has independent
start and path phases, both initialized to zero.

Until start phase becomes 4, candidate construction and input judgement are the
shared TAP operations: per-lane candidates, manager-selected equality, logical
rising edge, timing classification, forced resolution, active-result control,
and source category 0/1 selection all retain their TAP definitions. Resolving
the start submits a result and sets start phase 4, but does not terminate the
slide. Start candidates cease after that phase.

## Generated path windows

The builder bounds each endpoint width to `[1, 16]`, then selects one external
five-float profile at index `16 - width`. Profile values come from the active
runtime configuration block beginning at `+0x730` and are explicit
reconstruction inputs. The first value defines a lane-anchor offset
`(value - lane) * 0.5`; the remaining four are relative window endpoints. A
separate runtime correction is added to all endpoint values.

For each adjacent path-point pair, a lane window exists throughout the swept
half-open corridor between their lane/width spans. Lanes inside the start span
use its first two profile endpoints directly; lanes outside it receive a
distance-weighted interpolation between the points. The final two endpoints
are constructed the same way from the end span. The container-wide window uses
the root's first two endpoints and the final point's last two endpoints. The
exact implementation is `build_slide_generated_path`.

Every enabled path record and nested lane window has four ordered float
endpoints `(outer_early, center_early, center_late, outer_late)`. Classification
is literal:

| Condition | Phase |
| --- | ---: |
| disabled | 0 |
| `current < outer_early` | 1 |
| `outer_early <= current < center_early` | 3 |
| `center_early <= current <= center_late` | 5 |
| `center_late < current <= outer_late` | 4 |
| `outer_late < current` | 2 |

Only path records in phases 3, 4, and 5 participate. For each logical lane,
classify its nested window in every participating record and retain the maximum
numeric phase. Thus phase 5 dominates 4, and 4 dominates 3.

The exact classifiers are `classify_slide_window` and
`combine_slide_window_phases`.

## Two-bank path contact

Each lane owns the same two `armed` and two `active` bytes reconstructed for
HOLD. In phases 3, 4, and 5, apply the HOLD source-continuation rule exactly:
an already-held bank requires a previous sustain marker, while a released bank
arms for a later held sample.

- In phase 5, `active[bank] = armed[bank] && current_held[bank]`; active banks
  write current sustain markers.
- In phases 3 and 4, preserve arming and clear both active bytes.
- In phases 0, 1, and 2, clear both armed and both active bytes.

Any active center bank supplies true activity to the inactive-gap tracker.
Forced mode also supplies true independently of source input. Exact
reconstruction: `update_slide_lane_sources` and `slide_gap_active`.

## Gap tracking, checkpoints, and phases

The embedded tracker is the shared `HoldGapState`, with elapsed position
relative to adjusted slide start. Its configured floor, update gate, end,
threshold comparison, retained maximum, forced activity, result-byte mapping,
and post-checkpoint reset are identical to the HOLD rules.

Before absolute adjusted start, path phase is 0. At or after that start:

- inspect only the first `0x20`-byte checkpoint;
- it is due when `time * 0.06 + runtime_correction <= current`;
- ordinary mode maps the retained gap index through `{0:4, 1:3, 2:2, 3:1,
  other:0}`; forced mode replaces that byte through the shared selector;
- apply active-result control;
- map checkpoint type 2, 3, or 4 to source category 4, 5, or 6; use -1 for any
  other type;
- submit only when the checkpoint emission byte is nonzero;
- regardless of emission, reset the retained maximum to the current open gap
  and remove exactly one front entry.

If checkpoints remain, current gap index 0 gives path phase 2 and every other
index gives phase 3. An empty checkpoint vector gives phase 4 immediately; no
synthetic end checkpoint is added.

Generated path segments have a separate one-shot feedback consumer. A due
segment whose ending boundary marker is set reads the current ordinary or
forced gap grade and routes category 2 to feedback/resource lookup only. It
does not call the shared result dispatcher, apply active-result control, reset
the gap tracker, consume an authored checkpoint, or change completion. The
final generated segment is always marked. This path is excluded from normative
judgement output.

The update order is start component, path component, then completion check.
Both component phases must equal 4 before the note requests base state 2. That
request follows the shared deferred lifecycle: commit and removal occur in a
later manager stage. If start and a checkpoint resolve in the same substep, the
start result is routed first.

Exact reconstruction: `SlideCheckpointProgress`, `update_slide_checkpoints`,
and `slide_is_terminal`. Evidence:
`claim.note.slide-path-sustain-judgement`; tests:
`tests/slide_path_test.cpp`.
