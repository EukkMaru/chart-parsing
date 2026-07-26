# Claim: FLK candidate contribution is asymmetric with its start gate

- ID: `claim.matching.flick-candidate-asymmetry`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `matching.candidates`, `matching.priority`, `note.flick`, `interactions.cross_note`
- Last reviewed: 2026-07-20

## Statement

An awaiting-edge FLK contributes its scheduled value to the manager's
smallest-nonnegative lane candidate, but its own ordinary edge acceptance does
not require equality with that selected value. Consequently an earlier FLK
candidate can suppress a later TAP/CHR/HOLD/Slide start on the lane, while an
earlier candidate from one of those gated families cannot suppress an
otherwise timed and accepted FLK edge.

## Anchors

- `game.exe @ RAM:00b2b690, FUN_00b2b690, complete candidate reduction before full-vector updates`
- `game.exe @ RAM:00c1fd80, FUN_00c1fd80, phase-0 FLK candidate exposure`
- `game.exe @ RAM:00c1aa90, FUN_00c1aa90, shared per-lane candidate builder`
- `game.exe @ RAM:00c20340, FUN_00c20340, FLK local rising-edge/timing path without selected-candidate read`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, shared TAP/CHR/HOLD/Slide selected-candidate equality gate`
- `game.exe @ RAM:00c29c10, FUN_00c29c10, unresolved HOLD-start candidate phase guard`
- `game.exe @ RAM:00c0dae0, FUN_00c0dae0, unresolved Slide-start candidate phase guard`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, result dispatch before local finalization`

## Observations

- The manager clears all 16 lane candidates, visits every active primary note
  to prepare/expose candidates, reduces each lane to the smallest nonnegative
  value, and only then begins a separate full-vector note-update pass. Neither
  pass consumes a rising-edge bit or a candidate value.
- FLK clears its local candidate array and calls the shared candidate builder
  only in phase 0. Its ordinary edge scan later reads the derived logical
  rising bit and primary checker result, but never reads the manager candidate
  array or its own exposed candidate array.
- TAP's shared start gate requires the manager lane value to be nonnegative and
  exactly equal to the note's exposed lane value before timing classification.
  CHR delegates to that same path. The unresolved start components of HOLD and
  Slide call the same gate and stop exposing candidates after their start phase
  reaches 4.
- Thus, with FLK value `F` and a gated-family value `G` on the same lane, both
  see the same selected minimum. If `F < G`, FLK can accept and the gated note
  cannot. If `G < F`, both can accept because only the gated note matches the
  minimum. If `F == G`, both can accept. Actual acceptance still separately
  requires each note's own timing window and the shared rising edge.
- In ordinary play, an accepted FLK edge moves phase 0 to phase 1 and the first
  held-source sample only initializes its motion extrema. It therefore does not
  submit a result on the candidate-asymmetry substep. Its direction or motion
  timeout result occurs later from phase 1, when FLK no longer exposes a
  candidate; its edge-timeout boundary is also outside the upper-exclusive
  candidate interval.
- Forced-result mode can resolve phase-0 FLK at its external timing point while
  candidate preparation still occurs, but the gated note families also bypass
  physical candidate equality in that mode. Any simultaneous forced results
  follow active-vector storage order and the already reconstructed terminal
  route; candidate priority does not select that order.

## Reasoning

The separate all-note preparation and update passes make the minimum stable for
every note in a substep. Applying the two statically distinct start gates to
that one value yields the directional suppression cases directly. The phase
and interval audit separates this input-priority effect from result routing:
ordinary FLK cannot both impose this candidate asymmetry and emit its own result
on the same substep.

## Alternatives and falsifiers

- Competing explanation: FLK's missing equality check is performed by the
  manager before invoking the note update.
- Evidence that would disprove this claim: a manager branch skipping FLK update
  when its candidate loses, an indirect selected-candidate read on the FLK edge
  path, candidate/input consumption during vector traversal, or HOLD/Slide
  using a start gate other than `FUN_00c1af30`.

## Unknowns

- Storage order for particular chart records is already observable in the
  active vector but its full parser/factory ordering across every family is not
  restated here; it matters only for simultaneous result routing, not candidate
  selection.
- Exact player-facing consequences of a suppressed gated note remain governed
  by its later input or expiration path.

## Consequences

- Ghidra mutations: none in the live project; temporary-clone analysis only.
- Spec sections: `spec/matching.md`, `spec/notes/flick.md`.
- Reconstruction code: `LaneCandidateGate` and
  `lane_candidate_gate_allows_start` in `include/chart/reconstruction.hpp`.
- Tests: `tests/candidate_interaction_test.cpp`.

## Verification

The manager preparation/update loops, FLK candidate and edge virtuals, shared
TAP gate, HOLD/Slide phase guards, ordinary FLK first-sample motion behavior,
timeout inclusivity, and forced branch were inspected independently. Focused
tests cover `F < G`, `G < F`, equality, and the absent-candidate sentinel.
