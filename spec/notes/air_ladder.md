# AirLadder note

## Parser chain and runtime selection

`ALD` has command ID `0x29` and resolves to parsed type 9. The parser stores a
chain of 0x20-byte control points. A new command continues an existing nonempty
type-9 chain only when its connection selector and style equal the root's saved
values and its position, lane, width, and numeric property match the previous
endpoint. Otherwise it starts a new chain. After every append, scheduled values
are recomputed for the chain and the root's saved current position is advanced.

Runtime construction has an exact class-selection exception:

```text
selector == 0 && style_code == 15
    ? HeavenHoldNote
    : AirLadderNote
```

Style code 15 is exact executable string `NON`. The ordinary AirLadder path
allocates `0x11c` bytes and owns its runtime checker vector and resolved count.
The HeavenHold branch is a separate gameplay class and is not silently modeled
as AirLadder. A missing style token produces the accessor's empty string and
style code 0; legacy one-short ALD records therefore remain ordinary
AirLadder. The exceptional path is normative in
`spec/notes/heaven_hold.md`. Evidence: `claim.note.air-ladder-judgement` and
`claim.note.heaven-hold-judgement`.

## Checkpoint construction and input

AirLadder load creates one 0x88-byte runtime record for each parsed control
point. Every record owns the shared retained-profile timing checker initialized
with derived input profile 7. Profile 7 is the inclusive externally configured
scalar range synthesized from the six photo-sensor inputs and retained in the
301-snapshot history. Its numeric range and timing windows remain external
parameters; the physical source framing, synthesis, and fixed profile index
are normative.

AirLadder exposes no lane candidate. It neither constrains nor reads the active
manager's lane-candidate reduction.

## Gameplay update and completion

One gameplay update visits every runtime record in order. A record whose
retained checker state is below 2 is updated. If the resulting state is above
1, AirLadder stores its result, submits source category 18, increments the
resolved count, and continues iterating. Multiple checkpoints can therefore
resolve in the same substep; this is not a front-only queue.

Category 18 maps to shared category 7 and is aggregate-authoritative under
normal valid runtime bounds. Existing terminal-route state sends the event to
the shared observer-only path.

The runtime also copies an external completion threshold selected through the
parsed configuration key. For ordinary finite values, deferred terminal
transition is requested only when:

```text
current >= completion_threshold
&& resolved_count == checkpoint_count
```

Equality at the threshold passes. The presentation half of the update does not
submit results. Construction/reset clears the vector and counters; destruction
releases all checker records and resources. Reconstruction:
`select_ald_runtime`, `air_ladder_new_resolution_count`,
`air_ladder_is_terminal`, and the `air_ladder_*` constants; focused tests:
`tests/air_ladder_judgement_test.cpp`.
