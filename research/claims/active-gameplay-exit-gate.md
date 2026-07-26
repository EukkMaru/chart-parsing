# Claim: active gameplay exits only after a controller-specific post-update gate

- ID: `claim.pipeline.active-gameplay-exit-gate`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `state.ownership`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

The registered active-gameplay callback completes the full outer gameplay
update and an ordinary report attempt before checking for scene exit. On the
ordinary controller path, either result-count completion or the selected
terminal predicate requests exit. On the alternate controller path, nested
state 3 requests exit immediately; otherwise both result-count completion and
an independent nested ready byte are required. The ordinary selected-terminal
predicate is not an independent alternate-path exit condition.

The accepted gate leaves structural active state `0x0f`; it does not by itself
stop outer gameplay updates. States `0x10` through `0x12` continue them as
closed by `claim.pipeline.post-active-gameplay-drain`.

Result-count completion is enabled by setup only when the five expected
progress-category counts have a nonzero 32-bit total. Once enabled, completion
uses the unsigned inclusive comparison `expected_total <= processed_total`.

## Anchors

- `game.exe @ RAM:00da25b0, FUN_00da25b0, active callback registration`
- `game.exe @ RAM:00da3480, FUN_00da3480, update, report, exit gate, and transition`
- `game.exe @ RAM:00da9820, FUN_00da9820, full outer gameplay update`
- `game.exe @ RAM:00da94c0, FUN_00da94c0, interval-gated and forced-final report producer`
- `game.exe @ RAM:00b00be0, FUN_00b00be0, ordinary/alternate controller selector`
- `game.exe @ RAM:00b98370, FUN_00b98370, ordinary completion-or-terminal predicate`
- `game.exe @ RAM:00b98330, FUN_00b98330, result-count completion wrapper`
- `game.exe @ RAM:00b8fab0, FUN_00b8fab0, enabled inclusive total comparison`
- `game.exe @ RAM:00b8f300, FUN_00b8f300, processed-result total`
- `game.exe @ RAM:00cec260, FUN_00cec260, nested-state-3 predicate`
- `game.exe @ RAM:00cec200, FUN_00cec200, nested ready-byte accessor`
- `game.exe @ RAM:00b97ad0, FUN_00b97ad0, result-owner reset`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, fourteen-count setup loop`
- `game.exe @ RAM:00b97990, FUN_00b97990, dispatch-count distribution`
- `game.exe @ RAM:00b987b0 and RAM:00b8ffc0, setup finalization and enable gate`
- `game.exe @ RAM:005f5b10 and RAM:00af8990, transition timestamp source and owner`

## Observations

- `FUN_00da25b0` stores the thunk for `FUN_00da3480` in the active callback
  slot. The callback first invokes `FUN_00da9820` with one, then invokes
  `FUN_00da94c0` with zero, before reading any exit predicate.
- The outer update ends with the kind-2 aggregate reevaluation closed by
  `claim.judgement.periodic-aggregate-reevaluation`. A terminal byte latched by
  that reevaluation is therefore visible to the ordinary exit gate in the same
  callback.
- When the controller selector is clear, `FUN_00b98370` first checks
  result-count completion and, if incomplete, checks the currently selected
  ordinary/alternate terminal source. Either true result returns true.
- When the selector is set, the callback does not call `FUN_00b98370`. It first
  accepts `FUN_00cec260`, whose low byte is exactly the comparison of a nested
  state field with 3. If false, it requires both `FUN_00b98330` completion and
  the low ready byte returned by `FUN_00cec200`.
- Gameplay setup first calls `FUN_00b97ad0`, which clears the result owner's
  completion enable and per-category counts. It then iterates exactly fourteen
  dispatch-category counts through `FUN_00b97990` before finalizing through
  `FUN_00b987b0` and `FUN_00b8ffc0`.
- The setup map distributes every dispatch count into one of five progress
  categories. `FUN_00b8ffc0` writes enable one only if their 32-bit sum is
  nonzero. Because reset immediately precedes this setup, a zero total leaves
  completion disabled.
- While enabled, `FUN_00b8fab0` sums the five expected counts and compares that
  unsigned total inclusively with `FUN_00b8f300`, which sums all runtime-valid
  result-byte counts in those five progress categories.
- After any accepted exit gate, the callback records an epoch-seconds
  transition timestamp. A scene-owned selector byte then chooses structural
  state code `0x10`, or code `0x11` plus a forced-final report call. An optional
  instrumentation write occurs before the timestamp but does not feed any exit
  branch.

## Reasoning

Registration closes the callback identity, and the callback's straight-line
call order places all current gameplay mutations before the gate. The ordinary
wrapper and alternate branch have disjoint predicate structures, so combining
them into one terminal disjunction would incorrectly allow ordinary terminal
state to end the alternate path. Reset, all fourteen setup inputs, both fixed
maps, the enable writer, and the processed-total reader close result-count
completion without assigning unknown player-facing category names.

## Alternatives and falsifiers

- Competing explanation: the alternate controller also exits when the ordinary
  selected-terminal predicate is set, or the completion check is strict.
- Evidence that would disprove this claim: an alternate branch call to the
  combined terminal wrapper, a comparison requiring processed greater than
  expected, another writer enabling zero-count setup, or an exit check before
  the final kind-2 aggregate evaluation.

## Unknowns

- Player-facing identities of the controller selector, nested state 3, nested
  ready byte, scene selector, and state codes `0x10`/`0x11` remain unassigned.
- The report payload's presentation meaning is outside judgement scope; only
  its ordering and forced-final call are reconstructed here.
- The duration and gameplay effects after this gate are closed separately by
  `claim.pipeline.post-active-gameplay-drain`.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and the function/callback
  audit used the temporary static project clone.
- Spec sections: `spec/timing.md`, `spec/judgement.md`.
- Reconstruction code: `result_count_completion_enabled_for_setup`,
  `result_count_completion_reached`, `ActiveGameplayExitContext`, and
  `decide_active_gameplay_exit`.
- Tests: `tests/gameplay_exit_test.cpp`.

## Verification

The registered callback, both controller branches, nested accessors, ordinary
combined predicate, completion reset/setup/enable/read chain, transition time
source, both state-code writes, and report call order were inspected
independently. Focused tests cover equality, disabled zero-count setup, both
ordinary exit sources, the alternate conjunction and state-3 bypass, and both
structural transition variants.
