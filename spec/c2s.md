# C2S ingestion

## Chart-path derivation

Gameplay setup first initializes three selected-content strings to empty and
looks up the current content record in a process-owned catalog populated from
an external resource. A missing record uses an empty default. The source chart
path remains empty unless the selected record contains at least six fixed-size
selector entries.

When those entries are available, the setup descriptor's one-byte selector is
used if it is below the external selector-table count. Otherwise it is clamped
to that count minus one. The path string from the resulting entry becomes the
source path. The executable does not check a zero external count or verify the
resulting index against the content record; these malformed external-resource
domains remain explicit invalid configuration in the clean-room interface.
Exact selector-table values and player-facing identities are not guessed.

The gameplay load path then searches the selected source string from offset
zero for the first exact, case-sensitive substring `.xml`. If found, the four
matched bytes are replaced with `.c2s`. If absent, the source string is passed
onward unchanged. The rule is substring-based; it does not require `.xml` to
be the final suffix. An unavailable source therefore remains empty and is still
passed into the destructive chart-load entry.

Evidence: `claim.pipeline.source-chart-path-selection` and
`claim.pipeline.c2s-path-derivation`. Reconstruction:
`chart::reconstruction::select_source_chart_path` and
`chart::reconstruction::derive_c2s_path`. Focused test:
`tests/chart_path_test.cpp`.

## Gameplay load and failure lifecycle

The gameplay-facing loader is destructive at the parsed-chart boundary. It
clears every existing parsed record and related chart container before the
file precheck and tokenizer are invoked. A failed precheck or failed
tokenization therefore leaves an empty parsed chart; it does not retain the
previous chart.

Successful tokenization enters the parser orchestration, which performs a
second chart reset and then the ordered parse/postprocessing passes. The
orchestration returns success after those passes. A rejected individual record
is skipped and does not convert the whole tokenized load into failure. Thus a
tokenized chart with zero accepted records is still a successful load.

The gameplay loader applies mode/profile/window configuration only on the
successful-tokenization branch. Its scene-setup caller receives no success
value and continues either way. State 0 derives the final parsed-record count
and fills an ordered pending queue with indices zero through count-minus-one;
it does not construct runtime notes. Precheck/tokenizer failure therefore
continues setup with an empty queue and zero initial runtime notes. Successful
loads queue every accepted parsed record, including region-only records that
will later have no primary factory case.

Runtime materialization is specified under `Runtime note materialization` in
`spec/timing.md`. It occurs after the current outer update's manager substeps,
so a newly created object first participates in the following outer update.

This load entry clears parsed chart state only; it is not the runtime-note
teardown routine. The manager constructor performs the full reset before the
state-machine's initial state-0 setup. That state advances forward and has no
recovered in-scene transition back to state 0. Leaving structural active state
does not immediately destroy runtime notes: states `0x10` through `0x12` still
run the complete gameplay update. State-`0x13` entry then deletes and clears
both active runtime pointer vectors. Full manager reset also
clears the third manager-owned pointer vector, parsed chart state, forced-mode
bytes, input/source mappings, timing/result fields, and configuration holders.
The manager destructor deletes all three pointer vectors and destroys the
chart owner. No retry that reuses live runtime notes was recovered.

Evidence: `claim.pipeline.runtime-note-materialization-order` and
`claim.pipeline.post-active-gameplay-drain`. Reconstruction:
`chart::reconstruction::reconstruct_gameplay_chart_load_attempt`. Focused test:
`tests/chart_load_lifecycle_test.cpp`.

## Derived parser-summary boundary

The successful-load postprocessor retains a derived block of per-family
counts, aggregate counts, first/last positions, and a bounded position
distribution in the parser object. It also fills a separate 60-byte count
summary supplied by the loader. Neither summary is authoritative gameplay
state: the loader never reads its stack-local output, and gameplay setup
recomputes its record count and fourteen setup categories directly from the
finalized parsed-record vector.

An earlier postprocessor also computes a note-only horizon and an all-event
horizon. The note-only scan selects family-specific roots or final endpoints,
including supported attached AirHold/AirSlide endpoints, while omitting SLA.
It replaces the current position only when the candidate is later by more than
`1/192` in scalar chart position. The all-event horizon starts from that value
and can be extended by BPM, MET, CLK, tree-owned, and other auxiliary position
records. The note-only value is otherwise unused. The all-event value only
closes derived BPM-duration statistics and generated meter/grid coverage; it
does not enter the authoritative BPM lookup, runtime-note queue or factory,
candidate timing, input, judgement, or terminal state.

A separate parser-library routine can recompute the retained summary and check
selected family counts against a fixed six-profile availability table, writing
20-byte validation records. That routine has no recovered caller in this
binary and is not a gameplay compatibility or VERSION gate. The clean-room
gameplay core therefore does not reproduce these summary/validation records;
it derives required counts and gameplay timing from reconstructed parsed
records and the BPM map at their actual consumer boundaries.

Evidence: `claim.pipeline.parser-derived-summary-boundary`.

## Mapped parser structure (not yet normative)

The reader has a descriptor-driven, multi-pass organization described by
`claim.parser.command-descriptor-schema` and
`claim.parser.multi-pass-dispatch`. Whole-load failure propagation and
record-handler rejection are specified above. Group-0 field handling is now
normative below. Shared group-2 missing/malformed-field behavior is also
normative below. Family-specific association and domain checks remain owned by
their event claims; no complete event grammar should be inferred from the
structural map alone.

## Shared event token behavior

Only descriptor-recognized command names enter parser dispatch; unknown names
are discarded during line loading. The two gameplay callers of the group-2
handler disable its descriptor field-count/presence validator. A recognized
event is therefore not rejected solely for missing or extra fields.

The descriptor registry is exactly 91 case- and length-sensitive spellings
with IDs `0x00` through `0x5a`. The backward-compatible corpus also contains
twenty `T_PROG_00`, `T_PROG_05`, ..., `T_PROG_95` records in every chart.
Those spellings exist in a separate initialized name table but are absent from
the descriptor registry. They fail lookup and are discarded before every
parser pass; their values do not populate group-3 derived storage and cannot
select gameplay behavior. Evidence:
`claim.parser.legacy-t-prog-command-exclusion`.

Each family reads the indices it needs. Missing, empty, out-of-range-index, or
descriptor-incompatible numeric fields become zero. Accessed nonempty numeric
fields use prefix-accepting float or base-10 integer conversion: trailing text
after a valid prefix is tolerated, while no-conversion and range errors escape
through the gameplay load chain as exceptions. Extra fields are ignored unless
a family branch explicitly reads their index. Later family-specific matching
can still skip a record without failing the tokenized load. Evidence:
`claim.parser.event-token-fallback`.

## Header commands

Every parse resets header state, tokenizes recognized commands, and applies
group-0 records in source order. Repeated implemented headers therefore use the
last record. A recognized `RESOLUTION` record is deliberately rejected by the
handler, does not change the reset value 384, and does not advance accepted-
record accounting.

The reset state and implemented fields are:

| Command | Reset | Present record |
|---|---|---|
| `VERSION` | two empty strings and zero triples | store two raw strings and scan each as `%d.%d.%d` |
| `MUSIC`, `SEQUENCEID` | 0 | replace from one integer field |
| `DIFFICULT`, `CREATOR` | empty | replace from one string field |
| `LEVEL` | 0.0 | replace from one float field |
| `BPM_DEF` | four 150.0 values | replace four float fields |
| `MET_DEF` | 4, 4 | replace two integer fields |
| `RESOLUTION` | 384 | ignore/reject the record |
| `CLK_DEF` | 0 | replace from one integer field |
| `PROGJUDGE_BPM` | 240.0 | replace from one float field |
| `PROGJUDGE_AER` | float bits `0x3f7fbe77` | replace from one float field |
| `TUTORIAL` | false | replace with `integer field > 0` |

Missing strings become empty. Missing, empty, out-of-range-index, or type-
incompatible numeric fields become zero. A nonempty compatible numeric field
uses prefix-accepting float or base-10 integer conversion: trailing characters
after a valid prefix are tolerated, while no-conversion and range errors throw
through the recovered load chain. Extra fields do not cause header rejection
and are ignored by the handler.

VERSION always retains each raw field. Each component triple is all zero unless
exactly three decimal conversions succeed; trailing content after a successful
third conversion is tolerated. The triples are storage-only on the recovered
gameplay path: no parser, postprocessor, runtime factory, or judgement branch
reads them. Backward-compatible resource strings in the local corpus therefore
do not select executable behavior here.

Header-only mode reads at most the first 20 source lines and skips BPM, event,
and postprocessing passes. Its recovered metadata caller consumes CREATOR and
authored BPM_DEF field 2. Full gameplay parsing reads the entire source. When
BPM records exist, a later statistics pass replaces the binary's four internal
BPM_DEF storage fields; BPM_DEF never creates a tempo record.

Gameplay relevance is intentionally narrow. Fixed RESOLUTION drives all chart
position conversion; MET_DEF can synthesize a meter/grid origin but does not
alter note schedules; CLK_DEF feeds scheduled click feedback whose return is
ignored after note updates. PROGJUDGE_BPM selects adaptive Air cadence,
PROGJUDGE_AER supplies the final AHD/ASC end margin, and TUTORIAL enables the
key-zero selector-zero open-interval exclusion used by AirHold, AirSlide, and
HeavenHold. MUSIC, SEQUENCEID, DIFFICULT, LEVEL, CREATOR, VERSION, and derived
BPM_DEF statistics have no recovered judgement consumer. Evidence:
`claim.parser.header-default-dispatch`.

## Timing commands

The gameplay parse has a normative timing pre-pass. `BPM` (ID 13) parses
integer major/minor plus a float BPM into the authoritative schedule vector;
all BPM records are finalized before ordinary timing and note commands.
`MET` (ID 14) parses integer major/minor plus two integer meter fields only in
the subsequent ordinary pass. `PROGJUDGE_BPM` (header ID 10) supplies the
adaptive Air cadence reference. `RESOLUTION` (header ID 8) is recognized by
the descriptor/tokenizer but has no header-handler case in this binary, so the
reset value 384 remains in force. Exact normalization, scheduling, defaults,
and malformed-domain limits are specified in `spec/timing.md`. Evidence:
`claim.timing.tempo-measure-schedule`.

## Event-family type mapping

The group-2 event parser resolves named note families to parsed-record types as
specified by `claim.parser.event-family-type-map`. `TAP` resolves to type 0,
which is independently connected to the runtime TAP object. Shared types do not
yet imply identical behavior: family-specific fields and later subtypes remain
open. `FLK` resolves to type 6, independently connected to the runtime
`FlickNote` and its motion judgement by
`claim.note.flick-motion-judgement`. `CHR` resolves to type 4 and constructs the
TAP-derived `CharaTapNote`, whose result-category distinction is specified by
`claim.note.chara-tap-result-category`. This mapping is structural and is not
yet a complete event grammar.

`MNE` resolves to parsed type 11 and is independently connected through the
runtime factory and RTTI to `projView::MineNote`. Its gameplay behavior is
specified in `spec/notes/mine.md` and
`claim.note.mine-contact-aggregate-judgement`; this does not yet establish every parsed
field or malformed-record rule for the event family.

`AIR`, `AUR`, `AUL`, `ADW`, `ADR`, and `ADL` resolve to secondary type 3.
Unlike root event families, their handler searches for a compatible existing
record and attaches the secondary type, property, and mirrored direction code;
it does not append a standalone record. The exact match requirements and
runtime `AirNote` construction are normative in `spec/notes/air.md`. Evidence:
`claim.note.air-secondary-judgement`.

`SLD`, `SXD`, `SLC`, and `SXC` resolve to type 2. Their handler appends a first
control point to a new parsed record or continues an existing compatible
record by matching its last point and connection field. The normative
continuation predicates, command-field pairs, manager-owned path conversion,
and runtime behavior are in `spec/notes/slide.md`. Evidence:
`claim.note.slide-path-sustain-judgement`.

Before type-specific generated-path construction, exact field-8 style `HLD`
(style code 1) changes any completed Slide chain from type 2 to type 13 and
normalizes its root and control-point discriminator slots to code 10. The
SLD/SLC versus SXD/SXC command-form flag is preserved, so rewritten records
construct `HeavenHoldNote` with start-profile pair 0/1 or 2/3 respectively.
Evidence: `claim.note.slide-hld-heaven-retyping`.

`AHD` and `AHX` resolve to secondary type 5. Their sixth token selects the
compatible root family for first attachment or type 5 for continuation. Only
AHX-tagged saved endpoints become authored timing checkpoints; AHD extends the
path/end without adding one. The exact match, continuation, construction, and
consumer rules are normative in `spec/notes/air_hold.md`. Evidence:
`claim.note.air-hold-secondary-judgement`.

`ASD` and `ASC` resolve to secondary type 8 and share an eleven-field schema.
Their sixth field selects a compatible root for first attachment or names the
previous ASD/ASC family for continuation. Command ID `0x27` makes the appended
control point ASD-marked; `0x28` makes it ASC. The exact matching, marker,
construction, and consumer rules are normative in
`spec/notes/air_slide.md`. Evidence:
`claim.note.air-slide-secondary-judgement`.

`ALD` resolves to parsed type 9 and builds compatible chains of 0x20-byte
control points. The runtime normally constructs AirLadderNote, but selector
zero plus exact style `NON`/code 15 selects HeavenHoldNote. Ordinary chain,
class-selection, and consumer rules are normative in
`spec/notes/air_ladder.md`. A missing legacy style token becomes empty string /
code 0 and does not select the exception. `HHD` and `HHX` have command IDs
`0x2c` and `0x2d`, resolve to parsed type 13, and always construct the same
HeavenHoldNote class. Exact generation and consumer rules are normative in
`spec/notes/heaven_hold.md`. Evidence: `claim.note.air-ladder-judgement` and
`claim.note.heaven-hold-judgement`.

`ASO` resolves to parsed type 10 and chains two-endpoint AirSolid paths. Its
twelve fields, integer-tenth property conversion, per-endpoint mirroring,
color/style table, continuation predicate, and nonjudgement runtime boundary
are normative in `spec/notes/air_solid.md`. The local corpus has no ASO
records; the structural reconstruction is exact-binary-derived and uses
synthetic focused tests. Evidence: `claim.note.air-solid-nonjudgement`.

## SLA region directives

`SLA` resolves to parsed type 12 but is not a runtime note. Its six numeric
fields after the command are major position, minor position, lane, width,
duration in minor-position units, and an anonymous integer tag.

Width is clamped to 1 through 16. Mirrored parsing replaces the lane with
`16 - lane - width`. Start position is normalized from `(major, minor)` and
end position from `(major, minor + duration)` at the fixed resolution 384.

For an integer-lane query, add `1/192` to its chart-position scalar. An SLA
matches exactly when:

```text
region_start <= shifted_query < region_end
region_lane <= query_lane
query_lane + query_width <= region_lane + region_width
```

The scan starts from tag zero and retains only strictly greater matching tags,
so overlap selects the greatest positive tag. AirLadder's interpolated float-
lane query uses the same time and maximum rules with `-0.00001F` left and
`+0.00001F` right containment tolerances.

The parser assigns selected tags to note roots/endpoints and path records, but
the runtime factory has no type-12 case. Downstream tag readers select a
tag-keyed scroll-position transform for projected presentation state. The
authoritative scheduled position, manager clock, candidate/input gates,
classification, results, and terminal state do not read the tag. SLA therefore
has no recovered gameplay-generation or judgement effect. Evidence:
`claim.parser.sla-region-selection`. Reconstruction: `C2sSlaRegion`,
`parse_c2s_sla_record`, and `select_c2s_sla_tag`. Focused test:
`tests/sla_region_test.cpp`.
