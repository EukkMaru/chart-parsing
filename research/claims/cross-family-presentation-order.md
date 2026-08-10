# Claim: cross-family updates use two active vectors before one effect pass and externally keyed scene sorting

- ID: `claim.presentation.cross-family-update-composition`
- State: active
- Maturity: reconstructed
- Confidence: high for executable-owned update, feedback, scene, and pass order;
  high that concrete final occlusion remains external
- Owner: codex-root
- Coverage rows: `audit.cross_family_visual`, `render.feedback_layering`,
  `pipeline.presentation_boundary`
- Last reviewed: 2026-08-10

## Statement

One gameplay outer update finishes every note-manager substep using a complete
primary-vector pass followed by a complete attached-secondary-vector pass,
then updates the ten effect lists once, materializes pending records, and
exports scene state. Feedback creation and shared overlay writes therefore
follow primary-first/secondary-second dynamic construction order. Drawing is
not that update order: submissions traverse `BgScene` before `MainScene`, take
the first matching external pass, and are sorted by the selected pass keys;
source order survives only when that exact stable sort leaves keys equal.

## Anchors

- `game.exe @ RAM:00da62c0, FUN_00da62c0, pending scan after manager/effects, hash cb798253647b9f10eed6cd3d3aeaa4417cbc410ea3682481f762a20829cfa27b`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, distinct primary/secondary appends, hash ea011be474fda85c8cad0ca59b2c5f27d46a17a6652204e3930fa1aeb9674681`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, primary then secondary dispatch/removal, hash bed7892c43aec7a948d83aa2d146cbf6c46fa1ade2a95847409152f529cd84e9`
- `game.exe @ RAM:00da8730, FUN_00da8730, ordinary catch-up then one effect update, hash ed2d269bf3ba4bfcefd8300792569c3c467773fc2b2a543636e5db55b918712a`
- `game.exe @ RAM:00da70c0, FUN_00da70c0, alternate one-snapshot input path, hash 0959c2ccedac3b54b30f05483aab971b3b4f2c214ba70001388c319a7259880b`
- `game.exe @ RAM:00da9820, FUN_00da9820, outer operation order, hash ce8c5ca2608c3b0e7c39932b2e6042056865380a6824f611ea0342be79c90f1c`
- `game.exe @ RAM:00b1d880, FUN_00b1d880, ten-list forward update, hash e6500a80c166c7029bc3559937ee028e71f74430c84b8c6b2f0f8f0d8653518d`
- `game.exe @ RAM:0060ada0, FUN_0060ada0, stored child traversal, hash fbfa41ea943c45f71662836d9c1c3d1693e6071e96b80adca6ab342ed9dd98b0`
- `game.exe @ RAM:00d7e550 and RAM:00d7e350, Joint validation and submission, hashes 63ca6a8fac377d8be80bc4798b93b6db0e3a868ab804ff20a126f914a4166273 and 782a23c0695543971d538d73a78b945a81b97baf6880d9addaf8e47acf8b5c6e`
- `game.exe @ RAM:00bfdec0, FUN_00bfdec0, feedback scene/sequence binding, hash 1bdbd89b14dd8b8474b61e2b2b2efc6df629587c91434cd1d2854c94694b8e68`
- `game.exe @ RAM:00648a90, RAM:00646fd0, and RAM:0064be60, first-match routing, exact stable sorting, and draw traversal, hashes 9f97abad88236c3ef7c4ae2dc4951bbb41f4560abe70190489689fa5a916518b, 59d48bccc7632a6400f98f85045094a92dca5c9cd0d4dd5124fcd34a5739b945, and d3c411478a88232a7205daed25eb9a2a4eab7a56b8b30f01e5ab77a3170cf6e0`

## Observations

- The factory stores roots in the manager vector at `+0x2e8` and optional
  AIR/AirHold/AirSlide attachments in the separate vector at `+0x2f4`.
  Within each vector, appends preserve pending-scan encounter order. There is
  no combined vector in which an attachment is adjacent to its root.
- Every manager substep completes candidate collection over primaries, ticks
  every primary forward, performs the primary removal sweep, then ticks every
  secondary forward and performs its removal sweep. A secondary cannot
  interleave with its root or precede a later primary in that substep.
- The twelve family slots all keep their family-owned ordering. TAP/CHR, Mine,
  Air, AirLadder, AirSolid, AirHold, and AirSlide run gameplay/result/lifetime
  work before their presentation half. Hold, Slide, and HeavenHold update
  their start/path components, test terminal state, and then update their
  marker/body presentation. Flick's integrated routine likewise resolves its
  state/result branch before the final root-model visibility/transform branch.
- Result feedback is synchronous inside those updates. Its ordinary attempts,
  lane overlay, authoritative dispatch, cue, and Slide extras retain the order
  in `claim.presentation.shared-result-feedback`. Thus simultaneous feedback
  calls occur in complete primary-vector order and only then complete
  secondary-vector order. The lane serial-group overwrite rule and the
  process-global feedback-player sequence key make that ordering observable.
- The ordinary path can perform its bounded catch-up substeps, but calls the
  EffectManager only after the loop. The alternate path performs one manager
  substep and then the same effect update. Both update the ten lists once in
  indices 0 through 9. An effect created during any catch-up substep is not
  advanced between note substeps; it participates in that one later list pass.
- Pending materialization is after that effect pass. A newly constructed note
  cannot update, emit feedback, or alter shared overlay state in the same
  outer call. Post-manager scene-resource setters are later still and are
  one-way exports.
- Family-local dynamic primitives preserve their proved local order: Hold and
  HeavenHold submit one body stream; Slide/AirLadder/AirSlide own three Joint
  children whose callbacks are visited as streams 0, 1, and 2. This does not
  establish a global cross-family layer.
- The renderer visits `BgScene` priority 9900 before `MainScene` priority
  10000. Within a scene, each submission is captured by its first external
  Type/User/Range match, then the selected pass performs one of eight exact
  sorts. Equal finite keys retain the producer's submission order; unequal
  material, depth, or layer keys override note update order.

## Reasoning

The two append destinations and two independent manager loops directly
contradict the former single interleaved-vector model. Following both outer
branches proves that no effect-list update or pending construction occurs
between catch-up note substeps. The shared feedback consumer then connects
note update order to creation serials and lane-overlay overwrite order. The
separate graph/pass trace proves why that ordering cannot be promoted to final
draw order: pass routing and sorting consume external material-derived keys
after the producers submit.

## Alternatives and falsifiers

- Competing explanation: a root's attached secondary updates immediately
  after that root. This is false when another primary is live: the later
  primary runs before every secondary.
- Competing explanation: transient effects advance once per catch-up substep.
  The sole ordinary EffectManager call is after the loop.
- Competing explanation: dynamic construction or result order is the final
  occlusion order. The scene/pass sort path can reorder unequal keys.
- Evidence that would disprove this claim: another live note vector, an effect
  update inside the ordinary catch-up loop, a gameplay submission bypassing
  the mapped scene/pass consumers, or a post-sort reordering stage.

## Unknowns

- Concrete gameplay `BasePass` rows, material flags/keys, graph relationships,
  model and texture contents, depth/blend state, and camera-motion payloads are
  external resources. The executable proves their selectors and consumers but
  does not contain the deployed values needed for one unconditional pixel
  overlap table.
- Source order between objects in different external graph parents cannot be
  replaced with note-manager order. A clean-room viewer must either load an
  original external configuration or expose those values explicitly.

## Consequences

- Corrected claim: `claim.interactions.cross-family-candidate-result-order`
  now distinguishes the two active vectors and primary-first result order.
- Ghidra mutations: expanded supported plate comments at `00b28cc0`,
  `00b2b690`, `00da8730`, and `00da9820`; no symbols were renamed.
- Spec sections: `spec/matching.md`, `spec/timing.md`, and
  `spec/presentation.md`.
- Reconstruction code: `RuntimeActiveVectors`,
  `runtime_manager_update_order`, and
  `build_cross_family_outer_update_schedule`.
- Tests: `tests/candidate_interaction_test.cpp` and
  `tests/cross_family_presentation_order_test.cpp`.

## Verification

The exact factory appends, both manager vectors and removal loops, ordinary and
alternate outer paths, ten-list effect update, pending scan, every family
state-1 wrapper, shared feedback sequence/overlay owners, local Joint traversal,
scene priorities, pass routing, and all eight sorts were rewalked. The focused
synthetic matrix covers multiple roots with attachments, two catch-up substeps,
the single effect-list pass, late materialization/export, equal-key stability,
and external-key reordering. Full build/test and harness results are recorded
in the session handoff.
