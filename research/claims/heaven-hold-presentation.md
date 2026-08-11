# Claim: HeavenHold renders one authored-control mesh with separate root/body lifetime

- ID: `claim.presentation.heaven-hold-authored-mesh`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `parser.events`, `state.ownership`,
  `audit.indirect_calls`; viewer rows `render.heaven_hold`, `render.slide`,
  `render.air_ladder`, `time.playback_seek`
- Last reviewed: 2026-08-10

## Statement

Every factory-reachable `projView::HeavenHoldNote` owns one start resource and
one body resource. Its body mesh is built from the authored root and every
authored `0x24` endpoint, not from the generated judgement queue. HHD/HHX token
10 selects endpoint mirroring and a geometry mode through two executable-owned
tables. Retyped HLD and zero/`NON` ALD retain selector zero. The HLD rewrite's
separate integer 10 is consumed as path scalar `1.0`, not as that selector.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, HHD/HHX schema, chaining, selector and command-form fields, hash c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:011c54f0 -> RAM:011cfed0, HHX token-11 exact eight-entry decode, hashes 5dd1725fafa5661eb8917cb8041b65c56351f28cfc66c1d26702f63da875415a and 999a3648bfd8ee35088e6b68434f9b3a4d06467af4d45fe4160edabef93e71cc`
- `game.exe @ RAM:00da3710 -> RAM:011c3680, parsed-record construction and +0xb0 zero default, hashes 294ca727f8b2634e6b7b587928c2f9cc5872cbcdad96968a47dead3f5d386bb9 and 5bb15d0ebae9a07c166c0ad38258451c1bf4fc3384a6170e241b87ee0c7c66c7`
- `game.exe @ RAM:011c4af0, FUN_011c4af0, HLD/zero-NON retyping and root/control scalar writes, hash 17a970c5253bf29358a26ef8c82a1ab47b8beb3edfee363255fc55135de57127`
- `game.exe @ RAM:00b24b70, FUN_00b24b70, authored precompute and geometry descriptor, hash 763d61bf88deedba10e937f0da2e059b5ac445aa52c8e896af792ee80d16ab07`
- `game.exe @ RAM:011cbbe0 and RAM:011cbbb0, +0xb0 mirror/mode selectors, normalized hashes fe1286ea8b693ad23514a92dce718aec8fc0ba5231146e99a8704f6649215e6f`
- `game.exe @ RAM:00c15cd0, FUN_00c15cd0, precompute lookup and two-resource load, hash c50031aaae7a2cbe7ca4f58358625460d71a077381717e3eaa9cc6a3b60a0306`
- `game.exe @ RAM:00c149a0, FUN_00c149a0, endpoint projection, phase modes and visibility, hash 55c4af0cd9eaad714ecae4f9d8096940bdfbdd472ecbfeaf5ab367610bc01c85`
- `game.exe @ RAM:00c08420, FUN_00c08420, one-stream mesh transform, clipping and emission, hash 2c9d1425d6262aa157c14519a9b7e342e2e8816ffa1f0abf1a6aa2afc7425e2a`
- `game.exe @ RAM:006ddc30, FUN_006ddc30, 0x18-byte vertex append, hash 3cac7fa29640e1cac62b72547ad7306252a8970b411aa3f68da28de8819c993f`
- `game.exe @ RAM:00c145d0 -> RAM:00c154a0, body position and authored-span locator, hashes 08e69d5a489bee292a2ac371f3d6cf38bba49bd8a8ebd35b72f5a5e204788491 and a47de2e5ceb3c3c89b7dc3c678285284235260c850d89cf1f1f577180a2d06e3`
- `game.exe @ RAM:00c152a0 and RAM:00c13bc0, resource/precompute reset and destruction, hashes 3b8642047fc716ddc485e052a98ff84bb244f6d852606f392f47df7bc2b728fc and 31390d040986cb39ecf2ebeca812baeb5430579e074490a089e939787a7527b4`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, attached-secondary body-resource selector, hash d2e5865736dbc63e4e92294cfabd119e14c26dfee62a143e57f90f77d955b449`

## Observations

- HHD/HHX use an eleven-data-field layout before HHX's optional extra field:
  root position/lane/width, a root scalar rounded to tenths, duration, ending
  lane/width, an ending scalar rounded to tenths, and integer selector. HHX
  sets the existing command-form byte and decodes its extra field through the
  exact `UP/DW/CE/RC/LC/RS/LS/BS` table; HHD leaves that byte clear.
- Direct type-13 chaining requires the same command-form family, decoded HHX
  extra value when present, exact selector, selector parity, and matching last
  endpoint position/lane/span/scalar. A compatible record appends one `0x24`
  control rather than creating another runtime note.
- Parsed-record construction initializes `+0xb0` to zero. The HLD rewrite
  writes integer 10 only to root `+0x30` and control `+0x18`, then changes type
  2 to 13. The HeavenHold precompute divides those fields by ten, so an HLD
  path receives scalar `1.0`. Neither the rewrite nor the zero/`NON` ALD path
  writes `+0xb0`; both therefore use selector zero.
- `FUN_011cbbe0` maps selector values 0..4 to mirror flags
  `[0, 1, 0, 1, 0]` and returns zero outside the table. A set flag reflects
  every authored endpoint span about the root lane center before precompute.
  `FUN_011cbbb0` maps 0..4 to geometry-mode bytes
  `[0, 0, 1, 1, 0x83]` and returns `0x83` outside the table.
- Precompute emits one `0x34` span per authored `0x24` control. Each span
  carries start/end schedules, lane spans, independently selected SLA keys,
  decoded-width-derived centers, tenths path scalars, and boundary flags. A
  second vector contains root plus every authored endpoint as decoded width,
  lane center, and path scalar. The generated judgement vector is not read.
- Runtime load copies the accepted-event identity, performs a checked
  precompute lookup, copies the generated judgement queue separately, and
  initializes one root resource plus one body resource hidden. The root
  resource selects its ordinary/extended family from the command-form byte.
  The body selects its second family only when the primary owns attached
  secondary type 3, 5, or 8; this selector is not the HHD/HHX command form.
- Presentation projects the root and every authored endpoint with that
  endpoint's stored SLA key. Root visibility is `start_phase != 4`; body
  visibility is `path_phase != 4`. Path phase 2 selects presentation mode 1.
  Otherwise a completed start selects mode 2 only for path phase 3 and mode 1
  for the other nonterminal states; all earlier states use mode 0.
- The shared builder rejects mismatched point cardinalities. It derives one
  `0x44` draw span per adjacent point. When a nonzero geometry mode crosses
  raw-relative zero, it removes earlier spans and inserts the interpolated
  origin. It then reverses the surviving sequence and reflects projected,
  lateral, vertical, and resource-coordinate fields about the first/final
  midpoint while reversing, but not reflecting, raw-relative values and
  decoded widths.
- The builder clips projected coordinates and all continuous geometric fields
  to `[-600, 50]`, preserving raw-relative values. Every enabled span emits one
  single-sided quad: exactly six `0x18`-byte vertices. Lateral half extent is
  decoded width multiplied by two render units, which equals the authored lane
  boundaries under the common four-units-per-lane transform. There is one
  dynamic primitive submission, not Slide's or AirLadder's three streams.
- Presentation modes 0 and 1 use exact base white `0xffffffff`. Mode 1 writes
  scale
  `sin(fmod(counter * 0.05, 1) * 2*pi) * 0.25 + 1.5`. Mode 2 selects a second
  exact alternate gray `0xff666666`. Resource identities, textures, and final
  pixels are not present as chart constants. The packed colors are closed
  executable startup constants.
- Reset hides both resources and releases the accepted-event precompute
  reference; destruction repeats guarded precompute release and destroys the
  owned queues/resources.

## Reasoning

The direct parser, constructor defaults, and HLD rewrite separate three fields
that were previously conflated: command form, path scalar, and selector. The
precompute's exact offset reads then identify each consumer. The accepted-event
identity closes precompute ownership from producer to runtime lookup; matching
vector cardinalities close root/control geometry to mesh emission. Independent
phase reads and reset paths close both resource lifetimes.

## Alternatives and falsifiers

- Competing explanation: HLD's rewritten 10 selects mode `0x83`, or generated
  judgement samples define the body ribbon.
- Evidence that would disprove this claim: an HLD writer to parsed `+0xb0`, a
  precompute point sourced from generated `+0x158`, a second body submission,
  or a root/body visibility path that bypasses the two component phases.

## Unknowns

- The external root/body resource identities, material and
  texture contents, resource intrinsic scale, shaders, and final camera/pixel
  composition remain unavailable product data. Their exact selector branches
  and consumers are closed.
- The chart-only viewer has no cabinet input. It uses deterministic no-input
  lifetime clipping; exact phase-driven mode transitions remain represented in
  the clean-room API for a future gameplay-state simulation.
- HHD/HHX have no local corpus instances, so their exact parser and mesh paths
  are verified structurally and with synthetic tests, not corpus playback.

## Consequences

- Ghidra mutations: supported-role plate comments at `00b24b70`, `00c08420`,
  `00c145d0`, `00c149a0`, `00c152a0`, `00c154a0`, `00c15cd0`, `00c158f0`,
  `00c16340`, `011cbbb0`, and `011cbbe0`; default symbols/types retained.
- Spec sections: `spec/c2s.md`, `spec/notes/heaven_hold.md`, and
  `spec/notes/slide.md`.
- Reconstruction: selector tables, authored path construction, origin trim,
  nonzero-mode reflection, clipping, six-vertex mesh, resource selectors,
  phase visibility/mode, and animation scale in `include/chart/reconstruction.hpp`.
- Viewer: HHD/HHX schema and chaining plus normalized HeavenHold presentation
  for direct, HLD-styled Slide, and zero/`NON` ALD origins.
- Tests: `tests/heaven_hold_presentation_test.cpp` and existing HeavenHold/SLA
  tests.

## Verification

The parser schema/defaults/rewrite were inspected independently from the
precompute and runtime consumers. Precompute point and segment cardinalities
were cross-checked against the update arrays and mesh-builder guards. The mesh
append target and all six call sites in both winding branches were inspected
separately. Focused tests cover every selector table entry and fallback,
endpoint reflection, HLD scalar conversion, zero crossing, nonzero-mode
reflection, both clip limits, exact vertex count/layout, phase modes,
visibility, and the attached-secondary resource selector. All 36 CTest targets
and the 50-chart browser audit pass; full-corpus audit is recorded by the
session handoff.
