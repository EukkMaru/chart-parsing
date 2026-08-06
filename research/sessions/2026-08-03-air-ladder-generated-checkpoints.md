# Session 2026-08-03: AirLadder generated checkpoints

- Investigator: codex-root
- Ghidra writer: none; read-only MCP analysis
- Coverage rows claimed: `parser.events`, `judgement.types`,
  `note.other_variants`, `state.ownership`, `audit.closure`;
  `render.air_ladder`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Resolve GitHub issue 3's ALD checkpoint contradiction from the exact type-9
parser producer through runtime judgement and presentation input, then correct
every dependent clean-room and viewer artifact without fitting to footage.

## Findings

- `claim.note.air-ladder-generated-checkpoints` supersedes the old ALD claim.
  Type 9 owns separate `0x24` authored controls and `0x20` generated samples.
- ALD's fifth data field is the fixed-grid sampling interval. Positive
  advancing chains sample from the root, interpolate lane, vertical tenths,
  and decoded width, and include the final endpoint only on an exact landing.
- AirLadder creates one profile-7 checker and one individual presentation
  record per generated sample. A later correction closed the distinct main
  path: root plus authored controls enter the shared type-9 geometry builder,
  which emits three presentation streams.
- Parsed `+0x84` is the accepted-event identity for the type-9 precompute, not
  an external configuration key. Its final authored schedule supplies the
  terminal boundary copied to runtime `+0x118`.
- `claim.audit.primary-ald-checkpoint-correction` recorded the first dependent
  cleanup but was itself superseded by the later precompute/presentation
  correction. `claim.audit.primary-ald-precalc-correction` is the active
  primary correction record. The independent closure verdict remains
  superseded; 27 stage-one rows are verified and `audit.closure` remains open
  for a fresh independent review.
- The viewer now regenerates ALD chains/samples from the recovered interval,
  assigns per-sample SLA tags, and no longer uses fixed 1/32 or whole-height
  rung synthesis. The same pass fixed the SLA query shift to half an authored
  tick, preserved DCM source order, and stopped RESOLUTION/CLK_DEF from
  replacing the fixed 384 grid.

## Ghidra mutations

None. Function names remained default because the new roles were documented
and implemented without taking Ghidra write ownership.

## Validation

- `python3 scripts/harness.py doctor`: exact target/project/tools/MCP passed at
  session start.
- `python3 scripts/harness.py validate`: 28-row ledger and required structure
  pass after contradiction cleanup.
- Focused AirLadder build/test passed.
- Full normal build and CTest passed 34/34.
- Headless Firefox loaded and rendered `scripts/c2s-viewer.html` successfully.
- Content-free aggregate pass scanned 7,752 charts, 195,469 ALD lines, both
  known field shapes, 91,368 chains, 18 interval values, and chains up to 459
  controls. The recovered producer yields 100,952 samples across 43,577
  positive advancing chains; 47,791 nonpositive-interval chains yield none.

## Unresolved and contradictions

- `render.air_ladder` remains partial, but the later session closes complete
  vertex fields, clipping, extents, winding, diagnostic counter categories, first/later resource
  selection, and resource-independent effect transform. External resource
  semantic roles and final scene layering remain open. The later session closes
  effect expiry and identifies the formerly auxiliary authored float as each
  endpoint's SLA projection key. Viewer colors/camera/bar silhouette remain
  explicitly provisional.
- `audit.closure` needs a fresh independent contradiction audit. The
  superseded audit cannot validate its own missed premise.
- The viewer still has broader parser/model/presentation gaps recorded in
  `research/VIEWER_COVERAGE.tsv`; this session closes only the ALD generated
  checkpoint producer and the adjacent fixed-grid/SLA/DCM corrections.

## Handoff

This handoff is superseded by
`research/sessions/2026-08-03-air-ladder-precalc-presentation.md`. Continue at
the three type-9 resource submissions after `FUN_00c32330` to close scene
roles/layering. Do not change generated checkpoint count/cadence, authored main
geometry, endpoint SLA keys, or checkpoint expiry: those paths are now closed.
