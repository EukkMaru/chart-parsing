# Claim: common note widths use a fixed clamp, encoding, and bounded extent

- ID: `claim.parser.common-lane-width-encoding`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `pipeline.boundaries`, `note.tap`, `note.hold`, `state.ownership`
- Last reviewed: 2026-07-27

## Statement

The common group-2 note path clamps authored width to 1 through 16, stores the
fixed encoding `width - 1`, decodes indices 0 through 15 to widths 1 through
16 during runtime loading, and bounds the resulting lane interval to the
logical 16-lane domain.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, common field parse and clamp, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:011cbb70, FUN_011cbb70, fixed width encoder, hash 1141becb372790a02cb031bf5cc3183b7b7b8e0a0d6368b5fc7be77d21d317fb`
- `game.exe @ RAM:01be59b0 and RAM:01be59f0, initialized encoder tables`
- `game.exe @ RAM:011cbb50, FUN_011cbb50, fixed width decoder, hash 36b5c04795b7369470ae4774b337f5ac579f45c5c04fdd6b8a3bf30ee4452cb9`
- `game.exe @ RAM:01be5840, initialized decoder table`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, runtime lane extent, hash d2e5865736dbc63e4e92294cfabd119e14c26dfee62a143e57f90f77d955b449`

## Observations

- The common event parser reads lane from field 3 and width from field 4.
  Missing integer fields use the token accessor's zero default. Lane is
  retained as authored; width is clamped to the inclusive range 1 through 16.
- The encoder's initialized entries for inputs 0 through 16 are
  `0, 0, 1, ..., 15`; negative input first becomes zero and input greater than
  16 selects the fixed final value 15.
- The encoder tables have no writer in the program. Their only code reads are
  inside the encoder.
- The decoder accepts only indices 0 through 15 and reads initialized widths
  1 through 16 at a fixed stride. Any other encoded value returns zero. Its
  table has no writer and no other code reader.
- Shared runtime loading copies the raw lane, decodes the stored width, then
  computes start as `max(lane, 0)` and count as
  `max(min(lane + width, 16) - start, 0)`. Both arithmetic operations use
  native wrapping 32-bit integer semantics; this matters only for malformed
  extreme lane values.

## Reasoning

The parser clamp makes every ordinary common-note record use encoder indices
0 through 15. The immutable encoder and decoder tables are exact inverses on
that accepted domain. The shared loader is used by the runtime roots, so the
same bounded logical-lane rule reaches candidate and contact consumers.

## Alternatives and falsifiers

- Competing explanation: a chart-version or compatibility path rewrites the
  width tables before gameplay.
- Evidence that would disprove this claim: a writer to any involved table, a
  version-selected alternate decoder, or a runtime root bypassing the common
  lane loader while claiming the same geometry.

## Unknowns

- None for the common width encoding and lane-bound calculation. Family-
  specific endpoint geometry remains governed by its own claim.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/c2s.md`, `spec/notes/tap.md`,
  `spec/notes/hold.md`, `spec/notes/flick.md`, `spec/notes/mine.md`.
- Reconstruction code: `encode_c2s_note_width`,
  `decode_c2s_note_width`, `parse_c2s_common_lane_geometry`, and
  `bounded_note_lane_extent`.
- Tests: `tests/common_lane_width_test.cpp`.

## Verification

Focused tests cover encoder and decoder endpoints, parser-default width,
both-side clipping, fully out-of-range lanes, invalid decoded width, and
32-bit lane-addition and subtraction wrap. Full-table xrefs independently rule
out runtime compatibility writers.
