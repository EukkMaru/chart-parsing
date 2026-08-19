# AirLadder note

## Parser chain and runtime selection

`ALD` has command ID `0x29` and resolves to parsed type 9. Its fifth data field
(after major position, minor position, lane, and width) is a signed integer
sampling interval in the parser's fixed 384-tick major-unit grid. The parser
stores authored endpoints as `0x24`-byte controls and owns a separate generated
`0x20`-byte vector.

A command continues an existing nonempty type-9 chain only when its sampling
interval and style equal the root's saved values and its position, lane,
encoded width, and integer-tenth vertical value match the previous endpoint.
Otherwise it starts a new chain. After each append, the parser regenerates the
entire sampled vector, recomputes every generated schedule through the tempo
map, and advances the root's saved current endpoint.

Runtime construction has an exact class-selection exception:

```text
sampling_interval == 0 && style_code == 15
    ? HeavenHoldNote
    : AirLadderNote
```

Style code 15 is exact executable string `NON`. The ordinary AirLadder path
allocates `0x11c` bytes and owns its runtime checker vector and resolved count.
The HeavenHold branch is a separate gameplay class and is not silently modeled
as AirLadder. A missing style token produces the accessor's empty string and
style code 0; legacy one-short ALD records therefore remain ordinary
AirLadder. The exceptional path is normative in
`spec/notes/heaven_hold.md`. Evidence:
`claim.note.air-ladder-generated-checkpoints` and
`claim.note.heaven-hold-judgement`.

## Generated checkpoint construction

Generation first clears the old vector. It produces no records if there are no
authored controls, the sampling interval is nonpositive, or the root grid tick
is not less than the final control's grid tick.

For an advancing chain with a positive interval:

1. Start the generated cursor at the root tick, so the root itself is the first
   checkpoint.
2. At each cursor below the final tick, select the nonzero authored-control
   span containing that cursor. Duplicate-position controls are skipped.
3. Linearly interpolate lane, vertical value (stored as integer tenths before
   conversion), and fixed-table-decoded width across that span.
4. Canonicalize the cursor back to chart position, append an enabled `0x20`
   generated record, and advance by the exact sampling interval.
5. Append the final authored endpoint only when the cursor lands on its tick
   exactly. An overshoot does not add the endpoint.

On ordinary monotone input, checkpoint count is therefore:

```text
floor((final_tick - root_tick) / sampling_interval) + 1
```

The parser then replaces each generated record's schedule slot using the tempo
schedule. A positive interval whose wrapped signed cursor falls below its prior
value enters a pathological source expansion domain; the clean-room helper
reports that disposition instead of attempting the expansion.

AirLadder load creates one `0x88`-byte runtime record for each generated
`0x20` record. Every runtime record owns the shared retained-profile timing
checker initialized with derived input profile 7. Profile 7 is the inclusive
externally configured scalar range synthesized from the six photo-sensor
inputs and retained in the 301-snapshot history. Its numeric range and timing
windows remain external parameters; the physical source framing, synthesis,
and fixed profile index are normative.

AirLadder exposes no lane candidate. It neither constrains nor reads the active
manager's lane-candidate reduction.

## Gameplay update and completion

One gameplay update visits every generated runtime record in order. A record
whose retained checker state is below 2 is updated. If the resulting state is
above 1, AirLadder stores its result, submits source category 18, increments
the resolved count, and continues iterating. Multiple checkpoints can
therefore resolve in the same substep; this is not a front-only queue.

Category 18 maps to shared category 7 and is aggregate-authoritative under
normal valid runtime bounds. Existing terminal-route state sends the event to
the shared observer-only path.

The parser assigns each accepted type-9 event a chart-derived identity at
parsed `+0x84`. `NotesPreCalcManager` uses that identity to retrieve the
authored type-9 precompute; it is not an external configuration key. The
precompute stores the final authored endpoint schedule, which runtime copies
to `+0x118`. For ordinary finite values, deferred terminal transition is
requested only when:

```text
current >= final_authored_endpoint_schedule
&& resolved_count == generated_checkpoint_count
```

Equality at the authored end passes. The executable's exact guard is
`current <= end && end != current`; an unordered float therefore bypasses the
schedule guard but still requires every checker resolved. Construction/reset
clears the vector and counters; destruction releases all checker records,
authored precompute state, and resources. Evidence:
`claim.note.air-ladder-precalc-presentation`.

## Presentation boundary

Type-9 precompute walks the `0x24` authored controls, emits one `0x34` main
segment per authored span, and builds a separate root/control path-point
vector carrying decoded width, lane center, vertical value, and a final-point
marker. The presentation update projects root plus those authored endpoints,
checks their cardinality against the precomputed path, and passes both
raw-relative and transformed arrays to the shared geometry builder.

The precompute selects an SLA tag independently for the root and every authored
endpoint with the ordinary integer-span selector. Presentation uses each
endpoint's own tag as the key for its STP/SFL/SLP projection transform; an
endpoint never inherits the chain root's tag merely because both belong to one
AirLadder. Generated samples separately use the tolerant float-span selector
described in `spec/c2s.md`.

Generated `0x20` samples take a separate presentation branch for individual
checkpoint effects; they are not the main path vertices. Each effect chooses
slot `clamp(trunc(decoded_width + 0.999) - 1, 0, 15)` from the first-sample or
later-sample external resource table. Its transform is centered at
`lane + width/2`, uses vertical
`(value - 1) * 3.8934999 + 0.14999962`, starts projected position at `-10000`,
and sets lateral scale to `width / external_resource_intrinsic_width` when that
width is positive, otherwise 1. The resource itself is outside the clean-room
product.

Each generated runtime record initializes its one-byte result-table index to
`0xff`. Resolution maps the checker's judgement tier through the executable's
embedded tier-0-through-11 mapping
`[0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0]` and stores that index. Presentation obtains the current row
count of the externally loaded
`earth::Table<projDB::NotesJudgeResultTableRecord>`, narrows it to one byte,
and uses the exact visibility rule:

```text
visible_update = !(stored_result_table_index < byte(loaded_row_count))
```

An in-range resolved index therefore hides the external checkpoint resource;
the unresolved `0xff` sentinel stays on the visible-update path. The external
rows and their player-facing names are not copied or inferred.

Type 9 clips main
segments to projected range `[-600, 50]` and fills three resource-owned vertex
streams. It then calls the render diagnostic accumulator in category order 7,
9, 8; this counts vertex triples and is not a draw-order interface. Every
vertex is `0x18` bytes with render lateral,
vertical and projected coordinates, an executable-owned packed color, and two
neutral coordinates. Streams 0 and 1 use exact white `0xffffffff`; stream 2
uses exact low-alpha white `0x40ffffff`. Stream 0 is one full-vertical
six-vertex quad with decoded-width
half extent `1.5 * width`. Stream 1 is one six-vertex quad on vertical zero;
its width scale is `0.75` below/equal zero, `0.65` at/above `15.574`, and the
recovered linear expression between. Stream 2 is a full-vertical quad emitted
in both windings (twelve vertices), with fixed half extent `1.96`; nonzero
parsed style code selects `0.98`. Style 15 sets all three main resource IDs to
`-1`, so positive-interval `NON` remains an ordinary AirLadder with checkpoint
effects but no main three-stream path. Zero-interval `NON` takes the separate
HeavenHold class before this path.

The final float in every neutral vertex comes from an embedded 16-entry table
indexed by normalized style code. The exact table is reconstructed as
`air_ladder_style_vertex_coordinates`; invalid style codes normalize to code
zero before lookup. This closes the numeric producer/consumer without claiming
a texture-atlas meaning or copying the excluded resource.

The three streams are owned by three consecutively constructed
`projView::Joint` graph children. Their graph callbacks run in stream-index
order 0, 1, 2. Each valid callback copies its vertices, executable-selected
primitive topology/mode (`[3,3,2]` in stream order), and runtime resource handle into a
dynamic primitive. With the ordinary null collector, compatible adjacent state
may be batched before a `0x38` record is appended to the default command vector.
The handle is selected through a 16-entry wrapper pool populated from checked
`0x10`-byte `projDB::TextureTableRecord` rows; row `+0x0c` resolves an external
filename/path. Resource ID `-1` retains a zero handle and suppresses that
Joint's submission.

This closes the asset-independent scene traversal and command-submission order.
The external files determine player-facing resource roles, material behavior,
and final pixel compositing, so those are deliberately not named or reproduced.
The main stream field layouts, extents, winding, clipping, primitive-counter
call order, and checkpoint-effect lifetime predicate are reconstructed. The
viewer must retain authored controls for its main
trace and the recovered generated cadence for checkpoint effects; it must not
invent a fixed subdivision or additional whole-height rungs. Evidence:
`claim.note.air-ladder-precalc-presentation` and
`claim.presentation.joint-dynamic-primitive-producer-closure`.

Reconstruction: `AirLadderPathPoint`,
`generate_air_ladder_checkpoints`, `select_ald_runtime`,
`air_ladder_new_resolution_count`, `air_ladder_is_terminal`, and the
`air_ladder_*` constants. Focused tests:
`tests/air_ladder_judgement_test.cpp`.
