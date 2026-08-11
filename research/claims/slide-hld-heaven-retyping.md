# Claim: HLD-styled Slide chains are retyped to HeavenHold before path generation

- ID: `claim.note.slide-hld-heaven-retyping`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`, `judgement.types`, `note.slide`, `note.other_variants`, `state.ownership`
- Last reviewed: 2026-07-21

## Statement

In the ordinary gameplay parse path, an `SLD`/`SXD`/`SLC`/`SXC` chain whose
field-8 style maps to exact string `HLD` is changed from parsed type 2 to type
13 before generated-path construction. The pass writes path-scalar integer 10
to the root and every control point, preserves the command-form flag, and
therefore constructs `HeavenHoldNote`: SLD/SLC retain start-profile pair 0/1,
while SXD/SXC retain pair 2/3.

## Anchors

- `game.exe @ RAM:011c64a0, FUN_011c64a0, field-8 Slide style accessor`
- `game.exe @ RAM:011d01e0, FUN_011d01e0, exact SLD/HLD/GRN style table`
- `game.exe @ RAM:011c8870, FUN_011c8870, Slide command-form flag and chain construction`
- `game.exe @ RAM:011c7980, FUN_011c7980, ordered post-parse call sequence`
- `game.exe @ RAM:011c4af0, FUN_011c4af0, type and discriminator rewrite`
- `game.exe @ RAM:011bda60, FUN_011bda60, type-13 generated-record producer`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, type-13 HeavenHold factory case`
- `game.exe @ RAM:00c18800, FUN_00c18800, preserved flag to start-profile selection`

## Observations

- The four Slide commands enter the same parsed type-2 handler. SLD/SLC store
  zero in parsed byte `+0x98`; SXD/SXC store one. Field 8 is independently
  decoded through a three-entry exact string table: `SLD` is code 0, `HLD` is
  code 1, and `GRN` is code 2. Empty or unrecognized strings also return zero.
- The parser stores the style code at root `+0xa4` and requires it when joining
  a compatible command to an existing type-2 chain.
- After all group handlers finish, `FUN_011c7980` invokes `FUN_011c4af0`
  before the generated-record producers. For every type-2 record whose
  `+0xa4` equals one, that pass writes 10 to root `+0x30`, writes 10 to `+0x18`
  of every `0x24`-byte control point, and changes root type `+0x10` to 13. The
  HeavenHold precompute consumes those fields as tenths, yielding scalar
  `1.0`. The pass does not modify command-form byte `+0x98` or the separate
  presentation selector at `+0xb0`; the latter retains constructor-default
  zero.
- The later shared producer handles the rewritten record through its type-13
  branch and fills the primary generated vector. The factory's type-13 case
  constructs RTTI-identified `projView::HeavenHoldNote`, not `SlideNote`.
- HeavenHold start-checker construction selects its lower profile family for a
  clear `+0x98` byte and its upper family for a set byte. The runtime input
  variant then chooses the member of that pair.
- A read-only aggregate over all 8,156 local `.c2s` entries found two exact
  Slide field-8 `HLD` lines in one chart. Both are ten-field `SXD` records with
  trailing `UP`, so this snapshot exercises the extended/profile-2-or-3 form.

## Reasoning

The style table and parser field write identify the rewrite predicate without
assigning a conventional meaning to an unknown numeric field. Call ordering
proves the type change occurs before type-specific generation. The subsequent
producer, factory, and checker-profile reads close the rewritten record to its
gameplay class and profile family. The unchanged command-form byte explains
why the rewrite has two possible start-profile pairs.

## Alternatives and falsifiers

- Competing explanation: `HLD` is only a presentation style and the record
  still constructs `SlideNote`, or all rewritten forms use one start profile.
- Evidence that would disprove this claim: a later pass restoring parsed type
  2, a factory override for rewritten records, a writer replacing `+0x98`
  before HeavenHold load, or a non-`HLD` style producing code 1.

## Unknowns

- The player-facing name of path-scalar value `1.0` and the `UP` extra-table
  value remain unresolved; their interfaces and consumers remain explicit.
- The local corpus does not exercise SLD/SLC/SXC with style `HLD`, although the
  exact parser and rewrite branches accept all four command forms.

## Consequences

- Ghidra mutations: none; GhidraMCP remained unavailable, so analysis used the
  temporary static project clone.
- Spec sections: `spec/c2s.md`, `spec/notes/slide.md`,
  `spec/notes/heaven_hold.md`.
- Reconstruction code: Slide style decoding, retype predicate, path-scalar
  constant, and extended-form HeavenHold profile selection.
- Tests: `tests/heaven_hold_judgement_test.cpp`.

## Verification

The parser style mapping, postprocessor order, rewrite loop, type-13 producer,
factory, and profile constructor were traced independently. Focused tests cover
exact/case-sensitive style decoding, code-10 normalization, both command-form
families, type selection, generated-queue selection, and profile pairs. Corpus
aggregation independently confirms that the exact local snapshot contains the
extended form.
