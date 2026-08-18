# Claim: Runtime note models and Joint textures use a closed presentation pool boundary

- ID: `claim.presentation.model-resource-pool-boundary`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `config.external_presentation`, `audit.indirect_calls`,
  `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

Runtime note-model handles and all six recovered Joint initializers use the
same presentation resource owner. Startup constructs the owner, a later
power-on phase rebuilds its model and texture contents from checked
`ModelSetTableRecord` and `TextureTableRecord` selectors, and runtime helpers
acquire and release lazily created `InstancingModel` objects by model-set ID.
The executable closes selection, invalid-ID behavior, pooling, activation, and
teardown. Concrete model/texture paths and resource payloads remain external.
The separate RTTI-identified `CacheManager` is not a direct runtime-note or
Joint resource owner.

## Anchors

- `game.exe @ RAM:00b2c140 / 00b2c220, InitializePresentationModelResourcePools / DestroyPresentationModelResourcePools, owner construction and destruction, hashes 76eea48891eb8b969f5e1aac307bbde7fb3a9bcb96dcd11ec462c0dc1268d79a and a76d6397cea6d12ce38ffe5d9c5f83fcff626352015309daebd0104fa8ada655`
- `game.exe @ RAM:00b2d160, InitializePresentationModelAndJointTexturePoolContents, fixed table scans, last-row texture clamp, and checked population, hash 7101f27648ef39415140d10b2ca5ae72109e49b26bce1e66ea994c65982a61e0`
- `game.exe @ RAM:00b2ca30, ClearPresentationModelAndJointTexturePools, pooled-instance and texture-wrapper teardown, hash 99bde7587bb8e56f8dbd72e6e8a91e3df6497fb3686544d02b513f2037e1064e`
- `game.exe @ RAM:00b2cbe0 / 00b2cae0 / 00b2ce50 / 00b2d340, entry lookup/create and instance acquire/release, hashes bc4661790fdc75f1c2394ab52d8d88b7ce33c869d4fec260a8e8401d8534d7a4, 089ba5a8262edf66c26edc206eb93cfcd6cd30e2ef46cab26800b01c9201c8b1, ef74877f05aa1747b27e4affd196c2c49613bd853776398afc96c3d7e3515b97, and 9bc71c1e4d2fba88824cd156b7a42fb546c378a874fc3dc0243136b6574f06a4`
- `game.exe @ RAM:00d7ee80 / 00d7ef30 / 00d7ef90, tracked handle release/replace helpers, hashes fd57d8b4fb74144292817a12eb0c6029ff3e834be7dc55186353d661ab1f8672, 0e565d398af823208c98abd47d1b9140dcf0969383477afc9e42d4eab040f00f, and f85323f5b9ca03f4846a190d0498f1e26cfbd9a29d5659108823883128d933a1`
- `game.exe @ RAM:00c32d20, LoadInstancingModelFromModelSet, checked ModelSet-to-Model load boundary, hash d924e53e21cc26a63dbf6de88c7c677e9bfafdd71aeb54cbbb7c0e5ecc341437`
- `game.exe @ RAM:00c33080 / 00c33360, InitializeTexturePoolSpriteWrapper / LoadTexturePoolSpriteFromCheckedRow, air::Sprite-backed wrapper construction and checked row load, hashes 69de547e6ec0ed5687933a2e8bc26f72fea30f618da4ec7bd1e2f956d25bb64f and 592f74cc4d21c6552b703e6832224b3e2ee9355b3088c2e1b2bf3be82b40f5a4`
- `game.exe @ RAM:00b2d220 / 00b2d010 / 00c331a0, SetPresentationJointTextureWrapperPoolSize, GetPresentationJointTextureHandleByIndex, and GetTexturePoolSpriteResourceHandle, hashes 4d74c1554b3a07a04486f04be7daa10355805d3fb8d944d754db62305696697b / 1edb5e087415550d0d3930a14c96704e38c96cbb20c8d5c03a8c4e631c4b4e5b / 1b8e59707ba9b5e029f07150c5f8bb2049f7f60e78e81b50b0e01c378b7467f9`
- `game.exe @ RAM:00c33120 / 0102f030 / 00b2c800, DestroyTexturePoolSpriteWrapper, ClearAirSpriteResource, and DestroyPresentationJointTextureWrapperStorage, hashes 95c4089c23cff78110f87dbd66f31f4f0c53ac95d280980d4ac028b965e1f418 / 556371ffe8b9c27aac25004bdeeff260cad360578efaa984c0ca4931097308c9 / 9c504bcbb05b3672fec52261f8ad871d277860fd4180d1e61635e8593c3cde1a`
- `game.exe @ RAM:0102e190 / 0102ec70, InitializeAirSprite / LoadAirSpriteResourceByPath, wrapper payload and resource path, hashes 30c45545367d5685629a691de62abe9808935539144ee4bb81b386f9db25c88c and b3746662e9b3a593d451d816a4fa27ae8837746b4ed6375331352b9e4cea28d7`
- `game.exe @ RAM:00c033a0 / 00c034a0 / 00c03580 / 00c03620 / 00c03710 / 00c03790, six Joint descriptor initializers reading handles from the same Sprite-wrapper owner`
- `game.exe @ RAM:01c77e68 / 01c77e8c, complete direct-reference sets for the presentation owner and separate CacheManager singleton`

## Observations

- The presentation owner is allocated as 0x18 bytes and initializes two
  vector-like containers. Its complete global-reference set consists of
  system construction/destruction, one power-on population call, six Joint
  descriptor initializers, and three tracked model-handle helpers.
- Population clears prior contents and checks model-set IDs 0 through 407.
  A pool entry is created only when the checked `ModelSetTableRecord` field at
  `+0x1c` is nonzero. It then resizes the texture-wrapper collection to exactly
  16. Slot `i` loads checked `TextureTableRecord` row `i` while `i` is below
  the external row count; otherwise it loads row `count - 1`. Thus a short
  nonempty table repeats its final row, a zero-row table selects `-1` and
  leaves all wrappers invalid, and a table longer than 16 contributes only
  rows 0 through 15. Each wrapper owns an `air::Sprite`, loads through the
  Sprite resource path, and exposes its resulting handle to Joint descriptors.
  The complete Sprite draw-caller set contains no pool-wrapper method, so these
  objects are resource carriers, not an additional direct note-Sprite renderer.
- The common Joint handle accessor rejects every requested unsigned index at
  or beyond the current wrapper count. Consequently negative signed selectors
  also reject after unsigned interpretation. Its complete caller set is the
  six Joint descriptor initializers. Admitted access returns the embedded
  Sprite handle, which can still be zero after an external load failure.
- Wrapper destruction clears a valid row's Sprite resource, restores row
  `-1`, deletes the Sprite, and zeros its pointer. Pool clear, shrink,
  destruction, and vector cleanup all reach that path. Resource clearing
  releases and zeros the handle and clears its name state without resetting
  the last stored dimensions; failed/cleared wrapper handles cannot pass Joint
  submission admission.
- Entry lookup returns null for ID `-1`; otherwise it returns the existing
  0x2c-byte keyed entry or appends one. Acquisition reuses the first free
  instance or lazily allocates a 0x14c-byte `InstancingModel`, loads the
  entry's model-set ID, marks it occupied, and activates it. Release finds the
  matching occupied instance, deactivates it, and marks it free.
- The ModelPoolList virtual hooks create, destroy, and toggle model instances.
  Per-entry creation/destruction and active/peak counters do not feed chart,
  judgement, result, or timing owners.
- Tracked handles are pairs of model-set ID and instance pointer. Release
  requires a nonnull pointer and ID other than `-1`, resolves the keyed pool
  entry, releases the instance, and clears the pair only on success. Replace
  releases the old pair before acquiring and storing the requested ID.
- The model loader resolves checked ModelSet and Model table fields before
  passing the selected external path/resource to the model object. Invalid
  selectors retain the table accessors' established `-1`, zero, or empty-path
  fallback rather than substituting a different model.
- `CacheManager` has a distinct RTTI descriptor, vtable, constructor,
  destructor, singleton global, and broad game-object consumer set. Its
  complete direct-reference set contains none of the runtime note-class or six
  Joint initializer paths above. Those paths read `01c77e68` directly.

## Reasoning

Construction, population, every direct global reference, virtual pool hooks,
and tracked-handle helpers form one closed owner lifecycle. The checked table
accessors link the externally loaded table rows to this pool, while the six
Joint initializers and model handle consumers close its presentation use.
The disjoint singleton references establish that `CacheManager` is a broader
game-object cache, not an alias for the runtime note/Joint resource pool. This
does not exclude CacheManager from non-note UI or scene presentation.

## Alternatives and falsifiers

- Competing explanation: runtime notes obtain meshes or textures through
  `CacheManager` and the small owner is only a preload helper.
- Evidence that disproves it: every recovered note model-handle and Joint
  texture initialization reads the small owner directly, while CacheManager's
  direct-reference closure contains none of those functions.
- Competing explanation: every possible ModelSet row is preloaded.
- Evidence that disproves it: the fixed 0..407 scan creates entries only when
  the checked row field at `+0x1c` is nonzero; model instances themselves are
  then created lazily on acquisition.
- Evidence that would disprove this claim: a second write to either owner, a
  runtime-note/Joint consumer of CacheManager, an indirect pool hook outside
  the recovered vtable, or a model/texture selector that bypasses the checked
  table boundary.

## Unknowns

- Concrete ModelSet, Model, and Texture row values and their referenced
  resource payloads are external to the executable snapshot.
- The semantic product labels for the checked ModelSet `+0x1c` field and the
  16 texture rows are not assigned beyond their observed inclusion/use.
- CacheManager's unrelated game-object consumers are outside this note-view
  slice unless a traced dependency reaches chart generation or judgement.

## Consequences

- Ghidra mutations: supported names at `00b2c140`, `00b2c220`, `00b2d160`,
  `00b2ca30`, `00b2cbe0`, `00b2cae0`, `00b2cbc0`, `00b2ce50`, `00b2d340`,
  `00b2d390`, `00d7ee80`, `00d7ef90`, `00c32d20`, `00c33080`, and
  `00c33360`; compact plate comments on population, lookup, model lifecycle,
  tracked-handle, Sprite-wrapper, and load boundaries.
- Spec sections: `spec/presentation.md` external presentation resource pools.
- Reconstruction code: `presentation_joint_texture_source_rows` and
  `presentation_joint_texture_index_admitted`; the asset-free renderer does
  not clone the proprietary model cache or resource payloads.
- Tests: `tests/scene_camera_presentation_test.cpp` covers zero-, short-,
  exact-, and over-16-row selection plus negative and out-of-range handle
  admission.

## Verification

The complete direct-reference sets for both singleton globals were enumerated.
The presentation owner's construction, population, every Joint/model consumer,
virtual create/destroy/activate hooks, tracked-handle helpers, clear path, and
system destruction were walked in both directions. The CacheManager exclusion
is limited to direct runtime-note/Joint resource ownership and does not claim
that its unrelated UI consumers are invisible.
