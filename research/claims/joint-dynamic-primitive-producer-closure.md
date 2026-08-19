# Claim: every Joint dynamic-primitive producer is assigned to a recovered owner

- ID: `claim.presentation.joint-dynamic-primitive-producer-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer rows `render.hold`, `render.slide`,
  `render.air_slide`, `render.air_ladder`, `render.air_solid`, and
  `render.heaven_hold`
- Last reviewed: 2026-08-18

## Statement

Every `projView::Joint` constructed by this executable snapshot belongs to one
of six RTTI-identified wrapper classes. Five wrappers are reachable from
factory or precompute ownership and cover Hold, Slide, AirSlide/AirLadder,
AirSolid, and HeavenHold. The sixth belongs to Field, whose sole recovered
allocation root has no incoming reference and is excluded explicitly. All
reachable Joint children share one graph callback, one dynamic-primitive
submission path, and one unregistering teardown. No additional Joint-backed
chart primitive remains unassigned.

## Anchors

- `game.exe @ RAM:00bfebb0, InitializeJointDynamicPrimitiveNode, sole base Joint constructor, hash f3f9eed732002e4f91d8f62528b3deccd7c18d15cd1f128d9aec558bce59132f`
- `game.exe @ RAM:00bfec90 / 00bfeef0 / 00bff1f0 / 00bff140 / 00bff440 / 00bff510, six complete constructor callers, hashes 7639412ecc4b50cae3be0fd2d20688f32b56442659fec1fb770b678c26a2444c / 5527ad8f429bf0f04c9ddbe2206cef9db53889ef55437e010d5afb55f4f3b5a9 / 9e125243cbff277870d3f937c6ddc34b7d48c8e4d5928bd83979b70ebd6932a7 / 2f63350ccfdbb479e5dc47c3850c91ba4acbe98cea0e521ceebaaaf3b5e1d7e4 / 6a9a9a9f44d32f4c6bbb58d49c2e31c0c9a322e7e529280e645e6f1476d43f81 / 66de92b297888e5864942a07d4526e0a4e117b343c31a030739c8fb01b0ddb32`
- `game.exe @ RAM:00c033a0 / 00c034a0 / 00c03580 / 00c03620 / 00c03710 / 00c03790, all six descriptor initializers, hashes 5db616a7a5ea7f20cc629e70691b0ce5500cdb8753cdc8c226b0f2151eb1b1f5 / ed4401caef2ac5582ec41dc19571a8cd298ba22367a31c37dd5fc0771c9f1265 / 1ed92855d3f1a21984ca366bce2424f6667210b143bd155872cba1137192cbd3 / fe48f967c6f5d90c9a8870ed73b2f45695be72d39abf1d3aaf361ee2b80b3693 / e23892449da86f81dbd8bc23be6389b612f39952e23374db85285232b5adbba8 / 11ffe4b74695ebecbdf85ec509bfed650c209ae26e50c0f03697aefe537b9d6a`
- `game.exe @ RAM:00b236a0 and 00c28330, AirLadder selector 9 and AirSlide selector 8 producers, hashes e4793bf2a9246ddbe30bb6a6e56d30e903dc19fba6fa63e220eedd48908bbf3c and 0707c8f057766eaaae2e0aad6d07346b82614d791e3c4fc908fe0296502489a9`
- `game.exe @ RAM:00d7e550 -> RAM:00d7e350, sole Joint graph callback and sole submitted-state builder, hashes 63ca6a8fac377d8be80bc4798b93b6db0e3a868ab804ff20a126f914a4166273 and 782a23c0695543971d538d73a78b945a81b97baf6880d9addaf8e47acf8b5c6e`
- `game.exe @ RAM:00609a00 -> RAM:0060ccf0 -> RAM:00605ef0 and RAM:0060ada0, graph registration and ordered traversal, hashes 57fae60beb1d9b36de9de7d1d046eb3bfc2dbb71f6d91c112aa6d5859e187e41 / 8ee2d67bb919ed328da77783875ecb26704f9a6401ed0920b6cfd22119b63894 / 4c9c26639dd752bfd59e5adf1e80b905c497fa0723867cadaea2a16475750b39 / fbfa41ea943c45f71662836d9c1c3d1693e6071e96b80adca6ab342ed9dd98b0`
- `game.exe @ RAM:00bffa60 -> RAM:00d7e290 -> RAM:00609cf0, vertex destruction and graph unregistering teardown, hashes a6f0e4e6b69f41a528d5be8cbe68d9bba1305aeb2fbd551938df2bcc50cc439b / 33d7b15d811cd8cb04c5cbbc3936c181ae404cc424dfad435a249b8906c93bf7 / e2c971032f5a0dd3a40e3e87d93aa13b91b423017f342441ae3376a3c8cc0e8a`
- `game.exe @ RAM:0066dbc0 -> RAM:0062e4d0 -> RAM:0063b360, compatible-batch/default-command boundary, hashes 61db305364da24f183847cf64c00956d6fdef0c376b6c48fb5fc2ae1bc70c0ae / 2a3e3bcc252530443d55da9af110ac47bd735e6b63694ad3391c26075f8f902d / 06ee2b304a2c4db2f82df081cacd6d28ad04bb80e1527e241d74227817019ef5`
- `game.exe @ RAM:00b28c10, unreferenced Field allocation root, hash cd76d0e5b996a48c66fd4771a2a6f9de851d8b7e964ab4e2ec7e0bd463ac3594`

## Observations

- The only reference to the base Joint constructor is its import thunk. The
  complete caller set of that thunk is exactly the six wrapper constructors
  below; no factory, view, manager, or other class constructs a base Joint
  directly.

| Wrapper | Recovered owners | Joint children | Submitted topology/mode |
| --- | --- | ---: | --- |
| `JointSlide` | type-2 Slide precompute | 3 | `[4,3,3]` |
| `JointAirSlide` | type-9 AirLadder precompute and runtime AirSlide view | 3 | `[3,3,2]` on both reachable paths |
| `JointAirSolid` | type-10 AirSolid precompute | 1 | `3` when its valid handle permits submission |
| `JointHeavenHold` | type-13 HeavenHold precompute | 1 | `4` |
| `JointHold` | runtime Hold view | 1 | constructor default `4` |
| `JointField` | unreferenced Field root only | 1 | `0`, unreachable exclusion |

- The shared Air-path initializer computes stream 0 as
  `3 + unsigned(selector - 8 > 1)`, then assigns streams 1 and 2 to 3 and 2.
  AirLadder's producer writes selector 9; AirSlide's producer writes selector
  8. Both therefore use `[3,3,2]`. This contradicts and corrects the former
  AirLadder `[4,3,2]` transcription.
- AirSolid writes topology 3 only while installing a valid checked texture
  handle. An invalid index retains the constructor-default zero handle and is
  rejected before submission, so every submitted AirSolid primitive has
  topology 3. HeavenHold and Slide assign their topology values regardless of
  handle validity. Hold leaves the base Joint default of 4 unchanged.
- The Joint graph vtable has exactly one callback target. It first requires a
  nonzero runtime resource handle, then requires a nonempty vertex count
  divisible by three. The builder repeats those guards, copies the descriptor
  and each 0x18-byte vertex, and submits through the existing optional-
  collector boundary. A null collector reaches the adjacent-compatible batch
  check and default command append.
- Construction registers each Joint child through `sea::GraphNode`. Three-child
  wrappers construct in stream-index order, and recursive traversal visits the
  stored child vector in order. Every wrapper destructor deletes all of its
  Joint children. Base Joint teardown releases vertices and reaches GraphNode
  removal before primitive-base destruction.
- `JointField` is structurally complete, but its class constructor and setup
  are reached only from the allocation helper at `00b28c10`. That helper has no
  callers, data references, or recovered absolute-pointer producer in this
  snapshot. It is not silently counted as factory-reachable gameplay.

## Reasoning

The complete constructor-xref set bounds every possible owner of the common
Joint callback. Following each wrapper upward identifies its chart/runtime
producer; following the descriptor initializers and callback downward fixes
resource admission, topology, traversal, batching, and command submission.
Following each wrapper's virtual destructor back through the base closes graph
registration lifetime. The Field root is retained as an explicit negative
path rather than inferred away.

## Alternatives and falsifiers

- Competing explanation: another runtime class constructs or submits a Joint
  indirectly, or AirLadder stream 0 uses topology 4.
- Evidence that would disprove this claim: a seventh caller of the base Joint
  constructor thunk, another callback pointer in the Joint graph vtable, a
  second caller of the submitted-state builder, an incoming producer for
  `00b28c10`, or an AirLadder caller value other than the observed literal 9.

## Unknowns

- Texture rows, resource payloads, material/shader behavior, blend/depth state,
  and final pixels are external inputs. Their checked handle-selection and
  zero-handle admission behavior are closed; their contents are not copied or
  guessed.
- This claim closes Joint-backed primitive production. Instancing-model and
  non-Joint feedback primitives are owned by their separate active claims.

## Consequences

- Ghidra mutations: supported names and compact plate comments at the base
  Joint constructor/destructor, JointBase teardown, common admission/callback/
  submission functions, all six wrapper constructors, and all six descriptor
  initializers.
- Spec sections: `spec/presentation.md` and the affected note specifications.
- Reconstruction code: corrected
  `air_ladder_primitive_topologies{3,3,2}`; existing family geometry and
  topology constants remain unchanged.
- Tests: `tests/air_ladder_presentation_test.cpp` now asserts the corrected
  topology triple; existing Slide and AirSlide topology/geometry tests remain
  independent controls.

## Verification

Constructor, initializer, callback, submitted-state, registration, traversal,
destructor, and unregister xrefs were enumerated independently. The two inputs
to the shared selector were traced at their producers rather than inferred
from family appearance. The Field allocation root was checked separately as a
negative reachability path. Focused and full-suite verification is recorded in
the active session handoff.
