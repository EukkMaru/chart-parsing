# The cmr & ilcmr formats — a guide

*Formal guide, 2026-08-20. The normative grammars live in `cmr/SPEC.md`
and `ilcmr/SPEC.md`; where this guide and a spec disagree, the spec wins.
Tool usage is in `cmr/tools/HOWTO.md`.*

---

## 1. Why two formats

**cmr** is the normalized chart language: the system of record. Every
construct maps 1:1 onto the runtime record set, deterministically, in
both directions. It answers "what exactly is in this chart" in words a
person can read — `EXTAP`, not a bare four-letter token; `until 12:336`,
not a tick count buried in field six.

**ilcmr** is the authoring layer above it: comma-per-beat rhythm text in
the simai tradition. It answers "let me write a jack in one line". Many
ilcmr spellings mean the same chart; the compiler resolves them onto cmr,
where exactly one spelling exists.

```
chart.ilcmr  --ilc-->  chart.cmr  <--(dev-side proof only)-->  upstream
```

Both formats are **certified**: converting the entire 7,752-chart local
corpus through cmr and back is model-equal under the recovered reference
parser, and the ilcmr round trip holds a semantic fixed point. The
viewer builds its model natively from cmr.

## 2. Shared concepts

- **The grid.** A measure is always 384 ticks. Time is written
  `measure:tick` (`12:336`); tempo (BPM) decides how long a tick lasts,
  meter (METER) only paints the beat grid.
- **The field.** Sixteen ground lanes, `L0` (left) through `L15`, each
  note carrying a width in lanes. ilcmr instead writes the *span* between
  lane edges: `L6 w4` in cmr is `6-10` in ilcmr — edge 6 to edge 10,
  width = difference.
- **Families.** Point notes (`TAP`, `EXTAP`, `FLICK`, `DAMAGE`), holds
  (`HOLD`, `EXHOLD`), slide chains (`SLIDE`, `EXSLIDE`), the air layer
  (`+air` suffixes, `AIRHOLD`, `AIRSLIDE`), air walls (`CRUSH`), and the
  exotic trio (`TRACE`, `HEAVEN`, `KEYZONE`).
- **Association is positional.** Chains continue where the previous
  segment ended; air secondaries attach to the first compatible root at
  the exact attachment point, and every root has exactly one secondary
  slot. Record order is therefore semantic — both formats preserve it.
- **Attachment points.** An air rides its owner's *current endpoint*:
  a tap's own position, a hold's `until` point, a chain's final control.

## 3. Reading and writing cmr

A `.cmr` file is three sections behind a version pragma. Comments run
`//` to end of line (they do not survive conversion, except that files
served to the viewer are displayed verbatim).

```
cmr 1

[meta]
version: 1.13.00 1.13.00
music: 9000
difficulty: 00
creator: whoever
bpm-default: 140 140 140 140
meter-default: 4/4
stats: derived

[events]
BPM 0:000 140
METER 0:000 4/4
SCROLL 6:000 x2 until 7:000
STOP 7:000 until 7:048

[notes]
@0
TAP 000 L0 w4
EXTAP 096 L4 w4 note UP
HOLD 192 L8 w4 until 1:096 +air:U
@1
SLIDE 192 L12 w4
  ~> 2:000 L8 w4
  -> 2:192 L4 w4 end +air:UR
```

Things worth knowing:

- **`[meta]`** uses friendly lowercase keys; `difficulty:` and
  `creator:` are raw strings (comment stripping is off for them).
  Statistics are never authored: `stats: derived` declares the converter
  owns them, and `stats.overrides:` records any authored values from an
  imported chart verbatim.
- **`[events]`** lines are self-timed and stay in source order (the
  depth query is order-sensitive). Spans end with `until`, factors are
  `x2`, `x-0.5`; a keyed scroll writes `key <n>` and pairs with a
  `KEYZONE` region that assigns that key to the notes inside it.
- **`[notes]`** opens measure blocks with `@N`; note lines then give
  only the tick. Any position may still be written in full `m:t` form,
  and `until`/chain steps always are.
- **Chains** are a header (the start) plus indented steps (each step is
  one record's *end*). The arrow is the record's spelling: `->` marked
  (judged), `~>` shape-only. The last step says `end`. Arrow families
  always use a block even for a single record; `CRUSH`/`TRACE`/`HEAVEN`
  write single records inline with `until`.
- **`style` / `fx`** carry source tokens verbatim — the known
  vocabularies are the game's decode tables, not the grammar. A step
  omits the clause to inherit the header, or writes `style none` for
  explicit absence.
- **`+air:U|UL|UR|D|DL|DR`** rides the owner line (or the `end` step)
  and emits at the owner's attachment point. An air that cannot attach
  is written as an explicit `AIR:U ... orphan` diagnostic line.
- One comment is machine-written: a note whose runtime family differs
  from its keyword (an `HLD`-styled slide becomes a heaven hold) gets a
  `// -> heaven hold` retype note.

## 4. Reading and writing ilcmr

ilcmr is a token stream over the same grid. Whitespace and newlines are
free; commas advance time.

### Time

```
(140) {8}                    // bpm 140; eighth-note steps (384/8 ticks)
2-6, 2-6, 2-6, 2-6,          // four eighths on span 2-6
10-14, 10-14, 10-14, 10-14, |
```

`{n}` makes each comma advance `384/n` ticks. `|` is a bar check: it
emits nothing and errors unless you are exactly on a measure boundary —
sprinkle them, they catch dropped commas immediately. Divisions that do
not divide 384 (`{5}`, `{7}`) are legal; onsets snap to the nearest tick
with a loud warning each.

### Notes

| token | meaning |
| --- | --- |
| `2-6` | tap on span 2–6 |
| `x2-6` | EXTAP (`.CE` tags another subtype; bare = `UP`; trailing `.` = none) |
| `f2-6` | flick (carries the universal `L` extra) |
| `d2-6` | damage (mine) |
| `h2-6[4:2]` | hold, two quarter-notes long |
| `hx2-6[4:2].W` | EX hold, `.tag` = feedback word |
| `s2-6[8:1]~6-10[8:1]>10-14` | slide: `~` shape leg, `>` judged leg |
| `sx…` | EX slide |
| `2-6/10-14` | chord: both at this comma |

Durations are simai-style `[division:count]`: `[4:2]` is two
quarter-notes regardless of the current `{n}`.

### The air layer

```
6-10^          up air        6-10^> up-right      6-10v  down air
h2-6[4:2]^     air fires at the hold END (attachment point)
6-10^h[4:1]    air-hold above the note, one beat (plain end)
6-10^hx[4:1]   air-hold whose end is a judged action
6-10^s@5[8:1]>2-6@7[8:1]~10-14@5   air-slide: spans with @heights
```

Air-holds chain plain segments with `~[d:c]`; air-slide legs use `>`
(judged) and `~` (shape) like ground slides, heights carrying forward
when omitted.

### Events and second-class forms

```
(180)               (meter 3/4)          (click)
(speed 2.0) … (speed off)                // scroll span, state-until-changed
(speed 2.0 key 1)                        // keyed scroll (pairs with keyzone)
(stop [4:1])        (depth 1.5) … (depth off)

keyzone 0-8 key 1 [1:4]
crush 0-16@1 every 48 style RED [1:1] > 0-16@5
trace 2-6@0.5:0.8 [4:1] > 6-10@0.5:0.8
heaven 6-10@5 disc 0 [1:1] > 6-10@5
```

Speed/depth are states: they open a span at the current position and the
next change (or `off`) closes it. The keyword forms are verbose but real
ilcmr — walls and zones never force you out of the language.

### Escapes

A `!` line carries one verbatim cmr line, self-timed:

```
! SLIDE 18:000 L0 w8 style HLD
! -> 18:192 L0 w8 end
```

The decompiler emits escapes wherever a friendly spelling could change
first-match association (styled slides, crossing chains, contended
attachments) — about 0.6% of corpus records. In hand-written files you
will rarely need one.

## 5. Guarantees

- **cmr round trip**: import → emit reparses to an identical model
  (records, chains, attachments, regions, tempo/meter, header), proven
  corpus-wide. Byte equality is *not* promised; meaning is.
- **ilcmr fixed point**: decompile → recompile is model-identical to the
  original, corpus-wide.
- **Viewer equivalence**: the viewer's cmr-built model and its draw-call
  stream match the reference path frame-for-frame, corpus-wide.
- Warnings are never silent: snapped onsets, clamped widths, escape
  counts, dropped orphans all print.

## 6. Pointers

- `cmr/SPEC.md` — normative cmr grammar (owner decisions logged inline)
- `ilcmr/SPEC.md` — normative ilcmr grammar and the thirteen rulings
- `cmr/tools/HOWTO.md` — converter and gate commands
- `cmr/demos/music900x/` — four commented tutorial charts, also served
  by the site's chart list
