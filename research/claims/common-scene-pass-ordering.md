# Claim: common scene pass routing and ordering

- ID: `claim.presentation.common-scene-pass-ordering`
- State: active
- Maturity: reconstructed
- Confidence: high for pass planning, routing, sorting, and traversal; medium for the external pass/material boundary
- Owner: codex-root
- Coverage rows: `render.feedback_layering`; `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

The executable stably plans active external `BasePass` records by draw index,
routes each scene submission to the first matching pass, sorts that pass by one
of eight exact key orders, and submits configured pass ranges while traversing
draw indices 0 through 31; the concrete gameplay pass table and material state
are external resource data rather than executable constants.

## Anchors

- `game.exe @ RAM:006212c0, FUN_006212c0, BasePass parameter descriptors/defaults/enums, hash 13dd7b4d0cd77e39450efad59ffb8a1a267eb0dd4244d31f16d16503dcbd0f63`
- `game.exe @ RAM:00621b10, FUN_00621b10, active BasePass six-field copy, hash e9b21e2c41f543b64c16a9ecdfe52ce039f0a8a4ee8084167cb229169bca0dd4`
- `game.exe @ RAM:00621b60, FUN_00621b60, PassIndex accessor, hash 5a573619fbc3685c8bdfa00f0b3dd92853697079dc62f453ee842f73ce451560`
- `game.exe @ RAM:00647d30, FUN_00647d30, stable ascending PassIndex insertion leaf, hash c665fd201f8bea503e0fc98907e3096246779047154e4ebfbf26bf1d36cdd219`
- `game.exe @ RAM:00648d80, FUN_00648d80, active-pass planning and model/filter configuration, hash 674a772bd1d75cc3732cd37b01c95aa6a762704d0d53a4c6e2787a2a39a20f7b`
- `game.exe @ RAM:00648a90, FUN_00648a90, first-match Type/User/Range submission routing, hash 9f97abad88236c3ef7c4ae2dc4951bbb41f4560abe70190489689fa5a916518b`
- `game.exe @ RAM:00646de0, FUN_00646de0, double-buffered pass allocation/configuration, hash 6b35dfd77ac8bd0faa5578ee362554c19b506e3d528808777b1ecb4f301cd011`
- `game.exe @ RAM:00646980, FUN_00646980, 0x38-byte pass submission append, hash 114f553ad2880871527efbd0cc911577dc6640203b75b16553c18775c44aec52`
- `game.exe @ RAM:00646fd0, FUN_00646fd0, eight-way per-pass sort dispatch, hash 59d48bccc7632a6400f98f85045094a92dca5c9cd0d4dd5124fcd34a5739b945`
- `game.exe @ RAM:0064be60, FUN_0064be60, draw-index 0..31 pass-range traversal, hash d3c411478a88232a7205daed25eb9a2a4eab7a56b8b30f01e5ab77a3170cf6e0`
- `game.exe @ RAM:00646b00, FUN_00646b00, sorted pass submission loop, hash 3fbf5cadfd10d0b0d0b2dfd0f766173b5b5af436b48cf9fdf9e9d8864fac9527`
- `game.exe @ RAM:00646ac0, FUN_00646ac0, post-draw pass-count clear, hash 18fcf5825298e0566385d2863ce519877a8bbab126b4c214eb36b06bc446f56f`
- `game.exe @ RAM:00600830, FUN_00600830, immediate scene process/traverse/clear sequence, hash e1ac474f89f4610497a2b263f9d9776a3f11acb427e17a6c3794ce0af151be31`

## Observations

- `BasePass` declares `PassIndex` in 0..31, `Entry`, `Type` in 0..7,
  `User` in 0..15, `Sort` in 0..7, `Range` in 0..4, `RangeValueF32`, and
  `RangeValueU32`. Embedded labels identify the Type values as Opaque, Punch,
  Trans, 2D, Reduce, 3DAll, All, and OpaquePunch; the five Range labels are
  All, DepthFront, DepthBack, LayerFront, and LayerBack.
- Pass planning stably sorts graph objects by ascending `PassIndex`. An
  `Entry=false` object is omitted. Each survivor copies exactly Type, User,
  Range, Sort, and the two range values into one 0x18-byte routing record. The
  resulting internal pass index is registered with both the model pass list
  and the filter's first/last range for its external draw index.
- Type matching accepts equality, All for every submission, OpaquePunch for
  submission types 0 or 1, and 3DAll for types 0 through 2. User must equal the
  four-bit submission user. Range 0 always accepts; ranges 1 through 4 accept
  respectively `depth < F32`, `F32 <= depth`, `U32 <= layer`, and
  `layer < U32`. The first matching planned pass receives the record; later
  matches are not considered.
- A pass contains 0x38-byte submission records. Sort 0 and 1 use the same
  descending unsigned pair at offsets +0x34 then +0x30, populated by the
  backend material-sort callback. Sort 2/3 use float
  +0x30 descending/ascending. Sort 4/5 use unsigned 16-bit +0x14
  descending/ascending. Sort 6/7 use +0x14 ascending, then float +0x30
  descending/ascending.
- Sort leaves use insertion sorting for at most 32 records. Larger ranges split
  at `(count + 1) / 2` and use the executable's adaptive merge. Equal finite
  keys preserve submission order. Floating unordered comparisons are not
  guessed away: strict insertion comparisons and inclusive merge comparisons
  both return false on NaN, and the reconstruction preserves the binary's
  forward/backward merge choice.
- The filter visits every draw index from 0 through 31. For a configured
  first/last internal-pass range it calls the model draw loop for every pass in
  that range; the model walks the now-sorted submission records forward. The
  immediate scene path processes passes, traverses the filter, and then resets
  every pass end pointer to its begin pointer while retaining allocation.

## Reasoning

The BasePass descriptor and field-copy callback establish the meaning and
domain of each routing field. The stable PassIndex sort, model configuration,
and filter range writes connect graph order to internal pass order. The
first-match routing loop closes which submission enters which pass. The sort
switch plus all insertion/merge comparators closes record order, including
equal and unordered branches. Finally the 32-slot filter traversal, model draw
loop, and clear operation close the pass from configuration through one scene
draw cycle.

## Alternatives and falsifiers

- Competing explanation: note creation order alone is final layer order. The
  per-pass sort dispatch and sorted model draw loop contradict it except for
  equal keys retained by the sort.
- Competing explanation: Sort 1 is the inverse of Sort 0 because its label is
  `ShaderLower`. The switch sends both values to the same implementation in
  this snapshot.
- Evidence that would disprove this claim: a live bypass that submits gameplay
  note models outside ScenePassModule/SceneModelModule, a second active filter
  traversal order, or a material callback that reorders records after the
  model loop.

## Unknowns

- The external resource graph's actual gameplay BasePass instances and values.
- External model/material payloads that populate material-sort keys and hardware
  state beyond the identified submission fields.
- Exact depth-test, blend, shader, texture, and final pixel output selected by
  those external materials.

## Consequences

- Ghidra mutations: created the previously missed default function at
  `00621c40`; added compact plate comments at `006212c0`, `00621c40`,
  `00648d80`, `00648a90`, `00646fd0`, `0064be60`, and `00600830`.
- Spec sections: `spec/presentation.md` pass configuration, routing, sorting,
  and traversal.
- Reconstruction code: external pass descriptors, plan validation, matching,
  routing, and exact adaptive sorting in `include/chart/reconstruction.hpp`.
- Tests: `tests/scene_pass_ordering_test.cpp`.

## Verification

The focused test checks stable PassIndex planning, disabled-pass omission,
first-match capture, aggregate Type rules, all four range boundaries, malformed
external domains, every sort mode, equal-key stability, and the exact 32/33
record NaN threshold behavior. Build, focused/full tests, and harness validation
are recorded in the session note.
