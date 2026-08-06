# TEMPORARY: grand-plan project status (2026-08-07)

This is a temporary checkpoint summary for reviewing the project from another
machine. The durable authorities remain `docs/COMPLETION.md`,
`docs/VIEWER_ROADMAP.md`, `research/STATUS.md`, `research/COVERAGE.tsv`, and
`research/VIEWER_COVERAGE.tsv`. Supersede or delete this file when those
documents next receive a consolidated handoff.

## Grand goal

Build a faithful, clean-room, offline player/viewer for arbitrary `.c2s` chart
files, limited to CHUNITHM-style chart gameplay behavior. It must:

- parse arbitrary compatible `.c2s` input instead of recognizing known songs;
- reproduce gameplay-affecting timing, generation, input, judgement, state,
  and resource-independent presentation behavior from this exact `game.exe`;
- use original product-owned visuals and ship no game charts, artwork, audio,
  video, fonts, models, effects, extracted tables, accounts, servers, or player
  records;
- identify every unavailable resource/configuration value by its exact loader,
  selector, fallback, and consumer rather than guessing a canonical value;
- pass broad corpus and synthetic verification, then be reviewed by the owner
  against real gameplay footage and gameplay experience.

Pixel identity with copyrighted assets is not the goal. Behavioral, temporal,
and resource-independent geometric fidelity is.

## Where the project is now

### Stage one: `.c2s` ingestion through judgement outcome

The automatic reconstruction/test portion is complete enough that
`scripts/harness.py next` has no ordinary implementation target left. All 27
behavioral coverage rows are verified after correcting the AirLadder
authored-versus-generated ownership contradiction.

The stage is **not formally closed**. Its last gate is `audit.closure`: a fresh
independent investigator must try to contradict the corrected workspace. The
previous independent review is superseded because it predated the AirLadder
correction. The current investigator cannot self-certify this gate.

Current automated evidence:

- exact binary identity and local Ghidra/MCP health pass `harness.py doctor`;
- `harness.py validate` passes 28 ledger rows and 13 required files;
- the full C++ build and CTest suite pass 35/35;
- aggregate compatibility work covers all 7,752 local charts without
  committing chart content.

Therefore: **the leave-it-to-harness implementation loop is done, but its
independent closure review is still pending.**

### Stage two: canonical offline viewer/player

Stage two is active and far from its completion gates. The existing
`scripts/c2s-viewer.html` is a useful arbitrary-file baseline, but it still
contains provisional fitted/product behavior inherited from its original
repository-only implementation. A green stage-one harness does not validate
those presentation choices.

Important binary-backed presentation slices completed so far:

- AirLadder authored main-path ownership versus generated checker/effect
  ownership;
- AirLadder terminal schedule, endpoint SLA keys, three resource-independent
  vertex streams, clipping, effect transforms/lifetime predicate, Joint order,
  TextureTable handle path, and default command append;
- ordinary Slide root/endpoint ownership: SLD/SXD marked endpoints versus
  SLC/SXC shape-only controls, with the final endpoint forced;
- ordinary Slide field-9 feedback-only role and endpoint result-index lifetime;
- ordinary Slide marker-coordinate grouping, judgement-plane split, local
  projected clipping, full-width 6/18-vertex stream, one-lane center stream,
  mode-1 overlay, and Joint order 0/1/2;
- corresponding clean-room C++ helpers, focused tests, claims/specs, Ghidra
  annotations, and partial viewer corrections.

These closures describe construction and geometry without assigning excluded
material/texture meanings. The viewer uses labeled asset-free substitutes
where original resources are unavailable.

Major stage-two work still open:

- close the global presentation boundary through all factories, managers,
  vtables/callbacks, fields, resets, and indirect consumers;
- verify the viewer's arbitrary-file parser against all registered commands,
  compatibility forms, malformed inputs, and source-located diagnostics;
- separate raw parsing, normalized model, deterministic state/playback, render
  primitives, and camera/UI responsibilities;
- reconstruct shared playfield projection/camera relationships and retain only
  genuinely unavailable final parameters as labeled configuration;
- complete presentation slices for Tap/CharaTap, Hold, Flick, Mine, Air,
  AirHold, AirSlide, AirSolid, HeavenHold, cross-family feedback/layering, and
  mirroring;
- connect recovered judgement/lifecycle phases to deterministic playback,
  seeking, result-driven visibility, and Slide/AirLadder presentation modes;
- build content-free keyword/feature inventories and a coverage-selected
  reference-render suite with outliers, interactions, extremes, and holdouts;
- eliminate every undefined keyword, heuristic fallback, unexplained visible
  primitive, unlabeled fitted constant, and unclassified footage difference;
- verify malformed/extreme input handling, large-chart performance, offline
  privacy, and absence of copyrighted assets from release inputs;
- run fresh cross-family and binary-saturation audits after the vertical slices
  close.

At this checkpoint, most viewer coverage rows are still `open` or `partial`,
product status is mostly `provisional`, and owner review remains `pending`.
Completed Slide and AirLadder subpaths do not imply that their entire viewer
rows—or the viewer as a whole—are complete.

## Immediate work order

1. Give a different investigator the stage-one `audit.closure` contradiction
   audit selected by the harness.
2. Continue stage-two projection separation and issue-5 transform research.
3. Connect recovered ordinary Slide and AirLadder runtime/result states to the
   viewer without inventing external values.
4. Trace the separate HeavenHold presentation path and then complete the
   remaining note-family vertical slices.
5. Normalize the current HTML prototype into parser/model/state/render seams
   with synthetic tests.
6. Build and repeatedly run the aggregate feature inventory and reference
   comparison matrix, reopening the owning row for every ambiguity.
7. After all rows close, perform independent binary/product saturation audits
   and prepare the owner review package.

## What “done” will mean

Agents may finish only after all non-owner gates in `docs/COMPLETION.md` have
evidence: every relevant binary path is closed or explicitly external, every
viewer ledger row has binary and product closure, arbitrary/corpus input and
regressions pass, no unexplained fallback remains, and an independent audit
finds no additional in-scope behavior.

The final gate belongs to the owner. The owner must test arbitrary files and
review the comparison matrix against real gameplay footage/experience, then
explicitly accept or reject every residual fitted asset-independent parameter
or presentation difference. Automation cannot pass that experiential gate.
