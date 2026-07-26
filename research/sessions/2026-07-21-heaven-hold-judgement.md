# Session 2026-07-21: HeavenHold and legacy ALD closure

- Investigator: codex-root
- Ghidra writer: none; GhidraMCP unavailable
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`,
  `note.other_variants`
- Binary hash checked: yes
- MCP health checked: yes; no client tools and no listener at the configured endpoint

## Goal

Close the selector-zero/style-`NON` ALD runtime, the shared parsed-type-13
HHD/HHX path, generated-record ownership, candidates, input, judgement,
completion, lifetime, and legacy one-short ALD compatibility.

## Findings

- Added `claim.note.heaven-hold-judgement`: HHD/HHX and exceptional ALD share
  RTTI-identified HeavenHoldNote, with a TAP-style start and separate two-bank
  inactive-gap path.
- Parsed-record construction clears both generated vectors. The postprocessor
  fills primary vector `+0x158` for type 13 but bypasses type 9, establishing
  exceptional ALD's intentionally empty queue.
- Type 13 uses adaptive 384-tick root-to-final-end samples: kind 0 / source 2
  for interior records and enabled kind 1 / source 3 at the endpoint. Ordinary
  due-record reset precedes the emission gate.
- HHD and exceptional ALD select start profile pair 0/1; HHX selects 2/3.
  Start result sources are the common TAP variant categories 0/1. Both
  component phases must equal 4 for terminal state.
- The once-only adjusted-end branch reaches indexed feedback only and does not
  submit a result or finish a phase.
- Missing legacy ALD style returns the tokenizer's empty string and style code
  0. It cannot satisfy code 15/`NON` and remains AirLadder.
- Added `spec/notes/heaven_hold.md`, cross-spec updates, clean-room HeavenHold
  helpers, and a focused test.

## Ghidra mutations

None. The live project could not be reached through GhidraMCP. Static analysis
continued in `/tmp/chart-readonly-20260720`; no `chart.rep` workspace file was
edited.

## Validation

- Binary identity remained the expected SHA-256
  `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`.
- `python3 scripts/harness.py validate` passed before durable coverage update.
- CMake configure/build passed.
- All 19 CTest targets passed, including
  `chart.heaven_hold_judgement`.
- Constructor/vector initialization, every primary-vector producer branch,
  factory/load selection, start and path result calls, feedback-only boundary,
  terminal predicate, and destruction were checked independently.

## Unresolved and contradictions

- HHD/HHX are absent from the local corpus; binary support is closed, but no
  authored instances can contradict the supported schema locally.
- External start windows, gap thresholds, adjusted-end values, corrections,
  forced timing, input profile meanings, and resource identities remain
  parameters.
- Decreasing/malformed type-13 endpoint behavior is outside the parser's
  observed invariant and was not invented.
- No live Ghidra names, comments, types, or bookmarks could be applied.

## Handoff

`note.other_variants` is mapped and released. Resume the still-owned
`pipeline.boundaries` at the successful-load caller that invokes
`RAM:011bda60`, `RAM:011bd360`, `RAM:011ba710`, and `RAM:011bb0f0`. Trace its
false/error returns and partial cleanup into active-note construction, result
state, input history, retry/reload, and teardown. `judgement.types` is mapped
and released; revisit player-facing identities only if an exact-binary consumer
makes them gameplay-relevant.
