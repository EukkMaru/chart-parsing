# Claim: the note presentation boundary is closed over twelve runtime vtables and eleven preload classes

- ID: `claim.pipeline.presentation-boundary-map`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.presentation_boundary`, `render.feedback_layering`,
  `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

Every parsed-record note object reachable from the runtime factory belongs to
one of twelve concrete vtables. The active manager reaches presentation only
through those objects' state-1 update and shared result/feedback slots; the
separate loading barrier constructs eleven of the same classes and reaches
only their resource preload/finalize slots. HeavenHold is a runtime-only
twelfth class and is intentionally absent from preload. Both object sets and
all three owning vectors have closed reset/destruction paths.

## Anchors

- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, complete runtime factory, hash ea011be474fda85c8cad0ca59b2c5f27d46a17a6652204e3930fa1aeb9674681`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, active-manager indirect dispatch, hash bed7892c43aec7a948d83aa2d146cbf6c46fa1ade2a95847409152f529cd84e9`
- `game.exe @ RAM:00b2a3c0, FUN_00b2a3c0, preload construction and indirect dispatch, hash 788eddda03de9911bd7d7232e92c2a133701f1a06d7c77f2719ec548e9957f24`
- `game.exe @ RAM:00b2b630, FUN_00b2b630, preload restart, hash 1665e9de1f11059306934fb5f073f418724130b8ba7bfcc3f57c7d1f404182e9`
- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, full reset, hash bd29062d3b65f1f97ae93fc82fb2ada49f46aa046abb6fb1af6034af02963fd7`
- `game.exe @ RAM:00b27210, FUN_00b27210, destruction, hash 9a01b09d3d02e45df68fef641835b450ee13e241243df9ffde766f701fad5f2a`
- `game.exe @ RAM:00b28890, FUN_00b28890, shared active chart-space projection, hash 19696db1083c59e7e5324f7e5dcc4866796038ba33244d08ddd4b650966ceae8`
- `game.exe @ RAM:018d92d4 through RAM:018d9b30, twelve concrete note vtables`

## Observations

- The runtime factory switch has nine primary construction cases and three
  attached-secondary cases. It calls primary virtual `+0x20` or secondary
  virtual `+0x24`, then appends the objects to different active vectors.
- The active manager calls primary `+0x30` and `+0x34` before input synthesis,
  then ticks primaries and attached secondaries through their common state
  wrapper. State 1 reaches concrete virtual `+0x2c`. The wrapper and result
  path may additionally reach the shared/family `+0x44` and `+0x48` surface.
- The complete family-specific dispatch matrix is:

  | Class | Vtable | Parsed load | Attached load | Active update | Candidate pair | Terminal query | Family maintenance | Preload init / step / finalize |
  | --- | --- | --- | --- | --- | --- | --- | --- | --- |
  | Slide | `018d92d4` | `00c10720` | `00c1d1b0` | `00c10ed0` | `00c0dae0` / `00c0db10` | `00c0fdc0` | `00c0fac0` | `00c0fd90` / `00c0fe20` / `00c0fcc0` |
  | AirLadder | `018d9374` | `00c132f0` | `00c1d1b0` | `00c13810` | `00c1b170` / `00c11c90` | `00c12d80` | `00c12b00` | `00c12d70` / `00c12df0` / `00c12c50` |
  | HeavenHold | `018d93e8` | `00c15cd0` | `00c1d1b0` | `00c16340` | `00c13e30` / `00c13e60` | `00c158f0` | `00c152a0` | `00c158e0` / `00c15990` / `00c15810` |
  | AirSolid | `018d945c` | `00c16fa0` | `00c1d1b0` | `00c17040` | `00c1b170` / `00c11c90` | `00c16f70` | `00c16e60` | `00c16f60` / `00c16f90` / `00c16e90` |
  | Tap | `018d9760` | `00c1da90` | `00c1d1b0` | `00c1dc30` | `00c1d4e0` / `00c1d500` | `00c1d9a0` | `00c1d8a0` | `00c1d990` / `00c1d9b0` / `00c1d8c0` |
  | Air | `018d97d4` | `00c1d120` | `00c1e730` | `00c1ec70` | `00c1b170` / `00c11c90` | `00c1e350` | `00c1e220` | `00c1e330` / `00c1e4a0` / `00c1e260` |
  | Mine | `018d9878` | `00c1f940` | `00c1d1b0` | `00c1fac0` | `00c1b170` / `00c11c90` | `00c1f690` | `00c1f590` | `00c1f680` / `00c1f7a0` / `00c1f5b0` |
  | Flick | `018d98f0` | `00c200a0` | `00c1d1b0` | `00c20340` | `00c1fd80` / `00c1fdb0` | `00c1fee0` | `00c1fde0` | `00c1fed0` / `00c1ff30` / `00c1fe00` |
  | AirHold | `018d996c` | `00c1d120` | `00c23f50` | `00c24cd0` | `00c1b170` / `00c11c90` | `00c23850` | `00c236a0` | `00c23840` / `00c23a60` / `00c23770` |
  | AirSlide | `018d9a14` | `00c1d120` | `00c28330` | `00c297d0` | `00c1b170` / `00c11c90` | `00c27d50` | `00c27bd0` | `00c27d40` / `00c27f60` / `00c27c70` |
  | Hold | `018d9abc` | `00c2adc0` | `00c1d1b0` | `00c2b480` | `00c29c10` / `00c29c40` | `00c2aae0` | `00c2a9c0` | `00c2aad0` / `00c2ab00` / `00c2aa00` |
  | CharaTap | `018d9b30` | `00c1da90` | `00c1d1b0` | `00c1dc30` | `00c1d4e0` / `00c1d500` | `00c1d9a0` | `00c1d8a0` | `00c2b640` / `00c2b650` / `00c2b580` |

- Slots `+0x3c` and `+0x40` are the same no-op targets on all twelve
  vtables. Slot `+0x44` is the common result/feedback consumer. Slot `+0x48`
  is its common one-position wrapper on ten vtables; Mine and Flick each use
  an opcode-identical wrapper which applies the shared active result-control
  remap to the incoming result byte before calling that common wrapper. These
  edges are part of the separate feedback/layering slice; they are not
  additional note factories or position transforms.
- A fresh preload constructs exactly Tap, CharaTap, Hold, Slide, Air, AirHold,
  AirSlide, AirLadder, AirSolid, Flick, and Mine, in that order. Each receives
  `+0x50` once, `+0x54(step)` until its ready byte is set, and `+0x58` whenever
  the stage advances. No parsed record is supplied and no active-vector append
  occurs.
- The primary, secondary, and preload vectors start at manager offsets
  `+0x2e8`, `+0x2f4`, and `+0x2dc`. Full reset and destruction independently
  delete every owned object; preload restart deletes only the preload vector.
- The common projection reached by the concrete updates returns chart-space
  depth, not a camera-space or pixel coordinate. With optional offset and DCM
  behavior it returns `-65 - approach_scale * (base_offset + delta) * speed *
  factor`. Reset owns exact defaults `approach_scale = 1.5` and `speed = 1.0`;
  setup replaces speed and copies the base offset from its traced configuration
  producer. A later model/primitive path owns camera, material, and pixel
  behavior.
- `projView::InstancingModel` construction and loading are resource consumers.
  The executable selects integer resource-table rows; those rows supply
  external model/material records. The clean-room boundary therefore retains
  the exact selector and transform while treating mesh/texture contents as
  unavailable, not as guessed geometry.

## Reasoning

The factory switch bounds every runtime class. Resolving each concrete vtable
through its thunks closes all manager-reachable indirect targets rather than
sampling one family. The preload switch independently bounds the loading-only
objects and explains the otherwise similar `+0x50/+0x54/+0x58` targets.
Distinct manager vectors and independent reset/destruction loops prove that no
hidden fourth note container or preload-to-gameplay transfer exists on this
path. The shared projection's callers then provide a common asset-independent
depth boundary for the concrete presentation slices.

## Alternatives and falsifiers

- Competing explanation: a separate renderer creates another gameplay note
  class after the parsed-record factory.
- Evidence that would disprove this claim: another append into either active
  vector, a factory-reachable vtable outside the twelve rows, or a manager
  indirect call whose target is not represented by the matrix.
- Competing explanation: preload objects become the live chart notes.
- Evidence that would disprove this claim: a move from the preload vector to an
  active vector, a parsed-record load call on a preload object, or a preload
  slot reaching input/result mutation.

## Unknowns

- Concrete family presentation behavior remains in the matrix's load, active
  update, maintenance, and feedback callees and is closed by vertical claims,
  not by this boundary claim alone.
- Final camera/viewport conversion and external model/material contents are
  downstream of the recovered chart-space projection.

## Consequences

- Ghidra mutations: plate comments at `00b28cc0`, `00b2a3c0`, `00b2b630`,
  `00b2b690`, `00b2ae30`, `00b27210`, and `00b28890`.
- Spec sections: `spec/timing.md` and per-family presentation sections.
- Reconstruction code: existing runtime-note dispatch, preload state, and
  projection helpers; concrete presentation behavior remains family-owned.
- Tests: existing `runtime_note_dispatch_test`, `note_view_preload_test`, and
  projection/materialization tests.

## Verification

The runtime factory, all twelve vtables, all family-specific slots from
`+0x20` through `+0x58`, the active manager, the preload switch, all three
vector append/cleanup paths, and the shared projection were resolved as
independent surfaces. The resulting class set agrees exactly with the RTTI
order recovered by the preload-barrier claim, with HeavenHold as the sole
runtime-only exception.
