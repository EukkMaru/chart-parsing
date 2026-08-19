# Claim: AirHold uses root, checkpoint-pair, and projected-envelope resources

- ID: `claim.presentation.air-hold-model-path`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.air_hold`, `time.playback_seek`, `config.external_presentation`
- Last reviewed: 2026-08-18

## Statement

AirHold presentation consists of a width-scaled root resource, two identically
placed resources for every unresolved authored AHX checkpoint, and two resources
scaled across the current projected path envelope. Root family determines the
start anchor and ordinary/type-13 vertical origin. The path phase selects one
of three external resource pairs and an exact first-resource scale. Gameplay
and lifetime update before presentation in each scheduled tick; once both
component phases reach 5, the common deferred terminal request is made and the
presentation update stops.

The AHD/AHX parser consumes exactly six data fields. Any later corpus label,
including `DEF` or `PNK`, is an ignored extra and cannot select presentation.
A final AHD contributes to the envelope endpoint but creates no AHX resource
pair; the fixed envelope pair still exists independently.

## Anchors

- `game.exe @ RAM:00c21020, FUN_00c21020, constructor/default presentation state`
- `game.exe @ RAM:00c23f50, FUN_00c23f50, attached load and resource construction`
- `game.exe @ RAM:011c8870, FUN_011c8870, AHD/AHX field consumption and command-form flag, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:00c228e0, FUN_00c228e0, gameplay/lifetime update half`
- `game.exe @ RAM:00c229c0, FUN_00c229c0, resource transform/visibility update half`
- `game.exe @ RAM:00c24cd0, FUN_00c24cd0, ordered active wrapper`
- `game.exe @ RAM:00c23850, FUN_00c23850, two-component terminal predicate`
- `game.exe @ RAM:00b2b240, FUN_00b2b240, width table index`
- `game.exe @ RAM:00b2b290, FUN_00b2b290, native-width scale`
- `game.exe @ RAM:00b28680, FUN_00b28680, common Air vertical transform`
- `game.exe @ RAM:00b29760 / 00b28890, endpoint schedule selection and projection`
- `game.exe @ RAM:00c1b720, FUN_00c1b720, external resource offsets`
- `game.exe @ RAM:00b29b20, FUN_00b29b20, resource-animation visibility probe`
- `game.exe @ RAM:00c236a0, FUN_00c236a0, family maintenance/finalization`
- `game.exe @ RAM:00c23840 / 00c23a60 / 00c23770, preload family`
- `game.exe @ RAM:00c21910 / 00c21d70, destructor and deleting destructor`

## Observations

- Construction initializes the independent start and path phases, ordinary
  presentation value 1, unit resource scales, owned checkpoint/resource
  containers, and missing handle sentinels in the `0x2f0`-byte object.
- The parser's type-5 case validates access through the sixth data field,
  reads no later field, and stores only whether the command ID is AHX. Because
  event descriptor arity enforcement is disabled, later `DEF`/`PNK` strings
  are accepted and ignored rather than decoded as style.
- Attached load reuses the already reconstructed root-family anchor rules.
  Root types 0/4/6/11 use root start; type 1 uses root end; types 2/13 use the
  final path/control. Type 13 also copies its final control value into the
  vertical field. Other accepted roots retain value 1. The common transform is
  `(value - 1) * 3.8934999`, hence ordinary vertical zero.
- Decoded width selects index `clamp(width - 1, 0, 15)` in each external
  width-indexed table. Root lateral placement is the common center
  `4*lane + 2*width - 32`. Its lateral scale is `1` if the selected external
  resource reports native width below 1, otherwise `width/native_width`.
- Root load constructs two width-indexed resource families and two fixed
  resource families. Each authored AHX checker constructs two additional
  width-indexed resources. The checker count is saved AHX endpoints plus the
  final endpoint only when the final command is AHX. A final AHD therefore has
  no checkpoint pair, while the two fixed envelope resources are still
  constructed. The identities are external global tables; their selection
  and consumers are closed without assigning asset names.
- The resource wrapper provides three external offsets. Root placement is
  `(base lateral + offset X, base vertical + offset Y,
  projected start + offset Z)` with scale
  `(width/native_width, 1, 1)`. It is visible exactly while start phase is not
  5. The nonterminal path can therefore continue after the root resource hides.
- Every unresolved AHX checkpoint projects its own stored schedule/tag. Its
  pair shares one transform:
  `(base lateral + offset X, 15.724 + offset Y,
  checkpoint projection + offset Z)` and the same width/native-width scale.
  A resolved checker hides both resources; it is excluded from later envelope
  calculations.
- Six load-time attachment verticals are embedded:
  `{0, 15.574, 15.574, 0, 15.724, -15.574}`. These are resource attachment
  coordinates, not inferred visible asset extents.
- The active presentation initializes an envelope with projected start, then
  folds projected end and every unresolved AHX checkpoint using ordered
  min/max comparisons. It separately records whether authored end or any
  unresolved checkpoint is still in the future relative to manager current.
  If future unresolved path exists and start phase is already 5, it also folds
  the cached projection of raw position zero into the envelope.
- Both envelope resources use position
  `(base lateral + offset X + 0.0001, base vertical + offset Y,
  maximum projection + offset Z)` and projected scale
  `(maximum - minimum) * 0.25`. One has lateral scale 1; the other has the
  root's width/native-width scale. Both are made visible while the note is
  nonterminal.
- Path phase 3 selects the best-current-gap external pair at unit scale and
  also raises the shared field-feedback flag. Phase 4 selects the
  other-current-gap pair and scales its first external resource to 0.5. All
  other phase values select the default pair at unit scale. The second member
  of every pair stays at unit scale.
- Resource animation/state advancement uses the shared visibility predicate
  on the adjusted start schedule. With this call's flags, raw delta below 30
  succeeds immediately; farther values use the shared projected inclusive
  `[-550,550]` gate. A successful probe advances every present root, envelope,
  and per-checkpoint resource; it does not change authoritative judgement.
- The active wrapper calls the gameplay/lifetime half before presentation.
  The first half projects start, updates all three judgement streams, tests
  start phase 5 and path phase 5, and requests deferred terminal. The second
  half tests the same predicate and returns immediately when terminal, so it
  does not submit new transforms that tick. It does not explicitly hide the
  last resource state at terminal.
- Maintenance finalizes both resources for every authored checkpoint and the
  fixed resource set. Destruction additionally releases each owned checkpoint
  pair, containers, component state, and shared base.
- Preload init is a no-op. Steps 0 through 4 cover the width-indexed/fixed root
  and checkpoint model families; steps 5 through 8 cover four path/feedback
  families. Steps 9 through 14 are idle and the class reports ready after step
  14. Reset/finalize releases the preload objects and hides their views.

## Reasoning

The constructor, complete attached-load function, active wrapper and its two
direct callees, terminal virtual, maintenance, preload machine, full vtable,
and destructor were followed together. This separates gameplay phase writes
from their presentation consumers and prevents a phase-dependent resource
variant from being mistaken for a new judgement rule. External table addresses
are treated as selection evidence, not copied asset data.

## Alternatives and falsifiers

- Competing explanation: every AHX point contributes visible geometry after
  resolution. Falsifier: a later active path that re-shows the two resources
  after the observed resolved-state hide branch.
- Competing explanation: path phase 4 changes authored vertical geometry.
  Falsifier: a write from that branch to endpoint/root vertical state rather
  than the observed external-resource selector and 0.5 scale.
- Competing explanation: terminal update hides or destroys resources
  immediately. Falsifier: an AirHold-reachable hide/destruction edge before
  the maintenance/removal path rather than the observed early return.
- Competing explanation: trailing `PNK` selects a pink final-AHD resource.
  Falsifier: any type-5 parser or AirHold load read of token index 7 or later,
  or a resource-table branch on retained trailing text.

## Unknowns

- External resource identities, native widths, intrinsic geometry, colors,
  materials, animation contents, and texture roles remain unavailable runtime
  data. Selection sites, fallback, scale, placement, and consumers are exact.
- Shared final camera/viewport mapping, cross-family ordering, and blend
  composition remain open in shared viewer rows. `render.air_hold` is therefore
  partial even though the family-local path is closed.

## Consequences

- Ghidra mutations: supported plate comments at `00c21020`, `00c23f50`,
  `00c228e0`, `00c229c0`, `00c24cd0`, `00c23850`, `00c236a0`, `00c23a60`,
  `00c23770`, and `00c21910`.
- Spec: `spec/notes/air_hold.md`.
- Reconstruction: AirHold resource selector, visibility, transform, envelope,
  phase-variant, and feedback helpers in `include/chart/reconstruction.hpp`.
- Tests: `tests/air_hold_presentation_test.cpp`.

## Verification

Normalized opcode hashes for principal anchors:

- `00c21020`: `2732f918cdadbf6cfc21562968c6667353e4c39c70d12dbe8a40ccbff8bc4dd2`
- `00c23f50`: `7f1bfe8d8c2283e695e9c036f05e590860b660caa48caec874876532e2e6fd2d`
- `00c228e0`: `51dfd6cbedbdc2a5585b787ab71009ce8905ab2431c12d3ad8f616c3dde36edc`
- `00c229c0`: `3d0318ae8d98c745d7bc0a45b4b0466bd68b53cacadb873a3efd7afaec5fb8d5`
- `00c24cd0`: `22d52cd71f40ed597fdb6b20ec118a0ab5df7850419e1e9ac83833115130e8d9`
- `00c23850`: `8036cb802baf8ad9af863ae887e3af4a5f949e7c8feb92d2af9983c09782ec63`
- `00c236a0`: `5daed0d6c6c17778d8c562abf0522da8a09552b328c28a3c5d8201aafc7aa4a6`
- `00c23a60`: `0b6abad16271d21f28bb35a223ff9f5800402a868deeab955f46df2c14687afa`
- `00c23770`: `b3d22ae27c3edc1db2a37d209a5c1688e959f31dc17db6dcef12adbb6ff1259d`
- `00c21910`: `584d363b74466c0be28f713ff0e238339eb1a9bd48884342fc9d7006d8d56f3c`
- `00b2b240`: `f22c328502800ac1ab302479077362561e130ed59c980d1a85b50d3c02d0b354`
- `00b2b290`: `6f0c54e3427cff15ccc090a3c3770e29592b9d052ef7db880a02976386a3f53f`
- `00b29b20`: `e6cd1e0dbb99a0e1a1e478ec2683a0b6ba53c652aeaf125e2a140be97f768823`

Focused tests cover width indexing, root/checkpoint visibility, terminal update
suppression, all three phase variants, field-feedback gate, exact attachment
constants, external offsets, native-width fallback/ratio, unresolved-only
envelope folding, future/judgement-plane inclusion, lateral bias, and the two
envelope lateral scales. Judgement tests additionally cover six consumed data
fields, ignored later fields, and final-AHD versus final-AHX checkpoint count.
