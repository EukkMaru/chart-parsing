# The cmr chart format, version 1

cmr is the normalized chart language of this project: the same numbers as
c2s, arranged so a person can read them. Every construct maps deterministically
onto the exact c2s record set and back; the recovered parser semantics in
`spec/` remain the ground truth for what those records mean. This document is
the product spec for the format and its converters. It is not a claim about
`game.exe`; where a representation choice rests on inference rather than the
binary, it is marked and ledgered in GitHub issue #12.

Every design decision below was made explicitly by the owner (2026-08-07
question sessions, 28 decisions), with edge-case machinery contributed by a
three-way design panel.

**Revised 2026-08-20** against the corpus-grounded spec audit (15 confirmed
findings, 5-cluster adversarially-verified sweep): air suffixes emit at the
owner's ATTACHMENT POINT (hold end, chain final — not "the owner's time");
arrow-carrying families always use one-step blocks for single records so
lone SLC/AHD-vs-AHX/ASC-vs-ASD spellings survive; `style`/`fx` carry any
verbatim token (the vocabularies are decode tables, not grammar; corpus
authors `SXD` in the style slot); AIRSLIDE steps gain `from h<v>` for the
583 corpus height-jump junctions; AIRHOLD/AIRSLIDE lose direct type-13
attachment (via HLD-styled slides only) while suffix AIRs gain CRUSH/TRACE/
HEAVEN roots; HEAVEN/TRACE adopt the uniform chain layout; string meta keys
run verbatim to EOL with comments disabled; `difficulty:` is a raw string;
meter fractions allow zero/signed components on METER and meter-default
alike; binary-ignored corpus extras (CHR subtype, FLK `L`, AHD label) are
preserved behind `note <T>`; emitted record order is semantic and preserved;
the emit-then-reparse gate covers the full association pass; the compared
model is enumerated; the unreachable `from <m:t>` epsilon escape and the
air-hold step L/w escape are retired. Owner decisions 2026-08-20: one-step
blocks, `note` preservation, verbatim-to-EOL strings, uniform chain layout.

**Revised 2026-08-11** against the post-decomp canon: the HEAVEN family
gains its recovered layout (per-endpoint verticals, lateral endpoint,
`disc`, and `fx` replacing `xtok`), TRACE's properties become the recovered
shell heights `hA`/`hB`, slide step width follows the six-field legacy
presence rule, the attachment table gains type-13 roots and loses the
free-standing `NONE`, METER's field order graduates from corpus-inferred to
recovered, and DEPTH's query-only semantics are documented. Grammar shape,
keywords, and every owner decision are unchanged.

## 1. Files

- **`chart.cmr`** — the whole chart in one file, sections in order:
  `[meta]`, `[events]`, `[notes]`.
- **`chart.cmt` / `chart.cev` / `chart.cnt`** — the split form: each file is
  exactly one section's body (no `[section]` header line). `join` concatenates
  them with headers; `split` is its inverse. Both forms are the same language.
- First non-blank, non-comment line of a `.cmr` file (and of a `.cmt`) is the
  required version pragma: `cmr 1`. A missing or unknown version is a hard
  error.
- Encoding UTF-8, LF line endings canonical. Comments run from `//` to end of
  line and are authoring sugar: they do not survive conversion to c2s.
- The parser is **strict**: an unknown keyword, malformed field, or
  out-of-place line is a hard error with file, line, and a suggestion when one
  is close (`SLIED` → `did you mean SLIDE?`). We own this format; there is
  never a legitimate unknown. (This is the SLP lesson made policy.)

## 2. Lexical conventions

- **Time** is `measure:tick` on the 384-tick fixed grid (`RESOLUTION` is
  recognized-but-ignored by the binary; cmr inherits the fixed grid). Ticks
  are written zero-padded to three digits in canonical output (`49:193`,
  `2:000`); hand-written unpadded ticks are accepted.
- Inside `[notes]`, a `@M` measure-block header sets the current measure and
  note lines give only the tick. `measure:tick` is accepted anywhere a bare
  tick is, and is required for any position outside the current block
  (`until`, chain steps).
- **Lane and width**: `L<lane> w<width>` — the two c2s numbers verbatim.
  Lanes 0–15 canonical (integers outside survive verbatim; the binary allows
  them), width 1–16 (the binary clamps; the converter clamps identically and
  warns on out-of-range authored values).
- **Heights** (air verticals): `h<value>`, the authored number verbatim.
  Tenths storage is family-specific: CRUSH/TRACE/HEAVEN parse to integer
  tenths, AIRSLIDE keeps raw floats — the converter must NOT normalize
  AIRSLIDE heights. `h1` is the lane surface, `h5` the ordinary air height.
- **Durations** never appear as raw tick counts in cmr: spans end with
  `until <measure:tick>` (always the full form, even same-measure). The
  converter derives the c2s duration.
- Numbers are written minimally (`180` not `180.000`, `h7.5` not `h7.50`);
  value equality is what round-trips, not digit strings. Factors are signed
  (`x-0.025`); a `until` at or before the start is legal and emits the signed
  or zero duration verbatim (zero-length ASC spans exist in the corpus).
- Authored positions are canonicalized by floor division onto `measure:tick`
  with `0 <= tick < 384` (model-equal; the binary canonicalizes the same
  scalar).
- c2cmr numeric-token handling follows the recovered fallback: a missing
  field is the zero default; a numeric prefix with trailing text converts
  with a warning; a no-conversion or out-of-range token is a HARD ERROR (the
  binary's load fails there too).
- cmr2c2s emits single-TAB-delimited fields with CRLF line endings — the
  only corpus-attested shape. The reference tokenizer accepts a superset;
  emission does not rely on that tolerance.

## 3. `[meta]`

Fully modeled, friendly lowercase keys. The header commands map:

| cmr key | c2s command | notes |
| --- | --- | --- |
| `version:` | `VERSION` | both tokens, space-separated |
| `music:` | `MUSIC` | |
| `sequence:` | `SEQUENCEID` | |
| `difficulty:` | `DIFFICULT` | the raw STRING verbatim (`00` ≠ `0`; corpus has `UNKNOWN`); exempt from minimal-number formatting |
| `level:` | `LEVEL` | |
| `creator:` | `CREATOR` | raw bytes to end of line — comment stripping is DISABLED on string-valued meta lines (corpus: `CREATOR .jack//Roots`); leading/embedded spaces and an empty value are preserved |
| `bpm-default:` | `BPM_DEF` | four values |
| `meter-default:` | `MET_DEF` | count/unit fraction with the SAME flip as `METER` (`MET_DEF 8 6` ↔ `meter-default: 6/8`; 62 corpus charts are asymmetric); zero/negative components legal |
| `resolution:` | `RESOLUTION` | recognized-but-ignored by the binary; emitted for compatibility |
| `clock:` | `CLK_DEF` | |
| `progjudge-bpm:` | `PROGJUDGE_BPM` | |
| `progjudge-aer:` | `PROGJUDGE_AER` | |
| `tutorial:` | `TUTORIAL` | |

**Statistics are derived, not stored.** The binary's own derived-summary pass
clears the complete 45-integer group-3 destination without reading it and
rebuilds it from finalized parsed records (`spec/c2s.md`), so authored `T_*`
values never reach the runtime model. The registered vocabulary is the exact
91-name descriptor registry (claim.parser.command-descriptor-schema) —
nothing is prefix-matched, and an unregistered `T_*` spelling is an unknown
command like any other. `cmr2c2s` regenerates the registered
`T_*` block by implementing that derivation. Where a source chart's authored
values disagree with the derivation, `c2cmr` warns and records the
disagreeing values as explicit overrides:

```
stats: derived
stats.overrides:
  T_JUDGE_FLK: 88   // derived 86
```

Overrides exist so re-emitted files can diff cleanly against originals; model
equality never depends on them. Honesty note (2026-08-20): the per-name
derivation formulas are NOT recovered canon — the binary's derived-summary
pass is proved to overwrite, but its arithmetic is a cmr-owned convention
implemented in the converter and marked as such; any name whose derivation
is unimplemented is emitted from the override verbatim. Canonical `[meta]`
also records the source's `T_*` NAME SET when it differs from the full
registered-plus-legacy set (`stats.absent:` list), so re-emission reproduces
exactly the names the source carried — 4,805 corpus charts omit the
`T_CHRTYPE_RC/LC/RS/LS/BS` group. All thirteen header keys are required in
canonical output; a hand-written file may omit one, meaning the binary's
reset default, and cmr2c2s then emits no line for it. Duplicated keys are a
hard error. The 24 legacy spellings (`T_FIRST_*`, `T_FINAL_*`, `T_PROG_*`)
are discarded by the binary's line loader before any parser pass and follow
the same regeneration/override mechanism.

## 4. `[events]`

Per-line, self-contained times; no measure blocks. **Source order is
preserved verbatim within the section** — the binary's DCM query is
source-order sensitive, so region reordering is a semantic change and the
converter never does it.

| cmr | c2s | shape |
| --- | --- | --- |
| `BPM <m:t> <bpm>` | `BPM` | |
| `METER <m:t> <count>/<unit>` | `MET` | natural fraction; c2s stores unit-then-count, converter flips |
| `SCROLL <m:t> x<factor> until <m:t>` | `SFL` | key 0 |
| `SCROLL <m:t> x<factor> until <m:t> key <n>` | `SLP` | keyed schedule; the `key` clause is always written for SLP, even `key 0` (presence of `key` = SLP) |
| `STOP <m:t> until <m:t>` | `STP` | factor-zero interval, key 0 |
| `KEYZONE <m:t> L<lane> w<width> until <m:t> key <n>` | `SLA` | lane region assigning scroll key `n` to notes inside; SLA is a group-2 record — cmr2c2s emits it back at its recorded source position among the note records, not at the region block |
| `DEPTH <m:t> x<factor> until <m:t>` | `DCM` | scales projected distance |
| `SCROLL-DEAD <m:t> x<factor> until <m:t>` | `SFE` | recognized then rejected by the binary (no handler case, no fields read); c2cmr reads its fields with the shared zero fallback and cmr2c2s re-emits that four-field shape — NOT byte-verbatim |
| `CLICK <m:t>` | `CLK` | point event |

The meter fraction — on `METER` and `meter-default:` alike — is the one
place cmr deliberately un-inverts c2s (`METER 46:000 3/4` ↔ `MET 46 0 4 3`).
Both components are arbitrary signed integers written verbatim: `4/0`,
`0/0`, and negative values are legal (the corpus carries zero components;
spec/timing.md's low-32-bit unsigned arithmetic defines their meaning) and
the strict parser accepts them. The unit-then-count reading is now
recovered, not corpus-inferred: the producer computes `beat = 384/unit` and
`bar = count*384/unit` (claim.parser.tempo-measure-schedule; 2026-08-11
revision). `DEPTH` is ONLY the separate source-order projection-factor
query — it never joins the scroll schedule ("the chart-region transform is
fully defined by STP/SFL/SLP intervals", spec/timing.md), and its cabinet
query anchors at current playback position plus the note's adjusted delta.

## 5. `[notes]`

Measure blocks in ascending order; one line per note — simultaneous notes are
consecutive lines sharing a tick, there is no chord construct. Keywords are
UPPERCASE player words. No authoring macros: cmr is strictly 1:1 with the
record set, and canonical output is single-spaced. The normalizer writes no
comments, with one exception (§5.6).

### 5.1 Simple notes

```
TAP <tick> L<lane> w<w> [+air:DIR]
EXTAP <tick> L<lane> w<w> [note <T>] [+air:DIR]   // c2s CHR
FLICK <tick> L<lane> w<w> [note <T>] [+air:DIR]   // c2s FLK
DAMAGE <tick> L<lane> w<w> [+air:DIR]             // c2s MNE
```

**`note <T>` — binary-ignored extras, preserved.** The binary reads exactly
four data fields for CHR/FLK; the corpus-universal fifth tokens (CHR's
`UP/DW/CE/RC/LC/RS/LS/BS` subtype word on 780k records, FLK's `L`) are
accepted-but-ignored extras. cmr preserves them verbatim behind `note`,
written iff present (owner decision 2026-08-20): they are NOT model state,
but keeping them makes re-emitted files diff clean and lets the `T_CHRTYPE_*`
statistics regenerate. The same clause carries AIRHOLD's ignored trailing
label (below).

### 5.2 Holds

```
HOLD <tick> L<lane> w<w> until <m:t> [+air:DIR]
EXHOLD <tick> L<lane> w<w> until <m:t> [fx <word>] [+air:DIR]   // c2s HXD
```

Holds are single records (the binary has no hold chaining). The `fx` clause is
the 8-word feedback selector, written iff the source record carries the token
(corpus: only `HXD` ever does):

| cmr word | c2s token |
| --- | --- |
| `up` | `UP` |
| `down` | `DW` |
| `center` | `CE` |
| `right-center` | `RC` |
| `left-center` | `LC` |
| `right-side` | `RS` |
| `left-side` | `LS` |
| `both-sides` | `BS` |

### 5.3 Chains: SLIDE, AIRHOLD, AIRSLIDE, CRUSH, TRACE, HEAVEN

Every multi-record construct is one block: a header line plus indented steps.
The arrows carry the recovered marked/shape split uniformly across families:

- `->` — **marked** step: the segment ending here is the judged/marked
  spelling (`SLD`/`SXD`, `ASD`, `AHX`).
- `~>` — **shape** step: the segment ending here is the shape-only spelling
  (`SLC`/`SXC`, `ASC`, `AHD`), or a families' only continuation spelling
  (`ALD`, `HHD`/`HHX` extensions, `ASO`).
- `end` on the last step marks the chain final. For SLIDE the binary forces
  the final endpoint resource regardless of spelling, so `end` on a `~>` step
  round-trips a chain whose last record is a shape spelling; for AIRHOLD and
  AIRSLIDE the final arrow is semantically load-bearing (an AHD/ASC final
  really has no authored action) — `end` marks position, the arrow carries
  the fact.

**Single records (2026-08-20 revision, owner decision):** a family whose
spelling lives in the arrow — SLIDE/EXSLIDE, AIRHOLD, AIRSLIDE — NEVER uses
an inline form: a single record is a header plus exactly one arrow step
carrying `end`. This keeps a lone `SLC` (real: 2301_03), the 141k lone AHD
vs 3.4k lone AHX chains, and lone ASC vs ASD distinguishable. CRUSH, TRACE,
and HEAVEN, whose spelling is in the keyword, keep the inline `until` form
for single records.

Steps give the segment's **end**: absolute `measure:tick`, then the endpoint
geometry. Each step emits one c2s record whose start is the previous point —
exactly; the recovered matcher's epsilon is half a fixed-grid tick, so on
the integer grid a continuation start always EQUALS the previous end and the
former `from <m:t>` override is retired as unreachable.

```
SLIDE <tick> L<lane> w<w> [style <S>] [fx <word>]
  ->  <m:t> L<lane> w<w> [style <S>] [fx <word>] [plain|ex] [from <m:t>]
  ~>  <m:t> L<lane> w<w> [...]
  ->  <m:t> L<lane> w<w> end [+air:DIR]
EXSLIDE ...                                  // chain is SXD/SXC; steps inherit
```

EX-ness lives in the word (`EXSLIDE`) and steps inherit it; a mixed chain
(c2s permits `SLD` and `SXD` segments in one chain) overrides per step with
`plain` or `ex`. `style` and `fx` carry the source token VERBATIM — any
token: the listed vocabularies (`SLD`/`HLD`/`GRN`; the eight feedback words)
are the binary's decode tables, not the grammar. The corpus authors `SXD` in
the style slot; it decodes to code 0 exactly as any unknown token does, and
chains continue on the DECODED code, so mixed-token chains are legal and
per-step overrides spell them. Clause presence round-trips token presence;
`style none` on a step spells token-absence inside a styled chain. `fx`
appears only on EX segments (the binary reads it only on `SXD`/`SXC`) and
inherits from the header to EX steps with per-step override, `fx none` for
absence. Step width follows the presence-round-trip rule: the recovered
six-field legacy form constructs the control with the record's OWN start
width (the reading the reference parser implements), so an omitted step `w`
emits the six-field record — and an omitted `w` therefore also forbids that
step's own `style`/`fx`/`note` tokens (a six-field record has no later
fields) — while an explicit `w` emits the seven-field form, even when the
values coincide.

```
AIRHOLD <tick> L<lane> w<w> on <FAMILY> [note <T>]
  ~> <m:t>                       // extends (AHD)
  -> <m:t> end                   // air action (AHX)
AIRSLIDE <tick> L<lane> w<w> h<v> on <FAMILY> [style <S>]
  ~> <m:t> L<lane> w<w> h<v> [from h<v>] [style <S>]  // shape (ASC)
  -> <m:t> L<lane> w<w> h<v> [from h<v>] end          // air action (ASD)
```

The AIRHOLD trailing token (corpus `DEF`/`PNK`) is a **binary-ignored
label**, not a style: the AHD/AHX handler consumes exactly six data fields
(spec/notes/air_hold.md, 2026-08-18). It rides the `note` clause, header and
steps alike, written iff present. AIRSLIDE's `style` is a real read field
and accepts any verbatim token with the SLIDE inheritance/override rule.
An AIRSLIDE step's start vertical is a free record field: `from h<v>` writes
a start vertical that DIFFERS from the previous step's end (the continuation
predicate matches position/lane/width, not verticals — 583 corpus records
across 19 charts jump height at a junction); omitted, the previous end
vertical is emitted.

`on <FAMILY>` is the root-attachment token in player words (exact token
mapping in §6). Recovered attachment rules (2026-08-20 revision): AIRHOLD
and AIRSLIDE initial attachment supports root parsed types **0, 1, 2, 4, 6,
and 11 only** (TAP/EXTAP/FLICK/DAMAGE/HOLD/EXHOLD/SLIDE/EXSLIDE) — direct
`on HEAVEN`/`on EXHEAVEN` is INVALID; a type-13 anchor arises only through
`on SLIDE`/`on EXSLIDE` naming an HLD-styled chain, which the parser retypes
after attachment and which then supplies its stored final vertical as the
base. Attachment is at the root's CURRENT ENDPOINT — hold end for type 1,
the chain's final path point for type 2, the start otherwise — with an
unused secondary slot, first compatible root in source order. There is no
free-standing form: a failed search appends nothing. ASD/ASC continuation
additionally requires the previous control's ASD marker to match the
referenced spelling. Air-hold steps carry no lane/width (the recovered
continuation predicate forces them equal to the root's; the former escape
is retired — a varying record would be a failed continuation, not a step).
Continuation records' root tokens are derived from block structure: `AHD`
for AirHold continuations, `ASD`/`ASC` per the previous marker; initial
records use the canonical family representative (`TAP`, `CHR`, `FLK`,
`MNE`, `HLD`, `SLD`), which is model-equal because only the resolved type
is compared.

```
CRUSH <tick> L<lane> w<w> h<v> every <ticks> [style <S>]
  ~> <m:t> L<lane> w<w> h<v> [style <S>]
  ~> <m:t> L<lane> w<w> h<v> end
```

`every <n>` is the ALD sampling interval, named for what it is. The interval
sits only on the header (the continuation predicate requires it equal to the
root's). Chain membership compares the decoded style CODE, not the token, so
token PRESENCE can vary inside one chain: the header carries the first
record's token and steps override with `style <S>` / `style none` where the
source differs. A single-record CRUSH carries its endpoint inline:
`CRUSH <tick> L0 w4 h1 every 12 until <m:t> L8 w4 h5 style RED`. The inline
`until` form belongs ONLY to CRUSH, TRACE, and HEAVEN (§5.3 single-record
rule); the arrow families always use a one-step block.

`CRUSH ... every 0 ... style NON` is the HeavenHold class selection;
`SLIDE ... style HLD` is the slide retype. Both keep their honest spelling
(§5.6).

```
TRACE <tick> L<lane> w<w> hA <a> hB <b> until <m:t> L<lane> w<w> hA <a> hB <b> [style <S>]
TRACE <tick> L<lane> w<w> hA <a> hB <b> [style <S>]          // multi-record
  ~> <m:t> L<lane> w<w> hA <a> hB <b> [style <S>]
  ~> <m:t> L<lane> w<w> hA <a> hB <b> end
```

TRACE (c2s `ASO`, corpus-absent, exact-binary-derived) carries two
integer-tenth **shell surface heights** per endpoint — the roles are now
recovered (claim.presentation.air-solid-path with
claim.note.air-solid-nonjudgement; the 2026-08-11 revision retired the
neutral `p1`/`p2` names). Exact `NON` style is the missing-resource
sentinel. Multi-record traces chain on equal style code, matching endpoint,
and both heights; per the uniform layout (owner decision 2026-08-20) the
multi-record header is the root start only, every record is a `~>` step,
the last carries `end`, and only the single-record case uses inline
`until`. Steps take `style <S>` / `style none` overrides for per-record
token presence.

```
HEAVEN <tick> L<lane> w<w> h<v> disc <n> until <m:t> L<lane> w<w> h<v>
HEAVEN <tick> L<lane> w<w> h<v> disc <n>              // multi-record
  ~> <m:t> L<lane> w<w> h<v>
  ~> <m:t> L<lane> w<w> h<v> end
EXHEAVEN <tick> L<lane> w<w> h<v> disc <n> [fx <word>] until <m:t> L<lane> w<w> h<v>
```

Recovered type-13 layout (2026-08-11 revision, spec/notes/heaven_hold.md):
`HHD`/`HHX` carry their own integer-tenth verticals at BOTH endpoints, an
independent lateral endpoint (lane and width), and an integer presentation
discriminator whose parity joins the chain key — `disc <n>`, always written
(the parser stores it unconditionally). The EXHEAVEN extra token is no
mystery: HHX decodes it through the same exact eight-entry feedback table as
extended slides, so it is the same `fx <word>` clause, written iff present
(the former `xtok` clause is retired). Chains extend only when the extended
flag, feedback code, discriminator, parity, and the previous endpoint's
point AND vertical all match; `~>` steps carry the full endpoint geometry.
Uniform layout (owner decision 2026-08-20): the multi-record header is the
root start only, every record is a step, the last carries `end`; inline
`until` is the single-record form. Because the chain key fixes them, `disc`
and `fx` sit on the header alone and the emitter copies them onto every
emitted record.

### 5.4 Air suffixes

`+air:U +air:UL +air:UR +air:D +air:DL +air:DR` map to
`AIR AUL AUR ADW ADL ADR`. The suffix sits on the owner line (or on a chain's
`end` step) and emits the AIR record **at the owner's ATTACHMENT POINT**
(2026-08-20 revision — the binary attaches at the root's current endpoint):

- `TAP`/`EXTAP`/`FLICK`/`DAMAGE`: the note's own time, lane, width;
- `HOLD`/`EXHOLD`: the **`until` point** — hold end time, the hold's lane
  and width (all 87,470 corpus HLD-rooted AIRs sit there; emitting at the
  start would orphan every one);
- `SLIDE`/`EXSLIDE` and every chain family: the chain's FINAL point — end
  time, end lane, end width — which is why the suffix rides the `end` step
  (or the inline `until` line for CRUSH/TRACE/HEAVEN).

The AIR (type 3) root set is wider than the air-path one: an AIR may attach
to ANY root family, including `CRUSH`, `TRACE`, and `HEAVEN` (types 9, 10,
13) — the suffix is legal on those constructs' final lines too. The root
token emitted is the canonical family representative. An AIR record carrying
a style token writes `+air:U style <S>` (verbatim, any token).

Records that would fail attachment (orphans — no compatible root) have no
suffix form; they are written as an explicit diagnostic line, and the
converter warns:

```
AIR:U <m:t> L<lane> w<w> on TAP orphan
```

### 5.5 Style tokens

`style` clauses carry the source token **verbatim — any token**; `fx`
clauses use the eight friendly words (§5.2 table, an owner decision) for the
known tokens and raw passthrough for anything outside the table.
The 16-entry table (`DEF RED ... NON`), the slide styles (`SLD HLD GRN`),
and the eight feedback words are the binary's DECODE tables, not the cmr
grammar: an out-of-table token (the corpus has `SXD` in a slide style slot)
is written and re-emitted verbatim and decodes to code 0 exactly as the
binary does. Clauses are written **iff the token is present in the source
record**; an omitted clause is a record with no token (the legacy short
forms); `style none` / `fx none` on a chain step spells token-absence where
the header carries a token. Clause presence round-trips token presence in
both directions — absence is never normalized to `DEF`.

### 5.6 The one generated comment

The normalizer is otherwise silent, but a note whose runtime family differs
from its keyword gets the retype comment — the single most surprising fact in
the format:

```
CRUSH 0 L4 w4 h5 every 0 until 3:192 L4 w4 h5 style NON  // -> heaven hold
SLIDE 192 L8 w4 style HLD                                 // -> heaven hold
```

## 6. c2s mapping table

| cmr | c2s spelling(s) |
| --- | --- |
| `TAP` | `TAP` |
| `EXTAP` | `CHR` |
| `FLICK` | `FLK` |
| `DAMAGE` | `MNE` |
| `HOLD` / `EXHOLD` | `HLD` / `HXD` |
| `SLIDE` marked/shape | `SLD` / `SLC` |
| `EXSLIDE` marked/shape | `SXD` / `SXC` |
| `+air:U/UL/UR/D/DL/DR` | `AIR AUL AUR ADW ADL ADR` |
| `AIRHOLD` `~>` / `->` | `AHD` / `AHX` |
| `AIRSLIDE` `~>` / `->` | `ASC` / `ASD` |
| `CRUSH` | `ALD` |
| `TRACE` | `ASO` |
| `HEAVEN` / `EXHEAVEN` | `HHD` / `HHX` |
| `KEYZONE` | `SLA` |
| `SCROLL` / `SCROLL key` / `STOP` / `DEPTH` / `SCROLL-DEAD` / `CLICK` | `SFL` / `SLP` / `STP` / `DCM` / `SFE` / `CLK` |
| `BPM` / `METER` | `BPM` / `MET` |

Root-attachment tokens (`on <FAMILY>`) use the same left column, restricted
per secondary family (§5.3/§5.4): AIRHOLD/AIRSLIDE accept only the six
point/sustain families (types 0/1/2/4/6/11); suffix AIRs accept every root
construct. The emitter writes the canonical family representative as the
record's root-family token (`TAP`/`CHR`/`FLK`/`MNE`/`HLD`/`SLD`/`ALD`/
`ASO`/`HHD`). `SFE`, `ASO`, `HHD`, `HHX` are corpus-absent and covered by
synthetic tests only.

## 7. Round-trip contract

The bar is **parsed-model equality**: `cmr2c2s(c2cmr(X))` must produce c2s
whose parsed model under the recovered parser semantics is identical to
`X`'s. The compared model is enumerated (2026-08-20): per record the
resolved parsed type, canonical position, lane, encoded width, duration/
endpoint fields, decoded style/feedback CODES, chain association, and
secondary type/direction; regions per key with factor/start/end; the
source-order DCM list; and the `[meta]` header values (the reference model
gains a header extension for this — the viewer parser alone does not store
DIFFICULT/CREATOR). Root-token SPELLING on secondaries and binary-ignored
extras compare by resolved type only. Byte-identity of the c2s text is not
promised (field spacing may differ); model identity is.

Enforcement:

1. **Emit-then-reparse gate**: after emitting c2s, the emitter re-runs the
   emitted stream through the FULL reference association pass
   (normalizeEventAssociations + buildAirLadderChains) — continuation
   predicates AND first-compatible-root initial attachment with slot
   consumption — and verifies the records reassociate into exactly the
   authored constructs. Emission that would be re-parsed differently is a
   hard error.
2. **Chain grouping on import** (`c2cmr`) implements the binary's sequential
   first-match continuation exactly — including the stacked/merge junction
   multiplicity rules — so blocks reflect what the game would build.
3. **Corpus proof**: `cmrcheck` runs the full round trip over every local
   chart and compares parsed models. The reference model is the viewer's own
   `parseC2s` + association pass (owner decision 2026-08-11) — corpus-proven
   through the browser audit and implementing the recovered rules — with
   sampled deep-checks against the C++ reconstruction as the stricter
   authority. All 7,752 charts (1,837 reference `_03`) must pass before the
   converter is considered correct.
4. **Record order is SEMANTIC** (2026-08-20, supersedes the old family/lane
   sort): first-unused-slot attachment and DCM's source-order scan make the
   emitted order load-bearing. c2cmr writes `[notes]` lines in c2s source
   order within a tick; cmr2c2s emits records in cmr line order, each chain's
   records contiguous from its root, and every secondary (suffix AIRs,
   AHD/ASD/ASC initial records) immediately after its owner's records. SLA
   lines re-emit at their recorded source position. A non-ascending source
   chart that this ordering cannot reproduce is a c2cmr hard error with the
   offending records named — no silent reorder. `[events]` stays verbatim
   source order.

## 8. Tooling

- **`c2cmr`** — c2s → cmr (stdin/file → stdout/file); warns on stat
  mismatches (recorded as overrides), out-of-range widths, and orphans.
  Orphan AIRs get the diagnostic line form; orphan AHD/AHX/ASD/ASC records
  and unknown commands are dropped with a warning (model-equal — the binary
  appends nothing for them either) and counted by cmrcheck.
- **`cmr2c2s`** — cmr → c2s; runs the emit-then-reparse gate always.
- **`cmrcheck`** — round-trip verifier over files or directories.
- `split` / `join` between `.cmr` and `.cmt`/`.cev`/`.cnt` are flags on the
  converters.

The viewer's cmr reform (normalized-only input) follows once the converters
prove out on the corpus.

## 9. Open items (ledgered in issue #12 where inference-based)

- ~~`p1`/`p2` trace property roles; `xtok` HHX token role~~ — RESOLVED
  2026-08-11: shell surface heights (`hA`/`hB`) and the shared eight-entry
  feedback table (`fx`).
- ~~`METER` unit-then-count reading~~ — RESOLVED 2026-08-11: recovered
  producer math.
- The stats derivations must reproduce the binary's derived-summary pass;
  mismatches surface as overrides and are individually investigable.
- Air-hold step `L`/`w` variance and `from` slack: representable, expected
  never to occur in real charts; `cmrcheck` counts occurrences.
