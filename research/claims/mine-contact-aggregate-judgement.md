# Claim: MNE contact resolution submits an authoritative aggregate event

- ID: `claim.note.mine-contact-aggregate-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `input.logical_state`,
  `parser.events`, `matching.candidates`, `judgement.types`, `judgement.miss`,
  `note.other_variants`, `state.ownership`, `config.external`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

MNE accumulates the clipped previous-to-current interval while any covered
logical lane is held. At nonnegative adjusted time it terminates successfully
when accumulated contact reaches its external requirement, or unsuccessfully
when accumulated contact plus all remaining eligible time is strictly less
than that requirement. Either result uses source category 17, which maps
through dispatch category 11 to progress category 0 and aggregate category 8;
a runtime-valid MNE result is therefore authoritative aggregate input.

## Anchors

- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed type-11 factory case`
- `game.exe @ RAM:00c1ec90, FUN_00c1ec90, MineNote construction`
- `game.exe @ RAM:018d9878 and RAM:01c32620, MineNote vtable and RTTI`
- `game.exe @ RAM:00c1f940, FUN_00c1f940, load/reset path`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, sentinel candidate output`
- `game.exe @ RAM:00c1ee00, FUN_00c1ee00, contact and result state machine`
- `game.exe @ RAM:00c2de10, FUN_00c2de10, logical held-level query`
- `game.exe @ RAM:00c1f8f0, FUN_00c1f8f0, result-remap wrapper`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared submission and finalization`
- `game.exe @ RAM:00d7f030, FUN_00d7f030, source 17 to dispatch 11`
- `game.exe @ RAM:011cc170 and RAM:011cc130, progress 0 and aggregate 8 lookups`
- `game.exe @ RAM:00b95870, FUN_00b95870, authoritative aggregate consumer`

## Observations

- Construction and successful load clear local phase and accumulated contact.
  MNE emits no manager candidate and does not use the TAP timing checker.
- Current and previous adjusted deltas share scheduled time, external field
  `+0x720`, and one runtime correction. After external start `+0x724`, any held
  lane credits the whole positive overlap of the substep with the interval
  ending at external field `+0x728`; lane count does not multiply credit.
- Resolution waits for nonnegative current time. Requirement equality succeeds.
  Impossibility requires a strict shortfall after adding the nonnegative
  remaining interval, so equality with the best possible total does not fail.
- Local phase 1 submits provisional byte 0 and phase 2 byte 4 through the
  active-result remap. Forced mode waits for its separate configured point,
  presets phase 2, and changes to phase 1 only for selected byte zero.
- The shared handler receives fixed source category 17 and finalizes the note
  after result dispatch. The two-stage fixed mapping converts source 17 to
  dispatch 11, progress 0, and aggregate 8. A valid result updates ordinary
  progress/observer state, authoritative aggregate state, and terminal rules.
  An already-active terminal route still selects observer-only dispatch.
- Note finalization remains outside result routing, so both authoritative and
  observer-only paths request the same deferred note removal.

## Reasoning

The original MNE state-machine evidence remains compatible with the corrected
downstream mapping. The second lookup and aggregate validator establish that
MNE is not a presentation-only result family: its anonymous result byte and
external contribution tables can affect ordinary outcome and terminal state.

## Alternatives and falsifiers

- Competing explanation: dispatch category 11 is forwarded unchanged and
  rejected by the aggregate's `[0, 9)` check.
- Evidence that would disprove this claim: the second lookup receiving a value
  other than original dispatch category 11, a different category from the MNE
  wrapper, or aggregate validation occurring before the 11-to-8 conversion.

## Unknowns

- External timing/requirement values, result-remap configuration, contribution
  values, and player-facing names remain parameters.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Supersedes: `claim.note.mine-contact-judgement`.
- Spec sections: `spec/notes/mine.md`, `spec/judgement.md`, `spec/input.md`,
  `spec/matching.md`, `spec/configuration.md`.
- Reconstruction code: existing MNE helpers plus corrected shared-result maps.
- Tests: `tests/mine_contact_test.cpp`, `tests/shared_result_test.cpp`.

## Verification

The MNE factory/RTTI/lifetime and contact boundaries were retained from the
independently tested prior claim. The source, dispatch, progress, and aggregate
categories were then rederived from the sole caller and both fixed table
lookups; focused tests verify the corrected route.
