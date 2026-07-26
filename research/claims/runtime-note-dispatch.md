# Claim: all factory note classes share a deferred three-state gameplay dispatch

- ID: `claim.pipeline.runtime-note-dispatch`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `state.ownership`, `audit.indirect_calls`, `interactions.cross_note`
- Last reviewed: 2026-07-21

## Statement

Every primary and attached-secondary runtime class reachable from the parsed
record factory uses the same base transition order and registered callback
table. Construction requests state 0 while current state is -1. The first later
manager substep commits state 0 and requests state 1; the second commits state
1 and invokes that class's gameplay update. A state-1 terminal request remains
pending until a later substep commits state 2, after which the manager removes
the object in the same pass. Only primary objects enter the preceding candidate
phase. The inherited auxiliary state-action selector is dormant on every
factory-reachable note path because both of its constructor-owned maps remain
empty.

## Anchors

- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, closed primary/secondary factory switch`
- `game.exe @ RAM:00a33e10, FUN_00a33e10, base current/requested-state initialization`
- `game.exe @ RAM:00c19af0 and RAM:00c19c00, NotesBase construction and state-0 request`
- `game.exe @ RAM:00c19960, FUN_00c19960, three-state callback-table registration`
- `game.exe @ RAM:00c1ac40, RAM:00c1ac60, and RAM:00c1ac20, entry/update/exit callback selection`
- `game.exe @ RAM:00c19ad0, FUN_00c19ad0, state-0 predicate and state-1 request`
- `game.exe @ RAM:00c19ab0, FUN_00c19ab0, state-1 +0x2c dispatch`
- `game.exe @ RAM:00c10f40, FUN_00c10f40, common per-note tick wrapper`
- `game.exe @ RAM:00c0dd10, FUN_00c0dd10, pending-state commit before callbacks`
- `game.exe @ RAM:00c0db20, FUN_00c0db20, exact/fallback state-action selection`
- `game.exe @ RAM:00a35960 and RAM:00a36570, exact-factory and fallback-map lookups`
- `game.exe @ RAM:00d9bd40, FUN_00d9bd40, contrasting root-state child-factory registration`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, candidate/primary/secondary/removal order`
- `game.exe @ RAM:00c1a650, FUN_00c1a650, current-state-2 removal predicate`
- `game.exe @ RAM:018d92d4 through RAM:018d9b30, twelve concrete gameplay vtables enumerated below`

## Observations

- The shared base initializes current state `+0x10` to -1 and requested state
  `+0x14` to 0. Each concrete constructor reaches that base before installing
  its final vtable and loading the parsed record.
- One registered parameter object supplies three callbacks for each of states
  0, 1, and 2. All transition-entry and transition-exit callbacks are no-ops.
  In the per-state update column, state 0 calls virtual `+0x28` and requests
  state 1 when it returns true; state 1 calls virtual `+0x2c`; state 2 is a
  no-op.
- All twelve concrete gameplay vtables resolve `+0x28` to a function that
  unconditionally returns true. Activation therefore always consumes exactly
  the first later manager substep. There is no family-specific readiness gate
  at this state-machine layer.
- The common tick increments its local counter, commits any pending transition,
  dispatches the callback for the resulting current state, then runs a separate
  state-action hook when one was installed. A request made during the callback
  is not recommitted in the same tick.
- The common tick has an outer byte guard at base offset `+0x65`. Shared
  `NotesBase` construction clears offsets `+0x64..+0x65` together. The complete
  factory-reachable constructor/load set, manager loops, transition helpers,
  registry consumers, and destruction paths contain no later writer or setter
  for `+0x65`; the note path's only direct access after construction is the
  common tick read. The guard therefore remains zero and cannot pause a runtime
  note in this snapshot.
- The factory/vtable matrix is closed over every nondefault factory arm:

  | Factory source | Runtime class | Manager vector | Candidate phase | State-1 update |
  | --- | --- | --- | --- | --- |
  | type 0 | TapNote | primary | TAP family | `RAM:00c1dc30` |
  | type 4 | CharaTapNote | primary | TAP family | `RAM:00c1dc30` |
  | type 1 | HoldNote | primary | HOLD family | `RAM:00c2b480` |
  | type 2 | SlideNote | primary | Slide family | `RAM:00c10ed0` |
  | type 6 | FlickNote | primary | FLK family | `RAM:00c20340` |
  | ordinary type 9 | AirLadderNote | primary | all sentinels | `RAM:00c13810` |
  | exceptional type 9 or type 13 | HeavenHoldNote | primary | HeavenHold family | `RAM:00c16340` |
  | type 10 | AirSolidNote | primary | all sentinels | `RAM:00c17040` |
  | type 11 | MineNote | primary | all sentinels | `RAM:00c1fac0` |
  | attached type 3 | AirNote | secondary | not called | `RAM:00c1ec70` |
  | attached type 5 | AirHoldNote | secondary | not called | `RAM:00c24cd0` |
  | attached type 8 | AirSlideNote | secondary | not called | `RAM:00c297d0` |

- Before ticking any primary, the manager invokes primary virtuals `+0x30`
  and `+0x34`, reduces the 16 lane values, and runs the shared input stage. It
  then ticks all primaries in vector order, removes current-state-2 primaries,
  ticks all secondaries in their separate vector, and removes current-state-2
  secondaries. Secondary objects never enter the candidate loop.
- Runtime materialization occurs after all manager substeps in the current
  outer update. Consequently, the first possible type-specific update is the
  second manager substep after construction. With a two-substep catch-up this
  can occur within the next outer update; with a one-substep path it cannot.
- A state-1 update may request state 2. The current state remains 1 through the
  manager's removal check. On a later substep the common wrapper commits state
  2, dispatches no gameplay update, and the manager then deletes the object.
- The post-callback state-action hook is distinct from the registered gameplay
  callback table. The base constructor creates an empty exact child factory at
  `+0x44` (its ordered-map root/count are `+0x48/+0x4c`) and an empty fallback
  callable map at `+0x50/+0x54`. The transition selector first requests the
  current-state key from the exact factory, then tries the fallback map. With
  both containers empty it returns null, so the transition stores null at
  `+0x40` and the common tick skips the auxiliary virtual call.
- All twelve concrete constructors, nine distinct primary load callbacks, and
  three secondary load callbacks were checked after the shared base
  construction. None registers an exact child factory, installs a fallback
  callable, or writes either container count. The factory caller then appends
  the object directly to the manager vector. A repository-wide gameplay-code
  instruction audit found no other direct access to these two `NotesBase`
  containers.
- This absence is not inferred from generic convention. The same framework's
  RTTI-identified root state machine explicitly calls six derived-constructor
  registration helpers for state keys 2 through 7 (and a seventh for key 1)
  after its base constructor. The exact executable has
  `FactoryConstructor<..., StateMachineBase>` RTTI for those child classes but
  no note-related child factory type. `NotesBase` instead installs only its
  fixed three-state parameter callback object.
- `NotesBase` itself is inserted into the process-wide state-machine registry
  on construction and removed on destruction. The recovered registry
  consumers use its parent/child link for hierarchy inspection; registration
  does not populate either per-instance child factory map.

## Reasoning

The factory switch bounds the possible primary and attached-secondary classes.
Enumerating those vtables closes the virtual `+0x28` and `+0x2c` targets rather
than inferring them from a representative note. The shared callback table and
transition routine then establish when each target can execute. Constructor,
load, factory-append, and registry closure show that the second post-callback
indirect edge has no target on this path. Manager vector order separately
establishes which objects receive candidate calls and when a committed terminal
state is observed. The base-guard writer/read closure also establishes that
every manager tick reaches the transition and registered-callback sequence;
there is no hidden per-note pause state between creation and removal.

## Alternatives and falsifiers

- Competing explanation: some concrete note delays activation through its
  `+0x28` predicate.
- Evidence that would disprove this claim: another factory-reachable vtable or
  a `+0x28` implementation that can return false.
- Competing explanation: a state-1 callback can request and commit state 2 in
  the same substep.
- Evidence that would disprove this claim: a second pending-transition commit
  after the callback and before the manager removal query.
- Competing explanation: attached AIR-family objects participate in lane
  candidate reduction.
- Evidence that would disprove this claim: insertion of a secondary into the
  primary vector or a candidate loop over the secondary vector.
- Competing explanation: a runtime note installs an auxiliary state-action
  child that runs after its gameplay callback.
- Evidence that would disprove this claim: a factory-reachable constructor,
  load path, or later manager/registry consumer that inserts into the exact or
  fallback child map before a transition.

## Unknowns

- Allocation-failure behavior is outside the reconstructed normal path.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP was unavailable and
  all enumeration used the temporary static project clone.
- Spec sections: `spec/timing.md`, `spec/matching.md`.
- Reconstruction code: `runtime_note_tick_is_suppressed`,
  `make_runtime_note_lifecycle` and
  `dispatch_runtime_note_substep`, plus the closed
  `runtime_note_has_state_action_child` invariant, in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/runtime_note_dispatch_test.cpp`.

## Verification

The factory cases, primary and secondary append destinations, manager loops,
base constructor, common tick and its guard reference set, state transition
routine, nine callback-table entries, twelve `+0x28` targets, twelve `+0x2c`
targets, both empty child
containers, twelve constructors, twelve load callbacks, candidate slots, and
destructors were checked as independent surfaces. The global registry and a
root state machine with explicit child-factory registrations were used as
independent positive controls for the dormant `NotesBase` hook. The AirSolid vtable was
rechecked by slot during this enumeration, correcting its manager-adjacent
`+0x34` target to the shared no-op while leaving its separate end comparison at
`+0x38`. The focused dispatch test preserves construction, two-substep
activation, deferred terminal commit, and same-pass removal eligibility.
