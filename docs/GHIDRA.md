# Ghidra and MCP guide for stage two

## Role in the viewer phase

The exact-snapshot gameplay investigation is closed. Ghidra is now used only
for focused static questions raised by the product, such as an unexplained
command field, generated path, render lifetime, camera transform, or resource-
independent geometry rule. Start from the viewer discrepancy and known anchors;
do not resume broad whole-program exploration by default. Once focused slices
are closed, a final systematic presentation saturation audit is mandatory; the
known GitHub issues are not evidence that all relevant render paths were found.

Static analysis may recover presentation behavior, transformations, state
ordering, and parameter selection. It must not be used to extract, reproduce,
or ship proprietary textures, models, audio, fonts, shaders, effects, or other
assets.

## Local identity

- Ghidra: `/home/maru/ghidra_12.1.2_PUBLIC`
- Project: `/home/maru/personal/chart-parsing/chart.gpr` (`chart`)
- Project database: `chart.rep/`
- Program snapshot SHA-256:
  `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`
- GhidraMCP plugin backend: `http://127.0.0.1:8089`

Run `python3 scripts/harness.py doctor` before analysis. The GUI must have the
`chart` project and `game.exe` open with the MCP server enabled under
**Tools > GhidraMCP > Start MCP Server**. A listening TCP port alone does not
prove that the client loaded the native tools.

### Codex bridge registration

The plugin's HTTP/Unix-socket backend is not a streamable MCP endpoint at the
bare `/mcp` URL. Codex uses the installed stdio bridge:

```toml
[mcp_servers.ghidra-mcp]
command = "/home/maru/ghidra-mcp/.venv/bin/bridge-mcp-ghidra"
args = ["--transport", "stdio", "--no-lazy"]

[mcp_servers.ghidra-mcp.env]
GHIDRA_MCP_URL = "http://127.0.0.1:8089"
PYTHONIOENCODING = "utf-8"
```

Existing threads do not hot-load newly registered MCP servers; start a fresh
session after changing the client configuration. The bridge prefers the live
per-user Unix socket and falls back to the configured TCP URL.

## Focused investigation workflow

1. Reproduce and classify the viewer mismatch under `docs/WORKFLOW.md`.
2. Exhaust active claims, specs, reconstruction helpers, tests, and corpus
   invariants before opening a new binary question.
3. Name the smallest missing fact and its expected product consumer.
4. Use strings, xrefs, imports, callers/callees, hashes, data references, and
   existing note/view RTTI anchors to narrow the target.
5. Trace upstream ownership and downstream consumers, including reset/error and
   indirect paths. Do not stop at a visually plausible function.
6. Record concise original observations in a claim; never paste raw assembly or
   decompiler output.
7. Reopen only the exact stage-one coverage row if verified gameplay evidence
   changes. Presentation-only findings use product evidence labels and need not
   invalidate the closed gameplay ledger.
8. Update `research/VIEWER_COVERAGE.tsv` independently for binary closure,
   product implementation, and owner review. None implies either of the others.

When a corpus keyword is absent from the current viewer/spec, begin with the
live command descriptors/registrations and group dispatch, then trace its exact
handler or rejection/ignore path. Do not normalize its spelling, invent an
alias, or classify it as chart damage because it looks unfamiliar.

When a presentation value is external, continue upstream through configuration
selection and loading far enough to identify the source file/resource/table/key
whenever the binary exposes it. If the raw value is genuinely absent, stop at
that proved boundary and parameterize it; do not infer a canonical number from
appearance.

Because entry passes through boot/account/selection flow, do not start at the
PE entry and recursively document everything. Anchor on the chart command or
render state that produced the concrete discrepancy.

## Annotation discipline

- Preserve default names until responsibility is supported.
- Use hypothesis comments/bookmarks for tentative roles.
- Check meaningful callers before changing a signature.
- Infer structure fields from repeated offset, width, ownership, and use; do
  not invent semantic names to make output prettier.
- Treat vtables/function pointers as coverage obligations when their targets
  can affect the active question.
- Distinguish executable logic from externally loaded parameters and from
  asset contents that will not be recovered or shipped.
- Let MCP convention validation guide symbol syntax; semantic certainty remains
  the investigator's responsibility.

## Safe mutations

Renames, comments, bookmarks, types, structs, and enums are acceptable only
after the confidence gate in `docs/EVIDENCE.md`. Apply them through Ghidra/MCP.
Do not edit `chart.rep`, bulk-delete symbols, clear analysis, re-import over the
working program, invoke debugger/dynamic endpoints, or execute the game.

After a mutation, re-decompile affected callers/callees, save the program, and
record the change in the active claim/session handoff. One agent owns Ghidra
writes at a time.
