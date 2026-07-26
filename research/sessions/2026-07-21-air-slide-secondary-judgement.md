# Session 2026-07-21: AirSlide secondary judgement

- Investigator: codex-root
- Ghidra writer: none (MCP unavailable)
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`,
  `note.other_variants`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close the corpus-backed ASD/ASC secondary type-8 path from parser attachment
through generated/authored judgement, state reset, result routing, and deferred
lifetime, then advance the unbounded-note target.

## Findings

- Added `claim.note.air-slide-secondary-judgement` at reconstructed/high
  confidence.
- The executable command descriptor fixes ASD as marker true and ASC as false.
  ASD restarts adaptive sampling and owns authored checkers; ASC carries the
  cursor and owns none.
- AirSlide's ordinary due-record path classifies/resets retained gap state even
  when emission is disabled, a verified difference from AirHold.
- Added `spec/notes/air_slide.md`, cross-spec boundaries, clean-room helpers,
  and focused tests.

## Ghidra mutations

None. GhidraMCP was not exposed and `127.0.0.1:8089` was not listening.
Static investigation used only the temporary project clone; the live project
was not mutated.

## Validation

- Confirmed the pinned binary SHA-256 through the workspace harness.
- Aggregated the local corpus without copying chart records: 42,017 ASD and
  163,804 ASC lines, all with the descriptor's expected field count.
- Rebuilt the workspace successfully.
- All 17 CTest targets passed, including the new
  `chart.air_slide_judgement` target.
- `python3 scripts/harness.py validate` is required after the final durable
  coverage update.

## Unresolved and contradictions

- Parser-owner configuration values remain external and parameterized.
- Derived profile 6/7 physical identities and full synthesis remain open.
- An early hypothesis treated the sixth-field branch constants as family enum
  values. Direct inspection showed they are raw event-table indices: 19 is ASD
  and 20 is ASC. The durable claim/spec use the corrected interpretation.
- No Ghidra names, comments, or types could be persisted without MCP.

## Handoff

AirSlide anchors are parser `RAM:011c8870`, postprocessor `RAM:011bda60`,
factory `RAM:00b28cc0`, vtable `RAM:018d9a14`, load `RAM:00c28330`, path update
`RAM:00c261d0`, and terminal predicate `RAM:00c27d50`. Continue with `ALD`,
parsed type 9 and the largest remaining corpus-backed family. Active ownership
remains with `codex-root`.
