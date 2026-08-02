# Claim: the fresh independent closure review found gameplay blockers

- ID: `claim.audit.independent-closure-review`
- State: superseded
- Maturity: verified
- Confidence: high
- Owner: closure_auditor
- Coverage rows: `audit.closure`
- Last reviewed: 2026-07-27

## Statement

A fresh read-only investigator independently audited the exact-binary
entry-to-exit gameplay chain and found that completion is not closed. It
reproduced one direct contradiction in the active SLA claim, two missing
gameplay producer paths (projection schedules and HXD profile selection), and
additional explicit parser, interaction, configuration, and control-flow gaps
that must be closed before an independent passing review is possible.

## Anchors

- `game.exe @ RAM:011c0e80 -> 00b29c90 -> 00da62c0, SLA tag assignment through materialization`
- `game.exe @ RAM:011c7d30 -> 011c6720/011c58f0 -> 00b29c90, projection schedule path`
- `game.exe @ RAM:011c8870 -> 00c1d1d0 -> 00c18800, HXD checker-profile path`
- `research/COVERAGE.tsv, completion-state inventory`
- `docs/COMPLETION.md, required gate definitions`

## Observations

- Binary identity, MCP health, corpus readability, harness validation, build,
  and the pre-audit 29-test suite all passed.
- The reviewer independently followed parsed SLA tag fields `+0x7c/+0x80`
  into the pending-record far predicate, disproving the active
  presentation-only conclusion.
- The reviewer recovered STP/SFL/SLP keyed interval production, DCM
  source-order factor selection, reset ownership, and their materialization
  consumers. These producers were absent from the clean-room API.
- The reviewer recovered HXD's extended flag, exact subtype table, runtime
  copy, and type-1 checker-profile override from selector zero to four. This
  compatibility branch was absent from HOLD specification/tests.
- The review also identified unresolved live group-3 command classification,
  cross-family creation/fanout ordering, copied event fields, Slide generated
  endpoint details, result-control lifetime/precedence, malformed BPM domains,
  materialization runtime-input producers, and remaining forced-mode producer
  closure.
- Every gameplay coverage row was still `mapped` or `investigating`; the
  completion contract explicitly rejects both states.

## Reasoning

The independent investigator used disjoint read-only work and reported exact
anchors and falsifiable paths. The primary investigator then reproduced the
three concrete behavior findings in the live Ghidra project and added focused
clean-room tests. Because the remaining gate gaps are still explicit and no
post-fix independent audit has passed, the only supported closure verdict is
not closed.

## Alternatives and falsifiers

- Competing explanation: the reported gaps are bookkeeping only and do not
  alter gameplay.
- Evidence that disproves that explanation: both the SLA schedule path and HXD
  profile branch can change when or under which external windows a note is
  judged.
- Evidence that would supersede this claim: closing every recorded gap,
  promoting every coverage row under the completion rules, and obtaining a new
  independent contradiction review with no residual blocker.

## Unknowns

- The exact outcomes of the remaining open investigations are not predicted.
- A future clean audit must be performed after, not during, their closure.

## Consequences

- Ghidra mutations: none by the independent reviewer.
- Superseded by: `claim.audit.independent-post-fix-closure-review`, which records
  the required fresh review after every blocker in this report was corrected.
- Supersedes: `claim.audit.adversarial-saturation-review`.
- Corrective claims:
  `claim.parser.sla-materialization-selection`,
  `claim.timing.projection-schedule-materialization`, and
  `claim.note.hold-extended-profile-selection`.
- Spec sections: all completion gates remain governed by
  `docs/COMPLETION.md`.
- Reconstruction code: the three reproduced findings now have focused
  clean-room helpers; other gaps remain open.
- Tests: the baseline 29 passed; corrective tests increase the current suite
  to 31.

## Verification

The reviewer supplied a gate-by-gate `NOT CLOSED` verdict and exact function
hashes. The primary investigator independently re-decompiled the relevant
functions, read the initialized HXD table, recovered the missed schedule
comparator boundary, and reproduced the behavior with synthetic tests. This is
a valid independent failure report, not a passing closure certificate.
