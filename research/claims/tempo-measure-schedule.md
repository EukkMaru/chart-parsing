# Claim: BPM records alone define the chart-position schedule used by gameplay and adaptive Air cadence

- ID: `claim.timing.tempo-measure-schedule`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `timing.tempo_measure`
- Last reviewed: 2026-07-21

## Statement

During gameplay parsing, `BPM` records are normalized at fixed resolution 384,
sorted, and assigned cumulative millisecond positions before ordinary commands
are parsed. This BPM vector is the sole position-to-schedule map and the source
of the BPM values used by AirHold, AirSlide, and HeavenHold adaptive sampling.
`MET` records consume that schedule to build meter/grid data but do not alter
note schedules or cadence.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, command IDs, names, and field types`
- `game.exe @ RAM:011c3990, FUN_011c3990, header defaults and fixed resolution`
- `game.exe @ RAM:011c7040, FUN_011c7040, chart/timing-container reset`
- `game.exe @ RAM:011c7980, FUN_011c7980, ordered header/BPM/ordinary passes`
- `game.exe @ RAM:011c8410, FUN_011c8410, header command dispatch`
- `game.exe @ RAM:011c8350, FUN_011c8350, BPM record parser`
- `game.exe @ RAM:011c71b0, FUN_011c71b0, position canonicalization`
- `game.exe @ RAM:011c3c60, temporary-clone recovered function, position comparator`
- `game.exe @ RAM:011baae0, FUN_011baae0, BPM sort and cumulative schedule finalization`
- `game.exe @ RAM:011c0680, FUN_011c0680, position-to-schedule lookup`
- `game.exe @ RAM:011c7d30, FUN_011c7d30, MET record dispatch`
- `game.exe @ RAM:011ca4d0, temporary-clone recovered function, ordinary position parse/schedule`
- `game.exe @ RAM:011bb0f0, FUN_011bb0f0, MET sort, zero synthesis, and grid generation`
- `game.exe @ RAM:011c0800, FUN_011c0800, postprocessing schedule refresh`
- `game.exe @ RAM:011bda60, FUN_011bda60, adaptive Air-path BPM consumers`

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
- Position-to-schedule lookup scans the finalized vector backward, selects the
  latest record whose scalar position is at most target plus `1/192`, and adds
  the target delta at that record's BPM to its cumulative value. Empty maps or
  a target before every qualifying record return zero.
- The ordinary `MET` parser runs only after BPM finalization. It canonicalizes
  and schedules each 0x18-byte record through the same lookup, then stores its
  two meter integers. The meter postprocessor sorts these records, removes
  equivalent adjacent positions, and synthesizes a position-zero record from
  `MET_DEF` when needed. It populates four scheduled grid vectors; a zero in
  either meter component stops further meter subdivision generation.
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

- The sort is not stable for comparator-equivalent BPM positions and ignores
  source sequence. The local corpus's eight duplicate BPM positions all repeat
  the same BPM, so differing-BPM duplicate precedence remains outside verified
  corpus behavior.
- No positive/finite BPM validation was found on the recovered path. Every BPM
  in the local corpus is positive. Malformed zero/negative/nonfinite BPM and
  adaptive Air with an empty map are not assigned clean-room behavior.
- Meter field player-facing names and the rendering/audio meanings of all four
  generated grid vectors are not assigned. Their lack of input to note
  scheduling and adaptive cadence is normative; presentation behavior is out
  of scope unless a later judgement dependency appears.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary project clone only. The comparator boundary
  at `RAM:011c3c60` and common record helper at `RAM:011ca4d0` exist only in
  that clone.
- Spec sections: `spec/timing.md`, `spec/c2s.md`, and the Air-family timing
  references.
- Reconstruction code: `ChartPosition`, `BpmScheduleRecord`, position
  normalization, BPM finalization, position-to-schedule, and scheduled-BPM
  selection helpers.
- Tests: `tests/tempo_map_test.cpp`.

## Verification

- Focused tests cover fixed-grid normalization, out-of-order BPM sorting,
  previous-BPM interval integration, lookup across two BPM changes, the
  before-first Air fallback, exact change boundaries, and integration with the
  adaptive sample-step rule.
- A complete local-corpus aggregate found 7,752 charts, all with resolution
  384 and a BPM record at position zero. Across 24,735 BPM records there were
  no nonpositive values. Eight duplicate BPM positions were present and every
  duplicate repeated the same value; no duplicate MET position was found.
- The header handler, BPM finalizer, generic schedule lookup, MET
  postprocessor, final schedule refresh, and repeated Air lookup sites were
  inspected independently.
