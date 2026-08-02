# Slide notes

This section specifies parsed type 2, produced by `SLD`, `SXD`, `SLC`, and
`SXC`. It reconstructs gameplay state only; path mesh, effects, and feedback
resources are excluded except where their state crosses a result boundary.

## Parsing and path ownership

Each command supplies a `0x24`-byte control point. A command continues an
existing type-2 parsed record only when all of these match its last point:

- the parsed connection field;
- mirrored logical lane origin;
- bounded width;
- chart position under the parser's existing float-position equality helper.

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
results.

One exact style is an exception to this type-2 path. Field 8 is decoded by an
exact three-string table: `SLD` is code 0, `HLD` is code 1, and `GRN` is code
2; empty, differently cased, or otherwise unrecognized values also become
zero. After all commands have been joined into chains, but before generated
paths are built, every type-2 chain with style code 1 is changed to type 13.
The pass writes discriminator code 10 to root `+0x30` and to `+0x18` of every
control point. It preserves the command-specific first field: SLD/SLC keep
zero, while SXD/SXC keep one. These rewritten records bypass `SlideNote` and
follow the HeavenHold generation, factory, input, and judgement path specified
in `spec/notes/heaven_hold.md`.

Evidence: `claim.note.slide-path-sustain-judgement` and
`claim.note.slide-hld-heaven-retyping`.

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
