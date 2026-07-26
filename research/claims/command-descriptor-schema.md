# Claim: C2S commands are registered as 91 fixed descriptor records

- ID: `claim.parser.command-descriptor-schema`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.header`, `parser.events`
- Last reviewed: 2026-07-20

## Statement

The snapshot registers exactly 91 C2S command descriptors, with numeric IDs
zero through `0x5a`. Each contains a command string, descriptive string,
command group, total tab-field count, and up to twelve per-argument type
codes; negative type-code slots terminate the meaningful argument list.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, static descriptor initializer`
- `game.exe @ RAM:011cc1b0, FUN_011cc1b0, descriptor constructor called through thunk RAM:00439716`
- `game.exe @ RAM:011c7980, FUN_011c7980, descriptor-group consumer in parser passes`

## Observations

- `FUN_00528b60` constructs descriptor objects for sequential IDs `0x00` through
  `0x5a`, including observed strings such as VERSION, BPM_DEF, TAP, HLD, SLD,
  AIR, and FLK.
- Its descriptor-constructor thunk has exactly 91 call xrefs, all within this
  initializer. Exact fixed-array lookup is closed by
  `claim.parser.legacy-t-prog-command-exclusion`.
- `FUN_011cc1b0` stores the ID, two strings, group selector, and twelve signed
  codes. It computes the total field count as one command token plus the number
  of nonnegative codes.
- Examples independently align with corpus shapes: TAP has four argument codes
  and four corpus arguments; BPM_DEF has four; VERSION has two.
- The complete group-0 set is independently closed by
  `claim.parser.header-default-dispatch`: VERSION and metadata strings use the
  raw string accessor, MUSIC/SEQUENCEID/MET_DEF/RESOLUTION/CLK_DEF/TUTORIAL use
  integer-compatible fields, and LEVEL/BPM_DEF/PROGJUDGE fields use float-
  compatible fields.
- `FUN_011c7980` retrieves a descriptor-associated group and uses it to choose a
  parser pass/handler family.

## Reasoning

Constructor layout, multiple typed examples, and parser consumption jointly
establish an executable schema rather than a display-only command list. The
field-count computation explains why the command token contributes one beyond
the corpus argument arity.

## Alternatives and falsifiers

- Competing explanation: the signed values are formatter flags unrelated to
  accepted field types.
- Evidence that would disprove this claim: a field parser that does not consume
  these stored codes, or initialized records whose layout/count is unrelated to
  the command lookup and typed field accessors.

## Unknowns

- The semantic mapping for type-code values used outside the recovered header,
  SLA, and established event families is not yet closed.
- The separate legacy `T_PROG_*` name table has no executable reader; its
  historical non-gameplay purpose remains unassigned.

## Consequences

- Ghidra mutations: recovered the missing initializer function at
  `RAM:00528b60` and added a plate comment.
- Spec sections: structural note in `spec/c2s.md`.
- Reconstruction code: none until type-code and validation behavior are closed.
- Tests: corpus arity comparison performed read-only; no normative parser test.

## Verification

Header and SLA descriptors were traced through tokenization, descriptor-type
lookup, typed accessors, conversion wrappers, and their handler cases. The
complete constructor-xref and exact-lookup sets close the registry cardinality.
Event handler call sites independently establish that descriptor arity
validation is dormant on gameplay loads; descriptor types still govern
accessed fields.
