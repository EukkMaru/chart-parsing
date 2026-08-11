# Claim: AirSlide root, ASD resources, authored path, and presentation lifetime are exact

- ID: `claim.presentation.air-slide-model-path`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.air_slide`, `render.feedback_layering`,
  `render.mirroring_geometry`, `state.ownership`
- Last reviewed: 2026-08-10

## Statement

The exact AirSlide presentation path is resource-backed at the root and every
ASD-marked authored control, and uses the shared type-8/9/13 three-stream
geometry builder for the continuous root-to-final authored path. The active
wrapper runs gameplay/lifetime first and presentation second. Presentation is
suppressed only after the same start-phase-5/path-phase-5 terminal predicate;
resolved ASD controls hide both of their resources, while unresolved controls
are projected and updated. Geometry mode, feedback, transforms, preload,
maintenance, reset, and destruction are all family-local binary facts rather
than viewer-fitted behavior.

## Anchors

- `game.exe @ RAM:00c24d40 and RAM:00c25750, constructor/destructor ownership, hashes d0fcce4eab440807ede5e442d1d3ef908fc9ce1e88de7e074f31bbfd9c4450ac and c5c461aa63eba2c3b02b70ddc2439f87e0ca28acd43c727d962da9090e70cc1d`
- `game.exe @ RAM:00c28330, attached load and root/control geometry construction, hash 0707c8f057766eaaae2e0aad6d07346b82614d791e3c4fc908fe0296502489a9`
- `game.exe @ RAM:00c26bb0 -> RAM:00c27d50, gameplay/lifetime and exact terminal predicate, hashes 8f57bb1f00dca459509222165a7a24b84455aae7d52eba62e375d28c3caee877 and 8036cb802baf8ad9af863ae887e3af4a5f949e7c8feb92d2af9983c09782ec63`
- `game.exe @ RAM:00c26e00 -> RAM:00c03c00, family update and shared geometry, hashes 4c2310bb857fffda782d728e8e3c60ad99ea9292070ff9decbbb3a72fa769c7b and d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`
- `game.exe @ RAM:00c033a0, three resource descriptors/topologies, hash 5db616a7a5ea7f20cc629e70691b0ce5500cdb8753cdc8c226b0f2151eb1b1f5`
- `game.exe @ RAM:00c27bd0, RAM:00c27c70, and RAM:00c27f60, maintenance/reset/preload, hashes 37228cd7f629f394773252198bdf4bb27d1c5680b2556e80d9782886dcf1f9e1, b3d22ae27c3edc1db2a37d209a5c1688e959f31dc17db6dcef12adbb6ff1259d, and 9f91d7f7a0e5b61a00a567d212bdcb1d6dfe694139b76d689bf9be4bc738260c`
- `game.exe @ RAM:00b28680, RAM:00b286a0, RAM:00b28820, RAM:00b2b240, and RAM:00b2b290, common transforms and width/native-width selectors, hashes 1cc795d4bdfb7e1e4f971672fd21b7eaa2a3ef012b82b2bc753e726b14909e68, 31a7cd6a4f8746d96c42089a7f6b85e83c376c6d9d5ca3b1208ba7b3fe6cea6b, 48582c5e1559c4da78c278131bf51e208fa9ff87db46f1a3b98492aa42954b00, f22c328502800ac1ab302479077362561e130ed59c980d1a85b50d3c02d0b354, and 6f0c54e3427cff15ccc090a3c3770e29592b9d052ef7db880a02976386a3f53f`

## Observations

- The constructor allocates the `0x258`-byte AirSlide family object. Its
  matching destructor releases the authored segment records, retained
  checkers, two per-ASD resource handles, both ASD/schedule arrays, root
  resource, shared geometry wrapper, and base-family state.
- Attached load creates one `0xac` runtime segment per authored control. Each
  segment retains previous and current endpoint values. The main geometry path
  uses the root/previous side of each segment and then the final segment's
  current side with the final marker set. This is exactly root followed by all
  authored controls; generated judgement samples do not replace the authored
  geometry.
- Only ASD-marked control indices enter the resource/checker index. Each gets
  two width-indexed external handles. Their common origin is lane center,
  `(vertical - 1) * 3.8934999 + 0.14999962`, and parked depth `-10000`.
  Both use the external-native-width lateral scale. The second additionally
  uses vertical scale `rendered_vertical * 0.063597046`; the first remains
  unit-scaled vertically.
- Root lateral position is the common lane center. Root vertical position is
  `(initial_value - 1) * 3.8934999`, where the ordinary initial value is one
  and a type-13 attachment substitutes the final control's integer-tenth
  vertical. Root vertical scale independently uses the authored root vertical
  transformed by the same common helper and multiplied by `0.06420958`.
  Depth starts at `-10000`; lateral scale is decoded width divided by the
  external resource's positive native width, else one.
- During active presentation, external x/y/z offsets are added to root and ASD
  origins. Root visibility is start phase not equal to five. An unresolved ASD
  control projects its own schedule/tag, updates both resources, and stays
  visible; a resolved one hides both. Root and ASD animation advance only
  when the common adjusted-root near/range gate accepts the frame.
- Each update rebuilds two equal-cardinality arrays in root-then-control order:
  adjusted schedule minus manager current, and raw schedule minus manager
  current. It also recomputes every control's projected depth and lateral
  center before calling the shared geometry builder.
- Path phase three selects geometry mode one and sets the shared field-feedback
  byte. Phase four selects mode two. Every other phase selects mode zero.
  Shared streams zero and one use exact base white `0xffffffff` in modes zero
  and one and alternate gray `0xff666666` in mode two; stream two always uses
  low-alpha white `0x40ffffff`.
  The parsed appearance field maps `0 -> 2`, `1 -> 0`, `2 -> 1`, with default
  zero, and indexes three external resource tables at offsets row, row+3, and
  row+6. AirSlide stream topologies are `3,3,2`. Diagnostic counter calls use
  categories `4,6,5` in stream order `0,2,1`; that call order is not the Joint
  scene traversal order.
- Preload steps zero through two traverse all sixteen width slots for the two
  root tables and the ASD table. Steps three through six preload shared groups
  9, 10, 12, and 13. Steps seven through twelve are idle; a step above twelve
  latches ready. Maintenance finalizes two handles for each ASD index, then the
  root and geometry wrapper.

## Reasoning

The load-time previous/current segment fields, the update-time root/control
array construction, and the shared builder's cardinality checks form one
closed producer-consumer chain. The separate ASD index proves that action
resources are marker-owned while the continuous mesh remains authored-path
owned. Terminal, reset, maintenance, and destructor references close the
family lifetime rather than inferring visibility from conventional note
behavior.

## Alternatives and falsifiers

- Competing explanation: every ASC owns an action resource, generated samples
  form the visible path, root scale uses the type-13 replacement value, or
  presentation persists after the terminal conjunction.
- Evidence that would disprove this claim: a non-ASD insertion into the action
  index, a shared-geometry input sourced from the generated vector, a root
  scale load from the initial-value field, or an active presentation caller
  that bypasses the exact terminal predicate.

## Unknowns

- External resource identities, their native widths, material/texture
  contents, and final pixel semantics are absent source inputs and remain
  explicit parameters or excluded assets. Their selectors and consumers are
  closed.
- Final camera/viewport mapping and cross-family composition are shared
  downstream rows, not unresolved AirSlide-family branches.
- Owner footage review and viewer state integration remain product work; they
  do not weaken the static family-local claim.

## Consequences

- Ghidra mutations: supported-role plate comments at `00c24d40`, `00c25750`,
  `00c26bb0`, `00c26e00`, `00c27bd0`, `00c27c70`, `00c27d50`, `00c27f60`,
  `00c28330`, and `00c297d0`; no symbols or types changed.
- Spec sections: `spec/notes/air_slide.md`.
- Reconstruction code: `air_slide_*` presentation selectors/constants,
  root/action transforms, authored geometry path, and dual schedule arrays in
  `include/chart/reconstruction.hpp`; shared exact geometry remains under the
  existing `air_ladder_*` neutral helpers because both call `00c03c00`.
- Tests: `tests/air_slide_presentation_test.cpp` plus existing AirSlide
  judgement and shared-geometry tests.

## Verification

The constructor/load/update/terminal/maintenance/reset/preload/destructor paths
were followed in both directions. Instruction-level checks disambiguated the
root position value from the independent root-scale value and the segment's
previous/current width, lane, and vertical fields. The focused AirSlide
judgement and presentation tests pass; the repository-wide build and test
suite are recorded at the session checkpoint.
