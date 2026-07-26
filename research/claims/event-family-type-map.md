# Claim: event command families map to fixed parsed-record types

- ID: `claim.parser.event-family-type-map`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `note.tap`, `note.slide`, `note.other_variants`
- Last reviewed: 2026-07-26

## Statement

The snapshot maps its 25 named note-event command families to a fixed internal
parsed-record type before runtime object construction; in particular, `TAP`
maps to type 0.

## Anchors

- `game.exe @ RAM:011cc7d0, FUN_011cc7d0, event-family string lookup,
  hash 0cca4d56663e04cd69a898ced86737703d81efd233c5bfa99b419706072514e6`
- `game.exe @ RAM:011cc970, FUN_011cc970, family-enum to parsed type lookup,
  hash 8ea356716fa7783d3ddb035c138ab7464d5ffb4591670e3044967a166718863e`
- `game.exe @ RAM:011d02f0, FUN_011d02f0, composed string-to-type lookup,
  hash 7aafd0bea66d2719236cfca1c4ca9e1a8a3f597d342e594503b9585aac157961`
- `game.exe @ RAM:01be5c7c, initialized family table`
- `game.exe @ RAM:011c8870, FUN_011c8870, group-2 event parser consumer,
  hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`

## Observations

- The string lookup searches 26 initialized records and returns the record
  index, or negative one when no string matches.
- The type lookup uses the same index and reads a signed type value from the
  initialized record. Index 0 is a sentinel with type negative one.
- The non-sentinel map is:

  | Commands | Parsed type |
  | --- | ---: |
  | TAP | 0 |
  | HLD, HXD | 1 |
  | SLD, SXD, SLC, SXC | 2 |
  | AIR, AUR, AUL, ADW, ADR, ADL | 3 |
  | CHR | 4 |
  | AHD, AHX | 5 |
  | FLK | 6 |
  | ASD, ASC | 8 |
  | ALD | 9 |
  | ASO | 10 |
  | MNE | 11 |
  | SLA | 12 |
  | HHD, HHX | 13 |

- The group-2 event handler stores the resolved value in the parsed record's
  type field. Its simple path includes types 0, 4, 6, and 11.
- An independent full-switch audit found handler cases for exactly types 0, 1,
  2, 3, 4, 5, 6, 8, 9, 10, 11, 12, and 13. This equals the initialized map's
  complete nonnegative range; type 7 and every other value have no hidden
  family branch.
- Type 12 (`SLA`) is intentionally absent from the runtime-note factory. It is
  a region directive whose tag propagation is reconstructed by
  `claim.parser.sla-region-selection`.

## Reasoning

The shared initialized record supplies both lookup stages, and the parser
stores their result in the field later switched on by the runtime object
factory. This connects command spelling to gameplay record type rather than
only to display metadata.

## Alternatives and falsifiers

- Competing explanation: the mapped value selects a parser helper but is
  replaced before runtime construction.
- Evidence that would disprove this claim: a data-flow path overwriting the
  stored type before the factory, or a factory switch reading a different
  field.

## Unknowns

- The index-0 sentinel spelling has no gameplay interpretation assigned.
- The corpus has no observed `ASO`, `HHD`, or `HHX` lines; binary support is
  established independently of corpus occurrence.
- Some mapped families share a parsed type but can still diverge through other
  fields or later subtyping.

## Consequences

- Ghidra mutations: added a complete parsed-type case-set plate comment at
  `RAM:011c8870`.
- Spec sections: `spec/c2s.md`, `spec/notes/tap.md`, `spec/notes/slide.md`.
- Reconstruction code: type-12 region parsing/selection is implemented; the
  full map is not yet a complete parser API.
- Tests: `tests/sla_region_test.cpp` covers the type-12 directive boundary.

## Verification

The string lookup, 26-entry enum/type table, composed lookup, and complete
group-2 handler switch were independently rewalked in the live Ghidra project.
The type-0 path was separately followed into the runtime factory and TAP RTTI,
as recorded by `claim.note.tap-construction`.
