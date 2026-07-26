# Claim: the bounded post-update cue scheduler is outcome-external

- ID: `claim.pipeline.post-update-cue-report-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

The sole bounded scheduler after the active-note, lazy-materialization,
meter-grid, and key-beam passes can start at most four audio/effect cues and
retain timestamps for reporting. Its state does not feed input synthesis,
active-note dispatch, candidate selection, judgement, result aggregation, or
the gameplay clock. Observed cue-backend time does participate in a later
scene-transition callback, so the scheduler is outcome-external but is not
independent of post-gameplay presentation lifetime.

## Anchors

- `game.exe @ RAM:00da9820, FUN_00da9820, sole scheduler call and outer-update order`
- `game.exe @ RAM:00da5bd0, FUN_00da5bd0, bounded cue/event scheduler`
- `game.exe @ RAM:00b98010 and RAM:00b96440, current-time input`
- `game.exe @ RAM:00a84520 and RAM:00a845b0, configured resource selection`
- `game.exe @ RAM:007ed890 and RAM:007ee190, bounded event-table insertion`
- `game.exe @ RAM:00a850c0 and RAM:00a84870, cue-controller selection and launch`
- `game.exe @ RAM:00a83750 and RAM:01222570, controller/backend initialization`
- `game.exe @ RAM:00da03e0, RAM:00da06c0, and RAM:00da3a10, field initialization and reset`
- `game.exe @ RAM:00da25b0, FUN_00da25b0, scene-transition callback registration`
- `game.exe @ RAM:00da2d30, FUN_00da2d30, cue-time scene-transition consumer`
- `game.exe @ RAM:00da2630, FUN_00da2630, teardown callback registration`
- `game.exe @ RAM:00da1d20, FUN_00da1d20, report submission and cue-state reset`
- `game.exe @ RAM:00a85f90, FUN_00a85f90, bounded report/log consumer`

## Observations

- The outer gameplay update reaches `FUN_00da5bd0` only after both ordinary and
  alternate input/note-manager branches have converged, lazy note
  materialization has run, and the meter-grid and KeyBeamManager passes have
  completed. The scheduler has one direct caller through its unique thunk.
- The scheduler admits a trigger only while its scene-local count is below
  four. It reads the current time, configured cue availability, cue-controller
  activity, a terminal predicate, and retained prior timestamps. A successful
  trigger increments the count, captures the first trigger time, inserts a
  type-6 record in the bounded event table, starts a selected cue controller,
  and resets a local timer.
- Construction and gameplay reset initialize the trigger count, timestamp
  pairs, cue handles, prior-observation fields, and retained maximum cue time.
  These fields belong to the scene owner rather than the active-note manager,
  input history, parser/chart, or result aggregator.
- The cue launch path resolves one of 51 fixed-size controller objects and
  invokes backend-style reset/start, channel/mode, and gain operations. Its
  initialization chain includes the fixed backend value 48000 and resource
  setup. The calls return no value to the note-manager path.
- The teardown callback is installed by `FUN_00da2630`. `FUN_00da1d20` passes
  the bounded count and retained timestamps to `FUN_00a85f90`, whose closed
  path formats them into a ten-entry report/log ring, then resets scheduler
  fields and clears the associated cue controllers.
- The separate callback installed by `FUN_00da25b0` invokes the same outer
  update and reads observed times from both cue handles. It compares one
  backend time with the retained maximum derived from the other while deciding
  whether to write scene state `0x11`. It has no active-note, input, candidate,
  result, or gameplay-clock write.

## Reasoning

Caller order and the scheduler's store set put cue generation downstream of
the complete current gameplay pass. Constructor/reset and teardown closure
show that its retained fields are scene-local cue/report state. The resolved
controller/backend and report paths do not cross into gameplay owners, so a
cue cannot alter later note input or judgement despite remaining active across
updates. The scene-transition callback is a real consumer, however: backend
cue timing can affect when that callback selects its next state after gameplay.
The safe boundary is therefore judgement/result exclusion, not a claim that
the call is temporally inert for the enclosing scene.

## Alternatives and falsifiers

- Competing explanation: cue-controller or report state is consulted by a
  later note update, result rule, or the authoritative play-clock producer.
- Evidence that would disprove this claim: an alias from the scheduler fields,
  backend callbacks, or event-table records into input history, active notes,
  candidate reduction, result aggregation/remapping, terminal outcome, or the
  play-clock correction/rate owners.

## Unknowns

- Exact player-facing cue identity and externally selected resource values are
  unavailable and remain outside the reconstructed outcome path.
- The semantic name of scene state `0x11` and the precise visual transition
  guarded by the two cue times are not assigned.
- Backend failure behavior is not assigned to the successful gameplay path.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary static project clone. Missing thunk functions
  were materialized only in that clone to resolve callback registrations.
- Spec sections: `spec/timing.md` post-update cue/report boundary.
- Reconstruction code: none; the recovered logic is outside judgement and
  result outcomes.
- Tests: none; the exclusion is supported by caller order, complete scene-field
  lifetime, resolved indirect consumers, and sink closure.

## Verification

The sole outer caller, trigger predicates and cap, configured-resource and
cue-controller chain, constructor/gameplay resets, both registered scene
callbacks, report serializer, controller clearing, and all gameplay-owner
writes in those paths were inspected independently. The only recovered
gameplay-adjacent downstream dependency is the post-gameplay scene-state
condition.
