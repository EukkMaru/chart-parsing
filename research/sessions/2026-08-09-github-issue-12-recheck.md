# Session 2026-08-09: GitHub issue 12 recheck

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: none; documentation audit only
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Determine whether GitHub issue 12 adds a new question or changes any conclusion
from the complete twelve-issue verification pass, and report the current grand-
goal progress without treating issue state as an evidence grade.

## Findings

- Issue 12 is unchanged since 2026-08-07 08:00:13 UTC and has no comments. It
  is the same issue already included in the twelve-issue pass.
- Every requested check still has a durable exact-binary answer:
  sustain endpoint SLA keys and Slide post-shrink endpoint width are covered by
  `claim.presentation.sustain-endpoint-sla-selection`; Air-family vertical
  transforms and direction mirror signs by
  `claim.presentation.common-air-transform`; MET unit/count arithmetic by
  `claim.timing.tempo-measure-schedule`; and DCM query scope by
  `claim.timing.projection-schedule-materialization`.
- The old universal authored-value-5 Air-height premise and the old
  interval-relative/alternate DCM interpretation remain rejected.
- Remaining camera, resource-geometry, replacement-glyph, result-state, and
  HeavenHold/type-13 work is already represented by stage-two viewer rows. It
  does not reopen the seven questions in issue 12.

## Ghidra mutations

None. The issue and active claims were audited read-only; no new binary question
required a new symbol, type, comment, bookmark, or decompilation pass.

## Validation

- `python3 scripts/harness.py doctor`: exact binary identity, local project,
  corpus, tooling, socket, and bridge configuration passed.
- Live Ghidra MCP listed connected project `chart` with current program
  `game.exe` open.
- `python3 scripts/harness.py validate`: passed after documentation edits with
  28 coverage rows and 15 required files.
- `python3 scripts/harness.py next --count 10`: only stage-one
  `audit.closure` was suggested.
- `cmake --build build -j2`: all targets built successfully.
- `ctest --test-dir build --output-on-failure`: 35/35 tests passed.

## Unresolved and contradictions

- No new contradiction was found in issue 12.
- Stage one still requires the fresh independent contradiction audit after the
  corrected ALD findings.
- Stage two remains far from closure: most presentation rows are open or
  partial, product verification is not complete, the zero-ambiguity visual
  suite has not been accepted, and owner review is pending.

## Handoff

Do not reinvestigate issue 12 unless its body/comments change or a new exact-
binary contradiction appears. Continue from `research/VIEWER_COVERAGE.tsv`,
preferably with a bounded HeavenHold/type-13, shared camera, parser numeric-
failure, or seek-equivalence slice. No coverage or Ghidra write ownership is
active.
