# Session 2026-08-10: Mine/Flick feedback remap correction

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `render.feedback_layering`,
  `pipeline.presentation_boundary`, `judgement.types`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Freshly verify every runtime note vtable's shared feedback slots and correct
any contradiction exposed by the binary-saturation pass.

## Findings

- `claim.presentation.shared-result-feedback` now records the exact `+0x48`
  split: ten families use the common one-position wrapper, while Mine and
  Flick use opcode-identical result-remap wrappers.
- The prior Mine-only position interpretation was false. The shared bridge
  applies the already-reconstructed first-unit active result-control rule to
  the incoming byte. Mine's held-lane average belongs only to its separate
  zero-result success-effect path.
- Direct memory reads of all twelve vtables also rechecked `+0x20` through
  `+0x58`; the existing load, activation, update, candidate, terminal,
  maintenance, preload, and shared `+0x44` inventory had no other mismatch.

## Ghidra mutations

Replaced the incorrect plate comment at `00c1f8f0` and added supported plate
comments at `00c20050`, `00c1a960`, and `00b93ba0`. No symbols were renamed and
no types were applied.

## Validation

- Focused shared-feedback, Mine-presentation, and Flick-motion tests pass.
- Full CTest, harness validation, and Ghidra save: pending the continuing
  saturation pass.

## Unresolved and contradictions

- The former Mine-only position-wrapper interpretation is disproved and
  corrected in claims, specs, reconstruction code, tests, and Ghidra.
- External effect resources and final scene/material/camera output remain the
  already-labeled external boundary; no value was inferred from appearance.

## Handoff

Continue the binary-saturation ledger at the common camera/resource-graph
boundary. Determine whether the exact gameplay resource-loader path statically
selects an SbUvcFileLoader-to-MainScene link or whether only the external graph
file can supply that relationship.
