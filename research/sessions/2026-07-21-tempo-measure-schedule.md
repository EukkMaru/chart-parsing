# Session 2026-07-21: tempo/measure schedule

- Investigator: codex-root
- Ghidra writer: none; temporary analysis clone only
- Coverage rows claimed: `timing.tempo_measure`; advanced to `parser.header`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Recover BPM/measure population, canonical position conversion, cumulative
scheduling, adaptive Air tempo selection, and reset/error ownership without
expanding into presentation-only meter behavior.

## Findings

- Added `claim.timing.tempo-measure-schedule`.
- `BPM` ID 13 is parsed in a dedicated pre-pass, normalized with executable-
  fixed resolution 384, sorted, and cumulatively scheduled in milliseconds
  before MET or notes are parsed. The shared backward lookup is the sole
  position-to-schedule map.
- AirHold, AirSlide, and HeavenHold independently scan the finalized BPM vector
  by scheduled milliseconds. Their cadence reference is named header
  `PROGJUDGE_BPM`, reset to 240 and replaceable by header ID 10.
- `MET` ID 14 consumes the BPM schedule. Its postprocessor sorts/deduplicates,
  synthesizes position zero from `MET_DEF` if needed, and builds four scheduled
  meter/grid vectors. MET does not alter note schedules or adaptive cadence.
- The `RESOLUTION` descriptor has no header-handler case; reset value 384 stays
  authoritative. The local corpus uniformly agrees.

## Ghidra mutations

- Live project: none; GhidraMCP was unavailable and port 8089 was not
  listening.
- Temporary clone: used existing recovered boundaries at `RAM:011c3c60` and
  `RAM:011ca4d0`. No live symbol, comment, type, structure, or enum changed.
- Supported annotations to apply when MCP returns include the BPM record
  comparator at `RAM:011c3c60`, position-to-schedule helper at `RAM:011c0680`,
  and common parsed-position scheduler at `RAM:011ca4d0`; none were applied.

## Validation

- `python3 scripts/harness.py doctor`: binary/project/corpus/tool identity
  passed; MCP remained unavailable.
- `python3 scripts/harness.py validate`: passed, 28 coverage rows and 10
  required files.
- Full configure/build passed.
- `ctest --test-dir build --output-on-failure`: all 21 tests passed, including
  new `chart.tempo_map` cases.
- Corpus aggregate: all 7,752 charts declare resolution 384 and BPM at zero;
  all 24,735 BPM records are positive. Eight duplicate BPM positions repeat
  the same BPM, and no duplicate MET position occurs.

## Unresolved and contradictions

- Differing-BPM duplicate positions remain ambiguous because the epsilon
  comparator ignores source sequence and the sort is not stable. This case is
  absent from the local corpus.
- No positive/finite BPM validation was recovered. Generic schedule lookup has
  an empty-map zero path, but adaptive Air assumes a nonempty map. Malformed
  timing outside the corpus remains intentionally unspecified.
- The four meter/grid vectors have presentation/audio consumers whose full
  meaning is out of scope; their lack of input to note scheduling/cadence is
  established.

## Handoff

`timing.tempo_measure` is mapped and released. `parser.header` is claimed by
codex-root. Continue at descriptor initializer `RAM:00528b60`, reset
`RAM:011c3990`, handler `RAM:011c8410`, VERSION helper `RAM:011c8630`, and token
field accessors `RAM:011cf450`, `RAM:011cf6d0`, and `RAM:011cf760`. Close reset
defaults, duplicate precedence, malformed fallbacks, and ignored-header
behavior before raising confidence.
