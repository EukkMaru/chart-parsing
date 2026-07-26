# Claim: CHR is TAP gameplay with a distinct result category

- ID: `claim.note.chara-tap-result-category`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`, `matching.candidates`, `matching.priority`, `judgement.types`, `judgement.windows`, `judgement.miss`, `note.tap`, `note.other_variants`, `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-20

## Statement

The `CHR` family maps to parsed type 4 and constructs
`projView::CharaTapNote`. It inherits TAP construction, candidate priority,
rising-edge gate, timing classification, forced result, deferred terminal
lifecycle, and shared result wrapper. Its gameplay-visible specialization is
that the common TAP finalizer submits source category 1 instead of ordinary
TAP's category 0, which the shared result map converts to category 4 instead of
category 0.

## Anchors

- `game.exe @ RAM:011cc970, FUN_011cc970, CHR family to parsed type 4 map`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed type-4 factory case`
- `game.exe @ RAM:00c2b4f0, FUN_00c2b4f0, CharaTapNote construction and fixed variant flags`
- `game.exe @ RAM:018d9b30, CharaTapNote vtable`
- `game.exe @ RAM:01c326f0, CharaTapNote RTTI type descriptor`
- `game.exe @ RAM:00c1da90, FUN_00c1da90, shared TAP record loading/checker initialization`
- `game.exe @ RAM:00c1d4e0, FUN_00c1d4e0, shared TAP candidate preparation`
- `game.exe @ RAM:00c1dc30, FUN_00c1dc30, shared TAP update callback`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, shared candidate/input/window/forced gate`
- `game.exe @ RAM:00c1d510, FUN_00c1d510, variant-flag source-category selection`
- `game.exe @ RAM:00c1cce0, FUN_00c1cce0, shared result wrapper`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, source-category mapping and result dispatch`
- `game.exe @ RAM:00c2b540, FUN_00c2b540, CharaTapNote destruction`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, common active-note lifecycle and removal`

## Observations

- The initialized event map assigns `CHR` to type 4. Factory case 4 allocates
  the same `0x1c8` size as TAP and invokes a constructor whose RTTI identifies
  `projView::CharaTapNote`.
- That constructor first invokes the `TapNote` constructor, replaces the vtable,
  sets variant byte `+0x79` to 1, suppression byte `+0x7b` to 0, and initializes
  its variant resource field. Shared loading preserves the effective `1/0`
  variant/suppression values for type 4.
- CharaTapNote and TapNote vtables have identical targets at the record-load,
  candidate preparation, candidate adjustment, note update, terminal predicate,
  base result owner, result wrapper, and common cleanup-control slots. The
  differing slots are destructor/resource preparation callbacks; their traced
  bodies allocate, select, stop, or destroy feedback/presentation resources and
  do not alter candidates, checker results, input, result bytes, note phase, or
  manager state.
- The shared load uses decoded width, lane extent, scheduled time, and checker
  initialization. Type 4 also participates in successful-load adjacent-record
  postprocessing, so externally selected type-4 groups can change parameterized
  endpoints while the runtime endpoint algorithm remains the TAP algorithm.
- The common gate applies the same manager-selected candidate equality, logical
  rising edge, adjusted fine-window classification, active result control,
  expiry, and forced-result branches. It stores the same detailed anonymous
  result code used by ordinary TAP.
- On completion, `FUN_00c1d510` selects source category 1 exactly when variant
  byte `+0x79` is nonzero and suppression byte `+0x7b` is zero; otherwise it
  selects 0. CharaTapNote's fixed flags select 1. Ordinary TapNote selects 0.
- The shared category table maps source 1 to category 4 and source 0 to category
  0. Both satisfy the normal aggregate and result-observer category bounds, but
  they occupy distinct authoritative category buckets and can therefore affect
  category-specific terminal rules or progress counts differently.
- Common terminal request, following-pass candidate exposure, state commit,
  and manager removal are unchanged. Construction/load is the only reset to a
  fresh runtime object; no variant callback returns a terminal object to active
  state.

## Reasoning

The family table, type switch, RTTI, base-constructor call, slot-by-slot vtable
comparison, shared gameplay callees, fixed flags, and downstream category map
close the path. The only differing value entering gameplay result ownership is
the source category; the resource-only overrides do not feed the recovered
judgement or manager state.

## Alternatives and falsifiers

- Competing explanation: CHR has a hidden input or timing rule despite sharing
  TapNote code.
- Evidence that would disprove this claim: a CharaTapNote override on a manager-
  invoked gameplay slot, a variant resource callback writing checker/result
  state, or a later write changing the fixed variant/suppression pair before
  finalization.

## Unknowns

- Player-facing meaning of the distinct mapped category and category-specific
  externally loaded terminal rules remains unresolved.
- Exact type-4 timing/adjacency configuration values are absent and remain
  parameters.
- Resource identities selected by the CHR-only callbacks are outside scope
  because no traced dependency changes gameplay generation or judgement.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/notes/tap.md`, `spec/c2s.md`, `spec/matching.md`,
  `spec/judgement.md`.
- Reconstruction code: `tap_variant_source_category` in
  `include/chart/reconstruction.hpp`; all other behavior reuses TAP helpers.
- Tests: `tests/shared_result_test.cpp` plus the existing TAP suites.

## Verification

Constructor inheritance, RTTI, all vtable entries, differing override bodies,
shared load/candidate/update/finalizer functions, fixed flag writes/reads,
category conversion, common terminal predicate, and manager removal were
checked independently. The category helper is composed with the already tested
shared map.
