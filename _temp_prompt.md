# Resume the canonical offline C2S viewer project

Work autonomously in `/home/maru/personal/chart-parsing` until the available
session ends or a genuine external blocker prevents useful in-scope work. The
active goal is a faithful, asset-independent C2S chart-gameplay player/viewer
for arbitrary user-selected files.

## Mandatory startup

1. Read `AGENTS.md` completely.
2. Read every document it requires, in order.
3. Treat `docs/VIEWER_ROADMAP.md` as the durable issue/status handoff. Read the
   linked GitHub issues for their original reports, then read
   `research/GITHUB_ISSUE_VERIFICATION.md` for the exact-binary verdicts.
4. Run:

   ```bash
   python3 scripts/harness.py validate
   cmake --preset dev
   cmake --build --preset dev
   ctest --preset dev
   ```

5. Inspect `scripts/c2s-viewer.html` and reproduce the selected discrepancy
   before editing it.
6. Run `python3 scripts/harness.py doctor` and connect to the existing Ghidra
   `chart` project only when a focused issue requires binary evidence.

## Current starting point

Stage-one gameplay behavior is corrected and verified except for the fresh
independent closure gate. An exact type-9 trace showed that the former ALD
claim and closure audit had conflated `0x24` authored controls with `0x20`
generated checkpoints. The replacement producer, specs, C++, focused/full
tests, viewer model, and aggregate corpus pass are current; 27 coverage rows
are verified and `audit.closure` remains open for a new independent review.
Do not reuse the superseded closure verdict or reset the gameplay ledger into
the product backlog.

The existing offline viewer is the stage-two baseline. It was initially built
by an agent that could read this repository but not inspect the binary. It can
parse arbitrary C2S files and approximate the chart gameplay, but it frequently
diverges from gameplay footage in shape and presentation. Its fitted values and
guesses are not canonical merely because they already exist in code.

The renderer author's 12 GitHub issues and both temporary handoff notes were
reviewed on 2026-08-07. Their durable verdicts are in
`research/GITHUB_ISSUE_VERIFICATION.md`. Binary questions from that set are
answered; the exact timing sort and full-corpus browser parser audit are now
implemented; remaining work is exact hostile-input parity,
type-13 presentation, resource-independent camera/geometry, result-state
integration, and the broader presentation saturation audit. Footage is for
discrepancy discovery and visual validation; gameplay/spec changes still
require exact evidence.

## Current priorities

1. Finish numeric failure/range behavior and hostile signed/unsigned domains
   in the viewer parser. The snapshot timing introsort is already ported and
   self-tested; do not replace it with host sorting.
2. Implement the distinct HeavenHold/type-13 presentation path and continue
   the shared camera/viewport and resource-independent geometry trace.
3. Keep parser, normalized model, schedule/playback, and renderer behavior
   logically separated even if the prototype remains one HTML file.
4. Replace chart-specific heuristics with rules that work for arbitrary C2S
   input. Keep truly external visual values configurable and labeled as fitted.
5. Expand synthetic/model/renderer tests and written cross-chart comparison
   evidence without committing real charts or footage.
6. After the issue queue is closed, continue the systematic binary presentation
   map and perform a fresh saturation audit. Do not stop merely because no
   reported discrepancy remains.
7. Render a coverage-selected, growing set from `music/`. A reviewed chart
   passes only at zero undefined keywords, heuristic/fallback primitives,
   appearance-only constants, and unclassified footage differences.

## Required standard

- Never run or debug `game.exe`; static analysis only.
- Never ship/copy charts from `music/`, game media/assets, or footage.
- The product must work without `music/` and parse selected files locally.
- No accounts, network/server behavior, telemetry, player records, proprietary
  media, or official branding.
- Classify every change as recovered, corpus-supported, observed, fitted,
  product choice, or hypothesis.
- Fix the owning parser/model/render layer; do not key behavior to song IDs or
  local chart paths.
- Preserve superseded research claims and reopen verified coverage only on new
  exact-binary evidence.
- Do not implement a canonical behavior from convention, corpus correlation,
  footage, or an existing-viewer guess. Close its producer/consumer path in the
  exact binary; parameterize any value the binary only loads externally.
- Never “correct” an unfamiliar corpus keyword as a typo. Find its exact
  registration, handler/compatibility behavior, or proved ignore/reject path.

## Session end

Run applicable validation and tests, update the product-stage section of
`research/STATUS.md`, and leave a concise temporary handoff with exact issue,
code, claim/spec, and Ghidra anchors. Remove or promote older temporary notes
after their leads are resolved.

Do not claim completion until every active stage-two gate in
`docs/COMPLETION.md` passes, including arbitrary-file behavior, asset
independence, canonical issue resolution, comparison coverage, robustness, and
independent binary saturation.
The owner, not the agent or test suite, gives final acceptance after reviewing
the workflow against real gameplay footage and gameplay experience.
