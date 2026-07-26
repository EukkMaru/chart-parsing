# Session 2026-07-20: CHR TAP-equivalence and result category

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`, `note.other_variants`
- Binary hash checked: yes
- MCP health checked: yes; endpoint unavailable

## Goal

Close parsed type 4 (`CHR`) through construction, gameplay virtual dispatch,
result submission, and lifecycle, distinguishing real gameplay differences
from resource-only subclass behavior.

## Findings

- `research/claims/chara-tap-result-category.md`

Type 4 constructs RTTI-identified `projView::CharaTapNote`, a TapNote-derived
object with identical gameplay slots. Its fixed variant flags change the common
finalizer's source category from ordinary TAP's 0 to 1, which the shared map
converts from category 0 to category 4. The remaining subclass overrides are
resource preparation/destruction and do not feed recovered judgement state.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Missing vtable thunk
creation and decompilation occurred only in `/tmp/chart-readonly-20260720`, a
disposable clone of the project; source artifacts were not changed.

## Validation

- Family map, type-4 factory, constructor inheritance, RTTI, every vtable slot,
  differing override bodies, common TAP gate/finalizer, category conversion,
  terminal predicate, and manager cleanup were checked independently.
- The category selector is covered in `tests/shared_result_test.cpp`; all other
  gameplay uses the existing TAP reconstruction/tests.
- CMake build and CTest are pending the final durable update.
- `harness.py validate` is pending the final durable update.

## Unresolved and contradictions

- The initial hypothesis of total TAP gameplay identity was too broad: the
  common finalizer reads the fixed subclass variant flags and selects a distinct
  result category. Durable artifacts now preserve that difference.
- Player-facing category identity and category-specific external terminal rules
  remain unresolved.
- Type-4 timing and adjacent-record values are externally loaded and remain
  parameters.
- GhidraMCP is not exposed and its configured HTTP endpoint is not listening,
  so justified names/comments could not be persisted to the live project.

## Handoff

Use family/type lookup `RAM:011cc970`, factory `RAM:00b28cc0`, CharaTapNote
constructor `RAM:00c2b4f0`, vtable `RAM:018d9b30`, RTTI descriptor
`RAM:01c326f0`, shared load `RAM:00c1da90`, gate `RAM:00c1af30`, finalizer
`RAM:00c1d510`, result wrapper `RAM:00c1cce0`, and shared owner
`RAM:00c1c340`. Next, take parsed type 11 (`MNE`) at constructor
`RAM:00c1ec90`. Active ownership remains with codex-root for
`pipeline.boundaries`, `judgement.types`, `note.tap`, `note.hold`, and advanced
`note.other_variants`.
