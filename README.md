# Offline C2S chart-gameplay viewer

This repository is building a clean-room, offline parser and gameplay viewer
for arbitrary `.c2s` charts. The target is faithful chart behavior—timing,
lanes, paths, generated elements, note lifecycle, playback, and inspection—
rendered with original code and visual primitives.

The current prototype is [scripts/c2s-viewer.html](scripts/c2s-viewer.html).
It is already useful, but it was initially implemented from repository docs
without direct binary access and is known to diverge from actual gameplay
footage in several shape/presentation details. The active phase reviews those
ambiguities against the completed gameplay reconstruction and focused Ghidra
decompilation to produce the most faithful canonical renderer the evidence
supports.

## Clean-room and asset boundary

`game.exe`, the Ghidra project, `music/`, `music.zip`, and gameplay footage are
local reference/test material only.

- The official and unofficial charts under `music/` are used for aggregate
  parser tests and human comparison. They are not shipped or embedded.
- The viewer must accept arbitrary user-selected `.c2s` files and must work
  without this repository's corpus.
- No original game artwork, textures, models, logos, audio, music, video, fonts,
  effects, server code, accounts, or player records belong in the product.
- Selected files are parsed locally; the viewer does not upload them.

In simple terms, the project recreates only the CHUNITHM-style chart-gameplay
surface, not the surrounding commercial game or services, and is not presented
as an official client.

## Start here

Agents and contributors should read [AGENTS.md](AGENTS.md) and the required
documents in its stated order. The durable product/issue handoff is
[docs/VIEWER_ROADMAP.md](docs/VIEWER_ROADMAP.md); the complete 12-issue and
temporary-handoff verdict is
[research/GITHUB_ISSUE_VERIFICATION.md](research/GITHUB_ISSUE_VERIFICATION.md).

Run the clean-room validation and reconstruction tests with:

```bash
python3 scripts/harness.py validate
python3 scripts/harness.py viewer-audit
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

Open `scripts/c2s-viewer.html` in a browser and select a local `.c2s` file to
use the current viewer. No chart is bundled into the page.

## Evidence foundation

Stage one statically reconstructed the exact local `game.exe` snapshot from
C2S ingestion through judgement outcome. Its durable evidence is in
[research/STATUS.md](research/STATUS.md),
[research/COVERAGE.tsv](research/COVERAGE.tsv), `research/claims/`, `spec/`,
and `include/chart/reconstruction.hpp`. All 28 gameplay rows passed an
independent closure audit.

Stage two uses that closed foundation to implement and verify the parser,
playback model, and renderer. Footage observations identify mismatches; exact
binary evidence decides gameplay/canonical shape rules when available. Fitted
or original presentation choices remain labeled as such.

Known issues are only starting leads. The investigation continues through an
independent binary saturation audit, and the final workflow is accepted by the
owner through comparison with real gameplay footage and gameplay experience;
automated tests do not substitute for that review.

The reference suite is coverage-driven across many local charts. A chart fails
review if any keyword is waved away as a typo, any primitive is drawn “just
because,” or any value is chosen only because it looks suitable. Each must be
traced to the exact binary behavior, or—when the numeric contents are external—
to the binary's loader/source, selection, and consumer with the substitute
clearly labeled.
