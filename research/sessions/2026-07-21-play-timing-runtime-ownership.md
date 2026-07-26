# Session 2026-07-21: play-timing runtime ownership

- Investigator: codex-root
- Ghidra writer: none; temporary analysis clone only
- Coverage rows claimed: `timing.clock`; advanced to `timing.tempo_measure`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close the optional accumulated-clock rate producer and the selected runtime
correction table's selector, load source, bounds, and reset lifetime.

## Findings

- Added `claim.timing.play-timing-runtime-ownership` and updated
  `claim.timing.gameplay-clock-reconstruction`.
- RTTI identifies the optional rate object as
  `projView::ViewTimingManager`. It is constructed disabled with unit scalar
  factors and has no recovered internal mutating reference, making the guarded
  accumulated-rate product dormant in normal executable flow.
- The correction selector is the current bounded `PlayOptionSet` ID. The
  process-owned external table is named
  `PlayOptionPlayTimingOffsetTableRecord.bin`; its bounded getter reads a double
  at record `+0x10`, and the judgement manager narrows it to float before
  subtraction. Missing/out-of-range data contributes zero.
- The table clears before load, supports an existing override-directory file
  before the base-directory file, and has dedicated singleton cleanup outside
  chart/gameplay reset. The named resource is absent from both the workspace
  and `music.zip`, so values remain explicit inputs.

## Ghidra mutations

- Live project: none; GhidraMCP was not exposed and port 8089 was not
  listening.
- Temporary clone: recovered the function boundary at `RAM:0113ee00` to inspect
  table singleton cleanup. No live symbols, comments, types, or structures were
  changed.
- Supported live-project annotations to apply when MCP returns:
  `RAM:00c320d0` as the `ViewTimingManager` constructor and `RAM:0113ee00` as
  play-timing-offset-table singleton cleanup. These were not applied.

## Validation

- `python3 scripts/harness.py validate`: passed, 28 coverage rows and 10
  required files.
- `cmake --build build -j2`: passed.
- `ctest --test-dir build --output-on-failure`: all 20 tests passed, including
  the expanded `chart.play_clock` selection/bounds cases.
- Static resource checks found neither named external timing table in the
  workspace or `music.zip`.

## Unresolved and contradictions

- A hidden internal setter for the ViewTimingManager header was considered.
  The singleton pointer's complete literal-reference inventory contains only
  construction, destruction, and three read-only consumers; constructor escape
  is only the singleton store. An out-of-process memory writer remains outside
  normal recovered control flow.
- External correction values, directory roots, PlayOptionSet player-facing
  labels, and corrupt/partial table behavior remain unresolved.

## Handoff

`timing.clock` is mapped and released. `timing.tempo_measure` is now claimed by
codex-root. Start with the shared grid/schedule helpers at `RAM:011c5170`,
`RAM:011c52d0`, `RAM:011c5310`, and the parser/postprocessor call paths already
recorded in the AirHold, AirSlide, and HeavenHold claims. Trace parsed timing
commands, table ownership/reset, position-to-schedule consumers, malformed
timing paths, and external inputs before naming or reconstructing units.
