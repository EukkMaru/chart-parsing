# Claim: material pass flags and offscreen requests

- ID: `claim.presentation.material-pass-flags`
- State: active
- Maturity: reconstructed
- Confidence: high for flag selection, precedence, sort-key consumption, and offscreen request propagation; medium for the external/backend state boundary
- Owner: codex-root
- Coverage rows: `render.feedback_layering`; `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

The executable maps three generic scene-submission backend kinds to material
attributes, derives pass Type and User with exact backend-specific precedence,
obtains the two-word key used by the ShaderUpper/ShaderLower sorts, and turns
four material bits into pass-scoped or submission-scoped color/depth offscreen
requests. Concrete material values and the final hardware state selected by the
backend remain external inputs.

## Anchors

- `game.exe @ RAM:0063b830, FUN_0063b830, backend-kind-1 wrapper construction, hash 6d05e1492e5bb0fe18e2a26d3829c7c8cf93d1b14e5fd19456d1167de44a56f5`
- `game.exe @ RAM:0063b880, FUN_0063b880, backend-kind-0 wrapper construction, hash a6d5b1f059cc636c8f4097c04369dd49a4eeb90f0a0e035e6991946365a38e03`
- `game.exe @ RAM:0063b8f0, FUN_0063b8f0, backend-kind-2 wrapper construction, hash 15aeb22b0a588c5644e1b29eecd487731146f7c1a0a7f0c3824108f752b2a4f8`
- `game.exe @ RAM:0063b650, FUN_0063b650, backend material-attribute selection/default, hash 10b2d35e48467129b43100221401c2c2d99ce3d0a06c2f1bbcda3d1005285afd`
- `game.exe @ RAM:0063b740, FUN_0063b740, backend-specific pass-Type classification, hash c1591a9500c39e8c99856c733ea999eae6ab10ff66a1112fd1763ea6896fe38d`
- `game.exe @ RAM:0063bbb0, FUN_0063bbb0, 64-bit material-sort callback/default, hash eb27ad0d8173ec3e02358c24e258a6367bdb847f7c3bca11c97fda6ae5cacb1a`
- `game.exe @ RAM:00648a90, FUN_00648a90, four-bit User extraction and pass routing, hash 9f97abad88236c3ef7c4ae2dc4951bbb41f4560abe70190489689fa5a916518b`
- `game.exe @ RAM:00649080, FUN_00649080, sort-key refresh and offscreen request aggregation, hash 1c528056441cb36a0a2fd9f41413d35b945860b9951788102033fed3d716f530`
- `game.exe @ RAM:00601bd0, FUN_00601bd0, scene-wide RequestColor/DepthOffscreen propagation, hash 87736745e8d9038752b20f840aab4e07932bafb395bff42ce0c3b7bcb3047b3b`
- `game.exe @ RAM:0064be60, FUN_0064be60, pass-scoped target selection, hash d3c411478a88232a7205daed25eb9a2a4eab7a56b8b30f01e5ab77a3170cf6e0`
- `game.exe @ RAM:00646b00, FUN_00646b00, submission-scoped target selection before draw, hash 3fbf5cadfd10d0b0d0b2dfd0f766173b5b5af436b48cf9fdf9e9d8864fac9527`
- `game.exe @ RAM:0063b560, FUN_0063b560, final generic backend draw dispatch, hash 397b54fe8255546b45dda5b0b7a99c9be051e517eacf4a01e06b11045a8f2af3`

## Observations

- Backend kind 0 reads its material-attribute record at payload `+0x88`;
  kind 1 reads it at payload `+0x58`. Kind 2 uses a process default whose flag
  word is zero. The pass router extracts User as `(flags >> 25) & 0xf`, so the
  default kind-2 path has User zero.
- Kinds 0 and 1 start with Opaque, select Punch for bit `0x40`, and select
  Trans for bit `0x20`; Trans wins when both bits are set. Kind 0 then selects
  2D for bit `0x80`. Kind 1 instead selects 2D when the flag sign bit or a
  secondary payload bit `0x10` is set. For both kinds, bit `0x2000` selects
  Reduce after every earlier test and therefore has final precedence.
- Kind 2 selects Trans when one nested payload byte is nonzero, otherwise
  Punch or Opaque from a second nested byte. Its wrapper bit `0x08` then
  overrides that result with 2D. It does not consult the kind-0/1 material
  classification bits.
- Before sorting a pass whose Sort value is below 2, the prepass refreshes the
  record's two words at `+0x30/+0x34` from a backend callback. Kind 0 invokes
  its payload virtual callback. Kind 1 invokes its indexed backend callback
  only when its alternate callable is absent; otherwise it returns zero. Kind
  2 returns zero. Both ShaderUpper and ShaderLower order the high word and then
  low word descending. No evidence assigns the low word an independent depth
  meaning.
- Material bits `0x200` and `0x400` add the current external PassIndex to the
  color and depth offscreen masks respectively and also raise the corresponding
  scene-wide request. Bits `0x800` and `0x1000` raise only the scene-wide color
  and depth request and are supplied directly to the active filter source
  immediately before that submission is drawn.
- Scene preparation maps the two accumulated request bytes to BasicScene
  parameter indices 14 and 15, whose declared names are
  `RequestColorOffscreen` and `RequestDepthOffscreen`. During draw-index
  traversal, the filter receives the two pass-mask bits; the sorted draw loop
  supplies the two per-submission bits and reapplies filter-source state before
  dispatching the backend draw.

## Reasoning

The wrapper constructors establish the three kind tags rather than relying on
their visual use. The material accessor, Type classifier, and pass router close
flag-to-routing behavior. The prepass connects the same attribute word to the
two-word sort callback and four offscreen request bits. Scene parameter
propagation and both filter call sites then close those requests through the
point immediately preceding generic backend drawing.

## Alternatives and falsifiers

- Competing explanation: ShaderLower reverses the backend sort key. Both sort
  values reach the same high-then-low descending implementation.
- Competing explanation: `0x800/0x1000` are pass masks. They never set the mask
  words; they raise scene-wide requests and are passed at individual draws.
- Evidence that would disprove this claim: another live material accessor for
  these three wrapper kinds, a later Type/User rewrite before first-match
  routing, or a draw bypass that ignores both the pass and submission target
  selections.

## Unknowns

- The concrete material flag words, callback-returned sort keys, shaders,
  textures, blend/depth state, and filter-source graph objects selected by the
  external gameplay resources.
- The final backend/hardware interpretation of those external payloads and the
  resulting pixels. The executable-owned request routing is closed; absent
  resource content is not reconstructable as a constant.

## Consequences

- Ghidra mutations: compact evidence comments at `0063b650`, `0063b740`,
  `0063bbb0`, `00649080`, `00601bd0`, and `00646b00`.
- Spec section: `spec/presentation.md` material pass classification and
  offscreen requests.
- Reconstruction code: `presentation_pass_type_from_material`,
  `presentation_pass_user_from_material_flags`, and the offscreen request
  accumulation/selection helpers in `include/chart/reconstruction.hpp`.
- Tests: `tests/material_pass_flags_test.cpp`.

## Verification

The focused test checks every classification override and precedence edge,
User extraction, the separation of pass-scoped masks from submission-scoped
bits, scene-wide request accumulation, and invalid PassIndex rejection. Full
build/tests and harness validation are recorded in the session note.
