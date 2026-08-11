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
- **Heights** (air verticals): `h<value>`, the authored number verbatim;
  tenths precision (the binary stores integer tenths). `h1` is the lane
  surface, `h5` the ordinary air height.
- **Durations** never appear as raw tick counts in cmr: spans end with
  `until <measure:tick>` (always the full form, even same-measure). The
  converter derives the c2s duration.
- Numbers are written minimally (`180` not `180.000`, `h7.5` not `h7.50`);
  value equality is what round-trips, not digit strings.

## 3. `[meta]`

Fully modeled, friendly lowercase keys. The header commands map:

| cmr key | c2s command | notes |
| --- | --- | --- |
| `version:` | `VERSION` | both tokens, space-separated |
| `music:` | `MUSIC` | |
| `sequence:` | `SEQUENCEID` | |
| `difficulty:` | `DIFFICULT` | the raw number (viewer may display a name) |
| `level:` | `LEVEL` | |
| `creator:` | `CREATOR` | verbatim to end of line |
| `bpm-default:` | `BPM_DEF` | four values |
| `meter-default:` | `MET_DEF` | natural fraction, e.g. `4/4` |
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
equality never depends on them. The 24 legacy spellings (`T_FIRST_*`,
`T_FINAL_*`, `T_PROG_*`) are discarded by the binary's line loader before any
parser pass; cmr regenerates them by the same derivations used for display
tooling, with the same override mechanism.

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
| `SCROLL <m:t> x<factor> until <m:t> key <n>` | `SLP` | keyed schedule |
| `STOP <m:t> until <m:t>` | `STP` | factor-zero interval, key 0 |
| `KEYZONE <m:t> L<lane> w<width> until <m:t> key <n>` | `SLA` | lane region assigning scroll key `n` to notes inside |
| `DEPTH <m:t> x<factor> until <m:t>` | `DCM` | scales projected distance |
| `SCROLL-DEAD <m:t> x<factor> until <m:t>` | `SFE` | parsed then rejected by the binary (no handler case); preserved verbatim |
| `CLICK <m:t>` | `CLK` | point event |

The meter fraction is the one place cmr deliberately un-inverts c2s
(`METER 46:000 3/4` ↔ `MET 46 0 4 3`). The unit-then-count reading is now
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
EXTAP <tick> L<lane> w<w> [+air:DIR]        // c2s CHR
FLICK <tick> L<lane> w<w> [+air:DIR]        // c2s FLK
DAMAGE <tick> L<lane> w<w> [+air:DIR]       // c2s MNE
```

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
- `end` on the last step marks the chain final (the binary forces the final
  endpoint regardless of spelling; `end` on a `~>` step round-trips a chain
  whose last authored record is a shape spelling).

Steps give the segment's **end**: absolute `measure:tick`, then the endpoint
geometry. Each step emits one c2s record whose start is the previous point.
The rare source chart that exploited the c2s matcher's epsilon slack (a
segment starting *near* but not *at* the previous end) writes the override
`from <m:t>` on that step.

```
SLIDE <tick> L<lane> w<w> [style <S>] [fx <word>]
  ->  <m:t> L<lane> w<w> [style <S>] [fx <word>] [plain|ex] [from <m:t>]
  ~>  <m:t> L<lane> w<w> [...]
  ->  <m:t> L<lane> w<w> end [+air:DIR]
EXSLIDE ...                                  // chain is SXD/SXC; steps inherit
```

EX-ness lives in the word (`EXSLIDE`) and steps inherit it; a mixed chain
(c2s permits `SLD` and `SXD` segments in one chain) overrides per step with
`plain` or `ex`. `style` is the slide style token (`SLD`/`HLD`/`GRN`), written
iff present, header value inherited by steps, per-step override allowed. `fx`
appears only on EX segments (the binary reads it only on `SXD`/`SXC`).
Step width follows the presence-round-trip rule (2026-08-11 revision): the
recovered six-field legacy form constructs the control with the record's own
start width, so an omitted step `w` round-trips that form — the emitter
writes the six-field record — while an explicit `w` emits the seven-field
form, even when the values coincide.

```
AIRHOLD <tick> L<lane> w<w> on <FAMILY> [style <S>]
  ~> <m:t>                       // extends (AHD)
  -> <m:t> end                   // air action (AHX)
AIRSLIDE <tick> L<lane> w<w> h<v> on <FAMILY> [style <S>]
  ~> <m:t> L<lane> w<w> h<v>     // shape (ASC)
  -> <m:t> L<lane> w<w> h<v> end // air action (ASD)
```

`on <FAMILY>` is the root-attachment token in player words (`TAP`, `EXTAP`,
`FLICK`, `DAMAGE`, `HOLD`, `EXHOLD`, `SLIDE`, `EXSLIDE`, `HEAVEN`,
`EXHEAVEN`, `AIRHOLD`, `AIRSLIDE` — exact token mapping in §6). Recovered
attachment rules (2026-08-11 revision, spec/matching.md and the air-family
claims): initial attachment supports root parsed types 0, 1, 2, 4, 6, 11,
and 13, at the root's current endpoint, requiring an unused secondary slot;
there is **no free-standing form** — the former `NONE` token is removed,
because a failed root search takes the parser diagnostic path and appends
nothing. A type-13 root supplies its chain's stored final vertical as the
attachment base. ASD/ASC continuation additionally requires the previous
control's ASD marker to match the referenced spelling. Air-hold steps omit
lane/width (the family is static); a step may carry `L`/`w` explicitly as
the lossless escape if a source record varies them. Continuation records'
own root tokens are derived from block structure, per the recovered
continuation rules.

```
CRUSH <tick> L<lane> w<w> h<v> every <ticks> [style <S>]
  ~> <m:t> L<lane> w<w> h<v> [from <m:t>]
  ~> <m:t> L<lane> w<w> h<v> end
```

`every <n>` is the ALD sampling interval, named for what it is. Interval and
style sit only on the header: the binary's continuation predicate requires
them equal to the root's, so they are chain-level by recovered fact, not by
convention. A single-record construct carries its endpoint inline instead of
steps: `CRUSH <tick> L0 w4 h1 every 12 until <m:t> L8 w4 h5 style RED`. The
same inline-`until` rule applies to every chain family's single-record case.

`CRUSH ... every 0 ... style NON` is the HeavenHold class selection;
`SLIDE ... style HLD` is the slide retype. Both keep their honest spelling
(§5.6).

```
TRACE <tick> L<lane> w<w> hA <a> hB <b> until <m:t> L<lane> w<w> hA <a> hB <b> [style <S>]
```

TRACE (c2s `ASO`, corpus-absent, exact-binary-derived) carries two
integer-tenth **shell surface heights** per endpoint — the roles are now
recovered (claim.note.air-solid-presentation; 2026-08-11 revision retires
the former neutral `p1`/`p2` names, exactly the spec bump the original
decision anticipated). Exact `NON` style is the missing-resource sentinel.
Multi-record traces chain on equal style code, matching endpoint, and both
heights, with `~>` steps carrying the same shape.

```
HEAVEN <tick> L<lane> w<w> h<v> disc <n> until <m:t> L<lane> w<w> h<v>
EXHEAVEN <tick> L<lane> w<w> h<v> disc <n> [fx <word>] until <m:t> L<lane> w<w> h<v>
  ~> <m:t> L<lane> w<w> h<v>                          // extension records
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

### 5.4 Air suffixes

`+air:U +air:UL +air:UR +air:D +air:DL +air:DR` map to
`AIR AUL AUR ADW ADL ADR`. The suffix sits on the owner line (or on a chain's
`end` step) and emits the AIR record at the owner's time, lane, and width with
the owner's family as root token — which is total for well-formed charts,
because the binary's attachment predicate *requires* those fields to match.
An AIR record carrying a style token writes `+air:U style <S>`.

Records that would fail attachment (orphans — no compatible root) have no
suffix form; they are written as an explicit diagnostic line, and the
converter warns:

```
AIR:U <m:t> L<lane> w<w> on TAP orphan
```

### 5.5 Style tokens

The 16-entry style vocabulary (`DEF RED ORN YEL LIM GRN AQA CYN DGR BLU PPL
VLT PNK GRY BLK NON`) and the slide styles (`SLD HLD GRN`) are written
verbatim behind `style`, **iff the token is present in the source record**.
An omitted clause is a record with no token (the legacy short forms); an
explicit `style DEF` is a record with the literal token. Clause presence
round-trips token presence in both directions — absence is never normalized
to `DEF`.

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

Root-attachment tokens (`on <FAMILY>`) use the same left column; the emitter
writes the corresponding c2s spelling as the record's root-family token.
`SFE`, `ASO`, `HHD`, `HHX` are corpus-absent and covered by synthetic tests
only.

## 7. Round-trip contract

The bar is **parsed-model equality**: `cmr2c2s(c2cmr(X))` must produce c2s
whose parsed model under the recovered parser semantics is identical to
`X`'s — every record, field value, chain association, style code, and region
in source order. Byte-identity of the c2s text is not promised (field
spacing and record order may differ); model identity is.

Enforcement:

1. **Emit-then-reparse gate** (panel consensus, all three judges): after
   emitting c2s, the emitter re-runs the emitted record stream through the
   real recovered continuation predicates (slide, AHD/AHX, ASC/ASD, ALD, ASO,
   HHD/HHX) and verifies the records reassociate into exactly the authored
   blocks. Emission that would be re-parsed differently is a hard error. This
   catches the one layout c2s cannot spell: a chain record starting on
   another chain's identical resting frontier, which the game parser would
   silently merge.
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
4. Emitted record order: chains contiguous (root first), constructs ordered
   by (start position, family, lane, source order); `[events]` verbatim
   source order.

## 8. Tooling

- **`c2cmr`** — c2s → cmr (stdin/file → stdout/file); warns on stat
  mismatches (recorded as overrides), out-of-range widths, orphan AIRs.
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
