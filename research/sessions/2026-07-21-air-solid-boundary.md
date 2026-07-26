# Session 2026-07-21: AirSolid parser and nonjudgement boundary

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `parser.events`; advanced to `pipeline.boundaries`
- Binary hash checked: yes
- MCP health checked: yes; GhidraMCP unavailable/not listening

## Goal

Close the last untraced root event family by recovering absent-corpus `ASO`
from its group-2 handler through runtime construction, configuration ownership,
scheduled/indirect calls, candidate participation, result routing, terminal
state, and destruction. Keep field meanings structural where the binary does
not establish semantics.

## Findings

- Added `research/claims/air-solid-nonjudgement.md`.
- ASO's twelve fields, integer-tenth conversion, exact 16-entry style table,
  independent endpoint mirroring, and continuation predicate are reconstructed.
- Parsed type 10 constructs RTTI-identified `projView::AirSolidNote`. Scene
  setup builds its projected configuration under accepted source-sequence
  order, and runtime loading retrieves that configuration by the same key.
- The scheduled type-specific callback is presentation-only. Candidate
  collection writes all negative sentinels; no ASO-reachable method queries
  gameplay input, submits a result, or requests terminal state.
- A stored-end comparison returns a boolean, but the recovered gameplay-manager
  caller discards it. No automatic lifetime claim was inferred from that
  comparison. Scene teardown remains the closed deletion path.
- The local corpus contains no ASO records, so compatibility checks are
  exact-binary-derived and synthetic rather than corpus-backed.

## Ghidra mutations

None. GhidraMCP was not exposed and its configured endpoint was not listening.
All inspection occurred statically in the temporary project clone. Temporary
clone thunks/functions and helper scripts are not live-project annotations.

## Validation

- Added `C2sAsoEndpoint`, `C2sAsoSegment`, `parse_c2s_aso_record`, exact
  color/style lookup, tenths conversion, and `c2s_aso_segments_connect`.
- Added `tests/aso_parser_test.cpp` and CTest target `chart.aso_parser`.
- Full configure/build succeeded.
- All 24 CTest tests passed.
- `python3 scripts/harness.py validate` passed with 28 coverage rows and 10
  required files.
- `python3 scripts/harness.py next` selected the later independent
  `pipeline.boundaries` saturation/contradiction audit as the next bounded P0
  target; ownership was advanced accordingly.

## Unresolved and contradictions

- The four anonymous ASO property meanings and valid authored domains remain
  unknown; code/spec names are deliberately structural.
- Non-finite and out-of-range float-to-integer conversion is not reconstructed.
- Presentation resource identities are out of gameplay scope.
- The end-comparison virtual has no observed effect at its gameplay-manager
  caller. A future additional consumer would require reopening the lifecycle
  portion of the claim.

Subsequent independent vtable enumeration corrected the caller wording: the
manager invokes the shared no-op at AirSolid vtable offset `+0x34`; the pure
end-position comparison is the separate `+0x38` override and has no recovered
gameplay caller. The no-judgement/no-terminal conclusion is unchanged. The
claim and clean-room spec carry the explicit revision.

## Handoff

`parser.events` is released at mapped/high. ASO resumes at handler
`RAM:011c8870`, continuation comparison `RAM:011b4e80`, style lookup
`RAM:011cfce0`, sample builder `RAM:011c6a50`, configuration build/insertion
`RAM:00b236a0`/`RAM:00b264e0`, factory `RAM:00b28cc0`, RTTI/vtable
`RAM:01c324f8`/`RAM:018d945c`, load/update `RAM:00c16fa0`/`RAM:00c17040`,
candidate output `RAM:00c1b170`, and manager caller `RAM:00b2b690`.

Continue the claimed `pipeline.boundaries` row. Build an explicit edge ledger
from chart-load entry through parser passes, setup/factory dispatch, scheduled
callbacks, candidate/input/result ordering, terminal requests/removal, and
scene teardown. Use completed vertical claims as cross-checks, but reopen any
edge whose producer, consumer, negative path, or indirect target lacks durable
ownership.
