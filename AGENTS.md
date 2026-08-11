# Agent operating contract

## Mission

Build a clean-room, offline C2S chart-gameplay viewer from the completed
gameplay reconstruction in this repository. The product must accept arbitrary
user-selected `.c2s` files, parse them locally, reproduce the recovered chart
timeline and note behavior, and render a faithful original presentation without
shipping or depending on the game's copyrighted art, audio, video, fonts, or
other resources.

In plain terms, this phase recreates only the CHUNITHM-style chart gameplay
surface: chart parsing, scheduling, note/path behavior, playback, inspection,
and—where exposed—local logical-input/judgement simulation. It does not recreate
accounts, online services, player records, unlocks, song delivery, proprietary
media, or cabinet/server infrastructure. It is not affiliated with or presented
as an official game client.

Stage one—the exact-snapshot gameplay reconstruction—is complete and remains
the evidence foundation. Stage two—the offline viewer—is the active goal. The
12 GitHub issues have been investigated but are not the bounds of stage two. Keep
expanding and closing the binary-backed presentation map until a deliberate
saturation audit finds no unexplored gameplay-render path, field consumer,
indirect behavior, state transition, or relevant external-parameter selection.

Read, in order:

1. `docs/SCOPE.md`
2. `docs/WORKFLOW.md`
3. `docs/HARNESS.md`
4. `docs/EVIDENCE.md`
5. `docs/GHIDRA.md`
6. `docs/COMPLETION.md`
7. `docs/VIEWER_ROADMAP.md`
8. `research/STATUS.md`, `research/COVERAGE.tsv`, and
   `research/VIEWER_COVERAGE.tsv`
9. `research/GITHUB_ISSUE_VERIFICATION.md`

Then follow `docs/HARNESS.md`: run the preflight/validation/selection commands,
build the reconstruction, and run its tests. Run `python3 scripts/harness.py
doctor` before any new Ghidra work.

## Non-negotiable rules

- Do not execute `game.exe`, use Wine, attach a debugger, emulate the whole
  program, or bypass its boot checks. Static Ghidra analysis remains allowed.
- Treat `game.exe`, `music.zip`, `music/`, the Ghidra project, and any gameplay
  footage as local reference material. Do not ship, publish, embed, or copy
  them into product code, fixtures, documentation, screenshots, or releases.
- `music/` contains official and unofficial charts solely for local testing,
  aggregate analysis, and human comparison. The product must not require that
  directory and must work with arbitrary `.c2s` files selected at runtime.
- Never embed real chart lines or large derived tables from the corpus. Commit
  only small synthetic fixtures authored from the clean-room specification.
- Do not copy game textures, models, artwork, logos, sounds, video frames,
  fonts, shaders, or binary resources. Use original primitives, colors,
  geometry, labels, and optional user-supplied media outside the repository.
- The viewer is offline-first. A selected chart stays in the local process or
  browser; do not upload it or add telemetry/network calls without a separate,
  explicit product decision from the owner.
- Accounts, server protocols, persistent player records, leaderboards, unlocks,
  monetization, and proprietary song/media playback are out of scope.
- Keep evidence labels honest. Recovered rules, corpus invariants, footage
  observations, fitted visual parameters, and original product choices are
  different kinds of knowledge and must never be presented as interchangeable.
- Never call behavior verified because it looks conventional or resembles the
  original footage. Gameplay/spec changes require exact-snapshot evidence;
  visual fidelity may use documented human comparison under `docs/EVIDENCE.md`.
- Every claimed canonical behavior—including visible shape, motion, lifetime,
  layering, and feedback triggers—must close to this exact binary. Corpus data,
  footage, conventions, and existing viewer code may locate a question but may
  not answer it. A reasoned interpretation is acceptable only when its binary
  observations, control/data flow, alternatives, and falsifiers are recorded;
  unsupported deduction is not an implementation rule.
- Public/community information and player reports are useful leads, not proof
  of hidden gameplay rules.
- A keyword present in a local chart is never dismissed as a typo merely because
  the current viewer or spec does not recognize it. These charts are accepted
  by the base game. Locate the exact registration, parser/compatibility handler,
  or proven ignore path and document what the keyword does or why it has no
  gameplay-visible effect.
- “This looks appropriate” and “the current viewer draws it this way” are
  failures, not evidence. For each rendered primitive/configuration, cite the
  binary selection path and the construction/update behavior it reaches. For
  an unavailable external value, trace the executable's loader/source identity,
  table/key/index selection, fallback, and consumer before parameterizing it.
- Git reads are allowed. Commit, push, history rewrite, branch changes, or other
  Git mutations require explicit user authorization for that operation.

## Product architecture boundary

Keep these layers separable even if the prototype remains a single HTML file:

```text
C2S bytes/text
  -> parser and diagnostics
  -> normalized chart model
  -> tempo/measure/projection and generated-note schedules
  -> deterministic playback/gameplay state
  -> original render scene/primitives
  -> offline viewer controls and inspection
```

The renderer must consume the normalized model rather than reinterpret raw
token positions independently. Timing, mirroring, continuation, generated
checkpoints, and note-family semantics belong in shared model/playback logic so
tests and future frontends can reuse them.

## Ghidra policy in stage two

The exact binary is authoritative for canonical presentation behavior. Organize
Ghidra work as focused, auditable slices rather than undirected exploration,
but do not bypass it with corpus inference, footage fitting, or existing-viewer
assumptions where active claims/specs do not already close the presentation
path. Finish with the systematic saturation audit required by
`docs/COMPLETION.md`.

Renames, types, enums, structs, comments, and bookmarks remain allowed only
through Ghidra/MCP and only after the confidence gate in `docs/EVIDENCE.md`.
Do not edit `chart.rep` files directly. Do not extract or reproduce proprietary
assets while investigating presentation behavior. Record meaningful mutations
in a claim and session note, and reopen only the exact affected stage-one
coverage row when new binary evidence contradicts it.

## Work loop

1. Read the current viewer handoff/status and inspect the existing behavior
   before choosing a bounded discrepancy or product slice.
2. State the expected behavior and its evidence class: recovered, corpus-
   supported, footage-observed, fitted, or original product choice.
3. Reproduce the issue with a minimal synthetic chart when possible. Local
   `music/` charts may be used without copying them into the repository.
4. Trace the rule through parser, normalized model, schedule/playback, and
   renderer. Fix the owning layer instead of adding a chart-specific render
   exception.
5. Add focused automated tests for parsable/model behavior and lightweight
   renderer assertions where practical. Do not use copyrighted golden images.
6. Compare representative local charts and, for visual claims, human-observed
   gameplay footage at matched chart/time/speed conditions. Record written
   measurements and discrepancies, not copied frames.
7. If the discrepancy reveals a spec conflict, separate viewer bugs from
   research gaps. Reopen stage-one evidence only after exact-binary analysis.
8. Run `harness.py validate`, build, and tests. Update product status and a
   concise handoff; remove temporary handoffs after their content is promoted
   or dismissed.
9. After the known queue is empty, perform a fresh saturation pass over all
   note/view classes, parsed-field consumers, virtual/indirect calls, reset and
   destruction paths, configuration sources, cross-note ordering, and error
   paths. “No issue remains” is not equivalent to “nothing remains to explore.”
10. Continuously render a coverage-selected set from `music/`. Every reviewed
    chart must finish with zero undefined keywords, unexplained primitives,
    heuristic fallbacks, or unproven constants. Any one of those is a failed
    chart and creates or reopens a research row.

Use one owner per implementation slice. Parallel agents, when explicitly
requested, should receive disjoint parser, model, renderer, or read-only
research targets.

## Definition of done

Do not claim the viewer goal is complete until the active stage-two gates in
`docs/COMPLETION.md` pass and the owner accepts the result after reviewing it
against real gameplay footage and gameplay experience. Stage-one's verified
coverage remains necessary but is not sufficient. Automation supports but
cannot replace this acceptance. Completion also requires an independent binary
saturation audit showing that no gameplay-render behavior remains merely
assumed, conventional, or unexplored.
