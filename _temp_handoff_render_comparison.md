# Temp handoff: what a render-comparison session turned up

Temporary note, not normative. Delete once its contents are either promoted to
claims/product rules or dismissed.

Stage-two transition note: statements below that call rendering “out of scope”
describe the former reconstruction-only contract. Rendering is now the active
product scope. The observations and fits still do not rewrite verified gameplay
claims by themselves; use the evidence labels and canonicalization workflow in
`docs/EVIDENCE.md` and `docs/WORKFLOW.md`.

All priority, inference, and “cheapest next move” language below is historical
and is superseded by `docs/VIEWER_ROADMAP.md`. In particular, the later binary
trace contradicts the lead that ALD generates extra judged checkpoints. Retain
this file only for its raw written observations and unresolved field leads.

## What this session was

An offline chart viewer (`scripts/c2s-viewer.html`) was built from this
repository's own specs and claims, then compared frame-by-frame against
gameplay footage of the same `.c2s` files. Seven local charts across the 2000
and 8000 index ranges.

The method's one virtue is that the viewer is not an independent
reimplementation. Command vocabulary and field layouts come from `spec/c2s.md`,
timing from the `BPM`/`MET` schedule per `spec/timing.md`, the slide boundary
marker and chain continuation from `spec/notes/slide.md`, the `ASD`/`ASC` and
`AHX`/`AHD` checkpoint split from the air specs, the air-up start from
`air_hold_start_profile` / `air_slide_start_profile`, note speed from the
recovered materialization projection. So a disagreement with the cabinet is
either a viewer bug or a spec gap, and the viewer bugs were fixable and got
fixed. What survived is in GitHub issues 2 through 5.

Its limits are real. Footage is watched, not instrumented. Counts of fast
elements are estimates. Judgement outcomes are mostly invisible — you can see
what renders and hear what sounds, not what scores. Player vocabulary in the
issues is the observer's, not this project's.

## Leads, roughly by value

**1. `ALD` generated judged checkpoints are recovered (issue #3).**
The earlier spec was wrong. Type-9 ALD has a `0x24` authored-control vector and
a separate `0x20` generated vector. `FUN_011c6a50` samples from the root at the
positive fifth-data-field interval, interpolates lane, vertical value, and
decoded width, and the runtime builds one profile-7 checker per generated
sample. The replacement claim is
`claim.note.air-ladder-generated-checkpoints`; the original claim remains
preserved as superseded history.

Next move: finish the exact roles of the three type-9 presentation streams in
`FUN_00c03c00`; the fixed 32-division and whole-height viewer rules are invalid.

**2. Slide checkpoint classes are recovered and the counterexample is
retracted (issue #4, closed 2026-08-07).**
The ending boundary marker IS the discriminator
(`claim.note.slide-presentation-classes`): segment byte `+0x35` from the
command that ENDS the segment, root and final forced set, endpoint resource
allocated iff nonzero, guide feedback through a disjoint one-shot consumer on
the same predicate. The "demonstrably not it" counterexample (2891_03 bars
48/59/62) was an artifact of the then-viewer anchoring markers at record
STARTS; the binary anchors them at ENDS. Re-derived end-anchored, all three
footage observations confirm the rule: the bar 59/62 junctions are ended by
unmarked `SXC` segments, and the `SXD` marks land one junction later. The
"parsed connection field" is field 8, the decoded `SLD`/`HLD`/`GRN` style
code. Residual observation, tracked under `render.slide`: at 2891_03 bar 53
the shrink-point note appeared at the pre-shrink width; the claim records
endpoint selection only "by bounded width" without pinning which endpoint's
width.

**3. Profile 7 may test absence of upward exit (issue #2).**
Weakest of the three, and explicitly filed as a hypothesis. Rests on the
65/215 sentinel asymmetry, the reset seeding to 65, and two player reports.
Falsifiable as described in the issue.

**4. The former `ALD` connection-selector hypothesis was misnamed.**
The field is the exact fixed-grid sampling interval. Values such as 1, 2, 4,
6, 12, 16, 24, and 38400 directly control generated sample cadence; zero also
participates in the exact zero/`NON` HeavenHold class-selection branch. The
viewer must treat it as recovered model input, not a visual class guess.

## Friction worth fixing while you are in there

**The 91-entry descriptor registry is asserted but never enumerated.**
`spec/c2s.md` states the count and the ID range and stops. That is what made
`SLP` invisible until a chart forced the question, and any other unrecognised
spelling would be equally invisible today. Writing the list down is cheap and
closes a whole class of blind spot.

**Nobody documents who fills the authored checkpoint vector.**
`spec/notes/hold.md` and `spec/notes/slide.md` both say the note *copies* a
checkpoint vector from the parsed record. Neither says where it came from.
This may be the same question as lead 2.

**`claim.parser.sla-region-selection` is a good cautionary tale.**
It concluded that `SLA` could not affect gameplay. That reads as obviously
right — type 12 constructs no runtime object. It was wrong, and the proof-check
pass caught it: the tag selects a keyed scroll schedule, the schedule feeds
materialization, and materialization gates when a note can participate. Worth
keeping in mind for anything currently concluded to be inert on the grounds
that it builds nothing.

## Do not trust these parts of the viewer

They are fitted or invented, and several are flagged in issue #5:

- All playfield proportions. Rendering is out of scope, so none of the
  geometry could have come from the binary.
- The `ALD` rung generation grid, fitted to observed counts.
- The selector gate on rung generation, inferred from two player reports.
- Authored height 1.0 meaning the lane surface, and the per-chart air-height
  anchor. Inferred; `AIR` records carry no height field to calibrate against.
- The `MET` field order read as unit-then-count. Inferred from the corpus.
- Scroll transform magnitudes. The structure follows `spec/timing.md` exactly,
  but the source works in a derived millisecond domain and the viewer works in
  the tempo map's own milliseconds.

One part is better supported than it was: the note-speed mapping,
`5388.9 / speed` ms, derived from the materialization eligibility band. The
unproven step was whether that band is also the visible extent. A player
paused footage and the viewer at the same instant at speed 9.0 and reported
near-identical visible note counts. Still observation, not proof, but no longer
an assumption.
