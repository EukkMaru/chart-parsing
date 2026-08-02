# Claim: active result-control state can replace a TAP tier with zero

- ID: `claim.judgement.active-tier-zeroing`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `judgement.types`, `judgement.miss`, `config.external`, `interactions.cross_note`, `note.tap`
- Last reviewed: 2026-07-27

## Statement

After coarse timing classification and before detailed-code conversion, a
loaded skill profile's first change-result control replaces a valid provisional
tier with tier 0 when that tier is less than or equal to the first valid
threshold. Later controls and the separate temporary-effect lifetime predicate
do not participate.

## Anchors

- `game.exe @ RAM:00c1af30, FUN_00c1af30, classification-remap-conversion order`
- `game.exe @ RAM:00c1a960, FUN_00c1a960, remap bridge`
- `game.exe @ RAM:00b97ab0, FUN_00b97ab0, result-controller bridge`
- `game.exe @ RAM:00b95c90, FUN_00b95c90, remap forwarding`
- `game.exe @ RAM:00b93ba0, FUN_00b93ba0, loaded-profile and first-unit threshold test, hash c6a2236cbca7f998527eb863064d2cf0bb6a5faff609318923094d18872fde85`
- `game.exe @ RAM:00af78f0, FUN_00af78f0, tier-0 value producer`
- `game.exe @ RAM:00c196f0, FUN_00c196f0, detailed-code conversion consumer`
- `game.exe @ RAM:0108e060, FUN_0108e060, runtime result-type count`
- `game.exe @ RAM:00b94a60, FUN_00b94a60, identity-change reset/rebuild gate, hash 1b591dc0b83153dfacca1a57359d854249f37f2a06701318974c3018b95972da`
- `game.exe @ RAM:00b94ac0, FUN_00b94ac0, source-order skill-control unit construction, hash 8a805dd6e033a3be52443714a46f34384795f529b2524b286aeb1977dcc620fa`
- `game.exe @ RAM:008da420, FUN_008da420, selected-profile metadata copy including profile ID, hash e5d70eb405ef6ef0effb3d4da6319a56f98c6c698548cb4066f84120d5d0ec34`
- `game.exe @ RAM:018d7868, SkillChangeJudgeResult control-unit vtable`
- `game.exe @ RAM:01c2df60, SkillChangeJudgeResultData RTTI type descriptor`
- `game.exe @ RAM:00b93f90, FUN_00b93f90, profile-ID and control-vector reset, hash f9f1eceb8064b2c1c4ddb372164ee212443756944534477043ed7964f157d830`
- `game.exe @ RAM:00b943b0, FUN_00b943b0, separate temporary-effect lifetime predicate, hash e04c40df9fe10e54929b0f469d699b9d6f9523e1d1d76c447545d43e7c25dbc2`
- `game.exe @ RAM:005242f0, FUN_005242f0, NotesJudgeResultTableRecord registration`

## Observations

- The TAP gate obtains a provisional coarse tier and side code from its lane
  classifier or expiry path.
- It calls the remap chain before passing those values to the detailed-code
  converter.
- The remap returns its input unchanged unless the controller has a
  nonnegative loaded profile ID, a nonempty control vector, and a non-null
  source record in the vector's first unit.
- Both the input tier and control-record threshold must be below the runtime
  result-type count. If `input <= threshold`, the output is explicitly set to
  zero; otherwise it remains the input tier.
- Tier 0 subsequently maps to anonymous detailed result code 11.
- Slide start reaches the same remap through the shared TAP gate. Each ordinary
  Slide path checkpoint also calls the remap bridge after retained-gap byte
  conversion and before its result wrapper.
- The producer's case-5 unit uses RTTI identifying
  `SkillChangeJudgeResultData`; matching source records are appended to the
  dedicated vector in source iteration order. The remap reads only the first
  unit. It does not search for a currently active unit or combine later
  thresholds.
- When any component of the selected identity triple changes, the setup gate
  resets the prior controller before rebuilding it. The metadata copy writes
  the selected profile ID; reset writes `-1`, destroys the units, and restores
  an empty range.
- Other skill consumers call a separate predicate that combines the loaded ID
  with temporary-effect timing fields. The remap does not call that predicate
  and does not read those timing fields.
- A direct-reference audit of the remap bridge closed all twelve callers:
  Slide path, the shared TAP/CHR/HOLD/Slide start gate, AIR timing, HeavenHold,
  MNE, FLK, AirHold, AirSlide, HOLD, and the MNE/FLK result wrappers. No
  unidentified note-family caller remains.
- The validity bound comes from a runtime-loaded table registered as
  `NotesJudgeResultTableRecord`, so its unavailable count is correctly treated
  as an input rather than guessed from the executable.

## Reasoning

The data flow closes the transformation order and proves a judgement-affecting
override outside the timing windows. The controller's load/reset ownership and
first-record access close both lifetime and multiple-unit precedence: it is a
profile-loaded thresholded demotion to tier 0, not a timed unit-selection
algorithm or a change to interval membership or delta.

## Alternatives and falsifiers

- Competing explanation: the remap changes only result presentation after the
  gameplay result is fixed.
- Evidence that would disprove this claim: score/state consumers receiving the
  pre-remap tier, or the remap call occurring after detailed result storage.

## Unknowns

- The result-type count and thresholds originate in runtime-loaded state; their
  values are unavailable in the executable snapshot and remain explicit
  reconstruction inputs.
- Skill fields unrelated to this remap are outside this claim.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/judgement.md`, `spec/configuration.md`,
  `spec/notes/tap.md`.
- Reconstruction code: `apply_loaded_active_result_controls` in
  `include/chart/reconstruction.hpp`.
- Tests: load/reset absence, missing first source, first-unit precedence,
  threshold boundaries, and tier-0-to-detailed-11 composition in
  `tests/tap_window_test.cpp`.

## Verification

Focused tests cover unloaded state, empty vectors, missing first sources,
first-unit precedence, inclusive thresholding, invalid type and threshold
values, and composition with detailed-code conversion. Producer RTTI, source
order, identity copy, reset ownership, separate lifetime predicate, and every
direct remap caller were independently traced in the exact snapshot.
