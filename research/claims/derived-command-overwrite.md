# Claim: all live group-3 authored values are overwritten before gameplay setup

- ID: `claim.parser.derived-command-overwrite`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `parser.compatibility`, `pipeline.boundaries`, `state.ownership`
- Last reviewed: 2026-07-27

## Statement

Descriptor IDs `0x2e` through `0x5a` are 45 live group-3 commands with one
integer field. Ordinary parsing temporarily stores the last accepted value in
the ID-indexed parser range `+0x174..+0x224`, but the mandatory derived-summary
pass clears that complete range without reading it and rebuilds unrelated
chart-derived statistics before parsing returns. Authored group-3 values
therefore cannot select gameplay in this snapshot, although malformed accessed
integers can still escape through the load chain.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, IDs 0x2e..0x5a descriptors, hash 75169f04d17368b9a9e109d2139f11160db1abd0f64796fc9d59d9d467b1ec37`
- `game.exe @ RAM:011c7980, FUN_011c7980, group-3 indexed write and mandatory postparse call, hash b72ab41d5af91578014761c4671a625a0505d482c4cb2271ecd6e5563932da34`
- `game.exe @ RAM:011c1ce0, FUN_011c1ce0, destructive summary rebuild, hash 981d1303f51b9757d197a4c4ec32789d10ca234dc3aa174d7f1815eee13fdc77`
- `game.exe @ RAM:011c7040, FUN_011c7040, parser reset, hash 85e469f01af6c35d128516dd25bdab6d72eabe497b619db19768a06671a8304c`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, successful gameplay load caller`

## Observations

- Every group-3 descriptor has one integer-compatible argument. In exact ID
  order the command families are:
  - `T_REC_` and `T_NOTE_` with suffixes TAP, CHR, FLK, MNE, HLD, SLD, AIR,
    AHD, ALL;
  - `T_NUM_` with TAP, CHR, FLK, MNE, HLD, SLD, AIR, AHD, AAC;
  - `T_CHRTYPE_` with UP, DW, CE, RC, LC, RS, LS, BS;
  - `T_LEN_` with HLD, SLD, AHD, ALL; and
  - `T_JUDGE_` with TAP, HLD, SLD, AIR, FLK, ALL.
- The main parser switch subtracts `0x2e` from the descriptor ID, accepts only
  indices below 45, reads integer field one through the shared accessor, and
  writes parser integer slot `0x5d + index`. Source-order duplicates replace
  the earlier temporary value.
- Missing, empty, or incompatible fields become zero. Numeric prefixes are
  accepted; no-conversion and range failures escape. Extras are ignored.
- Chart reset zeros these 45 slots. More importantly, the ordinary parser
  unconditionally calls `FUN_011c1ce0` after all event postprocessing.
- `FUN_011c1ce0` begins by zeroing exactly 45 integers at
  `+0x174..+0x224` and fourteen following summary integers. It does not read an
  authored slot before the clear. It then derives family counts, lengths,
  subtype counts, aggregate counts, horizons, and distributions solely from
  finalized parsed records.
- The successful gameplay-load path later invokes the same summary rebuild
  again. Existing producer/consumer closure shows these derived values do not
  feed runtime-note setup or judgement; gameplay scans the parsed-record
  vector independently.
- Header-only mode skips the group-3 pass and the later summary work entirely.

## Reasoning

The temporary group-3 destination and the summary producer's initial clear are
the same exact storage range. The clear dominates parser return and has no
read-before-write path, so no authored value survives to any parser consumer.
This is stronger than a lack of observed gameplay reads: the data is
destructively replaced before the gameplay caller regains control.

## Alternatives and falsifiers

- Competing explanation: group-3 values seed or constrain the derived summary.
- Evidence that would disprove this claim: any read of `+0x174..+0x224`
  between the group-3 write and initial summary clear, a conditional path
  around `FUN_011c1ce0` in ordinary parsing, or a summary computation that
  consults the pre-clear value.

## Unknowns

- The historical/editor-facing meaning of authored totals is outside the
  exact gameplay path.
- Other builds may have consumed these values; this claim is snapshot-specific.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/c2s.md`.
- Related boundary:
  `claim.pipeline.parser-derived-summary-boundary`.
- Reconstruction code: `C2sDerivedCommandStorage`, exact command inventory,
  temporary application, and destructive summary-rebuild start.
- Tests: `tests/derived_command_test.cpp`.

## Verification

Focused tests cover all 45 exact names and order, the `T_NUM_AAC` exception,
nearby nonmembers, duplicate replacement, missing/default, numeric-prefix,
malformed, extra-field, and destructive-clear behavior. Corpus aggregation
found every live name with exactly one argument; most occur once in all 7,752
charts, with expected older-chart absence for MNE and current-schema
chart-type variants.
