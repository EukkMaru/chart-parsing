# Resume the chart gameplay reverse-engineering project

Work autonomously in `/home/maru/personal/chart-parsing` until the available
session ends or a genuine external blocker prevents all useful in-scope work.
Do not stop after orientation, a superficial function summary, or one bounded
finding. Do not ask me to choose routine research targets.

## Mandatory startup

1. Read `AGENTS.md` completely.
2. Read every document it requires, in the specified order.
3. Read the current handoff:
   `research/sessions/2026-07-26-outer-parser-timing-input-closure.md`.
4. Run:

   ```bash
   python3 scripts/harness.py doctor
   python3 scripts/harness.py validate
   python3 scripts/harness.py next
   ```

5. Connect to the existing Ghidra `chart` project and select the open
   `game.exe` program through the native `ghidra-mcp` tools.

GhidraMCP 5.15.0 is already registered globally in Codex through the local
stdio bridge. Port 8089 is the Ghidra plugin backend, not a streamable MCP
`/mcp` endpoint. If native Ghidra tools are absent, confirm this is a fresh
Codex session and consult `docs/GHIDRA.md`; do not mistake a sandboxed TCP
probe for proof that the live Unix-socket server is down.

## Current starting point

No coverage row is owned. All 28 rows are mapped, but the grand goal remains
open because the independent contradiction gate is not satisfied. Claim
`audit.closure` in
`research/COVERAGE.tsv` before making Ghidra mutations.

The first task is a genuinely fresh independent saturation and contradiction
audit. Use `research/claims/adversarial-saturation-review.md` only as an
inventory, not as proof. Repeat the `docs/COMPLETION.md` entry-to-exit,
indirect-call, state ownership/reset, cross-note interaction, corpus
compatibility, supersession/reference, spec, reconstruction, and test checks.
Reopen a mapped row only for a concrete contradiction.

The previous same-investigator pass closed the outer-update direct callees,
91-entry live command vocabulary versus 20 unused legacy spellings, music
cursor/timer and AutoScan counter domains, TouchSlider/photo-sensor framing,
and complete input snapshot/profile synthesis. Their claims and exact anchors
are linked by the latest handoff and `research/STATUS.md`. Do not treat their
mapped status as independent confirmation.

## Required research standard

- Static analysis only.
- Never execute `game.exe`, use Wine, attach a debugger, or emulate the whole
  program.
- Do not run Git commands or modify Git state.
- Do not copy raw assembly, decompiler dumps, binary data, or real chart
  records into workspace files.
- Public/community information is only a lead; confirm behavior in this exact
  binary.
- Do not guess unavailable external constants. Recover their interfaces,
  selection, lifetime, and consumers, then parameterize missing values.
- Apply Ghidra names, comments, types, structures, and enums only through MCP
  and only after satisfying the confidence gate in `AGENTS.md`.
- Preserve superseded claims as contradiction history. Never cite them as
  active coverage evidence.
- Do not broaden into rendering, UI, accounts, skills, or scoring unless a
  traced dependency can change gameplay generation or judgement.

For every target, trace both upstream and downstream behavior, make falsifiable
evidence-backed claims, update the clean-room spec, implement reconstructed C++
and focused tests when evidence is sufficient, and run applicable validation,
builds, and tests.

## Durable state and session end

Keep these current throughout the work:

- `research/STATUS.md`
- `research/COVERAGE.tsv`
- `research/claims/`
- `research/sessions/`
- `spec/`
- `include/chart/reconstruction.hpp`
- relevant tests

Before ending a session:

1. Run `python3 scripts/harness.py validate`.
2. Run the applicable CMake build and tests.
3. Update coverage and release or explicitly advance ownership.
4. Update `research/STATUS.md`.
5. Write a dated handoff using `research/templates/SESSION.md`.

The grand goal remains open. Do not claim completion unless every gate in
`docs/COMPLETION.md` passes, including closure, compatibility, contradiction,
and independent saturation audits.
