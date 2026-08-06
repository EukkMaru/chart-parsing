# Stage-two product workflow

## Preserve the closed research foundation

The 27 currently verified behavioral rows in `research/COVERAGE.tsv`, their
active claims, the normative `spec/` files, and
`include/chart/reconstruction.hpp` are the clean-room gameplay foundation.
`audit.closure` remains pending after the corrected ALD vector contradiction;
do not cite the superseded independent verdict. Do not reset the gameplay
ledger into a viewer task list or silently edit a verified rule to fit footage.

When new exact-binary evidence contradicts a verified rule, reopen only the
affected row, preserve the old claim as superseded history, correct the spec
and reconstruction, and rerun the stage-one gates. A viewer bug or aesthetic
disagreement alone does not reopen gameplay evidence.

## Strategy: parse, model, render, compare

The open GitHub issues seed the investigation; they are not a completeness
boundary. After resolving them, continue systematic class/field/call-path
coverage until the saturation procedure below produces no new relevant target.

### 1. Keep one normalized chart model

Raw token access belongs in the parser. The rest of the product consumes named,
typed structures for headers, authored events, chains, generated records,
schedules, diagnostics, and source locations. Do not let the renderer infer a
second command schema from field indices.

The logical separation applies even while the prototype is one HTML file:

- decode/tokenize/parse;
- normalize and connect chart records;
- finalize timing, projection, and generated paths;
- update deterministic playback state;
- build render primitives;
- draw UI and inspection overlays.

### 2. Complete vertical visual slices

Work one bounded behavior through the whole pipeline, such as “ASC carries the
AirSlide sampling cursor” or “which Slide controls render checkpoints.” Each
slice should account for:

- accepted and malformed C2S forms;
- normalized ownership/continuation;
- timing, lane, width, mirroring, and generated records;
- visibility start/end and seek reconstruction;
- geometry, layering, animation, and labels;
- inspector diagnostics;
- automated tests and representative local chart checks.

Prefer fixing the owning layer over adding chart names, song IDs, magic record
indices, or one-off canvas exceptions.

### 3. Compare against independent observations

For a human footage comparison, record in prose:

- the local chart identity without copying its contents;
- chart position or scheduled time and viewer seek state;
- playback/note speed and any viewer calibration values;
- the visible reference behavior;
- the viewer behavior;
- the size/direction of the discrepancy;
- whether the result is recovered, observed, fitted, or unresolved.

Use several charts and tempo/measure regions before promoting a fitted visual
constant. Footage can validate appearance and motion, but it cannot establish
invisible judgement logic or justify copying visual assets. Do not commit
screenshots, frames, clips, or audio from the reference.

### 4. Select many corpus references by coverage

Run an aggregate, content-free inventory over all local charts, then select a
large human-render suite by feature coverage rather than convenience. The suite
must collectively exercise:

- every observed keyword and field shape, including rare and legacy forms;
- every note/runtime presentation class and command variant;
- generated/authored controls, boundaries, styles, directions, and connections;
- lane/width extremes, mirroring, near/mid/far positions, tempo/projection
  changes, simultaneous/compound interactions, and large-chart stress cases;
- charts outside the initial investigative set as a holdout against overfitting.

Prefer a deterministic set-cover selection from aggregate feature signatures,
then add outliers and an owner-selected/randomized holdout. Do not write chart
contents into the repository or logs. “Sufficient charts” is coverage-based:
the suite must cover every known feature bucket, stop revealing new buckets in
a documented saturation pass, contain no failed chart, and be accepted as
sufficient by the owner. A fixed chart count alone is not evidence of breadth.

For each reviewed chart, maintain only local or aggregate identifiers plus an
ambiguity count. The pass condition is zero:

- undefined or casually corrected keywords;
- generic/heuristic/fallback primitives;
- visible configurations without a binary-backed selector and consumer;
- constants justified only by appearance;
- unclassified divergence from footage/gameplay experience.

Any nonzero count fails that chart and creates or reopens the narrowest parser,
model, presentation, external-configuration, or interaction row.

## Discrepancy triage

Classify every mismatch before changing code:

1. **Parser bug** — wrong command, field, default, continuation, or diagnostic.
2. **Model/schedule bug** — parsing is right but normalized timing/path state is
   wrong.
3. **Renderer bug** — model state is right but geometry, lifetime, layering, or
   drawing is wrong.
4. **Known external parameter** — the logic is recovered but a runtime value is
   absent; keep it configurable and document calibration.
5. **Research gap** — active claims/specs cannot explain the behavior; use
   focused static analysis and reopen evidence only when justified.
6. **Original product choice** — accessibility or presentation differs by
   design and is not described as reference behavior.

The category and evidence should appear in the issue/handoff so a visual fit is
not later mistaken for a recovered gameplay rule.

## Testing layers

- **Parser tests:** minimal synthetic lines/files, compatibility, malformed
  fields, source locations, and diagnostics.
- **Model tests:** normalized chains, schedules, mirroring, generated records,
  sorting, wrap/nonfinite domains, and deterministic seek state.
- **Renderer tests:** primitive counts/types, lane/time bounds, continuity,
  visibility, layering, and stable output metadata. Avoid copyrighted goldens.
- **Corpus checks:** local aggregate parsing of all `music/` charts, with no
  chart content written to the repository or test logs.
- **Reference-render checks:** a coverage-selected, growing corpus suite with a
  per-chart zero-ambiguity gate; automated output helps inventory primitives
  but does not approve their fidelity.
- **Human visual checks:** documented comparison matrix across representative
  note families, tempos, legacy versions, and difficult interactions.
- **Regression checks:** existing 34 C++ tests and `harness.py validate` stay
  green whenever shared gameplay/spec behavior changes.

Automation proves only the properties it asserts. Passing parser/model/render
tests cannot certify that a primitive has the reference shape, transform,
motion, lifetime, or layering. Keep those properties pending until their binary
paths are closed and the owner reviews representative output against real
gameplay.

## Binary presentation-closure loop

For every command variant and runtime view/note class:

1. Trace parsed fields into construction, generated records, and presentation
   state. Account for every consumed field and every visible output field.
2. Trace update, visibility, transform, geometry, animation/effect trigger,
   layering, reset, seek-equivalent reconstruction, teardown, and error paths.
3. Resolve vtable slots, callbacks, table dispatch, alternate branches, and
   relevant callers/callees. A familiar class name or one decompiled method is
   not closure.
4. Identify every external value. Recover its lookup key/index, selection,
   loader/source identity when exposed, default/failure behavior, units as
   supported by use, and all consumers.
   Keep unavailable contents parameterized and never manufacture a canonical
   number.
5. Record stable anchors, concise observations, reasoning, alternatives, and
   falsifiers. Implementation follows only after competing explanations are
   excluded by the binary.
6. Update `research/VIEWER_COVERAGE.tsv`, the owning claim/presentation note,
   the normalized model, renderer, and synthetic tests.

When all planned rows appear closed, conduct a fresh breadth-first saturation
audit starting from chart/view factories and presentation manager entry points.
Inventory every reachable gameplay-visible class, virtual target, resource-
independent transform, state owner, and configuration selector. Cross-check the
inventory in both directions: parsed field to visible consumer, and visible
primitive/state back to its chart or runtime producer. Any unexplained reachable
path, field, constant, or branch reopens work.

The viewer ledger uses fixed, independent states:

- `binary_status`: `open`, `partial`, `closed`, or `not_applicable`;
- `product_status`: `not_started`, `provisional`, `implemented`, or `verified`;
- `owner_review`: `pending`, `accepted`, or `rejected`.

`closed` requires the binary proof standard in `docs/EVIDENCE.md`.
`implemented` means the product follows that record and has focused tests;
`verified` additionally requires the applicable automated/corpus checks.
Neither changes `owner_review`. Put “heuristic,” “fitted,” and other evidence
labels in the evidence/next column rather than overloading lifecycle status.

## Session protocol

At session start:

1. Read `AGENTS.md`, the current status, and latest product handoff.
2. Run `python3 scripts/harness.py validate` and the applicable tests.
3. Inspect the current viewer before accepting a reported discrepancy.
4. Choose one bounded parser/model/render question and state its evidence class.
5. Consult `research/VIEWER_COVERAGE.tsv`; do not treat an empty issue queue as
   permission to stop while any binary or product row remains open.

At session end:

- update product-facing docs or issue notes;
- update specs/claims only when their evidence genuinely changed;
- run applicable parser/model/renderer and reconstruction tests;
- record local corpus/footage comparisons without copying source material;
- update `research/STATUS.md` product-stage summary;
- update `research/VIEWER_COVERAGE.tsv` with binary, implementation, and human-
  review status as separate dimensions;
- write a concise temporary handoff when work remains, then remove or promote
  old temporary handoffs once resolved.

The final session does not self-certify visual completion. Prepare the review
matrix and build for the owner; completion requires the owner's explicit
acceptance against real gameplay footage and experience.

## Multiple agents

Default to one active implementer. When parallel work is explicitly requested,
give agents disjoint layers or note families. Only one agent may mutate Ghidra
at a time. Avoid concurrent edits to the single-file viewer unless ownership is
split into nonoverlapping functions or the code has first been modularized.
