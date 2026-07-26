# Claim: three post-active scene states continue full gameplay updates before teardown

- ID: `claim.pipeline.post-active-gameplay-drain`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `state.ownership`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

Leaving structural active state `0x0f` for state `0x10` or `0x11` does not stop
gameplay processing. The registered update callbacks for states `0x10`,
`0x11`, and `0x12` each invoke the complete outer gameplay update with argument
one before testing their own presentation/lifetime transition. Runtime-note
teardown first occurs on entry to state `0x13`; states `0x13` and `0x14` do not
invoke the outer gameplay update.

Consequently notes, input-derived judgement, ordinary result routing, and the
final periodic aggregate reevaluation can remain live for one or more updates
after the active exit gate first requests a state change.

## Anchors

- `game.exe @ RAM:00da25b0, FUN_00da25b0, states 0x0f through 0x11 callback registration`
- `game.exe @ RAM:00da2630, FUN_00da2630, states 0x12 through 0x14 callback registration`
- `game.exe @ RAM:00da3480, FUN_00da3480, active-state update and 0x10/0x11 writes`
- `game.exe @ RAM:00da1ac0 and RAM:00da2d30, state-0x10 entry/update`
- `game.exe @ RAM:00da1b00 and RAM:00da2e20, state-0x11 entry/update`
- `game.exe @ RAM:00da1b60 and RAM:00da2f30, state-0x12 entry/update`
- `game.exe @ RAM:00da1c00 and RAM:00da2ff0, state-0x13 teardown entry/presentation update`
- `game.exe @ RAM:00da1d20 and RAM:00da3140, state-0x14 report entry/final update`
- `game.exe @ RAM:00da9820, FUN_00da9820, common complete outer update`
- `game.exe @ RAM:00b28a00, FUN_00b28a00, two runtime-note vector teardown`
- `game.exe @ RAM:00b2e5b0 and RAM:00b21ea0, input/view-owner teardown`
- `game.exe @ RAM:00b984e0 and RAM:00b949c0, narrow auxiliary result-field clear`
- `game.exe @ RAM:00cdf480, FUN_00cdf480, ordinary state-0x12 release signal`
- `game.exe @ RAM:00cec260 and RAM:00cec200, alternate state-0x12 release predicates`

## Observations

- The callback tables are consecutive entry/update/exit triplets. The triplet
  containing `FUN_00da3480` is state `0x0f`: that update writes `0x10` or
  `0x11`. The next five registered triplets line up with the observed writes
  `0x11`, `0x12`, `0x13`, and `0x14`.
- State-0x10 update `FUN_00da2d30` begins with `FUN_00da9820(1)`, forces a
  report, and only then applies its cue-time and external transition conditions
  for state `0x11`.
- State-0x11 update `FUN_00da2e20` also begins with `FUN_00da9820(1)`. Its two
  scene variants use different external/presentation predicates and structural
  reason codes, but every successful branch writes state `0x12` only after the
  outer update.
- State-0x12 update `FUN_00da2f30` again begins with `FUN_00da9820(1)`. On the
  ordinary controller path it advances when a local bypass is set, or when an
  ordinary wait is inactive and a release signal is present. On the alternate
  path, nested state 3 or the nested ready byte advances. The transition
  cleanup then writes state `0x13`.
- State-0x13 entry `FUN_00da1c00` destroys and empties both runtime-note vectors,
  clears input/view resources, and resets the forced-result selection. Its
  result-owner call clears only two dynamic configured-rule notification masks;
  it does not invoke the full source-result reset or clear aggregate values,
  counts, or latched terminal bytes. State-0x14 later persists the full result
  object as closed by `claim.pipeline.final-result-persistence-order`.
- State-0x13 update `FUN_00da2ff0` is a presentation/external wait that can write
  state `0x14`; it does not call `FUN_00da9820`. State-0x14 entry submits the
  retained report and resets report/cue fields, while its update clears an
  enclosing run flag. Neither callback updates notes or results.
- The registered exit callbacks for these triplets are no-ops or set only an
  enclosing scene byte. They do not add a hidden result reset or note update.

## Reasoning

The callback table order, explicit next-state writes, and repeated direct call
to the same outer update close the state interval over which gameplay remains
live. The first destructive note-vector call is in the entry callback reached
after state `0x12`, not in the active exit callback. Therefore modeling the
`0x10`/`0x11` write as immediate gameplay cessation would drop observable
post-active ticks.

If ordinary terminal routing caused the first exit, its latched flag remains
visible during these ticks and later note results use the already-reconstructed
observer-only route. If alternate nested state 3 caused exit without result
completion or terminal latching, later post-active notes can still submit
authoritative results before state-0x13 teardown. Every such update also retains
the kind-2 aggregate reevaluation.

## Alternatives and falsifiers

- Competing explanation: the post-active callbacks update presentation only,
  or teardown runs immediately when `0x10`/`0x11` is written.
- Evidence that would disprove this claim: a different target for any of the
  three `FUN_00da9820(1)` calls, state-machine entry dispatch that skips the
  registered update callbacks, a note-vector teardown before those calls, or a
  full result reset on the `0x0f`-to-post-active transition.

## Unknowns

- Player-facing names and exact duration semantics for states `0x10` through
  `0x14`, cue-time thresholds, local bypass/wait fields, and external release
  signals remain unassigned.
- Presentation conditions determine how many post-active updates occur. Their
  interfaces and ordering are recovered, but unavailable runtime timing values
  are not guessed.

## Consequences

- Ghidra mutations: none in the live project; temporary thunk/function
  materialization was confined to the static clone because GhidraMCP remained
  unavailable.
- Spec sections: `spec/timing.md`, `spec/judgement.md`.
- Reconstruction code: `post_active_outer_update_required`,
  `PostActiveDrainGateContext`, and
  `post_active_drain_ready_for_teardown`.
- Tests: `tests/gameplay_exit_test.cpp`.

## Verification

All entry/update/exit triplets from active state `0x0f` through final state
`0x14`, every next-state write, the three common outer-update calls, the
state-0x12 ordinary and alternate gates, runtime-note/input/view destruction,
the narrow result-field clear, and later report/final callbacks were inspected.
Focused tests cover the live-state set and both state-0x12 controller gates.
