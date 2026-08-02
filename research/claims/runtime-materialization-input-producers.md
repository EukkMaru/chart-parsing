# Claim: setup resolves both runtime materialization projection inputs

- ID: `claim.configuration.runtime-materialization-input-producers`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.clock`, `state.ownership`, `config.external`
- Last reviewed: 2026-07-27

## Statement

Gameplay setup derives the materialization speed from a
`PlayOptionSpeedTableRecord` selected by `PlayOptionSpeedID`, after an optional
first-unit `SkillBefore` type-6 override, and derives the projection base offset
from configuration key `[OFFSET] DRAW`, whose executable descriptor default is
`0.0F`.

## Anchors

- `game.exe @ RAM:00da06c0, FUN_00da06c0, setup composition, hash 0678d314e7aa6bbc63258739956ed62c800f8de9e915e805b011b2c593634604`
- `game.exe @ RAM:00d87c00, FUN_00d87c00, SkillBefore speed-ID selection, hash 495a8ed7c5d8272c1c9b4d046be0676a785d4147398284466a4f846de4bba469`
- `game.exe @ RAM:00b6dfb0, FUN_00b6dfb0, validated PlayOptionSpeedID store, hash b242c4ee0de2e7a7f376bb39502981983b3035c5abfcaa05bd0c96951264d449`
- `game.exe @ RAM:010753a0, FUN_010753a0, speed-table cardinality, hash 2242aaf6b079d3865837de3f3cda443f23f119d8a3979cfd9551ec577490d612`
- `game.exe @ RAM:01075530, FUN_01075530, speed-record lookup, hash 5a20f2eb3db99899522d4aafd01fe51abe0d7cffb34d44ce444643b8e862fbfa`
- `game.exe @ RAM:010743a0, FUN_010743a0, RTTI-identified PlayOptionSpeedTable construction, hash fd2ceb8dfaf4a9795ebfa2953573b6e0120427b56fc4f545aea2721013bf46b1`
- `game.exe @ RAM:00b2b4f0, FUN_00b2b4f0, sole runtime-speed setter, hash af6b17cc509380c5168b5f2989c026b1576736dbcae2495b9de5fc4a085c52f1`
- `game.exe @ RAM:004dcff3, configuration descriptor 285 initialization`
- `game.exe @ RAM:00c30510, FUN_00c30510, configuration load/finalization, hash 3e26b3559acd8d387dc7de1ad73ae2498e76422591847be59958bf877d3cb36b`
- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, manager reset input copies, hash bd29062d3b65f1f97ae93fc82fb2ada49f46aa046abb6fb1af6034af02963fd7`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, successful chart setup input copies, hash 3d3ca283af0a2b8c0a02013e185d1341c44f9b4a30d782a798f8fcf0f45bedf8`
- `game.exe @ RAM:00b29c90, FUN_00b29c90, materialization consumer, hash 88a81914b32218d202582c3aa46579ad51e4dec0f6a32da750c57cce674a7584`

## Observations

- Fresh play-option construction initializes the speed ID to zero. Setup
  copies that current ID, then looks up the selected `SkillBefore` record by
  its identity.
- The `SkillBefore` scan walks units in source order. The first unit with type
  6 is decisive. Its unsigned speed ID replaces the current ID only when it is
  below the `PlayOptionSpeedTable` record count. An invalid first type-6 ID
  returns immediately with the current ID, so a later valid unit is ignored.
- The validated store repeats the same table-count bound. Speed lookup indexes
  a 0x38-byte `PlayOptionSpeedTableRecord` and reads its double at record
  offset `+0x10`; an invalid ID returns zero.
- Setup narrows that double to float, applies the scalar maximum operation with
  `0.1F`, and calls the sole setter for manager field `+0x354`. The operand
  order also maps NaN to `0.1F`; positive infinity remains positive infinity.
- Configuration descriptor 285 is initialized with section `OFFSET`, key
  `DRAW`, and float default `0.0F`. The loader stores its selected float at
  source field `+0x570` and copies it to finalized field `+0xafc`.
- Manager reset and every successful chart setup copy finalized `+0xafc` into
  materialization field `+0x3a8`. The materialization predicate reads
  `+0x354` and `+0x3a8`; no second transform or fallback intervenes.

## Reasoning

The exact table RTTI and record stride identify the speed resource rather than
merely an anonymous float. Following setup from current option ID through the
ordered skill-unit override, count checks, record lookup, float narrowing,
clamp, sole manager setter, and materialization read closes its producer path.
The configuration descriptor, loader finalization copy, both manager copies,
and predicate read similarly close the base-offset path. Runtime resource
contents remain unavailable, but their selection and default behavior do not.

## Alternatives and falsifiers

- Competing explanation: materialization speed is an arbitrary caller float.
- Evidence that would disprove this claim: another writer reaching manager
  `+0x354`, or setup indexing a different table after the skill override.
- Competing explanation: the base offset is a chart command or a hardcoded
  constant.
- Evidence that would disprove this claim: a materialization-time writer to
  `+0x3a8`, or a different finalized configuration field reaching the
  predicate.

## Unknowns

- Actual `PlayOptionSpeedTable` rows and deployed `[OFFSET] DRAW` values are
  external resources absent from the executable snapshot. They remain clean-
  room inputs.
- The upstream configuration-path choice selects which external file is
  loaded, but does not change the recovered key, default, final field, or
  gameplay consumer.

## Consequences

- Ghidra mutations: none; the supported roles retain default symbols.
- Spec sections: `spec/configuration.md`, `spec/timing.md`.
- Reconstruction code: `MaterializationSkillBeforeUnit`,
  `PlayOptionSpeedTableRecord`, `select_materialization_speed_id`,
  `resolve_materialization_runtime_speed`, and
  `resolve_materialization_projection_base_offset` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/runtime_materialization_test.cpp`.

## Verification

The current-ID initialization, first-unit stop behavior, invalid-ID paths,
table stride/value field, narrowing/clamp order, speed setter, descriptor
identity/default, loader finalization copy, both manager copy sites, and
materialization reads were traced separately. Focused tests cover no override,
valid override, invalid-first blocking, invalid current ID, negative, NaN and
infinite speed values, and present/missing `[OFFSET] DRAW`.
