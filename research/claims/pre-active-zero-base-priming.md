# Claim: five pre-active states run zero-base gameplay priming updates

- ID: `claim.pipeline.pre-active-zero-base-priming`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.clock`,
  `timing.discontinuity`, `input.buffering`, `state.ownership`,
  `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-26

## Statement

The registered update callbacks for structural scene states `0x0a` through
`0x0e` invoke the complete outer gameplay update with argument zero before
active state `0x0f`. That argument clears the accumulated gameplay-clock
fields and supplies a zero-initialized physical-input sample, but it does not
bypass active-note state dispatch, candidate preparation, lazy runtime-note
materialization, removal, or the final kind-2 aggregate reevaluation.

States `0x0f` through `0x12` invoke the same outer update with argument one.
States `0x13` and `0x14` do not invoke it.

## Anchors

- `game.exe @ RAM:00da9820, FUN_00da9820, complete outer update, hash ce8c5ca2608c3b0e7c39932b2e6042056865380a6824f611ea0342be79c90f1c`
- `game.exe @ RAM:00da26b0, RAM:00da2730, RAM:00da25b0, and RAM:00da2630,
  consecutive scene callback registration`
- `game.exe @ RAM:00da2c80, FUN_00da2c80, state-0x0a update, hash 45a120c121f88898cb76c5e780fcf7cf0414663e343012b9edbaf86064909616`
- `game.exe @ RAM:00da3150, FUN_00da3150, state-0x0b update, hash 2f85d72e38c459d7703830607f0000dfa4e807d62562bb63a270799bcce936f0`
- `game.exe @ RAM:00da32e0, FUN_00da32e0, state-0x0c update, hash 003a4396db2772c74541de25821acc72af1bf757eabcb64f1683757933700e58`
- `game.exe @ RAM:00da33f0, FUN_00da33f0, state-0x0d update, hash 7fb7e18bb0493d84bb2b51a327753ae72a4f0dd419079225d487e136de0efbe3`
- `game.exe @ RAM:00da3420, FUN_00da3420, state-0x0e update, hash 8cf57cb435089370db623e37f22f19ba5311c5c20b53299e4d7f90da85df8832`
- `game.exe @ RAM:00da6850, FUN_00da6850, false-argument clock reset,
  hash 3398b5e53561cdee93f8519a77c4d2db07a014427ac0b4108fa6e2469b9253eb`
- `game.exe @ RAM:00da8730 and RAM:00da70c0, ordinary and alternate input paths`
- `game.exe @ RAM:00c2d7e0, FUN_00c2d7e0, ten-dword input-sample clear`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, complete note-manager pass,
  hash bed7892c43aec7a948d83aa2d146cbf6c46fa1ade2a95847409152f529cd84e9`

## Observations

- The complete thunk-xref set for `FUN_00da9820` contains nine update
  callbacks. Their callback-table slots are based at owner offset `+0x04` with
  twelve bytes per scene state, fixing their structural state numbers.
- The five callbacks in slots `0x0a` through `0x0e` pass zero. The active
  callback in slot `0x0f` and the three post-active callbacks in slots `0x10`
  through `0x12` pass one. The registered updates for `0x13` and `0x14` do not
  call the outer update.
- With argument zero, `FUN_00da6850` clears the raw, prior, smoothed, prior
  smoothed, and accumulated clock fields. The ordinary catch-up path can still
  apply its already-reconstructed `-1`/zero substep offsets to that zero base;
  the alternate path has no substep offset.
- Both input paths construct their ten-dword sample as all zeroes before the
  argument-controlled physical-bank population. A false argument therefore
  appends a real zero-level/zero-edge input snapshot rather than skipping
  input history.
- The note-manager call, pending-record materialization, meter/grid and effect
  passes, scene exports, cue scheduler, converted-position setter, and kind-2
  aggregate reevaluation remain on the converged path for both argument
  values. Within the note manager, the argument controls only a later
  audio/effect trigger helper; candidate collection, primary/secondary note
  ticks, result dispatch reached by those ticks, and removal run regardless.
- The state-`0x0e` callback performs its priming update before sampling its cue
  handle and requesting active state `0x0f`. There is no separate clock/input
  enable between that outer update and the state write.

## Reasoning

The callback registration fixes the caller identities and state numbers
independently of the individual transition conditions. The false-argument
branches reset clock state and suppress physical input population, while the
shared continuation still reaches every gameplay owner. These calls therefore
prime runtime-note construction and state machines at a zero base; they are
not presentation-only frames and cannot be omitted from reconstruction.

## Alternatives and falsifiers

- Competing explanation: the pre-active calls update only views, or the false
  argument prevents note state machines and result rules from running.
- Evidence that would disprove this claim: a false-argument branch around the
  note manager, materializer, or aggregate evaluator; a nonzero physical-input
  producer after the sample clear; another caller outside states `0x0a`
  through `0x12`; or an entry to state `0x0f` that precedes the state-`0x0e`
  outer update.

## Unknowns

- Presentation and external readiness conditions determine how many priming
  updates occur before state `0x0f`; their player-facing identities are not
  assigned.
- The ordinary AutoScan successful-update counter can make the first priming
  call include its bounded prior substep. Its increment and modulo-`2^32`
  behavior are closed by `claim.timing.autoscan-counter-wrap`.
- This claim introduces no special rule for zero- or negative-scheduled notes.
  They follow their already-reconstructed ordinary state machines at the
  priming positions.

## Consequences

- Ghidra mutations: created missing callback functions at `RAM:00da2c80`,
  `00da2f30`, `00da3150`, `00da3140`, `00da33f0`, and `00da3420`; added
  structural-state plate comments at the parent and five pre-active callbacks.
- Spec sections: `spec/timing.md`, `spec/judgement.md`.
- Reconstruction code: `OuterGameplayUpdateMode` and
  `outer_gameplay_update_mode_for_scene_state` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/gameplay_exit_test.cpp`.

## Verification

The nine caller xrefs were independently matched to consecutive callback
registrations, then re-decompiled after recovering the six missing live-project
function boundaries. Focused assertions cover every structural state from
before `0x0a` through after `0x14`, including the priming/live/none partition.
