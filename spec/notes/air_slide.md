# AirSlide secondary note

## Parser attachment and control markers

`ASD` and `ASC` resolve to secondary parsed type 8 and share an eleven-field
schema. Their command IDs are `0x27` and `0x28`; the parser stores
`command_id == 0x27` on each 0x24-byte control point. This specification calls
that byte the ASD marker because the identity is executable data, not because
of an assumed player-facing meaning.

The sixth field names the record family to attach to or continue. For an
initial attachment, the parser searches for a compatible root at equal current-
endpoint chart position, lane, and width with an unused secondary slot. HOLD
supplies its endpoint and Slide its current final path record; point roots'
current endpoint equals their start. The parser installs type 8 and
appends the current control point. References to ASD or ASC instead select the
continuation path: the existing type-8 control vector must be nonempty, its last
position/lane/width must match, and its last ASD marker must equal the
referenced family. A failed root or continuation match takes the parser
diagnostic path.

The runtime factory allocates a separate `projView::AirSlideNote`, links it
from the root, appends the root to the primary vector, and appends AirSlideNote
to the separate secondary vector. The secondary owns its vectors, checkers,
phases, gap state, and deferred lifetime; every primary updates before it.
Evidence: `claim.note.air-slide-secondary-judgement`.

## Start and contact components

The root-relative start anchor policy and retained-profile classifier are the
same as AirHold:

- root start for root types 0, 4, 6, and 11;
- root end for type 1; and
- the last root control point for types 2 and 13.

The parser leaves the start profile-group discriminator at its reset value.
Roots 1/2/13 therefore select external profile 0, roots 0/4/11 profile 2, and
root 6 profile 4. An accepted start submits source category 9 and moves the
start phase to 5.

Sustain reads derived profile 7 and then profile 6. It owns the same admission
latch as AirHold:

```text
admitted = admitted || !profile_7 || profile_6
active = start_reached && admitted && profile_7
```

The inactive-gap state is the same four-threshold tracker used by HOLD and
AirHold, but AirSlide has its own external gate, floor, end, and selected
threshold interface. Reconstruction: `AirSlideContactState`,
`update_air_slide_contact`, and `air_slide_start_profile`.

AirSlideNote never exposes a lane candidate. Its derived profiles and scheduled
secondary records neither constrain nor consult manager lane reduction.

## Generated path construction

Grid positions use the single-precision formula:

```text
grid_tick = trunc((major + minor * 0.25F) * 384.0F + 0.5F)
```

The conversion uses `CVTTSS2SI`: NaN, infinity, and out-of-range values become
`INT32_MIN`. Cursor initialization, restart, and advance use wrapped 32-bit
addition before the signed `cursor < end` test.

The adaptive step starts at 384 ticks. While the BPM selected for the current
scheduled milliseconds is below four times the `PROGJUDGE_BPM` header, double
the BPM and integer-halve the step. The authoritative BPM schedule and
position conversion are specified in `spec/timing.md`; path interpolation
remains an explicit clean-room interface.

Sampling maintains one cursor across control segments:

1. Initialize it one adaptive step after the root and mark the next sample
   disabled.
2. Emit type-5 samples only while the cursor is strictly before the current
   control point. Advance by a newly selected adaptive step after every sample.
3. If a completed control point is ASD-marked and another segment follows,
   append that point as a disabled type-6 boundary, restart the cursor one step
   after it, and mark the next sample disabled.
4. If that point is ASC, append no boundary and carry the already computed
   cursor into the next segment.
5. Append the final control point once as a disabled type-7 end record.

Thus the first sample after the root or an ASD restart is disabled; later
samples are enabled until another restart. Types 5/6/7 are producer tags. The
runtime consumer uses only schedule and emission for gameplay.

The control-point comparison is signed. In the ordinary nonwrapping domain, a
control point before the current cursor contributes no interior samples;
processing continues and the final authored control is still appended as the
disabled type-7 end record. At an extreme tick, however, anchor-plus-step or a
later cursor advance can wrap from positive to negative and create a very
large source expansion. A zero adaptive step can hold a cursor below the end
forever. `evaluate_air_slide_segment_generation` and
`evaluate_air_slide_cursor_advance` expose those source dispositions.

A final ASC enables a nonnegative end-margin filter; final ASD bypasses it. The
margin is `PROGJUDGE_AER`, which resets from float bits `0x3f7fbe77`. For each
record it disables emission when:

```text
record_grid_tick + round(adaptive_step * end_margin) >= final_grid_tick
```

The margin conversion is `CVTTSS2SI`; positive infinity/out-of-range maps to
`INT32_MIN`, and record-plus-margin wraps before the signed comparison. NaN and
negative margins bypass the filter.

Positive `TUTORIAL` enables the separate key-0 interval table, which also
disables emission only for an interval with selector zero and strict
`start < scheduled < end`. Both filters are one-way. Reconstruction:
`generate_air_slide_path_records` and
`filter_air_slide_path_emissions`.

## Generated and authored judgement

One substep considers only the generated vector front and consumes at most one
due record. Ordinary mode classifies the retained maximum inactive gap through
anonymous bytes `4, 3, 2, 1, 0` and resets the maximum to any open current gap
for every due record. It tests the emission byte only afterward. Consequently,
a disabled record submits no result but still performs the ordinary reset. An
enabled record submits source category 11. Forced selection supplies its fixed
byte and bypasses the ordinary reset. Reconstruction:
`update_air_slide_generated_checkpoint`.

Only ASD-marked controls are inserted into the authored-checker index. Each
uses the AIR-style retained-profile checker; only the current index is updated,
so at most one resolves per substep. A resolved ASD submits source category 13
when it is also the final control point, otherwise category 12. An ASC never
owns one of these checkers. Reconstruction:
`air_slide_authored_checkpoint_count` and
`air_slide_authored_source_category`.

The path phase reaches 5 only when the generated vector is empty and all
indexed ASD checkers are complete. Deferred terminal transition additionally
requires start phase 5. Categories 9, 11, 12, and 13 map to shared categories
3, 5, 7, and 7 and are aggregate-authoritative under normal valid runtime
bounds. Reconstruction: `air_slide_path_complete`,
`air_slide_is_terminal`, and the `air_slide_*_source_category` constants;
focused tests: `tests/air_slide_judgement_test.cpp`.

## Presentation vertical transforms

Authored AirSlide control verticals use exact common transform
`(value - 1) * 3.8934999`. ASD action/checkpoint resource origins use the same
transform plus `0.14999962`. A type-13 attachment may replace the root-side
initial value from the root's final control; otherwise the constructor default
is 1. Resource contents and final pixel extent remain external. Evidence:
`claim.presentation.common-air-transform`; reconstruction:
`common_air_render_vertical` and `common_air_action_render_vertical`.

## Presentation resources and authored path

The active AirSlide wrapper always runs gameplay/lifetime before presentation.
The presentation half returns immediately only after the same exact terminal
predicate used by lifetime handling: start phase 5 and path phase 5. Before
that conjunction, the root resource is visible while start phase is not 5.

Root placement and scale deliberately use two different vertical values. The
ordinary placement value is one; a type-13 attachment replaces it with the
final authored control's integer-tenth vertical. Root vertical placement is
the common Air transform of that value. Its vertical scale instead transforms
the authored root vertical and multiplies by exact `0.06420958`. Lateral
placement is the common lane center, depth is initially `-10000`, and lateral
scale is decoded width divided by a positive external native resource width,
else one. Per-frame external x/y/z offsets are added after those base values.

Load constructs one `0xac` runtime segment for every authored control. Each
record retains both previous- and current-endpoint data. The continuous path
is therefore root followed by all authored controls: the loader emits the
previous side of each segment and marks only the last segment's current side
as the final endpoint. The generated judgement vector does not supply this
mesh. At update, parallel arrays carry adjusted schedule minus manager current
and raw schedule minus manager current in the same root-then-control order.
Every control's lateral center and projected depth are recomputed before the
shared type-8/9/13 geometry call. Clipping, neutral vertex fields, extents, and
winding are the exact shared rules reconstructed in the AirLadder section.

Only ASD-marked control indices own authored action resources. Each owns two
width-indexed external resources at common lane center, common action vertical,
and projected control depth. Both use decoded-width/native-width lateral
scale. The first has unit vertical scale; the second has exact vertical scale
`common_action_vertical * 0.063597046`. An unresolved ASD projects its own
schedule/tag and shows/updates both resources; resolution hides both. ASC
controls still shape the authored mesh but never enter this action-resource
index.

Path phase 3 selects shared geometry mode 1 and writes the shared field-
feedback flag. Phase 4 selects mode 2; all other phases select mode 0. The
primary streams use exact white `0xffffffff` in modes 0/1 and exact gray
`0xff666666` in mode 2; the third stream always uses low-alpha white
`0x40ffffff`. The
parsed appearance field maps `0 -> 2`, `1 -> 0`, `2 -> 1`, defaulting to zero,
then selects external resource entries at row offsets `r`, `r+3`, and `r+6`.
AirSlide uses stream topologies `[3,3,2]`. Diagnostic primitive counters are
called as categories `[4,6,5]` for streams `[0,2,1]`; this diagnostic order is
not a draw-order contract. Root and ASD resource animations advance only when
the common adjusted-root near/range predicate accepts the frame.

Preload steps 0-2 walk all sixteen width slots for two root tables and the ASD
action table. Steps 3-6 load shared groups 9, 10, 12, and 13; steps 7-12 are
idle, and a step above 12 latches readiness. Maintenance finalizes both handles
for every ASD index, then the root and shared geometry wrapper. Reset and
destruction close both shared child collections, all per-control records and
resources, schedule arrays, root resource, and base-family state.

External resource identities, native dimensions, materials, and textures are
not present in the executable and are not copied. Their width/style selectors,
transform consumers, lifetime, and exact fallback behavior are normative.
Evidence: `claim.presentation.air-slide-model-path`; reconstruction and focused
tests: `air_slide_*` helpers and `tests/air_slide_presentation_test.cpp`.
