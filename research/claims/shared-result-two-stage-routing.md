# Claim: shared results use separate dispatch, progress, and aggregate categories

- ID: `claim.judgement.shared-result-two-stage-routing`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `judgement.miss`,
  `note.tap`, `note.hold`, `note.other_variants`, `state.ownership`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

The shared result path uses three category domains. A 19-entry source mapping
first selects dispatch category 0 through 13. The sole dispatcher then maps
that value independently to a progress category 0 through 4 and an aggregate
category 0 through 8. Every source category that reaches dispatch therefore
reaches the authoritative aggregate when its result byte is runtime-valid.

The fixed dispatch-to-progress map for categories 0 through 13 is:

`[0, 1, 2, 3, 0, 3, 4, 3, 3, 3, 3, 0, 3, 3]`

The fixed dispatch-to-aggregate map is:

`[0, 1, 2, 3, 4, 5, 6, 7, 5, 7, 7, 8, 7, 7]`

## Anchors

- `game.exe @ RAM:00c1c340, FUN_00c1c340, sole source-map and dispatcher caller`
- `game.exe @ RAM:00d7f030, FUN_00d7f030, 19-entry source-to-dispatch map`
- `game.exe @ RAM:00b97730, FUN_00b97730, terminal gate and both second-stage mappings`
- `game.exe @ RAM:011cc170, FUN_011cc170, dispatch-to-progress lookup`
- `game.exe @ RAM:011cc130, FUN_011cc130, dispatch-to-aggregate lookup`
- `game.exe @ RAM:01be5b1c, initialized fourteen-record mapping table`
- `game.exe @ RAM:00b8e4a0, FUN_00b8e4a0, progress count and observer update`
- `game.exe @ RAM:00b95870, FUN_00b95870, aggregate validation and mutation`
- `game.exe @ RAM:0108e060, FUN_0108e060, runtime result-byte count`
- `game.exe @ RAM:00b97990, FUN_00b97990, expected-count setup using the same mappings`

## Observations

- `FUN_00c1c340` is the only caller of the result dispatcher and the only caller
  of the 19-entry map. It validates the original source category, maps it, and
  passes the resulting dispatch category by address.
- The dispatcher requires that value to be in `[0, 14)`. Before any second-stage
  mapping, an active selected terminal predicate takes the observer-only path
  and skips ordinary result and aggregate mutation.
- On the normal path, the dispatcher calls `FUN_011cc170` with the original
  dispatch-category pointer and sends its output to `FUN_00b8e4a0`. It
  separately calls `FUN_011cc130` with that same original pointer and embeds
  its output in the event passed to `FUN_00b95870`.
- Register-level inspection confirms that the second lookup does not consume
  the first lookup's output. The two functions index distinct fields in the
  same initialized `0x14`-byte record table.
- All fourteen progress outputs are in `[0, 5)`. With a runtime-valid result
  byte, `FUN_00b8e4a0` increments the corresponding count and invokes the
  installed observer. Thus every normally valid dispatch category, not merely
  dispatch categories 0 through 4, participates in completion progress.
- All fourteen aggregate outputs are in `[0, 9)`. With a runtime-valid result
  byte, `FUN_00b95870` therefore accepts every dispatch category and performs
  its saturating event-count, contribution, retained-event, and terminal-rule
  work.
- Source category 17 follows the first-stage path to dispatch category 11,
  then progress category 0 and aggregate category 8. This directly contradicts
  the superseded interpretation that category 11 bypasses aggregation.
- Gameplay setup calls `FUN_00b97990` once for each of the fourteen configured
  dispatch-category counts. It distributes each count through the same
  progress map. Summing the five expected buckets consequently recovers the
  total of all fourteen inputs independent of their distribution.

## Reasoning

The sole-caller chain, call-register identities, fixed table contents, and
independent downstream validators establish three domains rather than a single
mapped category. Because each second-stage range fits its consumer, category
alone cannot produce the prior `dispatched_without_aggregate` route; only an
invalid result byte does so on this closed caller path.

## Alternatives and falsifiers

- Competing explanation: `FUN_011cc130` remaps the progress category produced
  by `FUN_011cc170`, making aggregate eligibility conditional on the first map.
- Evidence that would disprove this claim: a call sequence loading the first
  mapping result into the second lookup's `this` register, a table writer that
  changes the initialized values before gameplay, or another dispatcher caller
  that bypasses the source-to-dispatch path.

## Unknowns

- Player-facing names for the three category domains remain unassigned.
- Runtime result-byte records and contribution values remain external.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Supersedes: `claim.judgement.shared-result-routing` and the downstream
  category conclusion in `claim.note.mine-contact-judgement`.
- Spec sections: `spec/judgement.md`, `spec/notes/mine.md`,
  `spec/matching.md`.
- Reconstruction code: `map_shared_result_category`,
  `map_dispatch_progress_category`, `map_dispatch_aggregate_category`,
  `route_shared_result`, and `dispatch_shared_result`.
- Tests: `tests/shared_result_test.cpp`, `tests/mine_contact_test.cpp`.

## Verification

The initialized table was derived independently from both lookup functions;
the caller's registers were checked at each lookup; both downstream range
checks and the sole dispatcher caller were enumerated. Focused tests cover all
three maps and source-category-17 aggregation.
