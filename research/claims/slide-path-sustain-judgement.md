# Claim: SlideNote combines a TAP start with path-gap checkpoints

- ID: `claim.note.slide-path-sustain-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`, `input.logical_state`, `input.buffering`, `matching.candidates`, `judgement.types`, `judgement.windows`, `judgement.miss`, `note.slide`, `state.ownership`, `config.external`, `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-27

## Statement

Parsed type 2 ordinarily constructs a `SlideNote` whose shared TAP start
resolves once, while a separately generated path arms two-bank lane sources,
feeds the shared inactive-gap tracker only from center-window contact, consumes
at most one due checkpoint per gameplay substep, and reaches deferred terminal
state only when both start and checkpoint components equal phase 4. Exact
field-8 style `HLD` is the pre-generation exception specified by
`claim.note.slide-hld-heaven-retyping`.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, type-2 command parsing and control-point continuation`
- `game.exe @ RAM:011caf70, FUN_011caf70, 0x24-byte control-point append`
- `game.exe @ RAM:00da1492, FUN_00da06c0 dispatch tail, parsed-type runtime-builder switch`
- `game.exe @ RAM:00b267f0, FUN_00b267f0, type-2 path-container allocation and keyed insertion`
- `game.exe @ RAM:00b25510, FUN_00b25510, parsed control-point to runtime path/window construction`
- `game.exe @ RAM:00b1f0f0, FUN_00b1f0f0, complete 0x78-byte generated-record copy`
- `game.exe @ RAM:00b23590, FUN_00b23590, runtime path-container lookup`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, type-2 SlideNote factory case`
- `game.exe @ RAM:00c0c720, FUN_00c0c720, SlideNote constructor and component resets`
- `game.exe @ RAM:018d92d4, SlideNote primary vtable`
- `game.exe @ RAM:01c323bc, SlideNote RTTI type descriptor`
- `game.exe @ RAM:00c10720, FUN_00c10720, parsed-record load and checker/path/presentation initialization`
- `game.exe @ RAM:00c0dae0, FUN_00c0dae0, candidate clearing and start-candidate construction`
- `game.exe @ RAM:00c0ea40, FUN_00c0ea40, shared TAP start gate and start result`
- `game.exe @ RAM:00c0de10, FUN_00c0de10, path windows, source continuation, gap update, and checkpoints`
- `game.exe @ RAM:00c1b800, FUN_00c1b800, generated-marker feedback-only consumer`
- `game.exe @ RAM:00c0fdc0, FUN_00c0fdc0, two-component terminal predicate`
- `game.exe @ RAM:00c10ed0, FUN_00c10ed0, start/path/finalization update order`
- `game.exe @ RAM:00c0cb80, FUN_00c0cb80, path-resource and owned-vector destruction`
- `game.exe @ RAM:00c1c130, FUN_00c1c130, deferred terminal request`

## Observations

- `SLD`, `SXD`, `SLC`, and `SXC` resolve to parsed type 2 and pass a six-field
  validation path. Their handler builds `0x24`-byte control-point entries. A
  command continues an existing record only when its connection field, last
  point lane, width, and chart position match; otherwise it appends a new
  `0x174`-byte parsed record. Mirroring transforms the lane origin before the
  comparison. The four commands set two additional fields in the fixed pairs
  `SLD=(0,1)`, `SXD=(1,1)`, `SLC=(0,0)`, and `SXC=(1,0)`; their player-facing
  meanings are not assigned.
- After chain construction, style code 1/exact field-8 `HLD` changes the record
  to type 13 before this type-2 builder runs. Those records construct
  `HeavenHoldNote`; all observations below apply to records that remain type 2.
- Parsed-type postprocessing dispatches type 2 to a dedicated builder. It
  allocates a shared `0x78`-byte path container, converts the parsed
  `+0x44..+0x48` control-point vector into generated `0x78`-byte path records
  with per-lane `0x14`-byte windows, and inserts the container into a map keyed
  by parsed field `+0x84`. Lookup is checked and takes the out-of-range path if
  the key is absent; the note does not silently construct a replacement.
- The builder selects one five-float runtime-loaded profile by
  `16 - clamp(width, 1, 16)` from the active configuration object's block at
  `+0x730`. The first float produces `(profile_value - lane) * 0.5`; the
  remaining four are endpoint offsets. A separately selected runtime
  correction is added to every endpoint. The values themselves are not
  embedded in this executable and remain explicit reconstruction inputs.
- The path container's global window is enabled. Its first two endpoints are
  the root scheduled position plus the root profile's first two offsets and
  the correction. Its final two endpoints use the saved end position and the
  final control's last two profile offsets and the same correction.
- Every adjacent root/control or control/control pair creates one generated
  segment. Its global window takes the first point's first two endpoints and
  the second point's last two. A logical lane receives a nested window exactly
  when it lies in the swept half-open corridor: it is at or beyond either
  point's lane origin and before either point's lane-plus-bounded-width end.
  This deliberately fills lanes between disjoint endpoint spans.
- A lane inside both endpoint spans keeps the segment-global endpoints. A lane
  outside the start span interpolates the first two endpoints; a lane outside
  the end span interpolates the last two. The builder uses lane center
  `lane + 0.5`, the two profile-derived lane anchors, absolute distances, and
  normalized distance weights to blend scheduled positions and matching
  profile offsets.
- The parser's second command-form field is one for `SLD`/`SXD` and zero for
  `SLC`/`SXC`. Generated segment `+0x34` carries the preceding boundary marker
  and `+0x35` the ending marker. The root boundary is forced to one, and the
  final ending marker is forced to one regardless of the last authored form;
  nonfinal boundaries retain the authored marker. The separate final-segment
  byte at `+0x20` is set only on the last generated record.
- The runtime factory allocates `0x294` bytes for type 2 and calls the class
  constructor. RTTI names the class `projView::SlideNote`. Construction sets
  start phase `+0xe8` and path phase `+0xec` to zero; no other gameplay update
  function writes either component outside the state paths cited above.
- Load copies parsed `+0x84` into the note's path key, copies a vector of
  `0x20`-byte checkpoint records, initializes the shared TAP checker, clears
  both 16-lane/two-bank source arrays, and initializes the same four-threshold
  gap tracker used by HOLD. The absolute adjusted start is scheduled start plus
  the selected runtime correction. Gap update gate, floor, end, and threshold
  selection follow the already reconstructed type-2 branch of the shared gap
  initializer.
- Candidate preparation clears all 16 outputs. Until start phase 4 it invokes
  the shared TAP candidate builder; after phase 4 it leaves every output at the
  sentinel. The start update invokes the ordinary selected-candidate/rising-edge
  gate. A successful or forced start submits one shared TAP-family result with
  source category 0 or 1, performs feedback, and sets start phase 4 without
  requesting root termination.
- The path container has its own enable byte and absolute early gate. Each
  enabled path record and each nested lane window uses four ordered endpoints:
  before the first is phase 1, first-to-second is phase 3, the closed
  second-to-third interval is phase 5, greater-than-third through the inclusive
  fourth is phase 4, and greater than the fourth is phase 2. Disabled is phase
  0. Only records in phases 3, 4, and 5 contribute. Overlap reduces each lane
  by the maximum numeric phase, so center 5 dominates late 4 and early 3.
- A lane in phase 3, 4, or 5 uses the exact HOLD two-bank continuation rule.
  Phase 5 alone can set active bytes and write current sustain markers. Phases
  3 and 4 preserve arming but clear activity; phases 0, 1, and 2 clear both
  arming and activity. Activity from any center lane feeds the gap tracker.
  Forced mode independently supplies true activity.
- The path update calls the gap tracker with current time relative to adjusted
  start. Before absolute adjusted start, path phase is zero. At or after that
  start, current gap index 0 selects path phase 2 while any other index,
  including -1 before the external gap gate, selects phase 3 as long as
  checkpoints remain.
- Only the front checkpoint is tested. Its scaled time plus runtime correction
  is due on an inclusive `<= current` comparison. A due entry selects ordinary
  output from the retained maximum gap, or the shared forced-result selector,
  then passes through active-result control. Type fields 2, 3, and 4 select
  source categories 4, 5, and 6; other types supply -1. Its emission flag gates
  the result wrapper, but gap reset and removal still occur when the flag is
  clear. Exactly one `0x20` entry is removed per call.
- Empty checkpoints set path phase 4 immediately after adjusted start; path
  records do not add an implicit shared result or terminal checkpoint.
  When a due generated record has its end marker or final-segment byte set,
  it reads the ordinary/forced gap grade once and forwards category 2 only to
  `FUN_00c1b800`. That consumer performs bounded feedback/resource lookup and
  never calls the shared result dispatcher, active-result remap, note-result
  wrapper, or gap reset. Each record's consumed byte prevents repeat feedback.
  Generated markers therefore affect feedback timing but not judgement output
  or the authored checkpoint queue. Their separate persistent endpoint-
  resource ownership and visibility are specified by
  `claim.note.slide-presentation-classes`.
  The terminal predicate requires both start phase 4 and path phase 4. The main
  wrapper runs start first, path second, then requests base state 2 through the
  shared deferred finalizer. This ordering also means a same-substep start
  result can activate the terminal observer route before a due path checkpoint.
- Destruction and cleanup release note-owned checkpoint/source/presentation
  vectors and disable resources referenced through the shared keyed path
  container. The path container itself is manager-owned rather than copied into
  the note.

## Reasoning

The parser continuation predicate, keyed preprocessing insertion, and checked
runtime lookup close ownership from chart control points to the path state read
by `SlideNote`. Independent writes to the two component phases, combined with
the wrapper order and terminal predicate, establish a TAP-like start plus a
non-candidate sustain/checkpoint component rather than a monolithic judgement.
The exact window comparisons and source-array writes establish center contact,
not merely path proximity, as the active input to inactive-gap grading.

## Alternatives and falsifiers

- Competing explanation: every slide point creates an independent runtime note
  or every active path-window phase counts as held sustain.
- Evidence that would disprove this claim: a parser continuation that appends a
  second parsed record, a phase-3/4 lane setting an active byte, more than one
  checkpoint erase in a single update, or terminal request while either
  component phase differs from 4.

## Unknowns

- Player-facing names for the root command-form bit, the `HLD` rewrite's
  code-10 discriminator, result bytes, source categories, and phases 2/3
  remain unresolved. The second command-form bit's generated-boundary and
  persistent endpoint-resource consumers are now closed by
  `claim.note.slide-presentation-classes`.
- The numerical contents, units, defaults, and player-facing schema of the
  runtime-loaded `+0x730` endpoint-profile block remain unavailable. Its exact
  selection and use are closed and parameterized.
- Generated-boundary feedback resource identity remains excluded, but its
  selector, one-shot lifetime, persistent endpoint ownership, result-index
  lifetime, and lack of shared judgement or gap-state mutation are closed by
  `claim.note.slide-presentation-classes`.
- The physical labels of the two source banks remain unassigned, but their
  complete snapshot synthesis and sustain-marker ownership are closed by
  `claim.input.snapshot-profile-synthesis` and
  `claim.input.hold-source-continuation`.
- A missing parsed path key calls the standard out-of-range routine. The
  clean-room checked lookup raises `std::out_of_range`; no fallback path is
  invented.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/c2s.md`, `spec/input.md`, `spec/matching.md`,
  `spec/judgement.md`, `spec/configuration.md`, `spec/notes/slide.md`.
- Reconstruction code: `SlideEndpointProfile`, `SlideGeneratedPath`,
  `require_slide_generated_path`, `build_slide_generated_path`,
  `SlideWindowPhase`, `classify_slide_window`, `update_slide_lane_sources`,
  `SlideCheckpointProgress`, `update_slide_checkpoints`, and related slide
  helpers in `include/chart/reconstruction.hpp`.
- Tests: `tests/slide_path_test.cpp`.

## Verification

Parser construction, type postprocessing, keyed lookup, complete generated
builder/copy, factory/RTTI, load, candidate virtual, start update, path update,
feedback-only marker consumer, shared gap helpers, result wrapper, component
predicate, deferred finalizer, and destructor were traced independently.
Function hashes include `00b25510 =
a3a0ba09ade07133f23b8d78260d159511afc37d2d821c8ab595c7ef382f025d`,
`00b1f0f0 =
63c657283aca4b2d4ce0258891182556173b23cfffce87de08f44e24aca5bc0d`,
and `00c0de10 =
c1a647d7e4bb755159362c03cbaaa2002ef443d71f6d0b911ecc80089bd7a3cc`.
Focused tests cover profile selection, global and swept-lane endpoint
construction, marker propagation/final override, window boundaries, overlap
priority, source arming/activity, forced activity, category mapping, one-front
consumption, emission-independent reset/removal, component phases, and terminal
conjunction.
