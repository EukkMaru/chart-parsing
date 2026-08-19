# Claim: the bounded post-update cue scheduler is outcome-external

- ID: `claim.pipeline.post-update-cue-report-boundary`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `audit.indirect_calls`
- Last reviewed: 2026-08-18

## Statement

The sole bounded scheduler after the active-note, lazy-materialization,
meter-grid, and key-beam passes can start at most four audio/effect cues and
retain timestamps for reporting. Its state does not feed input synthesis,
active-note dispatch, candidate selection, judgement, result aggregation, or
the gameplay clock. Observed cue-backend time does participate in a later
scene-transition callback, so the scheduler is outcome-external but is not
independent of post-gameplay presentation lifetime.

## Anchors

- `game.exe @ RAM:00da9820, FUN_00da9820, sole scheduler call and outer-update order, hash ce8c5ca2608c3b0e7c39932b2e6042056865380a6824f611ea0342be79c90f1c`
- `game.exe @ RAM:00da5bd0, ProcessPostUpdateCueScheduler, bounded cue/event scheduler, hash 78ca6812ca3eedb0e9b2bf3069dfd5a242d58e478489b9b3f09d0d7015a678ac`
- `game.exe @ RAM:00b98010 and RAM:00b96440, current-time input`
- `game.exe @ RAM:00a84520 and RAM:00a845b0, configured resource selection`
- `game.exe @ RAM:007ed890 and RAM:007ee190, bounded event-table insertion`
- `game.exe @ RAM:00a850c0 and RAM:00a84870, cue-controller selection and launch`
- `game.exe @ RAM:00a83750 and RAM:01222570, controller/backend initialization`
- `game.exe @ RAM:00da03e0, field initialization/reset, hash 2542baee56d6eec2057b4e3756e436538ae32f98c286d730a87d35390c5f92bd`
- `game.exe @ RAM:00da3a10, constructor initialization, hash e86da15c33a46d773787edde2d4eda2ed149d9295500d9336d601bef1cffc253`
- `game.exe @ RAM:00da25b0, FUN_00da25b0, scene-transition callback registration, hash 25d0593431056fc7b4660823a4f69165f41ca073c604e666e597ce85cc5204a0`
- `game.exe @ RAM:00436d72, sole registered thunk to RAM:00da2d30`
- `game.exe @ RAM:00da2d30, UpdatePostGameplayCueTransition, cue-time scene-transition consumer, hash 29b3befc48c997f7b19c20bc8e0c2d89597a64cc23be81b539421d07e499c762`
- `game.exe @ RAM:00da2630, FUN_00da2630, teardown callback registration, hash 5dcbce834f4bf706fa5931fcd6a5b3df56be2b98d2e8ae221a2f097fd3514ef0`
- `game.exe @ RAM:00452919, sole registered thunk to RAM:00da1d20`
- `game.exe @ RAM:00da1d20, ReportAndResetPostUpdateCueState, report submission and cue-state reset, hash 8dffbb62aeebb797786626c06097b07d1809727d1352c6c4aa07d9a9eccc6150`
- `game.exe @ RAM:00a85f90, FUN_00a85f90, bounded report/log consumer, hash 3ba4afd8bb267bba3a86b3080473b28e10c9fccbd27336b65e5d10eb6ac8e7c8`

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
- The retained field lifecycle is exact. `+0x52c` is the trigger count;
  `+0x530/+0x534` capture the first successful trigger's scene time;
  `+0x538..+0x544` retain two externally resolved configuration pairs; and
  `+0x548/+0x54c` are the two cue handles. The constructor first seeds the
  fields, initialization installs the live handles and resets the count/pairs,
  the scheduler is their only gameplay-update writer, and the registered
  report callback is their terminal reader/reset owner.
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

- Ghidra mutations: the exact live project now names `RAM:00da5bd0`
  `ProcessPostUpdateCueScheduler`, `RAM:00da2d30`
  `UpdatePostGameplayCueTransition`, and `RAM:00da1d20`
  `ReportAndResetPostUpdateCueState`. Compact comments at `RAM:00da9820`,
  `RAM:00da5bd0`, `RAM:00da25b0`, `RAM:00da2d30`, `RAM:00da2630`,
  `RAM:00da1d20`, `RAM:00a85f90`, `RAM:00da03e0`, and `RAM:00da3a10`
  preserve caller order, callback registration, field ownership, reset, and
  the outcome boundary. The project was saved after mutation.
- Spec sections: `spec/timing.md` post-update cue/report boundary.
- Reconstruction code: none; the recovered logic is outside judgement and
  result outcomes.
- Tests: none; the exclusion is supported by caller order, complete scene-field
  lifetime, resolved indirect consumers, and sink closure.

## Verification

The exact current project confirms the scheduler thunk has one call reference
from the outer update. Callback thunks `00436d72` and `00452919` each have one
data reference, from the two named registration functions. Constructor and
initialization stores, scheduler reads/writes, report arguments and clears,
cue-controller clearing, the report sink, and every gameplay-owner write in
those paths were rechecked. The only recovered gameplay-adjacent downstream
dependency is the post-gameplay scene-state condition.
