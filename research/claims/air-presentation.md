# Claim: Air root motion is a direction-row model plus scaled manager reference and external offsets

- ID: `claim.presentation.air-root-motion-feedback`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.air`, `render.playfield_projection`,
  `render.feedback_layering`, `render.mirroring_geometry`,
  `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

AirNote's six direction/width resource rows, signed scale, base and external
translation, manager-reference motion, nonterminal visibility, terminal
feedback selectors, reset/destruction, and preload are closed. The model's
proprietary record contents remain external, but every executable selector,
arithmetic transform, phase gate, and effect/cue protocol that consumes them is
identified.

## Anchors

- `game.exe @ RAM:00c1e730, FUN_00c1e730, attached load/model setup, hash f7aff9f14a26a21480adb1d435ecdf141f1b3c2c9f58add1e1537ac3d12d62f6`
- `game.exe @ RAM:00c1e230, FUN_00c1e230, direction/width row selector, hash a4c41a1602e25d77f175b1998a20084d1cbf1c65c64f54735f6fbc2a53028ca1`
- `game.exe @ RAM:00c1ec70, FUN_00c1ec70, result-before-presentation wrapper, hash 22d52cd71f40ed597fdb6b20ec118a0ab5df7850419e1e9ac83833115130e8d9`
- `game.exe @ RAM:00c1df20, FUN_00c1df20, nonterminal model update, hash 6151bdcb024316987dc049dc645a591c5124167686eedd960378048b3157c754`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, manager Air-reference owner/update, hash bed7892c43aec7a948d83aa2d146cbf6c46fa1ade2a95847409152f529cd84e9`
- `game.exe @ RAM:00b29740 and RAM:00b29760, reference getter and scheduled-position adjustment, hashes 5744f778fa318f2e122140a8cc1710841dbe7eca62cb8c7b885684360c3eb054 and 2ff55f6f916496a319ff5e12f632860670076e2ec5e4e041016b69b9048e4ddb`
- `game.exe @ RAM:00c1b720, FUN_00c1b720, external resource offset transform, hash 1dd4f7a884733f78ace1cf660f85765f9bd3ec50626abe8bde52d33fb71f3d42`
- `game.exe @ RAM:00b286e0 and RAM:00b28890, depth contribution and common projection, hashes f3350ec844662de70db98bbbcc77816e9ec1b591a35cd229ac579a266c2d93cb and 19696db1083c59e7e5324f7e5dcc4866796038ba33244d08ddd4b650966ceae8`
- `game.exe @ RAM:00c1de20, FUN_00c1de20, terminal producer/feedback routing, hash 8017892106b31d92543c7da50ce0ad90044c39a0f192f484c0e66c050e105a76`
- `game.exe @ RAM:00c1e360 and RAM:00c1bfa0, cue and field-feedback gates, hashes fc230be81825159080ffb08a6192b8133265ebfd966d08e8bc85330db799ba80 and 4b8c421a1eabc505a622d99703420496ad217f0500a0e055e8b2dd17e6b57f81`
- `game.exe @ RAM:00c1e350 and RAM:00c1e220, terminal predicate and maintenance, hashes 608b17889fc3af3526f5997b9280a708e52711ac2e1b9ebad5006d2b0ade1490 and a3c40be8c939d6f1000992cebd401f77d88a055049bf1644d560513b40e2bdb2`
- `game.exe @ RAM:00c1e4a0 and RAM:00c1ddb0, preload and destruction, hashes 488545f993d8bf80863b877da0d64ee33e29c0dd2efec3b4a77c792f11124483 and 6c9df8d29ef58f649d2bc88c1e4af06929151c51add4d24378e97dceda174ea8`

## Observations

- The row selector clamps `decoded_width-1` to `[0,15]`. Direction 0 selects
  rows `161..146`; directions 1/2 share `177..162`; direction 3 selects
  `193..178`; directions 4/5 share `209..194`. The preload's four unique
  families independently confirm the two mirrored aliases.
- Base x is the common lane-span center. Base y is
  `(stored_vertical-1)*3.8934999`; constructor/load ownership establishes the
  stored vertical described by the common-Air claim. Base z starts at -10000.
  Scale magnitude uses the external model native width and executable depth
  scale. Only direction 2 and 4 negate lateral scale.
- The selected external resource record contributes offsets from fields
  `+0x9c`, `+0xa0`, and `+0xa4`. They become `4*x`, `3.8934999*y`, and the
  common base-offset/positive-DCM projected-depth contribution without a
  second -65 origin. These offsets are added to the executable base position
  immediately before transform submission.
- Each manager update sets Air reference `+0x308` to
  `mapped_chart_position*0.06*external_scale(+0x14)+216000`; a disabled Air
  configuration selects scale 1. Active directions 0..2 multiply that
  reference by external `+0x18` (or 1). Directions 3..5 multiply by external
  `+0x1c` (or 1) and fixed 0.4. Subtracting the region-adjusted scheduled
  position supplies the common projected-depth delta.
- A distance/materialization probe gates writes of that scaled reference into
  optional resource-owned animation fields, but it does not gate root
  visibility or transform submission. This distinction prevents treating the
  probe as culling.
- Load configures two external model-component slots with fixed selectors and
  hides the model. Every phase other than 3 forces it visible and submits the
  transform. The active wrapper runs result production first; a newly terminal
  phase 3 therefore skips same-tick model refresh but issues no hide. Family
  maintenance finalizes/clears the model handle, and destruction releases it.
- Parsed property 0 stores one for direction class 3..5 and zero for 0..2;
  property 1 stores zero; property 2 stores one. No read of this stored field
  occurs in the closed Air active/resource/reset path, and row selection uses
  direction directly. Its asset-facing meaning is therefore not invented.
- Terminal result routing chooses source category 7 for directions 0..2 and 8
  for 3..5. A nonzero returned feedback flag gates a fixed external cue:
  selector `0x3c` for directions 0..2 and `0x134` for 3..5. Only the first
  direction class additionally invokes the shared field-feedback producer.
- Preload stages the four unique model families, then shared feedback groups 7
  and 8. Readiness is set at step 12; reset finalizes owned preload objects.

## Reasoning

The row table, signed-scale branch, common transform consumers, manager field
writer, external-record offset reader, and active model submission close every
translation/scale input. Vtable order closes result-before-presentation state
and terminal persistence. Terminal and preload paths independently account for
both direction classes and all external consumers.

## Alternatives and falsifiers

- Competing explanation: mirrored direction spellings select distinct assets.
- Evidence that would disprove this claim: nonduplicate 1/2 or 4/5 row
  families, or a later resource replacement keyed by mirrored direction.
- Competing explanation: the distance probe hides/culls Air.
- Evidence that would disprove this claim: model visibility or transform
  submission nested under the probe rather than only animation-field writes.

## Unknowns

- Model meshes/materials/native widths, per-record offsets, chart-position
  mapping contents, enabled external scales `+0x14/+0x18/+0x1c`, cue contents,
  and shared field-feedback appearance are external. Their exact source slots,
  fallbacks, and consumers are known and remain clean-room parameters.
- The parsed property's stored bit has no observed active consumer. A future
  saturation edge that finds one must reopen this claim.
- Final camera/viewport conversion and cross-family composition remain in
  their shared rows.

## Consequences

- Ghidra mutations: plate comments at `00c1e230`, `00c1e730`, `00c1df20`,
  `00c1de20`, `00c1e350`, `00c1e220`, `00c1e4a0`, `00b29740`, `00c1b720`,
  and `00c1ddb0`.
- Spec section: `spec/notes/air.md`.
- Reconstruction: `air_model_resource_row`,
  `air_stored_presentation_property_bit`, `air_manager_reference_position`,
  `air_direction_scaled_reference`, `air_model_projection_delta`, the
  `air_external_resource_*` helpers, and terminal selector/gate helpers.
- Tests: `tests/air_presentation_test.cpp`.

## Verification

All six row-table families and all sixteen entries, attached load, model
component setup, manager reference writer/getter, schedule adjustment, both
direction-scaling branches and disabled fallbacks, distance-probe branches,
external record offsets, terminal/result order, cue and field-feedback gates,
maintenance/destruction, and the complete preload state machine were checked.
Focused tests cover every direction family, mirrored aliases/signs, clamping,
property mapping, reference arithmetic, both direction classes, projection
delta, positive/nonpositive external depth offsets, phase gate, and feedback
selectors.
