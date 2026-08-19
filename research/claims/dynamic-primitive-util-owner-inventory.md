# Claim: every direct `DynamicPrimitiveUtil` construction root is assigned

- ID: `claim.presentation.dynamic-primitive-util-owner-inventory`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer row `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

The complete direct caller set of the common `DynamicPrimitiveUtil`
constructor contains exactly 17 function roots. The chart-side roots are
`projView::JointBase` through one `air::Primitive` overload and the gameplay
background through `air::Sprite`; their behavior is closed by separate active
claims. Every other root is assigned to a named engine GUI/debug/font,
Surfride, SPK, WindManager, AuraScene, SGL-mask, or water-line owner. No
additional direct utility construction root remains available to hide an
unassigned chart-note primitive.

## Anchors

- `game.exe @ RAM:00416b49 -> RAM:006db3d0, sole constructor thunk and InitializeDynamicPrimitiveUtil, complete 17-function caller set, constructor hash d84b53aeecbbc2bc3d2ede521a9cdae2209c9f6fb48e5135cd518305c6bad2ed`
- `game.exe @ RAM:006c65b0 / 006c6f40 -> RAM:00666e90, two anonymous render owners with sole DefaultDebugScene constructor owner, hashes 2e86e9c509712463cd197f325b61a1a64167983034b2c652a3090d90f749c4ed / 7a82877fe0a24eaaf2d5d029b51be42f0b7bf534e87cbb556d8dae11f09334ab / 2a69738f3a81fecf4f1ccfed51127f35482b7f6540a7c789cae98ff6d7e8fbab`
- `game.exe @ RAM:006dc6d0 / 006dc7b0, extended utility constructors with complete caller and no-reference sets, hashes ffe11c783596d28a459167cf13c8bb16701e62d4bb16e6e84db44ecf0dad6c8f and e5bd7b62aa80f99c381e058b006a9c3d509accfbcd7c8006d8e45ab41305f0d4`
- `game.exe @ RAM:007251c0, InitializeAirGuiWindow, GUI root and complete derived-constructor callers, hash 59e05b32ffa82367afa16c50f0c6002fc5e808a96d69271288ac7da8f906bafe`
- `game.exe @ RAM:007290f0 / 00729210 / 00729320 / 00729430, four air::Primitive constructor overloads, hashes 4b2860d2e57ef010d52b76e1ec2d42490de726b5c4f51c30f4a762438982a6b4 / 091b388f0629d03859c2fecf25d51866c8597a69db7ae2e2028eefc2cdb427ec / 180626585e0ebc873b33e6600c47f24dcb4951d7bfd0f4da7b63e5df3eefd19e / b41446dc9ebf4ae63a755110ca1182d7fe2f7de745304657b739fc850ab72278`
- `game.exe @ RAM:01c17314 / 01c17bbc / 01c17bec / 01c18b9c / 01c195f8 / 01c1a224 / 01c1801c / 01c17cc8 / 01c17fb4 / 01c17fe4 / 01c17f50 / 01c17ee4 / 01c1979c / 01c19bc4, RTTI type descriptors assigning the non-Joint air::Primitive callers to GUI/debug/editor classes`
- `game.exe @ RAM:00d7e180, projView::JointBase constructor and sole chart-side air::Primitive owner, hash recorded by claim.presentation.joint-dynamic-primitive-producer-closure`
- `game.exe @ RAM:007bbab0 -> RAM:007b3ef0, InitializeFontTextBoxObject and sole font::FontManager owner, hashes 421e1d910611166735d5d8fe01a9a7c04bbe54523ddac11ee43dd26177d4bb1b and 363718cf27d6f6981b6c643402c1832f0abc39fd1cfb4644e6c16152684545af`
- `game.exe @ RAM:00aacb90 -> RAM:00a8f320, InitializeSurfrideRenderer and sole surfride::SrPlayer::Impl owner, hashes c6a4080c27ce6f99f67096c6c5a3b2684ea7b4675669f4606ecc52fede242c58 and 6603978db007a3d816c5f99270e2e75aadaf58b3bf9e6e652468b044f06057f2`
- `game.exe @ RAM:01260c50, InitializeSpkDynamicPrimitive, complete callers confined to SPK debug/font/SGL primitive functions, hash 3a051614dc3d812b1a668118d70dc7ba7e42eb64214f141693d7ddf46bfa742e`
- `game.exe @ RAM:012cfd50 -> RAM:012d03a0, InitializeStarSglMask and sole registered SGL-mask allocator, hashes b1cb9589cd352653fc31be7b9eef8365aee6c9bb133cc603bbc9dfd0778fa95d and 1e980c253623089aa1fa78c2523db801a1950634b8c7acd868315ca1ad3e56ff`
- `game.exe @ RAM:00d33bb0 / 0102e190 / 012b34b0 / 014fd9a0, WindManager / air::Sprite / AuraScene / star::SglVTFWaterLine roots, hashes and owner closures recorded by claim.presentation.air-sprite-dynamic-primitive-closure`

## Observations

- The only reference to `InitializeDynamicPrimitiveUtil` is its import thunk.
  The thunk's complete caller set is exactly:
  `006c65b0`, `006c6f40`, `006dc6d0`, `006dc7b0`, `007251c0`,
  `007290f0`, `00729210`, `00729320`, `00729430`, `007bbab0`,
  `00aacb90`, `00d33bb0`, `0102e190`, `01260c50`, `012b34b0`,
  `012cfd50`, and `014fd9a0`.
- `006c65b0` and `006c6f40` are both allocated by one function that creates a
  scene named `DefaultDebugScene`; neither has another caller. The extended
  constructor `006dc6d0` is used only by those two owners, `air::GuiWindow`,
  and the engine's `Debug` system window. Its fixed-default twin `006dc7b0`
  has no code or data reference in this snapshot.
- `air::GuiWindow` has four complete construction callers: the generic window
  allocator, `air::GuiPanel`'s in-place and allocating constructors, and
  `air::GuiMenuBar`. No projView class constructs this root.
- The four `air::Primitive` overloads have complete caller cardinalities
  2, 5, 5, and 5. Their owners are:

| Overload | Complete recovered owners |
| --- | --- |
| `007290f0` | shared `air::GuiTab` / `air::DebugGraphManipulator` / `air::GraphNodeManipulator` callback; `air::CameraManipulator` |
| `00729210` | `air::SystemWindow`; `air::PhysicalNodeEditor`; two helpers reached only by RTTI `air::ModelDebugWindow`; `air::IKNodeEditor` |
| `00729320` | `air::FilterNodeDebug`; `air::LightNodeDebug`; `air::LightPointNodeDebug`; `air::LightShadowParallelNodeDebug`; `air::DebugCameraScene` |
| `00729430` | `air::EditObj`; unreferenced engine helper `007333f0`; `air::CameraDebug`; `air::SceneNodeDebug`; `projView::JointBase` |

- The model-debug pair is reached together only from the `ModelDebugWindow`
  update. Its adjacent editor strings describe model/shape windows and
  texture/property controls. The SystemWindow path exposes literal Play/Pause
  debug controls. These assignments come from RTTI complete-object locators,
  vtable slots, and compatible callers rather than from naming convention.
- `font::TextBoxObject` is constructed only by `font::FontManager`.
  `surfride::SrRenderer` is embedded only by `surfride::SrPlayer::Impl`.
  These are common text and external animation subsystems, not new direct
  runtime-note primitive owners.
- `SpkDynamicPrimitive`'s complete constructor-thunk callers remain inside
  SPK functions. Direct vtable evidence identifies `SpkDebugPrimitive`,
  `SpkBasicFont`, and `SpkSglDynamicPrimitive`; the remaining callers build
  temporary instances inside the same SPK drawing range. No direct projView
  constructor occurs in this set.
- `star::SglMask` has one direct allocating wrapper registered by the star/SGL
  factory table. WindManager, AuraScene, and `star::SglVTFWaterLine` have
  already been assigned through their own vtable/RTTI chains. `air::Sprite`
  has the separately closed four-owner draw set.

## Reasoning

Constructor-thunk xrefs bound the root inventory without depending on class
names. Each root was then walked upward through its own thunk, complete caller
set, vtable data references, RTTI complete-object locator, and type descriptor
where available. The four generic `air::Primitive` overloads receive special
treatment because one caller is chart-side; exhaustive overload-caller
classification isolates that one Joint owner instead of excluding the class
wholesale. The remaining subsystem roots are named boundaries, so later work
can reopen a concrete Surfride/SPK/SGL consumer without inventing another
utility constructor.

## Alternatives and falsifiers

- Competing explanation: another note family constructs a generic
  `air::Primitive`, or an anonymous utility root is a hidden chart owner.
- Evidence that disproves it: every overload and utility constructor thunk has
  a complete caller set; only `projView::JointBase` occurs in those sets, while
  both anonymous roots have the sole literal `DefaultDebugScene` owner.
- Evidence that would disprove this claim: an eighteenth constructor-thunk
  caller, another projView/runtime-note class in an overload caller set, an
  incoming reference to `006dc7b0` or `007333f0`, or RTTI/vtable evidence that
  reassigns one of the listed debug/editor callbacks.

## Unknowns

- Surfride animation data, SPK particle data, SGL mask resources, and their
  resource-selected final pixels remain external rendering payloads. This
  claim assigns their common primitive construction roots; it does not claim
  that these generic subsystems can never be selected by a gameplay scene.
- The caller-visible meanings of the four setup arguments in the common
  dynamic-primitive utility remain intentionally unnamed where no engine enum
  or distinct consumer establishes a semantic label. Observed tuples and
  their geometry consumers are recorded by their owning claims.
- This structural inventory does not replace the separate finalizer/callback
  and gameplay-trigger claims for reachable Joint, Sprite, model, or feedback
  presentation. The common finalizer and complete destructor mapping are
  closed separately by
  `claim.presentation.dynamic-primitive-finalizer-teardown-closure`.

## Consequences

- Ghidra mutations: supported names for `InitializeAirGuiWindow`,
  `InitializeFontTextBoxObject`, `InitializeSurfrideRenderer`,
  `InitializeSpkDynamicPrimitive`, `InitializeStarSglMask`,
  `InitializeDefaultDebugScenePrimitiveOwners`, and
  `UpdateAirModelDebugWindow`; compact plate comments on all anonymous/common
  constructor roots and their ownership boundaries.
- Spec sections: `spec/presentation.md`, dynamic-primitive construction-root
  inventory.
- Reconstruction code: none; reachable executable-owned Joint/Sprite geometry
  remains in the owning reconstructions, while debug/editor and external
  payload subsystems are not cloned.
- Tests: structural closure is verified by complete binary xref/RTTI sets;
  existing Joint and Sprite focused tests cover the two chart-side roots.

## Verification

The common constructor thunk and every nested constructor thunk were enumerated
independently. All 17 roots were decompiled; every `air::Primitive` overload
caller was assigned using RTTI/vtable evidence, including the model-debug pair
and the one Joint exception. No-reference checks were repeated for `006dc7b0`
and `007333f0`. A separate reverse enumeration of the common destructor reaches
the same owner families plus compiler unwind paths, providing a lifecycle-side
cross-check. Ghidra was saved after names and plate comments were applied.
