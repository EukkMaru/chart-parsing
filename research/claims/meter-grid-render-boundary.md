# Claim: the post-materialization meter-grid pass is render-only

- ID: `claim.pipeline.meter-grid-render-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.tempo_measure`
- Last reviewed: 2026-07-21

## Statement

The outer gameplay update's direct call immediately after runtime-note
materialization reads generated meter/grid positions only to select visible
view markers. It submits a stack-local position list to rendering and cannot
alter the gameplay clock, pending or active notes, input, candidates, results,
terminal state, or later materialization eligibility.

## Anchors

- `game.exe @ RAM:011bb0f0, FUN_011bb0f0, generated meter/grid-vector producer`
- `game.exe @ RAM:00da9820, FUN_00da9820, outer update and post-materialization call order`
- `game.exe @ RAM:00da8420, FUN_00da8420, generated-grid scan and local visible-position collection`
- `game.exe @ RAM:011c5310, FUN_011c5310, selected grid-index lookup`
- `game.exe @ RAM:00b29b20, FUN_00b29b20, projected-position visibility predicate`
- `game.exe @ RAM:00b297c0, FUN_00b297c0, keyed position transform wrapper`
- `game.exe @ RAM:00b28890, FUN_00b28890, projection-factor transform wrapper`
- `game.exe @ RAM:00c23e30, FUN_00c23e30, stack-local float-vector append`
- `game.exe @ RAM:00d33590 and RAM:00d335a0, visible marker render submission`

## Observations

- Meter postprocessing creates four vectors of scheduled 24-byte grid records.
  `FUN_00da8420` reads one such vector through the process parser owner; it does
  not mutate the vector or any parsed record.
- The direct thunk for `FUN_00da8420` has one caller, `FUN_00da9820`. That caller
  runs the complete ordinary or alternate input/manager path, then lazy
  materialization, then this pass. Its return is void and no value is tested by
  the remaining outer update.
- The pass obtains a current index, scans only later grid records, and applies
  the same keyed-position/projection helpers used by view placement. Records
  outside the bounded projected range are skipped. Accepted values are appended
  as floats to a vector constructed on the stack.
- `FUN_00c23e30` is a generic vector append. At this call site its receiver is
  the stack-local float vector; it has no manager, note, input, result, or chart
  receiver.
- The terminal consumer walks the local floats against up to 512 view slots,
  derives camera-space geometry, and invokes render-object virtuals. The local
  vector is released before `FUN_00da8420` returns. No render return value or
  side state is read back by the outer gameplay update.
- A direct process-parser reference scan identifies this pass as the external
  reader of the selected generated-grid vector. Its store set is confined to
  temporary collection/allocation state and render/view owners; it has no edge
  to the authoritative BPM map, manager time, pending-index queue, active-note
  vectors, input snapshots, or result controller.

## Reasoning

Producer layout establishes that the input is derived meter/grid state rather
than note timing. The sole-caller order, stack-local collection lifetime, and
resolved terminal render consumer establish the output boundary. Since all
gameplay substeps and materialization for the current outer update have already
run and the pass writes no state they consume later, generated grid visibility
cannot feed gameplay generation or judgement in this snapshot.

## Alternatives and falsifiers

- Competing explanation: the selected grid marker updates a timing index or
  candidate threshold used on the next manager substep.
- Evidence that would disprove this claim: a store from this pass or its render
  consumer into the parser schedule, manager/input owner, pending queue, active
  note, result owner, or a return value that gates a later gameplay call.

## Unknowns

- The player-facing names of the four grid vectors and the exact visual marker
  represented by the selected vector remain outside gameplay scope.
- Allocation failure and render-backend failure behavior are not assigned to
  the successful finite gameplay path.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary static project clone.
- Spec sections: `spec/timing.md` meter/grid boundary.
- Reconstruction code: none; the pass is presentation-only.
- Tests: none; the exclusion is established by producer, caller, store, and
  terminal-consumer closure.

## Verification

The meter/grid producer, selected parser-vector read, complete pass caller set,
grid-index and projection helper chain, local append receiver, terminal render
consumer, temporary destruction, and outer-update continuation were inspected
independently. No path from the selected marker list reaches gameplay-owned
state.
