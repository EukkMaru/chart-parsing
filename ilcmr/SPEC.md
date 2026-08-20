# ilcmr — the authoring language above cmr

**Draft v1 — 2026-08-20.** Grammar fixed by owner rulings in the
2026-08-20 question sessions (three cards, twelve decisions, plus the
in-session amendment that became ruling 13). ilcmr is the
human-writable intermediate language of the chart stack:

```
chart.ilcmr  --ilc-->  chart.cmr  <--cmr tools-->  chart.c2s
```

ilcmr never targets c2s directly: the compiler (`ilc`) emits cmr, and all
association, attachment, and ordering semantics live in `cmr/SPEC.md`. The
decompiler (`cmr2ilc`) is heuristic in spelling but exact in meaning; both
ship in v1 (owner ruling) and are gated by the **semantic fixed point**:

> For every chart `C` in the corpus:
> `parse(cmr2c2s(ilc(cmr2ilc(C)))) == parse(cmr2c2s(C))`
> at the cmr compared-model level, including semantic record order.

Byte equality is explicitly NOT the bar — an authoring language has many
spellings for one chart. Meaning equality, always.

## 1. Design rulings (owner decisions, fixed)

| # | Axis | Ruling |
|---|------|--------|
| 1 | Note position | edge-span notation, always explicit (`6-10`), no sticky width |
| 2 | Non-dividing tuplets | snap to nearest tick with a loud per-onset warning |
| 3 | Air layer | glyph suffixes on the parent token (`^ v ^< ^> v< v>`) |
| 4 | Name | `ilcmr`, extension `.ilcmr` |
| 5 | Durations | `[div:count]` (simai spelling) |
| 6 | Slides | per-leg `>` chains, each leg with its own duration |
| 7 | Gimmick events | state-until-changed; compiler synthesizes spans |
| 8 | Escape hatch | `!` lines carrying one self-timed cmr record |
| 9 | v1 coverage | ground + air families get first-class sugar; CRUSH/TRACE/HEAVEN/KEYZONE unsugared in v1 (amended by 13) |
| 10 | Air heights | `@` + decimal (`@0.8`) |
| 11 | Chords | `/` separator |
| 12 | Tooling | compiler AND decompiler in v1, corpus fixed-point proof up front |
| 13 | Second-class types | every family has a **canonical in-language form** (§6.8, §7); `!` is raw injection only, never the canonical home of a family |

## 2. Lexical

- UTF-8, LF canonical. Comments `//` to end of line (inherited from cmr).
- Whitespace and newlines are insignificant in the body, except inside
  `&` header lines and `!` escape lines (both are line-oriented).
- The file is: a header block of `&` lines, then one body stream.

## 3. Header

```
&title   <verbatim to EOL>
&artist  <verbatim to EOL>
&creator <verbatim to EOL>
&difficulty <raw string>       // cmr rules: "00" != "0"
&level   <raw string>
```

All header keys are optional; absent keys take the cmr §3 reset defaults.
String-valued keys are verbatim to end of line — `//` does NOT start a
comment on a `&` string line (cmr's comment-stripping-disabled rule).
Unknown `&` keys pass through to the cmr `[meta]` extension with a warning.

**Derived meta is forbidden**, not optional: group-3 stats, timing
summaries, note counts. The compiler owns all of it. An `&` key naming a
derived stat is a hard error. (This is the "omitted meta" requirement: an
authored file simply has no stats to be wrong about.)

Initial BPM comes from the first `(bpm)` event, not the header.

## 4. Time model

- Time starts at 0:000. A measure is always **384 ticks** (the c2s grid);
  METER affects the displayed bar/beat grid only, never the tick length of
  a measure.
- `{n}` sets the division: each comma advances **384/n ticks**. `{8}` = 48
  ticks, `{12}` = 32, `{96}` = 4.
- Position is tracked as an exact rational internally. Only *emitted
  records* snap to the nearest integer tick. So `{5}` works: five commas
  return exactly to the next measure line (no cumulative drift), and each
  onset inside the quintuplet snaps (0, 76.8→77, 153.6→154, 230.4→230,
  307.2→307) with one warning per snapped onset naming the drift (ruling
  2). What compiles is what plays; the warnings say where they differ.
- `|` is the **bar check**: emits nothing, hard error if the current
  position is not a multiple of 384. Sprinkle them; they are the
  difference between catching a dropped comma at line 40 and producing two
  hundred measures of drift.
- Tokens between two commas all occur at the current position; the comma
  then advances. `(events)` may appear anywhere between commas and apply
  at the current position.

## 5. Spans

A span is `a-b` with integer edges `0 <= a < b <= 16`: the ground's 16
cells have 17 edges. `L<lane> w<width>` in cmr = span `lane-(lane+width)`.

- width = `b - a` — no off-by-one anywhere
- full field `0-16`, halves `0-8` / `8-16`
- mirror of `a-b` is `(16-b)-(16-a)`
- adjacency is visible: `2-6` and `6-10` touch

`b <= a` is a hard error. Spans are always explicit (ruling 1): no sticky
width, no bare left edge. A line means the same thing pasted anywhere.

## 6. Notes

### 6.1 Sigils (v1 first-class set, ruling 9)

| token | c2s | notes |
|-------|-----|-------|
| `2-6` | TAP | bare span |
| `x2-6` | CHR (EXTAP) | subtype tag §6.2 |
| `f2-6` | FLK (FLICK) | auto `L` extra, tag overrides |
| `d2-6` | MNE (DAMAGE) | |
| `h2-6[4:2]` | HLD (HOLD) | |
| `hx2-6[4:2]` | HXD (EXHOLD) | tag = `fx` word |
| `s...` | SLD (SLIDE) | chain, §6.3 |
| `sx...` | SXD (EXSLIDE) | chain, §6.3 |
| suffix `^ v ...` | AIR family | §6.4 |
| suffix `^h` / `^hx` | AHD / AHX | §6.5 |
| suffix `^s` | ASC/ASD (AIRSLIDE) | §6.6 |

The language has three tiers (ruling 13):

1. **Sugared** — the table above: terse spellings for what hobby charters
   reach for constantly.
2. **Canonical keyword forms** — §6.8 and the §7 event table: every
   remaining family spelled in-language with ilcmr conventions (spans,
   `@decimal` heights, `[div:count]` durations, stream timing). Verbose,
   but real ilcmr — a wall chart is still an ilcmr chart. This is what
   the decompiler emits.
3. **`!` raw injection** (§8) — the totality fallback for the residue no
   in-language form can express exactly.

### 6.2 The `.tag` — binary-ignored extras

Uniform mechanism for cmr's `note <T>` / `fx <word>` clauses:

- `x2-6` = CHR subtype `UP` — the corpus-dominant word (593,624 of
  780,281 CHR records).
- `x2-6.CE` = subtype `CE`; any word verbatim (`DW/CE/RC/LC/RS/LS/BS`
  and anything future).
- `x2-6.` (trailing dot, empty tag) = **no** fifth token (13,522 corpus
  records).
- `f2-6` = FLK with the corpus-universal `L`; `f2-6.T` / `f2-6.` as above.
- `hx2-6[4:2].W` = EXHOLD with `fx W`.

The tag is a spelling of the c2s trailing token, not a behavior: the
binary ignores these fields (cmr §5.1). Compile emits them into the cmr
`note`/`fx` clause; decompile emits the shortest spelling (bare when the
token equals the family default).

### 6.3 Slides

```
s2-6[8:1]>6-10[8:1]>10-14
sx2-6[4:1]>2-6            // EXSLIDE held in place one beat
```

Head span + duration, then `>` legs: each `>` consumes the pending
duration to reach its target span, and carries the next leg's duration
(the final target has none). Each leg is one cmr arrow step; the chain
compiles to the cmr SLIDE/EXSLIDE block with cmr's own chain rules
(HLD-styled retype, decoded style codes) untouched. Styled slides and
mid-chain style/fx changes are escape-only in v1.

### 6.4 Air suffixes (ruling 3)

`^` `^<` `^>` `v` `v<` `v>` map to cmr `+air:U UL UR D DL DR` (c2s
`AIR AUL AUR ADW ADL ADR`). The suffix sits after the base token,
including after its duration:

```
6-10^          // tap + up air
h2-6[4:2]^>    // hold + up-right air
```

The compiler emits the AIR record at the owner's **attachment point** per
cmr §5.4 — hold end, chain final, own time for point notes. The charter
writes the air on the note; the machine timing is the compiler's job.

### 6.5 Air-holds

```
6-10^h[4:1]              // AHD attached, one beat
6-10^hx[4:1]             // AHX: end action judged
6-10^h[4:1]~[4:1]~[2:1]  // three-segment AHD chain
6-10^h@0.5[4:1]          // at height 0.5
```

`^h` ends plain (AHD), `^hx` ends with the judged air action (AHX) — the
distinction cmr spells with `~>` vs `->` on the end step. `~` chains
additional plain segments. Height `@<decimal>` after the sigil, default
1.0; heights on air-holds must be integer tenths (cmr canon) — `@0.85`
here is a hard error. Mixed AHD/AHX interior joints are escape-only in v1
(open item: corpus survey of whether they exist at all).

Attachment follows cmr §5.3: the parent must be one of the six point/
sustain families (types 0/1/2/4/6/11). A `^h` on anything else is a
compile error.

### 6.6 Air-slides

```
6-10^s[8:1]>2-6@0.8[8:1]>10-14@0.0
```

Head `^s` with optional start height `@<decimal>` (default 1.0), then
legs like ground slides but each target span takes an optional `@h`;
omitted = carry the previous height. AIRSLIDE is the one family where
heights may be arbitrary decimals (cmr: raw floats, not normalized).
Discontinuous height jumps at a junction (cmr `[from h<v>]`, 583 corpus
records) are escape-only in v1.

### 6.7 Chords

`/` joins tokens at the same position (ruling 11):

```
2-6/10-14, h2-6[4:1]/x10-14^, ,
```

Written order is semantic: it becomes cmr's within-tick source order
(which cmr declares semantic in both directions). Secondaries (airs,
air-holds, air-slides) always emit immediately after their owner, per cmr.

### 6.8 Second-class canonical forms (ruling 13)

Keyword tokens, stream-timed like any note, chords and `>` chains as
usual. Heights on CRUSH/TRACE/HEAVEN must be integer tenths (cmr canon);
TRACE carries its two verticals as an `@a:b` pair.

```
crush 0-4@0.1 every 12 style RED [4:2] > 8-12@0.5
                          // ALD: start span+height, grid period in ticks,
                          // decoded style word, leg to end span+height
crush 2-6@0.5 every 0 style NON [4:1] > 2-6@0.5
                          // the HeavenHold class selection (cmr: every 0,
                          // style NON)
trace 2-6@0.5:0.8 [4:1] > 6-10@0.5:0.8        // ASO, hA:hB pair
heaven 2-6@0.5 disc 3 [4:1] > 6-10@0.5        // HHD
heavenx 2-6@0.5 disc 3 [4:1] > 6-10@0.5 .W    // HHX; .tag = fx word
keyzone 2-6 key 3 [4:2]   // SLA at this stream position (SLA's source
                          // position is semantic in cmr; the stream slot
                          // IS that position)
```

Multi-leg chains chain `>` steps exactly like slides; each leg carries
its own span, heights, and (for CRUSH) optional `style`. cmr's chain
semantics — decoded style codes, single-record inline `until` vs
header+steps — are the compiler's concern: it picks the cmr shape that
round-trips, the charter never sees it.

## 7. Events (ruling 7)

Parenthesized, inline, applied at the current stream position:

| event | c2s | semantics |
|-------|-----|-----------|
| `(170)` | BPM | sugar for `(bpm 170)` |
| `(bpm 170)` | BPM | tempo from here |
| `(meter 4/4)` | MET | grid meter from here; mid-measure allowed (c2s allows it), warning issued |
| `(speed 2.0)` | SFL | **state-until-changed**: opens a span from here to the next speed event or chart end; the compiler synthesizes the span records |
| `(speed off)` | — | closes the current span, no coverage until the next `(speed v)`; how decompiled charts spell SFL gaps |
| `(stop [4:1])` | STP | freeze of that tick-duration at this position; the comma stream is unaffected (STP stretches real time, not the grid) |
| `(click)` | CLK | click marker at this position |
| `(speed 2.0 key 3)` | SLP | keyed scroll state, per-key state-until-changed; `(speed off key 3)` closes it. Key presence = SLP, exactly cmr's rule |
| `(depth 2.0)` | DCM | projection-factor state-until-changed, `(depth off)` closes; DCM is source-order sensitive and the stream order IS the emitted order |
| `(scroll-dead 2.0)` | SFE | corpus-absent, recognized-then-rejected by the binary; state-until-changed for completeness |

`(speed 1.0)` emits a real explicit 1.0 span — it is not a no-op — so
charts whose c2s carries redundant neutral SFL records survive the fixed
point. Overlapping source spans (same key) cannot be expressed as state
and decompile to escape lines.

## 8. Escape hatch (ruling 8)

A `!` line carries exactly one **self-timed** cmr record body — a full
cmr note/event line including its `measure:tick` position, independent of
the surrounding stream position:

```
! 18:192 CRUSH 0 w16 h5 until 20:000
```

Anything cmr can spell is legal here; this is the totality guarantee.
With ruling 13 the escape's scope is only the residue: styled slides,
AIRSLIDE height jumps (`from h<v>`), mixed AHD/AHX interior joints,
same-key overlapping spans, unknown commands — plus deliberate raw
injection by charters who think in the machine model. The decompiler's
priority order is **sugar > canonical keyword form > `!`**, and it
reports per-family escape counts so residue is visible, never silent.

Compiled record order: the compiler collects all records, orders by tick
with stream-encounter order breaking ties (escapes slot by their stamped
time, tie-broken by their position in the stream). The decompiler places
escape lines at the comma boundary matching their time, preserving source
relative order, so the fixed point holds through escapes.

## 9. Diagnostics

Hard errors: span with `b <= a`; bar-check failure; unknown sigil; `^h`
on an invalid parent family; non-tenth height anywhere but AIRSLIDE (airs, air-holds, crush/trace/heaven);
derived-stat `&` key; malformed escape line (escape bodies are parsed
with the full cmr grammar, never passed through blind).

Loud warnings (never silent): every snapped onset from a non-dividing
`{n}` (with the tick drift); mid-measure `(meter)`; unknown `&` key
passthrough; decompiler escape fallback counts per family.

## 10. Worked example

```
&title  Example
&artist Somebody

(170) {8}
2-6, 2-6, 2-6, 2-6,                    // the jack: four 8ths, done
10-14, 10-14, 10-14, 10-14, |
{4} (speed 2.0)
h2-6[4:2]/x10-14^, , 6-10^h[4:1], , |
(speed 1.0)
s2-6[8:1]>6-10[8:1]>10-14^, , {8} f4-12, , |
crush 0-16@0.5 every 12 style RED [1:2] > 0-16@0.5, |
```

## 11. Open items

- Corpus survey: do mixed AHD/AHX interior joints exist? If not, `^h`/
  `^hx` covers everything and the escape note in §6.5 dies.
- Verify default air heights (1.0 assumed for `^h`/`^s` heads) against
  corpus distributions before the compiler hard-codes them.
- Styled-slide frequency: if styled SLD chains are common enough,
  v2 needs first-class spelling; measure before deciding.
- Wall *sugar* for v2: the §6.8 canonical form is complete but verbose;
  dense ALD grids may deserve a generator syntax. Measure real wall
  charts first.
- Decompiler division inference: per-measure `384/gcd(onsets)` is the
  baseline; evaluate hysteresis (avoid `{n}` churn between measures) on
  the corpus before fixing the canonical output.
- Linter/LSP (formatter with bar-check awareness, error positions) —
  after both tools prove the fixed point.
