# Gameplay timing

## Authoritative gameplay clock

The active-note manager does not consume raw wall time. On each enabled outer
update, the source maintains four float positions: raw, previous smoothed,
smoothed, and accumulated gameplay position.

The base is a current music-playback cursor. Its adapter queries a registered
handle for a nonnegative 64-bit cursor and a positive cursor-unit rate, then
computes:

```text
music_cursor_milliseconds =
    trunc(double(cursor) / double(cursor_units_per_second) * 1000.0)
```

Query failure, a cursor with its sign bit set, or a nonpositive rate yields
`UINT64_MAX`. Gameplay initialization narrows the unsigned result through
double to float, stores it as the scene base, and resets its elapsed timer.
The final pre-active state samples the same cursor again after its priming
update and enters live state only when the narrowed float is positive.

The elapsed timer is backed by `QueryPerformanceCounter`, accumulated at
microsecond scale, integer-divided by 1000, and exposed through its low 32
bits. A timer flag substitutes a captured integer value instead of reading the
live timer; both paths subtract the timer origin modulo `2^32`. The result is
converted as unsigned. Raw position is computed in source operation order as:

```text
raw = float(music_cursor_milliseconds) * 0.06
    + float(unsigned_elapsed_milliseconds) * 0.06
```

Thus the recovered chart scale is 0.06 units per millisecond, or 60 units per
second. Exact reconstruction helpers are `music_cursor_milliseconds`,
`music_cursor_base_time_value`, `elapsed_milliseconds_modulo`, and
`make_raw_play_position`. Evidence:
`claim.timing.music-cursor-base-time`.

For finite values, let `S` be the previous smoothed position and
`E = raw - S`. The ordinary quantizer produces `Q` as follows:

```text
E < -0.5          Q = S + floor(E + 1.5)
-0.5 <= E <= 2.5 Q = S + 1.0
2.5 < E           Q = S + floor(E - 0.5)
```

The alternate-mode quantizer is:

```text
E < -0.25                 Q = S + floor(2*E + 1.5) * 0.5
-0.25 <= E <= 1.25       Q = S + 0.5
0.9 <= E < 1.25          Q = float(double(S) + 0.8)  (takes precedence)
1.25 < E                  Q = S + floor(2*(E - 0.5) + 0.5) * 0.5
```

When a paired runtime flag condition is active, the quantizer and correction
are bypassed and `S` advances by `1.0` in ordinary mode or `0.5` in alternate
mode. The flags' player-facing meaning is not yet assigned.

Outside that bypass, if `abs(Q - raw)` is strictly greater than the initialized
threshold `0.1`, `Q` moves toward raw by the initialized step `0.01`; the inner
branch snaps to raw when it lies within the step. A discrepancy equal to `0.1`
is unchanged. Accumulated gameplay position then advances by:

```text
gameplay += (smoothed - previous_smoothed) * runtime_rate_factor
```

The rate owner is the process-level RTTI-identified
`projView::ViewTimingManager`. Gameplay-manager construction initializes its
enable byte to false and all five scalar fields to `1.0`. The executable's
complete reference set contains no internal writer for that singleton: normal
control flow therefore uses rate `1.0`. For completeness, the guarded branch
would select the product of the floats at object offsets `+0x0c` and `+0x10`:

```text
runtime_rate_factor = enabled ? factor_a * factor_b : 1.0
```

The same disabled object guards the paired fixed-increment bypass. The object
is created and destroyed with the gameplay manager, but ordinary manager/chart
reset does not recreate or modify its scalar header.

At the active-note manager boundary, the position used for TAP delta and other
note work is:

```text
manager_position = manager_base_offset
                 + gameplay_position
                 + substep_offset
                 - selected_runtime_correction
```

The correction selector is the current bounded `PlayOptionSet` ID. It indexes
records of size `0x38` in the process-owned external table
`PlayOptionPlayTimingOffsetTableRecord.bin` and reads a double from record
offset `+0x10`. The manager narrows that double to float before subtraction.
An absent table or out-of-range set ID returns zero.

The table loader clears prior records, prefers the named file under an existing
optional override directory, and otherwise uses the supplied base directory.
Its lazy singleton is destroyed independently from gameplay/chart resets. The
resource is absent from this workspace and `music.zip`, so values and directory
roots remain reconstruction inputs. Evidence:
`claim.timing.gameplay-clock-reconstruction` and
`claim.timing.play-timing-runtime-ownership`.

## C2S tempo and meter schedule

Gameplay parsing owns one authoritative mapping from chart positions to
scheduled milliseconds. A chart position is represented by canonical floats
`major` and `minor`, with scalar value:

```text
position_scalar = major * 4.0F + minor
```

The chart reset fixes the parser resolution at 384. Although `RESOLUTION` is a
recognized header descriptor, this executable's header handler has no case for
it; the local corpus also supplies 384 uniformly. For nonzero resolution, an
authored integer pair is normalized by the following source-level operations:

```text
grid = floor((authored_major + authored_minor / resolution)
             * 4.0F * 384.0F + 0.5F) / 384.0F
canonical_major = floor(grid * 0.25F)
canonical_minor = grid - canonical_major * 4.0F
```

`BPM` is command ID 13 with integer major/minor and float BPM. The parser scans
all BPM commands in a dedicated pre-pass, then sorts their 0x14-byte records
before parsing MET or any note event. Ordering uses:

```text
left_scalar + 1/192 < right_scalar
```

Let `P[-1] = 0`, let `B[-1]` be the first sorted record's BPM, and let
`T[-1] = 0`. For record `i`, the finalizer rounds the scalar delta on its
internal 1/384 grid and assigns:

```text
T[i] = T[i-1] + rounded(P[i] - P[i-1]) * 60000.0F / B[i-1]
```

The first record therefore uses its own BPM from chart zero to its position.
For a target chart position, the schedule lookup scans backward for the latest
record satisfying:

```text
record_scalar <= target_scalar + 1/192
```

It returns that record's cumulative milliseconds plus the rounded position
delta times `60000 / record_BPM`. An empty vector or a target before every
qualifying record returns zero. BPM changes do not retroactively affect the
preceding interval.

`MET` is command ID 14 with integer major/minor and two integer meter fields.
It is parsed only after BPM finalization and receives its scheduled value from
the same lookup. Meter postprocessing sorts and deduplicates MET positions,
synthesizes position zero from `MET_DEF` when necessary, and creates four
scheduled meter/grid vectors. A zero meter component stops further subdivision
generation. These values do not enter the BPM lookup, note schedule fields, or
adaptive Air cadence; their traced consumers maintain meter/grid index and
interpolation state.

The outer gameplay update has one direct generated-grid consumer immediately
after lazy materialization. It selects projected positions from one meter/grid
vector into a stack-local float list and submits that list to view-marker
rendering. The list is destroyed in the same call, the renderer returns no
gameplay value, and neither stage writes the clock, pending queue, active notes,
input, candidates, results, or terminal state. This post-materialization call
therefore adds no current- or next-substep gameplay dependency. Evidence:
`claim.pipeline.meter-grid-render-boundary`.

The following outer-update pass belongs to RTTI-identified
`projView::KeyBeamManager`. Each update clears its transient markers before
input synthesis and note ticks; note callbacks repopulate lane spans and effect
flags, then the post pass samples the already-completed 16-lane held snapshot.
It converts those inputs to retained beam-view states and keyed feedback
resources. The input history is owned and produced independently, no
KeyBeamManager field is read by input or judgement, and the downstream calls
return no gameplay value. Beam/resource feedback therefore creates no current-
or next-update gameplay dependency. Evidence:
`claim.pipeline.key-beam-feedback-boundary`.

Between the key-beam pass and cue scheduler, the outer update exports live
manager, mode, and score-like values to two scene-owned presentation resources.
The tiny setters update only nested values, dirty bytes, and one enable byte;
setup writes sentinels and scene exit writes zeros to the same destination.
Their resolved resource/player paths are effect/render sinks and have no return
or owner alias into the clock, input, active notes, candidates, judgement,
results, or terminal outcome. Evidence:
`claim.pipeline.post-update-scene-resource-boundary`.

One later outer-update helper is a bounded cue/report scheduler. It runs after
the complete current input/note-manager, materialization, meter-grid, and
key-beam sequence, admits at most four triggers, starts selected audio/effect
controllers, and retains trigger/backend timestamps for a teardown report.
Those fields and controller calls do not enter the play clock, input history,
active notes, candidates, judgement, results, or terminal-outcome rules. Cue
time is not entirely inert to the enclosing scene: a separately registered
post-gameplay callback compares observed times from the two cue handles while
selecting its next scene state. The cue scheduler is therefore excluded from
judgement/result reconstruction while its post-gameplay presentation-lifetime
dependency remains explicit. Evidence:
`claim.pipeline.post-update-cue-report-boundary`.

The final outer-update operation is gameplay-affecting: it submits a synthetic
kind-2 snapshot to the ordinary aggregate evaluator even when no note produced
a result. Immediately before that evaluation, the current NotesManager
position is converted to a signed 64-bit tick by single-precision multiplication
with the separately initialized `16.666666F` factor and floor toward negative
infinity. This factor is not replaced by mathematical inversion of the forward
`0.06F` scale; their float products can fall just below an integer boundary.
For example, the reconstructed conversion maps `0.06F` to 0 and `60.0F` to
999. The converted tick is retained in both scene and result-owner state and
feeds externally configured kind-2 progress rules. Eligible common rule work
then precedes the usual end-threshold producer, so periodic reevaluation can
latch terminal state for a subsequent dispatch or immediate gameplay-state
exit. Exact conversion is `periodic_aggregate_position_tick`. Evidence:
`claim.judgement.periodic-aggregate-reevaluation`; tests:
`tests/shared_result_test.cpp`.

The registered active-gameplay callback performs that entire outer update and
then attempts its ordinary interval report before testing scene exit. This
ordering makes any terminal state latched by the final kind-2 reevaluation
visible to the ordinary controller's exit predicate in the same callback. An
accepted exit records a transition timestamp. A scene-owned selector then
chooses structural state code `0x10`, or state code `0x11` after forcing one
final report. The player-facing names of the selector and state codes remain
unassigned. Exact controller-specific predicates are normative in
`spec/judgement.md`. Evidence: `claim.pipeline.active-gameplay-exit-gate`;
tests: `tests/gameplay_exit_test.cpp`.

That state change is not an immediate gameplay stop. The update callbacks for
structural states `0x10`, `0x11`, and `0x12` each run the same full outer update
with argument one before applying their presentation/lifetime gates. Thus
input, active notes, result dispatch, and the final kind-2 reevaluation remain
live during this post-active drain. Entry to state `0x13` is the first boundary
that destroys both runtime-note vectors and clears the input/view owners;
states `0x13` and `0x14` do not call the outer gameplay update. The exact live
state set and state-`0x12` controller gate are reconstructed by
`post_active_outer_update_required` and
`post_active_drain_ready_for_teardown`. Evidence:
`claim.pipeline.post-active-gameplay-drain`; tests:
`tests/gameplay_exit_test.cpp`.

State-`0x13` teardown does not erase the outcome produced by the last drain
update. Its only current-result writes clear two dynamic configured-rule
notification masks whose sole gameplay consumer already ran at the end of that
aggregate evaluation. On state-`0x14` entry, the selected destination record is
reset and the complete current `0x878`-byte result object is deep-copied into
it. Core counts, aggregate values, terminal flags, and controller state are not
reset between the last outer update and this persistence step. Evidence:
`claim.pipeline.final-result-persistence-order`.

Chart reset empties BPM, MET, and all four generated grid vectors. `BPM_DEF`
resets to four 150.0 values and a present record replaces all four, but it does
not synthesize a missing BPM record. A full parse with BPM records later
replaces the binary's internal BPM_DEF fields with derived tempo statistics;
the authoritative schedule and gameplay loader continue to read the finalized
BPM vector. Valid Air-bearing gameplay charts
must therefore have a nonempty positive BPM map; the executable has no safe
adaptive-Air fallback for an empty map, and no positive-BPM validation was
recovered. Evidence: `claim.timing.tempo-measure-schedule` and
`claim.parser.header-default-dispatch`.

## Recovered TAP comparison boundary

The TAP checker subtracts its converted scheduled position from the current
manager position. That delta drives candidate eligibility, five-band lane
classification, and the overall late-completion check. Evidence:
`claim.note.tap-construction`, `claim.note.tap-candidate-judgement-gate`, and
`claim.judgement.tap-window-classification`.

## AirHold postprocessor grid and cadence

AirHold path generation measures parser chart positions on a 384-tick
major-unit grid. In source float operation order:

```text
grid_tick = trunc((major + minor * 0.25F) * 384.0F + 0.5F)
```

Its sampling cadence starts at 384 ticks. The parser scans the finalized BPM
vector backward by cumulative scheduled milliseconds, using the latest BPM at
or before the query and the first BPM before the first change. The named
`PROGJUDGE_BPM` header is the comparison reference and defaults to 240. Repeat
while `selected_BPM < PROGJUDGE_BPM * 4.0F`: double the selected value and
integer-halve the cadence. The first lookup uses the segment endpoint; later
lookups use each generated sample's scheduled value. Exact generation and
filtering rules are normative in `spec/notes/air_hold.md`. Evidence:
`claim.timing.tempo-measure-schedule` and
`claim.note.air-hold-secondary-judgement`.

AirSlide uses the same 384-tick grid and adaptive step rule, but keeps one
cursor across ASC controls. An ASD control restarts it one adaptive step after
that control; the step at a restart is selected from the root/ASD scheduled
value, and later steps use generated-sample schedules. Exact boundary and
emission rules are normative in `spec/notes/air_slide.md`. Evidence:
`claim.note.air-slide-secondary-judgement`.

Type-13 HeavenHold also uses the 384-tick grid and adaptive cadence. Its first
lookup is at the root schedule, then each generated sample selects the next
step. Sampling covers one root-to-final-end span; kind-0 samples are strictly
interior and an enabled kind-1 record is placed at the endpoint. Exceptional
type-9 ALD skips this producer and retains the parsed constructor's empty
queue. Exact filtering and consumption are normative in
`spec/notes/heaven_hold.md`. Evidence:
`claim.note.heaven-hold-judgement`.

## Substep order and bounded catch-up

### Pre-active zero-base priming

The complete caller set for the outer gameplay update is the consecutive
structural state interval `0x0a..0x12`. States `0x0a..0x0e` call it with a
false argument before structural active state `0x0f`; states `0x0f..0x12`
call it with a true argument. States `0x13` and `0x14` do not call it.

For a false-argument update, the raw/prior/smoothed/accumulated clock fields are
cleared before substep dispatch and the physical input structure is initialized
to all zeroes without populating either source bank. This is not a view-only
path. The zero input snapshot is appended, the active-note manager performs
candidate and note-state work, pending chart records can materialize, and the
final kind-2 aggregate reevaluation still runs. The ordinary path retains its
bounded prior/zero substep offsets relative to the zero base; the alternate
path runs once at the zero base.

Pre-active frames can therefore construct and activate runtime notes before
live time/input begins, and any note or external kind-2 rule eligible at those
positions follows its ordinary state machine. Reconstruction is
`outer_gameplay_update_mode_for_scene_state`. Evidence:
`claim.pipeline.pre-active-zero-base-priming` and
`claim.pipeline.outer-update-direct-callee-closure`; tests:
`tests/gameplay_exit_test.cpp`.

The ordinary gameplay loop pairs operations in this order for each substep:

1. derive and append the input snapshot;
2. update active notes and perform candidate/judgement work.

Inside the active-note manager, a direct helper between candidate preparation
and the primary note ticks clears ten current render-category counts plus a
current total. Render geometry builders repopulate them from completed vertex
triples; companion fields retain peak counts and scene initialization clears
both halves. Their closed reference set has no gameplay-state consumer, so this
interposed diagnostic clear does not add a candidate, input, judgement, or
cross-note dependency. Evidence:
`claim.pipeline.render-primitive-counter-boundary`.

The controlling value is AutoScan's 32-bit successful-update sequence. It is
zeroed at construction and incremented once, with a native dword `INC`, only
when a newly available scan succeeds. BoardCtrl and the input owner copy that
dword unchanged into gameplay. There is no overflow branch or saturation.

Its first integer offset is computed by wrapping in 32 bits before the signed
clamp:

```text
wrapped = uint32(previous_counter - current_counter + 1)
offset = clamp(bit_cast<int32>(wrapped), -1, 0)
```

The loop increments that offset through zero. It therefore performs one pair
for offset zero when current, one pair when one counter behind, and exactly two
pairs at offsets -1 and zero for every lag of two or more. The same offset is
multiplied by `0.5` and added to both the smoothed input-time base and the
accumulated note-time base before their respective calls. The current counter
is stored only after the final offset. Adjacent wrap from `0xffffffff` to zero
therefore produces the normal zero substep.

The stable alternate global dispatch bypasses this catch-up loop, runs its
separate input path once, and invokes the note manager once with no substep
offset. Although the ordinary loop contains a `0.25` step selector, the outer
dispatch reaches that loop only after observing the alternate flag clear; the
selector would matter only if that global changed during the call.

Exact reconstruction is `first_catch_up_offset`. Evidence:
`claim.timing.gameplay-substep-order` and
`claim.timing.autoscan-counter-wrap`; tests: `tests/play_clock_test.cpp`.

## Loading-scene note-view preload barrier

Before runtime-note materialization is reachable, the loading scene completes
a separate note-view resource barrier. A fresh barrier constructs these
RTTI-identified presentation classes in order:

```text
Tap, CharaTap, Hold, Slide, Air, AirHold,
AirSlide, AirLadder, AirSolid, Flick, Mine
```

The coordinator begins at stage 1, which constructs Tap. Each later stage calls
the readiness virtuals of accumulated objects with one shared integer resource
step. Already-ready objects are skipped; the most recently constructed object
therefore controls advancement. Its first ready steps are, in the order above:

```text
8, 10, 12, 20, 12, 15, 13, 11, 0, 9, 29
```

The manager also requires the old step value to be at least 10. Since each
stage begins at zero, its call count is
`max(10, first_ready_step) + 1`. A fresh uninterrupted sequence consequently
returns complete on its 164th periodic step call, including the initial Tap
construction call. Advancement finalizes every accumulated presentation object
before constructing the next; final stage 12 finalizes all eleven and enters
complete stage 13.

These objects occupy a manager vector separate from both live-note vectors.
Restart, full reset, and destruction delete them; their readiness/finalize
paths do not call parsed-record dispatch, the runtime-note factory, input, or
result submission. The barrier can delay the loading-scene exit, but adds no
gameplay substep after that exit. External resource names and the loading
scene's wall-clock update cadence are outside this specification. Evidence:
`claim.pipeline.note-view-preload-barrier`; focused test:
`tests/note_view_preload_test.cpp`.

## Runtime note materialization

Scene setup does not immediately construct accepted parsed records. It queues
their final vector indices in ascending order. Once per later outer gameplay
update, after the complete ordinary catch-up loop or the single alternate
manager call, the scene scans the pending queue. Ineligible records remain
pending; eligible or invalid records are erased. Every eligible record is sent
through the factory before erasure, even when its type has no primary factory
case. Several eligible records can be consumed in one scan.

The scan's apparent scene-byte bypass is dormant in this binary. Construction
initializes that byte to zero as the high byte of an adjacent word, gameplay-
state entry clears it again, and its complete reference set contains no
nonzero writer or address escape. The same byte therefore cannot silently skip
materialization or force the enclosing gameplay-state exit.

For each start or supported endpoint probe, define:

```text
raw_delta = scheduled_milliseconds * 0.06F - manager_position
```

If `raw_delta < 30.0F`, the record is eligible immediately. At equality and
above, an optional nonnegative chart-region key first transforms the scheduled
position, producing `adjusted_delta`. This ordering is intentional: the region
transform does not affect the strict `30.0F` shortcut.

For the far path, `projection_factor` is selected from the chart-owned schedule
only when `adjusted_delta > 0`; otherwise it is `1.0F`. The source-order float
calculation is:

```text
projected = -65.0F
          - 1.5F * (projection_base_offset + adjusted_delta)
                 * runtime_speed * projection_factor
eligible = -550.0F <= projected && projected <= 550.0F
```

If the start fails, primary parsed types 1, 2, 9, 10, 12, and 13 retry with
their endpoint and separate region key. Types 0, 1, 2, 4, 6, 9, 10, 11, and
13 have primary factory cases. Attachment types 3, 5, and 8 are created only
after a supported root; type 12 is consumed without a runtime note.

New primaries append in scan/index order, with an attached secondary appended
immediately after its root. Because the manager has already completed all
substeps for that outer call, new objects cannot expose candidates, consume
input, submit results, or request terminal state until the following outer
update. Runtime speed and projection base offset remain explicit inputs; their
player-facing configuration identities are not assigned.

Every factory-reachable primary and attached-secondary class then follows the
same three-state dispatch. Construction leaves `current = -1` and
`requested = 0`. At the start of each note tick, a pending request is committed
before the callback for the new current state is selected:

```text
first later manager substep:
    commit 0; state-0 callback requests 1
second later manager substep:
    commit 1; call the concrete note's gameplay update
later substep after a terminal request:
    commit 2; run no gameplay callback; remove in the same manager pass
```

All concrete state-0 predicates are constant true. A transition requested by a
callback is not recommitted during that tick. Primaries enter the lane-candidate
phase before their common tick, including on the first later substep;
secondaries are never candidate-called. Consequently, a two-substep ordinary
catch-up can reach the type-specific update during the next outer call, while a
one-substep or alternate call cannot do so until a still later call. The base
framework has a second state-action hook after the registered callback, but it
is dormant for runtime notes in this binary. `NotesBase` construction creates
an empty exact child factory and an empty fallback callable map; all
factory-reachable constructors and record-load callbacks leave both empty.
Transition selection therefore stores a null child and the common tick skips
the auxiliary virtual call.

The common tick's outer `+0x65` byte guard is dormant as well. `NotesBase`
construction clears it, and the closed constructor/load, manager, registry,
transition, and destruction paths contain no writer or setter. Every manager
tick on a live runtime note therefore reaches the transition/callback sequence;
there is no recovered per-note pause mode.

Evidence:
`claim.pipeline.runtime-note-materialization-order`; focused test:
`tests/runtime_materialization_test.cpp`. Shared state dispatch evidence:
`claim.pipeline.runtime-note-dispatch`; focused test:
`tests/runtime_note_dispatch_test.cpp`.
