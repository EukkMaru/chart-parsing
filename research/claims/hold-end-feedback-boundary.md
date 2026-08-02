# Claim: the HOLD end branch does not submit a checkpoint result

- ID: `claim.note.hold-end-feedback-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `note.hold`, `judgement.types`, `audit.indirect_calls`
- Last reviewed: 2026-08-03

## Statement

The once-only HOLD branch after adjusted end converts its gap classification to
an indexed feedback resource request, but it neither calls the shared result
dispatcher nor changes the HOLD start/checkpoint phases or terminal request.

## Anchors

- `game.exe @ RAM:00c29c50, FUN_00c29c50, once-only end-threshold branch`
- `game.exe @ RAM:00c1b800, FUN_00c1b800, classification/category resource selection`
- `game.exe @ RAM:00a836f0, FUN_00a836f0, indexed resource preparation`
- `game.exe @ RAM:00a85a70, FUN_00a85a70, resource parameter selection`
- `game.exe @ RAM:00a85070, FUN_00a85070, resource instance selection`
- `game.exe @ RAM:00a850c0, FUN_00a850c0, resource control virtual calls`
- `game.exe @ RAM:004b8fe0, InitializeSoundControllerArray, 51-element static owner construction`
- `game.exe @ RAM:00a83080 and RAM:01223840, wrapper and embedded sound::Sound construction`
- `game.exe @ RAM:0196a9f0, sole sound::Sound vtable used by every indexed instance`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared authoritative result boundary not called here`
- `game.exe @ RAM:00b97730, FUN_00b97730, result-owner dispatcher not called here`

## Observations

- After the end threshold, HOLD selects either the retained-gap byte or the
  forced-result byte, passes it with a category to `FUN_00c1b800`, then sets its
  once-only flag.
- The helper validates the result byte and category, uses fixed lookup tables to
  choose a resource and parameters, and enters the common indexed resource
  subsystem.
- Static setup vector-constructs exactly 51 wrapper elements at `01c75b38`
  with stride `0x80`. Each wrapper's embedded object at `+4` is constructed by
  `FUN_01223840`, which writes the sole `sound::Sound` vtable `0196a9f0`.
- The final resource helper's targets are therefore fixed: optional slot
  `+0x74` reaches `01223e70`; slots `+0x14`, `+0x18`, `+0x54`, `+0x44`,
  `+0x20`, and `+0x24` reach `01224b80`, `01224ca0`, `01224e30`,
  `01224da0`, `01224ce0`, and `01224710`. They mutate only `sound::Sound`
  fields or call the audio middleware to attach, configure, and start sound.
  None registers a callback or reaches the result owner.
- There is no call from this branch to the HOLD result wrapper,
  `FUN_00c1c340`, `FUN_00b97730`, or the aggregate owner. The branch has no
  write to either HOLD phase, the checkpoint vector, or requested note state.
- Scheduled checkpoint emission remains a separate earlier branch and is the
  path that forwards category 2 or 3 results.

## Reasoning

The end branch consumes judgement-derived data but terminates at a statically
bounded `sound::Sound` owner. Constructor/vtable closure resolves every virtual
slot before its audio-middleware sink, rather than relying on ignored returns.
Its only HOLD-local mutation is the once flag. It therefore must not be
reconstructed as an implicit end checkpoint, a result submission, or a
phase-completion event.

## Alternatives and falsifiers

- Competing explanation: the indexed object can have another runtime vtable or
  one of the fixed sound targets synchronously re-enters gameplay.
- Evidence that would disprove this claim: another constructor/vptr writer for
  one of the 51 objects, a callback-registration edge in the resolved targets,
  a call path to the result owner, or a hidden alias from the once flag to
  phase/requested-state storage.

## Unknowns

- Exact resource identities and player-facing feedback meaning are outside the
  gameplay reconstruction.

## Consequences

- Ghidra mutations: created and renamed the supported static initializer at
  `RAM:004b8fe0` to `InitializeSoundControllerArray` and added a plate comment
  recording its 51-element owner/vtable boundary.
- Spec sections: `spec/notes/hold.md`, `spec/judgement.md`.
- Reconstruction code: none; the clean-room gameplay model intentionally has no
  synthetic end-result operation.
- Tests: existing empty-checkpoint and end-gap tests in
  `tests/hold_sustain_test.cpp` remain the relevant behavioral guard.

## Verification

The HOLD branch, feedback lookup, complete static object construction, sole
vtable, all invoked concrete slots, audio-middleware sinks, scheduled
checkpoint result path, phase writes, and shared result dispatcher were audited
as separate flows. No callback registration, call edge, return value, or state
alias connects the feedback path to HOLD gameplay state.
