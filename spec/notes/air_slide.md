# AirSlide secondary note

## Parser attachment and control markers

`ASD` and `ASC` resolve to secondary parsed type 8 and share an eleven-field
schema. Their command IDs are `0x27` and `0x28`; the parser stores
`command_id == 0x27` on each 0x24-byte control point. This specification calls
that byte the ASD marker because the identity is executable data, not because
of an assumed player-facing meaning.

The sixth field names the record family to attach to or continue. For an
initial attachment, the parser searches for a compatible root at equal chart
position, lane, and width with an unused secondary slot, installs type 8, and
appends the current control point. References to ASD or ASC instead select the
continuation path: the existing type-8 control vector must be nonempty, its last
position/lane/width must match, and its last ASD marker must equal the
referenced family. A failed root or continuation match takes the parser
diagnostic path.

The runtime factory allocates a separate `projView::AirSlideNote`, links it
from the root, and appends root then secondary to the active-note vector. The
secondary owns its vectors, checkers, phases, gap state, and deferred lifetime.
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

A final ASC enables a nonnegative end-margin filter; final ASD bypasses it. The
margin is `PROGJUDGE_AER`, which resets from float bits `0x3f7fbe77`. For each
record it disables emission when:

```text
record_grid_tick + round(adaptive_step * end_margin) >= final_grid_tick
```

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
