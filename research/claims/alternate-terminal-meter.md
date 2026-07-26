# Claim: selected modes use a post-result terminal meter

- ID: `claim.judgement.alternate-terminal-meter`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `judgement.miss`, `interactions.cross_note`, `state.ownership`
- Last reviewed: 2026-07-21

## Statement

When the bitwise OR of three controller selector bytes is exactly one, result
dispatch uses an alternate terminal meter instead of the ordinary aggregate
terminal flag. A non-terminal result completes its normal per-result and
aggregate work first, then an anonymous result code selects an externally
configured signed meter delta. Reaching the meter's terminal condition affects
later dispatches, not the event that caused the transition.

## Anchors

- `game.exe @ RAM:00b983e0, FUN_00b983e0, terminal-predicate selector`
- `game.exe @ RAM:00b97730, FUN_00b97730, pre-dispatch predicate and post-aggregate meter update`
- `game.exe @ RAM:00c41520, FUN_00c41520, alternate terminal predicate`
- `game.exe @ RAM:00c41880, FUN_00c41880, result-code-to-delta selection`
- `game.exe @ RAM:00c41700, FUN_00c41700, signed delta guards, clamp, and zero latch`
- `game.exe @ RAM:00c41610, FUN_00c41610, external meter configuration load`
- `game.exe @ RAM:00c41240, FUN_00c41240, zero initialization`
- `game.exe @ RAM:00b98770, FUN_00b98770, meter configuration wrapper`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, gameplay setup consumers`
- `game.exe @ RAM:00da8cf0, FUN_00da8cf0, bounded participant selection and count derivation`
- `game.exe @ RAM:00da8a20, FUN_00da8a20, guarded auxiliary capture and meter adjustment`
- `game.exe @ RAM:00da7b30, FUN_00da7b30, initial locked setup call`
- `game.exe @ RAM:00da9820, FUN_00da9820, later unlocked-write attempt`
- `game.exe @ RAM:00ce14b0, FUN_00ce14b0, count/limit presentation consumer`
- `game.exe @ RAM:00b96700, FUN_00b96700, ordinary-byte setup fallback write`
- `game.exe @ RAM:00b97ad0, FUN_00b97ad0, selector and result-owner reset`
- `game.exe @ RAM:00b985f0, FUN_00b985f0, selector-2 setup writer`

## Observations

- The controller selects this predicate when
  `(selector_0 | selector_1 | selector_2) == 1`; otherwise it reads the
  ordinary result-owner terminal flag.
- In the ordinary alternate-meter form, terminal means maximum `+0x10` is
  nonzero and current `+0x04` is zero. If mode byte `+0x01` is nonzero, the
  predicate instead tests whether auxiliary count `+0x48` is zero.
- Setup obtains the maximum, an automatic adjustment, four result deltas, and
  secondary-meter parameters from a runtime configuration record. Their values
  are not embedded in this executable and remain parameters.
- After ordinary event submission, result codes 0, 1, and 2 select signed
  deltas at `+0x18`, `+0x1c`, and `+0x20`; codes 3 and 4 share `+0x24`; other
  codes select zero.
- A selected positive delta drains current and a negative delta restores it:
  current becomes `clamp(current - delta, 0, maximum)`. Zero is latched in byte
  `+0x08`. A zero delta is ignored; restoration at maximum is ignored; and the
  auxiliary form rejects further deltas after the zero latch is set.
- The result dispatcher tests terminal state before event construction. Its
  meter update is after per-result state and authoritative aggregate submission,
  so a transition caused by one result is visible to the next dispatch in the
  same active-note manager pass.
- This meter family writes its own controller subobject and presentation/audio
  feedback state. The traced predicate and delta paths do not write active-note
  objects or input snapshots.
- Gameplay setup derives `+0x48` from a bounded prefix of runtime participant
  records. Setup modes 1, 2, and 4 admit only the local-identity record; other
  modes also admit flagged non-local records. The local record is counted unless
  the meter's zero latch is already set. A selected non-local record is counted
  only when its effective current contribution is nonzero.
- The same guarded initializer stores the participant limit at `+0x4c` and the
  current/maximum contribution sums at `+0x50/+0x54`. Byte `+0x40` guards all
  four writes. Initial gameplay setup calls the initializer with the lock
  request set; a later periodic path calls it without requesting a lock but
  cannot overwrite a locked capture.
- Construction and external configuration loading clear the guard and all four
  auxiliary integers. A complete direct-offset instruction scan, getter-caller
  audit, and meter-method audit found no result-time decrement of `+0x48`.
  Its gameplay-visible transition is therefore capture/reset, not per-result
  consumption in this executable.
- Selector 2 has one additional initial-setup fallback. After a successful
  writable capture, zero selected participants and a still-false selected
  terminal predicate cause the controller to request meter current zero (and
  latch zero when current was nonzero) and set the ordinary result-owner byte.
  The initial call has already requested the capture lock, so later periodic
  calls cannot repeat this branch.
- Gameplay setup is the only internal selector-2 writer and reset clears all
  selectors. Selector 2 remains one until the next setup/reset, so terminal
  dispatch continues selecting this meter rather than the simultaneously set
  ordinary byte. The ordinary write can affect direct controller data-source
  branches, but it is not a second selected terminal predicate in this mode.

## Reasoning

The predicate selector, update ordering, fixed result-code fan-out, meter clamp,
and setup/reset ownership form a closed alternate outcome boundary even though
player-facing mode names and configuration values are unavailable. The
auxiliary value is a setup-derived participant count, but the exact product
term for those participants remains intentionally unnamed.

## Alternatives and falsifiers

- Competing explanation: the alternate predicate is only visual and ordinary
  result aggregation continues after it becomes true.
- Evidence that would disprove this claim: a dispatcher path that ignores the
  selected predicate, a meter update before the current event's aggregate, or
  an active-note/input write in the predicate or delta consumer.

## Unknowns

- Player-facing identities of the three selector bytes, the meter mode byte,
  result codes, and configured delta units remain unresolved.
- Player-facing identities of setup modes 1, 2, and 4 and of the participant
  record flags/contributions remain unresolved.
- The automatic and secondary-meter fields are outside this claim except where
  they demonstrate external configuration ownership.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/judgement.md`, `spec/matching.md`.
- Reconstruction code: `AlternateTerminalMeter`, selected-predicate and
  empty-participant fallback helpers in `include/chart/reconstruction.hpp`.
- Tests: `tests/alternate_meter_test.cpp`.

## Verification

The selector, both predicate forms, result-dispatch order, complete jump table,
delta application, configuration initialization, zero initialization, setup
callers, all direct `+0x48` accesses, and every caller of both meter getters were
inspected independently. The selector-2 writer/caller set, ordinary-byte
setter, initial/periodic capture calls, and reset were also traced. Focused
tests cover selector semantics, every result-code arm, signed drain/restore,
clamping, zero latching, disabled maximum, participant selection, bounded
counting, capture locking, the one-shot empty-participant fallback, stable
selected-predicate behavior, and the auxiliary predicate/delta lockout.
