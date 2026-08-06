# Canonical viewer roadmap and durable handoff

## Purpose and current state

This document is the durable stage-two handoff. It records the product goal,
non-negotiable boundaries, current issue triage, known binary leads, and ordered
next work so a new session can continue after context compaction without
reconstructing project intent from chat history.

Stage-one behavior is corrected and verified for the exact local binary, but
its closure gate is reopened: 27 gameplay coverage rows are verified and
`audit.closure` awaits a fresh independent review after the ALD authored versus
generated-vector, precompute-identity, terminal-schedule, and main-presentation
corrections. The superseded audit is not current evidence.
Stage two remains active: turn `scripts/c2s-viewer.html` into the most faithful
clean-room offline C2S player/viewer the available evidence supports.
The current viewer is a functional baseline, but its original author could read
this repository and could not inspect `game.exe`; several presentation rules
and fitted constants are consequently provisional and often disagree with
actual gameplay footage.

The five GitHub issues are seed reports only. Work does not end when they close.
Continue until every applicable row in `research/VIEWER_COVERAGE.tsv` has
binary and product closure, a fresh reachability/field-consumer saturation
audit finds no new gameplay-visible behavior, and the owner completes the final
review.

“Canonical” means recovered chart behavior, timing, spatial relationships,
generated elements, note-shape rules, and motion where the executable exposes
them. It does not mean copying the game's artwork or achieving pixel identity
with proprietary resources.

## Non-negotiable product context

- Accept arbitrary user-selected `.c2s` files at runtime. The product must not
  depend on repository paths, known song IDs, or the local chart corpus.
- `music/` contains official and unofficial charts from several resource eras.
  It is local reference/test material only. Do not ship, embed, quote, or turn
  those charts into committed fixtures.
- Use small synthetic fixtures authored from the clean-room specification for
  committed tests. Corpus runs must emit only aggregate results and diagnostics.
- Do not copy or ship game textures, models, artwork, logos, fonts, audio,
  video, shaders, effects data, screenshots, extracted tables, or branding.
  Render with original primitives and product-owned presentation.
- Gameplay footage is a local human-comparison reference. Record written
  observations, timings, and measurements; do not commit copied frames or
  media.
- Keep the viewer offline and local-only. Accounts, servers, telemetry, player
  records, unlocks, scoring services, and song delivery are not part of this
  product.
- Never run or debug `game.exe`. Focused static Ghidra analysis is allowed;
  proprietary presentation assets are not analysis outputs.

## Authority and decision order

Use the narrowest applicable source and preserve its evidence class:

1. New, closed exact-snapshot static analysis may correct an existing fact.
   If it contradicts stage one, supersede the old claim and reopen only the
   affected coverage row.
2. Active claims, normative `spec/` documents, the clean-room reconstruction,
   and tests define the currently recovered gameplay/format behavior.
3. Aggregate corpus analysis establishes compatibility and vocabulary, not
   hidden semantics by itself.
4. Matched gameplay footage establishes observable presentation differences
   and can falsify an implementation, but it does not establish a canonical
   rule without the corresponding binary path.
5. Fitted values cover external/resource-only parameters and must remain
   configurable, labeled, and tested across multiple references.
6. Original product choices cover styling or interaction not recoverable from
   the executable and must not be presented as reference behavior.

See `docs/EVIDENCE.md` for the exact labels. A plausible visual match is not a
reason to rewrite a verified gameplay rule.

## Per-chart proof rule

Render many local references, but score them by provenance rather than by
whether they merely look plausible. For every reviewed chart:

- every keyword maps to an exact registration/handler, compatibility path, or
  proved rejection/ignore path; unfamiliar spelling is never called a typo;
- every visible primitive/configuration maps to the binary selector that chose
  its construction/update path and to what that path does;
- every numeric parameter maps to an embedded constant or to a traced external
  loader/source, key/index, fallback, and consumer;
- every difference from footage/gameplay experience is classified and either
  fixed from binary evidence or retained as an explicit external-value residual
  for owner review.

“This renders this way just because,” “this value seems appropriate,” and “this
keyword looks like a typo” are failures. An external value may remain unknown
only with a proved boundary such as “the executable loads/selects this value
from this identified resource/configuration source and consumes it here.” The
chosen asset-independent substitute stays labeled and configurable.

The reference suite is sufficient only when aggregate feature selection covers
all known keywords, variants, note/view classes, legacy forms, transforms,
interactions, extremes, and holdouts; another saturation pass finds no new
feature bucket; every reviewed chart has zero ambiguity/failure; and the owner
accepts the breadth. A raw chart count is not a substitute for coverage.

## Architecture invariants

Maintain one directional pipeline even if the prototype remains one HTML file:

```text
C2S input
  -> parser plus source-located diagnostics
  -> normalized chart records and chains
  -> schedules, projections, and generated records
  -> deterministic playback/gameplay state
  -> original render primitives and scene transform
  -> controls and inspector
```

- Only the parser may interpret raw token indices.
- Continuation, mirroring, defaults, generated paths/checkpoints, timing, and
  note ownership belong in the normalized model or schedule layer.
- Rendering consumes typed state; it must not maintain a second C2S schema.
- Camera/screen mapping and original styling stay separate from recovered
  chart-space geometry and playback state.
- Seek must reconstruct the same state as forward playback at the same time.
- Chart-specific conditions, song IDs, or corpus paths are forbidden fixes.

## GitHub issue review

The original renderer author filed five ambiguity issues. Their issue bodies
are leads from the repository-only implementation, not authoritative findings.

### Issue 1 — SLP parser vocabulary

[Issue 1: Parser vocabulary: SLP command is undocumented](https://github.com/EukkMaru/chart-parsing/issues/1)

**Status: answered by stage-one evidence; issue is stale.**

- `spec/c2s.md` documents the SLP fields and command role.
- `claim.schedule.projection-schedule-materialization` traces registration,
  parsing, and the shared STP/SFL/SLP/DCM schedule consumer.
- `include/chart/reconstruction.hpp` and
  `tests/projection_schedule_test.cpp` reconstruct and test the behavior.
- The local aggregate inventory counted 3,113 SLP records, so this is not an
  isolated spelling or unsupported command.

Next action: verify the viewer parser/model uses the recovered rule, add a
minimal synthetic test if absent, then annotate and close the GitHub issue. Do
not repeat vocabulary archaeology.

### Issue 2 — profile 7 upward-exit interpretation

[Issue 2: derived profile 7 may test absence of upward exit](https://github.com/EukkMaru/chart-parsing/issues/2)

**Status: gameplay mechanism recovered; proposed physical meaning unresolved.**

- `claim.input.snapshot-profile-synthesis` establishes profile 7 as an
  inclusive range test over the derived scalar and closes its state ownership,
  reset, and history behavior.
- The executable selects externally loaded range values; their numerical
  contents and physical/player-facing meaning are unavailable in this binary.
- Reset scalar 65 and the recovered upper-sentinel behavior do not establish
  whether a particular external range represents presence or absence of an
  upward exit.

Next action: keep the semantic interpretation labeled **Hypothesis**. Any local
input simulator should expose profile values as explicit configuration and
must not claim that profile 7 means “upward exit present” or “absent” without
new evidence. This issue does not block static chart rendering.

### Issue 3 — ALD visible elements and judged checkpoints

[Issue 3: ALD may generate judged checkpoints; two distinct elements](https://github.com/EukkMaru/chart-parsing/issues/3)

**Status: generated judged checkpoints, asset-independent geometry, scene
submission, and the effect-expiry predicate are recovered; result-state
integration remains unresolved and final pixel semantics are external data.**

The former stage-one claim had conflated two vectors. The preserved superseded
claim is `research/claims/air-ladder-judgement.md`; the replacement
`research/claims/air-ladder-generated-checkpoints.md` closes the actual path:

- type-9 parsing appends authored endpoints as `0x24`-byte controls;
- `FUN_011c6a50` regenerates a distinct `0x20`-byte vector from the root at
  ALD's positive fifth-data-field interval, interpolating lane, vertical
  tenths, and decoded width;
- the parser recomputes every generated record's schedule;
- `FUN_00c132f0` constructs one `0x88` runtime profile-7 checker/resource
  record per generated sample, with different first/later resource tables;
- `FUN_00c11ca0` can resolve and submit category 18 for every generated record;
- `FUN_00c12350` visits generated points for their individual effect records,
  then separately projects root plus authored-precompute endpoints for the
  main type-9 geometry branch in `FUN_00c03c00`.
- Every authored root/endpoint owns an independently selected SLA projection
  key; the viewer must not project a whole ALD segment with only its root key.
- The main builder clips segments to projected range `[-600, 50]`; its three
  stream layouts, extents, and vertex winding are reconstructed in neutral
  clean-room form. Its later 7, 9, 8 calls only count vertex triples in
  diagnostic categories; they do not prove layer order.
- The final neutral-vertex float comes from an exact embedded 16-entry style
  table; its numeric path is closed without assigning an excluded-resource
  meaning.
- Three consecutively registered `projView::Joint` graph children submit the
  streams in index order 0, 1, 2. Each checked descriptor selects a runtime
  handle through `projDB::TextureTableRecord`; the record supplies an external
  filename/path. Valid Joints append dynamic-primitive command records after
  compatible adjacent batching. This is submission order, not a license to
  infer external material roles or final pixel overlap.

The viewer's `ALD_RUNG_DIV = 32`, selector-only gate, and whole-height crossing
rules have no binary basis and must be removed. Generated rungs use the chart's
exact positive interval and interpolated properties. The recovered sample
producer is now implemented and tested in the clean-room C++ reconstruction.

Next exact task: connect the recovered checkpoint result-index predicate to a
binary-backed viewer judgement state, then continue the broader presentation
manager/cross-family inventory. Until that closes, render the main trace from
authored controls and checkpoint effects from exact generated samples,
keeping generated effects at their exact unresolved `0xff` state unless an
explicit input simulation and external result-table count are supplied. Label
external color/resource styling and the global chart-only Autoplay clip as
provisional product behavior.

### Issue 4 — Slide rendered checkpoint classes

[Issue 4: Slide checkpoints: two rendered classes](https://github.com/EukkMaru/chart-parsing/issues/4)

**Status: ordinary Slide visual classes, endpoint lifetime, resource-independent
path mesh, clipping, and three-Joint submission are recovered. Runtime phase
integration, external styling/final pixels, and HeavenHold remain open.**

The issue observes that some intermediate Slide controls look like visible
notes or trigger guide feedback while others appear to shape the path only.
`claim.note.slide-presentation-classes` now closes the class: ordinary nonfinal
SLD/SXD ending markers own a persistent endpoint resource, while SLC/SXC are
shape-only; the final endpoint is forced. The root is a separate resource.
This is independent of shared judgement results.

The root X bit selects an extended external root-resource branch. Exact field
8 style selects shared path/root resource families, with HLD retyped before
ordinary Slide construction. Optional field 9 uses the exact eight-string
decoder and selects result feedback only; it does not change the persistent
path or endpoint class. Player-facing resource names remain deliberately
unassigned.

`claim.note.slide-path-presentation-geometry` also closes the shared ordinary
Slide mesh. The binary builds adjacent straight endpoint segments, groups
longitudinal coordinates by authored/forced boundary markers, splits the first
raw-zero crossing, mode-culls past geometry, and clips all fields to projected
`[-600, 50]`. Its three streams are a full-width 6/18-vertex main mesh, a
fixed one-lane-wide center mesh, and a mode-1-only full-width overlay. Their
Joint callbacks run in construction order 0, 1, 2. Diagnostic categories
1/2/3 count triangles and are not layer identifiers.

Relevant closed anchors:

- parsing/continuation: `FUN_011c8870` and `FUN_011caf70`;
- generated path construction: `FUN_00b25510` to `FUN_00b1f0f0`;
- runtime factory/load: `FUN_00b28cc0` and `FUN_00c10720`;
- endpoint allocation/update: `FUN_00c0f8b0` and `FUN_00c0ee40`;
- shared segment/stream builder: `FUN_00c0a3d0` and split helper
  `FUN_00c02260`;
- JointSlide owner/order: `FUN_00bff510` and Joint constructor
  `FUN_00bfebb0`;
- keyed projected-position transform: `FUN_00b28890`;
- gameplay path/checkpoints/result-index write: `FUN_00c0de10`;
- field-9 feedback: `FUN_00c1c340` and `FUN_00c1b6c0`.

The viewer now keeps decoded style, root form, field-9 feedback code, and the
unresolved `0xff` endpoint state in its typed model. It draws binary-backed
marked endpoints independently of the debug shape-point toggle and places the
marker on the authored segment end, not the next segment start. Since original
resources are excluded, its white endpoint rule is explicitly an asset-free
presence placeholder, not a recovered glyph. The main Canvas ribbon now uses
the recovered endpoint-segment silhouette rather than ten fitted samples and
adds the exact one-lane-wide center-stream silhouette. Its colours remain
clearly labeled asset-free substitutes. Mode 1's overlay is withheld until
the viewer simulates the binary's start/path phases.

Next exact tasks: connect the recovered Slide phase machine to preview state,
trace HeavenHold presentation separately, and close the shared
camera/viewport path under issue 5. The base/alternate packed colours,
materials, shaders, and final pixel composition are external inputs, not
constants to infer from footage.

### Issue 5 — playfield, projection, and AIR geometry

[Issue 5: Viewer playfield geometry does not match cabinet proportions](https://github.com/EukkMaru/chart-parsing/issues/5)

**Status: confirmed fitted baseline; canonical relationships unresolved.**

The viewer currently uses fitted corridor/perspective, note-gap/thickness, AIR
overshoot, and AIR lean constants. They are not recovered simply because the
prototype names them as constants. The recovered scheduling/projection logic
must remain separate from viewport/camera mapping and original styling.

The gameplay checker not consuming an authored AIR/ALD height-like field does
not prove the renderer should ignore it: presentation load paths copy numeric
control fields into presentation objects. Trace their presentation consumers
before assigning or discarding a visual meaning.

Next exact task: recover resource-independent transform order, chart-space
lane/width/path geometry, primitive state, clipping/lifetime, and any numeric
parameter-selection logic. Treat matrices, viewport scale, and resource-only
values absent from the executable as configurable **Fitted** or **Product
choice** parameters. Compare near, middle, and far field positions at matched
chart time and speed; do not extract models, textures, or footage frames.

## Ordered implementation/research queue

1. **Make issue 1 true in the product.** Audit the viewer parser/model against
   the recovered SLP schedule rule and add a synthetic regression test.
2. **Close ALD presentation ownership.** Complete the focused trace described
   in issue 3, write a compact claim or presentation note, then implement its
   typed model and primitives.
3. **Close Slide visual discrimination.** Map parsed fields through the
   presentation consumer before replacing the viewer's heuristic classes.
4. **Separate projection layers.** Isolate chart-space/projected coordinates
   from camera/viewport transforms, then investigate issue 5 and calibrate only
   the unavailable final parameters.
5. **Normalize the prototype.** Move raw-token interpretation out of drawing,
   remove chart-specific assumptions, and make parser/model/render seams
   independently testable.
6. **Complete note-family slices.** Repeat parse/model/schedule/state/render
   verification for every constructible family and legacy/current form.
7. **Build the comparison matrix.** Cover all families, simultaneous and
   compound cases, tempo changes, near/mid/far positions, every observed
   keyword/variant, outliers, holdouts, and representative legacy corpus eras
   without committing reference content. Each reviewed chart must reach zero
   ambiguity under the per-chart proof rule.
8. **Run an independent product audit.** Apply every gate in
   `docs/COMPLETION.md` and perform a new binary saturation pass after the known
   queue is empty. Residual shape-affecting unknowns need documented external
   parameters or remain blockers.
9. **Prepare owner acceptance.** Present the complete comparison matrix,
   arbitrary-file workflow, and every residual fitted value to the owner. Only
   the owner can accept fidelity against actual gameplay footage/experience.

## Required verification record

For each slice or issue, leave enough durable evidence to reproduce the result:

- issue/slice and observed mismatch;
- exact evidence label for each rule or constant;
- parser/model/render owner and changed files;
- active claim/spec and stable Ghidra anchors, when applicable;
- minimal synthetic test and aggregate corpus result;
- matched footage comparison metadata in prose, without copied media;
- validation/build/test commands and results;
- remaining unknowns, fitted parameters, and next smallest action.

Update this roadmap when an issue changes state or its exact next anchor
changes. Put detailed evidence in claims/specs/tests and keep this document a
current index, not a chronological notebook. Use a temporary handoff only for
unfinished session-local details, then promote those details here or remove
them once resolved.

`research/VIEWER_COVERAGE.tsv` is the exhaustive work ledger. Keep binary
closure, product implementation, and human acceptance separate. A green test,
implemented renderer, or closed GitHub issue does not imply binary closure; a
binary-closed rule does not imply the current viewer implements it; and neither
implies the owner has accepted its visual result.

## Completion rule

The roadmap is not the completion checklist. `docs/COMPLETION.md` is normative.
Do not claim the viewer complete until every stage-two gate passes, including
arbitrary-file behavior, full note-family presentation, deterministic
seek/playback, asset independence, offline privacy, comparison coverage,
robustness, regression tests, issue resolution, and independent review.
The final experiential/footage review belongs to the owner, not automation or
the investigating agent.
