# Claim: sustained-note DCM is selected independently per projected endpoint

- ID: `claim.presentation.sustain-endpoint-dcm-projection`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `timing.clock`, `note.hold`, `note.slide`,
  `note.other_variants`, `state.ownership`
- Last reviewed: 2026-08-18

## Statement

Every recovered sustained-note presentation update applies keyed schedule
adjustment and the DCM lookup independently to each path point passed to its
geometry builder. DCM is neither a single entity/root-time body factor nor a
per-final-vertex query. The builders interpolate and clip between endpoints
whose projected values have already been computed.

## Anchors

- `game.exe @ RAM:00b28890, FUN_00b28890, shared positive-delta DCM projection, hash 19696db1083c59e7e5324f7e5dcc4866796038ba33244d08ddd4b650966ceae8`
- `game.exe @ RAM:00c2a440, FUN_00c2a440, Hold root/end projection, hash e97bd9b835a5ab2977b05632b91a12f393cf2f154ea039167ec4e510d36761ec`
- `game.exe @ RAM:00c0ee40, FUN_00c0ee40, Slide root/generated-endpoint projection, hash 081634ec8e24c1d970ce8ee43104eaef0940b64500bd6703aeb622b8df0e673d`
- `game.exe @ RAM:00c229c0, FUN_00c229c0, AirHold start/end/AHX projection, hash 3d0318ae8d98c745d7bc0a45b4b0466bd68b53cacadb873a3efd7afaec5fb8d5`
- `game.exe @ RAM:00c26e00, FUN_00c26e00, AirSlide root/authored-control projection, hash 4c2310bb857fffda782d728e8e3c60ad99ea9292070ff9decbbb3a72fa769c7b`
- `game.exe @ RAM:00c12350, FUN_00c12350, AirLadder checkpoint/authored-path projection, hash bc494d630f8811a52cfffd3252d7358558fb99e73a1cc951c33814f2dd88a458`
- `game.exe @ RAM:00c16a90, FUN_00c16a90, AirSolid root/authored-endpoint projection, hash a63e8c23215750c97ca1121fa86f66dcfd756e4e13d3423b75feeeaced018c24`
- `game.exe @ RAM:00c149a0, FUN_00c149a0, HeavenHold root/authored-endpoint projection, hash 55c4af0cd9eaad714ecae4f9d8096940bdfbdd472ecbfeaf5ab367610bc01c85`

## Observations

- The common helper queries DCM only for its own positive adjusted delta at
  `(manager + delta) * 16.666666F`, then applies that factor to that delta.
- Hold calls it separately for root and end. Slide calls it for root and every
  generated path endpoint before giving equal-cardinality arrays to the mesh.
- AirHold calls it for start, authored end, and each unresolved AHX point; its
  path envelope folds those separately projected values.
- AirSlide, AirSolid, and HeavenHold call it for the root and each authored
  control/end point. AirLadder does so for every generated checkpoint effect
  and separately for root plus authored endpoints in its main path.
- No geometry builder calls the DCM lookup while emitting vertices. Splits,
  clipping, tessellation, and ribbons interpolate the supplied projected
  endpoint fields.

## Reasoning

The shared helper establishes one factor query per invocation. Enumerating its
family-local callers and following each resulting value into the corresponding
point array/envelope closes the query granularity. The later mesh builders
operate only on projected floats, separating endpoint lookup from vertex
interpolation.

## Alternatives and falsifiers

- Competing explanation: a sustained body reuses the root's factor.
- Evidence that would disprove this claim: a family update caching one factor
  before its endpoint loop, or a mesh builder reaching the DCM schedule while
  appending a vertex.

## Unknowns

- Runtime speed and projection-base values are externally selected inputs;
  their producers are documented separately. They do not change DCM query
  granularity.
- External resources and final camera/material composition remain outside this
  endpoint-projection claim.

## Consequences

- Ghidra mutations: supported decompiler comment at `00b28890` records the
  per-endpoint caller contract; existing family plate comments identify their
  point arrays and geometry consumers.
- Spec sections: `spec/timing.md` and each sustained-note presentation section.
- Reconstruction code: `sustain_endpoint_projected_depth_from_schedule`.
- Viewer: every root/control/end depth uses `depthAt(time, key)` separately;
  body sampling interpolates those projected endpoints.
- Tests: `tests/projection_schedule_test.cpp` proves two endpoints in different
  DCM intervals receive different factors.

## Verification

The seven family update functions were inspected independently from the common
projection helper and from their mesh builders. The focused test uses factors
2.0 and 0.5 in disjoint intervals; reusing the root factor would produce a
different terminal depth and fails the asserted endpoint values.
