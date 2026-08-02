# Claim: all 24 unregistered corpus metadata spellings are discarded before dispatch

- ID: `claim.parser.legacy-metadata-command-exclusion`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `parser.compatibility`
- Last reviewed: 2026-07-27

## Statement

The corpus contains 87 of the executable's 91 exact descriptor spellings plus
24 backward-compatible metadata spellings absent from that registry. Exact
lookup rejects all 24 and the line loader discards them before header, timing,
event, or group-3 dispatch, so none can affect gameplay in this snapshot.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, complete 91-descriptor initializer, hash 75169f04d17368b9a9e109d2139f11160db1abd0f64796fc9d59d9d467b1ec37`
- `game.exe @ RAM:011cc2f0, FUN_011cc2f0, exact descriptor lookup, hash 256b6ea7e7d1b56390577d718c755968da9f464d819e8c6e6338cccf86cf90d9`
- `game.exe @ RAM:011cf810, FUN_011cf810, token split and lookup, hash 86db74905c2b19ecc28c9c36dc290bb0205d46c488e4ea9672dfcef000ca4776`
- `game.exe @ RAM:011d03f0, FUN_011d03f0, recognized-command append gate, hash 37222d0f36bf9ce57ea43a31e90c69a79072ddd05701758a43c0156a1ee18202`
- `game.exe @ RAM:01968054 through RAM:01968178, contiguous rejected-command literal block`

## Observations

- The descriptor initializer constructs IDs zero through `0x5a`. Lookup scans
  those 91 records with exact content and length and returns negative one after
  the last entry; there is no pattern, prefix, case-fold, or VERSION fallback.
- The line loader appends only a nonnegative descriptor result below 91. A
  rejected spelling is absent from every later parser pass.
- The 24 rejected corpus spellings are four boundary summaries
  `T_FIRST_MSEC`, `T_FIRST_RES`, `T_FINAL_MSEC`, and `T_FINAL_RES`, followed by
  twenty five-step progress names `T_PROG_00` through `T_PROG_95`.
- All 24 literals are present contiguously in executable data, but no function
  references the four boundary-summary strings and the separate legacy literal
  storage is not descriptor registration.
- Each rejected spelling occurs exactly once in each of 7,752 readable charts.
  The corpus's other 87 spellings are registered. The four registered but
  corpus-absent commands are SFE, ASO, HHD, and HHX; their parser/runtime
  behavior is established structurally with synthetic tests where applicable.

## Reasoning

Complete registry construction and exact lookup bound every possible accepted
name. Because negative lookup dominates the append gate, literal presence and
corpus frequency cannot create a hidden compatibility parser. The exact
87-plus-24 inventory reconciles corpus cardinality without treating four
unregistered boundary summaries as group-3 values.

## Alternatives and falsifiers

- Competing explanation: the four boundary summaries or `T_PROG_*` names are
  accepted by a pre-lookup or patterned compatibility handler.
- Evidence that would disprove this claim: a token append on negative lookup,
  a second descriptor registry, a pre-lookup handler, or any one of the 24
  names among the 91 constructor arguments.

## Unknowns

- The historical producer and editor-facing meaning of the ignored metadata is
  outside the exact snapshot's ingestion path.

## Consequences

- Ghidra mutations: none; the descriptor and lookup annotations already
  describe exact registration.
- Supersedes: `claim.parser.legacy-t-prog-command-exclusion`.
- Spec sections: `spec/c2s.md`.
- Reconstruction code: `ignored_legacy_metadata_commands` and
  `c2s_command_is_ignored_legacy_metadata`.
- Tests: `tests/c2s_header_test.cpp` and
  `tests/derived_command_test.cpp`.

## Verification

The complete initializer, lookup, and append gate were rechecked against the
four additional literals. Aggregate corpus scans count each of the 24 rejected
names 7,752 times and identify the four registered absent names. Focused tests
cover all rejected spellings, near misses, case sensitivity, and non-entry into
group-3 storage.
