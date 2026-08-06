# Claim: AirLadder precomputes authored geometry and its authored end schedule

- ID: `claim.note.air-ladder-precalc-presentation`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `judgement.types`, `state.ownership`;
  viewer row `render.air_ladder`
- Last reviewed: 2026-08-03

## Statement

Each accepted type-9 record receives a chart-derived event identity. A
`projView::NotesPreCalcManager` entry keyed by that identity precomputes one
main segment per authored `0x24` control, an authored root/control geometry
path, and the final authored endpoint schedule. Runtime AirLadder uses this
precompute for its main three-stream type-9 geometry and terminal schedule;
the distinct fixed-interval generated vector owns profile-7 checkers and
per-checkpoint presentation records, not the main path geometry.

## Anchors

- `game.exe @ RAM:011c7980, FUN_011c7980, accepted-event ordinal producer, hash b72ab41d5af91578014761c4671a625a0505d482c4cb2271ecd6e5563932da34`
- `game.exe @ RAM:011c8870, FUN_011c8870, stores the ordinal at parsed type-9 +0x84, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:00b264e0, FUN_00b264e0, builds and inserts the 0x78 type-9 precompute, hash ca9889f09ac061485f9e4547783966d62972986c2a25004924d1563603d98acb`
- `game.exe @ RAM:00b236a0, FUN_00b236a0, authored-control segment/path producer, hash e4793bf2a9246ddbe30bb6a6e56d30e903dc19fba6fa63e220eedd48908bbf3c`
- `game.exe @ RAM:011c1030, FUN_011c1030, independent authored root/endpoint SLA-key selector, hash bfec6950b674ce8edc1606194153e5c8bdd0bd24a832858be27caed9b68dfd22`
- `game.exe @ RAM:00b23470, FUN_00b23470, checked manager +0xc lookup by event identity, hash f0ee8059e6a782275f7157204f998c77b6497574a16fc06ca97551bfb3753ee6`
- `game.exe @ RAM:00c033a0 -> RAM:00c00cd0, precomputed geometry-wrapper initialization, hashes 5db616a7a5ea7f20cc629e70691b0ce5500cdb8753cdc8c226b0f2151eb1b1f5 and 5d7c589e6c64c73b01f7ba5ce2ba17e5fedec35f884c928cda6fee0281129b0a`
- `game.exe @ RAM:00c132f0, FUN_00c132f0, precompute lookup/end copy and generated-checker load, hash 2d7edc23ab6a2229647e9f6d2a9e3061e2ad6205607aa8bce11a531d69d6340c`
- `game.exe @ RAM:00c12d80, FUN_00c12d80, authored-end/all-checker terminal predicate, hash b267c18c2b3ab7640df3fc02354fb00cbb46328ad150432c449aa2e29574b516`
- `game.exe @ RAM:00c12350 -> RAM:00c03c00, authored main-path update and shared type-9 geometry, hashes bc494d630f8811a52cfffd3252d7358558fb99e73a1cc951c33814f2dd88a458 and d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`
- `game.exe @ RAM:00c13050, FUN_00c13050, generated-checkpoint resource transform/visibility update, hash 2544bba4e71bbaf3c6d7e31654357192fd0a0037b2a16d589a03873d1a81d211`
- `game.exe @ RAM:00c12120, FUN_00c12120, checkpoint result-index sentinel/resource initialization, hash 9b2a31c6ec8090725926c6c13a1ebfecbdd3862dc9a1d0fb4b2a75fec3cd3e9f`
- `game.exe @ RAM:00c11ca0 -> RAM:00c196f0 -> RAM:00c18270, resolved checkpoint tier production, result-index write, and embedded mapping, hashes 5ad898a40e7f8d1a0398d50cab2b52305cb3d22e99915687570ace0005f5bb6f, 612434c6f6dede886f2b8ac66aff0901b49ebe1b4cafce489978f747d02b48b9, and 466f0eaddee9aec2e17bb478140437ef316a7b8a8945cd5be7c087fa5f4ecc78`
- `game.exe @ RAM:0108d080 -> RAM:0108e060, NotesJudgeResultTable construction and 0x18-row count accessor, hashes fd2ceb8dfaf4a9795ebfa2953573b6e0120427b56fc4f545aea2721013bf46b1 and 5e228ba1de95638a398fbe0dffd53d9ebd379547d8fa1fc7c1694807969e79eb`
- `game.exe @ RAM:00c12b00, FUN_00c12b00, checkpoint-resource release/reset walk, hash 54a86a7d7b481bdab226ebce77050051ad47686ac628f77f9c671488f59a21da`
- `game.exe @ RAM:00b286a0, FUN_00b286a0, checkpoint vertical transform, hash 31a7cd6a4f8746d96c42089a7f6b85e83c376c6d9d5ca3b1208ba7b3fe6cea6b`
- `game.exe @ RAM:00b2b260 and RAM:00b2b300, checkpoint resource slot and intrinsic-width scaling, hashes ae79c07f9a9c421fd200b0e5be5d346fb5b535fc945e2d1a43c652969533ccb5 and 7d2f5b4d541a83549d8d4db4140d7080f6dd330f83821415aa8c370d34840584`
- `game.exe @ RAM:006ddc30, FUN_006ddc30, 0x18-byte vertex append, hash 3cac7fa29640e1cac62b72547ad7306252a8970b411aa3f68da28de8819c993f`
- `game.exe @ RAM:00c32330 -> RAM:00c322b0, primitive-counter wrapper/accumulator, hashes 73420ec10ffdbe80edfa7c860151a03f1f48a29c71929e624d38bbdf38a987b5 and 614410c6fc714a3c4ff3faabb3b225e1de0924f2cb1f809a55c02def1ff5b368`
- `game.exe @ RAM:00bfec90 -> RAM:00bfebb0, three-Joint owner/construction in stream-index order, hashes 7639412ecc4b50cae3be0fd2d20688f32b56442659fec1fb770b678c26a2444c and f3f9eed732002e4f91d8f62528b3deccd7c18d15cd1f128d9aec558bce59132f`
- `game.exe @ RAM:00d7e550 -> RAM:00d7e350, Joint graph callback and dynamic-primitive state fill, hashes 63ca6a8fac377d8be80bc4798b93b6db0e3a868ab804ff20a126f914a4166273 and 782a23c0695543971d538d73a78b945a81b97baf6880d9addaf8e47acf8b5c6e`
- `game.exe @ RAM:00609a00 -> RAM:00605ef0 -> RAM:0060ada0, graph registration and stored-order traversal, hashes 57fae60beb1d9b36de9de7d1d046eb3bfc2dbb71f6d91c112aa6d5859e187e41, 4c9c26639dd752bfd59e5adf1e80b905c497fa0723867cadaea2a16475750b39, and fbfa41ea943c45f71662836d9c1c3d1693e6071e96b80adca6ab342ed9dd98b0`
- `game.exe @ RAM:006dc000 -> RAM:0066dbc0 -> RAM:0062e4d0 -> RAM:0063b360, dynamic-primitive batching/default command append, hashes 4bacae0971c59df04f2ff551d469e052839c7f4a420d9ee3ec911eeb98815a39, 61db305364da24f183847cf64c00956d6fdef0c376b6c48fb5fc2ae1bc70c0ae, 2a3e3bcc252530443d55da9af110ac47bd735e6b63694ad3391c26075f8f902d, and 06ee2b304a2c4db2f82df081cacd6d28ad04bb80e1527e241d74227817019ef5`
- `game.exe @ RAM:00b2d160 -> RAM:00c331b0 -> RAM:010f8a40, resource-pool population, TextureTable filename load, and checked record accessor, hashes 7101f27648ef39415140d10b2ca5ae72109e49b26bce1e66ea994c65982a61e0, e3cc0bfdb95450f4d9d09ea6dfaeda35ffb75ae1c2ff98840f56b4485fad3195, and 487925ed07b27f31ff7905ebabdbf8a02698511de634935ba5cbb886e9ddfc38`
- `game.exe @ RAM:00b2d010 -> RAM:00c331a0, checked pool lookup and runtime-handle return, hashes 1edb5e087415550d0d3930a14c96704e38c96cbb20c8d5c03a8c4e631c4b4e5b and 1b8e59707ba9b5e029f07150c5f8bb2049f7f60e78e81b50b0e01c378b7467f9`
- `game.exe @ RAM:00b28870 and RAM:00b28680, lateral and vertical transforms, normalized opcode hash 1cc795d4bdfb7e1e4f971672fd21b7eaa2a3ef012b82b2bc753e726b14909e68`
- `game.exe @ RAM:00b21ea0, FUN_00b21ea0, manager-map reset and precompute destruction, hash 4d824d2126b6017b48290957efdb03e3fc279921be9fdf1f2ba721ddb77f53ff`

## Observations

- The type-9 parser writes its accepted-event/pass ordinal to parsed offset
  `+0x84`. The separate result-component identifier is at `+0x88`; neither
  field is an external configuration selector.
- `FUN_00b264e0` allocates a zeroed `0x78` object, delegates its fill to
  `FUN_00b236a0`, and inserts it into the manager map at `+0xc` under parsed
  `+0x84`. Missing-key lookup throws rather than selecting a fallback.
- The fill walks parsed `+0x44..+0x48` in `0x24`-byte authored-control steps.
  It emits a `0x34` main segment for root-to-first-control and every later
  authored span, stores the final authored schedule at precompute `+0x28`,
  and builds a `0x10` path-point vector containing root/control width, lane
  center, vertical value, and a final-point marker.
- The fill calls the integer-span SLA region selector independently for the
  root and every authored endpoint, stores those keys in each `0x34` span, and
  the presentation update passes each endpoint key to the keyed projection
  transform with that endpoint's schedule. The main path therefore does not
  inherit one root SLA key across a chain.
- The same fill selects three style-indexed resource descriptors and one
  embedded 16-entry style vertex-coordinate table at `018d3e98`. Style 15
  selects resource ID `-1`; an out-of-domain style normalizes to code zero
  before coordinate selection. The exact coordinates are reconstructed without
  assigning a texture/resource meaning. Resource identities remain excluded.
- Runtime construction copies parsed `+0x84` to runtime `+0xe8`, performs the
  checked lookup, and copies precompute `+0x28` to runtime `+0x118`. Only after
  that does it walk parsed generated records at `+0x6c..+0x70` to construct
  the independent `0x88` checker/effect records.
- Each generated effect chooses a clamped resource slot from decoded width:
  truncation of `width + 0.999`, minus one, clamped to `[0, 15]`. The first
  sample selects one external table and every later sample another. Its stored
  transform centers laterally at `lane + width/2`, maps vertical as
  `(vertical - 1) * 3.8934999 + 0.14999962`, begins projected position at
  `-10000`, and scales only the lateral axis by `width / resource_intrinsic`
  when the external intrinsic width is positive (otherwise scale 1).
- Each runtime checkpoint initializes a one-byte result-table index to `0xff`.
  When its checker resolves, the embedded judgement-tier mapping replaces that
  sentinel. Tiers 0 through 11 map to indices
  `[0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0]`. Presentation narrows the externally loaded
  `earth::Table<projDB::NotesJudgeResultTableRecord>` row count to a byte and
  hides the checkpoint resource exactly when `stored_index < narrowed_count`;
  otherwise it updates projection and requests visible state. Thus unresolved
  `0xff` remains visible for every byte-sized count. The table rows are
  external, but their `0x18` stride, count source, comparison, and consumer are
  closed.
- The runtime update first visits those generated records for their individual
  presentation state. It then appends root plus authored precompute endpoints
  to two position arrays and passes them with the precomputed path vector to
  `FUN_00c03c00`. Cardinalities must match or all three render resources are
  cleared.
- The shared type-9 builder clips segment endpoints to projected range
  `[-600, 50]`, interpolating every carried geometric value at a crossed
  boundary. Each vertex is `0x18` bytes: render-space lateral, vertical and
  projected coordinates, packed color, and two neutral coordinates.
- Stream 0 emits six vertices for one quad at the segment's full vertical
  endpoints. Its endpoint half extent is decoded width multiplied by `1.5` in
  render-space units. Stream 1 emits a six-vertex quad on vertical zero; its
  half extent applies the exact height-dependent scale `0.75` below/equal zero,
  `0.65` at/above `15.574`, and the observed linear expression between them.
  Stream 2 emits both windings of one full-vertical quad, twelve vertices, with
  fixed half extent `1.96`; parsed style code nonzero selects `0.98`. Style 15
  assigns resource ID `-1` to all three main streams even when a positive
  interval keeps the ordinary AirLadder runtime class.
- After filling the three stream vertex containers, the builder calls the
  diagnostic primitive counter in category order 7, 9, 8. The accumulator
  counts vertex triples and does not submit a draw, so those categories do not
  establish layer order.
- The wrapper constructs exactly three `projView::Joint` objects consecutively
  in stream-index order. Each Joint registers its `sea::GraphNode` base during
  construction. The graph traversal invokes the active node callback and then
  visits its stored child vector in order, so the three Joint callbacks occur
  in order 0, 1, 2.
- Descriptor installation resolves each valid resource index through a
  16-entry wrapper pool populated from
  `earth::Table<projDB::TextureTableRecord>`. The checked record accessor uses
  a `0x10` row stride and resolves record `+0x0c` through the shared string pool
  as the filename/path. Loading that external path produces the runtime handle
  later copied into Joint draw state. Resource index `-1` retains the default
  zero handle and makes the Joint callback skip submission. The per-stream
  values 4, 3, and 2 configure primitive topology/mode, not layer numbers.
- A valid Joint callback requires a nonempty vertex count divisible by three,
  copies the `0x18` vertices and runtime resource handle into dynamic-primitive
  state, and submits with a nullable collector pointer. The ordinary null-
  collector path may merge an immediately preceding compatible batch, then
  appends a `0x38` record to the default command vector. This closes scene
  traversal and command-submission order. Final pixel compositing and the
  player-facing role of each stream remain external-resource semantics rather
  than chart or executable constants.
- Terminal returns false only when `current <= authored_end` and the two
  values differ; otherwise it requires `resolved_count == checker_count`.
  For finite values this is inclusive `current >= authored_end`. An unordered
  float comparison bypasses only the schedule guard, not checker cardinality.
- Manager reset destroys every keyed precompute, including its main-segment,
  path-point, wrapper-resource, and owned auxiliary vectors.

## Reasoning

The parser-side ordinal write, manager insertion key, runtime key copy, and
checked lookup form a closed identity path. The precompute producer's authored
vector stride and the runtime main-geometry consumer's matching cardinality
form a separate closed presentation path. The generated-vector stride appears
only in the later checker/effect construction and update loop. The stored
precompute end schedule reaches the terminal predicate without an external
load, disproving both the former configuration-threshold explanation and the
former generated-main-geometry explanation.

## Alternatives and falsifiers

- Competing explanation: parsed `+0x84` selects external tuning, or generated
  checkpoints are the main type-9 ribbon vertices.
- Evidence that would disprove this claim: an external loader feeding parsed
  `+0x84` or precompute `+0x28`, a main-path append sourced from parsed
  `+0x6c`, a type-9 precompute keyed independently of the accepted-event
  ordinal, or a live ordinary type-9 geometry path that bypasses the checked
  precompute lookup.

## Unknowns

- The three resource identities, texture/material contents, player-facing
  stream roles, colors, shaders, and final pixel compositing are external
  product data and are not reconstruction inputs. Their executable selection,
  zero-handle skip, scene traversal, batching boundary, and default command
  append order are closed.
- The chart-only viewer does not yet simulate the logical input and result-
  table state that drives the checkpoint-effect lifetime predicate.
- The last vertex float has a closed embedded style-coordinate source and
  consumer, but its resource-facing semantic name remains intentionally
  unassigned because the corresponding assets are excluded.

## Consequences

- Ghidra mutations: supported-role plate comments at `00b264e0`, `00b236a0`,
  `00b23470`, `00c132f0`, `00c12d80`, `00c12350`, `00c13050`, `00c03c00`,
  `00c12120`, `00c11ca0`, `0108e060`, and `011c1030`, plus the supported
  singleton comment at `01c959d0`. Added supported boundary comments at
  `00bfec90`, `00c033a0`, `00d7e550`, `00d7e350`, `0060ada0`, `0066dbc0`,
  `00c322b0`, `00b2d160`, `00c331b0`, `010f8a40`, `00c196f0`, and
  `00c18270`; corrected `00c03c00` to distinguish diagnostic counting from
  Joint submission. Default symbols and types were retained.
- Spec sections: `spec/configuration.md`, `spec/judgement.md`, and
  `spec/notes/air_ladder.md`.
- Reconstruction code: authored geometry-path construction, exact terminal
  guard, segment clipping, render transforms, stream extents, and neutral
  vertex builders, including the embedded style-coordinate selector, in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/air_ladder_judgement_test.cpp` and
  `tests/air_ladder_presentation_test.cpp`.

## Verification

The accepted-event ordinal was followed independently from its pass owner to
the parsed record, manager insertion, runtime copy, and checked lookup. The
authored and generated vectors were followed separately through precompute,
runtime construction, update, terminal, reset, and destruction. Focused tests
exercise finite before/equal/after end values, unresolved cardinality, both
unordered-float cases, authored-path construction, clipping at both projected
bounds, all three neutral vertex streams, width/height extents, winding, and
generated-effect slot/transform selection. The presentation/SLA tests also
cover independent region selection and embedded style-coordinate lookup. The
full 35-target CTest suite and
`harness.py validate` pass after dependent artifact correction. Fifteen
coverage-spread local ALD charts, including positive-interval `NON` and an
ALD-plus-SLA case, loaded and rendered through the explicit headless viewer
hook with zero failures; four materially different outputs were inspected for
nonempty type-9 geometry.
Focused presentation tests also cover the `0xff` sentinel, in-range/out-of-
range result indices, all 12 embedded tier mappings, invalid clean-room tier
rejection, and exact byte narrowing of the external table count.
That product smoke test verifies implementation stability only and does not
assign external resource semantics or close the missing viewer-state unknown
above.
