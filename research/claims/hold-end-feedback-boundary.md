# Claim: the HOLD end branch does not submit a checkpoint result

- ID: `claim.note.hold-end-feedback-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `note.hold`, `judgement.types`, `audit.indirect_calls`
- Last reviewed: 2026-07-20

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
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared authoritative result boundary not called here`
- `game.exe @ RAM:00b97730, FUN_00b97730, result-owner dispatcher not called here`

## Observations

- After the end threshold, HOLD selects either the retained-gap byte or the
  forced-result byte, passes it with a category to `FUN_00c1b800`, then sets its
  once-only flag.
- The helper validates the result byte and category, uses fixed lookup tables to
  choose a resource and parameters, and enters the common indexed resource
  subsystem.
- The final resource helper invokes control slots for instance attachment,
  activation, unit gain, parameter selection, and start. Its return value is
  ignored by the HOLD branch.
- There is no call from this branch to the HOLD result wrapper,
  `FUN_00c1c340`, `FUN_00b97730`, or the aggregate owner. The branch has no
  write to either HOLD phase, the checkpoint vector, or requested note state.
- Scheduled checkpoint emission remains a separate earlier branch and is the
  path that forwards category 2 or 3 results.

## Reasoning

The end branch consumes judgement-derived data but terminates at the feedback
resource boundary. Its only HOLD-local mutation is the once flag. It therefore
must not be reconstructed as an implicit end checkpoint, a result submission,
or a phase-completion event.

## Alternatives and falsifiers

- Competing explanation: a resource virtual method synchronously re-enters the
  result dispatcher.
- Evidence that would disprove this claim: resolution of one of the invoked
  resource slots to a gameplay result submission, or a hidden alias from the
  once flag to phase/requested-state storage.

## Unknowns

- Exact resource identities and player-facing feedback meaning are outside the
  gameplay reconstruction.
- Concrete virtual targets in the common resource subsystem remain an indirect
  call audit item; no return data flows back to HOLD state.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/notes/hold.md`, `spec/judgement.md`.
- Reconstruction code: none; the clean-room gameplay model intentionally has no
  synthetic end-result operation.
- Tests: existing empty-checkpoint and end-gap tests in
  `tests/hold_sustain_test.cpp` remain the relevant behavioral guard.

## Verification

The HOLD branch, feedback lookup, common resource control path, scheduled
checkpoint result path, phase writes, and shared result dispatcher were audited
as separate flows. No return value or state alias connects the feedback path to
HOLD gameplay state.
