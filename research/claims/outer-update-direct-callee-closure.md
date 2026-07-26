# Claim: the outer update's direct-callee and transition surface is closed

- ID: `claim.pipeline.outer-update-direct-callee-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `state.ownership`,
  `audit.indirect_calls`
- Last reviewed: 2026-07-26

## Statement

All 44 direct callees of `FUN_00da9820` are classified. Every write into a
gameplay owner belongs to an already-reconstructed clock, input/note,
materialization, result, or periodic-aggregate path, except for the newly
documented pre-active dispatch distinction. The remaining callees and indirect
targets are one-way presentation/effect sinks, accessors, conversions, or
read-only predicates. Neither the parent nor any direct callee writes the scene
state field, so all structural exits and transitions occur in the nine
registered callers after the outer update returns.

## Anchors

- `game.exe @ RAM:00da9820, FUN_00da9820, direct-callee owner,
  hash ce8c5ca2608c3b0e7c39932b2e6042056865380a6824f611ea0342be79c90f1c`
- `game.exe @ RAM:00b1d880, FUN_00b1d880, fixed effect-list update,
  hash e6500a80c166c7029bc3559937ee028e71f74430c84b8c6b2f0f8f0d8653518d`
- `game.exe @ RAM:00b18370 and RAM:00b18280, EffectManager and EffList construction`
- `game.exe @ RAM:00b1d7e0, FUN_00b1d7e0, resolved EffList update slot`
- `game.exe @ RAM:00ce0ce0, FUN_00ce0ce0, alternate presentation-resource start,
  hash 62272f096ae68abc6ef9bb557410fa99ce2478f10547f28dc654707790edfbfe`
- `game.exe @ RAM:00da26b0 through RAM:00da2630, complete caller registration chain`

## Observations

- Core gameplay/control callees are `RAM:00afd040`, `00b00be0`, `00b2b690`,
  `00b97b60`, `00b98330`, `00b983e0`, `00b984e0`, `00b98650`, `00b98680`,
  `00cec200`, `00da62c0`, `00da6850`, `00da70c0`, `00da8730`, and
  `00da8cf0`. Their selector, manager, rule-mask, participant, metadata, and
  aggregate behavior is covered by the active gameplay clock, substep,
  materialization, result-routing, alternate-meter, and periodic-aggregate
  claims.
- Accessor/timer/conversion callees are `RAM:00a366b0`, `00a84a20`,
  `00a84c90`, `00af8990`, `00af9610`, `00b4e440`, `00b8ee50`, `00b8ef70`,
  `00b8faa0`, `00b98040`, `00b98050`, `00c41340`, `00c414f0`, `00e0a43e`,
  and `01084650`. Returned values either feed already-authoritative owners or
  the one-way scene export. Ignored completion/ready returns do not create a
  transition inside the parent.
- Presentation/effect callees are `RAM:00b1d880`, `00b30170`, `00b301d0`,
  `00cdfaa0`, `00ce0ff0`, `00d379a0`, `00d379c0`, `00d379f0`, `00d37a80`,
  `00d383b0`, `00d383e0`, `00d384e0`, `00da5bd0`, and `00da8420`. The
  KeyBeam, meter/grid, scene-resource, and cue paths are already bounded.
- The previously unowned `FUN_00b1d880` walks exactly ten pointers created by
  `projView::EffectManager` construction. Each points to an embedded
  RTTI-identified `EffList`; its fixed virtual update target visits effect
  objects, removes state-3 entries, retains a peak count, and clears a
  per-frame field. The ten initialized list identities are Bomb, Reaction,
  Text, Continue, AirRing, SonicBoom, CharaNote, CharaBG, Mine, and MineBG.
  No effect-list field aliases the parser, input history, active-note vectors,
  result owner, gameplay clock, or scene-state field.
- `FUN_00ce0ff0` forwards to `FUN_00ce0ce0`. That helper selects one of several
  fixed audio/effect resources only while its own handle is zero, stores the
  handle, and starts backend presentation state. Its complete store set has no
  gameplay or structural-transition owner.
- The parent has one explicit external virtual read used only as an input to a
  scene-resource setter. Cue-controller virtuals reached through
  `FUN_00a84a20`/`FUN_00a84c90` remain inside the bounded cue/report owner.
- The complete thunk-xref set for the parent has nine callers. They are the
  registered update callbacks for states `0x0a` through `0x12`; every scene
  state write occurs after the call returns. States `0x13` and `0x14` have no
  parent call.

## Reasoning

The direct-callee inventory accounts for every static call edge. Store-set and
owner checks reduce each edge either to a cited gameplay reconstruction or to
a closed one-way sink. The independently enumerated caller set proves that the
parent has no hidden alternate return target: structural transition decisions
are caller-owned and follow the update.

## Alternatives and falsifiers

- Competing explanation: an effect callback, cue backend, ignored predicate,
  or scene export aliases a gameplay owner or writes a structural state.
- Evidence that would disprove this claim: a new producer into an input,
  active-note, clock, result, or scene-state field from one of the excluded
  paths; another direct callee or parent caller; or an unresolved virtual
  target whose store set escapes its presentation owner.

## Unknowns

- Player-facing names of several selector/accessor fields remain deliberately
  unassigned.
- External cue, score-rank, timing-offset, and rule-table values remain
  unavailable parameters as documented by their owning claims.
- Presentation backend failure behavior is outside the successful gameplay
  outcome path.

## Consequences

- Ghidra mutations: added plate comments at `RAM:00da9820`, `00b1d880`, and
  `00ce0ce0`; callback function recovery and comments are recorded in
  `claim.pipeline.pre-active-zero-base-priming`.
- Spec sections: `spec/timing.md`.
- Reconstruction code: no separate implementation beyond the pre-active
  structural dispatch; all gameplay-affecting direct callees already map to
  existing reconstruction units.
- Tests: `tests/gameplay_exit_test.cpp`.

## Verification

Native Ghidra callee enumeration returned 44 unique direct thunk targets. The
set was checked against the parent's full decompilation, existing claim
anchors, both previously unowned presentation paths, all nine caller xrefs,
and the callback registration chain. Re-decompilation after function recovery
confirmed the state/argument partition and absence of an in-parent state write.
