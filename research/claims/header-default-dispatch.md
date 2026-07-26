# Claim: group-0 headers reset to fixed defaults and apply in source order without gameplay version gates

- ID: `claim.parser.header-default-dispatch`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.header`
- Last reviewed: 2026-07-21

## Statement

Each gameplay parse resets the group-0 header state, then applies every
descriptor-resolved header record in source order. Implemented commands replace
the current field, so the last duplicate wins; recognized `RESOLUTION` is the
sole group-0 descriptor deliberately rejected by the handler and cannot replace
the fixed value 384. Stored VERSION triples do not select parser,
postprocessor, runtime-note, or judgement behavior on the recovered gameplay
path.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, group-0 descriptors and field types`
- `game.exe @ RAM:011c3990, FUN_011c3990, complete header reset`
- `game.exe @ RAM:011c7040, FUN_011c7040, parse-time owner reset`
- `game.exe @ RAM:011c7980, FUN_011c7980, source-order header pass and accepted-record accounting`
- `game.exe @ RAM:011c8410, FUN_011c8410, command-ID dispatch and handler writes`
- `game.exe @ RAM:011c8630, FUN_011c8630, VERSION raw-string and triple parse`
- `game.exe @ RAM:011d0c90, FUN_011d0c90, full and 20-line reader modes`
- `game.exe @ RAM:011d03f0, FUN_011d03f0, recognized-command append gate`
- `game.exe @ RAM:011cf810, FUN_011cf810, token split and command lookup`
- `game.exe @ RAM:011cf450, FUN_011cf450, float field access`
- `game.exe @ RAM:011cf6d0, FUN_011cf6d0, integer field access`
- `game.exe @ RAM:011cf760, FUN_011cf760, string field access`
- `game.exe @ RAM:011cfb40, FUN_011cfb40, strtof wrapper`
- `game.exe @ RAM:011cb670, FUN_011cb670, base-10 strtol wrapper`
- `game.exe @ RAM:00958bb0, FUN_00958bb0, header-only metadata caller`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, full gameplay caller and immediate consumers`
- `game.exe @ RAM:011ba710, FUN_011ba710, full-parse BPM_DEF storage replacement`
- `game.exe @ RAM:011bb0f0, FUN_011bb0f0, MET_DEF consumer`
- `game.exe @ RAM:011bc930, FUN_011bc930, CLK_DEF/CLK merge`
- `game.exe @ RAM:011bda60, FUN_011bda60, Air-generation header consumers`
- `game.exe @ RAM:00b283a0, FUN_00b283a0, CLK-derived click consumer`

## Observations

The reset and handler establish this exact group-0 map. BPM_DEF values below
are listed in authored token order even though the binary stores them in a
different internal field order.

| ID | Command | Reset state | Present-record action |
|---:|---|---|---|
| 0 | `VERSION` | two empty raw strings and two zero triples | store two raw strings; parse each as three decimal components |
| 1 | `MUSIC` | 0 | replace with integer field 1 |
| 2 | `SEQUENCEID` | 0 | replace with integer field 1 |
| 3 | `DIFFICULT` | empty | replace with string field 1 |
| 4 | `LEVEL` | 0.0 | replace with float field 1 |
| 5 | `CREATOR` | empty | replace with string field 1 |
| 6 | `BPM_DEF` | 150.0, 150.0, 150.0, 150.0 | replace all four float fields |
| 7 | `MET_DEF` | 4, 4 | replace both integer fields |
| 8 | `RESOLUTION` | 384 | return rejected; perform no write |
| 9 | `CLK_DEF` | 0 | replace with integer field 1 |
| 10 | `PROGJUDGE_BPM` | 240.0 | replace with float field 1 |
| 11 | `PROGJUDGE_AER` | float bits `0x3f7fbe77` | replace with float field 1 |
| 12 | `TUTORIAL` | false | replace with `integer field 1 > 0` |

- Tokenization retains recognized commands without an exact-arity rejection.
  Missing string fields read as empty; missing, empty, out-of-range-index, or
  descriptor-incompatible numeric fields read as zero. Extra fields are
  retained in the token record but ignored by these handlers.
- Nonempty compatible numeric fields call `strtof` or base-10 `strtol`. The
  wrappers reject no-conversion and range-error inputs with standard
  exceptions, but do not require full-string consumption; a valid numeric
  prefix followed by other characters is accepted. No catch exists in the
  recovered parser bridge or gameplay load wrapper.
- VERSION stores each raw string first, then scans with `%d.%d.%d`. Unless
  exactly three conversions succeed, all three components for that string are
  zeroed. A successful three-conversion prefix is sufficient; trailing content
  is not checked.
- The header pass walks the token records in source order and invokes the
  handler for every group-0 record. Scalar and string writes do not preserve a
  first-seen value, establishing last-wins duplicate behavior. ID 8 returns
  false and does not advance accepted-record accounting.
- Full gameplay mode reads the entire source, completes BPM and ordinary event
  passes, and then builds runtime notes. Mode 1 limits input to 20 lines and
  skips every later pass; its recovered caller uses CREATOR and authored
  BPM_DEF field 2 for metadata. It does not read VERSION.
- On the full parse, a nonempty BPM vector causes the BPM statistics
  postprocessor to replace all four internal BPM_DEF storage fields. It does
  not synthesize a BPM record from BPM_DEF. The gameplay loader instead reads
  finalized BPM records directly.
- VERSION components and the MUSIC, SEQUENCEID, DIFFICULT, LEVEL, and CREATOR
  fields are copied with chart state but are not read by the ordered gameplay
  postprocessors, runtime-note construction, or judgement path. No VERSION
  comparison or compatibility branch occurs in that closed path.
- MET_DEF is used only to synthesize missing position-zero meter/grid state.
  CLK_DEF seeds the merged CLK position vector. The gameplay loader converts
  that vector to scheduled click triggers; their update plays feedback and its
  return is ignored after note updates, with no result or note-state write.
- The four gameplay-affecting header interfaces are fixed resolution 384,
  PROGJUDGE_BPM as the Air cadence reference, PROGJUDGE_AER as the final
  AHD/ASC end-margin multiplier, and TUTORIAL as the enable for key-zero
  selector-zero open-interval exclusion in AirHold, AirSlide, and HeavenHold.
  This chart TUTORIAL flag is separate from the runtime tutorial callback that
  can force result modes.

## Reasoning

The reset-to-tokenize-to-source-order dispatch chain proves field lifetime and
duplicate precedence. Independent field accessor and conversion wrappers
separate absent/empty zero fallback from nonempty malformed exceptions.
Complete inspection of the gameplay parser/postprocessors plus the immediate
full-load and runtime-note path found consumers only for the four stated
gameplay interfaces. The header-only caller independently explains why
BPM_DEF and CREATOR are retained without implying gameplay use. Since neither
mode reads VERSION components and the full path has no version-dependent
dispatch, corpus resource-version variation does not select behavior in this
snapshot.

## Alternatives and falsifiers

- Competing explanation: headers are first-wins, RESOLUTION changes position
  normalization, BPM_DEF supplies a missing tempo map, VERSION selects legacy
  event behavior, or CLK triggers alter judgement.
- Evidence that would disprove this claim: a guarded header write that
  preserves an earlier duplicate; a live ID-8 handler case; a BPM record
  synthesized from BPM_DEF; a VERSION component read controlling a parser,
  factory, postprocessor, or checker branch; or a CLK consumer writing note or
  result state.

## Unknowns

- The outer application-level disposition of an uncaught numeric conversion
  exception is outside the recovered successful gameplay-load contract.
- No local chart duplicates group-0 headers, so last-wins behavior is static
  evidence rather than a corpus compatibility requirement.
- CLK feedback resource identities and the presentation meaning of generated
  meter grids are outside scope; their lack of judgement-state writes is the
  relevant boundary.
- VERSION behavior is closed for the recovered gameplay path, not asserted for
  unrelated editor/export or network code.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary project clone only.
- Spec sections: `spec/c2s.md`, `spec/timing.md`,
  `spec/configuration.md`, and Air-family note specifications.
- Reconstruction code: `C2sVersion`, typed field conversion,
  `C2sHeader`, and `apply_c2s_header_record` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/c2s_header_test.cpp`.

## Verification

- Focused tests cover every reset default, VERSION success/failure and trailing
  content, numeric-prefix acceptance, missing-field zero fallback, last-wins
  duplicates, extra-field tolerance, RESOLUTION rejection, TUTORIAL's positive
  predicate, and malformed/range exceptions.
- The local corpus contains 7,752 charts. Every chart has exactly one of each
  group-0 header, every declared RESOLUTION is 384, and all charts provide a
  position-zero BPM record. VERSION aggregates span older resource strings,
  while the exact binary follows one common parser/factory path.
- The header-only caller, full gameplay caller, Air postprocessor, meter
  postprocessor, CLK merge, and CLK runtime consumer were inspected as
  independent downstream paths.
