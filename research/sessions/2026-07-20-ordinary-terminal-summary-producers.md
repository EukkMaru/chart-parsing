# Session 2026-07-20: ordinary terminal-summary producers

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: continuing `pipeline.boundaries` and
  `judgement.types`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not
  listening

## Goal

Close every ordinary producer of the two summary bytes that latch shared
terminal routing, including producer precedence, owner/reset lifetime, the
suppression global, and the runtime end metric, without reconstructing unrelated
scoring effects.

## Findings

- Added reconstructed claim
  `research/claims/ordinary-terminal-summary-producers.md`.
- A matched configured type-3 rule can separately request termination. That
  request zeros the contribution and sets only the primary summary byte.
- Only while primary is clear can the later end-threshold producer run. It
  zeros the contribution and sets both bytes, so the configured-rule request
  has primary-only precedence when both predicates would otherwise apply.
- The result owner latches summaries at `+0x2a8/+0x2a9` and explicitly clears
  both during reset. A separate external setter can replace primary.
- The end producer reads a separately reset and scaled aggregate metric. Its
  mode selector and external threshold tables remain structural parameters.
- The executable-owned suppression byte is initialized zero. Apparent code-gap
  references are padding, and no applicable nonzero writer was recovered.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. The temporary project
clone was used for static inspection only; source artifacts were not modified.

## Validation

- `python3 scripts/harness.py validate` passed with 28 coverage rows.
- CMake build succeeded.
- CTest passed all 15 tests. `chart.shared_result` now covers no-terminal,
  configured-rule, end-threshold, and simultaneous-predicate precedence.
- Rule-vector construction/reset, record evaluation, both producer branches,
  summary consumption, owner reset/setter, metric reset/update/getter, and all
  direct suppression-byte references were inspected separately.

## Unresolved and contradictions

- The configured rule fields, player-facing flag names, end-selector virtual
  target, external threshold values, and aggregate metric name are unresolved.
- A computed-address or untraced external suppression-byte write remains a
  falsifier; no such write is supported by direct references or resolved calls.
- The end-threshold producer did not contradict primary-only rule termination:
  its explicit primary-clear gate explains the observed distinction.

## Handoff

Resume at evaluator `RAM:00b92640`, rule evaluator `RAM:00b92ff0`, end-selector
dispatch `RAM:00b61ad0`, metric getter `RAM:00b8ef70`, metric update
`RAM:00b901a0`, owner summary consumption `RAM:00b96b30`, reset
`RAM:00b95f80`, and suppression byte `RAM:01c78bc4`. The highest-value next
question is the concrete virtual target selected through `RAM:00b61ad0` and
the structural meaning of its two end-threshold branches. Continue ownership
of `pipeline.boundaries` and `judgement.types`.
