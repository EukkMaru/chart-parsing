# Claim: AHD/AHX attach a candidate-free AirHoldNote with three result streams

- ID: `claim.note.air-hold-secondary-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`, `input.logical_state`,
  `input.buffering`, `matching.candidates`, `judgement.types`,
  `judgement.windows`, `judgement.miss`, `note.other_variants`,
  `state.ownership`, `config.external`, `interactions.cross_note`,
  `audit.indirect_calls`
- Last reviewed: 2026-08-07

## Statement

`AHD` and `AHX` attach parsed secondary type 5 to an existing root or extend an
already attached type-5 sequence. The runtime factory appends a separate
RTTI-identified `projView::AirHoldNote` after the root. It exposes no lane
candidate and owns three judgement streams: a root-relative retained-profile
start result, postprocessed generated-path inactive-gap results, and authored
AHX retained-profile checkpoint results. Terminal state requires both the
start stream and the combined path/checkpoint stream to finish.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, parsed type-5 attachment and extension`
- `game.exe @ RAM:011bda60, FUN_011bda60, anchor-chain sampling and emission filtering`
- `game.exe @ RAM:011cb0e0, FUN_011cb0e0, 0x0c-byte anchor append`
- `game.exe @ RAM:011c71b0, FUN_011c71b0, grid tick to chart-position conversion`
- `game.exe @ RAM:011c0680, FUN_011c0680, chart-position to scheduled-value conversion`
- `game.exe @ RAM:011c7710, FUN_011c7710, key-0 open-interval exclusion lookup`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, secondary type-5 allocation and append order`
- `game.exe @ RAM:00c21020, FUN_00c21020, 0x2f0-byte construction and resets`
- `game.exe @ RAM:018d996c, AirHoldNote gameplay vtable`
- `game.exe @ RAM:01c32670, RTTI type descriptor, projView::AirHoldNote identity`
- `game.exe @ RAM:00c23f50, FUN_00c23f50, root-relative load and component construction`
- `game.exe @ RAM:00c18f10, FUN_00c18f10, root-family start-profile selection`
- `game.exe @ RAM:00c18ff0, FUN_00c18ff0, inactive-gap tracker initialization`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, all-sentinel candidate output`
- `game.exe @ RAM:00c24cd0, FUN_00c24cd0, gameplay update virtual`
- `game.exe @ RAM:00c220f0, FUN_00c220f0, start-checker resolution and category 9 result`
- `game.exe @ RAM:00c221b0, FUN_00c221b0, sustain, generated records, and AHX checkpoints`
- `game.exe @ RAM:00c197d0, FUN_00c197d0, inactive-gap update`
- `game.exe @ RAM:00c184f0, FUN_00c184f0, retained maximum-gap classification`
- `game.exe @ RAM:00c19580, FUN_00c19580, post-result maximum-gap reset`
- `game.exe @ RAM:00c1acf0, FUN_00c1acf0, retained-profile timing checker`
- `game.exe @ RAM:00c23850, FUN_00c23850, two-component terminal predicate`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared result submission and deferred terminal request`

## Observations

- The type-5 parser requires six event fields after the command. Its sixth
  token resolves through the same family-to-type map. On first attachment that
  type must match an existing root at equal current-endpoint lane, width, and
  chart position. HOLD uses its endpoint and Slide its current final path
  record,
  and the root must not already carry an in-range secondary type. Corpus and
  binary paths agree on supported roots 0, 1, 2, 4, 6, and 11. Missing matches
  take the parser diagnostic path.
- A later command whose sixth token resolves to type 5 matches the existing
  secondary at its saved endpoint. Before replacing that endpoint, the parser
  appends it to the authored-checkpoint vector only when the previous command
  was `AHX`. Runtime loading also includes the final saved endpoint only when
  the final command was `AHX`. `AHD` therefore extends the endpoint/path but
  does not by itself create an authored timing checkpoint.
- Factory secondary type 5 allocates `0x2f0` bytes, constructs AirHoldNote,
  loads it through virtual slot `+0x24`, links it from the root, appends the
  root to the primary vector, and appends the secondary to its separate vector.
  The manager drains the primary vector first. The vtable candidate slot uses
  the same all-negative-sentinel implementation as AIR.
- The start checker anchors to the root start for root types 0, 4, 6, and 11,
  root end for type 1, and the last path/control point for types 2 and 13. The
  parser leaves its direction field at reset value zero, selecting profile 0
  for roots 1/2/13, profile 2 for roots 0/4/11, and profile 4 for root 6. It
  uses AIR's retained-profile classifier. Resolution submits source category 9
  and marks the start component phase 5.
- The sustain input reads derived profiles 7 and 6. A persistent admission bit
  latches when profile 7 is absent or profile 6 is present. Current activity is
  `start_reached && admitted && profile_7`; the admission update precedes this
  test, so simultaneous profiles 6 and 7 can admit activity in one substep.
- The inactive-gap tracker uses the same update, maximum, four-threshold
  classifier, anonymous result bytes `4,3,2,1,0`, and open-gap reset as HOLD.
  Its gate, participation floor, end, and thresholds come from a separate
  external AirHold configuration interface.
- Chart postprocessing first forms an anchor chain from the root-relative start,
  the saved AHX vector, and the final saved endpoint. It measures chart
  positions on a single-precision 384-tick major-unit / 96-tick minor-unit
  grid. Each segment begins with a 384-tick sample step; while the selected
  tempo-map value is below four times the parser reference, it doubles that
  value and integer-halves the step. Only samples strictly inside the segment
  are appended. The first sample of each segment has its emission byte clear,
  later samples have it set, every segment after the first receives a disabled
  type-6 boundary record, and the chain ends with one disabled type-7 record.
  The type values are structural producer tags; the gameplay consumer uses the
  scheduled value and emission byte.
- Grid conversion uses `CVTTSS2SI`: NaN, infinity, and out-of-range values
  become `INT32_MIN`. Tick subtraction/addition wraps at 32 bits. A wrapped
  segment delta above `INT32_MAX` selects the source's very large unsigned
  generation range; a signed endpoint crossing from near `INT32_MAX` to near
  `INT32_MIN` can instead have an ordinary small wrapped delta. An adaptive
  step that reaches zero cannot advance the source cursor. The clean-room
  evaluator reports both unsafe source-loop dispositions without executing
  them.
- Postprocessing can only clear additional emission bytes. When the final
  command is AHD and the end-margin input is nonnegative, a record is disabled
  when `record_grid_tick + round(sample_step * end_margin)` reaches or passes
  the final endpoint. Grid/margin conversion uses `CVTTSS2SI` integer-
  indefinite behavior and tick addition wraps at 32 bits; NaN/negative margins
  bypass the filter. Final AHX bypasses this filter. A separate enabled key-0
  interval table disables a record only when its scheduled value is strictly
  inside an interval whose selector is zero; both interval endpoints are open.
- Gameplay considers only the generated vector front and consumes at most one
  due record per substep. An enabled ordinary record classifies the retained
  maximum gap, submits source category 10, and then resets the maximum to the
  current open gap. A disabled record is still consumed but does not emit or
  reset. Forced-result selection emits its fixed byte without the ordinary
  maximum-gap reset.
- Every authored AHX position owns an independent retained-profile checker
  initialized with external profile 6. Only the current checker is updated;
  at most one can resolve per substep. Nonfinal authored checkpoints submit
  source category 12 and the final checkpoint submits 13.
- Categories 9, 10, 12, and 13 map to aggregate-authoritative categories 3, 5,
  7, and 7 respectively when result bytes and runtime table bounds are valid.
  Existing terminal-route state sends them to the observer-only path.
- The path component becomes phase 5 only when the generated vector is empty
  and all authored checkers have resolved. AirHoldNote's terminal predicate
  additionally requires start phase 5. It then requests the same deferred base
  transition used by other notes; construction/reset owns all component state
  and destruction releases both vectors and checker resources.

## Reasoning

The parser's saved-endpoint branch and the runtime count formula independently
establish AHX-only authored checkpoints. RTTI, allocation size, full vtable,
load calls, and two-phase predicate connect that parsed structure to one
secondary runtime owner. The fixed category arguments occur only after the
corresponding checker or gap result resolves, separating the three streams.
Instruction-level call order establishes contact admission, anchor order,
adaptive sampling, both one-way emission filters, front-only consumption,
maximum-gap reset conditions, and the terminal conjunction.

## Alternatives and falsifiers

- Competing explanation: every AHD/AHX line is a standalone note or authored
  checkpoint, AirHold competes in lane-candidate reduction, or completion of
  either component alone terminates it.
- Evidence that would disprove this claim: a standalone parsed-type-5 root
  factory case, AHD endpoint insertion into the authored checker vector, a
  non-sentinel candidate, more than one generated/AHX resolution in one
  substep, or a terminal predicate that does not require both phase fields.

## Unknowns

- `claim.timing.tempo-measure-schedule` closes the shared position conversion,
  BPM schedule, and `+0xc4` `PROGJUDGE_BPM` cadence-reference ownership.
  `claim.parser.header-default-dispatch` closes `+0xc8` as
  `PROGJUDGE_AER` with reset bits `0x3f7fbe77` and `+0xcc` as the positive
  predicate of `TUTORIAL`; exclusion interval contents remain explicit.
- Type-5 durations are signed and unvalidated. The producer compares a wrapped
  offset with the wrapped segment delta as unsigned. Any delta above
  `INT32_MAX` creates the pathological large-span generation class shared with
  type 13; signed ordering alone does not determine it. The clean-room domain
  evaluator reports this and zero-step cursor nonprogress without running an
  unsafe source loop.
- External timing endpoints, AirHold offset `+0x87c`, four thresholds
  `+0x888..+0x894`, corrections, defaults, and units remain parameters.
- `claim.input.snapshot-profile-synthesis` closes profiles 6 and 7 and their
  history. External synthesis thresholds/range remain parameters; this claim's
  read order and boolean consumer remain normative.
- Player-facing names for the three result streams, fine results, and source
  categories are not assigned.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Spec sections: `spec/notes/air_hold.md`, `spec/c2s.md`, `spec/input.md`,
  `spec/matching.md`, `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `AirHold*` parsing, generated-path, contact, lifecycle,
  and result helpers in `include/chart/reconstruction.hpp` plus the shared
  `AirTimingState` and `HoldGapState` primitives.
- Tests: `tests/air_hold_judgement_test.cpp`.

## Verification

Parser first/continuation/error paths, representative corpus chains, factory
allocation/append order, RTTI, constructor/destructor, full vtable, root anchor
selection, adaptive grid sampling, all generated record tags/initial flags,
AHD end-margin filtering, key-0 interval filtering, start profile, both
derived-input reads, gap update/classify/reset, generated-vector consumer,
authored checker construction and categories, terminal predicate, deferred
transition, and shared category routing were checked independently. Focused
tests cover AHX-only counting, grid/CVTTSS2SI/cadence rules, wrapped
small-boundary and large-span segment classification, zero-step nonprogress,
segment emission flags, both filters including indefinite end-margin
conversion, all supported root profiles, admission latching,
ordinary/disabled/forced gap reset, path completion, terminal conjunction,
candidate absence, and category routing.
