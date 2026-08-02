# Claim: parsed type 0 constructs a lane-bounded TAP runtime object

- ID: `claim.note.tap-construction`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `note.tap`, `state.ownership`, `audit.indirect_calls`
- Last reviewed: 2026-07-27

## Statement

A parsed record whose type field is 0 constructs a `projView::TapNote`; the
runtime object derives its start lane and width from the record, bounds its
covered lanes to the 16-lane logical domain, and stores record `+0x08`
multiplied directly as a float by `0.06F` as its scheduled position.

## Anchors

- `game.exe @ RAM:00da62c0, FUN_00da62c0, parsed-record iteration and factory call`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed type switch and virtual load`
- `game.exe @ RAM:00c1d380, FUN_00c1d380, type-0 object construction`
- `game.exe @ RAM:018d9760, TapNote vtable`
- `game.exe @ RAM:01c325d0, RTTI type descriptor for projView::TapNote`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, TAP record-to-runtime initialization`
- `game.exe @ RAM:011cbb50, FUN_011cbb50, encoded-width lookup`
- `game.exe @ RAM:018672f8, initialized 0.06F chart-position scale`

## Observations

- The chart setup path copies queued `0x174`-byte records and passes eligible
  records to a factory.
- Factory case 0 allocates a `0x1c8`-byte object, calls `FUN_00c1d380`, invokes
  virtual slot `+0x20` with the parsed record, and stores the object in the
  active-note vector.
- Constructor RTTI identifies its class as `projView::TapNote`.
- The load path copies record `+0x28` to object `+0x84` as the start lane. It
  transforms record `+0x2c` through a 16-entry lookup and stores the resulting
  width at object `+0x80`.
- The same path derives bounded start/end lane fields and prevents coverage
  outside lanes 0 through 15.
- It reads record `+0x08` as a float, multiplies it directly by the initialized
  `0.06F` global, and stores the float result at object `+0x90`. No integer
  conversion or explicit rounding lies on this field's path.
- It constructs an embedded `JudgeTapChecker` at object `+0xe8`.

## Reasoning

The parsed type switch, constructor RTTI, virtual load, and field consumers form
a continuous parsed-record-to-runtime-object path. The lane fields are later
consumed by TAP candidate and input loops.

## Alternatives and falsifiers

- Competing explanation: the object is a visual-only TAP representation.
- Evidence that would disprove this claim: input/judgement calls operate on a
  different object, or the copied fields are not used to bound judgement lanes.

## Unknowns

- None for the common TAP construction fields. Externally loaded checker
  values remain parameters under their owning configuration claims.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/notes/tap.md`, `spec/timing.md`.
- Reconstruction code: `make_note_scheduled_position`,
  `parse_c2s_common_lane_geometry`, `decode_c2s_note_width`, and
  `bounded_note_lane_extent` in `include/chart/reconstruction.hpp`.
- Tests: `tests/play_clock_test.cpp` covers positive and negative parsed
  positions using the direct scale; `tests/common_lane_width_test.cpp` covers
  the fixed width tables and bounded lane extent.

## Verification

The start lane, width, scheduled position, and embedded checker are independently
consumed along the per-tick judgement path in
`claim.note.tap-candidate-judgement-gate`. The scale matches the independently
recovered manager clock scale in `claim.timing.gameplay-clock-reconstruction`.
Table immutability and all width-domain edges are independently closed by
`claim.parser.common-lane-width-encoding`.
The three copied result-component identifiers and their only gameplay consumer
are independently closed by
`claim.judgement.result-component-identifier-flow`.
