# Session 2026-08-10: HeavenHold presentation

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `parser.events`, `state.ownership`,
  `audit.indirect_calls`; viewer rows `render.heaven_hold`, `render.slide`,
  `render.air_ladder`, `time.playback_seek`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Close the factory-reachable HeavenHold presentation path for direct HHD/HHX,
HLD-styled Slide, and zero/`NON` ALD: exact parser fields and chaining,
precompute ownership, authored versus generated geometry, selectors, resource
families, mesh construction, visibility, phase modes, clipping, submission,
reset, and destruction. Implement only the resource-independent result.

## Findings

- `claim.presentation.heaven-hold-authored-mesh` is normative. HeavenHold owns
  one root resource and one authored-control body stream. It emits one
  six-vertex `0x18`-vertex quad per surviving span and does not use generated
  judgement samples for body geometry.
- HHD/HHX have a distinct schema; they are not HOLD-tail aliases. Their root
  and ending scalars are stored in tenths, duration precedes the ending span,
  token 10 is a presentation selector, and HHX alone decodes token 11 through
  the exact eight-entry feedback table.
- The proof pass corrected a prior field conflation. HLD retyping writes
  integer 10 to root `+0x30`/control `+0x18`, which precompute consumes as path
  scalar `1.0`. It does not write presentation selector `+0xb0`; retyped HLD
  and zero/`NON` ALD retain the constructor's zero selector.
- Direct selector values 1/3 mirror authored endpoints about the root center;
  values 2/3 select nonzero mode 1; value 4 and out-of-range values select
  nonzero mode `0x83`. Nonzero geometry trims at a raw-zero crossing, then
  reverse-reflects the surviving path about its first/final midpoint.
- The viewer now normalizes all three origins into the HeavenHold path, parses
  and chains direct HHD/HHX, excludes HLD from ordinary Slide caps/streams,
  renders the authored single stream, and preserves endpoint-specific SLA
  projection. Asset styling remains explicitly original/provisional.

## Ghidra mutations

Added supported-role plate comments at `00b24b70`, `00c08420`, `00c145d0`,
`00c149a0`, `00c152a0`, `00c154a0`, `00c15cd0`, `00c158f0`, `00c16340`,
`011cbbb0`, and `011cbbe0`. The selector comments were corrected in-session
after producer-offset verification distinguished `+0xb0` from the HLD scalar
write. Default symbols and types were retained.

## Validation

- `python3 scripts/harness.py doctor` passed exact binary identity, project,
  tools, socket, and live Ghidra project checks at session start.
- `python3 scripts/harness.py validate` passes 28 coverage rows and 15 required
  files after the claim/spec/ledger update.
- Normal CMake development build completed; CTest passes 36/36, including the
  new `chart.heaven_hold_presentation` target.
- Focused tests cover selector tables and fallbacks, endpoint mirroring, HLD
  scalar conversion, raw-zero trimming, nonzero-mode reflection, both
  projected clip bounds, exact vertex size/count/winding, phase visibility and
  modes, animation baseline, and the attached-secondary body-resource family.
- The actual browser parser passed a 50-chart smoke and then all 7,752 local
  charts with zero parse errors, unknown records, rejected associations, or
  harness errors. This includes the corpus-backed HLD and zero/`NON` origins;
  direct HHD/HHX remain synthetic because the local corpus has none.

## Unresolved and contradictions

- The earlier interpretation that HLD's rewritten 10 selected geometry mode
  `0x83` is disproved. Constructor/default, rewrite destination, and precompute
  consumer offsets show that HLD selector `+0xb0` remains zero. All active
  claims/specs/code touched by that interpretation were corrected.
- Root/body resource identities, resource intrinsic
  scale, textures, materials, shaders, blend/depth state, and final pixels are
  external product data. Their source/selector/consumer boundaries are closed;
  their contents are not guessed or copied. Superseded 2026-08-10: the two
  vertex colors are exact startup constants `0xffffffff` and `0xff666666`;
  see `claim.presentation.shared-static-colors`.
- The chart-only viewer does not simulate cabinet input, so it cannot yet
  drive exact start/path phase transitions. The clean-room API contains the
  binary phase visibility and base/animated/alternate mode rules for later
  logical-state integration.
- HHD/HHX have no local corpus instances. Their parser path and presentation
  are exact-binary-backed and synthetically tested, but require owner-provided
  input or a future arbitrary chart for human visual review.

## Handoff

No Ghidra writer ownership remains active. The next root-owned Ghidra task is
`pipeline.presentation_boundary`: inventory every presentation factory,
manager, concrete vtable, callback, shared transform, resource loader, and
reset owner, then use that map to order the remaining Tap/Hold/Flick/Mine/Air/
AirHold/AirSlide/AirSolid vertical slices. Keep the independent closure audit
fresh and deferred until those stage-two rows and binary saturation are done.
