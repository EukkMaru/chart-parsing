# Session 2026-07-21: AirHold secondary and generated path

- Investigator: codex-root
- Ghidra writer: none; temporary project clone only
- Coverage rows claimed: `pipeline.boundaries`, `judgement.types`,
  `note.other_variants`
- Binary hash checked: yes
- MCP health checked: yes; configured endpoint unavailable

## Goal

Close common `AHD`/`AHX` parsed secondary type 5 from parser attachment through
runtime ownership, input, all result streams, generated-path production,
reset/error paths, and deferred removal.

## Findings

- Added and reconstructed
  `research/claims/air-hold-secondary-judgement.md`.
- Added the normative vertical slice in `spec/notes/air_hold.md` and integrated
  parser, input, timing, matching, judgement, and configuration consequences
  into the shared specifications.
- Established that AHX alone contributes authored profile-6 checkpoints; AHD
  extends the endpoint/path without adding one.
- Closed the candidate-free `projView::AirHoldNote` runtime object, its
  retained-profile start result, profile-6/profile-7 sustain admission,
  inactive-gap generated results, authored AHX results, shared category routes,
  two-component terminal predicate, destruction, and deferred lifetime.
- Reconstructed the generated producer: 384-tick grid, tempo-adaptive cadence,
  per-segment initial suppression, disabled boundary/end records, final-AHD end
  margin, and strict key-0 exclusion intervals.
- Added clean-room AirHold parsing, producer/filter, contact, checkpoint,
  lifecycle, and result helpers plus focused tests.
- Advanced the active `note.other_variants` target to corpus-backed `ASD`/`ASC`
  secondary type 8.

## Ghidra mutations

None in the live `chart` project. GhidraMCP was not exposed and its configured
HTTP endpoint was not listening. Static analysis used the temporary clone at
`/tmp/chart-readonly-20260720`; source artifacts and the live project were not
modified.

## Validation

- Binary SHA-256 matched the workspace identity.
- `python3 scripts/harness.py validate`: passed, 28 coverage rows and 10
  required files.
- CMake build: passed.
- CTest: 16/16 passed, including expanded `chart.air_hold_judgement` coverage
  for AHX-only authorship, grid/cadence generation, structural record flags,
  AHD end filtering, exclusion intervals, input admission, gap reset behavior,
  component completion, candidate absence, and result routing.
- Independently checked parser first/continuation/diagnostic paths, corpus root
  families, factory append order, RTTI/vtable, constructor/destructor, load,
  all gameplay slots, generated-vector append helper, position/time services,
  tempo-adaptive loop, both emission filters, and terminal removal.

## Unresolved and contradictions

- Parser-owner fields `+0xbc`, `+0xc4`, `+0xc8`, and `+0xcc` have exact
  producer interfaces but unresolved player-facing names, defaults, and units.
  The reconstruction keeps them explicit.
- The tempo-map population and full measure/tempo compatibility grammar remain
  open; the AirHold producer injects their recovered lookup/conversion
  interfaces.
- External checker/gap numeric values and physical meanings of derived profiles
  6 and 7 remain unavailable or unnamed and are parameterized.
- No persistent Ghidra annotations were applied because the live write channel
  was unavailable.

## Handoff

Continue `note.other_variants` with `ASD`/`ASC`, mapped to secondary parsed type
8. Begin at their attachment branch in `RAM:011c8870`, then resolve the type-8
branch in factory `RAM:00b28cc0`, allocation, constructor, RTTI, vtable slots,
input/result consumers, reset/destruction, and root compatibility. Use the
AirHold postprocessor at `RAM:011bda60` only where type 8 shares its bounded
sampling/filter stage. Broad ownership remains with `codex-root`.
