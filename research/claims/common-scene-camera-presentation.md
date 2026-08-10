# Claim: common scene and camera presentation

- ID: `claim.presentation.common-scene-camera`
- State: active
- Maturity: reconstructed
- Confidence: high for scene ownership, math, loader/selector behavior, and the
  external active-pose boundary
- Owner: codex-root
- Coverage rows: `render.playfield_projection`; `render.feedback_layering`; `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

Gameplay creates a background scene followed by a higher-priority main scene,
each with an active `air::BasicCamera`; the executable fixes the complete
projection/view/viewport algorithm and the fallback camera values, while a
generic SVO/UVC graph path can supply camera motion. Gameplay selects rows 43,
47, and 46 from externally loaded `AcroartsTableRecord.bin`, prefixes every
selected string with `acroarts/`, and loads the resulting Forester graphs. The
graph relationship, selected strings, motion payload, and resulting gameplay
pose are external to the executable snapshot.

## Anchors

- `game.exe @ RAM:00acdd90, FUN_00acdd90, gameplay scene construction and scene parameters, hash a17b85e4cf20273a33bb5062422b2c03c37b091abcc8c0318ea1addfd4949afe`
- `game.exe @ RAM:006ff3b0, FUN_006ff3b0, air::Scene replaces its default camera with BasicCamera and binds scene/aspect, hash 4c1847473379bd5986d9c8beaee1ada6f7dc39036f3632b518a8bbd699eebb2c`
- `game.exe @ RAM:00651530, FUN_00651530, camera parameter definitions and defaults, hash 6c0f9585d65247f5c9f899aad0d3c40c8ad07d0fcb71a2fb4c63a4bd6c8594b1`
- `game.exe @ RAM:00653750, FUN_00653750, projection/view recomputation and degeneracy handling, hash 8591871ba81b556dad4d58558edce72b1f78eb8f1f1e1fc00008bc1adb71a6c6`
- `game.exe @ RAM:006b0cb0, FUN_006b0cb0, perspective matrix, hash 3f2d2653ec78823d0190f7003805c97abbb169d910f899b19e37eec60a34dcd5`
- `game.exe @ RAM:006b06c0, FUN_006b06c0, look-at matrix, hash fb4a4e50ff700bcf9c0d6f1fe758ff4b155b13c091b93ba931454c6b55fa2973`
- `game.exe @ RAM:006522a0, FUN_006522a0, world-to-viewport conversion, hash 6a5451792f195d6822390d9cf97fdd0a39551d2634529028e5ac3aba7f44c3ee`
- `game.exe @ RAM:01434520, FUN_01434520, SbUvcFileLoader parameters/defaults, hash 8563308ec159fb2859dd7b78c24bd89b8e84dbbcfcbce2bdf933ff32e3014c81`
- `game.exe @ RAM:01435250, FUN_01435250, BasicCamera mixer population from UVC graph children, hash 21065b6c6265899c809e448577b985aced2d1aa473580417ca73f5586c087ac3`
- `game.exe @ RAM:01435c20, FUN_01435c20, motion control application, hash 346e83934370b24f31db43863ef9c7b9dca530760fa4b2bf167cacdf6b6aedf6`
- `game.exe @ RAM:00b2e990, FUN_00b2e990, three Forester players and external rows 43/47/46, hash 3f4ad7f3e9c6817573d6a9570a97cad3144e100b4f1b8f5b6f978c1ea0826b4e`
- `game.exe @ RAM:00b2f000, FUN_00b2f000, first-player acroarts root, LayerTable row 6, and draw mask, hash f8b8b1a4ab9e4fce4a0d2e4bc087177056aaeb5d39007dcac107e4c6c6a89dd4`
- `game.exe @ RAM:010e53a0, FUN_010e53a0, AcroartsTableRecord table construction, hash fd2ceb8dfaf4a9795ebfa2953573b6e0120427b56fc4f545aea2721013bf46b1`
- `game.exe @ RAM:010e4660, FUN_010e4660, external table path composition and load, hash 1d86f9e4b75354ce4a347cbb09a04d91a8c3738a2c37361cc580bb6dc3ddd5c3`
- `game.exe @ RAM:010e6570, FUN_010e6570, Acroarts row string accessor and empty fallback, hash 9a6ec16a9214c71ea130dd5fbead76e6cdd8e38a6263853a21075784c4dab719`
- `game.exe @ RAM:010f0080/010f1070, LayerTableRecord construction and row-value accessor, hashes fd2ceb8dfaf4a9795ebfa2953573b6e0120427b56fc4f545aea2721013bf46b1 / f9bd3d15d5c6288b41220a230213afcb637fcc5c44dda868cde38a0710b06199`
- `game.exe @ RAM:007d0320, FUN_007d0320, cached acroarts/ resource root, hash 243588228bb96838a33c6e92c7227f56434324ae5e8b7275caec9b32c0084336`
- `game.exe @ RAM:0126d580, FUN_0126d580, Forester parameter declarations/defaults, hash 3e1ce38dc07745d2a417f1acb15a899472eeccca37dbed3825efe310f4d4932d`
- `game.exe @ RAM:01271ac0, FUN_01271ac0, Forester graph-resource load/failure path, hash 27b3a8807f93e6697938bd44b5176fce0f434a270abf0affdada5daad75dd05b`

## Observations

- MainScene uses priority 10000, draw index 0, and the immediate path; BgScene
  uses priority 9900, draw index 16, and the non-immediate path. The global
  scene owner inserts by ascending signed priority and traverses forward.
- Scene construction is the only live writer that replaces the active camera
  field. The remaining direct field accesses are initialization, getters, and
  destruction. Forester declares `TargetScene` and `DrawMask` parameters but
  does not replace the active camera pointer. Field setup explicitly assigns
  only the first player's draw mask and each player's `2DLayer`; any nonempty
  `TargetScene` value must come from the external graph/parameter data.
- BasicCamera exposes setters for FOV, position, target, and up. A direct call-
  site sweep found only the generic camera-motion sampler plus unrelated UI,
  lighting, and reflection cameras; no gameplay-specific direct setter writes
  MainScene's pose.
- Every graph node receives a runtime ID in the global graph registry.
  SbUvcFileLoader resolves a supplied ID, requires BasicCamera RTTI, enumerates
  loader children, creates normal or offset mixer slots, binds motion handles,
  and installs frame/control parameters. Failed resolution or RTTI mismatch is
  a no-op; unresolved motion handles leave resource pointer and length zero.
- The generic table loader composes an externally selected database directory,
  a backslash, the table-record name, and `.bin`. The table type/name pair makes
  the resource-string source `AcroartsTableRecord.bin`. Its rows are 0x18 bytes;
  the accessor bounds-checks the selected index, resolves the row's string
  handle at +0x0c through the shared string arena, and returns the global empty
  string for a negative or out-of-range row.
- Gameplay field setup obtains Acroarts rows 43, 47, and 46. All three paths,
  including row 43 through `FUN_00b2f000`, concatenate the selected string with
  the cached `acroarts/` root before Forester loading. The three row indices are
  embedded globals with values 43, 47, and 46 in this snapshot.
- Field setup separately gets integer `2DLayer` values from rows 6, 29, and 29
  of externally loaded `LayerTableRecord.bin`. Its 0x10-byte row accessor reads
  +0x0c and returns zero for a negative or out-of-range row. These values feed
  Forester parameter index 0x17, which the constructor declares as `2DLayer`;
  they are not `TargetScene` selectors.
- Forester copies the final resource path, asks the resource system for its
  graph, and initializes a runtime player only when the resource and graph
  object resolve. Failure leaves graph/player setup absent. The selected table
  strings and resource graph contents are not executable constants.
- The exact default camera maps world Y=0 at all chart depths to the viewport's
  vertical center. It therefore cannot, by itself, explain the observed
  vertical travel surface; treating it as the final gameplay pose would be a
  contradiction rather than a safe fallback assumption.
- `FUN_00c1cd10` also projects a field anchor through MainScene's camera, but it
  has no code/data/thunk reference in this snapshot and is not used as proof of
  the live gameplay path.

## Reasoning

Construction and field-write closure establish the active camera object's
ownership. Matrix constructors plus the world-to-viewport consumer close the
asset-independent mapping. The UVC loader closes how an external graph may
alter the same camera class, including failure behavior, without exposing the
motion payload. The exact table loaders, row layouts, indices, path prefix,
consumers, and invalid/missing fallbacks close the executable side of the
external boundary. Only the contents of the named external table and graph
files can establish whether a loaded graph links an SbUvcFileLoader to
MainScene's BasicCamera or which numeric pose it supplies. Accordingly the
executable algorithm and fallback are reconstructable; the selected active
pose remains an explicit external parameter rather than an unresolved Ghidra
guess.

## Alternatives and falsifiers

- Competing explanation: constructor camera defaults are the final gameplay
  camera. This is contradicted by their inability to turn chart Z motion at
  Y=0 into vertical screen motion.
- Competing explanation: a loaded Forester player replaces Scene+0x90. The
  scene-camera write closure contradicts it; Forester instead owns named scene,
  mask, draw-index, and 2D-layer parameters.
- Evidence that would disprove this claim: a live writer replacing MainScene's
  camera, a non-UVC generic parameter path targeting it, or embedded resource
  contents that establish a different selector/fallback.

## Unknowns

- External-data only: the strings in Acroarts rows 43/47/46, which selected
  graph (if any) links an SbUvcFileLoader to the active MainScene camera, and
  that graph's camera-motion samples, animation length, and resulting pose.
- Runtime/environment only: framebuffer dimensions and final material/depth/
  blend/pixel output.
- No executable-side selector, consumer, or fallback remains open in this
  camera/resource slice.

## Consequences

- Ghidra mutations: compact plate comments on the scene constructor, camera
  projection/viewport function, UVC mixer loader, field resource loader,
  Acroarts/Layer table accessors, and Forester resource load.
- Spec sections: `spec/presentation.md` scene/camera and viewport sections.
- Reconstruction code: scene descriptors, camera defaults, matrix builders,
  viewport conversion, and explicit external camera controls in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/scene_camera_presentation_test.cpp`.

## Verification

The focused test independently checks scene order, constructor defaults,
aspect fallback, matrix coefficients, viewport-center behavior, the default
pose contradiction, an explicitly supplied pitched pose, both external table
names/fallbacks, all three Acroarts rows, the shared root, and all three layer
rows. Final suite and structural validation are recorded in the session note.
