# Claim: ASD/ASC attach a candidate-free AirSlideNote with generated and authored path results

- ID: `claim.note.air-slide-secondary-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`,
  `input.logical_state`, `input.buffering`, `matching.candidates`,
  `judgement.types`, `judgement.windows`, `judgement.miss`,
  `note.other_variants`, `state.ownership`, `config.external`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-26

## Statement

`ASD` and `ASC` attach parsed secondary type 8 and construct a separate
RTTI-identified `projView::AirSlideNote`. ASD-marked control points restart
generated-path sampling and own authored retained-profile checkpoints; ASC
points preserve the sampling cursor and own no authored checker. The runtime
exposes no lane candidate and terminates only after its independent start,
generated-record, and authored-checkpoint work is complete.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, ASD/ASC command IDs 0x27/0x28 and identical field schema`
- `game.exe @ RAM:01be5dac and RAM:01be5dbc, event-family records, secondary type 8`
- `game.exe @ RAM:011c8870, FUN_011c8870, type-8 attachment and continuation matching`
- `game.exe @ RAM:011bda60, FUN_011bda60, type-8 sampling and emission filters`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, secondary type-8 allocation and append order`
- `game.exe @ RAM:00c24d40, FUN_00c24d40, 0x258-byte construction and reset state`
- `game.exe @ RAM:018d9a14, AirSlideNote gameplay vtable`
- `game.exe @ RAM:01c3269c, RTTI type descriptor, projView::AirSlideNote identity`
- `game.exe @ RAM:00c28330, FUN_00c28330, root-relative load and ASD-checker construction`
- `game.exe @ RAM:00c26060, FUN_00c26060, start-checker resolution and category 9 result`
- `game.exe @ RAM:00c261d0, FUN_00c261d0, sustain, generated records, and ASD checkpoints`
- `game.exe @ RAM:00c297d0, FUN_00c297d0, gameplay/presentation update wrapper`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, all-sentinel candidate output`
- `game.exe @ RAM:00c27d50, FUN_00c27d50, two-component terminal predicate`
- `game.exe @ RAM:00c25750 and RAM:00c25d10, destruction and deleting destructor`

## Observations

- The command descriptors assign IDs `0x27` and `0x28` to ASD and ASC. Both
  require eleven fields after the command and both map to parsed secondary type
  8. The local control-point byte is exactly `command_id == 0x27`, so it is an
  ASD marker, not an inferred style bit. The local corpus contains 42,017 ASD
  and 163,804 ASC records, all with the expected total field count.
- The sixth field is resolved twice: once to the event-table index for branch
  selection and once to the parsed type for root matching. References to table
  index 19 (ASD) or 20 (ASC) take the continuation path. That path requires an
  existing type-8 secondary with a nonempty control vector, equality of the
  last control's position/lane/width fields, and a last marker matching the
  referenced family. Other resolved families take the root-attachment path,
  which also requires equal position/lane/width and an unset secondary slot.
  No match takes the parser diagnostic path.
- Factory case 8 allocates `0x258` bytes, constructs AirSlideNote, loads it via
  virtual slot `+0x24`, links it from the root, and appends root then secondary.
  Constructor/reset owns both phase fields, generated/control vectors, gap
  state, checker resources, and presentation resources; destruction releases
  those owned objects. The candidate virtual is the shared all-sentinel
  implementation.
- Load selects the same root-relative start anchors and profile groups as
  AirHold. The type-8 parser does not set the profile-group discriminator, so
  roots 1/2/13 select profile 0, roots 0/4/11 select profile 2, and root 6
  selects profile 4. The accepted start result submits source category 9 and
  sets the start phase to 5.
- Each type-8 control point has a runtime record, but only ASD-marked points are
  added to the authored-checker index vector. These use the retained-profile
  checker. A resolved ASD submits category 13 only when it is also the final
  control point; otherwise it submits category 12. Only the current indexed
  checker is advanced, so at most one authored result resolves per substep.
- Postprocessing emits gameplay records into the parsed vector copied by
  AirSlideNote. Grid ticks use
  `trunc((major + minor * 0.25F) * 384.0F + 0.5F)`. The cursor starts one
  tempo-adaptive step after the root. An ASD point before another segment emits
  a disabled type-6 boundary, restarts the cursor one step after itself, and
  disables the first following type-5 sample. ASC emits no boundary and carries
  the already calculated cursor into the next segment. The first sample after
  the root is also disabled. Later samples are enabled, samples must be
  strictly before the current endpoint, and one disabled type-7 final record
  is appended.
- The adaptive step begins at 384 ticks. While the tempo-map value is below
  four times the parser reference, the value is doubled and the integer step
  is halved. Restart steps use the root or ASD control's scheduled value; each
  emitted sample selects the step used to advance to the next cursor.
- A final ASC, not a final ASD, enables the nonnegative end-margin filter. It
  clears emission when
  `record_tick + round(adaptive_step * end_margin) >= final_tick`. The enabled
  key-0 open-interval lookup can clear more emission bytes. Neither filter can
  re-enable a record.
- Sustain contact reads derived profile 7 followed by profile 6 and uses the
  same admission latch and inactive-gap tracker as AirHold. Generated records
  are front-only and at most one is consumed per substep. In ordinary mode the
  gap is classified and reset for every due record before the emission byte is
  tested; a disabled record therefore resets retained gap state without
  submitting a result. An enabled record submits category 11. Forced selection
  bypasses the ordinary reset.
- The path phase reaches 5 only after the generated vector is empty and every
  indexed ASD checker has resolved. The terminal predicate also requires start
  phase 5. Category 9 maps to shared category 3, category 11 to 5, and 12/13 to
  7; normal valid events are aggregate-authoritative. The sibling presentation
  update has no result submission or write to the two gameplay phase fields.

## Reasoning

The command table directly fixes the ASD marker identity. Parser match
predicates, postprocessor marker branches, load-time checker selection, and
runtime result categories independently use the same byte, closing its path
from text command to outcome. RTTI, allocation, vtable, load, update wrapper,
terminal predicate, and destructor connect that parsed state to one owned
secondary runtime object. Instruction-level order distinguishes AirSlide's
disabled-record reset from AirHold's emission-gated reset.

## Alternatives and falsifiers

- Competing explanation: ASC points also create authored checkpoints, every
  control point restarts cadence, disabled records are inert, or AirSlide
  competes in lane-candidate selection.
- Evidence that would disprove this claim: an ASC control inserted into the
  checker index, a type-6 boundary after an ASC marker, preservation of the
  ordinary retained maximum after a due disabled record, a non-sentinel
  candidate, or terminal transition with either phase incomplete.

## Unknowns

- `claim.timing.tempo-measure-schedule` closes the shared position conversion,
  BPM schedule, and `+0xc4` `PROGJUDGE_BPM` cadence-reference ownership.
  `claim.parser.header-default-dispatch` closes `+0xc8` as
  `PROGJUDGE_AER` with reset bits `0x3f7fbe77` and `+0xcc` as the positive
  predicate of `TUTORIAL`; exclusion interval contents remain explicit.
- External start windows, AirSlide gap gate/floor/end, thresholds
  `+0x888..+0x894`, corrections, defaults, and units remain parameters.
- `claim.input.snapshot-profile-synthesis` closes profiles 6 and 7 and their
  history. External synthesis thresholds/range and player-facing names for
  result bytes and the three result streams remain open.
- Geometry/resource fields in the 0xac-byte runtime control record are not part
  of this gameplay claim unless a later traced consumer reaches judgement.

## Consequences

- Ghidra mutations: none; GhidraMCP remained unavailable, so analysis used the
  temporary project clone only.
- Spec sections: `spec/notes/air_slide.md`, `spec/c2s.md`, `spec/input.md`,
  `spec/matching.md`, `spec/judgement.md`, `spec/timing.md`,
  `spec/configuration.md`.
- Reconstruction code: `AirSlide*` parser-marker, generation, filtering,
  contact, lifecycle, checkpoint, and category helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/air_slide_judgement_test.cpp`.

## Verification

Command IDs/schema, raw event-table indices, parser root/continuation/error
paths, aggregate corpus field counts, factory append order, RTTI, allocation,
constructor/destructor, complete gameplay vtable, root anchor/profile selection,
ASD-only checker indexing, generated cursor/restart rules, both filters,
contact reads, gap classify/reset order, result categories, completion
conjunction, candidate absence, and shared routing were checked independently.
Focused tests cover marker/reference identity, continuous and restarted cadence,
record flags, both filters, ASD counting/categories, profile/contact selection,
disabled/forced reset behavior, terminal conjunction, and category mapping.
