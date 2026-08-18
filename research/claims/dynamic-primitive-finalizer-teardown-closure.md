# Claim: common dynamic-primitive finalization and teardown are closed

- ID: `claim.presentation.dynamic-primitive-finalizer-teardown-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer row `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

The common dynamic-primitive finalizer has exactly two installed vtable slots
and two non-vtable direct producer calls. It latches four offset-visible state
effects, submits one payload, and clears its pending pointer afterward. The
common destructor's complete caller set maps back to every construction-root
family and compiler unwind paths, but the helper itself only restores the base
vtable; it does not own hidden buffer release or container reset.

## Anchors

- `game.exe @ RAM:00413d7c -> RAM:006dc000, sole finalizer thunk and FinalizeAndSubmitDynamicPrimitive, hash 4bacae0971c59df04f2ff551d469e052839c7f4a420d9ee3ec911eeb98815a39`
- `game.exe @ RAM:0186b4a0 + 0x10 / RAM:0186d3f0 + 0x10, installed finalizer slots for RTTI sea::DynamicPrimitiveUtil and sea::DynamicPrimitiveEntryHelper`
- `game.exe @ RAM:019c9eac -> RAM:01c146b4 / RAM:019cbc14 -> RAM:01c146e4, complete-object locator to type-descriptor chains for the two vtable owners`
- `game.exe @ RAM:00d33bb0 / RAM:012b34b0, complete non-vtable direct finalizer calls from WindManager and AuraScene producers`
- `game.exe @ RAM:006dbc60, BuildAndSubmitDynamicPrimitiveLineSegment, setup tuple (3,1,2,1), two 0x10-byte vertices, same virtual finalizer, hash 64f5830be70dd1ee22d52dd357a53003960efd9dba2bc93281be7068e23e748b`
- `game.exe @ RAM:0040b771 -> RAM:006db5c0, sole common destructor thunk and DestroyDynamicPrimitiveUtil, hash 9156ac72ce82b3fce7bec8de251a2ca33353f9d98f110bb57816c7973809a9ca`
- `game.exe @ RAM:006c67c0 / 006c67f0 / 006c6960 / 006c72c0 / 006dc8f0, default-debug and extended-utility teardown callers, hashes 2e6408e438997689af33ec856c9947e277734123e1bf039f81d8222df8d03915 / c8b475c3b63defedc0f49e3201d922c8cae57d2600dc91c376cbefe918e98aa9 / 903c265d800c39077f1a393223a700fff9dbe489f00d939739b3fdfddc4edefa / 2a9f783f05e115e55fde8f8fdf0093d37ec76f48016ac5c03f57b9c28b8baf24 / c5b6298be34ec38a53e8317094f39ed657a71fd370a7fdbd4b3cffe9cb167cb9`
- `game.exe @ RAM:00725450 / 00729530 / 00729550 / 007bbee0, GuiWindow, air::Primitive, and font::TextBoxObject teardown callers, hashes 45b50a50e7643dfaf7b4dcbc642e370f23279dae7467f215980f526594a0c736 / 54a74d2b395679375995f384537e1b745f00a8f229ec7e0265fb63401f4e32ba / 8eb3c5da7e30a7dd5b92a07dc838ce3fe1aafabd47e3a53a8fda0b3640073742 / 326bcf26b20ac94bfe90063cd928f945f42db4a0fb31e69070530cb4b41de6f3`
- `game.exe @ RAM:00aad061, direct Surfride renderer cleanup call instruction in an incompletely recovered function chunk`
- `game.exe @ RAM:0102e300 / 01260d50 / 01260da0 / 012cfe90 / 012cfee0, Sprite, SPK, and SGL-mask teardown callers; SPK/SGL hashes ecee81e5ccc27cd1ebf2e66ca2f8d2482a3303b399867298470a6dabfac78abc / da2425d6e3f88398eb2ac7e601a9d1448bb4a2746e2cff1029ca0e6784ace531 / c8eca2a0fd5fef7f066f9a1b8a2a4d7e1e457e8af7823731b8d5462f707364b1 / ff271d028a5a17745c4ef364cbde0610940186d59a1d072336c227080002b9d3`
- `game.exe @ RAM:00d33bb0 / 012b34b0 / 014fd9a0, specialized Wind, Aura, and water-line stack/direct teardown paths`

## Observations

- The finalizer thunk has exactly four references: two executable vtable slots
  and direct calls from the WindManager and AuraScene primitive producers. The
  vtable RTTI identifies `sea::DynamicPrimitiveUtil` and
  `sea::DynamicPrimitiveEntryHelper`; constructor `006db3d0` installs the
  latter after initializing the base module.
- Finalization ORs bit 0 at utility `+0xa4`, replaces only bit 7 at `+0x80`
  from byte `+0x1c`, copies `+0x0c` to `+0x94` and `+0x08` to `+0x90`, calls
  the common submission path on payload `+0x20` with the optional collector,
  and clears pointer `+0x150` after the call returns.
- The line helper requests setup tuple `(3,1,2,1)`, writes two vertices of
  stride `0x10`, and dispatches through the finalizer slot at `+0x10`.
- The common destructor thunk's non-unwind calls occur in the default-debug
  owners, all extended utility owners, GuiWindow, ordinary/deleting
  `air::Primitive`, font text box, Surfride renderer, ordinary/deleting SPK,
  ordinary/deleting SGL mask, Sprite, Wind, Aura, and water-line paths. Thirteen
  compiler unwind handlers cover corresponding partial-construction paths.
- Decompilation of `006db5c0` shows only restoration of the
  `sea::BasePrimitiveModule` vtable. Storage release and wider object teardown
  occur in surrounding owner destructors. The prior plate comment implying
  common embedded-state release was therefore corrected.
- The Surfride call at `00aad061` passes the embedded utility at renderer
  offset `+0xf0`. Its surrounding cleanup range is not safely modeled as a
  standalone Ghidra function, so the stable call instruction is retained as
  the anchor rather than forcing a function mutation over questionable
  disassembly.

## Reasoning

The finalizer reference set closes both static virtual installation and direct
producer calls. Joint and Sprite have independently proved virtual-slot
dispatches, so they do not require another direct xref to the body. Reversing
the destructor thunk supplies a lifecycle inventory independent of constructor
xrefs; the recovered families agree with all 17 constructor roots and add only
expected deleting-destructor and compiler-unwind variants. The destructor body
itself falsifies hidden common storage ownership.

## Alternatives and falsifiers

- Competing explanation: another chart owner reaches an unenumerated
  finalizer, or the common destructor releases shared primitive storage.
- Evidence that would disprove this claim: another finalizer-thunk reference,
  a different vtable target reached by Joint/Sprite, an unmatched non-unwind
  destructor caller, or code in `006db5c0` beyond the observed base-vtable
  restoration.

## Unknowns

- The semantic enum names of the offset-visible setup/finalizer fields are not
  established and remain intentionally neutral.
- The external collector/default command's backend payload processing and
  final hardware rendering state are outside this state-transition claim.
- Compiler-generated unwind handlers are classified structurally; their
  source-level exception labels are not recoverable or gameplay-affecting.

## Consequences

- Ghidra mutations: corrected the `DestroyDynamicPrimitiveUtil` plate comment
  to state that it restores only the base vtable and owns no hidden release.
- Spec sections: `spec/presentation.md`, dynamic-primitive finalization and
  teardown.
- Reconstruction code: `DynamicPrimitiveFinalizeState`,
  `finalize_dynamic_primitive_state`, and `dynamic_primitive_line_setup` in
  `include/chart/reconstruction.hpp`.
- Tests: exact bit preservation/replacement, field copies, pending-pointer
  clear, and line setup tuple in `tests/scene_camera_presentation_test.cpp`.

## Verification

The finalizer thunk was checked from both vtable data and executable call
xrefs. The common destructor thunk was independently reversed through every
non-unwind caller and compiler unwind reference, then reconciled against the
complete constructor-root inventory. Focused C++ tests exercise both flag
states and ensure all other submission bits survive.
