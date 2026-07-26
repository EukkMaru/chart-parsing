# Claim: active result-control state can replace a TAP tier with zero

- ID: `claim.judgement.active-tier-zeroing`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `judgement.types`, `judgement.miss`, `config.external`, `interactions.cross_note`, `note.tap`
- Last reviewed: 2026-07-20

## Statement

After TAP timing classification and before detailed-code conversion, active
result-control state replaces a valid provisional coarse tier with tier 0 when
that tier is less than or equal to the active valid threshold; otherwise it
leaves the tier unchanged.

## Anchors

- `game.exe @ RAM:00c1af30, FUN_00c1af30, classification-remap-conversion order`
- `game.exe @ RAM:00c1a960, FUN_00c1a960, remap bridge`
- `game.exe @ RAM:00b97ab0, FUN_00b97ab0, result-controller bridge`
- `game.exe @ RAM:00b95c90, FUN_00b95c90, remap forwarding`
- `game.exe @ RAM:00b93ba0, FUN_00b93ba0, active-threshold test`
- `game.exe @ RAM:00af78f0, FUN_00af78f0, tier-0 value producer`
- `game.exe @ RAM:00c196f0, FUN_00c196f0, detailed-code conversion consumer`
- `game.exe @ RAM:0108e060, FUN_0108e060, runtime result-type count`
- `game.exe @ RAM:00b94ac0, FUN_00b94ac0, skill-control unit construction`
- `game.exe @ RAM:018d7868, SkillChangeJudgeResult control-unit vtable`
- `game.exe @ RAM:01c2df60, SkillChangeJudgeResultData RTTI type descriptor`
- `game.exe @ RAM:00b93f90, FUN_00b93f90, control-vector reset`
- `game.exe @ RAM:005242f0, FUN_005242f0, NotesJudgeResultTableRecord registration`

## Observations

- The TAP gate obtains a provisional coarse tier and side code from its lane
  classifier or expiry path.
- It calls the remap chain before passing those values to the detailed-code
  converter.
- The remap returns its input unchanged unless the controller has a nonnegative
  active-state field, a nonempty control vector, and a first control record.
- Both the input tier and control-record threshold must be below the runtime
  result-type count. If `input <= threshold`, the output is explicitly set to
  zero; otherwise it remains the input tier.
- Tier 0 subsequently maps to anonymous detailed result code 11.
- Slide start reaches the same remap through the shared TAP gate. Each ordinary
  Slide path checkpoint also calls the remap bridge after retained-gap byte
  conversion and before its result wrapper.
- The producer's case-5 unit uses RTTI identifying
  `SkillChangeJudgeResultData`; it is appended to the result controller's
  dedicated unit vector. Controller construction initializes that vector empty,
  and reset destroys its units and restores an empty range.
- The validity bound comes from a runtime-loaded table registered as
  `NotesJudgeResultTableRecord`, so its unavailable count is correctly treated
  as an input rather than guessed from the executable.

## Reasoning

The data flow closes the transformation order and proves a judgement-affecting
override outside the timing windows. It is a thresholded demotion to the
binary's tier 0, not a change to interval membership or delta.

## Alternatives and falsifiers

- Competing explanation: the remap changes only result presentation after the
  gameplay result is fixed.
- Evidence that would disprove this claim: score/state consumers receiving the
  pre-remap tier, or the remap call occurring after detailed result storage.

## Unknowns

- The exact skill-data field schema, activation duration, selection among
  multiple units, and precedence with other skill-control types remain open.
- The result-type count and threshold originate in runtime-loaded state; their
  available values and version compatibility are not yet closed.
- Remaining note-family checkers still require an independent remap audit.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/judgement.md`, `spec/configuration.md`,
  `spec/notes/tap.md`.
- Reconstruction code: `apply_active_result_threshold` in
  `include/chart/reconstruction.hpp`.
- Tests: threshold boundaries and the tier-0-to-detailed-11 composition in
  `tests/tap_window_test.cpp`.

## Verification

Focused tests cover inactive state, inclusive thresholding, invalid type and
threshold values, and composition with detailed-code conversion. Producer RTTI
and reset ownership were independently traced; detailed skill lifetime and
multiple-unit behavior remain open.
