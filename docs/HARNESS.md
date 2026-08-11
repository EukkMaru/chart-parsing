# Harness operating guide

## Purpose and authority

`scripts/harness.py` is the local preflight, structural validator, stage-one
work selector, content-free corpus inventory, and browser-parser corpus auditor
for this exact research workspace. It does not execute or emulate `game.exe`,
prove a claim by itself, validate the stage-two viewer ledger, judge rendered
pixels, or replace build/tests, binary analysis, independent review, or owner
gameplay review.

The harness reads local reference paths and metadata. The explicit `corpus`
command reads chart text and emits aggregate vocabulary/version counts. The
explicit `viewer-audit` command serves the local viewer and a content-free
manifest to an isolated headless Firefox process; the actual viewer parser
reads each chart and returns only aggregate exception, unknown-command, and
rejected-association counts. Neither command emits chart content.

## Required session sequence

At the start of every research or viewer-verification session:

1. Read the files listed in `AGENTS.md`, including this guide.
2. Run `python3 scripts/harness.py doctor` when the session may use Ghidra or
   when binary/project/MCP identity has not already been established.
3. Run `python3 scripts/harness.py validate` before trusting the stage-one
   ledger or editing a verified dependency.
4. Run `python3 scripts/harness.py next --count 10` to expose active stage-one
   ownership or the highest-priority mapped/unknown target.
5. Consult `research/VIEWER_COVERAGE.tsv` separately for stage-two work; the
   harness does not select or validate those product rows. Run `viewer-audit`
   before and after viewer parser/model changes.
6. Build and run the applicable focused tests before investigation when the
   current checkout may differ from the last verified handoff.
7. Create or update the dated session note before making evidence or Ghidra
   mutations.

At session end, rerun `validate`, the affected focused tests, the full suite
when shared reconstruction changed, and `next`. Record exact results and any
environmental limitation in the session handoff.

## Commands and exact meaning

### `doctor`

Checks:

- SHA-256 identity of local `game.exe` against the fixed expected snapshot;
- presence of the Ghidra project/database and local `music/` corpus;
- the configured Ghidra installation and required local tools;
- GhidraMCP TCP/Unix-socket visibility and Codex bridge configuration;
- whether a Ghidra project lock is present.

A successful doctor result establishes local identity and availability only.
It does not prove that the active agent exposes callable native Ghidra tools;
confirm that separately before analysis. A TCP failure with a visible Unix
socket may be a sandbox probe limitation, but a missing binary, hash mismatch,
missing project/database, or missing tool is a real preflight failure.

### `validate`

Validates `research/COVERAGE.tsv` and required workspace structure. It checks:

- the exact ten-column stage-one ledger schema;
- unique nonempty row IDs;
- allowed lifecycle/confidence values;
- named ownership for every `investigating` row;
- evidence/spec/test requirements for reconstructed and verified rows;
- safe workspace-relative path prefixes and file existence;
- valid claim state, rejecting superseded claims as active evidence.

Validation is structural consistency, not semantic proof. It currently does
not validate `research/VIEWER_COVERAGE.tsv`, inspect claim reasoning, compare
the viewer with footage, compile C++, execute tests, or pass an audit gate.

### `next`

If any stage-one row is `investigating`, reports those active rows first. If
none is active, reports `unknown` or `mapped` rows in priority then ledger
order. It does not select from the viewer ledger.

When the sole suggestion is `audit.closure`, ordinary automatic stage-one
implementation work is exhausted. That row must be completed by a fresh
independent investigator; the investigator whose corrections require review
must not self-certify it. Stage-two work may continue from
`research/VIEWER_COVERAGE.tsv`, but it is not a substitute for the independent
stage-one gate.

### `corpus`

Enumerates local `.c2s`, `Music.xml`, and jacket-file counts, strict UTF-8 chart
readability, declared VERSION strings, and aggregate command vocabulary. It is
safe for content-free compatibility inventory. Do not redirect raw chart lines
or extend it to commit source content, excerpts, song-specific tables, or
copyrighted media.

### `viewer-audit`

Starts a loopback-only temporary HTTP server, opens the actual
`scripts/c2s-viewer.html` parser in an isolated headless Firefox profile, and
parses every local `.c2s` from a content-free manifest. It fails when a chart
throws, a spelling remains unknown, an AIR-family association is rejected, the
browser/report path fails, or the returned chart count differs from the
manifest. Rejections are broken down only by aggregate command and referenced
root-family name. `--limit N` provides a quick sorted-prefix smoke test; the
default audits the full corpus. `--timeout` changes the default 300-second
browser limit.

This proves that the current browser parser accepts the exercised local corpus
with zero surfaced vocabulary/association failures. It does not compare
rendered pixels, prove geometry, exercise playback through time, establish
hostile-input parity beyond the corpus, or satisfy owner review. The loopback
server is stopped, the reporting browser is terminated, and its isolated
temporary profile is deleted on exit.

## Ledger ownership rules

Stage-one lifecycle values are `unknown`, `mapped`, `investigating`,
`reconstructed`, `verified`, and `excluded`. Confidence is independent.

- Before bounded stage-one work, change exactly the owning row to
  `investigating` and set the investigator in `owner`.
- Do not claim every affected row merely because a question crosses layers.
- A verified row is reopened only for an exact-binary contradiction, not a
  viewer mismatch or footage fit.
- Release ownership at handoff; a completed row uses owner `-`.
- `verified` requires active evidence, normative spec, and an existing focused
  test. Harness success does not supply those artifacts.
- The `audit.closure` row is special: independence is part of its evidence, so
  the corrected-work author cannot own the decisive review.

Stage-two uses the independent fields in `research/VIEWER_COVERAGE.tsv`:
`binary_status`, `product_status`, and `owner_review`. Keep them separate.
Because the harness does not enforce that ledger, manually preserve its six
columns and allowed states from `docs/WORKFLOW.md`.

## Verification matrix

Use the smallest applicable set, then expand with risk:

| Change | Minimum verification |
| --- | --- |
| Session/docs only | `harness.py validate`, link/path review |
| Stage-one ledger/claim/spec | `validate`, focused test, affected dependency review |
| Reconstruction header/source | configure/build, focused CTest, then full CTest for shared behavior |
| Viewer parser/model | synthetic parser/model checks plus full `viewer-audit` (or documented bounded smoke while iterating) |
| Viewer geometry/state | focused primitive/state checks, content-free corpus render smoke, documented owner comparison still pending |
| Ghidra mutation | prior doctor, inspected/dry-run mutation, re-decompile affected paths, save program, record anchors |
| Claimed stage completion | every completion gate, fresh required independent audit, and owner gate where applicable |

The absence of a JavaScript/browser runtime must be recorded; source review is
not a substitute for a viewer smoke test. Likewise, a browser smoke test proves
only the assertions it performs and never proves visual fidelity.

## Failure and safety behavior

- Stop on binary hash mismatch or a missing required reference/project path.
- Fix structural validation failures before adding new evidence to the broken
  ledger.
- Do not change a status merely to make `next` advance.
- Do not run `game.exe`, Wine, a debugger, or whole-program emulation.
- Do not interpret missing external resource values as permission to invent
  canonical constants.
- Do not treat a green harness as completion. Stage one still requires its
  independent closure gate; stage two requires its separate ledger, saturation
  audit, product verification, and owner gameplay acceptance.
