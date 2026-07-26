# Claim: the post-manager scene-resource setters are presentation exports

- ID: `claim.pipeline.post-update-scene-resource-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `audit.indirect_calls`
- Last reviewed: 2026-07-22

## Statement

The setter cluster between KeyBeamManager and the bounded cue scheduler exports
already-computed manager, mode, and score-like values into two scene-owned
presentation resources. The setters only update resource values and dirty or
enable flags; no exported field feeds the gameplay clock, input synthesis,
active notes, candidate selection, judgement, result aggregation, or terminal
outcome.

## Anchors

- `game.exe @ RAM:00da3a10, FUN_00da3a10, scene resource acquisition at +0x68 and +0x6c`
- `game.exe @ RAM:00da9820, FUN_00da9820, post-manager export order and source selection`
- `game.exe @ RAM:00cdfaa0, FUN_00cdfaa0, +0x6c resource enable writer`
- `game.exe @ RAM:00d379a0, FUN_00d379a0, empty companion hook`
- `game.exe @ RAM:00d37a80 and RAM:00d379f0, changed-value/dirty setters`
- `game.exe @ RAM:00d383b0 and RAM:00d383e0, mode-value setters`
- `game.exe @ RAM:00d384e0 and RAM:00d379c0, score-like value setters`
- `game.exe @ RAM:00da00c0, FUN_00da00c0, initial sentinel export`
- `game.exe @ RAM:00da27f0, FUN_00da27f0, scene-exit zero export`
- `game.exe @ RAM:00d376d0, FUN_00d376d0, presentation-resource field reset`
- `game.exe @ RAM:018d3bf4 and RAM:0196d734, ForesterPlayer RTTI-backed presentation classes`

## Observations

- Scene construction obtains resource objects into scene fields `+0x68` and
  `+0x6c` from the process resource catalog and invokes their setup slot. The
  outer update supplies those exact two pointers as the implicit owners of this
  setter cluster.
- The neighboring `+0x6c` call has one caller. When a NotesManager flag is set,
  it writes byte one to the resource's nested field `+0x2d8`; the immediately
  following companion hook on `+0x68` is empty.
- The remaining six setters target the `+0x68` resource. Four store integers at
  nested offsets `+0x180`, `+0x184`, `+0x188`, and `+0x18c`; changed values at
  the first three set corresponding dirty bytes. Two more store a mode byte and
  associated scalar at `+0x1c0` and `+0x1c4`. All return void.
- Their current-update inputs are read from NotesManager getters, a selected
  external/controller virtual, mode predicates, and score-like controller
  getters. Complement and difference calculations occur before the setters;
  the resource never supplies a value back to those authoritative owners.
- The changed-value setters have exactly three callers: initial scene setup,
  the outer update, and a scene-exit callback. Setup writes large sentinels,
  exit writes zeros, and the outer update writes live values. The mode-only
  setters are called solely by the outer update.
- The field-reset helper initializes the same value/dirty region before driving
  a separate effect-player state controller. Nearby constructors and resource
  loaders instantiate RTTI-identified `star::ForesterPlayer` and
  `projView::ForesterPlayer` objects and configure render/effect channels. The
  resolved downstream operations do not reference gameplay owners.

## Reasoning

The outer call order makes every source value downstream of the completed
current note-manager pass. Scene construction and setup/exit callers establish
that the destination is resource lifetime state, while the setter bodies
restrict writes to values, dirty bytes, and an enable byte. The associated
player classes and their channels are presentation/effect sinks. Because the
input, active-note, result, terminal, and clock owners neither alias these
resource fields nor consume setter return values, this is a one-way state
export rather than a hidden feedback path.

## Alternatives and falsifiers

- Competing explanation: a gameplay controller reads one of the exported
  nested values or dirty flags on a later update and changes note processing.
- Evidence that would disprove this claim: a resolved resource alias or player
  callback that writes input history, active-note state, candidates, result
  aggregation/remapping, terminal outcome, or the play-clock correction/rate
  owners based on these fields.

## Unknowns

- Player-facing names for the individual exported values and effect parameters
  are not assigned.
- The external catalog keys selecting the two scene resources are not given
  gameplay semantics.
- Render/effect backend failure behavior is outside the successful outcome
  path.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remained unavailable,
  so analysis used the temporary static project clone. Missing vtable thunk
  functions were materialized only in that clone to resolve player sinks.
- Spec sections: `spec/timing.md` post-manager presentation exports.
- Reconstruction code: none; the setters do not affect gameplay outcomes.
- Tests: none; the exclusion is supported by call ordering, exact setter store
  sets, caller/reset closure, resource ownership, and downstream player sinks.

## Verification

Both scene resource acquisitions, the sole `+0x6c` writer, every `+0x68`
setter, their complete thunk caller sets, setup sentinels, exit zeros, shared
field reset, RTTI, vtable targets, and outer-update continuation were inspected.
No reverse edge into a gameplay owner was found.
