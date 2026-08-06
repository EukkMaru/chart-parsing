# Claim: the manager-loop counter clear is render-only and cannot affect judgement

- ID: `claim.pipeline.render-primitive-counter-boundary`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `audit.indirect_calls`
- Last reviewed: 2026-07-21

## Statement

The direct call at `FUN_00b2b690` between candidate preparation and runtime-note
updates clears only per-substep render-primitive counters. Their producers count
completed vertex triples, their retained fields are peak counters, and no
recovered path reads either value back into chart, input, note, result, timing,
or terminal state.

## Anchors

- `game.exe @ RAM:00b2b690, FUN_00b2b690, manager update and call-order boundary`
- `game.exe @ RAM:00c32750, FUN_00c32750, per-substep current-counter clear`
- `game.exe @ RAM:00c326e0, FUN_00c326e0, current-and-peak clear during scene initialization`
- `game.exe @ RAM:00c322b0, FUN_00c322b0, bounded primitive-count accumulator`
- `game.exe @ RAM:00c32330, FUN_00c32330, sole accumulator wrapper`
- `game.exe @ RAM:00c03c00, FUN_00c03c00, geometry builder and diagnostic-category 0/4-9 producer, hash d2a412eb53cc16a40064b5d44527874fe83f9bfdfb64575d6282df16038a7ed6`
- `game.exe @ RAM:00c08420, FUN_00c08420, geometry builder and bucket 0 producer`
- `game.exe @ RAM:00c09d60, FUN_00c09d60, geometry builder and bucket 0 producer`
- `game.exe @ RAM:00c0a3d0, FUN_00c0a3d0, geometry builder and bucket 1-3 producer`

## Observations

- `FUN_00c32750` iterates eleven eight-byte entries and zeros only each first
  dword. The companion `FUN_00c326e0`, called from scene initialization
  `FUN_00da2160`, zeros both dwords in every entry.
- The sole accumulator accepts category values below ten. It derives an amount
  from a submitted object's `+0x1dc..+0x1e0` vector size using a 24-byte vertex
  stride and division by three, then adds that triangle count to the selected
  category and a separate total. Each second dword becomes the maximum observed
  first dword.
- The accumulator wrapper's complete caller set is four large geometry-building
  routines. Their eleven call sites use only categories 0 through 9 and submit
  render-object pointers whose vertex containers they have just populated.
- The manager clear occurs after all primary candidate virtuals and before any
  primary runtime-note tick. It neither reads nor writes the manager object.
- Exact-address scans of the counter block and complete thunk caller sets found
  only the accumulator wrapper, scene full clear, and manager current clear.
  The nearby formatter serializes counter pairs into text but has no recovered
  direct, thunk, or pointer caller. No consumer reaches gameplay-owned state.

## Reasoning

The accumulator's vertex-stride arithmetic, category-bounded render producers,
per-substep/scene reset split, and current/maximum pairing identify diagnostic
render accounting. More importantly for the boundary, the closed reference set
contains no read that can feed a gameplay branch. Its position inside the
manager loop therefore creates no hidden candidate or judgement ordering rule.

## Alternatives and falsifiers

- Competing explanation: the cleared values are a per-tick judgement-event
  accumulator used after note updates.
- Evidence that would disprove this claim: a read of the counter block by a
  gameplay-state owner, a producer not derived from render vertex containers,
  or an indirect formatter/consumer that writes a value back into note, input,
  result, timing, or terminal state.

## Unknowns

- Player-facing/debug names for the ten render categories are not needed for
  the gameplay boundary and remain unassigned.

## Consequences

- Ghidra mutations: supported-role comments at `00c322b0` and `00c03c00`
  distinguish diagnostic triangle counting from the separate Joint graph
  submission path; default symbols and types were retained.
- Spec sections: `spec/timing.md` manager update ordering exclusion.
- Reconstruction code: none; excluded render diagnostics are outside scope.
- Tests: none; the exclusion is supported by a closed static reference set.

## Verification

The accumulator's sole thunk, all eleven call sites, both reset functions, and
the counter block's exact-address references were independently enumerated.
The producers' vertex-container arithmetic was checked at each call site, and
no gameplay feedback consumer was recovered. Current exact-project MCP review
also followed type-9 geometry onward to the separate Joint GraphNode callback
and dynamic-primitive command queue, confirming that the 7/9/8 counter calls
are not submissions.
