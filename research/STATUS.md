# Research status

## Snapshot

- Research milestone: GitHub issues 1-15, 17, and 18 have been investigated
  against the exact snapshot or classified as product/owner work; human-only
  issue 16 is deliberately excluded. The corrected ALD
  presentation slice, ordinary Slide endpoint/path presentation, and
  HeavenHold authored presentation slices are complete.
  `claim.note.air-ladder-precalc-presentation` closes authored/generated
  ownership and asset-independent ALD geometry;
  `claim.note.slide-presentation-classes` closes Slide endpoint ownership and
  `claim.note.slide-path-presentation-geometry` closes its marker grouping,
  zero split, projected clipping, three vertex streams, and Joint callback
  order. `claim.presentation.heaven-hold-authored-mesh` closes the direct
  HHD/HHX schema, all three factory origins, authored single-stream path,
  selectors, phase visibility, clipping, submission, and reset. Former ALD
  correction records and the old
  independent closure verdict remain superseded. All other previously closed
  paths remain standing unless the required fresh contradiction audit exposes
  another dependency.
- GitHub issue 12 was rechecked on 2026-08-09: its body is unchanged, it has no
  comments, and all seven requested checks still map to active exact-binary
  claims. Its open GitHub state reflects remaining product boundaries, not a
  newly reported or unanswered binary question.
- The updated 2026-08-18 issue pass closes every static question listed in
  issue 15, puts the actual group-1 parser/schedule builder under an independent
  corpus differential for issue 17, and classifies all 42 issue-18 viewer
  provenance items. The durable ledgers are
  `research/GITHUB_ISSUE_VERIFICATION.md` and
  `research/VIEWER_PROVENANCE_AUDIT.md`.
- The follow-on external-table provenance slice closes six presentation
  families from the shared path composer through typed row access and first
  consumer. Their exact basenames are the registered record names plus `.bin`;
  row strides and invalid fallbacks are recovered, while only the selected
  database directory, row contents, and referenced resources remain external.
  Evidence: `claim.configuration.external-presentation-table-provenance`.
- A later resource-pool rewalk corrected the 16-wrapper TextureTable rule:
  slots beyond a short nonempty table repeat its last row, while a zero-row
  table selects `-1` and leaves every wrapper invalid. The complete six-caller
  Joint handle accessor, wrapper resize/clear/destruction paths, and zero-handle
  submission gate are now explicit and tested. Evidence:
  `claim.presentation.model-resource-pool-boundary`.
- The EffectManager indirect-hook rewalk corrected another flattened rule:
  EffList admission limits submissions since the previous update, not active
  occupancy. Non-forced append separately evicts the oldest active effect at
  capacity; forced preload can exceed capacity. All twelve vtable slots,
  per-update reset, cooldown, statistics, both all-list clear owners, and every
  all-list update owner are now closed and tested. Evidence:
  `claim.presentation.effect-list-lifecycle-closure`.
- The child EffectBase indirect layer is also closed. Its complete eight-slot
  vtable and all twelve callbacks establish exact pending/current phases
  `0..3`, one-update delayed natural terminal removal, same-pass explicit stop
  after note work, retained visibility bit `0x4`, Slide position matrix,
  one-shot runtime entry-0 selection, preload's exact visible/entry sequence,
  and destruction. Only external player payload and natural disappearance
  time remain inputs. Evidence:
  `claim.presentation.effect-player-state-machine-closure`.
- Binary: `game.exe`, SHA-256 `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`
- Ghidra project: local project `chart`
- Current Ghidra writer: codex-root (binary saturation and external-source
  closure)
- Stage-one status: the three affected parser/judgement/ownership rows are
  corrected and verified by focused/full tests plus aggregate corpus and
  viewer smoke coverage; only the independent closure-audit row remains open
- Active product phase: build a canonical offline C2S chart-gameplay viewer for
  arbitrary selected files, using original presentation and no shipped game
  assets or charts
- Stage-two status: open; the existing viewer is functional but contains fitted
  and repo-only assumptions that often diverge from gameplay footage
- Final acceptance authority: the owner, after hands-on review against real
  gameplay footage and gameplay experience; automation cannot close this gate

## Viewer product stage

- Baseline: `scripts/c2s-viewer.html` parses selected files locally and renders
  a deterministic corridor/playfield without embedding chart data.
- Provenance limitation: its initial author could inspect this repository but
  not `game.exe`. Existing renderer rules therefore range from recovered spec
  behavior to corpus inference, footage fitting, and original placeholders.
- Canonicalization method: use GitHub renderer issues and matched footage to
  identify discrepancies, then resolve rules from active claims/specs or
  focused Ghidra decompilation. Keep resource-only values configurable and
  labeled instead of presenting a fit as binary fact.
- Asset boundary: `music/` charts—official and unofficial—are local test inputs
  only and are not shipped. No game artwork, audio, video, fonts, models,
  effects, accounts, servers, or player records belong in the product.
- Architecture goal: one arbitrary-file parser and normalized chart model feed
  timing/playback and original render primitives; drawing code must not carry a
  second field schema or chart-specific exceptions.
- Durable stage-two handoff: `docs/VIEWER_ROADMAP.md` records the clean-room
  product boundary, issue 1-5 triage, exact current Ghidra leads, and ordered
  implementation/research queue.
- Shared scene/camera behavior is now binary-backed by
  `claim.presentation.common-scene-camera`: BgScene priority 9900 traverses
  before MainScene priority 10000; active BasicCamera ownership, constructor
  fallback values, aspect selection, projection/view matrices, and viewport
  conversion are exact. The generic SVO/UVC loader and its controls/failure
  behavior are also closed. Field setup selects rows 43/47/46 from externally
  loaded `AcroartsTableRecord.bin`, prefixes every row string with `acroarts/`,
  and uses the empty string on an invalid row; Forester `2DLayer` separately
  comes from rows 6/29/29 of `LayerTableRecord.bin` with zero as the invalid
  fallback. The prior claim that only two resource paths used the prefix is
  corrected. The graph linkage and motion samples are absent external data, so
  the final gameplay pose remains an explicit calibrated parameter rather than
  a recovered constant; no camera selector/consumer/fallback remains open in
  `game.exe`.
- External presentation-table lookup is now binary-backed by
  `claim.configuration.external-presentation-table-provenance`: the common
  manager's directory/name/`.bin` composition, six exact effective basenames,
  typed row strides, checked fallbacks, and first feedback/model/texture/
  field-line consumers are closed. The clean-room boundary retains absent row
  values and assets as explicit inputs instead of inventing substitutes.
- Runtime note-model and Joint-texture ownership is now binary-backed by
  `claim.presentation.model-resource-pool-boundary`: the dedicated owner
  performs the fixed ModelSet 0..407 inclusion scan, checked 16-row texture
  population, lazy keyed model acquisition/reuse, activation, release, and
  teardown. The separate RTTI `CacheManager` has no direct runtime-note or
  Joint initializer references; its unrelated game-object consumers are not
  conflated with this path. Concrete resource rows and payloads remain
  external.
- Cross-family precompute ownership is now closed by
  `claim.presentation.notes-precalc-manager-map-closure`: fresh chart load
  clears the manager, dispatches only parsed types 2/9/10/13, and stores Slide,
  AirLadder, AirSolid, and HeavenHold objects in four independent exact-key
  maps. All getters, family consumers, owned-object destruction, and recovered
  chart/scene/gameplay resets are enumerated; a missing key throws rather than
  choosing a fallback.
- Joint-backed primitive ownership is now closed by
  `claim.presentation.joint-dynamic-primitive-producer-closure`: the sole base
  Joint constructor has exactly six RTTI wrapper callers, five assigned to
  reachable Hold/Slide/AirSlide/AirLadder/AirSolid/HeavenHold owners and one
  retained as an unreachable Field exclusion. The common graph callback,
  resource admission, descriptor initializers, topology values, batching,
  default command append, and unregistering teardown are closed. This sweep
  corrected a real prior contradiction: AirLadder's producer passes selector
  9, so its topology triple is `[3,3,2]`, not `[4,3,2]`.
- Common Sprite-backed primitive ownership is now closed by
  `claim.presentation.air-sprite-dynamic-primitive-closure`: `air::Sprite`
  construction/resource replacement, exact 3-by-3 anchor rules, six-vertex
  position/UV order, scale-rotation-translation, optional UV matrix, color,
  submission, and teardown are reconstructed. Its complete four-function draw
  caller set assigns the sole chart-system use to the `projView::System`
  background SpriteNode and excludes font/ruby, `air::LedObject`, and
  `EmoteControl`. The 16 presentation-pool wrappers are confirmed
  Sprite-backed handle carriers for Joint descriptors, not another direct
  Sprite draw path; WindManager, AuraScene, and `star::SglVTFWaterLine` direct
  primitive producers have separate non-note owners.
- The common constructor-root sweep is now closed by
  `claim.presentation.dynamic-primitive-util-owner-inventory`: the sole
  `DynamicPrimitiveUtil` constructor thunk has exactly 17 function roots, all
  assigned. All four `air::Primitive` overload caller sets are complete; RTTI
  maps every non-Joint caller to GUI/debug/editor classes, while
  `projView::JointBase` is the sole chart-side owner. The two anonymous roots
  belong only to literal `DefaultDebugScene`, the default extended overload is
  unreferenced, and GUI/font/Surfride/SPK/SGL roots are retained as explicit
  subsystem boundaries rather than guessed away.
- The common primitive lifecycle is now closed in both directions by
  `claim.presentation.dynamic-primitive-entry-setup-reset-closure` and
  `claim.presentation.dynamic-primitive-finalizer-teardown-closure`. The two
  dynamic vtables share exact configuration, pending-pointer, reset, and
  finalizer slots; selector-to-stride rows and backing allocation remain
  explicit renderer inputs. The finalizer's bit/copy/submit/clear ordering is
  reconstructed, and the complete destructor reverse map agrees with every
  construction family. This pass corrected an overclaim: the common
  destructor restores only the base vtable and performs no hidden buffer or
  container release.
- The standalone convenience-helper reverse sweep is closed by
  `claim.presentation.dynamic-primitive-convenience-helper-owner-closure`.
  All five line/triangle variants are assigned: three are unreferenced, live
  engine calls collapse to a literal performance/debug overlay, and the only
  other live caller is the already excluded RTTI water-line owner. No chart
  family obtains a second geometry path through these generic helpers.
- Chart topology-derived submission flags are now closed by
  `claim.presentation.primitive-topology-derived-flags`. The complete setter
  caller inventory leaves only Joint and Sprite on the chart side; executable
  table rows for modes 2/3/4 and the threshold/clamp/bit updates are exact.
  These bits survive the common finalizer and are no longer treated as an
  opaque backend side effect.
- Shared scene pass behavior is now binary-backed by
  `claim.presentation.common-scene-pass-ordering`: active external `BasePass`
  objects are stably planned by `PassIndex`, submissions take the first exact
  Type/User/Range match, all eight per-pass sort modes and their 32-record
  insertion/adaptive-merge boundary are reconstructed, and draw-index 0..31
  traversal/reset is closed. Concrete pass rows and material-selected
  depth/blend/shader/texture/final-pixel state remain external and are the next
  static-analysis boundary.
- Material-to-pass behavior is now binary-backed by
  `claim.presentation.material-pass-flags`: all three generic submission kinds,
  exact Opaque/Punch/Trans/2D/Reduce and four-bit User selection, the two-word
  Sort 0/1 callback/default paths, and pass/submission color/depth offscreen
  request propagation are closed through the pre-draw filter calls. This also
  corrects the former unsupported name for the sort key's low word. Concrete
  material/filter graph rows and final backend depth/blend/shader/texture/pixel
  state remain external rather than recoverable constants.
- Cross-family chart mirroring is now binary-closed by
  `claim.presentation.chart-mirror-transform`: all roots and distinct control
  fields are reflected once during parsing with wrapped `16-lane-width`, AIR
  direction pairs/signs are exact, and every reconstructed family consumes the
  resulting coordinates through the same centered lateral basis or affine path
  operations. HeavenHold's authored selector transform and feedback X flip are
  explicitly separate. Viewer mirror selection and final camera/asset review
  remain product/owner work, not an open binary rule.
- Cross-family update/composition is now binary-closed by
  `claim.presentation.cross-family-update-composition`. The audit corrected an
  older single-vector model: roots and AIR-family attachments live in distinct
  vectors, and every primary update precedes every secondary update. All
  catch-up substeps finish before one ten-list effect pass; pending
  materialization and scene exports follow. Feedback serials/overlay writes
  retain that update order, while final draw overlap is separately governed by
  scene priority and the exact externally configured pass keys. The synthetic
  interaction matrix covers both equal-key stability and unequal-key
  reordering. External resource values and owner pixel review remain explicit
  boundaries rather than guessed binary facts.
- Exhaustive ledger: `research/VIEWER_COVERAGE.tsv` separates exact-binary
  closure, product implementation, and owner review. Known issues are seeds;
  the final binary saturation audit must discover and close unreported paths.
- Corpus/reference acceptance: use an aggregate feature inventory to render a
  broad local suite. Each reviewed chart must reach zero undefined keywords,
  unexplained primitives, heuristic fallbacks, appearance-only values, and
  unclassified differences; feature saturation plus owner review determines
  when the chart set is sufficient.
- Complete issue verdicts and handoff answers are durable in
  `research/GITHUB_ISSUE_VERIFICATION.md`. The pass recovered presentation-side
  endpoint SLA consumers for sustain types 1/10/13, Slide's post-shrink
  endpoint width, MET unit/count arithmetic, common Air vertical transforms,
  AIR direction resource-mirror pairing, exact DCM query scope, the full
  91-name registry, and orphan-secondary rejection. The viewer now uses
  source-order first-compatible Slide chains and AIR-family attachment,
  preserves the six-field legacy Slide endpoint-width inheritance, suppresses
  rejected orphans, exposes unknown `T_*`, removes its synthetic position-zero
  BPM record, ports the snapshot's three-way introsort, generates meter-aware
  beats/bars, fixes ordinary AirHold's vertical origin, and offsets AirSlide
  actions. HeavenHold/type-13 now has its distinct authored-control mesh,
  direct HHD/HHX schema/chaining, all three factory origins, selector
  transforms, clipping, and two-resource lifetime reconstructed and routed in
  the viewer. Its exact phase-state simulation remains a product gap alongside
  malformed numeric parity, seek-equivalent playback state, shared
  camera/viewport recovery, and result-state/external-resource integration.
- Browser parser/builder verification is now automatic: `harness.py viewer-audit`
  serves the actual viewer to isolated headless Firefox and fails on parser
  exceptions, unknown records, rejected AIR-family associations, group-1
  schedule mismatches, or ineffective mutation sentinels. The full 7,752-chart
  corpus passed with zero in every failure category; 595 group-1 charts and
  19,890 records matched the independent reference, and all three deliberate
  builder mutations were detected. A separate
  20-chart coverage-spread render smoke plus focused legacy/modern renders
  loaded successfully with empty unknown panels. This does not close visual
  fidelity or owner review.
- Corrected ALD corpus check: all 7,752 local charts yielded 195,469 ALD lines
  in the two known field shapes and 91,368 compatible chains. The recovered
  interval producer gives 100,952 samples across 43,577 positive advancing
  chains; 47,791 nonpositive-interval chains correctly produce none. Exact
  endpoint landing and overshoot are both present, with no new disposition.

## Established findings

- Gameplay setup freshly selects its source chart path from an externally
  populated process catalog. A record needs at least six fixed-size selector
  entries; the requested byte is clamped to the external selector-table's last
  entry. Missing catalog data produces an empty path, while malformed table
  cardinalities remain explicit. The first literal `.xml` substring is then
  replaced with `.c2s`, and the descriptor-driven parser resets chart-owned
  state before ordered passes.
- Group-0 parsing has fixed defaults and source-order last-wins writes.
  Recognized RESOLUTION is rejected so 384 remains fixed; missing numeric
  fields become zero, valid numeric prefixes tolerate trailing content, and
  nonempty no-conversion/range errors escape as exceptions. VERSION raw/triple
  fields do not gate the gameplay path. PROGJUDGE_BPM, PROGJUDGE_AER, and the
  positive TUTORIAL predicate own Air cadence, final AHD/ASC end filtering, and
  Air-family interval exclusion respectively.
- The successful parser tail computes detailed retained family/position
  summaries, note-only and all-event horizons, and a separate caller-supplied
  count object. The gameplay loader ignores the caller object and setup
  recomputes record/category counts directly from finalized records. The
  note-only horizon merely seeds the all-event horizon; the latter feeds only
  derived BPM-duration statistics and meter/grid coverage. The only
  summary-based six-profile validator is unreferenced in this snapshot, so
  none of this metadata is a hidden VERSION, note-availability, factory, or
  judgement gate.
- The gameplay loader clears parsed chart state before file validation.
  Precheck/tokenizer failure leaves an empty chart, while successful
  tokenization makes per-record rejection skip-only. Scene setup ignores load
  status and queues every surviving parsed-record index; failure therefore
  produces an empty queue and zero initial runtime notes. Runtime notes are
  closed by manager construction/full reset, two later scene teardown entries,
  and manager destruction; state-0 setup advances forward with no recovered
  in-scene retry.
- Runtime notes materialize lazily once per outer update after all current
  input/manager substeps. Pending records use a strict raw-delta shortcut below
  30 chart units, then an optional region-adjusted inclusive projected-position
  test; supported endpoints can rescue a failed start probe. Eligible entries
  are consumed even without a primary factory case. Construction leaves every
  factory class at current state -1 with requested state 0. The first later
  manager substep commits 0 and requests 1; the second commits 1 and first calls
  the concrete gameplay update. Primaries can expose candidates before their
  first common tick, while attached secondaries never enter candidate reduction.
  The apparent scan/scene-exit bypass byte is fixed at zero by construction and
  gameplay-state entry and has no nonzero writer or address escape.
- The far-path materialization speed is no longer an anonymous input. Gameplay
  setup starts from `PlayOptionSpeedID`; the first selected `SkillBefore` unit
  of type 6 can replace it only with a valid `PlayOptionSpeedTable` ID. The
  selected record double narrows to float and clamps to at least `0.1F`.
  Projection base offset is `[OFFSET] DRAW`, whose executable descriptor
  default is `0.0F`; reset and successful setup copy its finalized value into
  the materialization manager.
- The inherited state-action selector after the registered gameplay callback
  is dormant for runtime notes. Its exact factory and fallback callable map are
  constructed empty, and all twelve concrete constructors and load callbacks
  leave them empty; transitions therefore keep the child pointer null. A root
  state machine with explicit child-factory registrations was checked as a
  positive control rather than treating absence as a framework convention.
  The common tick's outer guard byte is likewise fixed at zero by base
  construction and has no later writer, so it cannot pause a runtime note.
- Initialized event-family data maps `TAP` to parsed type 0 and the remaining
  24 named note families to types 1 through 13.
- Group-2 gameplay callers disable descriptor arity validation. Unknown command
  names are dropped before dispatch; missing numeric event fields become zero,
  extras are ignored, valid numeric prefixes are accepted, and accessed
  no-conversion/range errors escape the load chain.
- `SLA` maps to parsed type 12 and defines a normalized half-open time/lane
  region with an integer tag. Overlaps select the greatest positive tag after
  the shared `1/192` query shift. Type 12 has no runtime factory case, but its
  selected root/end tag keys the STP/SFL/SLP transform in far-path note
  materialization. It can therefore advance or delay when another note first
  exists for candidates, input, and judgement, without rewriting that note's
  clock or windows.
- `ASO` maps to parsed type 10 and chains twelve-field two-endpoint paths.
  Widths clamp independently, mirroring uses each endpoint span, four
  structural floats become integer tenths with `int(value * 10 + 0.5)`, and
  the final exact style string selects a 16-entry coordinate table. Its
  RTTI-identified `projView::AirSolidNote` emits all candidate sentinels, reads
  no gameplay input, and submits no result. The family-local presentation path
  is now closed: the structural properties form two vertical shell surfaces;
  endpoint-SLA projection, `NON` resource suppression, raw-origin seam,
  clipping, bridge/shell topology, winding, and destruction are exact. Its
  scheduled update directly calls the authored-end predicate and requests the
  common deferred terminal state at `end <= current`, correcting the earlier
  no-terminal conclusion.
- AirSlide's family-local presentation path is closed. Its root uses separate
  placement and authored-root scale values, type-13 replaces only the placement
  value, and ASD controls alone own two width-indexed action resources. Runtime
  segments retain previous/current endpoints; root plus all authored controls
  feed the shared three-stream geometry, while parallel adjusted/raw schedules
  are rebuilt every update. Exact phase modes, feedback flag, animation gate,
  style rows, stream topologies/counters, preload, maintenance, reset, terminal
  guard, and destruction are recorded by
  `claim.presentation.air-slide-model-path`. Remaining AirSlide work is viewer
  state integration and shared downstream camera/material/layer composition,
  not an untraced family branch.
- Shared result feedback is closed through the external player/scene boundary.
  Every note vtable reaches the same pre-dispatch consumer. Ten `+0x48` slots
  use the common one-position wrapper; Mine and Flick use opcode-identical
  wrappers that apply the shared active result-control remap to the incoming
  result byte before calling it. Mine's held-lane average is confined to its
  separate zero-result success effect. Transient result masks, three ordinary
  effect gates and submit order, lane serial-group ownership, post-dispatch cue
  timing, Slide's two extended feedback effects, ten fixed effect-list
  capacities and cooldowns, normal/preload admission, lifetime update order,
  scene selection, monotonic player sequence, four-state wrapper lifetime,
  retained visibility, and Slide position control are reconstructed by
  `claim.presentation.shared-result-feedback`. External resource rows and
  final scene/material/camera composition remain explicit downstream work.
- The fresh saturation rewalk found a previously unclaimed live manager on the
  Slide extended-feedback path. `projView::CharaEffectManager` independently
  gates kind 6 through a width-dependent 32-subcell expiry map and kind 7
  through a seven-unit global cooldown; both reservations run without
  short-circuiting and have closed reset/destruction paths. Exact constants,
  the asymmetric last-cell rule, reconstruction, and tests are owned by
  `claim.presentation.slide-extended-feedback-admission`.
- Parsed type 0 constructs RTTI-identified `projView::TapNote`, with a start
  lane, table-decoded width, scheduled time, and embedded 16-lane checker. Its
  scheduled position is the parsed position converted directly to float and
  multiplied by exact initialized scalar `0.06F`.
- The active-note manager prepares candidates and selects the smallest
  nonnegative scheduled value per lane. TAP input requires equality with that
  selected candidate.
- Every gameplay source record contains two ordered 16-byte TouchSlider Y
  banks and a six-bit `photo_sensor_6..1` mask. Ordinary catch-up selects
  thresholded TouchSlider history offset `-1` or `0`; the alternate path reads
  the current thresholded sample. Both use the same snapshot synthesizer.
- Input snapshots derive rising edges independently across the two 16-source
  banks before folding them into 16 logical lanes. TAP reads the newest
  derived rising mask, not the held-level mask.
- The remaining snapshot state is closed. Six conditioned/residual
  photo-sensor bits produce an extrapolated/centroid scalar and bounded delta.
  Profiles 0 through 6 use externally configured signed-motion windows and
  thresholds; profile 7 uses only an external inclusive scalar range. The
  previous AIR marker suppresses only a newly qualified conditioned bit.
  Reset seeds scalar 65, and oldest-first eviction yields 301 retained
  snapshots in steady state.
- Parsed type 6 constructs RTTI-identified `projView::FlickNote`. It exposes a
  TAP-style candidate while awaiting its initiating rising edge but does not
  compare the selected candidate in its own edge path. After edge acceptance it
  tracks union and per-bank held-source centroids, completing on configured
  negative/positive travel or on separate edge/motion timeouts.
- FLK's missing equality gate produces directional cross-family priority. An
  earlier FLK candidate suppresses a later TAP/CHR/HOLD/Slide start, while an
  earlier gated-family candidate cannot suppress a locally valid FLK edge;
  equal candidates admit both. Ordinary FLK emits no result on its edge-start
  substep and stops exposing candidates before its later terminal result, so
  this priority asymmetry is separate from ordinary terminal-route ordering.
- `CHR` maps to parsed type 4 and constructs RTTI-identified
  `projView::CharaTapNote`. Its candidate, input, window, forced-result, and
  lifecycle slots are TAP's; fixed variant flags make the common finalizer use
  source category 1 (mapped category 4) instead of TAP's source/mapped category
  0.
- `MNE` maps to parsed type 11 and constructs RTTI-identified
  `projView::MineNote`. It exposes no candidate and credits the clipped
  previous-to-current interval whenever any bounded covered logical lane is
  held. External contact start/end/requirement values decide success when the
  threshold is reached or failure when the threshold becomes unreachable.
  Its source category 17 selects dispatch category 11, whose independent
  record fields select progress category 0 and aggregate category 8. A
  runtime-valid MNE result therefore updates the authoritative aggregate.
- AIR-family commands attach secondary type 3 and a mirrored direction code to
  an existing compatible root rather than creating standalone records. The
  factory appends an RTTI-identified `projView::AirNote` after that root; the
  secondary owns an independent candidate-free retained-profile timing checker
  and deferred lifetime.
- AIR selects one of six external checker/input profiles by root type and
  direction group. It marks the newest input snapshot at `+0x54` before reading
  the distinct derived profile bytes at `+0x4c..+0x53`. The marker affects the
  following snapshot's conditioned mask rather than forcing the current byte.
  Profile input can be retained before the pivot and emitted when current
  timing worsens. Direction groups submit source category 7 or 8, both
  aggregate-authoritative category 3.
- `AHD` and `AHX` attach secondary type 5 and construct a candidate-free
  RTTI-identified `projView::AirHoldNote`. AHX alone adds authored retained-
  profile checkpoints; AHD only extends the path/end. Its independent start,
  generated gap-checkpoint, and authored-AHX streams submit source categories
  9, 10, and 12/13, and both start/path phases must complete before deferred
  removal.
- AirHold postprocessing samples the start/AHX/end anchor chain on a 384-tick
  major grid with a tempo-adaptive halving cadence. The first sample per segment,
  segment boundaries, and final endpoint are disabled. Final AHD applies a
  nonnegative end-margin suppression rule that AHX bypasses; an enabled key-0
  open-interval table can only clear further emission flags.
- Gameplay parsing fixes position resolution at 384, performs a dedicated
  sorted `BPM` pre-pass, and cumulatively maps canonical chart positions to
  milliseconds using the preceding BPM. The shared lookup is authoritative for
  note, control-point, and generated-record schedules. AirHold, AirSlide, and
  HeavenHold scan that same finalized vector by scheduled milliseconds and
  compare the selected BPM against named header `PROGJUDGE_BPM`. `MET` consumes
  this schedule to build meter/grid vectors but does not alter note time or Air
  cadence; missing measure zero is synthesized from `MET_DEF`.
- BPM duplicate positions are ordered by the snapshot's compiled three-way
  introsort with no source-sequence tie-breaker; the clean-room sort reproduces
  its insertion, median/equal partition, and heap-fallback paths. Missing BPM
  fields become zero and no positive/finite validation follows. IEEE
  zero/negative/infinite/NaN schedule behavior is retained. Adaptive Air
  explicitly reports the source's invalid empty-map dereference boundary, the
  nonterminating doubling loop reached by a nonpositive selected BPM below its
  threshold, and downstream path nonprogress when a positive/infinite-
  reference case halves the integer step to zero.
- `ASD` and `ASC` attach secondary type 8 and construct a candidate-free
  RTTI-identified `projView::AirSlideNote`. The executable command table fixes
  ASD as the control marker: ASD controls restart generated sampling and own
  authored retained-profile checkers, while ASC carries sampling cadence and
  owns no checker. Its start, generated, and authored streams submit source
  categories 9, 11, and 12/13, and both component phases must finish.
- AirSlide uses the same 384-tick adaptive grid interface, but only the root and
  ASD restarts suppress the next sample. Final ASC enables the nonnegative end-
  margin filter that final ASD bypasses. A due disabled generated record still
  performs the ordinary inactive-gap classification/reset before its emission
  byte prevents result submission, unlike AirHold's disabled-record path.
- `ALD` maps to parsed type 9 and normally constructs candidate-free
  RTTI-identified `projView::AirLadderNote`. Authored endpoints occupy a
  `0x24`-byte control vector; a distinct producer samples a `0x20`-byte vector
  from the root at ALD's positive fixed-grid interval while interpolating lane,
  vertical value, and decoded width. Every generated point owns a retained-
  profile-7 checker. One update visits the entire runtime vector, so multiple
  accepted samples can submit source category 18 in one substep. Terminal
  state waits for every generated checker and the inclusive final authored
  endpoint schedule stored by the type-9 precompute. Parsed `+0x84` is the
  accepted-event identity used for that checked precompute lookup, not an
  external configuration key.
- AirLadder presentation keeps the two vectors separate. Generated samples own
  individual checkpoint-effect records. The main type-9 geometry is built from
  root plus authored-control endpoints, clipped to projected range `[-600,
  50]`, and written to three resource vertex streams. The later 7, 9, 8 calls
  are primitive-counter diagnostic categories, not proof of draw order. Exact
  vertex layout, extents, winding, style compact flag, generated-effect
  transform, and the result-table-index effect-expiry predicate are
  reconstructed. Three consecutively registered Joint graph children reach
  dynamic-primitive submission in stream order 0, 1, 2. Their runtime handles
  are selected through checked `TextureTableRecord` rows whose filename/path
  and material contents are external. Scene traversal, zero-handle suppression,
  compatible adjacent batching, and default command append are closed; viewer
  result-state integration remains open, while player-facing resource roles
  and final pixel compositing are excluded external semantics.
- The viewer's generated ALD records now carry the exact unresolved `0xff`
  index and use the exact visibility predicate. The former after-line opacity
  fade was removed. Resolved-state simulation remains open because profile-7
  input and the external result-table count are not chart data; the viewer does
  not fabricate them.
- AirLadder authored roots/endpoints select SLA projection keys independently;
  the viewer now uses each endpoint's own STP/SFL/SLP key instead of inheriting
  the segment root. The formerly anonymous precompute float is therefore a
  closed schedule selector. A separate final vertex coordinate comes from an
  exact embedded 16-entry style table; its resource-facing name is deliberately
  unassigned.
- The corrected viewer path was smoke-tested across 15 coverage-spread ALD
  charts selected from 1,627 files containing the command, including
  positive-interval `NON` and an ALD-plus-SLA case; every chart loaded/rendered
  headlessly and four materially different outputs were inspected. This establishes product-path
  stability only, not canonical styling or closure of the explicit resource,
  result-state integration, or external material semantics.
- The ALD factory has an exact alternate-class boundary: sampling interval zero
  plus style code 15/exact string `NON` constructs `projView::HeavenHoldNote`,
  the same class used by parsed type 13. The local current-schema corpus exposes
  six ALD lines with that visible pair. Legacy one-short ALD omits the style
  token; empty-string lookup yields code 0, so it remains ordinary AirLadder.
- `HHD`/`HHX` map directly to parsed type 13. A separate ordered postparser
  branch changes any completed type-2 Slide chain with exact field-8 style
  `HLD`/code 1 to type 13 before path generation, writing path-scalar integer
  10 to its root and every control point. Precompute consumes that value as
  `1.0`; it is not the separate presentation selector at parsed `+0xb0`, which
  remains zero. The local corpus exercises this path with two SXD/HLD lines in
  one chart.
- HeavenHold precomputes one body span per authored control and never takes
  body geometry from generated judgement records. Direct HHD/HHX token 10
  selects embedded endpoint-mirror and geometry-mode tables; HLD-origin and
  zero/`NON` ALD use selector zero. The single body stream clips to projected
  `[-600, 50]` and emits six `0x18`-byte vertices per surviving span. Root and
  body resources have separate start/path phase visibility. The clean-room
  API and focused tests cover nonzero-mode origin trimming/reflection, phase
  modes, resource selection, and reset; the viewer routes all three origins
  through this path with asset-free styling.
- All type-13 origins construct HeavenHoldNote. Its TAP-style start exposes
  candidates and preserves the source command-form flag: HHD and rewritten
  SLD/SLC select profile pair 0/1, while HHX and rewritten SXD/SXC select pair
  2/3. Exceptional ALD also uses pair 0/1. The two-bank sustain path tracks
  inactive gap and consumes at most one generated record per substep, using
  source 2 for samples and 3 for the enabled final endpoint. Ordinary due-
  record classification/reset precedes the emission gate.
- Parsed records initialize their primary generated vector empty. The shared
  postprocessor populates it for type 13 on a single adaptive 384-tick
  root-to-end span but bypasses type 9, so exceptional ALD intentionally has no
  generated queue. HeavenHold becomes terminal only after both start and path
  phases reach 4; adjusted-end grading is feedback-only.
- Type-13 duration is signed and has no nonnegative parser check. The producer
  wraps endpoint-minus-root at 32 bits and compares the delta as unsigned. A
  high-bit delta expands toward a pathological very large sampling range,
  while a signed wrap-boundary crossing can still have a small wrapped delta;
  signed endpoint ordering alone is insufficient. The reconstruction reports
  that source failure domain and zero-step cursor nonprogress instead of
  allocating or looping through either.
- `SLD`, `SXD`, `SLC`, and `SXC` construct chained parsed type-2 control-point
  records. Postprocessing converts each chain to a manager-owned keyed path
  container, and the factory constructs RTTI-identified
  `projView::SlideNote`. Its start component reuses TAP candidate/input
  judgement; its path component uses generated segment/lane windows and the
  HOLD two-bank continuation rule. Only center-window contact feeds the shared
  inactive-gap tracker. At most one due path checkpoint is consumed per
  substep, and both component phases must reach 4 before deferred termination.
- Ordinary Slide presentation has separate root, shared-path, and generated
  endpoint owners. A generated endpoint resource exists exactly for an ending
  SLD/SXD marker, with the final ending marker forced; SLC/SXC nonfinal
  controls only shape the path. Endpoint result index starts at `0xff` and the
  resource remains visible exactly while that index is not below the byte-
  narrowed external result-table count. The root X bit selects an extended
  external resource branch. Exact field-9 strings UP/DW/CE/RC/LC/RS/LS/BS
  select bounded result feedback and have no persistent geometry consumer.
  The viewer now places marked endpoints at their segment ends and shows
  unmarked controls only through an explicit debug toggle; its white endpoint
  rule is an original asset-free presence placeholder, not a recovered glyph.
- Ordinary Slide shared-path presentation builds adjacent straight endpoint
  segments, marker-delimited longitudinal coordinates, one raw-zero split,
  mode-dependent past culling, and exact projected clipping to `[-600, 50]`.
  It emits a full-width 6/18-vertex main stream, a fixed one-lane-wide center
  stream, and a mode-1-only full-width overlay through three registered Joint
  callbacks in order 0, 1, 2. Categories 1/2/3 only count triangles. A fresh
  saturation pass recovered three missed static initializers: exact packed
  colors are base white `0xffffffff`, shared low-alpha white `0x40ffffff`, and
  alternate gray `0xff666666`. Complete data-xref closure proves the same
  constants feed Hold, HeavenHold, AirSolid, and shared AirLadder/AirSlide
  geometry as well as Slide; they are binary constants, not external inputs.
  Material/final-pixel semantics remain external; phase simulation is product
  work, while the shared camera/viewport binary boundary is closed.
- The Mine zero-result success-effect width selector is corrected from an
  alleged external 17-value table to an exact binary startup table. A missed
  registered initializer at `004be800` leaves entries 0/1 zero and writes
  entries 2..16 as 1..15, so the consumer returns
  `max(clamp(width,0,16)-1,0)`. Claims, spec, reconstruction, tests, and Ghidra
  comments now use the binary-owned value.
- The authoritative clock starts from integer milliseconds at 0.06 chart units
  per millisecond, applies exact ordinary/alternate float quantization and
  discrepancy correction, integrates a runtime rate, then subtracts a selected
  runtime correction at the active-note manager boundary.
- The optional accumulated-clock rate owner is RTTI-identified
  `projView::ViewTimingManager`. Gameplay-manager construction leaves it
  disabled with unit scalar factors, and its closed executable reference set
  has no mutating producer, so normal control flow integrates at rate `1.0`.
  The manager correction selector is the bounded current `PlayOptionSet` ID;
  it indexes external `PlayOptionPlayTimingOffsetTableRecord.bin`, narrows the
  selected double to float, and uses zero for missing/out-of-range data. The
  table is a process-owned lazy singleton, not chart/reset-owned, and its
  values are absent from the workspace.
- Each ordinary gameplay substep derives/appends input before updating notes.
  Catch-up starts at `clamp(stored - current + 1, -1, 0)`, so even large lag
  produces only offsets -1 and zero at 0.5 scalar-unit spacing. Stable alternate
  dispatch bypasses this loop and updates input and notes once.
- TAP classification uses five center-first, lower-inclusive/upper-exclusive
  intervals and an early/late pivot. Its internal tables and detailed anonymous
  code conversion are reconstructed and covered by focused tests.
- After candidate reduction and before TAP-derived input classification, every
  enabled lane independently widens a center endpoint to the corresponding
  inner endpoint when the inner/middle difference is strictly below executable-
  owned epsilon `0.00001F`. Candidate exposure is unaffected because it reads
  only the outermost bounds.
- Successful-load postprocessing compares nearby chronologically ordered
  records with overlapping lane spans and stores per-lane preceding/following
  distances. Shared TAP/HOLD checker initialization uses those distances to
  tighten early/late interval sides within external caps; adjusted aggregate
  eligibility bounds are recomputed before gameplay.
- A loaded skill profile's first `SkillChangeJudgeResultData` control can
  demote a provisional tier to tier 0 before detailed-code conversion. Later
  controls are ignored. The rule is bounded by profile load/reset and does not
  consult the temporary-effect lifetime predicate used by other skill
  consumers. The runtime result-table count and thresholds remain explicit
  external inputs.
- Equal selected TAPs can independently observe the same unconsumed rising
  edge. A completed TAP requests terminal state and remains current-state 1 for
  the rest of that manager pass; if still timing-eligible, it participates in
  one following candidate reduction before state commit and removal. Its
  embedded checker stores the detailed result, and the TAP update rejects
  signed stored values greater than one before any repeat judgement. The
  generic note tick guard remains at its constructor-zero value on every
  resolved note callback path.
- Parsed type 1 constructs RTTI-identified `projView::HoldNote`. Its unresolved
  start reuses TAP candidate/rising-edge/expiration logic, after which start
  candidates are disabled.
- HOLD sustain tracks both physical source banks per lane. A source must have
  been observed released or admitted through the preceding snapshot's shared
  sustain marker; an admitted active source writes the marker into the current
  snapshot. One marked bank can arm both currently held banks for that lane.
- HOLD judgement tracks maximum inactive gap against four external thresholds.
  It processes at most one due checkpoint per manager substep and resets the
  retained maximum to any still-open gap after each checkpoint. Both start and
  checkpoint phases must reach 4 before terminal state is requested.
- Manager forced-result state can bypass physical start input, force HOLD
  sustain active, and supply fixed bytes or manager-wide cycling/random
  sequences in its selector implementation. Reset, selector cases, and
  cross-note cycle ownership are reconstructed. Whole-binary owner/write and
  address-escape closure proves that the exact snapshot can select only reset
  mode 0 or RTTI-identified tutorial mode 2. Thus byte 3 is the only reachable
  forced result when the separately initialized enable is active; cycling and
  RNG cases are dormant. The distinct companion field is fixed at zero and
  returns metadata value 1 whenever enabled.
- The shared result handler first converts 19 source categories into one of 14
  dispatch categories. Each dispatch record independently maps into one of five
  progress categories and one of nine aggregate categories. All fixed records
  reach both valid ranges, so every runtime-valid ordinary dispatch updates the
  authoritative count/accumulator and invokes the normal progress observer.
- Post-aggregate rule evaluation can set shared terminal-route state. Later
  same-pass notes still finalize, but their result events invoke a stored
  observer instead of updating normal result state. The default observer is a
  no-op; the only installed replacement increments a processed-event counter
  and clamps it to the precomputed count for progress categories 0 through 4. It
  has no active-note, input, aggregate, or terminal-state writes.
- Ordinary terminal summaries have two ordered producers. A configured rule
  request zeros the computed contribution and sets the primary summary only;
  only while primary remains clear can a later end threshold zero the value and
  set both summaries. Owner flags latch until reset. The suppression byte read
  by both branches is initialized zero and has no recovered nonzero writer.
- Every outer update then performs an unconditional kind-2 aggregate
  reevaluation from retained result-owner state and a zero event contribution.
  It converts current NotesManager position with single-precision
  `floor(position * 16.666666F)`, so the stored factor is observably not an
  exact inverse of `0.06F`. Kind 2 skips the event-only negative and configured
  terminal vectors, but common contribution/promotion rules and the final end
  threshold remain eligible. Periodic rule work can therefore change the
  aggregate and latch terminal routing without a new note result; because it
  follows the note pass, later dispatch or scene-exit logic observes the latch.
  Its optional source flag is a deterministic one-shot bit test/mark by
  source-order unit index, not the previously inferred random predicate.
- Contribution, promotion, negative-adjustment, configured-terminal, and
  result-remap controls all come from one selected `SkillBefore` record. A
  changed three-integer identity resets every vector before map lookup; the
  middle ID selects the record, a missing record leaves the vectors empty, and
  source unit types 0, 1, 2, 3, and 5 route in order while type 4 is ignored.
- The registered active-gameplay callback completes that outer update and an
  ordinary report attempt before checking scene exit. Result-count completion
  is enabled only for a nonzero expected total and uses unsigned
  `expected_total <= processed_total`. The ordinary controller exits on count
  completion or its selected terminal predicate; the alternate controller
  exits for nested state 3, or otherwise requires both count completion and a
  nested ready byte. An accepted exit timestamps the transition and selects
  structural state `0x10`, or `0x11` after a forced-final report. Player-facing
  identities of the selector, nested fields, and state codes remain unassigned.
- Structural states `0x10`, `0x11`, and `0x12` each continue to call the full
  outer gameplay update before their presentation/lifetime transition gates.
  Runtime notes, input-derived judgement, result routing, and periodic aggregate
  reevaluation therefore remain live after the first active-state exit. Entry
  to state `0x13` is the first post-active teardown: it destroys both runtime
  note vectors and clears input/view owners. State `0x13` and final report state
  `0x14` no longer call the outer update. A terminal-routed ordinary exit keeps
  later drain results observer-only, while an alternate nested-state-3 exit can
  still admit authoritative results before teardown.
- State-`0x13` teardown does not reset the current gameplay outcome. Its only
  result-object writes clear two dynamic configured-rule notification masks
  after their final evaluator consumer. State-`0x14` then resets the selected
  destination record and deep-copies the complete current `0x878`-byte result
  object, including per-result, ordinary aggregate, alternate controller, and
  terminal state. The last post-active update is therefore preserved rather
  than replaced by an earlier report snapshot.
- The later producer is selected by the live RTTI-identified
  `PlayOptionTrackSkipID`, not an unresolved arbitrary virtual. Zero disables
  it; ID 7 selects a current-track-derived external value, while other nonzero
  IDs map through `PlayOptionTrackSkipTable` into `ScoreRankTable`. It compares
  the aggregate metric against `(1010000U - selected_value) + 1U` with 32-bit
  unsigned arithmetic.
- Configured type-3 ordinary terminal rules evaluate only aggregate event type
  1 and keep `matched` separate from `terminal_requested`. Their five ordered
  conditions cover an event-derived value floor, a low event-count test, an
  aggregate threshold, a result-band cumulative-weight test, and a mode-2
  zero-gauge test. The result-band condition can consume a rule without
  requesting termination; its unit gauge decrement occurs before the final
  zero-gauge test, so gauge value one can reach zero and terminate in the same
  evaluation.
- When three controller selector bytes OR to exactly one, terminal dispatch
  instead uses a configurable meter. Normal results aggregate before applying
  an anonymous-code-selected signed delta. Current clamps to zero/maximum and a
  zero reached by one result affects later dispatches. A second predicate form
  uses a setup-derived participant count. Modes 1, 2, and 4 restrict its
  bounded selection to the local identity; other modes can admit flagged
  non-local records with nonzero effective current. Initial setup locks the
  captured count and limit, construction/configuration reset them, and no
  result-time decrement exists in the traced binary.
- Selector 2 also owns a one-shot empty-participant setup fallback. After a
  writable zero-count capture and only while the selected alternate predicate
  remains false, it zeroes/latches the meter and sets the ordinary result byte.
  Initial capture locks the path against periodic reapplication; selector 2
  remains active until the next gameplay setup/reset, so the ordinary-byte
  write is not the currently selected result predicate in that session.
- A third `NotesManager` vector is a pre-gameplay presentation-resource
  barrier, not another active-note container. It constructs RTTI-identified
  Tap, CharaTap, Hold, Slide, Air, AirHold, AirSlide, AirLadder, AirSolid,
  Flick, and Mine view objects in that order. Fixed readiness steps plus the
  manager's step-10 floor produce a 164-call fresh preload sequence. Loading
  state cannot exit until completion; reset/destruction delete the objects,
  and their resolved virtuals do not enter input, factory, or result paths.
- The otherwise-unowned direct manager-loop helper between candidate
  preparation and primary note ticks is render-only. It clears ten current
  primitive-category counts plus a total; geometry builders repopulate them
  from vertex triples, companion fields retain peaks, and the closed reference
  set has no feedback into gameplay state.
- The outer-update call immediately after lazy materialization is also
  render-only. It filters meter-derived bar-vector index 2 through projected
  visibility, stores accepted floats in a stack-local list, and submits them to
  view-marker rendering; no clock, queue, note, input, result, or terminal
  owner is written. A MET record with either zero component is retained as the
  terminal anchor in the bar and beat vectors before generation stops; later
  MET records do not restart those vectors.
- AHD/AHX consume six data fields. Later `DEF`/`PNK` suffixes are valid ignored
  extras, not styles. Only AHX endpoints own checkpoint resource pairs; a final
  AHD still participates in the always-present AirHold path envelope.
- HeavenHold has no zero/`NON` null-resource selector. Preload covers every row
  of its ordinary/secondary resource tables and runtime lookup uses width and
  flags only. Exceptional ALD's empty generated path closes body phase, while
  external resource contents remain the boundary for an observed missing root.
- Slide's fixed-width center stream is exact base white in every mode. Its main
  stream alone changes from white to gray in mode 2.
- Sustained presentation applies keyed schedule adjustment and DCM separately
  to each root/control/end point before geometry interpolation. Hold, Slide,
  AirHold, AirSlide, AirLadder, AirSolid, and HeavenHold have all been checked;
  none reuses an entity/root-time factor for the whole body.
- The following post-update pass is RTTI-identified
  `projView::KeyBeamManager`. Its transient note-authored lane markers are
  cleared before input/manager work; afterward it samples the completed logical
  held snapshot and drives only retained beam-view state and keyed feedback
  resources. The complete owner-reference set has no path back to input
  synthesis, candidates, judgement, results, or terminal state.
- The intervening post-manager setter cluster is a one-way export to two
  scene-owned presentation resources. It copies already-authoritative manager,
  mode, and score-like values into nested resource fields and dirty/enable
  flags. Setup writes sentinels and scene exit writes zeros; the associated
  player/effect paths do not feed any gameplay owner.
- A still-later outer-update helper is a scene-local cue/report scheduler. It
  runs after the complete current gameplay pass, admits at most four triggers,
  starts fixed-pool audio/effect controllers, and retains timestamps consumed
  by teardown reporting. Its fields do not feed input, notes, candidates,
  judgement, results, or the gameplay clock. Observed cue time does participate
  in a registered post-gameplay scene-state callback, so presentation lifetime
  may depend on the cue even though gameplay outcomes do not. The exact live
  project now closes its sole outer caller, both registered callback thunks,
  constructor/initialization/report reset owners, controller clearing, and
  terminal report sink.
- HOLD's once-only end branch selects and starts an indexed feedback resource.
  It does not submit a result, alter checkpoint/start phases, or request note
  termination, so no implicit end checkpoint belongs in the reconstruction.
- Window endpoints are selected from external configuration associated with a
  `JudgeTiming.ini` load path. Values are unavailable and remain parameters.

## Active question

The codex-root static-saturation pass has exhausted its current reachable
queue. Besides the closed common primitive, resource-pool, feedback-list, and
EffectBase paths, the last non-common residual—the post-update cue scheduler—
is now reclosed in the exact live project through its sole caller, both
registered callbacks, state initialization/reset, controller clearing, and
report sink. No known binary-reachable presentation or gameplay path remains
assigned to this investigator. The required next binary step is a fresh
independent contradiction audit; codex-root cannot self-certify that gate.
Issues 15, 17, and 18 no longer block the queue; owner-only issue 16 remains
outside static work.

## Tooling handoff

- The live Ghidra MCP connection is healthy for project `chart`, program
  `game.exe`. The codex-root saturation session has released coverage/Ghidra
  ownership for a fresh independent auditor.
- The latest persistent mutation names the post-update cue scheduler,
  transition callback, and report/reset callback at `RAM:00da5bd0`,
  `00da2d30`, and `00da1d20`. It documents the sole outer caller, both callback
  registrations, retained-field initialization/reset, terminal report sink,
  and exact judgement-external/post-gameplay-lifetime boundary. Ghidra was
  saved after the mutation pass.
- Follow `docs/GHIDRA.md` and run `python3 scripts/harness.py doctor` before a
  focused canonical-renderer investigation. Reopen stage-one coverage only if
  exact-binary evidence changes verified gameplay behavior.

## Blockers

- Stage two is not complete: codex-root's static queue is exhausted, but the
  required fresh independent binary contradiction audit is not complete.
  Viewer integration, deterministic comparison coverage, robustness/offline
  packaging, and owner review also remain incomplete. Human-only issue 16 is
  deliberately excluded from the static queue.

Externally absent judgement/profile values remain parameters. Their selection,
defaults where executable-owned, lifetime, and consumers are reconstructed, so
their absence is not itself a completion blocker.

## Next handoff

Preserve the verified stage-one ledger, issue-15 corrections, issue-17
differential, issue-18 provenance classifications, and the completed
presentation saturation chain. The next binary investigator must freshly
attempt the bidirectional contradiction audit rather than extending
codex-root's confidence. Product work can meanwhile normalize remaining viewer
state, implement deterministic seek/lifecycles, build the comparison matrix,
and prepare owner review. Record only original prose and measurements; do not
commit charts, footage, or extracted assets.
