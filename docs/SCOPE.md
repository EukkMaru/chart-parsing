# Stage-two scope and product boundary

## Product target

The active target is a clean-room offline viewer for arbitrary `.c2s` charts.
It should reproduce the chart-gameplay portion of the reference experience:
parse a chart, build its real timeline and generated paths, play or seek through
it deterministically, render every supported note family with original visual
primitives, and expose enough inspection to diagnose discrepancies.

The existing `scripts/c2s-viewer.html` is the prototype and comparison tool,
not an exception to the architecture or evidence rules.

## Target pipeline

```text
user-selected C2S bytes/text
  -> syntax/compatibility parsing and diagnostics
  -> normalized headers, events, chains, and derived records
  -> BPM/MET/projection/materialization schedules
  -> deterministic note/path/playback state
  -> optional logical-input and judgement diagnostics
  -> original scene geometry and animation
  -> offline viewer, seek controls, overlays, and inspection
```

Every parser, scheduling, continuation, mirroring, lane/width, generated-path,
ordering, timing, and visual-state rule that can change what the user sees or
when it appears belongs in scope.

## Included

- Local file selection, drag/drop, text decoding, parse errors, and diagnostics
- All 91 registered commands and the exact handling of unknown/legacy metadata
- Backward-compatible field shapes represented in the local corpus
- A stable normalized chart model independent of the viewer UI
- BPM, meter, tick, projection, scroll, materialization, and seek behavior
- Tap, hold, slide, air, flick, mine, ladder, solid, HeavenHold, attached
  secondaries, generated samples, checkpoints, paths, and command variants
- Authored and generated note placement, lifetime, visibility, continuity,
  mirroring, layering, and deterministic update order
- Original playfield geometry, lane grid, camera/perspective, note primitives,
  readable colors, animation, and accessibility controls
- Offline playback, pause, seek, speed, overlays, command inspection, warnings,
  and deterministic replay of the same chart state
- Reuse of the recovered input/judgement model for local diagnostics or future
  interactive play, without persistent scores or online identity
- Static analysis of rendering/presentation code only when it is needed to
  resolve a concrete product discrepancy and does not extract game assets
- Local aggregate/corpus testing and written human comparisons against actual
  gameplay footage at matched chart/time/speed conditions
- Coverage-driven local rendering of many charts from `music/`, including rare
  commands, compatibility forms, note variants, interaction-heavy charts, and
  projection/tempo extremes, with per-chart ambiguity accounting

## Reference-material boundary

`game.exe`, `music.zip`, `music/`, the Ghidra project, and gameplay footage are
reference inputs, not product dependencies.

- `music/` contains official and unofficial charts. They may be loaded locally
  for testing and human comparison, but no chart or excerpt is shipped,
  embedded, committed as a fixture, or copied into documentation.
- Automated committed fixtures must be small, synthetic, and authored from the
  clean-room format specification.
- Gameplay footage may be watched and measured. Do not commit frames, clips,
  audio, or other copied media; record only written observations and metadata
  sufficient to reproduce the comparison locally.
- The shipped viewer must accept arbitrary user-provided `.c2s` files and must
  not assume the repository's `music/` directory exists.

## Excluded

- Original game artwork, textures, models, logos, fonts, sounds, music, video,
  shaders, effects data, cabinet dumps, and other copyrighted resources
- Bundling or redistributing official or unofficial charts from `music/`
- Accounts, authentication, server protocols, online matching, telemetry,
  leaderboards, unlocks, purchases, achievements, and persistent player records
- Song/media delivery and a clone of the original selection/result interfaces
- Boot/security bypasses, executable patching, dynamic game instrumentation,
  Wine, debugging, or running `game.exe`
- Claims of official compatibility, affiliation, or pixel-identical proprietary
  presentation

An excluded subsystem enters scope only through the narrowest behavior needed
for chart playback or rendering. Reimplement the behavior with original assets
and code; do not reproduce the subsystem or its resources wholesale.

## What “faithful” means here

Faithful means that, for the same valid chart and playback conditions, the
viewer agrees on parsed structure, event timing, lane/width, path continuity,
generated elements, note lifecycle, ordering, and observable chart-gameplay
motion within documented tolerances. Visual styling remains original and may
differ from proprietary assets. Fitted presentation values must be labeled and
validated across multiple charts rather than tuned to one recording.

Every reference-facing rule must correspond to behavior traced in the exact
binary: parsing, timing, construction, transforms, geometry relationships,
visibility, animation, layering, and feedback triggers. Existing viewer
heuristics, corpus patterns, rhythm-game conventions, and footage-only
interpretations are not canonical evidence. Where the executable selects an
external resource value that is absent from the snapshot, recover the source,
selection, units, and consumer; expose the unavailable value as a labeled
parameter. Its numerical fit remains provisional until the owner accepts it.

Visual completion is ultimately judged by the owner against actual gameplay
footage and gameplay experience. Automated checks establish deterministic
structure and prevent regressions, but do not substitute for that review.

A reviewed chart passes only when every encountered keyword has an exact
parser/disposition explanation and every gameplay-visible output has a traced
binary construction/update/configuration path. “Typo,” “looks right,” “seems
appropriate,” and “the old viewer did it” are failed explanations. A missing
external numeric value can remain parameterized only after its loader/source,
selection, fallback, and consumer are identified; the missing contents and
chosen substitute remain explicit.

## Intended consumers

- Players and chart authors inspecting arbitrary C2S files offline
- Developers testing a reusable C2S parser/model
- Researchers comparing clean-room behavior with the verified reconstruction
- Future local frontends that reuse the parser and playback model without
  inheriting the prototype viewer's UI
