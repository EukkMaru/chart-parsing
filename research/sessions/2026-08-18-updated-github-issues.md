# Session 2026-08-18: updated GitHub issues and static saturation

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `audit.closure` plus reopened issue-specific rows
- Coverage ownership released: yes; `audit.closure` returned to `unknown`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Review updated GitHub issues except human-only issue 16, resolve every listed
binary question, correct the clean-room viewer/spec/tests where evidence
requires it, then continue the static saturation campaign.

## Findings

- Issue 15 AirHold, zero-meter, HeavenHold NON-resource, Slide-center-color,
  and sustained-body DCM questions are recorded in
  `claim.presentation.air-hold-model-path`,
  `claim.pipeline.meter-grid-render-boundary`,
  `claim.presentation.heaven-hold-authored-mesh`,
  `claim.note.slide-path-presentation-geometry`, and
  `claim.presentation.sustain-endpoint-dcm-projection`.
- Issue 17 exposed a real schedule-builder coverage hole: DCM entered the
  viewer's key-0 schedule in the old builder. The shipped parser now calls a
  named group-1 builder that admits only positive-duration STP/SFL/SLP keyed
  intervals, keeps DCM in source order, and excludes SFE/CLK.
- The browser audit compares that actual parser/builder output to the
  independent `scripts/c2s-schedule-reference.js` transcription. Full-corpus
  coverage is 595 group-1 charts and 19,890 records with zero mismatches; all
  three requested deliberate mutations are detected.
- All 42 issue-18 provenance items are classified in
  `research/VIEWER_PROVENANCE_AUDIT.md`: 25 direct recovered rules, eight
  product-only conventions, and nine corrected/mixed boundaries. Stale claim
  IDs were corrected and every live product choice is labeled at its use.
- Updated issue-14 comments exposed a present viewer regression: the AirHold
  root arrow call passed transformed constructor value 1 as an extent, which
  evaluates to zero and suppresses the glyph. The binary proves the root
  resource origin/visibility but not its external mesh. The viewer now draws a
  product-sized full-height substitute and labels that boundary.
- The external presentation-table follow-up closes the shared
  database-directory/registered-name/`.bin` composition plus typed loader,
  row-stride, invalid-fallback, and first-consumer chains for NotesEffect,
  NotesCharaEffect, Model, ModelSet, Texture, and FieldLineFile. Their exact
  basenames are proved; deployed rows and referenced resources remain external.
- The subsequent manager saturation rewalk found a real omitted path:
  `projView::CharaEffectManager` supplies separate lane-overlap and global-
  cooldown flags to Slide extended-feedback kinds 6 and 7. The exact 16-entry
  footprint table, one-unit per-width durations, seven-unit global duration,
  asymmetric comparison/write interval, both reset callers, destruction, and
  sole common-feedback consumer are now reconstructed and tested in
  `claim.presentation.slide-extended-feedback-admission`.
- The following owner pass closes the dedicated runtime note-model and Joint
  texture resource pool. It proves the fixed ModelSet 0..407 inclusion scan,
  checked Texture rows 0..15, lazy keyed instance reuse/release, model-load
  boundary, and complete owner lifecycle. The distinct RTTI `CacheManager`
  has no direct runtime-note or Joint initializer references and is not an
  alias for this owner.
- The next owner pass closes all of `NotesPreCalcManager`, not only AirLadder:
  the fresh-chart switch dispatches exactly types 2/9/10/13 into independent
  Slide/AirLadder/AirSolid/HeavenHold maps keyed by accepted-record identity.
  All four getters throw on a missing key, all direct consumers stay within
  their family, and the common clear owns every object/map reset.
- The Joint reverse sweep enumerates the complete base-constructor caller set,
  assigns every reachable child to Hold, Slide, AirSlide/AirLadder, AirSolid,
  or HeavenHold, and retains Field as an explicit unreachable allocation-root
  exclusion. Registration, stored-order graph traversal, handle/cardinality
  admission, dynamic batching/default submission, wrapper destruction, and
  graph unregistering teardown are closed.
- That sweep found and corrected a genuine prior transcription error. The
  shared Air-path initializer selects stream 0 topology 3 for selector 8 or 9
  and 4 otherwise. AirLadder writes literal 9 and AirSlide literal 8, so both
  reachable topology triples are `[3,3,2]`; AirLadder was previously recorded
  incorrectly as `[4,3,2]`.
- The next non-Joint sweep closes `air::Sprite` end to end. Construction and
  both resource setters, exact 3-by-3 anchoring, six-vertex position/UV order,
  scale-rotation-translation, optional UV matrix, packed color, primitive
  setup/submission, and destruction are reconstructed. The complete direct
  draw set contains only the gameplay background SpriteNode, font/ruby text,
  `air::LedObject`, and `EmoteControl`. The 16 checked presentation-pool
  wrappers are Sprite-backed handle carriers for Joint descriptors, not a
  fifth direct Sprite-rendered note path.
- `projView::System` owns the one chart-system SpriteNode, assigns an external
  render target, names it `BG SpriteNode`, submits it during graph traversal,
  and unregisters/destroys it during system teardown. WindManager, AuraScene,
  and RTTI `star::SglVTFWaterLine` direct primitives were followed to separate
  non-note owners rather than excluded by appearance.
- The subsequent constructor-root inventory enumerates all 17 direct callers
  of `DynamicPrimitiveUtil`. Every one is assigned. All four `air::Primitive`
  overload caller sets are complete at 2/5/5/5: RTTI/vtable evidence assigns
  every non-Joint caller to air GUI/debug/editor classes, and
  `projView::JointBase` is the only chart-side owner. The anonymous pair is
  created only for literal `DefaultDebugScene`; the default extended overload
  and one engine helper are explicit no-reference roots. FontManager,
  Surfride, SPK, and SGL-mask construction remain named subsystem boundaries.
- The following non-constructor pass closes both common dynamic vtables. Setup
  stores the exact layout selector, topology/mode, vertex count, flag, resolved
  external stride, 32-bit byte count, and acquired write pointer. The accessor
  returns that pointer slot; reset clears only the backend's recorded count.
  Finalization performs the exact bit latch, selector/mode mirror, submission,
  and post-call pointer clear.
- Reversing the common destructor independently reaches every constructor-root
  family plus deleting/unwind variants. Its body only restores the base vtable.
  A prior Ghidra comment implying embedded-state release was incorrect and was
  replaced; surrounding owner destructors, not the common helper, own local
  storage cleanup.
- The convenience-builder continuation exhausts all five standalone line and
  triangle helpers. Three are unreferenced; every live engine call collapses
  to a literal performance/debug overlay except the already assigned
  `star::SglVTFWaterLine` line producer. No projView/runtime-note owner appears.
- The topology-setter continuation maps all 28 calls and closes its embedded
  62-row property-table logic. Joint and Sprite are the only chart owners;
  their complete modes 2/3/4 have exact property and payload-bit outcomes.
- The resource-pool rewalk found one prior flattening: after constructing 16
  Sprite-backed wrappers, slot `i` uses TextureTable row `i` only while in
  range, then repeats the final row. A zero-row table produces `-1` for every
  slot. The six Joint initializers are the complete handle-accessor caller set;
  negative/out-of-range selectors return zero. Wrapper clear, shrink,
  destruction, and vector-cleanup paths all release the Sprite handle and
  delete the owned Sprite. The exact selector is now reconstructed and tested.
- The EffectManager indirect-hook continuation found another material
  correction. EffList capacity is used by two independent mechanisms: normal
  admission compares submissions since the previous list update, while
  non-forced append evicts the oldest active effect if occupancy is already at
  capacity. Forced preload skips eviction and cooldown restart and can exceed
  capacity. The complete twelve-slot vtable, update/removal/statistics reset,
  clear semantics, and every all-list clear/update caller are now closed.
- The child EffectBase continuation closes the next indirect layer rather than
  treating external-player lifetime as opaque. Its complete eight-slot vtable
  and all twelve state callbacks prove four current/pending phases, a one-list-
  update delay after natural instance disappearance, same-pass removal after a
  note-owned explicit stop, idempotent stop/destruction, retained visibility
  bit `0x4`, and the Slide translation-matrix setter. External payload contents
  and natural disappearance time remain inputs; the executable-owned response
  to them is exact.
- The neighboring-control continuation closes the retained Slide player's last
  live command. Construction/load arm a `-1` latch; the first phase-2 handle
  update selects loaded external entry 0 and clears it, while hidden phases do
  not consume it. Preload steps 11/12/13 apply visible+entry0,
  visible+entry1, and hidden+entry1 to every retained handle before readiness
  at step 20. The adjacent matrix-adjustment helper has zero references and is
  explicitly excluded from reachable gameplay.
- The final exact-project residual rechecks the post-update cue/report
  scheduler that had previously been proved only in a temporary clone. Its
  thunk has one outer-update call, each of its two callback thunks has one
  registration-table data reference, and constructor/initialization,
  scheduler, scene-transition, report/reset, controller-clear, and report-sink
  paths close the complete retained-field lifecycle. Cue time can delay the
  post-gameplay scene transition but cannot feed chart gameplay or judgement.
- The `render.air_slide` ledger's phrase “unresolved ASD transforms” was stale:
  the active claim, reconstruction, and focused test already close the exact
  transforms of ASD controls that are still in the unresolved gameplay state.
  The row now says “exact unresolved-ASD resource transforms.”
- Issue 16 remains deliberately excluded as the owner's human-review task.

## Ghidra mutations

After successful dry runs, added supported decompiler comments at `011c9829`,
`00c2473d`, `011bbbf4`, `00da8420`, `00c15990`, `00c15cd0`, `00c0a3d0`, and
`00b28890`. They record field consumption, AHX resource cardinality, zero-MET
anchor ordering, exact grid-vector selection, absence of a NON selector,
center-stream color, and per-endpoint DCM granularity.

The table-provenance continuation also applied supported names to the common
manager and six typed table initializers, six opcode-identical manager-path
loaders, and checked count/path/ID accessors enumerated in
`claim.configuration.external-presentation-table-provenance`. A compact plate
comment at `010e4660` records the generic directory/name/`.bin` composition.

The manager saturation continuation applied supported names and compact plate
comments at `00b30660`, `00b30750`, `00b309d0`, `00b30820`, `00b307d0`, and
`00b30970` for CharaEffectManager ownership, exact defaults, both admission
gates, reset, and destruction.

The model-resource continuation applied supported names at `00b2c140`,
`00b2c220`, `00b2d160`, `00b2ca30`, `00b2cbe0`, `00b2cae0`, `00b2cbc0`,
`00b2ce50`, `00b2d340`, `00b2d390`, `00d7ee80`, `00d7ef90`, and `00c32d20`.
Compact plate comments record population, lookup, instance lifecycle,
tracked-handle, and checked ModelSet-to-Model load boundaries. The boolean
release twin at `00d7ef30` retains its default symbol and has a supported
behavior comment because the name validator rejected a non-distinguishing
near-duplicate.

The precompute-owner continuation applied supported names at `00b1ed70`,
`00b1f870`, `00b21ea0`, `00b267f0`, `00b264e0`, `00b265e0`, `00b266e0`,
`00b23590`, `00b23470`, `00b234d0`, and `00b23530`. It also recovered,
disassembled, and named the four one-jump chart-load thunks at `0044fd5e`,
`004167e3`, `0045d8b4`, and `004285ba`, and documented the exact switch at
`00da1499` plus manager/builder/getter ownership boundaries.

The Joint-producer continuation applied supported names and compact plate
comments at `00bfebb0`, `00bffa60`, `00d7e290`, `00d7e330`, `00d7e350`,
`00d7e550`, all six wrapper constructors `00bfec90` through `00bff510`, and
all six descriptor initializers `00c033a0` through `00c03790`. The comments
record exact owner cardinality, selector-dependent topology, zero-handle
admission, Field exclusion, submission, and unregistering teardown.

The Sprite continuation applied supported names and compact plate comments at
`006db3d0`, `006db5c0`, `006dbc60`, `006dc000`, `0102e190`, `0102e300`,
`0102e3f0`, `0102ec70`, `0102edb0`, `00acd1b0`, `00acd9c0`, `00acdd90`,
`00ace220`, `00c33080`, `00c33360`, `00d33bb0`, `012b34b0`, and
`014fd9a0`. They record the common finalizer/line boundary, Sprite resource and
geometry lifecycle, gameplay background ownership, texture-wrapper role, and
the three direct non-note primitive owners.

The complete-root continuation applied supported names for
`InitializeAirGuiWindow`, `InitializeFontTextBoxObject`,
`InitializeSurfrideRenderer`, `InitializeSpkDynamicPrimitive`,
`InitializeStarSglMask`, `InitializeDefaultDebugScenePrimitiveOwners`, and
`UpdateAirModelDebugWindow`. Compact plate comments at all four
`air::Primitive` overloads, both default-debug roots, both extended utility
constructors, and the GUI/font/Surfride/SPK/SGL owner roots record complete
caller cardinality and the sole Joint exception. Ghidra was saved after both
passes.

The non-constructor continuation corrected the common destructor comment and
applied supported names/comments at `006db950`, `006db9d0`, `006c6cb0`,
`006c6e70`, `0066e630`, `0066d340`, and `0066d470`. They record exact entry
configuration, pending-pointer access, standalone collector storage, external
stride resolution, backend allocation, and count-only reset. Ghidra was saved
after the mutations.

The convenience-helper continuation added supported names/comments at
`006dbd80`, `006dbe80`, and `006dbf40`, plus a behavior/no-reference comment at
`006dbcf0`. The near-duplicate flag-clear line name was deliberately not
forced after the Ghidra validator rejected it; the default symbol remains.

The topology continuation applied supported names/comments at `006b5af0`,
`006ca9f0`, and `006caa80` for primitive submission-payload initialization,
embedded-table property application, and topology/mode selection. Ghidra was
saved after the mutations.

The resource-pool continuation applied supported names/comments at
`00b2d010`, `00c331a0`, `00c33120`, `0102f030`, `00b2d220`, and `00b2c800`,
and corrected the population comment at `00b2d160`. They record short-table
last-row repetition, zero-row invalid selection, unsigned handle admission,
Sprite resource clearing, wrapper resizing, and complete wrapper-storage
destruction. Ghidra was saved after the mutations.

The EffectManager continuation applied supported names/comments to the full
EffList vtable targets `00b19720`, `00b1a860`, `00b1d7e0`, `00b1a6c0`,
`00b1d0e0`, `00b1ce00`, `00b1ccb0`, `00b1ce20`, `00b1a6f0`, `00b1a5f0`,
`00b1a700`, and `00b1a660`; its constructor/destructor, all-list update/clear,
submission/preload functions, and both clear owners were named or documented
as supported. Ghidra was saved after the mutations.

The EffectBase continuation created and named the eleven previously undefined
state callbacks, named the complete construction/start/update/transition/
terminal/visibility/position/stop/destruction chain, and added compact comments
at the complete vtable, callback registration, state transition, list-removal,
and Slide-control anchors. Ghidra was saved after the mutations.

The neighboring-control continuation named the EffectBase resource-entry
selector, Slide presentation update, and Slide preload step at `00bfdd30`,
`00c0ee40`, and `00c0fe20`. Comments at those functions, downstream entry
resolution `01232710`, and zero-reference transform helper `00bfdb90` preserve
the exact caller inventory and external boundary. Ghidra was saved afterward.

The post-update cue continuation named `ProcessPostUpdateCueScheduler`,
`UpdatePostGameplayCueTransition`, and
`ReportAndResetPostUpdateCueState` at `00da5bd0`, `00da2d30`, and `00da1d20`.
Compact comments at those functions plus `00da9820`, `00da25b0`, `00da2630`,
`00a85f90`, `00da03e0`, and `00da3a10` preserve caller order, callback
registration, retained-field ownership/reset, the report sink, and the exact
outcome boundary. Existing unrelated constructor/outer-update comments were
merged rather than overwritten. Ghidra was saved afterward.

## Validation

- `python3 scripts/harness.py doctor` confirms the exact binary identity,
  project/database, corpus, local toolchain, and reachable MCP bridge.
- `python3 scripts/harness.py validate` passes with 28 coverage rows and all 15
  required files.
- `python3 scripts/harness.py next` now selects unowned `audit.closure` for the
  fresh independent audit. Viewer-ledger reconciliation leaves 21 binary rows
  closed, four explicitly not applicable to binary analysis, and only
  `audit.binary_saturation` partial pending that independent review.
- The full native build succeeds and all 50 CTest cases pass.
- `tests/scene_camera_presentation_test.cpp` now independently checks the
  Sprite setup tuple, defaults, exact vertex/UV order, center and bottom-right
  anchors, color replication, transform order, optional UV matrix, common
  setup field selection and 32-bit arithmetic, both finalizer flag states,
  field mirrors, pending-pointer clear, and zero/short/full/long TextureTable
  wrapper selection plus handle-index admission.
- `tests/shared_feedback_presentation_test.cpp` now distinguishes per-update
  submission budget from active occupancy and covers normal oldest-effect
  eviction, forced over-capacity append, post-update maximum/reset, and the
  clear slot's deliberately retained per-update counter. It additionally
  covers EffectBase start-to-live transition, delayed natural terminal,
  same-update explicit stop, pause, retained visibility, and translation
  matrix layout, plus the one-shot Slide entry-0 latch and exact preload
  visible/entry sequence.
- The full browser audit passes 7,752 charts with zero parse errors, unknown
  records, rejected associations, or harness errors. Its independent group-1
  differential covers 595 charts and 19,890 records with zero schedule
  mismatches; all three deliberate mutation sentinels are detected.

## Unresolved and contradictions

- AirLadder stream topology is corrected from the former `[4,3,2]` reading to
  `[3,3,2]`. The selector formula and both reachable literal producers are now
  represented in the claim, spec, reconstruction constant, and focused test.

- A zero meter component does not continue the recovered bar vector to an R
  boundary; it retains the current MET anchor and stops. A visually persistent
  field belongs to another static/external layer.
- Exceptional zero/`NON` ALD has no code-side null-resource selector. Its empty
  generated path explains body phase completion, but external resource content
  or owner-observed timing is still required to explain an absent start image.
- The viewer already queries root/control/end DCM independently; issue text
  describing a current entity-wide factor was stale against the present file.

## Handoff

The codex-root queue is statically saturated through the last non-common
cue/report scheduler, its registered callbacks, reset owners, controllers, and
report sink. Reconcile product-only viewer gaps separately; do not reopen a
binary row merely for missing external pixels or unfinished browser state.
The next binary task is the required fresh independent contradiction audit.
Do not self-certify `audit.closure` or `audit.binary_saturation` from this
session.
