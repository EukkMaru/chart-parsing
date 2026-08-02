# Claim: one selected SkillBefore record owns all gameplay control vectors

- ID: `claim.configuration.skill-before-gameplay-control-loading`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `judgement.types`, `state.ownership`, `config.external`,
  `interactions.cross_note`
- Last reviewed: 2026-07-27

## Statement

The result owner caches a three-integer selected identity, looks up the middle
ID in the ordered `SkillBefore` record map after any identity change, and routes
source units of types 0, 1, 2, 3, and 5 in source order into the five gameplay
control vectors. Reset, a missing record, type 4, and unknown unit types add no
control behavior.

## Anchors

- `game.exe @ RAM:00b94a60, FUN_00b94a60, identity-change gate, hash 1b591dc0b83153dfacca1a57359d854249f37f2a06701318974c3018b95972da`
- `game.exe @ RAM:00b93f90, FUN_00b93f90, all-vector reset, hash f9f1eceb8064b2c1c4ddb372164ee212443756944534477043ed7964f157d830`
- `game.exe @ RAM:011f8940, FUN_011f8940, ordered SkillBefore record map, hash 2ed9ec603c657ae774c451b95eea255f69e5c244aaaf99150a06160acbbcbd62`
- `game.exe @ RAM:00b94ac0, FUN_00b94ac0, record selection and unit routing, hash 8a805dd6e033a3be52443714a46f34384795f529b2524b286aeb1977dcc620fa`
- `game.exe @ RAM:008da420, FUN_008da420, selected metadata copy, hash e5d70eb405ef6ef0effb3d4da6319a56f98c6c698548cb4066f84120d5d0ec34`
- `game.exe @ RAM:00b93180, FUN_00b93180, type-0 contribution consumer, hash d26d8367132b48ad03a297b58bb0ac2112bfb80c49e7be2259ce8665bfa4445e`
- `game.exe @ RAM:00b92d30, FUN_00b92d30, type-2 negative-adjustment consumer`
- `game.exe @ RAM:00b92640, FUN_00b92640, type-1 promotion and type-3 terminal consumers`
- `game.exe @ RAM:00b93ba0, FUN_00b93ba0, type-5 result-remap consumer, hash c6a2236cbca7f998527eb863064d2cf0bb6a5faff609318923094d18872fde85`

## Observations

- Setup compares all three selected identity integers with cached owner fields.
  Exact equality retains every vector and its progress state. Any difference
  first resets cached metadata, progress fields, and all five vectors.
- Rebuild searches the ordered record map by the identity's middle integer.
  A missing key leaves the freshly reset owner empty. It does not install
  guessed defaults or preserve controls from the prior identity.
- A found record copies its shared metadata and walks its units in source
  order. Exact type routing is:
  type 0 `SkillGaugeAssistData`, type 1 `SkillGaugeKeepData`, type 2
  `SkillDamageGuardData`, type 3 `SkillDeathPenaltyData`, and type 5
  `SkillChangeJudgeResultData`. Type 4 and all default values are ignored.
- Each routed unit is appended to its dedicated owner vector. Its consumer
  therefore observes the selected record's order. Type 0 supplies common
  contribution rules, type 2 the event-only negative vector, type 1 the common
  promotion vector, type 3 configured terminal rules, and type 5 result-tier
  remapping.
- Reset also clears the one-shot/progress state shared by the rule consumers.
  A same-identity setup deliberately does not reset those fields.

## Reasoning

The identity comparison, reset-before-lookup order, ordered-map key, complete
unit switch, five append destinations, and five downstream consumer loops
close which external rule record can affect a session and what an absent or
unsupported unit means. The external numeric contents remain parameters, but
there is no untraced terminal-table selector or implicit default vector.

## Alternatives and falsifiers

- Competing explanation: terminal vectors come from independent tables or
  accumulate across selected skills.
- Evidence that would disprove this claim: a second population path, a vector
  append before the identity reset, or a consumer reading a sixth owner vector.
- Competing explanation: unit type 4 selects a hidden terminal rule.
- Evidence that would disprove this claim: a case-4 append or later indirect
  dispatch from the ignored unit.

## Unknowns

- External unit values, labels, and player-facing skill names are absent and
  remain parameters at their exact consumers.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/configuration.md`, `spec/judgement.md`.
- Reconstruction code: `SkillBeforeControlIdentity`,
  `skill_before_controls_require_rebuild`,
  `skill_before_control_lookup_key`, and
  `route_skill_before_gameplay_unit` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp`.

## Verification

The identity cache, missing-map path, reset order, source vector, all switch
cases, append destinations, and downstream vector consumers were checked
separately. Focused tests cover equality, each changed identity component, the
middle lookup key, all five routed types, type 4, and unknown values.
