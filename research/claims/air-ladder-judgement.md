# Claim: ordinary ALD constructs a candidate-free AirLadderNote with profile-7 checkpoints

- ID: `claim.note.air-ladder-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`,
  `input.logical_state`, `matching.candidates`, `judgement.types`,
  `judgement.windows`, `judgement.miss`, `note.other_variants`,
  `state.ownership`, `config.external`, `interactions.cross_note`,
  `audit.indirect_calls`
- Last reviewed: 2026-07-21

## Statement

An ALD parsed type-9 chain normally constructs RTTI-identified
`projView::AirLadderNote`. It exposes no lane candidate, builds one independent
retained-profile-7 checker for every parsed control point, can resolve multiple
checkers in one gameplay substep, submits source category 18 for each accepted
checker, and becomes terminal only after every checker resolves and an external
completion threshold is reached. One exact parsed-field combination dispatches
to the separate `HeavenHoldNote` class and is outside the ordinary path claim.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, ALD command ID 0x29 and descriptor`
- `game.exe @ RAM:01be5dcc, initialized ALD family record, parsed type 9`
- `game.exe @ RAM:011c8870, FUN_011c8870, type-9 chain creation/continuation`
- `game.exe @ RAM:011cf760, FUN_011cf760, missing-token empty-string fallback`
- `game.exe @ RAM:011cfce0, FUN_011cfce0, 16-entry style-string lookup`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, type-9 concrete-class selection`
- `game.exe @ RAM:00c110f0, FUN_00c110f0, 0x11c-byte AirLadder construction/reset`
- `game.exe @ RAM:018d9374, AirLadderNote gameplay vtable`
- `game.exe @ RAM:01c3249c, RTTI type descriptor, projView::AirLadderNote identity`
- `game.exe @ RAM:00c132f0, FUN_00c132f0, parsed-point to runtime-checker load`
- `game.exe @ RAM:00c19180, FUN_00c19180, fixed retained input profile 7`
- `game.exe @ RAM:00c13810, FUN_00c13810, update wrapper`
- `game.exe @ RAM:00c12050, FUN_00c12050, gameplay update and deferred transition`
- `game.exe @ RAM:00c11ca0, FUN_00c11ca0, all-checker update and category 18 submission`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, all-sentinel candidate output`
- `game.exe @ RAM:00c12d80, FUN_00c12d80, threshold/checker terminal predicate`
- `game.exe @ RAM:00c119a0 and RAM:00c11660, deletion and destruction`

## Observations

- ALD has command ID `0x29` and resolves to parsed type 9. Its parser creates a
  new record or continues an existing type-9 record whose chain is nonempty
  and whose connection selector, style code, last position, lane, width, and
  saved numeric property match the new command. Failure to match creates a new
  chain rather than taking the parser diagnostic used by secondary attachment.
  After append, the parser recomputes every 0x20-byte control point's scheduled
  value and updates the root's saved current position.
- The factory normally allocates `0x11c` bytes and constructs AirLadderNote.
  When parsed field `+0x78` is zero and style field `+0xa0` is code 15, it
  instead allocates `0x348` bytes and constructs `projView::HeavenHoldNote`.
  The executable style table maps code 15 to exact string `NON`. Type 13 always
  selects that same HeavenHold class. The local corpus has 195,469 ALD lines;
  184,831 use the current 12-token form, 10,638 use an older 11-token form, and
  six current-schema lines expose the visible zero/`NON` pair.
- The older one-short ALD form omits accessor index 11. The accessor returns an
  empty string for that missing token, and style lookup returns code 0 before
  any comparisons. It therefore cannot satisfy style code 15 and remains on
  the ordinary AirLadder path.
- AirLadder load converts every parsed control point in vector `+0x6c..+0x70`
  into one 0x88-byte owned runtime record. Each record initializes the shared
  retained-profile checker with fixed profile 7. The selected external record
  associated with parsed field `+0x84` also supplies a copied completion
  threshold at runtime offset `+0x118`.
- The candidate virtual is the shared all-negative-sentinel implementation.
  Gameplay does not compare a manager-selected lane candidate.
- Each update iterates the whole runtime vector. A checker is updated while its
  retained state is below 2. If the update leaves it above 1, AirLadder stores
  the returned result byte, submits source category 18, increments its resolved
  count, and continues to later records. There is no break or front-only gate,
  so multiple records can resolve in one substep.
- Source category 18 maps to shared category 7. Under normal valid runtime
  bounds it reaches the authoritative aggregate; preexisting terminal routing
  selects the shared observer-only path instead.
- After checkpoint updates, the concrete predicate requires the current
  gameplay value to be at or beyond the copied external threshold and the
  resolved count to equal the runtime record count. Equality at the threshold
  is accepted. Success requests the ordinary deferred base transition.
- Construction/reset clears the owned vector and counters. Destruction releases
  every runtime record and its resources before the `0x11c` allocation is
  freed. The sibling presentation update does not submit results.

## Reasoning

The initialized family table and parser data flow connect ALD to type 9. The
factory branch, allocation, constructor vtable, and RTTI identify the ordinary
runtime class and its exact exception. Load-time vector cardinality and fixed
profile selection connect every parsed point to one checker. The concrete
update loop and terminal predicate establish multi-resolution behavior,
category 18, candidate absence, completion ordering, and lifetime.

## Alternatives and falsifiers

- Competing explanation: ALD is always HeavenHold, checkpoints are front-only,
  profile selection varies per point, or AirLadder competes for lane input.
- Evidence that would disprove this claim: ordinary field combinations entering
  the `0x348` constructor, a non-profile-7 checker initializer, a loop exit after
  one resolution, a non-sentinel candidate, or terminal transition before the
  threshold/all-checker conjunction.

## Unknowns

- The player-facing meanings of the connection/numeric fields and style codes
  other than exact `NON` are not assigned.
- The external completion threshold, retained-profile windows/corrections,
  values, units, defaults, and selection mode remain parameters.
- `claim.input.snapshot-profile-synthesis` closes physical profile-7
  production and history ownership. Player-facing identity of source category
  18 and result bytes is not assigned.
- The special zero/`NON` HeavenHold gameplay path is reconstructed separately
  by `claim.note.heaven-hold-judgement`.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable.
  Temp-clone disassembly only recovered otherwise undefined import thunks.
- Spec sections: `spec/notes/air_ladder.md`, `spec/c2s.md`, `spec/input.md`,
  `spec/matching.md`, `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `AldRuntimeKind`, `ald_missing_style_code`,
  `select_ald_runtime`, and
  `air_ladder_*` checkpoint/lifecycle helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/air_ladder_judgement_test.cpp`.

## Verification

Descriptor/type mapping, parser new/continuation paths, style table, aggregate
corpus shape, both factory branches, allocation, RTTI, complete vtable, load
cardinality, profile selection, update-loop continuation, category submission,
candidate absence, terminal predicate, deferred transition, presentation
separation, and destruction were checked independently. Focused tests cover
the class-selection predicate, profile/category mapping, multi-resolution
counting, candidate absence, and both terminal conditions.
