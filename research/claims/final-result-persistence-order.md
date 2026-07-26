# Claim: final result persistence follows the last gameplay update without resetting outcome state

- ID: `claim.pipeline.final-result-persistence-order`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `state.ownership`,
  `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

After the last state-`0x12` outer gameplay update, state-`0x13` teardown does
not reset the current result outcome. It clears only two dynamic configured-rule
notification masks and destroys runtime-note/input/view state. State-`0x14`
entry then resets the selected destination record and deep-copies the complete
current `0x878`-byte result object into that retained record before later
report/cue cleanup.

## Anchors

- `game.exe @ RAM:00da2f30, FUN_00da2f30, last possible outer gameplay update`
- `game.exe @ RAM:00da1c00, FUN_00da1c00, state-0x13 teardown entry`
- `game.exe @ RAM:00b984e0, FUN_00b984e0, current-result subobject adjustment`
- `game.exe @ RAM:00b966d0, FUN_00b966d0, configured-rule owner adjustment`
- `game.exe @ RAM:00b949c0, FUN_00b949c0, two notification-mask clears`
- `game.exe @ RAM:00b952d0, FUN_00b952d0, sole rule-mask consumer after evaluation`
- `game.exe @ RAM:00b92640, FUN_00b92640, evaluator then notification dispatch order`
- `game.exe @ RAM:00da2ff0, FUN_00da2ff0, state-0x13 presentation-only update`
- `game.exe @ RAM:00da1d20, FUN_00da1d20, state-0x14 persistence/report entry`
- `game.exe @ RAM:00bd7ef0, FUN_00bd7ef0, destination selection/reset/copy`
- `game.exe @ RAM:00bd7d90, FUN_00bd7d90, retained result-vector growth`
- `game.exe @ RAM:00b97ad0, FUN_00b97ad0, destination-record full reset`
- `game.exe @ RAM:00bd6be0, FUN_00bd6be0, complete result-object assignment`
- `game.exe @ RAM:00b97290 and RAM:00bd6ed0, nested controller assignments`

## Observations

- The state-`0x12` update performs `FUN_00da9820(1)` before it can request state
  `0x13`. No later callback invokes the outer gameplay update.
- In state-`0x13` entry, `FUN_00b984e0` adjusts the current result object by
  `+0x350`; `FUN_00b966d0` then adjusts that owner by `+0x2ac`; and
  `FUN_00b949c0` clears only dwords `+0xe8` and `+0xec`. Relative to the complete
  result object, the only writes are `+0x6e4` and `+0x6e8`.
- Those dwords are dynamic bit masks for the eleven configured-rule records.
  Their only closed consumer is `FUN_00b952d0`, called at the end of aggregate
  evaluation to issue per-rule notification states. It tests the masks after
  the evaluator's rule vectors and has no caller after the final outer update.
- State-`0x13` teardown does not call the full current-result reset
  `FUN_00b97ad0` or the ordinary aggregate reset `FUN_00b95f80`. Aggregate
  values/counts, terminal bytes, progress counts, alternate controller state,
  and the remaining result object therefore survive.
- State-`0x14` entry resolves the selected retained-result vector and source
  current-result pointer, then calls `FUN_00bd7ef0`. Register-level inspection
  shows `FUN_00b97ad0` is invoked on the indexed destination entry, not on the
  source. `FUN_00bd6be0` then receives the unchanged current-result pointer as
  its copy source.
- The assignment copies the complete result object's scalar fields and invokes
  deep assignments for its large per-result owner, ordinary aggregate owner,
  and alternate controller before copying the final selector bytes. It is not
  a summary limited to pre-drain reporting fields.

## Reasoning

Pointer adjustments locate the only pre-persistence clears outside the core
outcome fields, and the mask consumer is lifetime-bounded to aggregate
evaluation, which has already ended. Destination register identity at
`FUN_00bd7ef0` distinguishes initialization of the retained slot from reset of
the current source. The subsequent full assignment preserves any result or
periodic aggregate mutation from the last post-active update.

## Alternatives and falsifiers

- Competing explanation: state-`0x13` resets the source outcome before the
  final retained-result copy, or the copy uses an earlier report snapshot.
- Evidence that would disprove this claim: source-object identity in the
  `FUN_00b97ad0` reset call, a core aggregate/terminal clear in state `0x13`, a
  later rule-mask gameplay consumer, or a copy source other than the current
  result object passed by state-`0x14` entry.

## Unknowns

- Player-facing identities of the retained result-vector index and the eleven
  configured-rule notification states remain unassigned.
- Downstream account/scoring consumers of the retained record are outside this
  project's gameplay-generation and judgement scope.

## Consequences

- Ghidra mutations: none in the live project; the register and subobject audit
  used the temporary static clone because GhidraMCP remained unavailable.
- Spec sections: `spec/timing.md`, `spec/judgement.md`.
- Reconstruction code: none; this is an ownership/persistence boundary after
  the last reconstructed gameplay update.
- Tests: none; verified by pointer-adjustment, sole-consumer, reset-target, and
  deep-copy closure.

## Verification

The final update, all state-`0x13` current-result writes, both pointer-adjusting
wrappers, all rule-mask consumers, absence of later aggregate evaluation,
state-`0x14` source/destination registers, destination growth/reset, and every
nested assignment in the `0x878`-byte result copy were inspected independently.
