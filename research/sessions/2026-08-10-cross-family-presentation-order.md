# Session 2026-08-10: cross-family presentation order

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `audit.cross_family_visual`,
  `render.feedback_layering`, `pipeline.presentation_boundary`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Close simultaneous/compound update and composition order across every runtime
note family, shared feedback/effects, scene traversal, and pass sorting without
inventing unavailable external material or camera values.

## Findings

- `claim.presentation.cross-family-update-composition` closes the two-vector
  manager schedule, family update-before-presentation order, one post-catch-up
  effect pass, late materialization/export, shared feedback ordering, local
  Joint traversal, and the boundary where external pass keys determine final
  ordering.
- The audit found and corrected a material contradiction in the prior
  cross-family claim/spec/helper: attachments are not adjacent to their roots
  in one active vector. Roots append to the primary vector, attachments append
  to the separate secondary vector, and all primaries update before all
  secondaries.
- A deterministic synthetic matrix now covers multiple compound roots, two
  catch-up substeps, the one ten-list effect update, materialization/export
  order, equal-key stability, and unequal-key pass reordering.

## Ghidra mutations

Expanded supported plate comments at `00b28cc0`, `00b2b690`, `00da8730`, and
`00da9820` to record the two owner vectors and outer update/effect/
materialization order. No tentative symbol was renamed and no type was applied.

## Validation

- Focused build/tests: pending final run.
- Full CTest and `python3 scripts/harness.py validate`: pending final run.
- Ghidra project save: pending final run.

## Unresolved and contradictions

- The former combined root/attachment order was disproved and replaced in the
  claim, specs, reconstruction helper, and focused test.
- Concrete BasePass rows, material keys/flags, graph relationships, camera
  motion, depth/blend state, and resource pixels are external inputs. Their
  binary selection and consumption rules are closed; their deployed values
  cannot be extracted from `game.exe` and must not be guessed.
- Owner visual review remains a product acceptance gate.

## Handoff

Run the binary-saturation ledger next. Recheck all remaining `partial` viewer
rows for a genuinely untraced executable selector or caller; convert resource-
only absence into an explicit boundary rather than leaving a vague static task.
The independent contradiction audit remains separate and must not be
self-certified by this investigator.
