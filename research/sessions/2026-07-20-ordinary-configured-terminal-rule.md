# Session 2026-07-20: ordinary configured terminal rule

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: continuing `pipeline.boundaries` and
  `judgement.types`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not
  listening

## Goal

Close the ordinary summary evaluator's configured type-3 rule: recover every
condition, its exact boundary, the distinction between matching and requesting
termination, owner-state mutation order, population, consumption, and reset.

## Findings

- Added reconstructed claim
  `research/claims/ordinary-configured-terminal-rule.md`.
- The evaluator accepts only aggregate event type 1 and returns separate
  `matched` and `terminal_requested` values.
- Five ordered conditions cover an event-derived value floor, low event count,
  aggregate threshold, result-band cumulative weight, and a mode-2 zero gauge.
- The result-band condition can match and consume a record without requesting
  termination. Its unit gauge update runs before the zero-gauge condition, so
  an eligible gauge at one can become zero and request termination in the same
  call.
- The type-3 population path copies `0x60`-byte records into manager ownership;
  matched records are marked consumed and counted independently of the
  terminal-request output.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Functions and decompiler
state used for call-path recovery existed or were created only in
`/tmp/chart-readonly-20260720`; source artifacts were not modified.

## Validation

- `python3 scripts/harness.py validate` passed with 28 coverage rows.
- CMake build succeeded.
- CTest passed all 15 tests. `chart.shared_result` covers the event gate,
  zero/nonzero floor boundaries and NaN, strict/inclusive thresholds,
  band-only matching, cumulative termination, and the same-call gauge
  transition.
- Population/copy/reset, caller outputs, record consumption, all condition
  branches, result-count source, weight getters, and gauge mutation order were
  inspected independently.

## Unresolved and contradictions

- Source field names, configured values, defaults, units, and player-facing
  rule labels are unavailable and remain structural parameters.
- The semantic identities of the aggregate event fields and result weights
  remain intentionally unassigned beyond traced data flow.
- The prior working assumption that any matched type-3 record necessarily
  requests termination is false: the result-band path can match alone.

## Handoff

The configured rule is closed at caller `RAM:00b92640`, evaluator
`RAM:00b92ff0`, population `RAM:00b94ac0`, cumulative-weight getter
`RAM:00c409d0`, gauge updater `RAM:00b95170`, and consumption
`RAM:011f8c30`. The next claimed target is common `AHD`/`AHX` secondary parsed
type 5: start at factory `RAM:00b28cc0`, constructor `RAM:00c21020`, and vtable
`RAM:018d996c`. Continue ownership of `pipeline.boundaries`,
`judgement.types`, and `note.other_variants`.
