# Claim: Air presentation shares an exact vertical transform and direction mirror pairing

- ID: `claim.presentation.common-air-transform`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: viewer rows `render.air`, `render.air_hold`,
  `render.air_slide`, `render.air_ladder`
- Last reviewed: 2026-08-07

## Statement

The resource-independent vertical origin used by AirHold, AirSlide, and
AirLadder is `(value - 1) * 3.8934999`. AirSlide action resources and AirLadder
checkpoint resources add `0.14999962` after that transform. Ordinary AirHold
authors no vertical value and retains constructor value 1, so its object
origin is vertical zero unless a type-13 root supplies its final control value.
AirNote mirrors its external resource's lateral scale for direction codes 2
and 4, the AUL and ADR codes; codes 1 and 5, AUR and ADL, keep positive scale.

## Anchors

- `game.exe @ RAM:00b28680, FUN_00b28680, common Air vertical transform`
- `game.exe @ RAM:00b286a0, FUN_00b286a0, action/checkpoint offset transform`
- `game.exe @ RAM:00c1e730, FUN_00c1e730, AirNote load and direction scale sign`
- `game.exe @ RAM:00c1df20, FUN_00c1df20, AirNote transform submission`
- `game.exe @ RAM:00c21020, FUN_00c21020, AirHold constructor default`
- `game.exe @ RAM:00c23f50, FUN_00c23f50, AirHold root-dependent load`
- `game.exe @ RAM:00c28330, FUN_00c28330, AirSlide path/action transforms`
- `game.exe @ RAM:00c03c00, FUN_00c03c00, AirLadder main/effect transforms`

## Observations

- The plain helper subtracts 1 and multiplies by `3.8934999`. The offset
  helper performs the same operations and then adds `0.14999962`.
- AirHold construction initializes its vertical field to 1. Ordinary root
  types leave it unchanged. The type-13 load case may replace it from the
  final root control before the plain transform is evaluated.
- AirSlide initializes the corresponding root value to 1, may replace it for
  a type-13 attachment, and transforms authored AirSlide control verticals
  with the plain helper. Its checkpoint/action records use the offset helper.
- AirLadder main authored geometry uses the plain helper; its generated
  checkpoint effect uses the offset helper, matching the existing type-9
  presentation claim.
- AirNote stores the parsed direction code. Its load chooses negative lateral
  resource scale exactly for integer codes 2 or 4, and positive scale for the
  other codes. The update submits that signed scale in the resource transform.

## Reasoning

The two small arithmetic helpers are shared callees whose inputs can be traced
back to each family-owned field. Constructor defaults and the sole type-13
overwrite close the otherwise ambiguous AirHold height. AirNote's sign branch
and downstream transform submission establish the left/right mirror pairing
without assigning a shape to the external resource.

## Alternatives and falsifiers

- Competing explanation: value 5 is a universal ordinary Air plane, or ADL and
  ADR mirror according to their spelling independent of up/down direction.
- Evidence that would disprove this claim: an ordinary non-type-13 AirHold
  write of a different vertical value, an AirSlide action path using the plain
  helper, or a later sign overwrite before AirNote transform submission.

## Unknowns

- External model/texture geometry determines the visual extent around each
  recovered object origin. It is not present and must not be inferred from the
  transform alone.
- The asset-free viewer's arrow angle and replacement glyph remain labeled
  product choices. Only the binary's mirror sign pairing is canonical.

## Consequences

- Ghidra mutations: none.
- Specs: the Air, AirHold, AirSlide, and AirLadder note specifications.
- Reconstruction: `common_air_render_vertical`,
  `common_air_action_render_vertical`, and
  `air_direction_resource_scale_sign`.
- Viewer: ordinary AirHold origin moved from the fitted value-5 plane to exact
  value-1/vertical-zero; AirSlide actions receive the recovered offset; AIR
  lean pairing is described as binary sign plus an asset-free glyph.
- Tests: `tests/air_ladder_presentation_test.cpp`.

## Verification

Both transform helpers, AirNote load/update, AirHold constructor/load, and
AirSlide load/action-record construction were inspected. Caller checks agree
on input field ownership and no alternate common vertical transform was found
for these paths.
