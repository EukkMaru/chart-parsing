# Claim: BPM records alone define the chart-position schedule used by gameplay and adaptive Air cadence

- ID: `claim.timing.tempo-measure-schedule`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `timing.tempo_measure`
- Last reviewed: 2026-08-07

## Statement

During gameplay parsing, `BPM` records are normalized at fixed resolution 384,
sorted, and assigned cumulative millisecond positions before ordinary commands
are parsed. This BPM vector is the sole position-to-schedule map and the source
of the BPM values used by AirHold, AirSlide, and HeavenHold adaptive sampling.
`MET` records consume that schedule to build meter/grid data but do not alter
note schedules or cadence. Duplicate ordering and malformed numeric domains
follow the exact compiled sort and unguarded IEEE arithmetic described below.
The first MET integer is the beat unit and the second is the count; they
generate 384/unit beat steps and count*384/unit bar steps.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, command IDs, names, and field types`
- `game.exe @ RAM:011c3990, FUN_011c3990, header defaults and fixed resolution`
- `game.exe @ RAM:011c7040, FUN_011c7040, chart/timing-container reset`
- `game.exe @ RAM:011c7980, FUN_011c7980, ordered header/BPM/ordinary passes`
- `game.exe @ RAM:011c8410, FUN_011c8410, header command dispatch`
- `game.exe @ RAM:011c8350, FUN_011c8350, BPM record parser, hash e1b36d7183a0997a3e63e91c6e13c5979ed518b68f8cf74200334e8198485b18`
- `game.exe @ RAM:011c71b0, FUN_011c71b0, position canonicalization`
- `game.exe @ RAM:011c3c60, FUN_011c3c60, position comparator, hash 5f574ab1fc0bbcc02a71944c01fc7b4fbafecaf00967a65c9446fc61b30af6c9`
- `game.exe @ RAM:011baae0, FUN_011baae0, BPM sort and cumulative schedule finalization, hash 98564a2ac893975a545b536033f19885882fbf650ff50a902bb83a3d177d62db`
- `game.exe @ RAM:011af030, FUN_011af030, 33-record introsort cutoff and recursion budget, hash d47649a2fee90a4691fa775b299bae401bef7142b609f11668275196fb5ff77e`
- `game.exe @ RAM:011ab860, FUN_011ab860, median/equal-region partition, hash f8a1c6dfc6c03eae9a5054c046fc6561eb8a379bdea5a0cd03902e1ef31d14e0`
- `game.exe @ RAM:011a9cd0, FUN_011a9cd0, insertion-sort path, hash 85aa90703e445c22697389912bc9a3da47f218afa8e511bd5bf61cb337ca24ea`
- `game.exe @ RAM:011aa760 and RAM:011aeb30, heap fallback build/sort, hashes 6e0b23f57e8ffbeacc317145c6b6ab3efcf5e88ce3344cead1d63828e75e7243 and 3261d61d5773cd531ac6ed9e57251676002b84e8fb9f75ad61a789837d92ea34`
- `game.exe @ RAM:011c0680, FUN_011c0680, position-to-schedule lookup, hash b03981bc66f73f41eb40b26fd79cd975379ff7899174d0f1a66e8a7f4db6deb4`
- `game.exe @ RAM:011c7d30, FUN_011c7d30, MET record dispatch`
- `game.exe @ RAM:011ca4d0, temporary-clone recovered function, ordinary position parse/schedule`
- `game.exe @ RAM:011bb0f0, FUN_011bb0f0, MET sort, zero synthesis, and grid generation`
- `game.exe @ RAM:011c0800, FUN_011c0800, postprocessing schedule refresh`
- `game.exe @ RAM:011bda60, FUN_011bda60, adaptive Air-path BPM consumers and empty-map/nonpositive domains, hash 1675cdb3e095f2ec5e4ae042533f3878cf4b39bcdbc163396696c51e0d8f5a1a`

## Observations

- The descriptor table assigns ID 13 to `BPM` with integer major, integer
  minor, and float BPM fields. ID 14 is `MET` with integer major/minor and two
  integer meter fields. Header ID 8 is `RESOLUTION`, and ID 10 is
  `PROGJUDGE_BPM`.
- Chart reset initializes the position resolution at chart offset `+0xbc` to
  384 and the `PROGJUDGE_BPM` float at `+0xc4` to 240. The group-0 handler has
  cases for neighboring IDs but deliberately has no case 8, so a tokenized
  `RESOLUTION` command cannot replace the reset value in this executable. It
  does parse ID 10 into `+0xc4`.
- The gameplay parser performs a dedicated scan for every ID-13 record, then
  calls the BPM finalizer before dispatching any group-1 `MET` or group-2 note
  command. Each BPM record stores canonical major/minor floats, a cumulative
  schedule float, the accepted-record sequence, and its BPM float.
- Position canonicalization converts authored `(major, minor, resolution)` to
  the scalar `major * 4 + minor`, rounds it on the executable's internal grid,
  and splits it back into canonical major/minor. At resolution 384, an authored
  minor step advances the scalar by `4 / 384`.
- The BPM finalizer sorts positions with the comparison
  `left_scalar + 1/192 < right_scalar`. Starting at position zero, it assigns
  each record a cumulative value by adding the canonical scalar delta times
  `60000 / previous_BPM`. The first sorted BPM is also the rate for the
  zero-to-first-record interval.
- The comparator never reads source sequence. The instantiated sort uses
  insertion sort below 33 records, median selection and a three-way equal
  partition for larger ranges, a three-quarter recursion budget, and heap
  fallback when that budget is exhausted. This completely determines
  differing-BPM duplicate behavior for the snapshot without importing the
  host standard library's unspecified equivalent-element order.
- The shared float accessor returns zero for missing/empty/incompatible fields,
  accepts a converted numeric prefix, and throws on no conversion or range
  error. No post-conversion branch checks sign or finiteness, so
  `strtof`-accepted infinity and NaN enter the BPM vector.
- Position-to-schedule lookup scans the finalized vector backward, selects the
  latest record whose scalar position is at most target plus `1/192`, and adds
  the target delta at that record's BPM to its cumulative value. Empty maps or
  a target before every qualifying record return zero.
- The ordinary `MET` parser runs only after BPM finalization. It canonicalizes
  and schedules each 0x18-byte record through the same lookup, then stores its
  two meter integers. The meter postprocessor sorts these records, removes
  equivalent adjacent positions, and synthesizes a position-zero record from
  `MET_DEF` when needed. Its generated beat step is unsigned integer
  `resolution / first_meter_integer`; its generated bar step is unsigned
  integer `second_meter_integer * resolution / first_meter_integer`. At fixed
  resolution 384, `4 3` therefore produces 96-tick beats and 288-tick bars.
  Each MET position restarts the sequences. A zero in either component breaks
  further subdivision generation before division.
- The shared final schedule refresh rewrites scheduled fields for MET, note,
  path/control, and generated grid records through the BPM lookup. MET values
  are never an input to that lookup.
- Ten repeated sites in the Air path postprocessor select a BPM by scanning
  finalized BPM records backward on their cumulative scheduled value. The
  latest record at or before the query supplies record `+0x10`; before the
  first schedule the first BPM is used. That value is compared with four times
  chart `PROGJUDGE_BPM` to halve the 384-tick Air sample step.
- Reset empties the BPM, MET, and four generated meter/grid vectors before a
  new parse. No BPM record is synthesized from `BPM_DEF`: an empty BPM vector
  makes the generic position lookup return zero, while adaptive Air generation
  assumes the vector is nonempty.
- Schedule finalization and lookup use unguarded single-precision division and
  addition. Zero, negative, infinite, and NaN BPM therefore retain their IEEE
  effects. In adaptive Air selection, an empty vector reaches an invalid first-
  record dereference. If a selected nonpositive BPM initially compares below
  `PROGJUDGE_BPM * 4`, repeated doubling cannot satisfy the loop exit; NaN
  comparison instead skips the loop, as does positive infinity. Sufficiently
  low positive BPM, or a finite BPM against infinite `PROGJUDGE_BPM`, can
  integer-halve the 384-tick step to zero before the doubling comparison ends;
  the downstream generated-path cursor then does not advance.

## Reasoning

The parser's pass order and common schedule helper establish causality: BPM is
complete before MET and note records receive scheduled values, and the helper
reads no meter state. The Air producer independently reads the same finalized
BPM record layout by scheduled value and the named `PROGJUDGE_BPM` header.
This closes the previously injected tempo lookup for three note families and
separates gameplay scheduling from the meter-derived grid consumers.

## Alternatives and falsifiers

- Competing explanation: `BPM_DEF` or `MET` changes note time, Air cadence uses
  meter subdivisions, or chart `RESOLUTION` changes the executable's grid.
- Evidence that would disprove this claim: a `MET` field read in the shared
  position-to-schedule calculation, a live case-8 header write to `+0xbc`, an
  Air cadence site reading a MET/grid record instead of BPM `+0x10`, or an
  ordinary event parsed before BPM finalization.

## Unknowns

- The exact player-facing labels and complete rendering/audio meanings of all
  four generated grid vectors are not assigned. The arithmetic roles of the
  two fields and the post-materialization render-only consumer are closed.

## Consequences

- Ghidra mutations: none. The live exact-snapshot project was inspected; no
  persistent rename was justified for the instantiated standard-library sort
  helpers.
- Spec sections: `spec/timing.md`, `spec/c2s.md`, and the Air-family timing
  references.
- Reconstruction code: `ChartPosition`, `BpmScheduleRecord`,
  `snapshot_bpm_sort`, position normalization, BPM finalization,
  position-to-schedule, scheduled-BPM selection, `c2s_meter_grid_steps`, and
  malformed adaptive-step evaluators.
- Tests: `tests/tempo_map_test.cpp`.

## Verification

- Focused tests cover fixed-grid normalization, out-of-order BPM sorting,
  previous-BPM interval integration, lookup across two BPM changes,
  two-record and large equal-region duplicate behavior, the before-first Air
  fallback, empty-map disposition, exact change boundaries, and
  zero/negative/NaN/infinite adaptive-step domains, including low-positive and
  infinite-reference zero-step nonprogress.
- A complete local-corpus aggregate found 7,752 charts, all with resolution
  384 and a BPM record at position zero. Across 24,735 BPM records there were
  no nonpositive values. Eight duplicate BPM positions were present and every
  duplicate repeated the same value; no duplicate MET position was found.
- The header handler, BPM finalizer, generic schedule lookup, MET
  postprocessor, final schedule refresh, and repeated Air lookup sites were
  inspected independently.
