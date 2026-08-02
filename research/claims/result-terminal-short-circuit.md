# Claim: terminal routing short-circuits aggregation but preserves observer notification

- ID: `claim.interactions.result-terminal-short-circuit`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.miss`, `matching.priority`, `interactions.cross_note`, `state.ownership`
- Last reviewed: 2026-07-22

## Statement

If one authoritative aggregate update sets the result owner's terminal-route
flag, every later dispatchable result while that flag remains set invokes the
stored result observer instead of updating per-result state or the aggregate;
the later note's own finalization still runs. The observer is not a hidden
aggregate: its only installed non-default target advances a capped processed-
event counter.

## Anchors

- `game.exe @ RAM:00b983e0, FUN_00b983e0, active terminal-predicate selection`
- `game.exe @ RAM:00b965f0, FUN_00b965f0, ordinary terminal-route flag read`
- `game.exe @ RAM:00c41520, FUN_00c41520, alternate terminal predicate`
- `game.exe @ RAM:00b97730, FUN_00b97730, callback-versus-submission branch`
- `game.exe @ RAM:00b8e4a0, FUN_00b8e4a0, normal per-result update and observer invocation`
- `game.exe @ RAM:00b8e480, FUN_00b8e480, observer-only invocation`
- `game.exe @ RAM:00b8e320, FUN_00b8e320, observer storage construction`
- `game.exe @ RAM:00b8ec80, FUN_00b8ec80, default no-op callable installation`
- `game.exe @ RAM:00da1f90, FUN_00da1f90, progress observer installation and expected-count initialization`
- `game.exe @ RAM:00da4bf0, FUN_00da4bf0, installed observer target`
- `game.exe @ RAM:00b8f3d0, FUN_00b8f3d0, expected-count sum over progress categories 0..4`
- `game.exe @ RAM:00c3e430, FUN_00c3e430, progress-object expected-count initialization`
- `game.exe @ RAM:00b96b30, FUN_00b96b30, post-aggregate terminal flag write`
- `game.exe @ RAM:00b92640, FUN_00b92640, runtime rule evaluation and terminal summary flags`
- `game.exe @ RAM:00b95f80, FUN_00b95f80, result-owner reset`
- `game.exe @ RAM:00b96700, FUN_00b96700, external terminal-route setter`
- `game.exe @ RAM:00da8a20, FUN_00da8a20, guarded empty-participant setup fallback`
- `game.exe @ RAM:00da8cf0, FUN_00da8cf0, initial and periodic capture producer`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, reset and stable selector ownership`
- `game.exe @ RAM:00b985f0, FUN_00b985f0, selector-2 setup writer`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, submission-before-note-finalization order`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, full active-note update pass`

## Observations

- The ordinary terminal predicate reads result-owner byte `+0x2a8`. A special
  mode instead queries a separate state object, but both predicates feed the
  same branch in the dispatcher.
- When the predicate is true, the dispatcher calls the object stored at
  `+0x334` through its `+0x08` virtual slot and skips result-controller update,
  event construction, aggregate mutation, and the alternate-mode gauge update.
- The same stored callable is invoked at the end of an ordinary valid per-result
  update after the dispatch record maps into progress category 0 through 4.
  Every one of the fourteen fixed dispatch records maps into that range. Its
  default target is a no-op. When the enclosing gameplay controller owns an
  optional progress object, it replaces the default target with
  `FUN_00da4bf0`.
- The installed target increments progress-object field `+0x198`, then clamps
  it to field `+0x194`. Initialization sets `+0x194` to the sum of existing
  result counts for progress categories 0 through 4. The target has no active-
  note, input, aggregate, or terminal-flag writes.
- A successful aggregate update runs `FUN_00b96b30`. Its evaluator returns a
  double and two summary bytes. A configured terminal rule can set only the
  primary byte; otherwise a later end threshold can set both. The ordered
  producers and their reset ownership are closed separately by
  `claim.judgement.ordinary-terminal-summary-producers`.
- Result-owner reset clears both bytes. A distinct setter can also replace the
  first flag, but its only caller is bounded to the selector-2 setup fallback.
  The caller must have an initially writable participant capture, a zero
  selected-participant count, and a still-false selected terminal predicate.
  It also requests a zero/latch update on the alternate meter.
- Gameplay setup resets the result owner and all three selector bytes before
  selecting modes. The only internal selector-2 writer is in that setup and
  writes one. Initial participant capture requests its lock; periodic gameplay
  calls cannot rewrite or reapply the fallback. No selector write occurs before
  the next gameplay setup/reset.
- Selector 2 keeps the alternate predicate selected while the externally set
  ordinary byte is latched. The setter therefore cannot itself change the
  currently selected pre-dispatch terminal predicate in that session. Direct
  controller reads of the enclosing `+0x5f8` byte can still change their data
  source branches; player-facing mode identities remain unassigned.
- The shared note-result handler invokes the dispatcher before applying its
  finalize argument. Its finalization branch is outside the dispatcher and
  therefore still requests the note transition and invokes the terminal hook
  after observer-only routing.
- The active-note manager does not stop its update-vector pass after one result.
  A flag produced by an earlier object is consequently visible to later objects
  in storage order during the same substep.

## Reasoning

The terminal byte is shared result-owner state, not per-note state. Immediate
write/read ordering makes simultaneous or equal-candidate fan-out
order-sensitive at the outcome boundary: later notes can still complete, but
their result events no longer enter the normal authoritative aggregate path.
Resolving the callable separates this from a second scoring or judgement path:
it is bounded progress notification. The configured rule conditions and
end-threshold inputs can remain parameterized without losing their recovered
summary precedence or this dispatch behavior. The independent progress and
aggregate mappings are closed by
`claim.judgement.shared-result-two-stage-routing`.

## Alternatives and falsifiers

- Competing explanation: the observer branch duplicates the normal aggregate
  update internally.
- Evidence that would disprove this claim: another installed callable target
  invoking the skipped aggregate with the same event, a manager break before
  later note updates, or note finalization being nested inside the skipped
  dispatcher branch.

## Unknowns

- The player-facing meaning of terminal summary bytes, configured rule units,
  and end-threshold selector values remain unresolved. Their structural
  producers are closed by
  `claim.judgement.ordinary-terminal-summary-producers`.
- Alternate-mode player-facing identities are intentionally unassigned. The
  externally set ordinary byte is lifetime-bounded: one initial selector-2
  setup path can set it, the setup lock prevents periodic reapplication,
  selectors remain alternate, and the next result-owner setup/reset clears it.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/matching.md`, `spec/judgement.md`.
- Reconstruction code: `SharedResultRoutingState`,
  `produce_ordinary_terminal_summary`, and `dispatch_shared_result` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp`.

## Verification

The terminal predicate, false and true dispatcher branches, aggregate-produced
flag, reset, external setter and its only caller, stable selector ownership,
capture lock, note-finalization tail, manager loop, observer constructor, only
replacement site, concrete callable, and progress-object initialization were
traced separately. Focused tests verify terminal-summary precedence, immediate
same-pass routing, and that the selector-2 setup fallback cannot select or
periodically reapply the ordinary byte while the alternate mode remains active.
