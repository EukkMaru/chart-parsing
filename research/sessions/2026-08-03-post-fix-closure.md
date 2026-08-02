# Session 2026-08-03: post-fix independent closure

- Investigator: codex-root; independent read-only review by closure_auditor
- Ghidra writer: codex-root
- Coverage rows claimed: all 28 gameplay rows, including `audit.closure`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Finish the exact-snapshot proof-check after the prior independent failure:
reproduce and close every remaining contradiction, obtain a fresh independent
post-fix verdict, and promote the workspace only if all gates in
`docs/COMPLETION.md` pass.

## Findings

- `claim.audit.primary-closure-corrections` records the reproduced corrections
  to every blocker from the earlier failed audit.
- Final malformed-chart work made ASO/Air float conversion and Air-family tick
  arithmetic source-exact: `CVTTSS2SI` integer-indefinite behavior, wrapped
  32-bit deltas/cursors/margins, high-bit large-span domains, AirSlide cursor
  wrap, and adaptive zero-step nonprogress are now explicit in code, specs,
  claims, and focused tests.
- `claim.note.hold-end-feedback-boundary` was closed through the static
  sound-controller initialization and all reachable `sound::Sound` virtuals;
  the branch is feedback/audio-only and cannot submit or reroute judgement.
- `claim.audit.independent-post-fix-closure-review` records the independent
  PASS on all twelve completion gates, the exact corpus/claim inventory,
  parameter boundary, control-flow audit, contradiction search, and residual
  uncertainty.
- The old `claim.audit.independent-closure-review` remains as superseded
  historical evidence of the earlier valid failure. All 28 coverage rows are
  now `verified`; none remains `unknown`, `mapped`, or `investigating`.

## Ghidra mutations

- Created the missing function at `RAM:004b8fe0` and renamed it
  `InitializeSoundControllerArray` after its construction loop and downstream
  uses established the role.
- Added a plate comment recording construction of 51 wrappers at
  `RAM:01c75b38` with stride `0x80`, the embedded `sound::Sound` object at
  offset `+4`, sole vtable `RAM:0196a9f0`, and why the resolved HOLD feedback
  callees are audio/configuration rather than gameplay callbacks.
- No broad type/structure mutation was applied. The program was saved and
  Ghidra writer ownership was released.

## Validation

- `python3 scripts/harness.py doctor`: exact SHA-256 and local source/project,
  tool, and MCP checks passed.
- `python3 scripts/harness.py corpus`: 7,752 charts, eight versions, zero
  unreadable, and 111 command spellings.
- Normal full build and CTest: 34/34 passed.
- Fresh UBSan build in `/tmp` and CTest: 34/34 passed.
- The independent reviewer also reported fresh Release and UBSan builds at
  34/34 each.
- `python3 scripts/harness.py validate`: passed after promotion with 28 rows.
- `python3 scripts/harness.py next`: reports no open row and verified closure.
- The promotion exposed two ledger-tool assumptions that were valid only for
  pre-closure rows. `harness.py` now validates semicolon-delimited evidence,
  spec, and test cells item by item for `verified` rows and reports completed
  closure when every row plus `audit.closure` is verified.

## Unresolved and contradictions

No gameplay-affecting contradiction remains. Numeric configuration resources
absent from the snapshot stay explicit parameters with recovered selection,
default where executable-owned, lifetime/reset, and consumers. Player-facing
labels and presentation resource identities remain intentionally unassigned.
Malformed source failure domains are modeled without executing unsafe source
loops or allocations. These residuals are allowed by `docs/COMPLETION.md` and
are enumerated in the final independent report.

## Handoff

The grand goal is complete for binary SHA-256
`4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520` at
apparent tick granularity. Preserve the verified ledger and final audit record.
If `game.exe` or the corpus changes, begin a new snapshot audit; do not carry
this closure verdict across versions. Reopen a specific row only on concrete
new exact-binary evidence that falsifies its active claims.
