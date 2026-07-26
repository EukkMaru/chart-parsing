# Session 2026-07-21: group-0 header defaults and dispatch

- Investigator: codex-root
- Ghidra writer: none; temporary analysis clone only
- Coverage rows claimed: `parser.header`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close every group-0 reset and handler write, VERSION parsing and gameplay
selection, duplicate precedence, malformed-field behavior, alternate parse
mode, and the boundary between stored metadata and gameplay consumers.

## Findings

- Added `claim.parser.header-default-dispatch`.
- Reset and handler inspection closes IDs 0 through 12, including fixed
  RESOLUTION 384, all scalar/string defaults, source-order last-wins behavior,
  and RESOLUTION's deliberate rejected/no-write case.
- Missing or empty numeric fields become zero before conversion. Nonempty
  compatible numeric fields accept a valid prefix with trailing content, but
  throw on no conversion or range error. VERSION retains raw strings and
  zeroes a whole triple unless `%d.%d.%d` makes exactly three conversions.
- VERSION is not a compatibility selector on the recovered gameplay path. The
  20-line header-only mode uses CREATOR and authored BPM_DEF field 2 for
  metadata; the full gameplay mode uses finalized records and one common
  parser/runtime path across the local resource-version range.
- Full parsing replaces internal BPM_DEF storage with derived BPM statistics
  when BPM records exist, but never creates a missing BPM record. MET_DEF is
  meter/grid-only. CLK_DEF/CLK records become scheduled click feedback whose
  update return is ignored after note processing and does not alter judgement.
- Named gameplay header consumers are now fixed: PROGJUDGE_BPM controls Air
  cadence, PROGJUDGE_AER controls the final AHD/ASC end-margin filter, and the
  positive TUTORIAL predicate enables key-zero interval exclusion for
  AirHold/AirSlide/HeavenHold.

## Ghidra mutations

- Live project: none; GhidraMCP was unavailable and port 8089 was not
  listening.
- Temporary clone: read-only investigation plus previously recovered function
  boundaries. No live name, comment, bookmark, type, structure, or enum was
  changed.
- Supported annotations to apply when MCP returns include the header reset at
  `RAM:011c3990`, group-0 dispatcher at `RAM:011c8410`, and VERSION parser at
  `RAM:011c8630`; none were applied.

## Validation

- Focused `chart.c2s_header` test passed after a full successful build.
- Tests cover every default, last-wins duplicate application, ignored
  RESOLUTION, missing and extra fields, numeric prefixes, invalid/range
  exceptions, VERSION all-or-zero triples, and TUTORIAL positivity.
- Independent downstream checks covered the header-only caller, full gameplay
  caller, BPM/MET/CLK postprocessors, Air generator, and CLK feedback consumer.
- Local-corpus aggregate: 7,752 charts, every group-0 header exactly once,
  every RESOLUTION 384, every chart with BPM at position zero, and multiple
  older VERSION/resource strings following the same exact-binary path.

## Unresolved and contradictions

- No local chart duplicates a group-0 header, so last-wins is supported by
  static source-order writes rather than a corpus compatibility case.
- An uncaught nonempty malformed numeric field leaves the successful-load
  contract; outer application exception disposition remains outside scope.
- CLK resource identity and meter-grid presentation are not reconstructed.
  Their lack of note/result-state writes is the relevant gameplay boundary.
- Live Ghidra annotations remain blocked solely by the unavailable MCP
  connection.

## Handoff

`parser.header` is mapped at high confidence and released. Its core anchors are
descriptor initialization `RAM:00528b60`, reset `RAM:011c3990`, ordered pass
`RAM:011c7980`, handler `RAM:011c8410`, VERSION helper `RAM:011c8630`, typed
accessors `RAM:011cf450`/`RAM:011cf6d0`/`RAM:011cf760`, metadata caller
`RAM:00958bb0`, gameplay caller `RAM:00b2a8c0`, and Air consumer
`RAM:011bda60`. Select the next target with the harness; event malformed-field
closure is the nearest parser continuation if no higher-value target supersedes
it.
