# Claim: every sustain presentation path projects its endpoint with the endpoint SLA key

- ID: `claim.presentation.sustain-endpoint-sla-selection`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `parser.events`, `timing.clock`; viewer rows
  `time.playback_seek`, `render.hold`, `render.slide`, `render.air_solid`,
  `render.heaven_hold`
- Last reviewed: 2026-08-07

## Statement

Runtime presentation for parsed sustain types 1, 2, 9, 10, and 13 projects
each authored endpoint using that endpoint's independently selected SLA tag.
The endpoint does not inherit the root tag. For ordinary Slide, the persistent
endpoint resource also selects its width from the ending control, not the
preceding control.

## Anchors

- `game.exe @ RAM:011c1030, FUN_011c1030, integer-span SLA selector`
- `game.exe @ RAM:00c2adc0, FUN_00c2adc0, HOLD root/end key load`
- `game.exe @ RAM:00c2a440, FUN_00c2a440, HOLD presentation projection`
- `game.exe @ RAM:00c0ee40, FUN_00c0ee40, Slide endpoint projection`
- `game.exe @ RAM:00c12350, FUN_00c12350, AirLadder endpoint projection`
- `game.exe @ RAM:00b24200, FUN_00b24200, AirSolid root/end precompute`
- `game.exe @ RAM:00c16a90, FUN_00c16a90, AirSolid endpoint projection`
- `game.exe @ RAM:00b24b70, FUN_00b24b70, HeavenHold root/end precompute`
- `game.exe @ RAM:00c149a0, FUN_00c149a0, HeavenHold endpoint projection`
- `game.exe @ RAM:00b25510, FUN_00b25510, Slide generated width layout`
- `game.exe @ RAM:00c0f8b0, FUN_00c0f8b0, Slide endpoint lazy allocator`

## Observations

- HOLD load selects a key for the parsed root and again for the full parsed
  end triple. Its presentation update passes the root key with the root
  schedule and the end key with the end schedule into the same keyed-position
  wrapper before common projection.
- Ordinary Slide and AirLadder already retain one key per generated or authored
  endpoint and pass that key with the endpoint schedule during path update.
- AirSolid precompute selects the root key and a key for every authored
  endpoint. Its update loops the generated segment records and projects each
  segment end with the stored end key.
- HeavenHold precompute performs the same independent selection for root and
  every authored endpoint. Its update loops the generated presentation
  records and projects each end with the stored key before common projection.
- The Slide segment builder stores the decoded preceding width at generated
  `+0x10` and decoded ending width at `+0x14`. The guarded endpoint allocator
  passes `segment + 0x14` through the width decoder and into its resource
  selector. A shrink-point endpoint therefore uses the post-shrink width.
- These presentation reads are separate from the endpoint retry in lazy note
  materialization. The same parsed tags affect both construction eligibility
  and the eventual endpoint transform, but through distinct consumers.

## Reasoning

Each family has a closed producer-to-consumer path: the selector stores the
endpoint key in family-owned precompute/runtime state, and the presentation
update supplies that exact field together with the endpoint's schedule to the
keyed transform. The Slide allocator separately resolves the ending width
field by address, so footage preference is not needed to choose between start
and end width.

## Alternatives and falsifiers

- Competing explanation: types 1, 10, or 13 select endpoint keys only for
  materialization, while drawing inherits the root key.
- Evidence that would disprove this claim: a presentation call that supplies
  the root key with an endpoint schedule, a later overwrite of a stored end
  key, or a Slide endpoint selector reading generated `+0x10` rather than
  `+0x14`.

## Unknowns

- External resource rows, textures, materials, and final pixels remain absent.
  They do not change which chart key or decoded width reaches selection.
- Camera/viewport conversion after the common projected coordinate remains a
  separate open viewer row.

## Consequences

- Ghidra mutations: none.
- Specs: `spec/timing.md` and the HOLD, Slide, AirSolid, AirLadder, and
  HeavenHold note specifications.
- Reconstruction: `slide_generated_endpoint_decoded_width` plus existing SLA
  selectors and projection schedule helpers.
- Viewer: every sustain endpoint uses `endSlaKey`; Slide caps use `endWidth`.
- Tests: `tests/slide_path_test.cpp` and existing SLA/projection tests.

## Verification

The selector, family precompute/load path, family update call, common keyed
wrapper, and Slide width producer/allocator were inspected independently. The
five factory-reachable sustain types all have a resolved endpoint presentation
consumer; no additional primary sustain factory case was found.
