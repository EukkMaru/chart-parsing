# Claim: TAP input is gated by the earliest per-lane candidate

- ID: `claim.note.tap-candidate-judgement-gate`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `input.logical_state`, `matching.candidates`, `matching.priority`, `judgement.miss`, `note.tap`
- Last reviewed: 2026-07-20

## Statement

For each logical lane, the update path selects the smallest nonnegative
scheduled-time candidate among eligible active notes; a TAP lane can be judged
from input only when its own candidate equals that selected value, while a TAP
that reaches its late bound without an accepted lane result completes through a
terminal result path.

## Anchors

- `game.exe @ RAM:00b2b690, FUN_00b2b690, active-note update and per-lane reduction`
- `game.exe @ RAM:00c1d4e0, FUN_00c1d4e0, TAP candidate preparation`
- `game.exe @ RAM:00c1aa90, FUN_00c1aa90, lane-window eligibility`
- `game.exe @ RAM:00c1b670, FUN_00c1b670, per-note candidate read`
- `game.exe @ RAM:00c1d510, FUN_00c1d510, TAP completion/result bridge`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, TAP input, candidate, and late-bound gate`
- `game.exe @ RAM:00c2de70, FUN_00c2de70, logical-lane bit test`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, downstream result propagation boundary`
- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, manager reset and active-object teardown`

## Observations

- Each manager update resets 16 candidate slots to negative one. It asks every
  active note to prepare candidates, then retains the smallest nonnegative
  value returned for each lane.
- TAP candidate preparation resets its own 16 slots, computes current gameplay
  time minus scheduled time, and exposes a candidate only for covered lanes
  whose configured outermost checker window includes that delta.
- The manager runs TAP's center-window mutation only after candidate exposure
  and reduction. It therefore cannot change candidacy in the same substep,
  although later input classification observes the changed center band.
- The TAP judgement gate loops covered lanes, tests a bit for the lane in input
  snapshot selector 0, and requires equality between the manager candidate and
  the TAP's candidate for that lane.
- It submits the delta to the selected lane's checker and stops at the first
  result whose table acceptance byte is nonzero.
- When no lane is accepted before the overall late bound, the update remains
  incomplete. At or beyond the late bound it selects a terminal table result
  and returns completion without a successful input lane.
- Completion is bridged through TAP virtual result functions into a shared
  downstream result handler. Objects whose state reaches terminal value 2 are
  later removed and destroyed.

## Reasoning

The manager's minimum reduction and the TAP equality test jointly implement
per-lane priority. The late-bound branch is independent of input acceptance and
therefore establishes automatic terminal completion for an expired TAP.

## Alternatives and falsifiers

- Competing explanation: the manager candidate is only a rendering hint and
  does not restrict input judgement.
- Evidence that would disprove this claim: a TAP input path that accepts a lane
  without the equality gate, or a post-reduction transformation that changes
  which candidate the checker compares.

## Unknowns

- Equal TAP candidates are covered by
  `claim.matching.tap-equal-candidate-fanout`; FLK cross-family asymmetry is
  covered by `claim.matching.flick-candidate-asymmetry`. Remaining candidate-
  producing family interactions and creation order remain open.
- Snapshot selector 0's bit is resolved as a derived rising edge by
  `claim.input.tap-rising-edge-snapshot`; its physical source remains open.
- Forced-result selection is covered by
  `claim.judgement.forced-result-mode`; its external activation identity,
  and exact terminal player-facing judgement name remain open. Adjacent-window
  adjustment is covered by `claim.judgement.tap-adjacent-window-adjustment`.
  Shared result ownership and terminal routing are covered by
  `claim.judgement.shared-result-two-stage-routing` and
  `claim.interactions.result-terminal-short-circuit`.
  Per-substep center widening is covered by
  `claim.judgement.tap-center-window-adjustment`.
- The authoritative clock cadence is closed; runtime mode/rate/correction
  producers remain open.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/input.md`, `spec/matching.md`, `spec/judgement.md`,
  `spec/notes/tap.md`.
- Reconstruction code: `reduce_lane_candidate` and the TAP lifecycle helpers in
  `include/chart/reconstruction.hpp`.
- Tests: candidate/lifecycle behavior is covered by
  `tests/tap_lifecycle_test.cpp`; classifier behavior is covered separately by
  `tests/tap_window_test.cpp`.

## Verification

Reset, eligibility, manager reduction, equality, input, late completion, and
cleanup were followed through separate functions. Equal TAP candidates were
independently audited in `claim.matching.tap-equal-candidate-fanout`.
