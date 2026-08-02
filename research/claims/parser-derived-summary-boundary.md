# Claim: derived parser summaries do not feed the gameplay path

- ID: `claim.pipeline.parser-derived-summary-boundary`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.compatibility`, `state.ownership`
- Last reviewed: 2026-07-27

## Statement

The successful gameplay load computes parser-owned note-count and position
summaries and a separate caller-supplied count summary, but neither result
selects load success, runtime-note materialization, timing, input, candidates,
or judgement. Gameplay setup instead derives its record count and result-
category totals directly from the finalized parsed-record vector.

## Anchors

- `game.exe @ RAM:011c1500, FUN_011c1500, successful-load postprocessing and caller-supplied count summary`
- `game.exe @ RAM:011bd360, FUN_011bd360, note-only and all-event horizon producer`
- `game.exe @ RAM:011ba710, FUN_011ba710, all-event horizon consumer for derived BPM statistics`
- `game.exe @ RAM:011bb0f0, FUN_011bb0f0, all-event horizon consumer for meter/grid generation`
- `game.exe @ RAM:011c1ce0, FUN_011c1ce0, parser-owned count/position summary producer`
- `game.exe @ RAM:011ca700, FUN_011ca700, temporary position-record append helper`
- `game.exe @ RAM:011c2e10, FUN_011c2e10, unreferenced profile-validation consumer`
- `game.exe @ RAM:011b2f80, FUN_011b2f80, validation-error vector destruction`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, sole live postprocessor caller and ignored output lifetime`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, gameplay record-count and category-count reconstruction`
- `game.exe @ RAM:011c6160, FUN_011c6160, indexed finalized-record accessor used by gameplay setup`

## Observations

- `FUN_011c1ce0` clears parser-object storage from `+0x174` through `+0x2bc`,
  scans every finalized `0x174`-byte parsed record and its relevant authored or
  generated positions, and rebuilds per-family counts, aggregate counts,
  first/last position values, and a bounded position distribution. Its only
  temporary dynamic state is a deque of twelve-byte position records, which is
  destroyed before return. It does not mutate the parsed-record vector or any
  runtime-note, result, input, or clock owner.
- The first 45 integers in that range temporarily receive the recognized
  group-3 `T_REC_*`, `T_NOTE_*`, `T_NUM_*`, `T_CHRTYPE_*`, `T_LEN_*`, and
  `T_JUDGE_*` values during main dispatch. The summary producer's initial clear
  overwrites all 45 without reading them before any derived computation.
  Exact grammar and malformed behavior are closed by
  `claim.parser.derived-command-overwrite`.
- Earlier in the same successful postprocess tail, `FUN_011bd360` resets two
  retained position triples. The first scan selects the latest applicable
  gameplay-record position: root positions for instantaneous families,
  explicit endpoints for HOLD, the final primary path point for Slide,
  AirSolid, AirLadder, and HeavenHold, and the applicable attached AirHold or
  AirSlide endpoint. The type-12 SLA region is deliberately omitted. A
  candidate replaces the current value only when it is later by more than the
  shared `1/192` scalar-position epsilon.
- The producer copies that note-only horizon into a second triple and extends
  it across the retained BPM, MET, tree-owned, CLK, and other auxiliary event
  position collections. Thus a timing or presentation event can extend the
  all-event horizon beyond the final gameplay record without changing the
  parsed-record vector.
- The note-only triple is not read after it seeds the all-event triple. The
  latter's closed in-library reader set is `FUN_011ba710`, which uses its
  scheduled component to close the last BPM interval while deriving internal
  BPM statistics, and `FUN_011bb0f0`, which uses its position as the terminal
  extent for generated meter/grid records. Those statistics and grid records
  are already excluded from the authoritative BPM lookup, note schedules,
  adaptive Air cadence, and judgement state.
- The live `FUN_011c1500` postprocessor calls that summary producer after path,
  schedule, endpoint, tempo-statistics, meter, tree, sort, and CLK work. It then
  makes a second pass over finalized records to fill a 60-byte count object
  supplied by its caller.
- `FUN_00b2a8c0` is the sole caller of `FUN_011c1500`. It supplies a stack-local
  60-byte object and never reads that object after the call. Its subsequent
  setup reads finalized BPM/CLK state and external configuration, not either
  count summary.
- The only recovered parser-library routine that semantically reads the
  `+0x174..+0x2bc` summary is `FUN_011c2e10`. It first clears a separate
  `+0x2c0..+0x2c8` vector of 20-byte records, recomputes the summary, checks
  selected family counts against a fixed profile table, and appends validation
  records. Its sole jump thunk has no direct, thunk, or pointer caller in this
  snapshot, so it is not reached by the gameplay parser or loader.
- Parser constructors, reset/copy helpers, and destruction account for the
  other in-library accesses to this storage. A separate scan of every function
  that directly loads the process parser pointer found no external read of the
  summary range.
- The same direct process-parser scan found no external read of either horizon
  triple at `+0x144..+0x158`. Both full parser orchestrators call the horizon
  producer before its two consumers, and the gameplay loader reaches that
  sequence only after successful tokenization.
- Gameplay setup computes the finalized record count from the parsed-vector
  begin/end difference divided by `0x174`, queues every index, accesses each
  record through `FUN_011c6160`, and independently accumulates fourteen setup
  categories. Those live counts are then supplied to the result owner.

## Reasoning

The producers' write confinement, the horizon reader closure, the sole live
caller's dead stack output, and the gameplay controller's independent scan
separate retained parser metadata from authoritative gameplay setup. The
dormant profile validator explains why detailed family summaries exist without
establishing a gameplay version gate. A compatibility branch would require a
live caller or a consumer of its validation vector, neither of which is present
in the exact binary's recovered gameplay path. Likewise, making an auxiliary
event later than the final note can extend derived statistics or meter/grid
coverage, but no edge carries that change into note creation or judgement.

## Alternatives and falsifiers

- Competing explanation: one summary or horizon field is copied through an
  accessor or validation wrapper and later controls runtime-note eligibility,
  timing, or result termination.
- Evidence that would disprove this claim: a live caller of `FUN_011c2e10`, an
  external read of parser offsets `+0x174..+0x2bc`, a read of the caller's
  60-byte output after `FUN_011c1500`, or a gameplay setup branch that uses a
  retained summary rather than the finalized records, or a horizon reader
  outside derived BPM statistics and meter/grid generation.

## Unknowns

- The player-facing purpose of the dormant six-profile validation table and
  its 20-byte output records remains unassigned because the routine is
  unreferenced.
- These summaries may remain useful to unrelated tooling in other builds; the
  claim is limited to this binary snapshot and its recovered gameplay path.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary project clone only.
- Spec sections: `spec/c2s.md` derived-summary boundary.
- Reconstruction code: none; non-authoritative summaries are outside the
  clean-room gameplay core.
- Tests: none; the exclusion is established by static producer/consumer and
  lifetime closure.

## Verification

The summary writer, horizon producer and family switch, horizon readers,
temporary container, parser storage range, validation reader,
validation-vector lifetime, both postprocessor caller sets, caller-local output
lifetime, direct process-parser references, and gameplay's independent
record/category scans were audited separately. No path from these derived
values reaches gameplay-owned state.
