# Claim: cross-family candidate fanout and simultaneous results follow active-vector order

- ID: `claim.interactions.cross-family-candidate-result-order`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `matching.candidates`, `matching.priority`, `interactions.cross_note`, `pipeline.boundaries`
- Last reviewed: 2026-07-27

## Statement

The active-note manager completes candidate preparation and minimum reduction
over the primary vector before updating any note, then updates every primary
in forward primary-vector order and every attachment in forward secondary-
vector order without consuming the selected candidate or rising edge.
Consequently equal TAP, CHR, unresolved HOLD, unresolved Slide, and unresolved
HeavenHold starts can all accept the same edge when their local checkers accept;
FLK also accepts independently of the selected minimum. Same-pass result events
are dispatched in primary-first/secondary-second vector order, so an earlier terminal event can
observer-route later events without preventing their local completion.

## Anchors

- `game.exe @ RAM:00b2b690, FUN_00b2b690, candidate/update/removal passes, hash bed7892c43aec7a948d83aa2d146cbf6c46fa1ade2a95847409152f529cd84e9`
- `game.exe @ RAM:00da62c0, FUN_00da62c0, pending queue scan, hash cb798253647b9f10eed6cd3d3aeaa4417cbc410ea3682481f762a20829cfa27b`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, root/secondary factory append, hash ea011be474fda85c8cad0ca59b2c5f27d46a17a6652204e3930fa1aeb9674681`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, shared equality/input gate, hash 9464761fd32bf91b11a85f074396ae85cfeee1dd0a271347cce81e80f913e0b3`
- `game.exe @ RAM:00c29c10, FUN_00c29c10, HOLD candidate wrapper, hash 42ff3459daafa907159ea6b55b69484d1dd2fce78b8f566bb7b20150225440dc`
- `game.exe @ RAM:00c0dae0, FUN_00c0dae0, Slide candidate wrapper, hash 42ff3459daafa907159ea6b55b69484d1dd2fce78b8f566bb7b20150225440dc`
- `game.exe @ RAM:00c13e30, FUN_00c13e30, HeavenHold candidate wrapper, hash d87f5a348b847759b101ab39e504a250407175d87d0de3a347ec8444ab25fb5c`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared result dispatch`

## Observations

- The manager clears 16 selected lanes, calls the candidate virtual for every
  active primary in forward vector order, and reduces each lane to its smallest
  nonnegative value. Only after that pass does it call every live object's
  update virtual in the same forward order.
- TAP and CHR use the shared selected-candidate equality gate. HOLD, Slide, and
  HeavenHold candidate wrappers expose their embedded shared checker until the
  start phase reaches 4, and their update paths delegate unresolved starts to
  the same shared gate.
- Those gates read but do not clear the selected lane or newest logical rising
  edge. Result dispatch also has no candidate/input clear, and terminal removal
  occurs only after the complete update pass.
- FLK contributes a candidate while awaiting its edge but its edge path does
  not read the selected lane. Ordinary edge acceptance starts motion tracking
  and cannot emit FLK's later motion result on that same substep.
- Gameplay setup queues final parsed indices in order. Each materialization
  scan visits the pending vector forward; existing objects remain ahead of new
  objects in each owner, newly supported roots append to the primary vector in
  scan order, and attachments append to a distinct secondary vector in their
  corresponding encounter order. Eligible factory-default records append
  nothing. Different eligibility updates can therefore make dynamic
  construction time, not merely chart index, decide order within either
  vector.
- Shared result dispatch handles each call synchronously. If an earlier event
  activates terminal routing, a later same-pass event still reaches its note
  finalizer and observer but no longer updates the authoritative aggregate.

## Reasoning

Because reduction is complete and read-only before any input gate, storage
order cannot break a candidate tie. Applying the one shared equality predicate
to all gated start families establishes conditional fanout across those
families, while FLK's missing equality read establishes its independent case.
The separate update passes and synchronous result call then make complete
primary-vector order followed by complete secondary-vector order the exact
downstream simultaneous-result order.

## Alternatives and falsifiers

- Competing explanation: each virtual family receives or consumes a private
  edge, or the manager stops after one accepted result.
- Evidence that would disprove this claim: a family-specific clear of the
  logical rising bit/selected candidate, a break from the update loop after
  terminal activation, a different start gate for an unresolved listed
  family, or an append path that inserts ahead of existing objects.

## Unknowns

- External per-family checker windows can make one tied local gate reject while
  another accepts; the claim is conditional on local acceptance.
- Attached secondaries remain ordered in their own vector and expose no lane
  candidate. They may submit results, but only after every primary update in
  the substep.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/matching.md`, `spec/timing.md`.
- Related claims: `claim.matching.tap-equal-candidate-fanout`,
  `claim.matching.flick-candidate-asymmetry`,
  `claim.pipeline.runtime-note-materialization-order`, and
  `claim.interactions.result-terminal-short-circuit`.
- Reconstruction code: `evaluate_lane_candidate_fanout`,
  `RuntimeActiveVectors`, `append_runtime_factory_events`, and
  `runtime_manager_update_order`.
- Tests: `tests/candidate_interaction_test.cpp`,
  `tests/shared_result_test.cpp`.

## Verification

Focused tests cover equal fanout across every candidate-producing start family,
per-note local rejection without edge consumption, unequal FLK asymmetry,
existing/root/secondary/factory-default two-vector append order, and ordered terminal
rerouting of a later same-pass result. The relevant family candidate wrappers,
shared input gate, manager loops, materializer, factory appends, and result
route were independently rewalked.
