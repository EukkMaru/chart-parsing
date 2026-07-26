# Claim: the post-judgement key-beam pass is feedback-only

- ID: `claim.pipeline.key-beam-feedback-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `audit.indirect_calls`
- Last reviewed: 2026-07-21

## Statement

The `projView::KeyBeamManager` pass after the input/manager update, lazy note
materialization, and meter-grid rendering consumes the completed logical input
snapshot plus note-authored lane markers only to update lane-beam view state
and keyed feedback resources. Its retained lane state does not feed input
synthesis, note dispatch, candidate selection, judgement, results, or terminal
state on the current or next update.

## Anchors

- `game.exe @ RAM:018d3fe0, RTTI .?AVKeyBeamManager@projView@@, owner identity`
- `game.exe @ RAM:00b2d6c0, FUN_00b2d6c0, manager construction and retained-state initialization`
- `game.exe @ RAM:00b2e5b0, FUN_00b2e5b0, full view/resource reset`
- `game.exe @ RAM:00b2e920 and RAM:00b2e960, transient lane-marker writers`
- `game.exe @ RAM:00b2f830, FUN_00b2f830, grouped beam-span writer`
- `game.exe @ RAM:00b30170, FUN_00b30170, per-update transient reset`
- `game.exe @ RAM:00b301d0, FUN_00b301d0, post-update lane sampling and output dispatch`
- `game.exe @ RAM:00c2de10 and RAM:00c2db20, logical-input snapshot source`
- `game.exe @ RAM:00b2fcf0, FUN_00b2fcf0, retained beam-view state and render dispatch`
- `game.exe @ RAM:00b30080, FUN_00b30080, keyed feedback-state dispatch`
- `game.exe @ RAM:00b15ce0 and RAM:00bf87f0, conditional keyed resource selection`
- `game.exe @ RAM:00da8730 and RAM:00da9820, reset/update ordering`

## Observations

- The owner is allocated once as a 184-byte object and its embedded RTTI names
  it `projView::KeyBeamManager`. Its only virtual slot is destruction; clients
  reach its concrete methods through the process-global owner pointer.
- Complete direct references to that global owner consist of construction and
  destruction, full setup/teardown resets, gameplay note/view callbacks that
  set lane spans or the three effect bytes at `+0xac..+0xae`, the per-update
  transient reset, and the one post-update pass. The input synthesizer has no
  owner reference.
- Both outer-update branches call `FUN_00b30170` before input synthesis and the
  active-note manager. It clears transient lane marks and overrides. Concrete
  note callbacks then repopulate those fields while the manager ticks notes.
- After both branches converge, the outer update performs lazy materialization
  and meter-grid rendering before its sole call to `FUN_00b301d0`.
  `FUN_00b301d0` obtains each of the 16 held-lane values through
  `FUN_00c2de10(lane, 0)`. That accessor selects an already-produced snapshot
  through `FUN_00c2db20`; it does not read KeyBeamManager storage.
- The post pass combines the sampled held mask, transient note markers, grouped
  lane spans, parser mode flags, and its own prior output state. It writes only
  KeyBeamManager's deduplication/smoothing fields before invoking
  `FUN_00b2fcf0` and `FUN_00b30080` per lane.
- `FUN_00b2fcf0` maps changed lane states to beam/view callbacks and projection
  or render helpers. `FUN_00b30080` maps the same finite state codes to keyed
  feedback booleans; its terminal chain conditionally selects or stops resource
  entries. These calls are void, and no value returns to the outer update.
- The complete owner-reference set contains no later reader that transfers its
  held mask, lane markers, output arrays, smoothing state, or effect flags into
  the input history, active-note manager, chart/parser, result controller, or
  gameplay scene state.

## Reasoning

RTTI identifies the subsystem, while the reset-before-manager and
sample-after-manager order establishes its direction of flow. The independent
input snapshot owner is upstream: KeyBeamManager reads the completed snapshot
but is absent from snapshot production. Its only downstream paths terminate in
view callbacks and keyed resource control, and its retained values are read
only to deduplicate or smooth later feedback. Therefore the note callbacks that
populate this manager are observable feedback side effects, not hidden
judgement state or a one-update input feedback loop.

## Alternatives and falsifiers

- Competing explanation: a retained beam lane or keyed resource state is read
  by the next input-synthesis pass and changes the logical held/edge masks.
- Evidence that would disprove this claim: an owner alias or downstream
  resource callback that writes the input snapshot/history, active-note
  candidates, clock, result aggregation, terminal controller, or a return value
  that gates any later gameplay call.

## Unknowns

- Exact player-facing beam-state names and resource identities remain outside
  gameplay scope.
- Backend render/resource failure behavior is not assigned to the successful
  finite gameplay path.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary static project clone.
- Spec sections: `spec/timing.md` post-judgement feedback boundary.
- Reconstruction code: none; KeyBeamManager state is presentation feedback.
- Tests: none; the exclusion is supported by owner-reference, caller-order,
  input-source, store-set, and terminal-consumer closure.

## Verification

The RTTI owner, constructor/reset paths, complete global-owner references,
concrete note-side writers, both pre-manager reset sites, the sole post-update
caller, input snapshot accessors, lane-state outputs, keyed-resource chain, and
outer-update continuation were inspected independently. No reverse edge from
KeyBeamManager into gameplay-owned state was found.
