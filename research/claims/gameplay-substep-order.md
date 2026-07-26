# Claim: each gameplay substep derives input before updating notes

- ID: `claim.timing.gameplay-substep-order`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.clock`, `timing.discontinuity`, `input.buffering`, `note.tap`
- Last reviewed: 2026-07-26

## Statement

Each ordinary gameplay substep first derives and appends an input snapshot,
then updates the active-note manager at the corresponding time. Its first
integer offset is the signed clamp of the native 32-bit modular result
`previous_counter - current_counter + 1`, and it increments through zero, so
it performs one or at most two input/note pairs.

## Anchors

- `game.exe @ RAM:00da9820, FUN_00da9820, outer gameplay update owner`
- `game.exe @ RAM:00da8730, FUN_00da8730, ordinary catch-up substep loop`
- `game.exe @ RAM:00da70c0, FUN_00da70c0, alternate input-update path`
- `game.exe @ RAM:00c2dfa0, FUN_00c2dfa0, input snapshot derivation`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, active-note manager update`
- `game.exe @ RAM:00a7fed0, FUN_00a7fed0, update-counter field reader`
- `game.exe @ RAM:00a80240, FUN_00a80240, update-counter field writer`
- `game.exe @ RAM:01032960, FUN_01032960, runtime counter adapter`
- `game.exe @ RAM:0103c3b0, FUN_0103c3b0, runtime counter source`
- `game.exe @ RAM:01043a90, FUN_01043a90, AutoScan sequence initialization`
- `game.exe @ RAM:01043c20, FUN_01043c20, successful AutoScan increment`
- `game.exe @ RAM:01043d20, FUN_01043d20, AutoScan sequence getter`
- `game.exe @ RAM:0175b95c, initialized ordinary scalar step 0.5`
- `game.exe @ RAM:01867724, initialized alternate scalar step 0.25`

## Observations

- The outer gameplay update reaches either `FUN_00da8730` or an alternate path
  containing `FUN_00da70c0`, then updates chart/object setup afterward.
- The ordinary substep loop calls the input processor immediately before the
  active-note manager on every iteration.
- Both calls receive time values formed from separate base fields plus the same
  loop-indexed offset.
- The ordinary dispatch reaches this loop only when the alternate global mode
  flag is clear, selecting an initialized scalar increment of 0.5. The loop
  contains a 0.25 selector for a mid-call flag change, while the stable
  alternate dispatch uses a different single-update path.
- The counter is the 32-bit AutoScan successful-update sequence. Construction
  zeros it; a newly available successful scan executes its only increment; and
  the BoardCtrl/input chain copies the dword unchanged into gameplay.
- The loop computes `stored - current + 1` modulo `2^32`, interprets the result
  bits as signed, clamps between -1 and 0, processes every integer through
  zero, and only then stores current. Consequently any ordinary lag of two or
  more counter values is represented by exactly offsets -1 and 0 rather than
  an unbounded replay, while adjacent wrap from `0xffffffff` to zero produces
  the ordinary zero offset.

## Reasoning

Call order and the shared offset establish the tick-local relationship needed
for judgement fidelity: notes observe the snapshot derived for their own
substep. The counter's sole increment and copy chain establish its successful-
scan cadence. The modular arithmetic and signed clamp instructions close both
the frame-drop and overflow paths and prove that catch-up is deliberately
capped at one prior substep.

## Alternatives and falsifiers

- Competing explanation: the repeated loop only interpolates rendering.
- Evidence that would disprove this claim: active-note manager analysis showing
  no gameplay state or judgement work, or a caller reversing input and note
  order on the ordinary gameplay path.

## Unknowns

- The semantic name of the alternate global mode is deliberately unassigned.
- Whether every chart setup call after this loop affects the following or the
  current substep requires further ordering analysis.

## Consequences

- Ghidra mutations: added plate comments at `RAM:01043c20` and
  `RAM:00da8730`.
- Spec sections: `spec/timing.md`, `spec/input.md`.
- Reconstruction code: `include/chart/reconstruction.hpp`.
- Tests: `tests/play_clock_test.cpp`.

## Verification

The input producer, snapshot selector, TAP consumer, active-note update, outer
update caller, complete AutoScan counter field inventory, BoardCtrl/input copy
chain, and instruction-level modular signed clamp were traced as independent
stages. Focused tests cover zero-, one-, and larger-lag inputs, adjacent full
wrap, reverse wrap, and a half-range discontinuity. Counter detail is
independently recorded in `claim.timing.autoscan-counter-wrap`.
