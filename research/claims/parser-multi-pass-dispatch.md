# Claim: parsed C2S lines are processed in descriptor-group passes

- ID: `claim.parser.multi-pass-dispatch`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.header`, `parser.events`, `state.ownership`
- Last reviewed: 2026-07-27

## Statement

After line loading, the chart parser clears prior chart-owned state and
processes descriptor-resolved records in ordered passes: group 0 first, a
special command-ID `0x0d` pre-pass, then a main dispatch over groups 1, 2, and
3, followed by postprocessing; parse mode 1 skips the latter passes.

## Anchors

- `game.exe @ RAM:011c7040, FUN_011c7040, chart-state reset before parsing`
- `game.exe @ RAM:011c77d0, FUN_011c77d0, load wrapper`
- `game.exe @ RAM:011c7810, FUN_011c7810, line-load and parse bridge`
- `game.exe @ RAM:011c7980, FUN_011c7980, ordered parser passes`

## Observations

- `FUN_011c7040` destroys existing `0x174`-byte records, resets their vector,
  and zeroes two bounded state ranges before new parsing.
- `FUN_011c7810` loads line records and calls `FUN_011c7980` only on its success
  path.
- `FUN_011c7980` resets state again, resolves a descriptor for each loaded line,
  and first invokes one handler only for descriptor group 0.
- Except in mode 1, it next finds command ID `0x0d` for a dedicated handler,
  then performs a main group switch: group 1 and group 2 have separate handlers;
  group 3 stores a bounded value indexed from command ID `0x2e`; group 0 and
  unknown/default groups do not take those main handlers.
- The fixed ordinary postparse tail begins by retyping Slide style-code-1 and
  ALD zero/NON exceptions to type 13, then performs schedule normalization,
  generated-record construction, sorting, and final index/statistics passes.
  In particular, the retype runs before the type-13 generated-record producer.
- The later successful-load postprocessor repeats the derived path/schedule
  work needed by the gameplay loader and ends with parser-owned and caller-
  supplied count summaries. Their lack of a gameplay consumer is closed by
  `claim.pipeline.parser-derived-summary-boundary` rather than inferred from
  their position in the tail.

## Reasoning

The reset-to-load-to-ordered-dispatch chain establishes parser staging and state
ownership. Repeated descriptor lookup connects the group values to the schema
registered by `FUN_00528b60`/`FUN_011cc1b0`.

## Alternatives and falsifiers

- Competing explanation: group switches are output/report generation rather
  than ingestion.
- Evidence that would disprove this claim: a caller reaching the record vector
  without this reset/dispatch chain, or handler analysis showing the passes
  consume already-built gameplay results rather than loaded C2S lines.

## Unknowns

- Group-3 temporary storage and its mandatory overwrite before parser return
  are closed by `claim.parser.derived-command-overwrite`.
- `claim.parser.header-default-dispatch` closes mode 1 as the 20-line metadata
  scan and closes unknown/malformed/partial-field behavior for group 0.
  `claim.parser.event-token-fallback` closes the corresponding shared event
  missing/extra/malformed-field behavior; family-specific matching remains in
  the note/event claims.
- `claim.timing.tempo-measure-schedule` closes special ID `0x0d` as the BPM
  pre-pass. `claim.timing.projection-schedule-materialization` closes the
  gameplay-relevant group-1 schedule commands. The group-2 family switch,
  association/default/error behavior, generated records, factories, and
  outcome consumers are closed by the event-family and note claims referenced
  from `parser.events`.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: normative staging and family routing in `spec/c2s.md`.
- Reconstruction code: the staged parser components in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/c2s_header_test.cpp`, `tests/derived_command_test.cpp`,
  `tests/aso_parser_test.cpp`, and the family-focused parser tests.

## Verification

The exact postparse call order was rewalked independently from the dispatcher,
and the first rewrite's type-2 predicate was traced back through its field-8
string table. The final summary producer, its dormant compatibility reader,
and the successful loader's ignored output were independently bounded.
Per-family handler algorithms are independently covered by their note/event
claims and focused tests.
