# Claim: HXD selects the extended HOLD checker profile

- ID: `claim.note.hold-extended-profile-selection`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `parser.compatibility`, `note.hold`, `judgement.windows`, `config.external`
- Last reviewed: 2026-07-27

## Statement

`HLD` and `HXD` both parse as record type 1, but HXD always sets an extended
form flag and decodes its optional sixth string through the exact eight-entry
subtype table. The extended flag changes HOLD's external judgement-checker
profile selector from zero to four; missing legacy subtype fields default to
index zero without clearing the extended form.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, HLD/HXD descriptors, hash 75169f04d17368b9a9e109d2139f11160db1abd0f64796fc9d59d9d467b1ec37`
- `game.exe @ RAM:011c8870, FUN_011c8870, type-1 form branch, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:011c5590, FUN_011c5590, optional subtype reader, hash d1267e6be742c16d30a9887fc3021df64dcea12c7b9e505b5aa209330bca23fc`
- `game.exe @ RAM:011cfed0, FUN_011cfed0, exact subtype lookup, hash 999a3648bfd8ee35088e6b68434f9b3a4d06467af4d45fe4160edabef93e71cc`
- `game.exe @ RAM:01be5e70 and RAM:019679f8, eight initialized subtype pointers and strings`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, common runtime copy, hash d2e5865736dbc63e4e92294cfabd119e14c26dfee62a143e57f90f77d955b449`
- `game.exe @ RAM:00c18800, FUN_00c18800, checker profile selection, hash 8f3ebb4a3fe05078f6c5b189ccc743cbcc70316400d353125b1c95ee34159c81`

## Observations

- HLD descriptor ID `0x19` has five integer arguments. HXD ID `0x1a` has the
  same five integers plus a string. Both resolve to parsed type 1.
- The type-1 parser writes a false extended flag for HLD. HXD writes true and
  reads field six through the optional string helper. The exact
  case-sensitive table order is `UP`, `DW`, `CE`, `RC`, `LC`, `RS`, `LS`,
  `BS`; missing, empty, and unknown values return index zero.
- Common runtime loading copies the extended flag and subtype code into the
  runtime note.
- Checker initialization starts from an executable-owned parsed-type selector
  table. For valid type 1 the ordinary selector is zero; an extended type 1
  overrides it with four before external judgement-window records are copied.
- The same override structure applies to extended parsed types 2 and 13.
  Parsed type 4 also selects four, type 6 selects six, type 11 selects eleven,
  and other valid types select zero.

## Reasoning

The descriptor distinction survives parsing and common runtime loading. The
checker initializer reads that same form flag before choosing the external
window-data block, so HXD is a gameplay compatibility form rather than only a
resource/style spelling. The subtype code itself is not used in this profile
decision.

## Alternatives and falsifiers

- Competing explanation: HXD differs from HLD only in visual assets.
- Evidence that would disprove this claim: the checker branch reading a
  different flag, or selector four resolving to the same external record by an
  executable-enforced alias rather than external data.

## Unknowns

- The actual selector-zero and selector-four window values are externally
  loaded and absent from the workspace.
- The player-facing names and presentation/resource consequences of the eight
  subtype codes are intentionally unassigned.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/c2s.md`, `spec/notes/hold.md`,
  `spec/configuration.md`.
- Reconstruction code: `C2sHoldCommandVariant`,
  `c2s_hxd_subtype_code`, and `note_checker_profile_selector`.
- Tests: `tests/hold_variant_test.cpp`.

## Verification

Focused tests cover all eight exact strings, case sensitivity, unknown and
missing defaults, HLD/HXD form distinction, legacy five-field HXD, every
special profile mapping, and invalid-type sentinels. Corpus aggregation found
36,010 HXD rows: 1,160 legacy five-argument rows and 34,850 six-argument rows
across versions 1.08 through 1.13.
