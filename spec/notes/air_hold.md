# AirHold secondary note

## Parser attachment and authored checkpoints

`AHD` and `AHX` resolve to secondary parsed type 5. They do not create a
standalone root record. The sixth event token names the record family being
attached or extended.

For an initial attachment, the parser searches for a root whose type equals
that token's resolved type and whose current-endpoint position, lane, and width
equal the command fields under the parser epsilon comparison. HOLD supplies
its endpoint and Slide its current final path record. The root must not already
have an in-range secondary type. The
supported root families observed in the exact binary/corpus path are TAP, HLD,
SLD, CHR, FLK, and MNE (parsed types 0, 1, 2, 4, 6, and 11). Failure to find a
match takes the parser diagnostic path.

For a continuation, the sixth token resolves to type 5 and the command matches
the saved endpoint of an existing AirHold secondary. The parser appends that
previous endpoint to the authored-checkpoint vector only if the previous
command was `AHX`, then replaces the saved endpoint and command flag. Runtime
loading likewise includes the final endpoint only if the final command is
`AHX`. Thus AHD extends the AirHold path/end without creating an authored
timing checkpoint; AHX does both. Reconstruction:
`air_hold_is_authored_checkpoint` and
`air_hold_runtime_checkpoint_count`.

The runtime factory allocates a separate `projView::AirHoldNote`, links it from
the root, appends the root to the primary vector, and appends AirHoldNote to the
separate secondary vector. AirHoldNote owns its component state and can outlive
a root removed earlier. The manager updates all primaries before any secondary.
Evidence: `claim.note.air-hold-secondary-judgement`.

## Start component

The start checker uses AIR's retained-profile timing classifier and the same
root-relative anchor policy:

- root start for root types 0, 4, 6, and 11;
- root end for type 1; and
- the last path/control point for types 2 and 13.

The secondary direction field stays at reset value zero. Root types 1, 2, and
13 therefore select external profile 0; types 0, 4, and 11 select profile 2;
type 6 selects profile 4. Exact window behavior is the AIR classifier in
`spec/notes/air.md`. When its fine result becomes accepted, the component
submits source category 9 and enters phase 5.

AirHoldNote never exposes a lane candidate and never reads the manager-selected
candidate. The start profile is derived input rather than a lane rising edge.

## Sustain contact and inactive gap

At every eligible path update, the runtime reads derived input profile 7 and
then profile 6. It owns a persistent admission bit:

`admitted = admitted || !profile_7 || profile_6`

Current path activity is:

`start_reached && admitted && profile_7`

The admission update runs first, so simultaneous profiles 6 and 7 can admit
activity in that same substep. Before the scheduled start, contact remains
inactive even if admission has latched. Reconstruction:
`AirHoldContactState` and `update_air_hold_contact`.

The inactive-gap state machine is structurally identical to HOLD's four-
threshold tracker. It clips elapsed time to its configured end, remembers
whether contact ever became active, retains the maximum gap since the greater
of last active time and the participation floor, and maps threshold indices to
anonymous result bytes `4, 3, 2, 1, 0`. It has its own external gate, floor,
end, and threshold interface; no values are assumed.

## Generated path results

Successful chart postprocessing constructs an anchor chain in this order:

1. the root-relative start anchor;
2. each saved AHX endpoint; and
3. the final saved endpoint, whether AHD or AHX.

It measures each anchor on a single-precision grid:

`grid_tick = trunc((major + minor * 0.25F) * 384.0F + 0.5F)`

Thus one major unit is 384 ticks and one minor unit is 96 ticks. NaN,
infinity, or an out-of-range converted grid value produces `INT32_MIN`
through `CVTTSS2SI`. All later tick subtraction/addition wraps at 32 bits.
For every adjacent anchor pair, select an initial sample step of 384 ticks. While the BPM
selected at the segment endpoint's scheduled milliseconds is below four times
the `PROGJUDGE_BPM` header, double that BPM and integer-halve the step. Start at one
step after the segment start and append samples only while the unsigned step
offset is strictly below the segment tick length. After each sample, recompute
the step from the BPM selected at that sample's scheduled value. The
authoritative BPM schedule and position conversion are specified in
`spec/timing.md`; clean-room note generation composes those helpers with the
separate path interpolation interface.

The type-5 duration accessor is signed and the parser does not reject extreme
endpoints. The segment length is wrapped `end_tick - start_tick`, then compared
with the wrapped offset as unsigned. A delta above `INT32_MAX` therefore enters
a very large source generation range; this is not equivalent to signed
`end < start` at the wrap boundary. The clean-room API reports
`source_large_unsigned_span_expansion` through
`evaluate_air_hold_path_generation`. A zero adaptive step is separately
reported as source cursor nonprogress; callers do not attempt either unsafe
source allocation/loop.

Generated records follow these exact flag rules:

- the first sample in every segment is type 5 with emission disabled;
- later samples in that segment are type 5 with emission enabled;
- before each segment after the first, append the current anchor as a disabled
  type-6 boundary; and
- append the final endpoint once as a disabled type-7 end record.

Types 5, 6, and 7 are producer-side structural tags. AirHold's gameplay
consumer uses each record's scheduled value and emission byte, so disabled
boundary/end records still affect how many substeps are required to drain the
front-only vector.

Two postprocessor filters can only clear more emission bytes:

- If the final command is AHD and the end-margin parameter is nonnegative,
  disable a record when
  `record_grid_tick + round(sample_step * end_margin) >= final_grid_tick`.
  Final AHX bypasses this end filter. The parameter is the `PROGJUDGE_AER`
  header and resets from float bits `0x3f7fbe77`. The rounding conversion is
  `CVTTSS2SI`, so positive infinity/out-of-range produces `INT32_MIN`; the
  record-tick addition then wraps before the signed comparison. NaN and
  negative margins bypass the filter through the ordered nonnegative test.
- When the parser's exclusion filter is enabled by positive `TUTORIAL`, select
  its key-0 interval
  vector and disable a record only when `start < scheduled < end` and that
  interval's selector equals zero. Both interval boundaries are strict.

Reconstruction: `air_hold_grid_tick`,
`evaluate_air_hold_path_generation`, `air_hold_sample_step`,
`generate_air_hold_path_records`, `air_hold_in_exclusion_interval`, and
`filter_air_hold_path_emissions`.

One gameplay substep considers only the vector front. If it is due, that one
record is consumed. A clear emission flag produces no result and does not reset
the retained maximum gap. An enabled ordinary record:

1. classifies the retained maximum gap;
2. converts it through `4, 3, 2, 1, 0`;
3. submits source category 10; and
4. resets the maximum to the currently open inactive streak.

Forced-result selection supplies its configured byte instead and bypasses the
ordinary maximum-gap reset. Reconstruction:
`update_air_hold_generated_checkpoint`.

## Authored AHX results and lifetime

Every authored AHX endpoint owns an independent AIR-style retained-profile
checker initialized with external profile 6. Only the current checker is
updated, and at most one authored checkpoint resolves in one substep. A
nonfinal authored checkpoint submits source category 12; the final one submits
13.

The path component enters phase 5 only after the generated record vector is
empty and all authored AHX checkers have resolved. AirHoldNote requests its
deferred base terminal transition only when the independent start component is
also phase 5. Candidate preparation therefore remains empty throughout its
lifetime.

Source categories 9, 10, 12, and 13 map to shared categories 3, 5, 7, and 7.
All are aggregate-authoritative when normal runtime category/result bounds
accept them. If terminal routing is already active, they take the shared
observer-only path. Reconstruction: `air_hold_*_source_category`,
`air_hold_path_complete`, and `air_hold_is_terminal`; focused tests:
`tests/air_hold_judgement_test.cpp`.

## Presentation resources and vertical origin

AirHold authors no independent vertical field. Its constructor initializes the
presentation value to 1, so ordinary roots reach exact common transform
`(1 - 1) * 3.8934999 = 0`. A type-13 root replaces the value from its final
control before the same transform. A viewer must not substitute an authored
value-5 plane as though it were binary state.

Decoded width selects external table index:

```text
clamp(decoded width - 1, 0, 15)
```

The root lateral position is `4*lane + 2*width - 32`. Its lateral resource
scale is 1 when external native width is below 1, otherwise
`decoded width / external native width`. The attached load creates two
width-indexed and two fixed resources for the root/path plus two width-indexed
resources for every authored AHX checker. Resource identities and intrinsic
geometry are external data; their consumers and transforms are normative.

The selected resource wrapper supplies three external offsets. The root
transform is:

```text
position = (base lateral + offset X,
            base vertical + offset Y,
            projected start + offset Z)
scale = (width/native width, 1, 1)
visible = start phase != 5
```

Every unresolved AHX checker projects its own stored schedule/SLA tag. Its two
resources share:

```text
position = (base lateral + offset X,
            15.724 + offset Y,
            checkpoint projection + offset Z)
scale = (width/native width, 1, 1)
```

Resolution hides both resources and removes that point from the presentation
envelope. Load-time resource attachment verticals are exactly:

```text
0, 15.574, 15.574, 0, 15.724, -15.574
```

They are attachment coordinates, not a claim about unavailable model extents.

## Projected path envelope

While nonterminal, presentation folds projected start, projected end, and all
unresolved AHX projections into an ordered minimum/maximum. It also records
whether authored end or any unresolved checkpoint remains in the future. If so
and the start phase is already 5, the cached projection of raw position zero is
included in the envelope.

Both path resources use:

```text
position = (base lateral + offset X + 0.0001,
            base vertical + offset Y,
            maximum projection + offset Z)
projected scale = (maximum projection - minimum projection) * 0.25
```

The first path resource has lateral scale 1. The second has the root's
width/native-width lateral scale. Both are shown while the note is
nonterminal.

Path phase selects external resources and feedback:

| Path phase | Resource variant | First scale | Field-feedback flag |
| ---: | --- | ---: | --- |
| 3 | best current gap | 1 | set |
| 4 | other current gap | 0.5 | clear |
| all other values | default | 1 | clear |

The second resource in each selected pair uses unit scale. A shared
schedule-visibility probe controls animation/state advancement: raw delta below
30 succeeds immediately; farther values must project inside inclusive
`[-550, 550]`. This is presentation state only.

Gameplay/lifetime update precedes presentation in a scheduled tick. Once start
and path phases both equal 5, the first half requests deferred terminal and the
presentation half returns without submitting new transforms. It does not issue
an immediate hide. Maintenance/final removal owns resource finalization and
destruction.

Preload steps 0 through 4 cover width-indexed/fixed root and checkpoint model
families. Steps 5 through 8 cover four path/feedback families. Steps 9 through
14 are idle; readiness is reported after step 14. Reset releases and hides the
preload objects.

Evidence: `claim.presentation.common-air-transform` and
`claim.presentation.air-hold-model-path`. Reconstruction:
`air_hold_resource_width_index`, `AirHoldResourceTransform`,
`build_air_hold_*`, and the phase/envelope helpers. Focused test:
`tests/air_hold_presentation_test.cpp`.
