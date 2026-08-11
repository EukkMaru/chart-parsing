# Claim: Tap and CharaTap share one width-indexed model transform with distinct resource families

- ID: `claim.presentation.tap-chara-model-transform`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.tap_chara`, `render.playfield_projection`,
  `render.feedback_layering`, `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

Tap and CharaTap use the same active presentation update and chart-space model
transform. Their visible root differs by an exact width-indexed resource family:
ordinary Tap selects embedded rows 81 through 66, while CharaTap selects rows
97 through 82. A third rows-113-through-98 family is available to the same
loader for its alternate extended variant. Each family preserves the same
lane-centered position, projected depth, decoded-width scale, and lifecycle.

## Anchors

- `game.exe @ RAM:00c1da90, FUN_00c1da90, parsed load and model setup, hash a2e505b8dfcdaad35c0a037f6a2fe5fe75e08574163bc64ed3ea323ff04e0d46`
- `game.exe @ RAM:00c1dc30, FUN_00c1dc30, active wrapper, hash 22d52cd71f40ed597fdb6b20ec118a0ab5df7850419e1e9ac83833115130e8d9`
- `game.exe @ RAM:00c1d640, FUN_00c1d640, model transform update, hash 705a566a10c28049e4b75e955e80029d1a95b15b1c4cc28daf37b94427b30264`
- `game.exe @ RAM:00b28820, FUN_00b28820, lateral-center transform, hash 48582c5e1559c4da78c278131bf51e208fa9ff87db46f1a3b98492aa42954b00`
- `game.exe @ RAM:00b28890, FUN_00b28890, shared projected depth, hash 19696db1083c59e7e5324f7e5dcc4866796038ba33244d08ddd4b650966ceae8`
- `game.exe @ RAM:00b2b240, FUN_00b2b240, width index clamp, hash f22c328502800ac1ab302479077362561e130ed59c980d1a85b50d3c02d0b354`
- `game.exe @ RAM:00b2b290, FUN_00b2b290, model native-width scale, hash 6f0c54e3427cff15ccc090a3c3770e29592b9d052ef7db880a02976386a3f53f`
- `game.exe @ RAM:00c1d8a0, FUN_00c1d8a0, owned-model hide, hash 5f2f284c1f83f62ac65231dc02a2a4bb453f2cfee245e4d38209d98b662cfc6b`
- `game.exe @ RAM:00c1d9b0 and RAM:00c2b650, Tap and CharaTap preload steps, hashes 43aa9c10c2a72e39793e88c3d9635e371672eca1d9ac72750028443baf24794d and ffab65a3b0f587b3210c64c34a82b404409f616061356506be8d7563849ed2f0`
- `game.exe @ RAM:00da3a10, FUN_00da3a10, executable-owned depth-scale selection, hash e86da15c33a46d773787edde2d4eda2ed149d9295500d9336d601bef1cffc253`

## Observations

- `TapNote` and `CharaTapNote` share parsed-load `00c1da90`, active update
  `00c1dc30`, phase query `00c1d9a0`, and maintenance `00c1d8a0` vtable slots.
  CharaTap construction changes only the variant bytes that select its
  extended resource path and result behavior.
- Width lookup uses `clamp(decoded_width - 1, 0, 15)`. The ordinary table is
  the embedded descending integer sequence `81..66`. Extended variant zero is
  `97..82`; extended variant one is `113..98`. CharaTap fixes extended variant
  zero, so it selects `97..82`.
- The selected integer is a resource-table row, not an extracted texture or
  model. `InstancingModel` loading follows that row into external resource
  records. The exact family/index is recoverable; the proprietary mesh,
  material, and final pixels are not present as clean-room output.
- Initial model translation is
  `(4*start_lane + 2*decoded_width - 32, 0, -10000)`. This places the center of
  a decoded lane span on a field where each lane is four chart-space units and
  the full sixteen-lane width is centered on zero.
- Initial scale is `(decoded_width / external_native_width, 1,
  executable_depth_scale)`. A native width below one selects lateral scale
  `1.0`. `PlayMusic` construction sets the depth scale to `1.3` when its
  process/platform mode byte is nonzero and otherwise `1.0`; this value is
  executable-owned rather than fitted.
- On every active update whose family phase is not beyond one, the update
  computes `delta = scheduled_time - manager_time`, calls the common
  projection with base-offset and positive-delta DCM behavior enabled, writes
  the scale-plus-translation matrix, and forces the model visible. The
  maintenance/reset slot forces it hidden. Destruction releases the owned
  `InstancingModel` before the shared note base.
- Tap preload step zero stages all sixteen ordinary model rows and step one
  stages shared feedback resources; readiness becomes true at step eight.
  CharaTap steps zero and one stage both extended model families, steps two and
  three stage its feedback/effect resources, and readiness becomes true at
  step ten. These are loading-only objects, not live notes.
- The other half of the active wrapper, `00c1d510`, is result/feedback
  dispatch. It does not alter the root model's lane, width, depth formula, or
  resource-family selection and remains assigned to the shared feedback slice.

## Reasoning

The two vtables establish shared active behavior. Constructor-fixed variant
bytes and the load-time table branches establish the family difference. The
embedded integer sequences, clamp helper, resource-native-width query, common
lateral helper, model matrix writer, reset, destructor, and preload paths
close the root model from selection through lifetime without interpreting
external asset contents.

## Alternatives and falsifiers

- Competing explanation: CharaTap is merely an ordinary Tap with a different
  color chosen after drawing.
- Evidence that would disprove this claim: CharaTap selecting the ordinary
  rows, a distinct active transform target, or a post-load rewrite of its
  model row.
- Competing explanation: one lane is one presentation unit.
- Evidence that would disprove this claim: a lateral consumer that bypasses
  `00b28820` or a different lane-center formula in the shared update.

## Unknowns

- The final appearance and native width stored in each external model record
  are unavailable. A clean-room renderer must use an original configurable
  substitute while retaining the exact family and width index.
- Result-triggered effect identities, feedback lifetime, final scene layering,
  and camera/viewport conversion remain in their dedicated open rows.

## Consequences

- Ghidra mutations: plate comments at `00c1da90`, `00c1d640`, `00b28820`,
  `00b2b240`, `00b2b290`, `00c1d8a0`, `00c1d9b0`, `00c2b650`, and
  `00da3a10`.
- Spec sections: `spec/notes/tap.md`, including its CharaTap section.
- Reconstruction code: `active_note_projected_depth`,
  `tap_model_lateral_center`, `tap_model_resource_row`,
  `tap_model_lateral_scale`, and `tap_model_depth_scale`.
- Tests: `tests/tap_presentation_test.cpp`.

## Verification

Both runtime vtables, both constructors, their shared load/update/reset,
distinct preload paths, all three resource-row tables, the common lateral and
depth transforms, and model destruction were checked independently. Focused
tests cover lane extremes, all resource-family endpoints, width clamping,
external-native-width fallback, executable depth-scale selection, and every
projection flag branch used by the active model.
