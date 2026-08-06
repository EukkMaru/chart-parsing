# Note matching

## Per-lane candidate reduction

At the recovered update boundary, the manager resets 16 lane candidates to a
negative sentinel. Active notes prepare their own candidates, after which the
manager retains the smallest nonnegative scheduled value seen for each lane. A
TAP input lane is eligible only when its own candidate equals the selected
manager value. Evidence: `claim.note.tap-candidate-judgement-gate`.

This establishes earliest-scheduled-value priority among unequal eligible
candidates.

Candidate exposure reads the per-lane outermost interval. Only after all
candidate preparation and reduction does TAP widen qualifying center endpoints
toward inner endpoints, before the later input-classification pass. The
mutation can change the fine result of an admitted edge but cannot retroactively
change lane candidacy or the selected minimum for that substep. Evidence:
`claim.judgement.tap-center-window-adjustment`.

Before runtime candidate reduction, chart-load postprocessing also compares
selected parsed records pairwise. Chronologically separated pairs with
overlapping lane extents can write preceding/following distances onto the
overlap lanes. Those distances trim the later checker windows at construction
time and can therefore change whether a note exposes or accepts a candidate;
they do not replace the runtime minimum reduction. Evidence:
`claim.judgement.tap-adjacent-window-adjustment`.

HOLD exposes candidates only until its start component reaches phase 4. Before
then it uses the same lane-window candidate builder and selected-candidate gate
as TAP. Its later sustain/checkpoint component does not expose start candidates.
Evidence: `claim.note.hold-construction-start-gate`.

FLK exposes the shared scheduled candidate only while awaiting its initiating
edge. Its own edge-update path checks the rising edge and primary timing
acceptance but does not read or compare the manager-selected candidate. FLK can
therefore constrain a TAP/CHR/HOLD/Slide candidate through reduction without
imposing the same equality gate on itself.

Let `F` be FLK's exposed value and `G` a gated-family value on the same lane,
assuming the shared rising edge and both local timing checks otherwise accept:

- `F < G`: FLK can start; the gated note cannot because `G` is not selected.
- `G < F`: both can start; the gated note matches `G`, while FLK ignores the
  selected value.
- `F == G`: both can start; reduction does not choose a single winner.

All candidate preparation/reduction completes before the full-vector update,
and neither candidate nor rising input is consumed by a note. Storage order
therefore does not change these start cases. In ordinary play, the edge-start
substep only initializes FLK motion tracking and emits no FLK result. Later FLK
direction/timeout results occur after it stops contributing candidates, so
ordinary candidate asymmetry and FLK terminal routing are disjoint substeps.
Forced mode can complete phase 0 after candidate preparation, but the other
start families bypass candidate equality in that mode as well; simultaneous
forced results follow vector order and shared terminal routing, not candidate
priority. Evidence: `claim.matching.flick-candidate-asymmetry` and
`claim.note.flick-motion-judgement`; reconstruction/tests:
`LaneCandidateGate`, `tests/candidate_interaction_test.cpp`.

CHR/CharaTap uses the full TAP candidate and selected-candidate equality path.
It therefore participates in TAP ties and deferred terminal candidate exposure
under the same local rules, with only its downstream result category differing.
Evidence: `claim.note.chara-tap-result-category`.

MNE exposes no candidate in any local phase: its candidate virtual leaves all
16 outputs at the negative sentinel, and its held-contact update never reads
the manager-selected candidates. It therefore neither constrains nor depends
on TAP/HOLD/FLK lane-candidate priority. Evidence:
`claim.note.mine-contact-aggregate-judgement`.

AIR is an independently updated secondary runtime object but exposes no lane
candidate in either local phase. Its retained profile-input checker does not
read the manager-selected candidates. An attached AIR therefore neither
changes nor depends on the TAP/HOLD/FLK lane reduction. Evidence:
`claim.note.air-secondary-judgement`.

AirHoldNote is likewise a separately updated secondary object whose candidate
virtual leaves all 16 lanes at the negative sentinel. Its start, generated
path, and authored AHX result streams use derived profiles and scheduled
secondary records without reading the manager-selected candidate. It neither
constrains nor depends on lane-candidate reduction. Evidence:
`claim.note.air-hold-secondary-judgement`.

AirSlideNote is another independently updated secondary with the all-negative-
sentinel candidate virtual. Its start, generated path, and ASD-authored streams
read derived profiles and scheduled records without reading manager-selected
lane candidates. Evidence: `claim.note.air-slide-secondary-judgement`.

Ordinary AirLadder uses the all-negative-sentinel candidate virtual. Its
profile-7 generated checkpoints are all updated directly and do not
participate in lane minimum reduction. Evidence:
`claim.note.air-ladder-generated-checkpoints`.

SLD/SXD/SLC/SXC construct one type-2 root for a compatible control-point chain.
At runtime, `SlideNote` exposes the shared TAP-style candidates only until its
start component reaches phase 4. Its later path contact and checkpoints do not
expose candidates. Evidence: `claim.note.slide-path-sustain-judgement`.

## Equal cross-family candidates and result order

Equal candidates do not select a single winner. Candidate preparation and
minimum reduction finish before any active-note input update. During the later
full-vector pass, the selected candidate and rising edge are read without
clearing either, and completed objects are removed only after that pass.
Consequently every locally accepting TAP, CHR, unresolved HOLD, unresolved
Slide, and unresolved HeavenHold start that shares the selected value can
independently accept the same lane edge. FLK can also accept because its edge
path ignores selected-candidate equality; ordinary FLK only starts motion on
that substep and emits no result yet.

The update pass preserves active-vector storage order. Existing objects precede
newly materialized ones; one pending scan appends supported roots in pending
index order and each attached secondary immediately after its root. Objects
constructed on earlier outer updates remain earlier even if their parsed index
is later. A factory-default record contributes no object.

Same-pass results therefore dispatch in that dynamic storage order. A result
can activate shared terminal routing; later tied notes still observe input,
complete locally, and invoke the observer, but their later result events do not
update the authoritative aggregate while that route is active. Evidence:
`claim.matching.tap-equal-candidate-fanout`,
`claim.interactions.cross-family-candidate-result-order`, and
`claim.interactions.result-terminal-short-circuit`; reconstruction/tests:
`evaluate_lane_candidate_fanout`, `append_runtime_factory_events`,
`tests/candidate_interaction_test.cpp`, and
`tests/shared_result_test.cpp`.

## Deferred terminal candidate

A TAP result requests terminal state 2 while leaving current state 1 unchanged
for the rest of that manager pass. On the following normal substep, the manager
prepares candidates before ticking note state machines. TAP candidate
preparation has no current-state, requested-state, or stored-result guard.
Consequently, a completed TAP that is still inside its configured candidate
windows contributes its scheduled value to one more lane reduction. Its pending
transition is then committed during the note-update pass and removal follows
after that pass.

This can delay a later unequal lane candidate for one substep. The completed
TAP cannot judge a second time in that substep because state 2 is committed
before the state-1 judgement callback. Independently, TAP's state callback
suppresses judgement when the embedded stored detailed result is greater than
1; that result is not consulted during the preceding candidate phase. Evidence:
`claim.matching.tap-deferred-terminal-candidate`; reconstruction and focused
phase-order tests: `tests/tap_lifecycle_test.cpp`.
