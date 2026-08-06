#!/usr/bin/env python3
"""Local diagnostics and ledger validation for the RE workspace.

This utility never executes the target, invokes Wine, or reads chart contents
beyond aggregate command/version inventory requested by the user.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import os
from pathlib import Path
import shutil
import socket
import sys
from urllib.parse import urlparse


ROOT = Path(__file__).resolve().parents[1]
EXPECTED_SHA256 = "4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520"
DEFAULT_GHIDRA_HOME = Path("/home/maru/ghidra_12.1.2_PUBLIC")
ALLOWED_STATUS = {"unknown", "mapped", "investigating", "reconstructed", "verified", "excluded"}
ALLOWED_CONFIDENCE = {"none", "low", "medium", "high"}
ALLOWED_CLAIM_STATE = {"proposed", "active", "superseded"}
REQUIRED_FILES = (
    "_temp_prompt.md",
    "AGENTS.md",
    "docs/SCOPE.md",
    "docs/WORKFLOW.md",
    "docs/EVIDENCE.md",
    "docs/GHIDRA.md",
    "docs/COMPLETION.md",
    "docs/VIEWER_ROADMAP.md",
    "research/STATUS.md",
    "research/COVERAGE.tsv",
    "research/VIEWER_COVERAGE.tsv",
    "spec/README.md",
    "CMakeLists.txt",
)


def ledger_paths(value: str) -> list[str]:
    """Split a semicolon-delimited coverage cell into workspace paths."""
    return [
        item.strip()
        for item in value.split(";")
        if item.strip() and item.strip() != "-"
    ]


def claim_state(path: Path) -> str | None:
    """Read the claim state from its compact metadata preamble."""
    try:
        with path.open(encoding="utf-8") as stream:
            for line in stream:
                if line.startswith("- State: "):
                    return line.removeprefix("- State: ").strip()
                if line.startswith("## "):
                    break
    except OSError:
        return None
    return None


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def mcp_address() -> tuple[str, int]:
    raw = os.environ.get("GHIDRA_MCP_URL", "http://127.0.0.1:8089")
    parsed = urlparse(raw)
    return parsed.hostname or "127.0.0.1", parsed.port or 8089


def socket_open(host: str, port: int) -> bool:
    try:
        with socket.create_connection((host, port), timeout=0.35):
            return True
    except OSError:
        return False


def mcp_socket_files() -> list[Path]:
    """Return visible per-user GhidraMCP Unix sockets without connecting."""
    user = os.environ.get("USER") or os.environ.get("USERNAME") or "unknown"
    directories: list[Path] = []
    runtime_dir = os.environ.get("XDG_RUNTIME_DIR")
    if runtime_dir:
        directories.append(Path(runtime_dir) / "ghidra-mcp")
    getuid = getattr(os, "getuid", None)
    if callable(getuid):
        directories.append(Path(f"/run/user/{getuid()}/ghidra-mcp"))
    directories.append(Path(f"/tmp/ghidra-mcp-{user}"))

    sockets: set[Path] = set()
    for directory in directories:
        try:
            sockets.update(path for path in directory.glob("*.sock") if path.exists())
        except OSError:
            continue
    return sorted(sockets)


def command_doctor(_: argparse.Namespace) -> int:
    failures = 0
    target = ROOT / "game.exe"
    print(f"workspace       {ROOT}")
    if target.is_file():
        actual = sha256(target)
        ok = actual == EXPECTED_SHA256
        print(f"binary identity {'OK' if ok else 'MISMATCH'}  {actual}")
        failures += not ok
    else:
        print("binary identity MISSING  game.exe")
        failures += 1

    paths = {
        "Ghidra project": ROOT / "chart.gpr",
        "Ghidra database": ROOT / "chart.rep",
        "chart corpus": ROOT / "music",
    }
    for label, path in paths.items():
        present = path.exists()
        print(f"{label:<15} {'OK' if present else 'MISSING'}  {path}")
        failures += not present

    ghidra_home = Path(os.environ.get("GHIDRA_HOME", DEFAULT_GHIDRA_HOME))
    headless = ghidra_home / "support" / "analyzeHeadless"
    print(f"Ghidra 12.1.2  {'OK' if headless.is_file() else 'MISSING'}  {ghidra_home}")
    failures += not headless.is_file()

    for tool in ("cmake", "c++", "python3"):
        found = shutil.which(tool)
        print(f"tool {tool:<9} {'OK' if found else 'MISSING'}  {found or '-'}")
        failures += found is None

    host, port = mcp_address()
    tcp_reachable = socket_open(host, port)
    print(f"MCP TCP probe  {'REACHABLE' if tcp_reachable else 'UNREACHABLE'}  {host}:{port}")
    sockets = mcp_socket_files()
    if sockets:
        print(f"MCP UDS socket PRESENT  {sockets[0]}")
        if not tcp_reachable:
            print("  The command sandbox may block TCP/Unix-socket probes; the client bridge can use UDS.")
    else:
        print("MCP UDS socket ABSENT")
    if not tcp_reachable and not sockets:
        print("  Start it in Ghidra: Tools > GhidraMCP > Start MCP Server")
    codex_config = Path.home() / ".codex" / "config.toml"
    try:
        registered = "[mcp_servers.ghidra-mcp]" in codex_config.read_text(encoding="utf-8")
    except OSError:
        registered = False
    print(f"Codex MCP config {'PRESENT' if registered else 'NOT FOUND'}  {codex_config}")
    print("  Also confirm the active agent session exposes ghidra-mcp tools.")

    locked = (ROOT / "chart.lock").exists()
    print(f"Ghidra GUI      {'project lock present' if locked else 'no project lock detected'}")
    return 1 if failures else 0


def load_coverage() -> tuple[list[str], list[dict[str, str]]]:
    with (ROOT / "research" / "COVERAGE.tsv").open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream, delimiter="\t")
        rows = list(reader)
        for order, row in enumerate(rows):
            row["__order"] = str(order)
        return list(reader.fieldnames or []), rows


def command_validate(_: argparse.Namespace) -> int:
    errors: list[str] = []
    for relative in REQUIRED_FILES:
        if not (ROOT / relative).is_file():
            errors.append(f"missing required file: {relative}")

    expected_fields = ["id", "area", "priority", "status", "confidence", "owner", "evidence", "spec", "tests", "next_action"]
    try:
        fields, rows = load_coverage()
    except (OSError, csv.Error) as exc:
        errors.append(f"cannot read coverage ledger: {exc}")
        fields, rows = [], []
    if fields != expected_fields:
        errors.append(f"coverage columns must be: {', '.join(expected_fields)}")

    seen: set[str] = set()
    for line, row in enumerate(rows, start=2):
        item_id = row.get("id", "")
        if not item_id or item_id in seen:
            errors.append(f"coverage line {line}: missing or duplicate id {item_id!r}")
        seen.add(item_id)
        if row.get("status") not in ALLOWED_STATUS:
            errors.append(f"coverage line {line}: invalid status {row.get('status')!r}")
        if row.get("confidence") not in ALLOWED_CONFIDENCE:
            errors.append(f"coverage line {line}: invalid confidence {row.get('confidence')!r}")
        status = row.get("status")
        if status in {"reconstructed", "verified"}:
            for field in ("evidence", "spec"):
                values = ledger_paths(row.get(field, "-"))
                if not values:
                    errors.append(f"coverage line {line}: {status} row lacks {field}")
        if status == "verified":
            if not ledger_paths(row.get("tests", "-")):
                errors.append(f"coverage line {line}: verified row lacks an existing test path")
        if status == "investigating" and row.get("owner") in {"", "-"}:
            errors.append(f"coverage line {line}: investigating row has no owner")

        expected_prefixes = {
            "evidence": "research/claims/",
            "spec": "spec/",
            "tests": "tests/",
        }
        for field, prefix in expected_prefixes.items():
            for value in ledger_paths(row.get(field, "-")):
                path = Path(value)
                if path.is_absolute() or ".." in path.parts:
                    errors.append(
                        f"coverage line {line}: {field} must be a safe workspace-relative path: {value}"
                    )
                    continue
                if not value.startswith(prefix):
                    errors.append(
                        f"coverage line {line}: {field} path must begin with {prefix}: {value}"
                    )
                resolved = ROOT / path
                if not resolved.is_file():
                    errors.append(
                        f"coverage line {line}: {field} path does not exist: {value}"
                    )
                    continue
                if field == "evidence":
                    state = claim_state(resolved)
                    if state not in ALLOWED_CLAIM_STATE:
                        errors.append(
                            f"coverage line {line}: claim lacks a valid State field: {value}"
                        )
                    elif state == "superseded":
                        errors.append(
                            f"coverage line {line}: active evidence cites superseded claim: {value}"
                        )

    if errors:
        print("validation FAILED")
        for error in errors:
            print(f"- {error}")
        return 1
    print(f"validation OK: {len(rows)} coverage rows, {len(REQUIRED_FILES)} required files")
    return 0


def priority_key(row: dict[str, str]) -> tuple[int, int, str]:
    priority = {"P0": 0, "P1": 1, "P2": 2}.get(row.get("priority", ""), 9)
    return priority, int(row.get("__order", "999999")), row.get("id", "")


def command_next(args: argparse.Namespace) -> int:
    _, rows = load_coverage()
    active = [row for row in rows if row["status"] == "investigating"]
    candidates = active or [row for row in rows if row["status"] in {"unknown", "mapped"}]
    candidates.sort(key=priority_key)
    if not candidates:
        closure = next(
            (row for row in rows if row["id"] == "audit.closure"), None)
        if (closure is not None and closure["status"] == "verified" and
                all(row["status"] in {"verified", "excluded"} for row in rows)):
            print("No open row. Coverage and closure audit are verified.")
        else:
            print("No open row. Run the closure audit before claiming completion.")
        return 0
    label = "Continue active ownership" if active else "Suggested bounded targets"
    print(label + ":")
    for row in candidates[: args.count]:
        print(f"- {row['id']} [{row['priority']}/{row['status']}] {row['next_action']} (owner: {row['owner']})")
    return 0


def command_corpus(_: argparse.Namespace) -> int:
    corpus = ROOT / "music"
    if not corpus.is_dir():
        print("music/ corpus is missing", file=sys.stderr)
        return 1
    charts = list(corpus.rglob("*.c2s"))
    metadata = list(corpus.rglob("Music.xml"))
    jackets = list(corpus.rglob("*.dds"))
    commands: set[str] = set()
    versions: set[str] = set()
    unreadable = 0
    for chart in charts:
        try:
            with chart.open("r", encoding="utf-8-sig", errors="strict", newline="") as stream:
                for raw in stream:
                    line = raw.rstrip("\r\n")
                    if not line:
                        continue
                    parts = line.split("\t")
                    command = parts[0]
                    commands.add(command)
                    if command == "VERSION" and len(parts) > 1:
                        versions.add(parts[1])
        except (OSError, UnicodeError):
            unreadable += 1
    print(f"song directories {sum(1 for path in corpus.iterdir() if path.is_dir())}")
    print(f"charts           {len(charts)}")
    print(f"metadata files   {len(metadata)}")
    print(f"jacket files     {len(jackets)}")
    print(f"unreadable charts {unreadable}")
    print(f"declared versions {', '.join(sorted(versions)) or '-'}")
    print(f"command vocabulary ({len(commands)}): {' '.join(sorted(commands))}")
    return 1 if unreadable else 0


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    commands = result.add_subparsers(dest="command", required=True)
    commands.add_parser("doctor", help="check local target, Ghidra, MCP, and tool identity").set_defaults(run=command_doctor)
    commands.add_parser("validate", help="validate research ledger and required structure").set_defaults(run=command_validate)
    next_parser = commands.add_parser("next", help="show active or highest-priority open targets")
    next_parser.add_argument("--count", type=int, default=5)
    next_parser.set_defaults(run=command_next)
    commands.add_parser("corpus", help="print aggregate corpus versions and command vocabulary").set_defaults(run=command_corpus)
    return result


def main() -> int:
    args = parser().parse_args()
    return args.run(args)


if __name__ == "__main__":
    raise SystemExit(main())
