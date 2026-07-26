# FLK note

## Construction and timing state

Parsed record type 6 constructs RTTI-identified `projView::FlickNote`. Shared
loading provides table-decoded width, raw and bounded lane extent, and scheduled
time. The note owns two timing checkers:

- the primary start checker uses the shared TAP initializer, including
  parser-derived adjacent-record endpoint trimming;
- the secondary motion checker is initialized for the bounded lanes from a
  separate external configuration group.

The secondary configuration also supplies a width-indexed observation span and
required lateral travel distance. All values remain parameters. Evidence:
`claim.note.flick-motion-judgement`.

## Candidate and initiating edge

Only phase 0 exposes per-lane scheduled candidates. It uses the shared checker
eligibility and therefore participates in the manager's smallest-nonnegative
lane reduction. A FLK-specific preparation step then widens a center endpoint
to its corresponding inner endpoint when inner and middle are strictly within
the binary's `0.00001F` epsilon. Candidate exposure precedes this mutation during the pass;
input classification follows it.

Ordinary start evaluation waits until the timing delta is at or above the
secondary overall lower bound. Before the primary overall upper bound, it scans
the covered lanes in ascending order and accepts the first lane with both a
derived rising edge and a nonzero primary fine-result acceptance code. Unlike
TAP and HOLD start, this path does not compare against the manager-selected lane
candidate. This is asymmetric: an earlier FLK candidate can block a later
TAP/CHR/HOLD/Slide start, but an earlier candidate from those gated families
does not block FLK. Equal candidates can admit both. Exact cases and the
ordinary/forced result-order boundary are normative in `spec/matching.md`.
At or above the primary upper bound FLK enters phase 2.

An accepted edge stores its primary fine index and enters phase 1. Motion is
processed immediately in that same note update.

## Lateral held-source motion

Let `start` and `count` be the bounded lane extent. Let `span` and `distance`
be external configuration values. Compute:

`padding = (span - count + 1) / 2`

using signed division truncated toward zero. Scan the inclusive interval
`clamp(start - padding, 0, 15)` through
`clamp(start + count - 1 + padding, 0, 15)`.

For that interval, compute nonempty count and average lane center
`sum(lane) / count + 0.5` for three groups in order:

1. union of both physical source banks;
2. bank 0;
3. bank 1.

Each group latches its first nonempty center as both retained minimum and
maximum. Later lower/higher centers update the corresponding extremum. A new
lower value whose distance from maximum is at least `distance` selects phase 4;
a new higher value whose distance from minimum is at least `distance` selects
phase 5. First completion in the group order above wins.

Boundary continuation is exact. When `start == 0` and logical lane 0 is held, a
nonempty group records status 1. When `start + count == 16` and logical lane 15
is held, it records status 2. If that group is empty on the next sample, status
1 supplies center `-0.5` and status 2 supplies `16.5`, then clears. With no
status, an empty group retains its prior center.

Phase 1 can track only while the delta is strictly below the secondary overall
upper bound. At or above it, phase 3 wins over a same-sample motion outcome.
The reconstruction is `FlickMotionTracker`, `update_flick_motion`, and
`advance_flick_phase`; tests are in `tests/flick_motion_test.cpp`.

## Results and lifetime

Phases 0/1 are nonterminal. Phase 2 is edge timeout, phase 3 is motion timeout,
and phases 4/5 are the two anonymous directions. Their metadata integers are
16, 16, 14, and 15 respectively. Timeout result bytes are 0 and 1 with side
code 2.

For phases 4/5, the start primary fine record supplies a provisional result tier
and side code. Add its coarse tier to the secondary completion fine record's
coarse tier. Sums below 3 become 3 and force side code 2. The fixed control-code
map for sums 0 through 10 is `0,1,1,2,2,4,4,4,4,4,4`; control code 4 forces
side code 0. The provisional downstream result remains the start coarse tier
and can be modified by active result-control state before shared result routing.
Exact composition is `compose_flick_direction_result`.

Manager forced-result state bypasses edge and motion after an external timing
point. It submits the forced byte with side code 0 and alternates phase 4 then 5
through a shared flag. Every FlickNote construction resets that flag so the next
forced direction is phase 4. Exact alternation is
`select_forced_flick_direction`.

Terminal phases use common manager cleanup after the full active-note pass.
Results then follow the shared category/aggregate/terminal-routing rules in
`spec/judgement.md`. Evidence: `claim.note.flick-motion-judgement`.
