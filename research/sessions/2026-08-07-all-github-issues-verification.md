# Session 2026-08-07: all GitHub issues and handoff verification

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: stage-two documentation/model/parser rows only; no
  stage-one ownership claimed
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Inventory and investigate all repository issues, answer every question from
the former temporary handoffs, verify conclusions against the exact local
snapshot, correct the clean-room reconstruction/viewer where the evidence
requires it, and leave a durable status record. The issue set is a seed list,
not the project completion boundary.

## Findings

### Setup and issue inventory

- The checkpoint branch fast-forwarded from `862d1e0` to remote `93e6136`
  before investigation; no pull conflict occurred.
- `docs/HARNESS.md` was created before issue work and added to the required
  read/validation sequence. It records command authority, non-authority,
  ownership, safety, and verification rules.
- GitHub contains exactly 12 issues. At inventory time 1, 4, and 5 were closed;
  2, 3, and 6 through 12 were open. Every body and comment was reviewed.
- Both former temporary handoffs were read, answered, promoted into
  `research/GITHUB_ISSUE_VERIFICATION.md`, and removed. Durable read order now
  points to that matrix.

### Binary findings

- DCM queries use keyed-adjusted entity time, then the source-order first
  nonzero covering interval after the exact `+1` shift. Materialization and
  active presentation use the same scope; endpoint retry recomputes from the
  endpoint key. Anchors: `FUN_00b29c90`, `FUN_00b28890`, `FUN_011c58f0`.
- Slide generated endpoint resources decode the ending/post-shrink width at
  generated `+0x14`; `+0x10` is preceding width. Anchors:
  `FUN_00c0f8b0`, `FUN_00b25510`.
- MET fields are unit then count. Beat step is `384/unit`; bar step is
  `count*384/unit`; either zero terminates subsequent generation. Anchor:
  `FUN_011bb0f0`.
- The exact 91-name descriptor registry and exact 24 rejected legacy names are
  closed. Unknown prefixes have no wildcard compatibility rule.
- AIR/AHD/ASD records attach in source order to an earlier compatible root and
  never create orphan standalone records. Failed searches diagnose/skip.
  Continuations use saved endpoints; ASD/ASC also require the referenced prior
  marker. Root endpoint position is read from parsed `+0x34`; lane/width use
  the final-path accessor for types 2/9/10/13. Anchors: `FUN_011c8870`,
  `FUN_004574f0`, `FUN_011c5ca0`, `FUN_011b4e80`.
- The accepted six-data-field legacy Slide form copies root width into its
  control. The handler's count includes the command token, so seven or more
  data fields overwrite from endpoint-width field 7. Missing endpoint width
  therefore inherits root width rather than becoming 1. Anchors:
  `FUN_011c8870`, `FUN_011b2470`.
- HOLD, AirSolid, and HeavenHold presentation consume independently selected
  endpoint SLA keys. The common Air vertical transform, AirSlide action
  offset, AIR resource mirror-sign pairing, and all five former handoff
  questions are answered in the durable issue matrix.

### Product and reconstruction changes

- The viewer mirrors the exact registry, exposes unknown `T_*`, removes the
  synthetic BPM-zero seed, uses the snapshot three-way introsort, generates
  exact MET beat/bar steps, and implements the closed DCM/endpoint-key rules.
- Slide and AIR-family normalization is source ordered, first compatible,
  single-secondary-slot, marker aware, and suppresses failed associations.
  Legacy Slide endpoint width now follows the binary field-count branch.
- Ordinary AirHold uses the recovered zero object origin; AirSlide authored
  controls/actions and AIR left/right resource signs use recovered transforms.
- Clean-room C++ gained the exact 91-name registry, meter steps, common Air
  transforms, Slide ending-width selector, and legacy Slide endpoint-width
  helper with focused tests.
- `harness.py viewer-audit` now starts a loopback-only server, runs the actual
  viewer parser in isolated headless Firefox, and returns only aggregate chart,
  exception, unknown, and rejected-association counts. It does not judge
  pixels or replace owner review.

## Ghidra mutations

None. Analysis used read-only decompilation/disassembly against the open exact
`game.exe` program. No symbol, type, comment, bookmark, or project mutation was
made.

## Verification

- Initial `harness.py doctor`: exact binary hash, project/database/corpus,
  Ghidra/tooling, MCP socket, and bridge configuration passed.
- GitHub inventory: all 12 issues and comments reviewed and classified.
- Full build succeeded; CTest passed 35/35 after reconstruction changes.
- Actual-viewer full corpus audit: 7,752 charts, 0 parse errors, 0 unknown
  records, 0 rejected associations, 0 harness errors.
- Browser render smoke: 20 coverage-spread charts loaded/rendered headlessly
  with populated, materially different outputs and empty rejection panels.
  Focused legacy AIR/AHD and modern ASC renders were inspected after the
  compatibility correction.
- Final `harness.py doctor`: passed with the same exact binary identity and all
  project/MCP/tool checks available. Final `validate`: passed with 28 coverage
  rows and 15 required files. Final `next --count 10`: selected only the
  independently owned `audit.closure` row; this investigator did not claim it.

## Unresolved and contradictions

- This session closes every binary question asked by the 12 issues and former
  handoffs; it does not close the grand product goal.
- Stage one still requires the separately owned fresh `audit.closure` review.
- Stage two still has hostile JS numeric/failure parity, seek-equivalent state,
  HeavenHold/type-13 presentation, shared camera/viewport recovery, remaining
  presentation-family slices, result-state/external-resource integration,
  independent binary saturation, and the owner's footage/gameplay review.
- External resource contents and configuration values remain parameters at
  their traced loader/selector boundaries; none were guessed or copied.

## Handoff

Continue from `research/VIEWER_COVERAGE.tsv`, with numeric parser parity or the
distinct HeavenHold presentation slice as the highest bounded product work.
Run `harness.py viewer-audit` after parser/model changes. Preserve the exact
snapshot sort and six-field Slide compatibility branch, and do not reinterpret
an accepted corpus form as a typo. Do not claim visual fidelity from the green
parser audit or render smoke; owner review remains pending. No stage-one
coverage ownership was taken or left active by this session.
