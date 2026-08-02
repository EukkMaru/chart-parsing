# Research status

## Snapshot

- Phase: all 28 gameplay coverage rows are verified. Exact-binary paths include live
  command-descriptor vocabulary, parser/factory/lifecycle boundaries, all
  runtime note families, TouchSlider/photo-sensor framing, complete input
  snapshot/profile synthesis and history ownership, gameplay clock/tick
  conversion, result routing, active/post-active updates, teardown, and final
  result persistence. Every concrete blocker from the failed independent
  review was reproduced and corrected; external values remain parameters
  where the executable only selects resource fields. A fresh read-only
  post-fix review passed all twelve gates in `docs/COMPLETION.md`; its dedicated
  record is `claim.audit.independent-post-fix-closure-review`.
- Binary: `game.exe`, SHA-256 `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`
- Ghidra project: local project `chart`
- Current Ghidra writer: none
- Grand-goal status: complete for this exact snapshot at apparent tick
  granularity

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
  Widths clamp independently, mirroring uses each endpoint span, four anonymous
  floats become integer tenths with `int(value * 10 + 0.5)`, and the final
  exact style string selects a 16-entry color table. Its RTTI-identified
  `projView::AirSolidNote` builds only projected resource state, emits all
  candidate sentinels, reads no gameplay input, submits no result, and has no
  recovered terminal-state request.
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
  RTTI-identified `projView::AirLadderNote`. Every parsed control point owns a
  retained-profile-7 checker; one update visits the entire vector, so multiple
  accepted points can submit source category 18 in one substep. Terminal state
  waits for every checker and an inclusive external completion threshold.
- The ALD factory has an exact alternate-class boundary: parsed selector zero
  plus style code 15/exact string `NON` constructs `projView::HeavenHoldNote`,
  the same class used by parsed type 13. The local current-schema corpus exposes
  six ALD lines with that visible pair. Legacy one-short ALD omits the style
  token; empty-string lookup yields code 0, so it remains ordinary AirLadder.
- `HHD`/`HHX` map directly to parsed type 13. A separate ordered postparser
  branch changes any completed type-2 Slide chain with exact field-8 style
  `HLD`/code 1 to type 13 before path generation, writing discriminator code
  10 to its root and every control point. The local corpus exercises this path
  with two SXD/HLD lines in one chart.
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
  render-only. It filters one generated meter/grid vector through projected
  visibility, stores accepted floats in a stack-local list, and submits them to
  view-marker rendering; no clock, queue, note, input, result, or terminal
  owner is written.
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
  may depend on the cue even though gameplay outcomes do not.
- HOLD's once-only end branch selects and starts an indexed feedback resource.
  It does not submit a result, alter checkpoint/start phases, or request note
  termination, so no implicit end checkpoint belongs in the reconstruction.
- Window endpoints are selected from external configuration associated with a
  `JudgeTiming.ini` load path. Values are unavailable and remain parameters.

## Active question

None. The fresh independent post-fix review passed. Reopen the exact affected
coverage row only if new binary evidence exposes a conflicting edge, writer,
selector, reset path, malformed domain, corpus invariant, indirect target, or
gameplay-affecting external value that bypasses the reconstructed interfaces.

## Tooling handoff

- The live Ghidra MCP connection is healthy for project `chart`, program
  `game.exe`, and no writer ownership remains active.
- The final persistent mutation creates and renames the static initializer at
  `RAM:004b8fe0` to `InitializeSoundControllerArray` and records why its 51
  fixed wrappers and sole embedded `sound::Sound` vtable make the HOLD
  adjusted-end branch feedback/audio-only.
- Follow `docs/GHIDRA.md` and run `python3 scripts/harness.py doctor` before
  any later exact-snapshot re-audit.

## Blockers

- None under `docs/COMPLETION.md`.

Externally absent judgement/profile values remain parameters. Their selection,
defaults where executable-owned, lifetime, and consumers are reconstructed, so
their absence is not itself a completion blocker.

## Next handoff

Preserve the verified ledger and independent closure report. If the binary or
corpus changes, treat it as a new snapshot and rerun identity, parser/corpus,
entry-to-exit, indirect-call, state/reset, interaction, contradiction, and
full test gates rather than carrying this verdict across versions.
