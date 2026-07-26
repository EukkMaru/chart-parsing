# Claim: HOLD checkpoints classify retained inactive gaps

- ID: `claim.note.hold-sustain-gap-lifecycle`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.clock`, `input.logical_state`, `judgement.types`, `judgement.miss`, `note.hold`, `state.ownership`, `interactions.cross_note`
- Last reviewed: 2026-07-20

## Statement

After its adjusted start, a HOLD tracks the longest inactive interval against
four external thresholds, emits at most one due checkpoint result per manager
substep, resets the retained maximum to any still-open gap after each emitted
checkpoint, and requests terminal state only after both its start phase and its
checkpoint phase reach 4.

## Anchors

- `game.exe @ RAM:00c2adc0, FUN_00c2adc0, sustain/checkpoint initialization`
- `game.exe @ RAM:00c18d60, FUN_00c18d60, gap tracker parameter load`
- `game.exe @ RAM:00c197d0, FUN_00c197d0, gap tracker update and current classification`
- `game.exe @ RAM:00c184f0, FUN_00c184f0, retained-gap classification`
- `game.exe @ RAM:00c19580, FUN_00c19580, post-checkpoint maximum reset`
- `game.exe @ RAM:00c29c50, FUN_00c29c50, sustain, final-bound, and checkpoint update`
- `game.exe @ RAM:00c0c370, FUN_00c0c370, checkpoint-vector front removal`
- `game.exe @ RAM:00c2b480, FUN_00c2b480, HOLD state-1 update order`
- `game.exe @ RAM:00c2aae0, FUN_00c2aae0, two-phase completion query`
- `game.exe @ RAM:00c1c130, FUN_00c1c130, deferred terminal request`

## Observations

- HOLD initialization copies a vector of `0x20`-byte checkpoint records and
  creates an embedded gap tracker. It appends exactly four floats from runtime
  configuration fields `+0x710` through `+0x71c`.
- The tracker's participation floor is configuration field `+0x708`. Its
  update gate is that floor plus a field which the start-window initializer
  explicitly zeroes for this object. Its end is the scaled record duration
  plus the same floor.
- An update before the gate returns -1 without mutation. Later elapsed time is
  clamped to tracker end. Active input latches `ever-active` and updates
  `last-active`; the current gap is clamped time minus the greater of
  `last-active` and the participation floor. The tracker retains the maximum
  such gap.
- If activity has ever occurred, classification returns the first threshold
  index whose value is greater than or equal to the gap, or 4 if none is. With
  no activity it returns 4. Retained-gap classification returns -1 only while
  current elapsed time remains below the participation floor.
- Anonymous result-byte conversion is index 0 to byte 4, 1 to 3, 2 to 2, 3 to
  1, and every other index to 0.
- The checkpoint path examines only the vector front. It is due when its scaled
  time plus the runtime offset is less than or equal to manager current time.
  A flagged checkpoint crosses the HOLD result wrapper with category 3 when
  record field `+0x10` equals 1, otherwise category 2. The vector then removes
  exactly that one front record.
- After a checkpoint, the retained maximum becomes the current gap rather than
  zero. This preserves an inactive streak spanning checkpoints while forgetting
  an older completed gap.
- The checkpoint phase is 2 only when the current gap class is index 0, and 3
  otherwise. An empty vector makes the phase 4. The main state-1 update runs
  start then sustain/checkpoint work; when both component phases are 4 it
  requests base state 2 through the same deferred current/requested mechanism
  established for TAP.
- Before adjusted start, sustain/checkpoint update returns with phase 0. At or
  after adjusted start, an already-empty checkpoint vector reaches phase 4
  without waiting for adjusted record end; terminal request then depends only
  on the start phase also reaching 4.
- Reaching adjusted record end through an additional runtime offset invokes a
  once-only anonymous grade/feedback path. It does not itself empty the
  checkpoint vector or call the HOLD result wrapper.

## Reasoning

The tracker fields, comparison direction, and post-checkpoint write establish
maximum inactive gap—not held duration—as the classified quantity. Front-only
inspection and one vector removal bound checkpoint emission to one per manager
substep even when the clock has advanced past several records. The two explicit
phase tests establish the terminal condition independently of visual updates.

## Alternatives and falsifiers

- Competing explanation: thresholds classify total held duration or a moving
  average.
- Evidence that would disprove this claim: a tracker update that accumulates
  active time, a loop capable of removing multiple checkpoint records in one
  call, or a terminal-state request while either component phase differs from
  4.

## Unknowns

- Values, units, ordering guarantees, and load source for configuration fields
  `+0x708` and `+0x710` through `+0x71c` remain unavailable.
- The authoritative player-facing names for result bytes and checkpoint
  categories remain open.
- The once-only end-bound helper is closed as a feedback-resource path without
  result submission or phase mutation by
  `claim.note.hold-end-feedback-boundary`.
- Scheduled result aggregation and terminal observer routing are covered by
  `claim.judgement.shared-result-two-stage-routing` and
  `claim.interactions.result-terminal-short-circuit`.
- Behavior for malformed threshold ordering is not normalized beyond the
  literal first-matching-threshold rule above.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/notes/hold.md`, `spec/configuration.md`,
  `spec/judgement.md`.
- Reconstruction code: `HoldGapState`, `update_hold_gap`,
  `classify_hold_gap`, `reset_hold_gap_after_checkpoint`, and
  `hold_gap_result_byte` in `include/chart/reconstruction.hpp`.
- Tests: `tests/hold_sustain_test.cpp`.

## Verification

Initialization, current update, retained classification, checkpoint reset,
front removal, result wrapper, phase assignment, and terminal request were
traced independently. Focused tests cover the strict update gate, endpoint
classification, retained maximum, end clamp, never-active path, and result-byte
mapping.
