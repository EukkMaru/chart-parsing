# Claim: the second ALD identity, terminal, and presentation contradiction is corrected

- ID: `claim.audit.primary-ald-precalc-correction`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `audit.closure`
- Last reviewed: 2026-08-03

## Statement

The primary investigator reproduced and corrected the second exact-binary
contradiction in the AirLadder slice. Parsed `+0x84` is an accepted-event
identity rather than an external tuning key; the terminal boundary is the
authored precompute's final schedule rather than an externally configured
threshold; authored controls rather than generated checkpoints feed the main
three-stream geometry. Generated checkpoints still own the judgement checkers
and their individual presentation effects. Every active dependent artifact
now uses that separation. This same-investigator correction deliberately does
not reassert independent stage-one closure.

## Anchors

- `game.exe @ RAM:011c7980 -> RAM:011c8870, accepted-event identity production/storage, hashes b72ab41d5af91578014761c4671a625a0505d482c4cb2271ecd6e5563932da34 and c634f8b3db46cfaaef0e2836a603435b1bce70cc8dd4980f939da3efdb24e97b`
- `game.exe @ RAM:00b264e0 -> RAM:00b236a0 -> RAM:00b23470, keyed type-9 precompute construction/fill/lookup, hashes ca9889f09ac061485f9e4547783966d62972986c2a25004924d1563603d98acb, e4793bf2a9246ddbe30bb6a6e56d30e903dc19fba6fa63e220eedd48908bbf3c, and f0ee8059e6a782275f7157204f998c77b6497574a16fc06ca97551bfb3753ee6`
- `game.exe @ RAM:00c132f0 -> RAM:00c12d80, runtime precompute copy and exact terminal guard, hashes 2d7edc23ab6a2229647e9f6d2a9e3061e2ad6205607aa8bce11a531d69d6340c and b267c18c2b3ab7640df3fc02354fb00cbb46328ad150432c449aa2e29574b516`
- `game.exe @ RAM:00c12350 -> RAM:00c03c00, authored main geometry and generated-effect update split, hashes bc494d630f8811a52cfffd3252d7358558fb99e73a1cc951c33814f2dd88a458 and d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`

## Observations

- The superseded primary ALD correction remains preserved. Its generated
  judgement cardinality is valid, but its external terminal-key and generated
  main-geometry premises are not active evidence.
- `claim.note.air-ladder-precalc-presentation` closes the identity, authored
  end schedule, precompute lifetime, authored geometry, exact clipping and
  neutral vertex generation, plus generated-effect resource-independent
  transform.
- Normative configuration, judgement, and AirLadder specs no longer expose an
  external AirLadder completion threshold or generated main-path premise.
- Clean-room helpers and focused tests encode the exact finite/unordered
  terminal predicate, authored path, clipping, three stream layouts, and
  generated-effect slot/transform rules.
- The viewer keeps authored and generated paths separate and labels colors,
  camera, unavailable resource/material semantics, and missing result-state
  simulation as open or product-owned rather than binary facts. The later
  trace closes Joint submission order without promoting it to final pixel
  compositing.

## Reasoning

The same accepted-event ordinal reaches parsed storage, precompute insertion,
runtime copy, and checked lookup. The precompute independently walks the
authored-control vector and stores the final authored schedule consumed by the
terminal predicate. Runtime constructs generated checker/effect records only
after that lookup, while the main presentation call receives root plus authored
endpoints. These disjoint continuous paths falsify all three former premises.

## Alternatives and falsifiers

- Competing explanation: the changes only rename fields while the external
  threshold or generated geometry remains active elsewhere.
- Evidence that would disprove this claim: an active normative artifact or
  implementation still sourcing AirLadder completion from an external table,
  feeding generated samples to the main type-9 path, or using a key unrelated
  to the accepted-event identity.

## Unknowns

- `audit.closure` still requires a fresh independent contradiction audit; this
  primary correction cannot audit its own search completeness.
- External presentation resources are excluded from clean-room output.
  Resource semantic roles and final pixel compositing depend on their external
  contents. Product integration of the now-closed checkpoint expiry predicate
  remains open in `render.air_ladder`; Joint traversal and default command
  submission order are closed. The formerly anonymous authored float is now
  closed as the independently selected endpoint SLA projection key.

## Consequences

- Ghidra mutations: supported-role plate comments at `00b264e0`, `00b236a0`,
  `00b23470`, `00c132f0`, `00c12d80`, `00c12350`, `00c13050`, `00c03c00`,
  `00bfec90`, `00c033a0`, `00d7e550`, `00d7e350`, `0060ada0`, `0066dbc0`,
  `00c322b0`, `00b2d160`, `00c331b0`, `010f8a40`, `00c196f0`, and
  `00c18270`; no symbols or types were changed.
- Spec sections: `spec/configuration.md`, `spec/judgement.md`, and
  `spec/notes/air_ladder.md`.
- Reconstruction code: exact AirLadder terminal, authored geometry, clipping,
  neutral streams, and effect-transform helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/air_ladder_judgement_test.cpp` and
  `tests/air_ladder_presentation_test.cpp`, plus the full suite.

## Verification

`harness.py doctor` confirms the exact binary/project and healthy MCP;
`harness.py validate` passes. The normal build and all 35 CTest targets pass.
A coverage-spread smoke suite loaded and rendered 15 local ALD charts with zero
headless failures, and four distinct outputs were inspected. That smoke suite
does not promote any explicitly open presentation semantic to binary fact.
