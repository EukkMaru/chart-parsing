# Claim: ALD generates sampled profile-7 checkpoints from authored controls

- ID: `claim.note.air-ladder-generated-checkpoints`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `judgement.types`, `note.other_variants`,
  `state.ownership`, `audit.closure`
- Last reviewed: 2026-08-03

## Statement

An ordinary type-9 ALD record owns a `0x24`-byte authored-control vector and a
distinct `0x20`-byte generated vector. When the interval field is positive and
the authored chain advances, the parser regenerates samples from the root at
that fixed chart-grid interval, interpolates lane, vertical tenths, and decoded
width across authored controls, and recomputes each sample's schedule. Ordinary
AirLadder loading creates one retained-profile-7 checker per generated sample,
not per authored control. Generated points also own individual presentation
records, while the main type-9 path geometry is precomputed from authored
controls under `claim.note.air-ladder-precalc-presentation`.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, type-9 append/regeneration caller, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:011caf70, FUN_011caf70, 0x24-byte authored-control append, hash 9e0dac01c2deaa654b9d6a48effcf051994f456b9e672d4e9e395ab1b395e07f`
- `game.exe @ RAM:011c6a50, FUN_011c6a50, fixed-interval generated-point producer, hash c77f4a33bf9abf2d2430acbdbe5ed88d13fd80dddfd9eb0cc98685392641413c`
- `game.exe @ RAM:011ca7a0, FUN_011ca7a0, 0x20-byte generated-record append, hash e4b7f6f8085451ef945b76d6b182e9534ee2c1a05845c62642077022e8e4ba1a`
- `game.exe @ RAM:011c0680 via thunk RAM:00449b1b, generated schedule recomputation`
- `game.exe @ RAM:00c132f0, FUN_00c132f0, generated-record to runtime-checker load, hash 2d7edc23ab6a2229647e9f6d2a9e3061e2ad6205607aa8bce11a531d69d6340c`
- `game.exe @ RAM:00c19180, FUN_00c19180, fixed retained input profile 7`
- `game.exe @ RAM:00c11ca0, FUN_00c11ca0, all-checker update and category 18 submission, hash 5ad898a40e7f8d1a0398d50cab2b52305cb3d22e99915687570ace0005f5bb6f`
- `game.exe @ RAM:00c12d80, FUN_00c12d80, authored-end/all-checker terminal predicate, hash b267c18c2b3ab7640df3fc02354fb00cbb46328ad150432c449aa2e29574b516`
- `game.exe @ RAM:00c12350, FUN_00c12350, generated-effect and authored-path presentation update, hash bc494d630f8811a52cfffd3252d7358558fb99e73a1cc951c33814f2dd88a458`
- `game.exe @ RAM:00c03c00, FUN_00c03c00, shared type-9 geometry construction, hash d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`

## Observations

- Type-9 parsing appends authored endpoints with a `0x24`-byte copier. After a
  new chain is created or a compatible chain is extended, it calls the
  generated-path producer on the subobject at parsed offset `+0x68`. That
  subobject's begin/end pointers are `+0x6c/+0x70`; its records are `0x20`
  bytes, and its interval is stored at `+0x78`.
- The producer clears the generated vector first. It returns empty when the
  authored-control vector is empty, the interval is nonpositive, or the root
  grid tick is not less than the final authored-control grid tick.
- In the advancing case, the cursor starts at the root grid tick and advances
  by the exact positive interval. Samples are emitted while the cursor is
  below the final tick. If an advance lands exactly on the final tick, the
  final authored endpoint is appended; an overshoot does not append it.
  Therefore ordinary monotone input produces `floor((end-start)/interval)+1`
  samples, including the root.
- For an interior cursor, the producer advances through the authored controls
  until it finds the nonzero-length segment whose end tick is greater than the
  cursor. It linearly interpolates the root/control lane integers, the
  integer-tenth vertical property after multiplication by `0.1`, and the
  fixed-table-decoded widths. A cursor equal to an intermediate control uses
  that control's values as the next segment's start. Duplicate-position
  controls are skipped as zero-length interpolation spans.
- A generated record contains a canonicalized chart position, later-computed
  schedule, interpolated lane, vertical value, decoded width, and an enabled
  byte. The parser walks the generated vector in `0x20`-byte steps and replaces
  each schedule slot using the tempo schedule.
- AirLadder load walks parsed `+0x6c..+0x70` in `0x20`-byte steps and constructs
  one owned `0x88` runtime record per generated record. It copies the generated
  schedule, lane, vertical value, width, and selected SLA tag, then initializes
  every retained checker with fixed profile 7. First and later generated
  records select different presentation resource tables.
- Gameplay visits every unresolved runtime checker in one update. A newly
  resolved checker submits source category 18 and increments the resolved
  count; iteration continues, so multiple generated samples can resolve in one
  substep. Candidate output remains all-negative-sentinel. Completion requires
  every generated checker resolved and reaching the inclusive final authored
  endpoint schedule stored by the type-9 precompute path.
- The sibling presentation update visits generated records for their own
  per-checkpoint presentation state, then builds the main geometry input from
  root plus authored precompute endpoints. Its type-9 branch submits three
  primitive/resource streams. The two presentation roles must not be
  collapsed into one vertex path.
- A content-free pass over all 7,752 local charts found 195,469 ALD lines in
  the two known field shapes. Exact continuation formed 91,368 chains: 43,577
  have a positive interval and advancing span, while 47,791 have a
  nonpositive interval and therefore generate no samples. The positive chains
  produce 100,952 samples in aggregate (maximum 113 in one chain); 13,101 land
  exactly on their final endpoint and 30,476 overshoot it. These counts are a
  compatibility check, not the source of the algorithm.

## Reasoning

The two append helpers establish different element widths and owners. The
type-9 parser passes the parsed root, interval field, and fixed resolution to
the `+0x68` producer, then schedules exactly the returned `0x20` records.
AirLadder load consumes that same vector width and cardinality. The checker
initializer and update paths connect every generated record to a profile-7
judgement lifecycle and individual presentation record. A separate authored
precompute supplies the main path and final endpoint schedule. This closes the
generated producer-to-consumer path without conflating the two owners.

## Alternatives and falsifiers

- Competing explanation: `+0x6c` contains authored ALD controls, the interval
  is only a visual selector, or generated samples do not own runtime checkers.
- Evidence that would disprove this claim: a `0x24` parse append into `+0x68`,
  a runtime load over the authored `+0x44` vector, a checker count independent
  of generated-vector length, or another ordinary type-9 producer replacing
  the fixed-interval vector before load.

## Unknowns

- A positive interval can wrap the signed cursor on extreme positions and
  expand source work. The exact valid-chart algorithm is reconstructed; safe
  product admission for hostile arbitrary files remains a separate robustness
  policy and must be labeled as such.
- Retained-profile-7 numerical configuration remains an unavailable
  parameter. The terminal schedule is not external: the authored precompute
  derives and stores it.
- Type-9 geometry emits three primitive streams, but their asset-independent
  visible roles, exact lifetime, and complete field-to-vertex classification
  are not yet closed. `render.air_ladder` remains partial.

## Consequences

- Ghidra mutations: none; all work was read-only through Ghidra/MCP.
- Superseded claim: `research/claims/air-ladder-judgement.md`.
- Spec sections: `spec/c2s.md`, `spec/judgement.md`,
  `spec/notes/air_ladder.md`.
- Reconstruction code: `AirLadderPathPoint`,
  `generate_air_ladder_checkpoints`, generation disposition, and existing
  `air_ladder_*` lifecycle helpers in `include/chart/reconstruction.hpp`.
- Tests: `tests/air_ladder_judgement_test.cpp`.

## Verification

The authored append, both type-9 regeneration call sites, fixed-interval
producer, generated append, schedule pass, runtime load cardinality, checker
profile, all-record update, terminal predicate, and sibling presentation path
were followed independently. Focused tests cover root cadence, exact-end and
overshoot behavior, interpolation across controls, duplicate positions,
nonpositive/no-forward cases, cursor-wrap classification, multiple checker
resolution, and the authored-end conjunction including unordered-float
behavior. The aggregate corpus pass exercised
18 interval values, chains up to 459 authored controls, both legacy/current
field shapes, and both exact-end and overshoot outcomes without exposing a new
generation disposition.
