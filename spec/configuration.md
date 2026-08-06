# Gameplay configuration

## Runtime materialization projection interface

The pending-record materialization gate uses executable constants `30.0F`,
`-65.0F`, `1.5F`, and inclusive `[-550.0F, 550.0F]` together with two runtime
inputs: a speed multiplier and a projection base offset.

Gameplay setup starts with the current `PlayOptionSpeedID`, which is zero after
fresh construction. It scans the selected `SkillBefore` record's units in
source order. The first unit of type 6 is decisive: a proposed ID below the
`PlayOptionSpeedTable` count replaces the current ID; an invalid proposed ID
leaves the current ID unchanged and stops the scan. Setup reads the selected
0x38-byte `PlayOptionSpeedTableRecord`'s double at `+0x10`, narrows it to
float, and applies the snapshot's `MAXSS` clamp against `0.1F`. Thus values at
or below `0.1F`, invalid IDs, and NaN become `0.1F`; larger values and positive
infinity survive. The result is the sole non-reset write to the manager speed
consumed by materialization.

The base offset is configuration float `[OFFSET] DRAW`. Its executable
descriptor default is exactly `0.0F`; configuration finalization copies it to
field `+0xafc`, and manager reset and successful chart setup copy that field
to the materialization owner. Deployed table rows and configuration values are
absent, so clean-room code accepts those resource contents as parameters while
preserving the recovered selection, clamp, and default.

A chart-owned keyed-region transform can replace the far-path scheduled
position, and a separate chart-owned schedule supplies a factor only for
positive adjusted deltas. Those values are reconstructed from
STP/SFL/SLP/DCM containers and an SLA-selected key. Exact container lookup,
conversion, and predicate order are normative in `spec/timing.md`. Evidence:
`claim.pipeline.runtime-note-materialization-order`,
`claim.timing.projection-schedule-materialization`, and
`claim.configuration.runtime-materialization-input-producers`.

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
must remain parameters. Parsed HXD records select extended HOLD profile 4
instead of ordinary HOLD profile 0; the two external profile contents must not
be assumed equal. Evidence: `claim.judgement.tap-window-classification` and
`claim.note.hold-extended-profile-selection`.

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

The result pipeline can hold ordered `SkillChangeJudgeResultData` control units
with coarse-tier thresholds. It does not change timing intervals; after
classification it replaces any valid provisional tier less than or equal to
the first unit's threshold with tier 0. Later units do not participate.

Construction initializes the dedicated unit vector empty. A changed skill
identity resets the prior controller state, copies the new nonnegative profile
ID, and appends matching units in source order. Reset stores profile ID `-1`,
destroys the units, and empties the vector. The remap requires the loaded ID,
the first unit, and that unit's source record, but does not call the separate
temporary-effect lifetime predicate used by other skill consumers.

The validity bound is the count of a runtime-loaded
`NotesJudgeResultTableRecord` table. Its values and skill thresholds remain
unavailable, so the reconstruction accepts profile ID, ordered units, and count
as explicit inputs. Evidence: `claim.judgement.active-tier-zeroing`.

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

The shared TAP-style checker selects external judgement-window records by
parsed type and extended form. Ordinary TAP/HOLD/Slide/HeavenHold selects
record 0; CHR selects 4; FLK selects 6; MNE selects 11; and an extended
HOLD/Slide/HeavenHold overrides its ordinary selector with 4. HXD is therefore
gameplay-distinct from HLD even when its optional subtype is missing. The
external record values remain parameters, but the selector table, extended
override, and legacy defaults are exact. Evidence:
`claim.note.hold-extended-profile-selection`; tests:
`tests/hold_variant_test.cpp`.

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

The enable byte is the current setup state's inner byte at `+0x24` and is
copied only after chart validation succeeds. Its upstream player-facing
semantic name remains unresolved, but its producer path is closed. A
whole-binary write and address-escape audit closes modes 1 and 3 through 6 as
unreachable in this exact snapshot: only reset/teardown mode 0 and tutorial
mode 2 can reach the manager field. The mode-6 RNG branch is consequently not
a gameplay input. The distinct companion selector at `+0x360` is reset to zero
and has no other writer or address escape, so its selector returns 1 whenever
the enable is active. Unsupported-at-runtime cases remain documented because
they exist in the selector implementation, not because gameplay can select
them. Evidence:
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

All ordinary gameplay control vectors come from one selected `SkillBefore`
record. The result owner caches a three-integer identity. Any changed component
resets all five vectors and their retained progress before looking up the
identity's middle ID in the ordered record map; exact identity equality retains
current progress. A missing record leaves every vector empty. Found source
units route in order as type 0 contribution, type 1 promotion, type 2 negative
adjustment, type 3 terminal, and type 5 result-remap controls. Type 4 and
unknown types are ignored. Evidence:
`claim.configuration.skill-before-gameplay-control-loading`.

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
accumulate into a linked persistent rule record. A unit marked one-shot tests
and sets its source-order index in a retained bit vector; an already set valid
bit rejects the unit. A malformed out-of-range index reads clear but cannot be
marked and therefore remains repeatable. A separate common promotion vector
can also raise the kind-2 computed value to an externally supplied record value
after its own gates pass.

Record values, units, labels, and the full external source schema are
unavailable and remain parameters. There is no random/probability decision on
this path. In particular, callers must not
substitute guessed cadence or promotion constants. Exact rule-family ordering
and kind eligibility are normative in `spec/judgement.md`. Evidence:
`claim.judgement.periodic-aggregate-reevaluation` and
`claim.configuration.skill-before-gameplay-control-loading`; reconstruction:
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

Ordinary AirLadder parsed offset `+0x84` is not a configuration key. The parser
writes the accepted-event ordinal there; `NotesPreCalcManager` uses it as the
checked identity for an authored type-9 precompute. The final authored endpoint
schedule from that precompute is copied to runtime `+0x118`, so no unavailable
completion-threshold value belongs in the configuration boundary. Numerical
retained-profile-7 support remains externally configured as described in the
input sections. Exact consumers are normative in `spec/notes/air_ladder.md`.
Evidence: `claim.note.air-ladder-precalc-presentation`; tests:
`tests/air_ladder_judgement_test.cpp`.

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
