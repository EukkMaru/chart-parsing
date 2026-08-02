# Claim: parsed component identifiers select retained result-event identity

- ID: `claim.judgement.result-component-identifier-flow`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `pipeline.boundaries`, `judgement.types`,
  `note.tap`, `note.hold`, `state.ownership`, `interactions.cross_note`
- Last reviewed: 2026-07-27

## Statement

The parser assigns source-ordered identifiers to a root's primary,
middle/end, and attached-secondary result components. Runtime loading copies
those three slots unchanged. Shared result submission selects primary for
source categories 0 and 1, middle/end for 2 through 6, and secondary for 7
through 13, then retains the selected identifier in the authoritative result
event.

## Anchors

- `game.exe @ RAM:011c78e0 and RAM:011c7980, parser-pass counter initialization and event calls`
- `game.exe @ RAM:011c8870, FUN_011c8870, root allocation and lazy attachment allocation, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:011cae00, FUN_011cae00, accepted 0x174-byte record append, hash 978c5e977e25f01b044ebb018a8af0d55b08b87e52757bef8b420c88e54d2245`
- `game.exe @ RAM:00c19c00, FUN_00c19c00, runtime identifier initialization`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, parsed-to-runtime copy, hash d2e5865736dbc63e4e92294cfabd119e14c26dfee62a143e57f90f77d955b449`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, category-to-slot selection, hash 6d22a0d5b382d07b18f6280e0005284623e862a2b09397886156820a8f22c698`
- `game.exe @ RAM:00b97730, FUN_00b97730, event construction`
- `game.exe @ RAM:00b95870, FUN_00b95870, aggregate mutation and retained-event copy`

## Observations

- Each header or full event pass initializes its identifier counter to zero
  and passes it by address through source-order group-2 handling.
- Parsed-record construction initializes all three slots to negative one.
  Accepted root types 0, 4, 6, and 11 reserve one identifier in the primary
  slot. Types 1, 2, 9, 10, 12, and 13 reserve two consecutive identifiers in
  primary and middle/end. Attachment-only types 3, 5, and 8 append no root and
  lazily assign the compatible root's secondary slot only if it is still
  negative. Counter arithmetic is native wrapping 32-bit arithmetic.
- Common runtime loading copies parsed offsets `+0x88`, `+0x8c`, and `+0x90`
  to runtime offsets `+0xa8`, `+0xac`, and `+0xb0`.
- The shared result handler defaults to runtime primary, selects middle/end for
  original source categories 2 through 6, and selects secondary for 7 through
  13. It performs this selection before the existing source-to-dispatch map.
- The dispatcher places the selected identifier in field zero of the result
  event. Aggregate contribution and validity branch on mapped category and
  result byte, not on this identifier, but the complete event is copied into
  retained result state and terminal-summary processing.

## Reasoning

The same source-order counter produces the three parsed slots, the common
loader preserves them, and the sole result handler consumes them as a
category-selected event field. Full offset-reference closure in the
factory-reachable note code found no other gameplay read of the three runtime
slots. This assigns both their ownership and their exact outcome-path role.

## Alternatives and falsifiers

- Competing explanation: one copied field controls candidate selection or
  result tier rather than only identifying the submitted component.
- Evidence that would disprove this claim: a candidate/checker read of these
  runtime slots, a second producer after loading, another result submitter with
  a different category partition, or aggregate contribution branching on the
  identifier.

## Unknowns

- Player-facing labels for the three component groups remain unassigned.
- Rejected malformed records can leave source-order gaps only where their
  family handler has already reserved identifiers; family-specific rejection
  claims govern those paths.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/c2s.md`, `spec/judgement.md`,
  `spec/notes/tap.md`, `spec/notes/hold.md`.
- Reconstruction code: `C2sResultComponentIdentifiers`,
  `allocate_c2s_root_result_identifiers`,
  `attach_c2s_secondary_result_identifier`, and
  `select_note_result_identifier`.
- Tests: `tests/result_identifier_test.cpp`.

## Verification

Focused tests cover the complete root-type allocation table, lazy one-time
secondary allocation, every source-category partition, default selection, and
counter wrap. The parser callers, constructor defaults, common copy, sole
result handler, dispatcher event construction, aggregate contribution, and
retained-event copy were checked independently.
