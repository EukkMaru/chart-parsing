# Session 2026-07-20: type-2 slide path and checkpoint judgement

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: `note.slide` (released mapped); continuing ownership of `pipeline.boundaries`, `judgement.types`, `note.tap`, and `note.hold`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not listening

## Goal

Close parsed type-2 ownership from `SLD/SXD/SLC/SXC` control points through the
runtime path container, `SlideNote` candidate/input state, path sustain,
checkpoint results, reset/error paths, and deferred lifetime.

## Findings

- Added reconstructed claim
  `research/claims/slide-path-sustain-judgement.md`.
- Added normative clean-room specification `spec/notes/slide.md` and connected
  it to ingestion, input, matching, judgement, and external configuration.
- Reconstructed exact six-phase path-window classification, overlap priority,
  two-bank arming/center activity, forced activity, checkpoint source-category
  mapping, one-front consumption, emission-independent gap reset/removal, and
  two-component terminal conjunction.
- Closed control-point ownership through the type-2 postprocessor's keyed,
  manager-owned runtime path container. The note copies checkpoints but uses
  checked lookup for the generated path.
- Confirmed start-before-path update ordering and the resulting same-substep
  shared terminal-route interaction.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Missing functions and
switch-case instructions were created/disassembled only in
`/tmp/chart-readonly-20260720`; source artifacts were not modified.

## Validation

- Binary identity and workspace health were checked by `harness.py doctor` at
  session start.
- CMake configure/build succeeded.
- CTest passed all 14 tests, including new `chart.slide_path` boundary,
  source-state, checkpoint, and terminal tests.
- `python3 scripts/harness.py validate` passed after the durable updates.
- Direct instruction inspection independently confirmed ambiguous window
  endpoint inclusivity and the two-byte lane-state clear.

## Unresolved and contradictions

- External generated-window values beginning at configuration `+0x730`, gap
  values, units, defaults, and schemas are absent and remain parameters.
- The two command-specific type-2 fields and anonymous result/category names
  remain structural rather than player-facing.
- Checked path lookup has an out-of-range failure path; no recovery behavior is
  inferred.
- No contradiction was found between the shared HOLD gap model and Slide's
  embedded tracker. Slide differs by generated lane-window gating and
  checkpoint category schema.

## Handoff

The slide slice is mapped and released. Resume its evidence at parser
`RAM:011c8870`, path builder `RAM:00b25510`, keyed insertion `RAM:00b267f0`,
factory `RAM:00b28cc0`, load `RAM:00c10720`, start update `RAM:00c0ea40`, path
update `RAM:00c0de10`, and terminal predicate `RAM:00c0fdc0`. The next
high-value bounded question is FLK's cross-family candidate asymmetry at
`RAM:00c1f6a0`/`RAM:00c20340` against manager reduction `RAM:00b2b690`, followed
by alternate-meter auxiliary-field ownership if the call paths intersect.
