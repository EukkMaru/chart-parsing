# Evidence and reconstruction standard

## Claim maturity

| Level | Meaning | Permitted consequence |
|---|---|---|
| Observed | Directly located data, xref, call, field access, or branch | Record anchor; no semantic rename |
| Supported | Multiple compatible observations establish a likely role | Careful rename/comment; retain uncertainties |
| Reconstructed | All relevant paths form a clean-room algorithm | Add specification and provisional C++ |
| Verified | Independent evidence or a focused test confirms the rule | Mark coverage verified |

Confidence (`low`, `medium`, `high`) is separate from maturity. A precisely
observed call may still have low semantic confidence.

## Required anatomy of a claim

Every material claim records:

- one falsifiable statement;
- stable anchors: program name, address/function, and preferably function hash;
- upstream/downstream context and relevant state;
- observations, then reasoning as a separate section;
- alternatives considered and what would falsify the claim;
- unknowns, especially external configuration values;
- impacted coverage rows/spec/tests;
- Ghidra mutations made.

Use `research/templates/CLAIM.md`.

## Stable references

Absolute addresses are valid for this exact binary but fragile as the database
changes. Pair an address with the current function name, call relationship, and
hash when MCP provides one. Example notation:

```text
game.exe @ RAM:00abcdef, CurrentFunctionName, called by X, hash <value>
```

Never use a renamed symbol alone as evidence; the name is an interpretation.

## Clean-room boundary

Repository artifacts may contain concise facts, identifiers, constants,
structure layouts, algorithms in original prose, reconstructed equations, and
human-written C++ implementing confirmed behavior. They must not contain:

- raw assembly instruction sequences;
- copied decompiler output or mechanically reformatted pseudocode;
- binary blobs, copied charts, or large string/table dumps;
- long verbatim game-code equivalents presented without derivation.

Synthetic test fixtures must be authored from the specification and kept
minimal. Corpus-wide scripts may compute counts/invariants without copying
records into the repository.

## Contradictions

Never silently edit away a contradicted claim. Mark it `superseded`, link the
new claim, explain the failed assumption, and revisit every dependent spec,
Ghidra annotation, coverage row, and test.
