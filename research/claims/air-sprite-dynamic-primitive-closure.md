# Claim: `air::Sprite` geometry and gameplay ownership are closed

- ID: `claim.presentation.air-sprite-dynamic-primitive-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer rows `render.playfield_projection`,
  `config.external_presentation`, and `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

The complete direct draw-caller set for `air::Sprite` contains four owners.
Only the `projView::System` owner belongs to the gameplay chart system, where
an embedded `air::SpriteNode` submits the externally supplied background render
target during graph traversal. The other three owners are font/ruby text,
`air::LedObject`, and `EmoteControl`. The executable-owned Sprite defaults,
resource-handle lifecycle, anchor selection, transform order, six-vertex and
UV order, color replication, optional UV transform, primitive setup, graph
registration, and teardown are closed. Resource pixels and loaded dimensions
remain external.

## Anchors

- `game.exe @ RAM:0102e190 / 0102e300, InitializeAirSprite / DestroyAirSprite, constructor defaults and complete resource/util teardown, hashes 30c45545367d5685629a691de62abe9808935539144ee4bb81b386f9db25c88c and b78a27c013764c86b38192c18368f18525edd0148c48be09f417df13b60e71da`
- `game.exe @ RAM:0102e3f0, BuildAndSubmitAirSpriteQuad, sole Sprite geometry/submission method, hash 6c21606c52be0d0a690b9303beb2cbf5910d2a135e2b4c4c317c248bac243a2b`
- `game.exe @ RAM:0102ec70 / 0102edb0, LoadAirSpriteResourceByPath / AssignAirSpriteResourceHandle, both resource-admission paths, hashes b3746662e9b3a593d451d816a4fa27ae8837746b4ed6375331352b9e4cea28d7 and fa63a5a54db9e4422f11630da4014a4482f03bee28acb27bbb5d787eda5a354c`
- `game.exe @ RAM:00acd1b0 / 00acdd90 / 00ace220 / 00acd9c0 / 00acd9f0, projView system construction, background setup, graph callback, node unregister, and system teardown, hashes 3b1f8447d49bd09e6549545ada2dcee6804045bd3af5a5ab38d62279ca69d9e0 / a17b85e4cf20273a33bb5062422b2c03c37b091abcc8c0318ea1addfd4949afe / ee258a97ac28316a734bf0f94d78c1d32c6e7630e4109bf7d7ea05d0317c2df3 / cc9677a165c58d8ac03fb575c3661448e929a267eb48f2bb628ffe6bc962a0a2 / 9177124e64283f4c45fe7b091bd8f2ea714a0767f588c02e41e9ac1a30588dc2`
- `game.exe @ RAM:01051f50 / 0102f130 / 00c3b250, remaining direct Sprite draw owners: font/ruby text, RTTI air::LedObject, and EmoteControl, hashes e59e9d3211f1bb3c690b083f40022ea6ced0ae4358c08481e311ccccd2076ab7 / 600dc2912f38c8f07bc41751f38efb06f861136022936c9cbf79cf898bc932dd / 43e11b5f4a07624dad0534657fe6de6b29c4a64dc71b673c9d308e441de766bc`
- `game.exe @ RAM:00c33080 / 00c33360, InitializeTexturePoolSpriteWrapper / LoadTexturePoolSpriteFromCheckedRow, Sprite-backed checked texture wrappers, hashes 69de547e6ec0ed5687933a2e8bc26f72fea30f618da4ec7bd1e2f956d25bb64f and 592f74cc4d21c6552b703e6832224b3e2ee9355b3088c2e1b2bf3be82b40f5a4`
- `game.exe @ RAM:006db3d0 / 006dc000, InitializeDynamicPrimitiveUtil / FinalizeAndSubmitDynamicPrimitive, embedded primitive construction and common finalizer, hashes d84b53aeecbbc2bc3d2ede521a9cdae2209c9f6fb48e5135cd518305c6bad2ed and 4bacae0971c59df04f2ff551d469e052839c7f4a420d9ee3ec911eeb98815a39`
- `game.exe @ RAM:00d33bb0 / 012b34b0 / 014fd9a0, separately owned WindManager, AuraScene, and RTTI star::SglVTFWaterLine direct primitive producers, hashes 60128d2eb45dcbbc7375837c2e502c1cb2c0df7dc4086c7d01396f8adbfc8c62 / 4439f62abce24af8ed9016b1ca0d1317a128a2a097adb2643f07bc037e49b77c / be5234ea44c5b0253c2a388ca6d699bf7dfba30a2d063f64b2b91edf15783a1b`

## Observations

- Sprite construction installs the `air::Sprite` vtable and embeds one
  `DynamicPrimitiveUtil`. It initializes a null resource, zero translation,
  resource dimensions 16 by 16, unit XY scale, zero rotation, UV rectangle
  `(0,0)-(1,1)`, white packed color, null optional UV matrix, anchor 0, and
  topology selector 3.
- Both resource setters release the previous handle and retain the new one.
  A successful path load copies the resource's integer width and height into
  Sprite dimensions; failure retains a zero handle. Destruction frees the
  optional transform, releases the resource and name, and destroys the
  primitive utility.
- Draw requests primitive setup arguments `(4,3,6,1)` and emits six 0x18-byte
  vertices. Anchor modes form a 3-by-3 grid: columns 0/3/6 use `[0,w]`,
  1/4/7 use `[-w/2,w/2]`, and all other values use `[-w,0]`; modes 0..2 use
  Y `[0,h]`, 3..5 use `[-h/2,h/2]`, and values 6 or greater use `[-h,0]`.
- Each local point is scaled on X and Y, rotated, then translated. Position
  order is `(x0,y0),(x0,y1),(x1,y0),(x0,y1),(x1,y1),(x1,y0)`. UV order is
  the corresponding `(u0,v0),(u0,v1),(u1,v0),(u0,v1),(u1,v1),(u1,v0)`.
  A present matrix transforms each UV pair before submission, and the same
  packed color is copied to every vertex.
- `projView::System` embeds the Sprite and graph node. Scene construction
  assigns an external render-target handle, names the node `BG SpriteNode`,
  and the node callback draws with the default/null collector boundary.
  Teardown unregisters the graph node and destroys the Sprite before the
  remaining system bases.
- The direct Sprite draw thunk has exactly four recovered function callers.
  RTTI/vtable/string ownership assigns them to the gameplay background node,
  font/ruby text, `air::LedObject`, and `EmoteControl`; there is no fifth
  direct Sprite draw owner.
- The presentation texture pool's 16 row wrappers each own an `air::Sprite`.
  Their checked row loaders use Sprite resource loading, while Joint
  descriptors consume the resulting handles. Those wrappers do not occur in
  the complete direct Sprite draw-caller set and therefore are resource
  carriers rather than an additional Sprite-rendered note path.
- Direct WindManager and AuraScene primitive finalizers and the shared line
  helper's `star::SglVTFWaterLine` owner were followed separately. Their
  vtable/RTTI ownership is outside runtime chart-note presentation; they are
  retained as explicit negative paths, not silently discarded.

## Reasoning

The Sprite constructor, both resource-entry paths, sole draw method, common
primitive finalizer, complete draw-thunk caller set, and destructor bound the
class behavior. Upward traversal of every caller establishes the one
chart-system owner and three non-chart owners. Following the chart owner in
both directions closes scene setup, graph submission, render-target
assignment, unregistering, and destruction. The separate texture-wrapper and
direct non-Joint producer walks prevent similarly shaped helper use from being
misclassified as an undiscovered note primitive.

## Alternatives and falsifiers

- Competing explanation: the 16 Joint texture wrappers also draw chart-note
  Sprites directly.
- Evidence that disproves it: their loader constructs and populates Sprite
  resources, but none of their methods occurs in the complete four-function
  direct draw-caller set; Joint initializers consume their handles separately.
- Evidence that would disprove this claim: a fifth caller of the Sprite draw
  thunk, a second projView Sprite graph owner, a different resource setter, a
  teardown path that bypasses graph unregistering, or a transformed vertex/UV
  order outside the recovered six-entry sequence.

## Unknowns

- The background render target's pixels, format, concrete dimensions, and
  producer contents are external resource state. The executable-owned
  selection, assignment, geometry, and failure behavior are closed.
- Resource filtering, blend/depth state, material/shader state, and final
  composited pixels are governed by the already separated external pass and
  material boundary.
- Font/ruby, cabinet LED, EmoteControl, WindManager, AuraScene, and water-line
  payload behavior is outside chart generation and judgement after the owner
  exclusions above.

## Consequences

- Ghidra mutations: supported names and compact plate comments at the Sprite
  constructor/destructor/draw/resource setters, projView system/background
  owner/callback/teardown, texture-wrapper constructor/loader, primitive-util
  constructor/finalizer/line helper, and the three separately owned direct
  primitive producers.
- Spec sections: `spec/presentation.md`, background SpriteNode and common
  Sprite quad.
- Reconstruction code: `AirSpriteQuadParameters`,
  `air_sprite_dynamic_primitive_setup`, and `build_air_sprite_quad` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/scene_camera_presentation_test.cpp` covers defaults, all
  position/UV ordering invariants, center and bottom-right anchors,
  scale-rotation-translation order, UV transformation, color, and setup tuple.

## Verification

Constructor, resource setter, draw thunk, graph callback, unregister, and
destructor xrefs were enumerated independently. Each of the four direct draw
callers was assigned using compatible upstream callers plus RTTI, vtable, or
adjacent class-name evidence. The separately owned direct finalizers and line
producer were checked as negative paths. Focused and full-suite executable
reconstruction results are recorded in the active session.
