# Claim: SLA regions assign maximum overlapping scroll tags without entering judgement

- ID: `claim.parser.sla-region-selection`
- State: superseded
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`
- Last reviewed: 2026-07-27

## Statement

This claim is superseded by
`claim.parser.sla-materialization-selection`. Its region parser and maximum-tag
selection remain correct, but its gameplay-exclusion conclusion is false.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, SLA descriptor registration`
- `game.exe @ RAM:011c8870, FUN_011c8870, type-12 field parse and record append`
- `game.exe @ RAM:011c71b0, FUN_011c71b0, start/end position normalization`
- `game.exe @ RAM:011c1030, FUN_011c1030, integer-lane region query`
- `game.exe @ RAM:011c1330, FUN_011c1330, float-lane region query`
- `game.exe @ RAM:011c0e80, FUN_011c0e80, per-record start/end tag assignment`
- `game.exe @ RAM:00b25510, FUN_00b25510, Slide path tag propagation`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, common note tag copy to runtime state`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed-type runtime factory exclusion`
- `game.exe @ RAM:00b29760, FUN_00b29760, tag-keyed projected-position wrapper`
- `game.exe @ RAM:011c6720, FUN_011c6720, tag-keyed scroll interval integration`

## Observations

- SLA has six integer arguments after the command: major position, minor
  position, lane, width, duration, and tag. Width is clamped to 1 through 16;
  mirrored parsing replaces lane with `16 - lane - width`. The start is the
  normalized `(major, minor)` position and the end is normalized from
  `(major, minor + duration)`.
- The ordinary query adds exact float `1/192` to the queried position scalar.
  A region matches when its start is at or before that shifted value, its end
  is strictly after it, and its integer lane interval fully contains the query
  interval. Starting from zero, the scan retains only a strictly greater tag.
- The AirLadder point query uses the same shifted half-open time test and
  maximum rule. Its lane containment permits exact initialized tolerances of
  `-0.00001F` at the left boundary and `+0.00001F` at the right.
- The postprocessor skips tag assignment for SLA records themselves. For root
  records it stores tags for the start and, where applicable, the terminal
  control point. Slide and Air-family builders query tags for their generated
  or authored point structures.
- The runtime-note factory has no type-12 case and returns without allocation.
  Common runtime loading copies the root tag into an auxiliary field. The
  closed direct reference set for both region-query helpers feeds note/path
  display structures; subsequent readers pass the tag with chart positions to
  the tag-keyed scroll integration used to derive projected positions.
- Independently reconstructed candidate and judgement paths continue to read
  the unprojected scheduled field and manager gameplay clock. No SLA tag read
  occurs in candidate reduction, input gates, window/gap classification,
  result routing, terminal state, or teardown.

## Reasoning

The original analysis stopped at the per-note candidate and judgement
consumers and treated position projection as presentation-only. It failed to
connect parsed tag fields `+0x7c/+0x80` to the earlier pending-record
materialization predicate. That predicate can retain or construct the note,
which changes the first later substep in which the otherwise-correct candidate
and judgement paths can run.

## Alternatives and falsifiers

- Competing explanation: SLA creates a hidden note, or its tag selects a timing
  or judgement profile.
- Evidence that disproved this claim: `FUN_00b29c90` reads the assigned tag and
  applies `FUN_011c6720` before its far-path materialization bounds.

## Unknowns

- The player/editor-facing name of the integer tag and the presentation effect
  of its scroll map remain intentionally unnamed.
- Malformed decreasing or zero-duration SLA regions are retained but cannot
  satisfy the ordinary half-open time predicate; no corpus example requires a
  special compatibility interpretation.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis and temporary function recovery stayed in the clone.
- Superseded by: `claim.parser.sla-materialization-selection`.
- Spec sections: corrected in `spec/c2s.md` and `spec/timing.md`.
- Reconstruction code: `C2sSlaRegion`, `parse_c2s_sla_record`, and both
  `select_c2s_sla_tag` variants in `include/chart/reconstruction.hpp`.
- Tests: `tests/sla_region_test.cpp`.

## Verification

Focused synthetic tests cover field defaults, width clamps, mirroring,
position normalization, time and lane boundaries, overlap maximum selection,
nonpositive tags, float-lane tolerance, extras, and malformed numeric input.
The local corpus independently contains 1,186 SLA records across 41 charts;
all have the six-argument shape and positive tags, with values from 1 through
500. No chart data was copied into the workspace.
