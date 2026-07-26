# Ghidra and MCP operating guide

## Local identity

- Ghidra: `/home/maru/ghidra_12.1.2_PUBLIC`
- Project: `/home/maru/personal/chart-parsing/chart.gpr` (`chart`)
- Project database: `chart.rep/`
- Program snapshot SHA-256:
  `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`
- GhidraMCP: 5.15.0, HTTP plugin endpoint `http://127.0.0.1:8089`

Paths may be overridden for diagnostics with `GHIDRA_HOME` and
`GHIDRA_MCP_URL`. Run `python3 scripts/harness.py doctor` before analysis.

The GUI must have the `chart` project and `game.exe` program open, with the MCP
server enabled under **Tools > GhidraMCP > Start MCP Server**. The chat/client
must also expose the `ghidra-mcp` MCP server; a listening HTTP port does not by
itself prove that the client loaded its tools.

### Codex bridge registration

The Ghidra plugin on port 8089 exposes its HTTP/Unix-socket backend. Its
`/mcp/instance_info` and `/mcp/schema` paths are plugin REST endpoints; the bare
`http://127.0.0.1:8089/mcp` path is not a streamable MCP transport.

Codex uses the separately installed Python stdio bridge from the local
GhidraMCP checkout:

```toml
[mcp_servers.ghidra-mcp]
command = "/home/maru/ghidra-mcp/.venv/bin/bridge-mcp-ghidra"
args = ["--transport", "stdio", "--no-lazy"]

[mcp_servers.ghidra-mcp.env]
GHIDRA_MCP_URL = "http://127.0.0.1:8089"
PYTHONIOENCODING = "utf-8"
```

This entry is installed in `/home/maru/.codex/config.toml`. Existing Codex
threads do not hot-load newly registered MCP servers; start a new session after
adding or changing the entry. The bridge prefers the live per-user Unix socket
when available and falls back to the configured TCP URL.

## Orientation before decompilation

Use strings, xrefs, imports, callers/callees, function hashes, and data references
to narrow the target. Avoid repeatedly requesting huge decompiler windows.
Inspect one bounded function and its necessary context at a time.

Because entry passes through boot/account/selection flow, do not start at the PE
entry and recursively document everything. Anchor on chart command strings,
file access, gameplay type dispatch, update-loop ownership, device reads, and
judgement/result enums; then connect those islands.

## Annotation discipline

- Preserve default names until responsibility is supported.
- Use explicit hypothesis comments/bookmarks for tentative roles.
- Check all meaningful callers before changing a signature.
- Infer structure fields from repeated base+offset access, width, and use; do not
  invent semantic fields merely to make decompilation prettier.
- Treat vtables/function pointers as coverage obligations. Enumerate possible
  targets or document why a target cannot affect gameplay.
- Record external-config consumers even when numeric values are absent.
- Let MCP convention validation guide final symbol syntax; semantic certainty is
  still the investigator's responsibility.

## Safe mutations

Renames, comments, bookmarks, types, structs, and enums are acceptable after the
confidence gate in `AGENTS.md`. Non-destructive scripts may inventory or analyze.
Do not directly edit files under `chart.rep`, bulk-delete symbols, clear analysis,
re-import the binary over the working program, or run debugger/dynamic endpoints.

After mutations, re-decompile affected callers and callees to catch type-induced
misinterpretations. Save the project and record the changes in the session note.
