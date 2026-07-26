# Gameplay configuration

## Runtime materialization projection interface

The pending-record materialization gate uses executable constants `30.0F`,
`-65.0F`, `1.5F`, and inclusive `[-550.0F, 550.0F]` together with two runtime
inputs: a speed multiplier and a projection base offset. Speed resets to
`1.0F` and has a direct setter; the base offset is copied from caller-owned
setup state. A chart-owned keyed-region transform can replace the far-path
scheduled position, and a separate chart-owned schedule supplies a factor only
for positive adjusted deltas.

The player-facing identities and complete producers of the speed and base
offset remain open, so clean-room code accepts them, the adjusted delta, and
the positive-delta factor as parameters. Exact predicate order is normative in
`spec/timing.md`. Evidence:
`claim.pipeline.runtime-note-materialization-order`.

## TAP timing-window interface

The TAP checker consumes configuration-selected values rather than fixed
thresholds recovered from executable immediates. Each of 16 logical lanes has:

- an enabled flag;
- five lower/upper endpoint pairs, outermost through center; and
- a pivot used to distinguish the two non-center sides.

The loader path names `JudgeTiming.ini`, and the runtime initializer selects
window data based on note and input/player modes before applying additional
global and per-record timing adjustments. The source configuration is absent
from this workspace, so endpoint values, defaults, units, and mode variants
must remain parameters. Evidence: `claim.judgement.tap-window-classification`.

The same initialization consumes chart-postprocessed per-lane flags and
distances for nearby preceding/following records. Runtime groups select which
parsed record types participate and provide pair thresholds, propagation
behavior, and caps for the early and late limits. Those values are also absent
and remain parameters. Their exact endpoint effect is normative in
`spec/judgement.md`. Evidence:
`claim.judgement.tap-adjacent-window-adjustment`.

The later per-substep center-window mutation does not load another external
threshold. It uses an executable-initialized `0.00001F` epsilon and compares
the already selected inner and middle endpoints independently. Its exact
ordering and effect are normative in `spec/judgement.md`. Evidence:
`claim.judgement.tap-center-window-adjustment`.

## Active result-control threshold

The result pipeline can hold a `SkillChangeJudgeResultData` control unit with a
coarse-tier threshold. It does not change timing intervals; after classification
it replaces any valid provisional tier less than or equal to the threshold with
tier 0. Construction initializes the dedicated unit vector empty, skill setup
can populate it, and result-controller reset destroys the units and empties the
vector.

The validity bound is the count of a runtime-loaded
`NotesJudgeResultTableRecord` table. Its values and the skill threshold remain
unavailable, so the reconstruction accepts activation, threshold, and count as
explicit inputs. Detailed activation duration and multiple-unit precedence are
open. Evidence: `claim.judgement.active-tier-zeroing`.

## HOLD inactive-gap interface

The HOLD gap tracker loads a participation-floor value from runtime
configuration field `+0x708` and exactly four ordered threshold values from
fields `+0x710`, `+0x714`, `+0x718`, and `+0x71c`. A separate field at `+0x70c`
adjusts the once-only end-bound check. These offsets identify the consumer
interface only; the owning configuration schema, values, units, defaults, and
mode selection are unavailable.

The clean-room reconstruction therefore accepts the floor, tracker end, and
four thresholds as parameters. It does not assume conventional timing values
or repair threshold ordering. Evidence:
`claim.note.hold-sustain-gap-lifecycle`; focused tests:
`tests/hold_sustain_test.cpp`.

Type-2 slide initialization uses this same gap interface. Its path preprocessor
also reads a width-indexed external block beginning at `+0x730` to generate the
four endpoints used by path-record and per-lane window classification. The
values and schema are absent; clean-room code accepts generated endpoints as
parameters and does not invent defaults. Evidence:
`claim.note.slide-path-sustain-judgement`; tests:
`tests/slide_path_test.cpp`.

## Forced-result mode interface

The gameplay manager owns an enable byte, a mode integer, a companion selector,
and a shared cycle counter. Reset clears all four. When enable is set and mode
is nonzero, shared start judgement and HOLD sustain/result paths use the forced
rules in `spec/judgement.md`.

The concrete nonzero in-binary producer belongs to RTTI-identified
`projView::PlayMusicTutorialObject::Impl`. At a scheduled tutorial-step
transition it writes mode 2 when an indexed step-record flag equals one, and
mode 0 otherwise. This produces anonymous byte 3 when the separately initialized
manager enable is active. Gameplay setup, teardown, controller destruction,
and manager reset clear the mode.

The enable byte and its companion are copied from caller-supplied setup state
only after a runtime validator accepts it. Their upstream semantic names remain
unresolved. No nonzero in-binary producer was found for modes 1 and 3 through 6
or companion selector `+0x360`; those cases remain supported interfaces rather
than assumed reachable modes. Mode 6's RNG is external to the pure
reconstruction, so its generated integer remains an input. Evidence:
`claim.judgement.forced-result-mode`; tests: `tests/forced_result_test.cpp`.

## Alternate terminal-meter setup interface

The alternate result meter loads its ordinary maximum, automatic adjustment,
four result deltas, and secondary-meter parameters from an external runtime
configuration record. Their values, units, defaults, and player-facing mode
names are absent and remain parameters.

Its auxiliary predicate value is not loaded from that record. Gameplay setup
derives a participant count from a bounded runtime record prefix and captures
it alongside the participant limit. Setup modes 1, 2, and 4 select only the
local-identity record; other modes can additionally select flagged non-local
records with nonzero effective current. The capture can be locked until the
meter is reconstructed or reconfigured. Exact participant-record field names
remain unresolved. Evidence: `claim.judgement.alternate-terminal-meter`;
tests: `tests/alternate_meter_test.cpp`.

## Ordinary terminal end-threshold interface

The ordinary terminal evaluator selects the live
`projDB::PlayOptionTrackSkipID` through fixed play-option table entry 32. Value
zero disables the end-threshold producer. Value 7 selects a current-track-
derived integer when that track exists in the runtime lookup, with zero as the
absence fallback. Other nonzero values map through
`PlayOptionTrackSkipTableRecord` field `+0x0c` to a score-rank ID, then through
`ScoreRankTableRecord` field `+0x0c` to the selected integer; invalid lookups
also supply zero.

The executable's table RTTI, record strides, fields, fallbacks, literal
1,010,000, and unsigned threshold comparison are recovered. The actual table
rows, current-track values, defaults, and player-facing choice labels are
external and must remain parameters. Evidence:
`claim.judgement.ordinary-terminal-end-threshold`; exact consumer:
`ordinary_terminal_end_threshold_reached` in the clean-room reconstruction.

## Configured ordinary terminal-rule interface

Type-3 rule records externally supply enable flags and thresholds for a
computed-value floor, first-bucket/event-count limit, aggregate-metric limit,
result-byte ceiling plus cumulative-weight threshold, and mode-2 zero-gauge
condition. The computed floor uses executable-owned double `0.01`; result-byte
bounds use the loaded `NotesJudgeResultTableRecord` count. Exact comparison
directions and rule ordering are normative in `spec/judgement.md`.

The configured field values, units, defaults, source schema names, and
player-facing labels are unavailable. Clean-room callers must supply them and
must not assume that a matched result band requested terminal: its cumulative
threshold can fail while the record is still consumed. Evidence:
`claim.judgement.ordinary-configured-terminal-rule`; reconstruction:
`evaluate_ordinary_configured_terminal_rule`.

## Periodic ordinary aggregate-rule interface

The type-0 load case populates the first ordinary contribution vector. Its
records can be evaluated by both kind-1 result snapshots and the unconditional
kind-2 snapshot at the end of an outer update. For kind 2, positive record
fields `+0x68` and `+0x6c` enable two persistent progress checks: one advances
from the converted signed-64 position tick through a configured interval, and
the other advances bounded buckets derived from the relationship between two
retained snapshot fields. Matching records can contribute to the aggregate or
accumulate into a linked persistent rule record. The same vector can consult a
runtime random predicate. A separate common promotion vector can also raise
the kind-2 computed value to an externally supplied record value after its
own gates pass.

Record values, probabilities, units, labels, and the full external source
schema are unavailable and remain parameters. In particular, callers must not
substitute guessed cadence or promotion constants. Exact rule-family ordering
and kind eligibility are normative in `spec/judgement.md`. Evidence:
`claim.judgement.periodic-aggregate-reevaluation`; reconstruction:
`apply_ordinary_periodic_aggregate`.

## FLK motion interface

FLK loads a second set of lane timing windows in addition to the adjusted
primary TAP-style checker. A width-indexed pair of external fields supplies the
held-source observation span and required lateral travel distance; another
external field supplies the forced-mode completion time. Values, defaults,
units, resource schema, and player-facing mode names are absent and remain
parameters. Their exact consumers are normative in `spec/notes/flick.md`.
Evidence: `claim.note.flick-motion-judgement`.

## MNE contact interface

MNE subtracts configuration field `+0x720` when deriving both current and
previous note-relative timing. Fields `+0x724`, `+0x728`, and `+0x72c` supply
the contact activation start, contact end, and required accumulated contact
respectively. Forced-mode MNE resolution additionally compares against field
`+0xb04`.

These offsets establish consumers, not values or a complete owning schema.
Their values, units, defaults, ordering guarantees, and mode selection are
unavailable and remain explicit parameters. Exact consumers are normative in
`spec/notes/mine.md`. Evidence:
`claim.note.mine-contact-aggregate-judgement`.

## Input profile synthesis interface

The input-manager reset loads the six-sensor conditioning history count and
required count from fields `+0x874` and `+0x878`. One executable mode flag
doubles the history count and transforms the requirement as
`2 * requirement + 1`; its player-facing identity is unresolved.

Profiles 0 through 6 select successive `0x3c`-stride records beginning at
`+0x898`. Within each record, fields `+0x04`, `+0x08`, `+0x0c`, and `+0x10`
supply the motion-history count, absolute threshold, positive threshold, and
negative-magnitude threshold. The same mode flag doubles each motion-history
count. Profile 7 receives a fixed stored count/threshold record, but synthesis
does not consult it. Global fields `+0x884` and `+0x880` supply profile 7's
inclusive lower and upper scalar-position bounds.

The executable fixes selection, transformations, comparison directions, and
profile-7 isolation. Runtime values, units, defaults, source threshold bytes,
device bindings, and player-facing profile names remain external parameters.
Exact synthesis is normative in `spec/input.md`. Evidence:
`claim.input.touch-photo-input-framing` and
`claim.input.snapshot-profile-synthesis`; reconstruction:
`InputProfileSynthesisConfig` and `synthesize_input_snapshot`.

## AIR profile timing interface

AIR selects one of six `0x3c`-stride external checker records from attached
root type and direction group. The selected record supplies the enabled timing
state and five interval pairs; runtime correction plus global field `+0x870`
and per-profile field `+0x898 + profile*0x3c` are added to its timing values.
The same profile index selects derived input source 0 through 5. A separate
external field at `+0xb04` participates in the forced-result timing point.

These offsets and selection rules establish consumers only. Endpoint values,
corrections, defaults, units, physical input meanings, and mode ownership are
absent and remain parameters. Exact selection and timing behavior are normative
in `spec/notes/air.md`. Evidence: `claim.note.air-secondary-judgement`.

## AirHold profile and inactive-gap interface

AirHold's root-relative start checker selects AIR profile 0, 2, or 4 from the
attached root family; every authored AHX checker instead selects profile 6.
The separate inactive-gap tracker uses field `+0x87c` in its gate, floor, and
end construction and loads exactly four thresholds from `+0x888`, `+0x88c`,
`+0x890`, and `+0x894`.

AirHold generated-path postprocessing consumes four fields from its parser
owner. Field `+0xbc` is passed as the nonzero divisor to the shared integer-grid
to chart-position converter; it is fixed at 384 because the recognized
`RESOLUTION` header is ignored. Field `+0xc4` is named `PROGJUDGE_BPM`, defaults
to 240.0, and is multiplied by four for the tempo-adaptive cadence comparison.
Field `+0xc8` is named `PROGJUDGE_AER`, resets from float bits `0x3f7fbe77`, and
supplies the final-AHD end margin when nonnegative; a negative or unordered
value disables that filter. Byte `+0xcc` is the positive predicate of the
`TUTORIAL` header and enables a separate exclusion lookup in the owner map at
`+0xe8`, using key zero and only intervals whose selector equals zero.

These are consumer interfaces, not complete schemas. External timing
endpoints, corrections, thresholds, exclusion interval contents, units, and
mode selection remain explicit inputs. Exact consumers are
normative in `spec/notes/air_hold.md`. Evidence:
`claim.note.air-hold-secondary-judgement` and
`claim.parser.header-default-dispatch`; tests:
`tests/air_hold_judgement_test.cpp`.

AirSlide consumes the same parser-owner interfaces `+0xbc`, `+0xc4`, `+0xc8`,
and `+0xcc` for position conversion, cadence comparison, final-ASC end margin,
and key-0 interval filtering. Its inactive-gap tracker uses the same external
offset family and four threshold fields `+0x888..+0x894`, but belongs to the
separate AirSlide runtime object. External values, units, and mode selection
remain parameters. Exact consumers are normative in
`spec/notes/air_slide.md`. Evidence:
`claim.note.air-slide-secondary-judgement` and
`claim.parser.header-default-dispatch`; tests:
`tests/air_slide_judgement_test.cpp`.

Ordinary AirLadder uses the parsed configuration key at `+0x84` to select an
external record. That record supplies retained-profile support data and the
completion threshold copied to runtime `+0x118`. Values, units, defaults, and
selection ownership remain parameters. Exact consumers are normative in
`spec/notes/air_ladder.md`. Evidence: `claim.note.air-ladder-judgement`;
tests: `tests/air_ladder_judgement_test.cpp`.

HeavenHold's start checker selects externally supplied profile records 0/1 for
HHD and exceptional ALD or 2/3 for HHX. Its inactive-gap path consumes the
shared gate, floor, end, and four threshold interfaces. Load also selects a
record through parsed key `+0x84`; a selected endpoint plus runtime correction
drives adjusted-end feedback, while global field `+0x70c` participates in that
comparison. Type-13 generation consumes parser-owner divisor/reference fields
`+0xbc`/`+0xc4` and optional key-zero interval filtering enabled by `+0xcc`,
but has no AirHold/AirSlide end-margin filter. The fields are fixed
RESOLUTION, `PROGJUDGE_BPM`, and the `TUTORIAL` positive predicate as above.
External values, units, and physical input meanings remain explicit
parameters. Exact consumers are
normative in `spec/notes/heaven_hold.md`. Evidence:
`claim.note.heaven-hold-judgement` and
`claim.parser.header-default-dispatch`; tests:
`tests/heaven_hold_judgement_test.cpp`.
