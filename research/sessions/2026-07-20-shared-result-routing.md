# Session 2026-07-20: shared result routing and alternate terminal meter

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`, `note.tap`, `note.hold`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close result dispatch from shared note submission through aggregate mutation,
terminal short-circuit, stored observer behavior, and the alternate terminal
predicate without expanding into scoring.

## Findings

- `research/claims/shared-result-routing.md`
- `research/claims/result-terminal-short-circuit.md`
- `research/claims/alternate-terminal-meter.md`
- `research/claims/hold-end-feedback-boundary.md`

The result observer was resolved through its constructor, only replacement
site, concrete callable, and captured progress object. The alternate terminal
predicate was separated into a configurable post-result meter with exact
selector, result-code fan-out, clamp, latch, and dispatch ordering.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Temporary function
creation and decompilation occurred only in `/tmp/chart-readonly-20260720`, a
disposable clone of the project; source artifacts were not changed.

## Validation

- Binary identity and workspace health checked with `harness.py doctor` at
  session start.
- CMake build completed.
- CTest passed 10/10 tests, including new shared-result observer cases and the
  alternate-meter test.
- `harness.py validate` pending the final coverage/session update.

## Unresolved and contradictions

- Initial “terminal callback” terminology was too strong: the same callable is
  used after ordinary mapped-category 0..4 result updates. The concrete
  installed target is bounded progress notification, so durable artifacts now
  call it a result observer.
- The player-facing meanings of result categories/codes, terminal summary
  bytes, alternate selector bytes, and meter units remain unknown.
- The producer and lifetime of alternate-meter auxiliary field `+0x48` remain
  open. External meter and contribution values remain parameters.
- GhidraMCP is not exposed and its configured HTTP endpoint is not listening,
  so justified names/comments could not be persisted to the live project.

## Handoff

For TAP follow-up, use shared checker initializer `RAM:00c18800`, candidate
gate `RAM:00c1af30`, and chart-load adjacency producer `RAM:011bccc0`. For
result-boundary follow-up, use dispatcher
`RAM:00b97730`, observer bridges `RAM:00b8e4a0`/`RAM:00b8e480`, installed
observer `RAM:00da4bf0`, alternate predicate `RAM:00c41520`, result delta
selector `RAM:00c41880`, and meter update `RAM:00c41700`. Active ownership
remains with codex-root for `pipeline.boundaries`, `judgement.types`,
`note.tap`, and `note.hold`.
