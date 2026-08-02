# Claim: every outer update reevaluates the ordinary aggregate periodically

- ID: `claim.judgement.periodic-aggregate-reevaluation`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.tick_conversion`,
  `judgement.types`, `state.ownership`, `config.external`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

After the current outer update has finished its input/note substeps and
post-note passes, it always submits a synthetic aggregate snapshot of kind 2.
The snapshot retains the ordinary owner's current aggregate fields and adds no
new result-event contribution. It includes the current NotesManager position
converted by single-precision multiplication with `16.666666F`, floor toward
negative infinity, and signed 64-bit conversion.

Kind 2 cannot run the event-only negative-adjustment vector or the configured
terminal-rule vector. The common contribution and promotion vectors remain
eligible, including two kind-2 progress conditions backed by external rule
fields. The common end-threshold producer also remains eligible. An outer
update can therefore change retained aggregate state and latch ordinary
terminal routing even when no note submitted a new result in that update.

## Anchors

- `game.exe @ RAM:00da9820, FUN_00da9820, outer-update ordering and sole periodic caller`
- `game.exe @ RAM:018d3f20, initialized single-precision factor 16.666666F`
- `game.exe @ RAM:00b98680 and RAM:00b968d0, converted-position forwarding and owner setter`
- `game.exe @ RAM:00b97b60, FUN_00b97b60, synthetic event clearing and retained-state copy`
- `game.exe @ RAM:00b96120, FUN_00b96120, kind-2 snapshot construction`
- `game.exe @ RAM:00b96b30, FUN_00b96b30, shared aggregate evaluator and terminal latches`
- `game.exe @ RAM:00b92640, FUN_00b92640, ordered rule-vector and end-threshold evaluation`
- `game.exe @ RAM:00b93180, FUN_00b93180, common contribution and kind-2 progress branches, hash d26d8367132b48ad03a297b58bb0ac2112bfb80c49e7be2259ce8665bfa4445e`
- `game.exe @ RAM:00b94420, FUN_00b94420, one-shot bit test, hash 641c57076a69a5979fecb92570b400e6a62552d629b64137bbd4677645779b28`
- `game.exe @ RAM:00b949e0, FUN_00b949e0, one-shot bit mark, hash 9f3d38aa66db1d298a4615eb49214eb8d06b902762dfea098b0bd8a2f30d9011`
- `game.exe @ RAM:00b92d30, FUN_00b92d30, kind-1-only negative adjustment`
- `game.exe @ RAM:00b92ff0, FUN_00b92ff0, kind-1-only configured terminal rule`
- `game.exe @ RAM:00b922b0, FUN_00b922b0, linked rule-record accumulator update`
- `game.exe @ RAM:00b94ac0, FUN_00b94ac0, external rule-vector population`
- `game.exe @ RAM:00b95f80, FUN_00b95f80, aggregate-owner reset`

## Observations

- The thunk for `FUN_00b97b60` has one caller, the outer gameplay update. Its
  call is unconditional and is the final substantial operation in that update.
- Immediately beforehand, the outer update reads the NotesManager's current
  floating position. It multiplies by the initialized `16.666666F` factor,
  implements a single-precision floor operation, converts the result to a
  signed 64-bit integer, and stores the same value in the scene and aggregate
  owner. The factor is deliberately kept distinct from the executable's
  forward `0.06F` chart-unit scale; the two floats are not exact inverses.
- `FUN_00b97b60` clears its temporary result records and copies retained owner
  fields into `FUN_00b96120`. That builder initializes the common snapshot,
  writes kind 2, copies the retained computed value, aggregate metric, timing,
  and record arrays, and leaves the per-event contribution at zero.
- The resulting snapshot enters the same `FUN_00b96b30` and `FUN_00b92640`
  chain used by a valid kind-1 result snapshot. The output replaces the owner's
  retained computed value and nonzero summary bytes latch the existing primary
  and secondary terminal flags.
- The first rule vector is externally populated by the type-0 load case. Its
  evaluator admits kind 2. While the evaluator's runtime-active byte is set,
  external positive fields at `+0x68` and `+0x6c` enable two additional
  progress checks: one compares bucketed signed-64 time values and the other
  advances a bounded percentage-like bucket derived from two snapshot fields.
  Exact field meanings and configured values are unavailable.
- A matching first-vector record either contributes to the current aggregate
  computation or, for one configured subtype, accumulates the value into a
  linked rule record through `FUN_00b922b0`. A source flag instead selects a
  deterministic one-shot gate: test the unit's source-order index in a retained
  bit vector, attempt to mark it, and reject only when the valid bit was already
  set. An out-of-range bit reads clear and cannot be marked, so malformed state
  remains repeatable. No random or probability predicate participates.
- The second-vector evaluator can inspect common predicates for kind 2, but
  every path that returns a negative adjustment requires kind 1. The
  fourth-vector configured-terminal evaluator rejects every kind other than 1
  at entry. The third promotion vector has no kind restriction and can raise
  the computed value to an externally supplied record value after its separate
  gates pass.
- The final track-skip/end-threshold branch does not restrict snapshot kind.
  Its existing runtime gates and aggregate comparison therefore run for kind 2
  after the other eligible vectors.
- Aggregate-owner reset clears the retained computed/timing fields and both
  terminal bytes. Rule-vector construction/reset owns the persistent
  per-record progress fields used across periodic evaluations.

## Reasoning

The unique caller, explicit kind tag, common evaluator, and kind checks close
the periodic entry-to-terminal path without assigning semantics to unavailable
rule data. Because the call occurs after the active-note pass, a terminal latch
cannot reroute a result already processed in that pass. It is visible to a
subsequent result dispatch and to the enclosing gameplay-state exit predicate.

## Alternatives and falsifiers

- Competing explanation: the final call merely snapshots telemetry for later
  display and cannot affect judgement outcome.
- Evidence that would disprove this claim: a separate nonauthoritative result
  owner at the final call, a kind-2 guard before common rule evaluation or
  terminal latching, or an earlier outer-update reset that discards all output
  before any terminal consumer can observe it.

## Unknowns

- Player-facing names, units, and values for the first and third rule vectors
  remain external. Their evaluated contribution/promotion is parameterized.
- The semantic names of the retained kind-2 snapshot fields are not assigned
  beyond the statically closed converted-position field.
- There is no runtime random/probability source on this evaluator path; the
  earlier interpretation is superseded by the exact bit-test/bit-mark trace.
- Exceptional conversion behavior for non-finite or out-of-range manager
  positions is outside the recovered valid gameplay domain.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Spec sections: `spec/judgement.md`, `spec/timing.md`.
- Reconstruction code: `OrdinaryAggregateSnapshotKind`,
  `periodic_aggregate_position_tick`, rule-kind predicates,
  `ordinary_rule_one_shot_allows`, and `apply_ordinary_periodic_aggregate` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp` covers conversion precision, kind
  eligibility, contribution/promotion order, and periodic end termination.

## Verification

The outer caller, position conversion, both owner setters, synthetic builder,
common evaluator, four vector families, linked-record sink, one-shot bit
owner, external vector loader, terminal latch, and reset were inspected
independently. Thunk-xref enumeration confirmed that the synthetic builder
chain is owned only by the outer update. Focused tests cover first use, repeat
rejection, word boundaries, disabled gating, and malformed out-of-range
repeatability.
