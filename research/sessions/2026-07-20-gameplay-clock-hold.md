# Session 2026-07-20: gameplay clock and HOLD vertical slice

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`, `note.tap`,
  `note.hold`
- Binary hash checked: yes
- MCP health checked: yes; configured endpoint was not listening and no
  GhidraMCP tools were exposed

## Goal

Establish a credible gameplay-time boundary without recursing from PE entry,
finish the ordinary TAP lifecycle baseline, and follow parsed type 1 through
HOLD construction, start judgement, physical-source sustain, checkpoint
classification, and terminal transition.

## Findings

- Reconstructed authoritative clock and catch-up behavior:
  `research/claims/gameplay-clock-reconstruction.md` and
  `research/claims/gameplay-substep-order.md`.
- Closed TAP deferred transition/candidate ordering:
  `research/claims/tap-deferred-terminal-candidate.md`.
- Identified runtime `projView::HoldNote` and its shared TAP start gate:
  `research/claims/hold-construction-start-gate.md`.
- Reconstructed the HOLD two-bank release/continuation-marker rule:
  `research/claims/hold-source-continuation.md`.
- Reconstructed HOLD inactive-gap thresholds, one-checkpoint-per-substep
  behavior, post-checkpoint reset, empty-vector behavior, and two-phase
  terminal condition: `research/claims/hold-sustain-gap-lifecycle.md`.
- Reconstructed manager forced-result fixed/cycling/random selection and its
  TAP/HOLD consumers: `research/claims/forced-result-mode.md`.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Static inspection used a
temporary project clone under `/tmp`; temporary function creation there did not
mutate the source project or workspace evidence.

## Validation

- Initial `python3 scripts/harness.py doctor` confirmed the expected binary
  SHA-256, project, corpus, and local tools; it also reported the unavailable
  MCP endpoint and existing project lock.
- Final `python3 scripts/harness.py validate`: 28 coverage rows and 10 required
  files, validation OK.
- CMake build completed for eight focused test executables.
- Final CTest run: 8/8 passed, including new play-clock, HOLD sustain, and
  forced-result tests.
- Instruction-level checks independently verified HOLD RTTI/vtable dispatch,
  distinct input snapshot fields, gap comparison directions, checkpoint front
  removal, forced-result jump tables, and requested/current terminal ordering.

## Unresolved and contradictions

- No live Ghidra names, comments, types, or bookmarks could be persisted because
  the requested MCP connection was unavailable. Confidence-gated annotations
  remain a future live-project task.
- HOLD's once-only end-bound grade calls `RAM:00c1b800`, which selects indexed
  resources and dispatches indirect object methods rather than calling the HOLD
  result wrapper directly. Those indirect targets are not closed enough to
  state that the path is presentation-only.
- The manager forced-result selector is exact, but its external producer,
  player-facing mode names, activation lifetime, and random seed remain open.
- External judgement-window and HOLD gap values are absent. Interfaces and
  consumers are recovered; values remain explicit parameters.
- Several decompiler outputs had incorrect argument counts or register
  propagation. Claims use checked calling instructions and independent
  consumers where those discrepancies mattered.

## Handoff

Keep the four claimed coverage rows owned by `codex-root`. The highest-value
next boundary is shared result ownership: start at `RAM:00c1c340`, distinguish
its gameplay result sinks from media/resource calls, and compare them with the
HOLD end helper at `RAM:00c1b800`. For HOLD itself, use `RAM:00c29c50` for the
once-only end branch and `RAM:00b29570` for forced selection. Do not reopen the
gap tracker unless a downstream consumer contradicts the documented anonymous
byte mapping.
