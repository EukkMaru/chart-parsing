# Claim: STP, SFL, SLP, and DCM schedules feed far-path materialization

- ID: `claim.timing.projection-schedule-materialization`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.discovery`, `parser.events`, `timing.clock`, `state.ownership`, `interactions.cross_note`, `config.external`
- Last reviewed: 2026-07-27

## Statement

Group-1 `STP`, `SFL`, and `SLP` records form resettable keyed interval
schedules that integrate target milliseconds, while `DCM` forms a separate
source-order factor schedule. Both results feed the pending-note far-path
materialization predicate after its strict raw-delta shortcut. Registered
`SFE` has no handler case; `CLK` is stored separately and has no recovered
gameplay consumer.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, group-1 descriptors, hash 75169f04d17368b9a9e109d2139f11160db1abd0f64796fc9d59d9d467b1ec37`
- `game.exe @ RAM:011c7d30, FUN_011c7d30, group-1 dispatch, hash a119eeac0e336b51c96b7d50b15096b3aa5d66f350db75040bc450fbc2792b25`
- `game.exe @ RAM:011ca300, FUN_011ca300, keyed-map insertion, hash 1f92893f134def8f2583166013c6741b0c5a530a7c7f0557c69eb145797d43e5`
- `game.exe @ RAM:011cab90, FUN_011cab90, DCM append, hash e4b7f6f8085451ef945b76d6b182e9534ee2c1a05845c62642077022e8e4ba1a`
- `game.exe @ RAM:011c3c60, FUN_011c3c60, keyed-interval start comparator, hash 5f574ab1fc0bbcc02a71944c01fc7b4fbafecaf00967a65c9446fc61b30af6c9`
- `game.exe @ RAM:011c1460, FUN_011c1460, per-key finalization`
- `game.exe @ RAM:011af4f0, FUN_011af4f0, compiled 32-byte-record MSVC three-way introsort`
- `game.exe @ RAM:011c6720, FUN_011c6720, keyed integration, hash b2799e1cd7b3106c6e20599a1e99eed1465844c510ca38b9c565642acd38bd75`
- `game.exe @ RAM:011c58f0, FUN_011c58f0, DCM factor lookup, hash a2647cdb25acb8e9cd54de8a356ef4b0e22d1d5b50e36e3592385fc0ea5c5ccd`
- `game.exe @ RAM:011c7040, FUN_011c7040, reset entry, hash 85e469f01af6c35d128516dd25bdab6d72eabe497b619db19768a06671a8304c`
- `game.exe @ RAM:011c3850, FUN_011c3850, container clears, hash cae07a08b8c01fc9f13a1881b624673fd479f02c29b80f6d30c8104a6696449b`
- `game.exe @ RAM:00b29c90, FUN_00b29c90, materialization consumer, hash 88a81914b32218d202582c3aa46579ad51e4dec0f6a32da750c57cce674a7584`

## Observations

- Descriptor IDs and argument types are: STP `0x0f` with three integers, SFL
  `0x10` with three integers and a float, SFE `0x11` with the same shape, SLP
  `0x12` with three integers, a float, and an integer key, DCM `0x13` with
  three integers and a float, and CLK `0x14` with two integers.
- The group-1 handler has cases for STP, SFL, SLP, DCM, and CLK but no SFE
  case. The ordinary parser call enables the implemented cases. SFE is
  recognized at descriptor lookup and then rejected without accessing fields.
- Interval records are 32 bytes: normalized start position and scheduled
  milliseconds, accepted-record source sequence, factor, then normalized end
  position and scheduled milliseconds. The authored duration is added to the
  minor field with wrapped signed 32-bit arithmetic. STP stores factor zero and
  key zero; SFL stores its float and key zero; SLP stores its float and integer
  key; DCM stores its float in the unkeyed vector.
- Missing accessed fields use the shared zero defaults. Numeric prefixes are
  accepted, conversion/range failures escape, and extra fields are ignored.
- The keyed map and DCM vector are cleared by chart reset. Postprocessing sorts
  each keyed vector by `left_start + 1/192 < right_start` using the snapshot's
  compiled 32-byte-record MSVC insertion/three-way-partition/heap introsort.
  This closes deterministic equivalent and unordered-NaN behavior without
  relying on host `std::sort`. The DCM vector is not sorted.
- Forward keyed adjustment starts with the target value. For every sorted
  interval starting before target and ending after `from`, it adds
  `(min(target,end) - max(from,start)) * (factor - 1)` when the overlap is
  nonnegative. Overlapping intervals all contribute. A missing key or
  backwards query returns target unchanged on the materialization mode.
- DCM lookup scans in source order at `query + 1.0F`, stops at the first future
  start, returns the first nonzero factor whose end is strictly later than the
  shifted query, and otherwise returns `1.0F`.
- Materialization converts manager and target chart units with the separately
  initialized `16.666666F`, applies keyed adjustment only for nonnegative tags,
  converts back with `0.06F`, and consults DCM only for a positive adjusted
  delta. Its ordered `30.0F <= raw` guard and two ordered out-of-bounds
  comparisons make unordered-NaN raw or projected values eligible.

## Reasoning

Descriptor registration, handler construction, owned-container reset and
sorting, both lookup algorithms, and the materialization call site close the
producer-to-consumer path. STP/SFL/SLP and DCM are therefore not merely scroll
rendering metadata: their output can decide whether a parsed record stays
pending or is constructed during the current outer update.

## Alternatives and falsifiers

- Competing explanation: the schedules are used only after runtime-note
  construction.
- Evidence that would disprove this claim: materialization reading a different
  container, or the queue ignoring the schedule-derived projection result.

## Unknowns

- The reverse keyed-adjustment mode exists for other projection callers but is
  not used by pending-note materialization and is outside this gameplay claim.
- External speed-table rows and deployed `[OFFSET] DRAW` values remain
  unavailable; their selection and producer paths are reconstructed by
  `claim.configuration.runtime-materialization-input-producers`.

## Consequences

- Ghidra mutations: created the previously missed function boundary at
  `RAM:011c3c60` after byte inspection and a successful dry run.
- Spec sections: `spec/c2s.md`, `spec/timing.md`,
  `spec/configuration.md`.
- Reconstruction code: `C2sProjectionSchedule`, group-1 parser application,
  finalization, keyed integration, DCM lookup, and materialization composition
  in `include/chart/reconstruction.hpp`.
- Tests: `tests/projection_schedule_test.cpp`,
  `tests/runtime_materialization_test.cpp`.

## Verification

Focused tests cover all accepted/rejected commands, exact field defaults,
malformed and extra fields, signed-duration wrap, exact keyed sorting including
equivalent/NaN starts, overlapping contributions, missing keys, backwards
queries, DCM shift/end/zero/source-order rules, reset, the source-shaped near
guard, unordered-NaN eligibility, and end-to-end far-path eligibility. Corpus
aggregation found 1,187 STP, 12,105 SFL, 3,113 SLP, and 2,986 DCM records with
the expected arities across the snapshot's backward-compatible chart versions.
The corpus has 63 duplicate keyed starts, all duplicate full records identical;
the exact sort remains normative for synthetic malformed or differing
equivalent records.
