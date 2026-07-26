# Logical input

## Gameplay-facing device record

Every input substep starts from one zero-initialized 40-byte record:

- dword `+0x00`: the substep scalar time copied through the snapshot;
- bytes `+0x04..+0x13`: TouchSlider Y selector 0 at gameplay X positions
  0 through 15;
- bytes `+0x14..+0x23`: TouchSlider Y selector 1 at the same positions;
- low six bits of dword `+0x24`: `photo_sensor_6` through
  `photo_sensor_1`, respectively.

The TouchSlider interface internally indexes a source as
`(15 - x) * 2 + y`; the gameplay record remains in increasing logical X
order. Source bytes use only the zero/nonzero distinction.

The ordinary catch-up path reads the thresholded 32-byte TouchSlider sample at
history offset `-1` or `0`, matching its clamped substep offset. The alternate
path reads the current thresholded sample without a history selector. Both
builders query the same six photo-sensor resources and invoke the same
snapshot synthesizer. Backend device bindings and per-source threshold values
are external parameters. Evidence: `claim.input.touch-photo-input-framing`.

## TAP-consumed rising edges

The normal TAP gate accepts logical lane indices 0 through 15 and reads the
newest processed snapshot. Each snapshot begins with 32 current source bytes in
two banks of 16. A nonzero byte asserts its source. For each source independently:

`source_rising = source_current && !source_previous`

Corresponding source-bank bits are then OR-folded into logical lanes:

`logical_rising[lane] = source_rising[lane] || source_rising[16 + lane]`

TAP tests this logical rising mask. A logical held-level mask is also derived,
but it occupies a different snapshot field and is not read by the ordinary TAP
gate. Because edge detection precedes bank folding, a new source in one bank
can produce a logical edge even while the corresponding source in the other
bank is held.

The exact reconstruction is `chart::reconstruction::derive_tap_lane_input`.
Evidence: `claim.input.tap-rising-edge-snapshot`; focused tests:
`tests/tap_input_test.cpp`.

The edge alone is insufficient for TAP judgement. Candidate priority and the
lane-specific timing checker must also accept the note.

## Substep ordering and history

The gameplay update derives and appends a snapshot immediately before updating
active notes for the same scalar-time offset. Selector 0 returns the newest
nonempty history entry. Catch-up can repeat input derivation and note updating
as paired substeps.

Reset seeds one zero snapshot with scalar photo position 65. Before each later
append, a count greater than 300 loses its oldest element. Normal operation
therefore stabilizes at 301 retained snapshots, newest-inclusive.

The catch-up counter is the modulo-`2^32` AutoScan successful-update sequence,
copied through BoardCtrl into the input owner. Ordinary gameplay wraps
`previous - current + 1` in 32 bits before its signed `[-1, 0]` clamp, so
adjacent overflow does not create a discontinuity and any larger lag still
admits at most one prior snapshot. Evidence:
`claim.timing.gameplay-substep-order` and
`claim.timing.autoscan-counter-wrap`. Snapshot capacity and all remaining
history fields are covered by `claim.input.snapshot-profile-synthesis`.

## HOLD sustain marker

Snapshots also contain a 32-source marker distinct from raw held levels and
logical TAP rising edges. HOLD reads this marker from a preceding selected
snapshot and writes it into the newest snapshot for each physical source that
qualifies as active sustain. The marker participates in HOLD source arming and
can preserve already-held input across substeps or overlapping HOLD consumers.

The exact two-bank qualification rule is normative in `spec/notes/hold.md`.
Slide and HeavenHold are the only other gameplay consumers of this marker;
FLK reads physical levels without reading or writing it. Evidence:
`claim.input.hold-source-continuation` and
`claim.input.snapshot-profile-synthesis`; focused tests:
`tests/hold_sustain_test.cpp`.

Type-2 slide paths reuse these same two-bank arming and marker rules on every
logical lane whose generated path window is in phase 3, 4, or 5. Only phase 5
can make a source active and write a marker; leaving phases 3..5 clears the
lane's armed and active bytes. Exact path-window rules are normative in
`spec/notes/slide.md`. Evidence:
`claim.note.slide-path-sustain-judgement`; tests:
`tests/slide_path_test.cpp`.

## FLK held-source motion

FLK uses the same newest snapshot in two distinct ways: its initiating input is
the folded logical rising edge, while its directional phase reads held levels
from each physical bank separately. It tracks the union, bank 0, and bank 1
centroids without consuming or clearing the snapshot. Exact range, edge
continuation, and extrema rules are normative in `spec/notes/flick.md`.
Evidence: `claim.note.flick-motion-judgement`; tests:
`tests/flick_motion_test.cpp`.

## MNE logical held contact

MNE reads the newest folded logical held-level state for each bounded covered
lane. It does not read rising edges or distinguish physical source banks. If
one or more lanes are held, the note credits the entire clipped
previous-to-current timing interval; additional held lanes do not increase the
credit. Exact timing and resolution rules are normative in
`spec/notes/mine.md`. Evidence: `claim.note.mine-contact-aggregate-judgement`; tests:
`tests/mine_contact_test.cpp`.

## AIR derived profiles and sampling marker

AIR selects one of derived input-profile bytes 0 through 5 from the newest
snapshot. Profile selection depends on the attached root family and AIR
direction group; the exact matrix is normative in `spec/notes/air.md`.

The six photo-sensor bits first produce a conditioned mask. For sensor `i`,
let `count[i]` be the number of snapshots with conditioned bit `i` among the
newest `min(conditioning_history_count, available_history)` entries. Starting
from the previous conditioned mask:

- `count[i] < conditioning_required_count` clears the bit;
- a sufficient count sets the bit when the previous AIR marker is clear;
- a sufficient count plus a set previous AIR marker preserves the bit's
  previous value.

The residual six-bit mask is:

`photo_sensor_mask & ~conditioned_photo_mask`

When the residual mask is nonempty, it produces a scalar position. The two
outer edge patterns extrapolate to 75/95 and 185/205; all other patterns use:

`90 + floor(20 * mean(asserted sensor indices 0..5))`

With no residual bit, the position becomes 65 when the previous position is
below 140 and 215 otherwise. The stored delta is current minus previous unless
it lies outside inclusive `[-50, 50]`, in which case it becomes zero.

Profiles 0 through 6 independently accumulate absolute motion, positive
motion, and negative-motion magnitude. The current delta is always included;
newest older deltas are added until the total reaches that profile's external
history count or history is exhausted. A profile is true when any of its
three external thresholds is met. Profile 7 instead tests only whether the
current position lies in its external inclusive range. Its stored eighth
motion-threshold record is not consumed by synthesis.

Immediately before reading the selected byte, AIR sets a separate marker at
snapshot `+0x54`. The eight profile bytes occupy `+0x4c..+0x53`, so this write
does not force or overwrite the current input. Snapshot initialization clears
the marker, and the next input-build pass applies the conditioned-mask rule
above. Reconstruction: `InputProfileSynthesisConfig`,
`InputSynthesisSnapshot`, `synthesize_input_snapshot`,
`AirProfileSnapshot`, and `sample_air_profile`; evidence:
`claim.input.snapshot-profile-synthesis` and
`claim.note.air-secondary-judgement`; tests:
`tests/input_profile_test.cpp` and `tests/air_judgement_test.cpp`.

## AirHold derived-profile contact

AirHold's start checker uses AIR profile 0, 2, or 4 according to its attached
root. Its sustain component separately reads derived profiles 7 and 6. It
latches admission when profile 7 is absent or profile 6 is present, then treats
profile 7 as active only after the scheduled start and while admission is
latched. Profile synthesis is normative above; its external thresholds and
range remain parameters. Exact boolean ordering is normative in
`spec/notes/air_hold.md`; reconstruction/tests:
`update_air_hold_contact`, `tests/air_hold_judgement_test.cpp`. Evidence:
`claim.note.air-hold-secondary-judgement`.

AirSlide uses the same ordered profile-7/profile-6 admission latch for its
sustain component and the same root-family start-profile groups. Its distinct
generated-record and ASD-checkpoint consumers are normative in
`spec/notes/air_slide.md`; reconstruction/tests:
`update_air_slide_contact`, `tests/air_slide_judgement_test.cpp`. Evidence:
`claim.note.air-slide-secondary-judgement`.

Ordinary AirLadder initializes one retained-profile checker at fixed derived
profile 7 for every parsed control point. It does not read lane candidates.
Exact update cardinality is normative in `spec/notes/air_ladder.md`; evidence:
`claim.note.air-ladder-judgement`.

HeavenHold uses paired physical-bank state across every lane in its span.
Asserted banks are admission-checked in order until one succeeds. A success
latches asserted banks; no success latches deasserted banks. Current activity
then requires scheduled start, retained latch, and asserted raw input. Forced
activity affects only the aggregate inactive-gap input. Its TAP-style start
selects profile pair 0/1 for HHD and exceptional ALD, or 2/3 for HHX. Physical
input banks and profile synthesis are normative above. Exact ordering is normative in
`spec/notes/heaven_hold.md`; reconstruction/tests:
`update_heaven_hold_lane_contact`, `tests/heaven_hold_judgement_test.cpp`.
Evidence: `claim.note.heaven-hold-judgement`.
