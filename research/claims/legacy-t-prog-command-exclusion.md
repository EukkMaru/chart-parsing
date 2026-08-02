# Claim: legacy T_PROG commands are discarded before parser dispatch

- ID: `claim.parser.legacy-t-prog-command-exclusion`
- State: superseded
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `parser.compatibility`
- Last reviewed: 2026-07-27

## Statement

This claim is superseded by
`claim.parser.legacy-metadata-command-exclusion`. Its exact-lookup and
`T_PROG_*` rejection findings remain correct, but its corpus-vocabulary
conclusion was incomplete: four `T_FIRST_*`/`T_FINAL_*` spellings are rejected
by the same path, while registered SFE, ASO, HHD, and HHX are absent from the
corpus.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, complete descriptor initializer,
  hash 75169f04d17368b9a9e109d2139f11160db1abd0f64796fc9d59d9d467b1ec37`
- `game.exe @ RAM:00439716, thunk_FUN_011cc1b0, complete 91-call constructor
  xref set`
- `game.exe @ RAM:011cc2f0, FUN_011cc2f0, exact descriptor lookup,
  hash 256b6ea7e7d1b56390577d718c755968da9f464d819e8c6e6338cccf86cf90d9`
- `game.exe @ RAM:011cf810, FUN_011cf810, token split and lookup,
  hash 86db74905c2b19ecc28c9c36dc290bb0205d46c488e4ea9672dfcef000ca4776`
- `game.exe @ RAM:011d03f0, FUN_011d03f0, recognized-command append gate,
  hash 37222d0f36bf9ce57ea43a31e90c69a79072ddd05701758a43c0156a1ee18202`
- `game.exe @ RAM:01968094 through RAM:01968178, twenty legacy progress
  strings in a separate initialized pointer table`

## Observations

- The descriptor initializer makes 91 consecutive constructor calls. The
  first ID is zero, the last is `0x5a`, and the descriptor constructor's
  complete thunk-xref set contains only those calls.
- The lookup scans the same 91 fixed-size records. It compares both byte
  content and length, returns the matching zero-based index, and returns
  negative one after the final entry. There is no prefix, wildcard,
  case-folding, or VERSION-dependent fallback.
- The line loader appends a token record only when lookup succeeds and the
  returned ID is below 91. A failed lookup therefore cannot reach the later
  group-3 storage branch or any gameplay event handler.
- The executable contains all twenty `T_PROG_*` literals in a separate
  initialized pointer table. Their only direct references are from that data
  table; the complete instruction search found no executable reference to the
  table or literals. Literal presence is not descriptor registration.
- Aggregate corpus inventory finds all twenty commands exactly once in each
  of the 7,752 local charts. The original inference that the other 91 corpus
  spellings were all registered was false and is corrected by the superseding
  claim.

## Reasoning

The complete descriptor-constructor set bounds the only live registry, and
the exact fixed-length lookup bounds how command tokens enter the parser.
Because the append gate rejects the negative lookup result, no later parser
or gameplay owner can observe the legacy progress values. This narrower
reasoning remains valid; the superseding claim applies it to the complete
24-spelling rejected corpus set.

## Alternatives and falsifiers

- Competing explanation: `T_PROG_*` is accepted through a patterned lookup,
  dynamically registered after startup, or handled before descriptor lookup.
- Evidence that would disprove this claim: a constructor call outside the
  recovered 91-call initializer, a lookup branch that recognizes one of the
  twenty names, a pre-lookup handler, or a token append on negative lookup.

## Unknowns

- The historical producer and intended editor-facing meaning of the legacy
  progress values are outside this exact binary's gameplay ingestion path.
- The separate initialized name table's former or external consumer is
  unassigned because it has no executable reference in this snapshot.

## Consequences

- Ghidra mutations: recovered the missing function at `RAM:00528b60`; added
  plate comments at `RAM:00528b60` and `RAM:011cc2f0`.
- Superseded by: `claim.parser.legacy-metadata-command-exclusion`.
- Spec sections: corrected in `spec/c2s.md`.
- Reconstruction code and tests: replaced by the complete 24-command inventory
  in the superseding claim.

## Verification

The initializer was recovered in the live Ghidra project after a successful
dry run. Its complete decompilation, constructor xrefs, fixed lookup bounds,
token append gate, literal/table references, whole-program instruction search,
and aggregate corpus vocabulary were checked independently. Focused tests
cover every excluded spelling and nearby nonmembers.
