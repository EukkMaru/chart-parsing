# Session 2026-07-26: final documentation and tooling handoff

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`; released to `mapped` with no
  owner at session end
- Binary hash checked: yes
- MCP health checked: yes; live `chart`/`game.exe` instance and stdio bridge
  verified

## Goal

Make the workspace safe to hand to a fresh investigator without relying on
collapsed chat context. Audit the durable status, coverage ledger, claim/spec/
test references, superseded interpretations, build instructions, current MCP
topology, active ownership, and exact next research target.

## Findings

- GhidraMCP 5.15.0 is live. On the final check, `/mcp/instance_info` reported
  project `chart`, current program `game.exe`, PID 23373, and TCP port 8089.
  The local Python bridge auto-connected through the per-user Unix socket and
  registered 184 tools.
- Codex has an enabled global `ghidra-mcp` stdio entry using
  `/home/maru/ghidra-mcp/.venv/bin/bridge-mcp-ghidra --transport stdio
  --no-lazy`. The exact entry is recorded in `docs/GHIDRA.md`. Port 8089 is
  the plugin REST backend; its bare `/mcp` path is not the MCP transport.
- Existing Codex threads do not hot-load a newly added MCP server. The next
  investigator must start a fresh Codex session in this workspace and confirm
  that the native `ghidra-mcp` tools are exposed before claiming Ghidra writes.
- `research/STATUS.md` had retained the resolved “MCP down” blocker and pointed
  to a result-persistence contradiction already closed by
  `claim.pipeline.final-result-persistence-order`. Both are corrected. The
  actual next target is the remaining direct-callee/write-alias and alternate-
  transition audit around `FUN_00da9820`.
- Active coverage no longer cites the superseded single-stage shared-result or
  nonaggregate-MNE claims. The C++ trace comments now cite
  `claim.judgement.shared-result-two-stage-routing` and
  `claim.note.mine-contact-aggregate-judgement`.
- `harness.py validate` now checks every semicolon-delimited evidence, spec, and
  test path, enforces its expected directory, rejects unsafe/missing paths, and
  rejects superseded claims as active evidence.
- The README now describes the reconstruction accurately as header-only under
  `include/`.

## Ghidra mutations

None. This session only performed read-only health/bridge checks and workspace
documentation/traceability maintenance. No symbols, comments, types, structures,
enums, or bookmarks were changed in the live project.

## Validation

- `python3 scripts/harness.py doctor`: binary, Ghidra project/database, corpus,
  Ghidra installation, and toolchain identity passed; the command sandbox could
  not reach TCP 8089 but saw the live per-user Unix socket and the Codex MCP
  registration.
- Direct read-only Unix-socket query: live `chart` project and open `game.exe`
  confirmed.
- Direct stdio bridge smoke test: auto-connected to `chart` and registered 184
  tools.
- `python3 -m py_compile scripts/harness.py`: passed.
- `python3 scripts/harness.py validate`: passed with 28 coverage rows and 11
  required files.
- `cmake --preset dev` and `cmake --build --preset dev -j2`: passed.
- `ctest --preset dev`: 28/28 passed.
- `python3 scripts/harness.py next --count 5`: selects
  `pipeline.boundaries` first, with no owner.
- Final ledger snapshot: 26 `mapped`, 2 `unknown`, 0 actively owned rows;
  57 active claims and 3 explicitly superseded claims.

## Unresolved and contradictions

- The grand goal is open. None of the completion gates in
  `docs/COMPLETION.md` should be treated as globally passed merely because
  individual claims and tests are strong.
- `input.transport` and `audit.closure` remain `unknown`. The other 26 rows
  remain `mapped`, not reconstructed or verified at whole-row scope.
- Exact external judgement configuration values remain absent. Continue to
  recover their interfaces, selectors, lifetimes, and consumers; keep numeric
  values parameterized.
- This documentation session did not resolve another gameplay branch and did
  not apply the previously deferred confidence-gated annotations to the live
  Ghidra project.
- The superseded claims remain intentionally present as contradiction history.
  They must not be restored as active coverage evidence.

## Handoff

The one-file continuation prompt is `_temp_prompt.md`; a new session can be
started with “read `_temp_prompt.md` and follow it.” It routes back to the
authoritative contract and this handoff without duplicating detailed findings.

Start a fresh Codex session in `/home/maru/personal/chart-parsing`. Follow
`_temp_prompt.md`, confirm the session exposes `ghidra-mcp` tools and that
`chart`/`game.exe` is current, and claim `pipeline.boundaries` before any Ghidra
mutation.

Resume at `FUN_00da9820`: enumerate the remaining direct callees, check their
writes and aliases against gameplay owners, and close alternate exit/transition
targets. The factory/common-state path and callbacks through final persistence
are already bounded at the anchors listed in `research/STATUS.md`. Do not reopen
the dormant child hook or state-`0x13` result-reset/final-report path without a
new producer or contradictory write. If the remaining callees are gameplay-
external, record that closure, update coverage/status, release ownership, and
advance to the next P0 result from `harness.py next`.
