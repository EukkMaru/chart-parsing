# AIR secondary note

## Parser attachment and runtime ownership

`AIR`, `AUR`, `AUL`, `ADW`, `ADR`, and `ADL` all describe parsed secondary
type 3. They do not append standalone chart records. The parser attaches them
to an existing root only when all of these match:

- the root has no assigned in-range secondary type;
- its parsed root type equals the command's requested root family;
- its start lane and width equal the AIR command values; and
- its chart position equals the AIR position under the parser epsilon rule.

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
transition. Commit and active-vector removal occur on the next normal note
tick. Because the root precedes its AIR secondary in the vector, a root result
can activate the shared terminal route before AIR resolves later in the same
pass. AIR may also outlive a root that resolved earlier. Evidence:
`claim.note.air-secondary-judgement`, `claim.judgement.forced-result-mode`, and
`claim.judgement.shared-result-two-stage-routing`.
