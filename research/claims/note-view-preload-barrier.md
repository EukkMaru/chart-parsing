# Claim: note-view resource staging gates the loading-scene exit

- ID: `claim.pipeline.note-view-preload-barrier`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `state.ownership`, `audit.indirect_calls`
- Last reviewed: 2026-07-21

## Statement

A fresh note-view preload owns eleven presentation objects in a third
`NotesManager` vector and advances them through a fixed sequential resource-step
schedule before the loading controller may exit. This vector is distinct from
both active runtime-note vectors and neither constructs parsed notes nor runs
judgement logic.

## Anchors

- `game.exe @ RAM:00b2b630, FUN_00b2b630, preload restart and owned-object deletion`
- `game.exe @ RAM:00b2a3c0, FUN_00b2a3c0, staged construction, readiness, and completion`
- `game.exe @ RAM:00b2ad20, FUN_00b2ad20, third-vector append`
- `game.exe @ RAM:00b2a3a0, FUN_00b2a3a0, third-vector nonempty query`
- `game.exe @ RAM:00da00c0, FUN_00da00c0, scene-entry restart decision`
- `game.exe @ RAM:00da27f0, FUN_00da27f0, loading-state exit gate`
- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, manager reset deletion and idle state`
- `game.exe @ RAM:00b27210, FUN_00b27210, destructor deletion and vector storage release`
- `game.exe @ RAM:00b294d0, FUN_00b294d0, finalize-all and idle helper`
- `game.exe @ RAM:00c1d9b0, FUN_00c1d9b0, Tap readiness step`
- `game.exe @ RAM:00c0fe20, FUN_00c0fe20, Slide readiness step`
- `game.exe @ RAM:00c16f90, FUN_00c16f90, AirSolid immediate readiness`
- `game.exe @ RAM:00c1f7a0, FUN_00c1f7a0, Mine readiness step`

## Observations

- `NotesManager` owns three adjacent vectors. The primary and secondary active
  runtime-note vectors begin at offsets `+0x2e8` and `+0x2f4`; this vector begins
  at `+0x2dc`. Active-note cleanup clears only the former two. Full manager
  reset and destruction independently delete every nonnull object in all three.
- Restart deletes existing third-vector objects, clears its end pointer, sets
  stage 1, and clears the resource-step counter. Stage 1 constructs the first
  object. Stages 2 through 12 test accumulated objects, finalize them when the
  stage advances, and construct the next object. Stage 12 advances to terminal
  stage 13 without constructing another object.
- RTTI identifies the exact construction order as
  `projView::TapNote`, `CharaTapNote`, `HoldNote`, `SlideNote`, `AirNote`,
  `AirHoldNote`, `AirSlideNote`, `AirLadderNote`, `AirSolidNote`, `FlickNote`,
  and `MineNote`.
- Each constructor result receives virtual `+0x50`, then is appended only to
  the third vector. On later calls, every not-yet-ready object receives virtual
  `+0x54` with the shared resource-step counter. A ready byte at object
  `+0xdc` prevents repeat readiness work. Stage advancement additionally
  requires the old counter to be at least 10.
- The first resource steps returning ready, in construction order, are
  `8, 10, 12, 20, 12, 15, 13, 11, 0, 9, 29`. Consequently each stage consumes
  `max(10, first_ready_step) + 1` periodic calls because counters begin at zero.
  Including the stage-1 construction call, a fresh uninterrupted sequence
  returns complete on its 164th preload-step invocation.
- Before each construction after the first, and once more at final completion,
  the manager invokes virtual `+0x58` on every accumulated object. Sampled and
  recovered targets release or reset presentation/resource containers; no
  target reaches parsed-record dispatch, the runtime-note factory, input, or
  result submission.
- The loading-state update calls this stepper with floor 10 and cannot take its
  scene-transition branch until it returns complete. The gameplay outer update
  and active-note manager are reached in a later controller state.
- Manager reset returns the coordinator to stage 0 and deletes the objects.
  Destruction also frees the vector allocation. The finalize-all/stage-0 helper
  at `00b294d0` has no recovered direct caller or thunk in this snapshot.

## Reasoning

The separate vector offsets and append helper distinguish these objects from
runtime notes. RTTI, the deterministic integer readiness functions, and the
loading-controller gate establish a presentation-resource barrier before
gameplay rather than another judgement container. It can delay entry into
gameplay, but once the controller advances it contributes no gameplay tick,
candidate, input, or result operation.

## Alternatives and falsifiers

- Competing explanation: the third vector contains live notes that participate
  in the active manager pass.
- Evidence that would disprove this claim: an append from parsed-record factory
  dispatch into this vector, a call to its readiness/finalize virtuals from the
  active gameplay loop, or a result/input mutation reachable from those slots.

## Unknowns

- Player-facing resource identities and the wall-clock cadence of loading-scene
  update calls are outside the clean-room gameplay model.
- The reason the unreferenced finalize-all helper remains in the executable is
  unknown; it is not included in the live ownership path.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only-clone
  analysis recovered two missing AirSolid virtual targets only.
- Spec sections: `spec/timing.md`.
- Reconstruction code: `NoteViewPreloadState`, fixed class order/readiness
  thresholds, and `advance_note_view_preload` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/note_view_preload_test.cpp`.

## Verification

The third vector's constructor, restart, step, append, reset, destructor, and
scene callers were traced independently. All eleven RTTI identities and their
`+0x50/+0x54/+0x58` slots were resolved; readiness thresholds were checked in
the concrete `+0x54` targets. The focused test walks the full sequence, checks
every per-stage call count and construction order, verifies the 164-call fresh
path, and exercises idle/reset behavior.
