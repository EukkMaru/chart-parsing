# Claim: every blocker from the failed independent review has a reproduced correction

- ID: `claim.audit.primary-closure-corrections`
- State: superseded
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `audit.closure`
- Last reviewed: 2026-08-03

## Statement

Superseded on 2026-08-03 first by
`claim.audit.primary-ald-checkpoint-correction` and then by
`claim.audit.primary-ald-precalc-correction`. The listed earlier corrections
remain historical facts, but the conclusion that no known gameplay path
remained open was falsified by the two type-9 ALD contradiction traces.

The primary investigator reproduced every concrete blocker and open path named
by `claim.audit.independent-closure-review` against the exact local binary,
then corrected the claims, normative specs, clean-room C++, and focused tests.
No known gameplay path remained open after those corrections. The subsequently
completed independent post-fix review verifies that conclusion in
`claim.audit.independent-post-fix-closure-review`.

## Anchors

- `game.exe @ RAM:011c7d30 -> 011c6720/011c58f0 -> 00b29c90, projection schedules and SLA materialization`
- `game.exe @ RAM:011c8870 -> 00c1d1d0 -> 00c18800, event fields and HXD checker selection`
- `game.exe @ RAM:00b2b690 -> 00da62c0/00b28cc0, construction, candidate, and result order`
- `game.exe @ RAM:00b25510 -> 00b1f0f0 -> 00c0de10, Slide generated endpoints and runtime consumption`
- `game.exe @ RAM:00b94a60 -> 00b94ac0/00b93ba0, selected SkillBefore controls and result remap`
- `game.exe @ RAM:011bda60, Air-family and type-13 generated-path domains`
- `game.exe @ RAM:00da06c0 -> 00b2b4f0/00b2a8c0, materialization and forced-mode setup producers`
- `game.exe @ RAM:00ce9270, sole reachable nonzero forced-mode writer`

## Observations

- The false presentation-only SLA conclusion is preserved as superseded
  history. STP/SFL/SLP/DCM parsing, ownership, ordering, schedule arithmetic,
  and SLA-keyed materialization are reconstructed end to end.
- HXD's extended flag, exact optional subtype defaults, runtime copies, and
  selector-4 judgement-window override are reconstructed across legacy and
  current corpus forms.
- All 45 live group-3 command spellings, temporary last-write behavior,
  malformed/default conversion, and mandatory destructive overwrite before
  gameplay return are normative.
- Dynamic primary/secondary construction order, complete candidate-producing
  family fanout, equal-edge nonconsumption, and same-pass terminal result
  rerouting are explicit and tested.
- Common encoded width, bounded lanes, HOLD end geometry, and all three
  parsed-to-runtime result-component identifiers have assigned consumers.
- Slide endpoint-profile selection, global/segment/lane interpolation, marker
  propagation, complete copied record width, sustain-marker ownership, and
  missing-key out-of-range behavior are reconstructed.
- Active-result control load/reset lifetime, first-unit precedence, validity
  bounds, and all twelve direct family callers are closed.
- The compiled BPM sort including equal partition and heap fallback is
  reconstructed. Empty maps, zero/negative/nonfinite schedule arithmetic, and
  nonprogressing adaptive-Air domains are explicit rather than normalized.
- Materialization speed and base offset have exact option/skill/configuration
  sources, defaults, bounds, narrowing, and setup/reset ownership.
- Whole-program owner/write/address-escape closure leaves only forced modes 0
  and tutorial-produced 2 reachable. The companion and counter stay at their
  reset behavior; cycling and RNG branches are dormant.
- Periodic terminal-rule gating is a deterministic one-shot bitset, not
  randomness. One selected SkillBefore record supplies every contribution,
  promotion, negative, configured-terminal, and result-remap control.
- AirHold and HeavenHold use wrapped deltas with unsigned bounds, so high-bit
  deltas reach pathological large-span generation regardless of signed
  endpoint ordering, while a signed wrap-boundary crossing can remain small.
  AirSlide ordinarily uses a signed no-interior-sample path for decreasing
  controls, but wrapped positive-to-negative cursor advances and zero steps
  have separate nonprogress/expansion dispositions. Slide's checked path-map
  failure is also explicit.

## Reasoning

Each correction follows a continuous producer-to-consumer path and has a
falsifier stated in its owning claim. The corrections do not infer missing
resource values: they recover selection, default, lifetime, and consumers and
leave only externally absent numeric rows as parameters. Superseded claims
remain available as contradiction history but are not cited by active
coverage.

## Alternatives and falsifiers

- Competing explanation: one failed-review item was only renamed or
  documented without reconstructing its executable behavior.
- Evidence that would disprove this claim: a listed corrective claim lacking a
  source producer, gameplay consumer, reset/error path, normative spec, or
  focused test; an active reference to the superseded SLA conclusion; or a
  fresh reviewer reproducing an omitted gameplay edge.

## Unknowns

- External judgement/profile/table values absent from the snapshot remain
  explicit inputs. Their interfaces are closed and they do not block
  reconstruction logic under `docs/COMPLETION.md`.
- This same-investigator correction record did not predict the required
  post-fix independent verdict; the separate read-only review has now supplied
  it in `claim.audit.independent-post-fix-closure-review`.

## Consequences

- Ghidra mutations: none for this aggregate record; owning claims document any
  earlier supported annotations.
- Corrective claims include
  `claim.parser.sla-materialization-selection`,
  `claim.timing.projection-schedule-materialization`,
  `claim.note.hold-extended-profile-selection`,
  `claim.parser.derived-command-overwrite`,
  `claim.interactions.cross-family-candidate-result-order`,
  `claim.parser.common-lane-width-encoding`,
  `claim.judgement.result-component-identifier-flow`,
  `claim.note.slide-path-sustain-judgement`,
  `claim.judgement.active-tier-zeroing`,
  `claim.timing.tempo-measure-schedule`,
  `claim.configuration.runtime-materialization-input-producers`,
  `claim.judgement.forced-result-mode`,
  `claim.configuration.skill-before-gameplay-control-loading`, and the
  Air-family generated-path claims.
- Reconstruction code: linked helpers in
  `include/chart/reconstruction.hpp`.
- Tests: the 34-target CTest suite plus corpus and coverage validation.

## Verification

`python3 scripts/harness.py validate` passes with 28 rows. The local inventory
contains 7,752 readable charts across eight declared versions: 87 registered
spellings are present, four registered spellings are absent, and 24 legacy
metadata spellings are rejected before dispatch. A full CMake build and all 34
CTest targets pass after the corrections.
