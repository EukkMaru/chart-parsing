# Converter howto

Three formats, one direction of trust: `c2s` (the game's text format,
never shipped), `cmr` (normalized system of record), `ilcmr` (the
human-writable authoring layer). Every converter is a Node ESM script,
no dependencies. Run everything from the repo root.

## Encode / decode

```
# c2s -> cmr (importer; the only tool that reads c2s)
node cmr/tools/c2cmr.mjs chart.c2s -o chart.cmr

# cmr -> ilcmr (decompiler; readable, comments preserved on hand files)
node ilcmr/tools/cmr2ilc.mjs chart.cmr -o chart.ilcmr

# ilcmr -> cmr (compiler)
node ilcmr/tools/ilc.mjs chart.ilcmr -o chart.cmr
```

There is no shipped cmr -> c2s CLI here; the emitter lives in
`cmr/tools/lib/emit_c2s.mjs` and exists to *prove* the round trip, not to
produce c2s for use. (The vendored chunimaru copy carries a
`cmr2c2s.mjs` wrapper for local debugging.)

A typical debug pair for one chart:

```
node cmr/tools/c2cmr.mjs   music2889/2889_03.c2s -o music2889/2889_03.cmr
node ilcmr/tools/cmr2ilc.mjs music2889/2889_03.cmr -o music2889/2889_03.ilcmr
```

Both outputs load in the viewer directly (drop or pick); parsing is
chosen by extension.

## Verify

```
node cmr/tools/cmrcheck.mjs chart.c2s [...]     # c2s <-> cmr round trip
node ilcmr/tools/ilcheck.mjs chart.c2s [...]    # ilcmr semantic fixed point
node scripts/verify_corridor.mjs chart.c2s      # viewer model equivalence
node scripts/verify_viewport.mjs chart.c2s      # identical draw calls
```

The bar is parsed-model equality under the viewer's reference parser —
never byte equality. All four gates pass 7,752/7,752 on the local corpus
(certified 2026-08-20); rerun them after touching any tool or the spec.

## Reading the output

- cmr is 1:1 with the record set; `stats.overrides` carries the source's
  authored `T_*` values verbatim (derivations unimplemented).
- ilcmr is mostly sugar; `!` lines are verbatim-cmr escapes the
  decompiler emits where a friendly spelling could change first-match
  association order (styled slides, junction crossings, attachment
  contention). ~0.6% of corpus records; expected, not an error.
- Warnings are loud by design: escape counts, snapped tuplets, clamped
  widths. Silence means clean.

## Viewer embedding

The viewer never calls these CLIs: `scripts/embed_corridor.mjs`
regenerates the corridor block inside `scripts/c2s-viewer.html` from the
same modules, and `scripts/sync_chunimaru.mjs` builds the deployed page
(cmr-only, c2s-scrubbed) plus the served `data/cmr` tree. Edit the
tools, re-run both generators, never the embedded copies.
