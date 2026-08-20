# The cmr corridor tools

Certified 2026-08-20: the bidirectional c2s <-> cmr conversion is
model-equal over all 7,752 local charts (cmrcheck, reference = the
viewer's parseC2s + association pass). cmr is therefore the system of
record; c2s exists only at the archival boundary.

## Deployment boundary

**Deployment-safe** (knows only cmr/ilcmr; may ship):
- `lib/ast.mjs` — cmr AST, canonical cmr emitter, shared tables
- `lib/parse_cmr.mjs` — strict cmr parser (the viewer port grows from this)
- `../../ilcmr/tools/ilc.mjs` — ilcmr -> cmr compiler
- `../../ilcmr/tools/cmr2ilc.mjs` — cmr -> ilcmr decompiler

**Dev-side only** (touches c2s; NEVER ship, never fetch at runtime):
- `lib/c2s_read.mjs`, `lib/assoc.mjs`, `lib/emit_c2s.mjs`
- `c2cmr.mjs` — the one-time importer (c2s -> cmr)
- `cmr2c2s` direction lives in `lib/emit_c2s.mjs` and exists only so the
  round trip can be proved; deployment never converts back
- `reference.mjs`, `cmrcheck.mjs`, `../../ilcmr/tools/ilcheck.mjs` — gates

## Usage

```
node cmr/tools/c2cmr.mjs chart.c2s -o chart.cmr      # import (dev)
node cmr/tools/cmrcheck.mjs charts...                # corpus proof (dev)
node ilcmr/tools/cmr2ilc.mjs chart.cmr -o chart.ilcmr
node ilcmr/tools/ilc.mjs chart.ilcmr -o chart.cmr
node ilcmr/tools/ilcheck.mjs charts...               # fixed-point proof (dev)
```

## v1 notes

- All authored `T_*` statistics are recorded as `stats.overrides` verbatim
  (no derivations implemented yet); model equality never depends on them,
  and an ilcmr round trip drops them (authoring files own no stats).
- ilcmr sugar defaults are corpus-anchored: bare `x` = CHR `UP`, `f` = FLK
  `L`, `s`/`sx` = style code 0 (absent = `SLD`), `^s` = `DEF`, fx default
  `UP`. Everything outside defaults uses canonical keyword forms or `!`
  escape lines (priority: sugar > keyword form > escape).
- Ordering-sensitivity escapes (cmr2ilc): the reference association is
  first-match over arrival order, so the decompiler escapes constructs
  whose emission order could flip an attachment: secondaries contending
  for the same (point, root family); air-hold/air-slide chains sharing a
  junction point; slide chains sharing a path point once any member of
  the colliding group is escaped (all-sugar groups keep item order and
  stay sugar). Escapes are placed at their slot, breaking the measure
  line, so within-tick order survives.
- Grammar additions made during implementation, pending owner blessing in
  the ilcmr spec: `>`/`~` marked-vs-shape legs on slides and air-slides
  (mirrors the cmr arrows); ` / ` spaced separator when a slot chords a
  keyword form; `.TAG` charset is uppercase.
