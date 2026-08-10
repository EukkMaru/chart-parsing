# AIR secondary note

## Parser attachment and runtime ownership

`AIR`, `AUR`, `AUL`, `ADW`, `ADR`, and `ADL` all describe parsed secondary
type 3. They do not append standalone chart records. The parser attaches them
to an existing root only when all of these match:

- the root has no assigned in-range secondary type;
- its parsed root type equals the command's requested root family;
- its current-endpoint lane and width equal the AIR command values; and
- its current-endpoint chart position equals the AIR position under the
  parser epsilon rule.

The lane/width accessor selects the final path record for parsed types 2, 9,
10, and 13 and otherwise selects the record's base fields. The position
comparison uses the record's stored current endpoint. HOLD therefore supplies
its end and Slide its current final path record.

A failed search follows the parser's diagnostic/failure path. A successful
attachment stores secondary type 3, the AIR property, and this direction code:

| Command | Ordinary | Mirrored |
| --- | ---: | ---: |
| AIR | 0 | 0 |
| AUR | 1 | 2 |
| AUL | 2 | 1 |
| ADW | 3 | 3 |
| ADR | 4 | 5 |
| ADL | 5 | 4 |

When the root is constructed, the runtime factory creates a separate
`projView::AirNote`, links it from the root for metadata propagation, and
appends the root then AirNote to the active vector. The AirNote owns its checker,
phase, outcome, and lifetime. It does not require the root to stay alive after
loading. Evidence: `claim.note.air-secondary-judgement`.

## Anchor and input profile

AIR scheduled position is derived from the attached root:

- root start for parsed root types 0, 4, 6, and 11;
- root end for type 1; and
- the last path/control point for types 2 and 13.

The external checker/profile group is selected as follows:

| Root types | Direction 0..2 | Direction 3..5 |
| --- | ---: | ---: |
| 1, 2, 13 | 0 | 1 |
| 0, 4, 11 | 2 | 3 |
| 6 | 4 | 5 |

Other root types are not handled by the profile initializer. Each valid profile
selects one of derived input bytes 0 through 5 and a `0x3c`-stride external
timing configuration record. Values and physical profile meanings remain
parameters.

The newest input snapshot contains eight derived profile bytes and a distinct
marker. At every eligible AIR check, the runtime sets the marker before reading
the selected current profile byte. This does not mutate that byte. The next
input-build pass consults the marker while deriving new profile state; its full
physical interpretation remains unresolved. The exact current-snapshot
ordering is modeled by `AirProfileSnapshot` and `sample_air_profile`.

## Candidate and retained timing state

AIR never exposes a lane candidate and never reads the manager-selected
candidate. It therefore neither constrains nor depends on the TAP/HOLD/FLK lane
reduction. Its local phase is 0 while awaiting a result and 3 after resolution.

The checker uses the same five center-first half-open interval geometry as the
shared classifier, but owns an independent fixed rank/result table:

| Fine index | Rank | Accepted | Coarse tier | Side code |
| ---: | ---: | ---: | ---: | ---: |
| 0 | 0 | no | 0 | 0 |
| 1 | 1 | no | 0 | 0 |
| 2 | 4 | yes | 0 | 1 |
| 3 | 6 | yes | 1 | 1 |
| 4 | 8 | yes | 2 | 1 |
| 5 | 10 | yes | 3 | 1 |
| 6 | 11 | yes | 4 | 0 |
| 7 | 9 | yes | 3 | 2 |
| 8 | 7 | yes | 2 | 2 |
| 9 | 5 | yes | 1 | 2 |
| 10 | 3 | yes | 0 | 2 |
| 11 | 2 | yes | 0 | 2 |

For finite values and an enabled checker:

1. Do nothing unless `outermost.lower < delta`; equality is not eligible.
2. Classify the current fine index. If its rank is at least the retained rank
   and the selected profile byte is nonzero, replace the retained index.
3. Before the pivot, return nonaccepting index 1 even when an input was
   retained.
4. At or after the pivot, return retained center index 6 immediately. For any
   other retained index, return it when it is better-ranked than the current
   timing location. A current nonzero sample of equal/better rank can also be
   returned immediately.
5. Otherwise continue with index 1 until `delta >= outermost.upper`, which
   returns fine index 11. Retained center takes precedence over this timeout.
6. Complete only at/after the pivot with a nonzero acceptance entry.

The exact pure state transition is `update_air_timing`; tests are in
`tests/air_judgement_test.cpp`.

## Result and lifetime

The provisional coarse tier can pass through active result-control remapping,
after which coarse tier and side code are canonicalized back to the detailed
fine code used downstream. Direction codes 0 through 2 submit source category
7; codes 3 through 5 submit category 8. Both map to shared category 3 and can
update the authoritative aggregate for a runtime-valid result byte. If shared
terminal routing is already active, the result instead takes the observer-only
path.

Manager forced-result state supplies a synthetic profile level and selected
result at its external timing point, while retaining the checker's strict early
and pivot gates. It bypasses ordinary acceptance only after those gates.

Completion sets local phase 3 and requests the common deferred base terminal
transition. Commit and secondary-vector removal occur on the next normal note
tick. Because the manager updates the complete primary vector before the
complete secondary vector, a root result can activate the shared terminal
route before AIR resolves later in the same pass. AIR may also outlive a root
that resolved earlier. Evidence:
`claim.note.air-secondary-judgement`, `claim.judgement.forced-result-mode`, and
`claim.judgement.shared-result-two-stage-routing`.

## Root presentation and motion

AirNote selects one external model record by direction and decoded width.
Clamp `decoded_width - 1` to `[0,15]`; the executable row families are:

| Direction code | First through last row |
| ---: | ---: |
| 0 | 161 through 146 |
| 1 or 2 | 177 through 162 |
| 3 | 193 through 178 |
| 4 or 5 | 209 through 194 |

Thus mirrored pairs 1/2 and 4/5 share model records. Direction code 2 (AUL)
and 4 (ADR) negate the external native-width lateral scale; all others keep it
positive. Initial base translation is the common lane center, vertical
`(stored_vertical - 1) * 3.8934999`, and parked depth `-10000`. The external
record also contributes three configurable offsets: `4 * record_x`,
`3.8934999 * record_y`, and the shared base-offset/DCM depth contribution for
`record_z` without another `-65` origin.

The manager's Air reference position is
`mapped_chart_position * 0.06 * external_reference_scale + 216000`, where a
disabled external Air configuration selects scale `1`. For direction codes
0..2, multiply that reference by the external upper scale (or `1` when
disabled). For codes 3..5, multiply by the external lower scale (or `1`) and
then by executable factor `0.4`. Subtract the region-adjusted scheduled
position and feed the result to the common projected-depth function with base
offset and positive-delta DCM enabled. Add the external record depth offset,
then submit the final translation and signed scale.

Load hides the model. Each nonterminal update forces it visible and submits the
transform. Terminal phase 3 skips model writes, so the last transform remains
until maintenance finalizes the external model handle; the terminal phase does
not issue a separate hide.

Parsed presentation property zero stores the lower-direction-class bit,
property one stores zero, and property two stores one. The closed Air family
path has no active consumer of that stored bit and resource selection uses the
direction directly. This is recorded as a no-observed-consumer result rather
than assigning an asset meaning.

When terminal routing returns a nonzero feedback flag, direction codes 0..2
select fixed external cue selector `0x3c`, while codes 3..5 select `0x134`.
Only codes 0..2 additionally call the shared field-feedback producer. Cue and
effect contents remain external; their selectors and gates are exact.

Preload stages the four unique model families, then feedback groups 7 and 8;
readiness is set at step 12. Evidence:
`claim.presentation.air-root-motion-feedback` and
`claim.presentation.common-air-transform`; reconstruction:
`air_model_resource_row`, `air_direction_resource_scale_sign`,
`air_manager_reference_position`, `air_model_projection_delta`, and the
`air_external_resource_*` helpers; focused tests:
`tests/air_presentation_test.cpp`.
