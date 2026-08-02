# Claim: gameplay event parsing bypasses descriptor arity validation

- ID: `claim.parser.event-token-fallback`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`
- Last reviewed: 2026-07-21

## Statement

Both gameplay callers of the group-2 event handler disable its descriptor-
arity validator. Recognized events therefore read missing numeric fields as
zero and ignore extra fields; nonempty accessed numeric fields accept a valid
prefix but propagate no-conversion or range exceptions. Unknown command names
are discarded before event dispatch.

## Anchors

- `game.exe @ RAM:011d03f0, FUN_011d03f0, recognized-command append gate`
- `game.exe @ RAM:011cf810, FUN_011cf810, token split and descriptor lookup`
- `game.exe @ RAM:011cf450, FUN_011cf450, float field accessor`
- `game.exe @ RAM:011cf6d0, FUN_011cf6d0, integer field accessor`
- `game.exe @ RAM:011cfb40, FUN_011cfb40, float conversion wrapper`
- `game.exe @ RAM:011cb670, FUN_011cb670, base-10 integer conversion wrapper`
- `game.exe @ RAM:011c75e0, FUN_011c75e0, dormant event arity/presence check`
- `game.exe @ RAM:011c78e0, FUN_011c78e0, secondary event-parse caller`
- `game.exe @ RAM:011c7980, FUN_011c7980, ordered gameplay parse caller`
- `game.exe @ RAM:011c8870, FUN_011c8870, group-2 typed-field consumers`

## Observations

- The line loader appends a token record only after its command string resolves
  to one of the initialized descriptors. An unknown spelling does not reach a
  group handler.
- This includes the four corpus-visible `T_FIRST_*`/`T_FINAL_*` summary
  spellings and all twenty `T_PROG_00` through `T_PROG_95` spellings, whose
  fixed-registry exclusion is closed by
  `claim.parser.legacy-metadata-command-exclusion`.
- The event handler has a field-count/presence helper, but both complete call
  sites pass its controlling argument as false. Each event-family branch then
  reads only the indices it uses; no live exact-arity rejection precedes those
  reads.
- An absent, empty, out-of-range-index, or descriptor-incompatible numeric
  field returns zero from the typed accessor. Extra retained tokens are inert
  unless a family branch explicitly requests their index.
- Compatible nonempty numeric fields use the same conversion wrappers as the
  reconstructed header parser. A valid numeric prefix is sufficient. No-
  conversion and range-error inputs throw, and no catch exists in the parser
  bridge or gameplay load wrapper.
- Family-specific association failures can still skip a record, such as a
  secondary command that cannot find its required root. The parser
  orchestration nevertheless returns success after tokenization.

## Reasoning

The closed caller set proves the event validator is dormant on gameplay loads,
while the per-branch field-access set establishes which retained fields can be
observed. The accessor/conversion chain distinguishes missing-field fallback
from malformed nonempty input. This is a shared ingestion rule; it does not
erase later family-specific matching or domain checks.

## Alternatives and falsifiers

- Competing explanation: descriptor arity is enforced during tokenization, or
  a malformed event merely causes that record to be skipped.
- Evidence that would disprove this claim: a tokenizer branch rejecting a
  recognized command solely for field count, a gameplay caller enabling the
  handler validator, or a catch converting numeric exceptions to record-local
  rejection.

## Unknowns

- The application-level presentation of an uncaught conversion exception is
  outside the successful gameplay-load boundary.
- Family-specific association, range, and compatibility failures are specified
  by their linked note/event claims.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP was unavailable and
  all inspection used the temporary static-analysis clone.
- Spec sections: `spec/c2s.md`.
- Reconstruction code: shared numeric accessors and the SLA record parser in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/c2s_header_test.cpp` and `tests/sla_region_test.cpp`.

## Verification

Focused tests independently cover missing numeric fields, ignored extras,
numeric-prefix acceptance, no-conversion exceptions, and range exceptions.
The two event-handler call sites were independently enumerated through the
handler thunk and both supply the disabled validation argument.
