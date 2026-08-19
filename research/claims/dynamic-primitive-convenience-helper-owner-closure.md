# Claim: common primitive convenience-helper owners are closed

- ID: `claim.presentation.dynamic-primitive-convenience-helper-owner-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer row `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

The complete common convenience-helper set comprises flagged/unflagged 3D
line segments, a flagged 2D triangle, and flagged/unflagged 3D triangles. Its
only live non-debug specialized caller is the already excluded
`star::SglVTFWaterLine` line producer. Every other caller is an engine
performance/debug overlay, a helper reachable only from that overlay, or an
unreferenced generic wrapper. No projView/runtime-note owner reaches these
convenience builders.

## Anchors

- `game.exe @ RAM:004037c9 -> RAM:006dbc60, BuildAndSubmitDynamicPrimitiveLineSegment, complete five-call set, hash 64f5830be70dd1ee22d52dd357a53003960efd9dba2bc93281be7068e23e748b`
- `game.exe @ RAM:006dbcf0, unreferenced flag-clear 3D line twin, hash ce6fa8648839b7a6a7663f707e18eecc7a3dcd0a19db35147494436a56a9f90b`
- `game.exe @ RAM:006dbd80, BuildAndSubmitDynamicPrimitiveTriangle2DFlagSet, no-reference set, hash 6a0393b066cd34669987f3f30d506da2139511dcd6df5d883cc3ffc6f6805abc`
- `game.exe @ RAM:0046e7c7 -> RAM:006dbe80, BuildAndSubmitDynamicPrimitiveTriangle3DFlagSet, complete six-call set, hash f07994fe2417e4af486e383a5969b984073422a17259aa2538230968777bfc62`
- `game.exe @ RAM:0046d485 -> RAM:006dbf40, BuildAndSubmitDynamicPrimitiveTriangle3DFlagClear, sole-wrapper path, hash 8d1b194503970fa1f6cbf83ce024671fc9de2cc9a666ee6ac8e671b46d8d0eea`
- `game.exe @ RAM:006c7770, performance/debug overlay with RAM/VRAM/DCall/DVtx labels and all live engine line/triangle helper calls, hash 1a1733b5e25b7979234011ea42128ccef4ea7e63ce4fbe89a13b976ef14237c7`
- `game.exe @ RAM:006c74e0 / RAM:006c73a0, helper paths reached only from the performance/debug overlay, hashes 7ce2d768e242f3da2590bced1871512c6888a213246d904194b202f44621bc56 / ff183eab1281978edeaa5754f13da978fb93b844d1e162bd804fdc3a7143a7a8`
- `game.exe @ RAM:006c6ae0 / 006c69a0 / 006c6b60 / 00667130, no-reference generic wrappers and their isolated nested calls, hashes 86a9487327898a26360c798741a780e6acb1773cce4f4346340ac834d5d41184 / 36e0b6640d883e9d894f23f33473e5df9dbf2334fbeefbb1c9824414d9f08a5b / 351a2a3ad2d93ab56bd3a853d1fa69a7a96a49dc49a0b049baec27acdef8fda2 / 73bbbf6b6403f6564755b8521805446ab7532518fa594a064d9ea997858179cb`
- `game.exe @ RAM:014fd9a0, SubmitStarWaterLineSegments and sole live non-debug line-helper caller outside the overlay, owner closure in claim.presentation.air-sprite-dynamic-primitive-closure`

## Observations

- The flagged 3D line helper's thunk has exactly five calls: two inside
  `006c7770`, one from `006c74e0` which is itself called only by `006c7770`,
  one from unreferenced `006c6ae0`, and one from
  `SubmitStarWaterLineSegments`.
- The flag-clear 3D line helper and flagged 2D triangle helper have no code or
  data references.
- The flagged 3D triangle thunk has six calls: two from `006c7770`, two from
  `006c73a0` reached only by `006c7770`, and two from `006c69a0` whose only
  thunk caller is unreferenced `00667130`.
- The flag-clear 3D triangle thunk has one call from `006c6b60`; that wrapper
  has no incoming code or data reference.
- `006c7770` formats and renders literal RAM, VRAM, total, CPU, GPU, DCall,
  DVtx, shader-constant, texture, model, primitive, filter, and build counters.
  Its callback chain terminates in a static engine callback slot. No chart
  record, runtime note, projView Joint, or gameplay-scene constructor occurs in
  that path.
- The five setup tuples are `(3,1,2,1)`, `(3,1,2,0)`, `(3,3,3,1)`,
  `(3,3,3,1)`, and `(3,3,3,0)`. Every builder obtains the common write-pointer
  slot, writes exactly its declared cardinality, and invokes the common
  finalizer with the standalone util's optional collector.

## Reasoning

Direct thunk xrefs bound every referenced helper; no-reference checks cover the
three dead roots and both isolated wrapper chains. Walking the live callers one
level upward collapses them to a literal performance/debug overlay plus the
separately RTTI-owned water-line producer. This is independent negative
evidence for the constructor-root inventory: even a chart-owned common utility
cannot gain hidden convenience geometry through another live caller.

## Alternatives and falsifiers

- Competing explanation: a chart family calls a helper indirectly through an
  unclassified callback, or one no-reference root is registered dynamically.
- Evidence that would disprove this claim: an additional helper thunk/data
  reference, a runtime-populated registration targeting one of the three
  no-reference helpers, or an upstream projView/runtime-note owner for a listed
  wrapper.

## Unknowns

- Performance/debug overlay enablement and hardware counters are engine debug
  state and are not reconstructed.
- The external water resource and final pixels remain outside this negative
  chart-owner claim.
- Semantic names for setup selector/topology enums remain unavailable; exact
  tuples are retained.

## Consequences

- Ghidra mutations: supported names/comments for the flagged 2D triangle and
  flagged/unflagged 3D triangle builders; a compact comment, but no forced
  near-duplicate name, on the unreferenced flag-clear line twin.
- Spec sections: `spec/presentation.md`, dynamic-primitive convenience-helper
  closure.
- Reconstruction code: none; no chart-side behavior is added.
- Tests: complete binary xref/no-reference and upstream-owner reconciliation;
  the common setup/finalizer tests already cover the shared state boundary.

## Verification

Each helper body was disassembled to recover its exact tuple, dimensionality,
vertex stride, cardinality, color copy, and finalizer call. All thunk/direct
xrefs were enumerated, every containing function was resolved, and each live
caller was walked to the debug callback or water-line owner. Ghidra was saved
after supported annotations.
