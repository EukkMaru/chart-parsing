# Claim: shared results are category-mapped before aggregate submission

- ID: `claim.judgement.shared-result-routing`
- State: superseded
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `judgement.miss`, `note.tap`, `note.hold`, `interactions.cross_note`
- Last reviewed: 2026-07-22

Superseded by `claim.judgement.shared-result-two-stage-routing`. This claim
mistook the first source-to-dispatch category for the category consumed by the
authoritative aggregate. `FUN_00b97730` applies two additional fixed mappings;
in particular, dispatch category 11 becomes aggregate category 8 rather than
being excluded.

## Statement

The shared note-result path first maps its source category through a fixed
19-case table. A mapped category from 0 through 13 reaches the result dispatcher
when no terminal route is active, but only mapped categories 0 through 8 with a
result byte below the runtime result-type count update the authoritative event
count and accumulated outcome value.

## Anchors

- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared result ordering and source-category validation`
- `game.exe @ RAM:00d7f030, FUN_00d7f030, source-to-result category map`
- `game.exe @ RAM:010e9200, FUN_010e9200, runtime source-category record count`
- `game.exe @ RAM:00b97730, FUN_00b97730, mapped-category dispatch gate`
- `game.exe @ RAM:00b95870, FUN_00b95870, aggregate validation and mutation`
- `game.exe @ RAM:0108e060, FUN_0108e060, runtime result-type record count`
- `game.exe @ RAM:00b95cb0, FUN_00b95cb0, parameterized contribution selection`

## Observations

- The shared handler validates the source category against a runtime vector of
  `0x44`-byte records, converts it with `FUN_00d7f030`, and submits the mapped
  value before processing its optional note-finalization argument.
- The fixed mapping is: `0->0`, `1->4`, `2..3->1`, `4..6->2`, `7..9->3`,
  `10..11->5`, `12..13->7`, `14..16->6`, `17->11`, and `18->7`; other inputs
  map to -1.
- The dispatcher accepts mapped values 0 through 13. When its pre-dispatch
  terminal predicate is false, it constructs an event and forwards it to the
  aggregate owner.
- A normal valid per-result update for mapped categories 0 through 4 invokes a
  stored observer before aggregate submission. Mapped categories 5 through 8
  can still update the authoritative aggregate without that normal observer
  invocation. If terminal routing is already active, every dispatchable mapped
  category takes the observer-only route before either validation layer.
- The aggregate owner independently requires mapped category 0 through 8 and a
  result byte below the count of runtime `0x18`-byte result records. On success
  it saturating-increments a 32-bit event count, computes a contribution through
  runtime tables/rules, updates an accumulated double, and retains a copy of the
  event.
- Lane feedback, effect creation, and media-resource calls are separate sibling
  branches in the shared handler and do not supply the aggregate mutation.

## Reasoning

The two validation layers distinguish dispatchable result events from events
that alter the authoritative aggregate. The detailed result byte produced by a
note remains data at the final aggregate validation boundary; it is not reduced
to a presentation-only effect before that point. The fixed category conversion
and route predicates can therefore be reconstructed without guessing the
runtime contribution values.

## Alternatives and falsifiers

- Competing explanation: a sibling feedback path, rather than `FUN_00b95870`,
  owns the outcome count and accumulated value.
- Evidence that would disprove this claim: a write to those aggregate fields
  before category/result validation, a category bypass around the mapped gate,
  or a different caller-supplied category at the `FUN_00b97730` boundary.

## Unknowns

- Runtime source-category and result-type table contents and contribution
  constants are externally loaded and remain parameters.
- Player-facing names for source categories, mapped categories, result bytes,
  and the accumulated double are not established here.
- Skill/rule evaluation downstream of the aggregate is bounded separately by
  `claim.interactions.result-terminal-short-circuit`.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/judgement.md`, `spec/matching.md`, `spec/notes/tap.md`,
  `spec/notes/hold.md`.
- Reconstruction code: `map_shared_result_category`, `route_shared_result`,
  and `saturating_result_count_increment` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp`.

## Verification

The source-category converter, both runtime-count producers, dispatcher gate,
aggregate gate, mutation order, reset owner, and shared-handler call order were
audited independently. Focused tests cover every fixed mapping case, both
validation layers, and 32-bit count saturation.
