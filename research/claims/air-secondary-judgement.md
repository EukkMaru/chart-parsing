# Claim: AIR attaches a candidate-free secondary object with retained-profile timing judgement

- ID: `claim.note.air-secondary-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.events`, `input.logical_state`, `input.buffering`, `matching.candidates`, `judgement.types`, `judgement.windows`, `judgement.miss`, `note.air`, `state.ownership`, `config.external`, `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-26

## Statement

The six AIR-family commands attach parsed secondary type 3 and a mirrored
direction code to a compatible root record rather than creating a standalone
record. The runtime factory creates a separately updated `projView::AirNote`
after the root; it exposes no lane candidate and resolves an external-profile
input through a retained-best timing state machine before submitting source
category 7 or 8.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, AIR-family parser attachment case`
- `game.exe @ RAM:019669a4, fixed direction-code pairs for AIR/AUR/AUL/ADW/ADR/ADL`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, root and secondary runtime factory`
- `game.exe @ RAM:00c1aa80, FUN_00c1aa80, root-to-secondary metadata link`
- `game.exe @ RAM:00c1dc50, FUN_00c1dc50, AirNote construction and phase reset`
- `game.exe @ RAM:018d97d4, AirNote vtable, gameplay dispatch family`
- `game.exe @ RAM:01c325f8, RTTI type descriptor, projView::AirNote identity`
- `game.exe @ RAM:00c1e730, FUN_00c1e730, root-relative anchor and secondary load`
- `game.exe @ RAM:00c18f10, FUN_00c18f10, root/direction profile selection`
- `game.exe @ RAM:00c18550, FUN_00c18550, external AIR checker initialization`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, all-sentinel candidate output`
- `game.exe @ RAM:00c1acf0, FUN_00c1acf0, ordinary/forced AIR timing gate`
- `game.exe @ RAM:00c17ce0, FUN_00c17ce0, retained-profile classifier`
- `game.exe @ RAM:01bdd668, retained classifier result/rank table`
- `game.exe @ RAM:00c2f200, FUN_00c2f200, newest-snapshot marker write`
- `game.exe @ RAM:00c2ddf0, FUN_00c2ddf0, derived profile-byte read`
- `game.exe @ RAM:00c2dfa0, FUN_00c2dfa0, marker consumer in next snapshot build`
- `game.exe @ RAM:00c1de20, FUN_00c1de20, terminal result and direction category`
- `game.exe @ RAM:018d986c, direction-to-source-category flag table`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared result and deferred terminal request`

## Observations

- The type-3 parser case scans existing `0x174`-byte root records. A match
  requires an unassigned/out-of-range secondary type, the requested root type,
  equal start lane and width, and equal chart position through the parser's
  epsilon comparison. On success it stores secondary type 3, optional ordering
  index, a parsed property, and a direction code on that root. No new root
  record is appended. Failure to find a match follows the parser diagnostic and
  false-return path.
- Direction codes are fixed as AIR `0`, AUR/AUL `1/2`, ADW `3`, and ADR/ADL
  `4/5`. Mirroring swaps AUR with AUL and ADR with ADL; AIR and ADW are stable.
- The main factory has no standalone parsed-type-3 root case. For any supported
  root whose secondary field is 3, it allocates a `0x170`-byte object, calls the
  constructor with the `projView::AirNote` RTTI/vtable family, loads it through
  virtual slot `+0x24`, stores it at root `+0xe0`, then appends the root followed
  by the AirNote to the active vector. The link is used by metadata propagation;
  AirNote outcome and destruction do not dereference a root backpointer.
- AirNote load copies direction code to local `+0xec` and initializes its
  checker. Root types 1, 2, and 13 select profile base 0; types 0, 4, and 11
  select base 2; type 6 selects base 4. Direction codes 0 through 2 use the
  base, while codes 3 through 5 use `base + 1`. Other root types do not enter a
  profile-initializer case.
- Scheduling is anchored to the root start for types 0, 4, 6, and 11, the root
  end for type 1, and the last path/control point for types 2 and 13. These
  choices affect AIR timing but do not make the root own AIR's outcome state.
- The selected profile initializes one embedded checker from an external
  `0x3c`-stride configuration group, adds selected runtime correction plus a
  global and per-profile offset, and selects input source indices 0 through 5.
  Endpoint values are not executable constants.
- AirNote's candidate virtual leaves all 16 entries at the negative sentinel.
  Its local phase initializes to 0; its terminal predicate tests phase 3. The
  outcome path is independently invoked from the active vector and has no
  selected-lane-candidate comparison.
- Once the current delta is strictly greater than the checker's outer lower
  endpoint, AIR first sets byte `+0x54` in the newest `0x58`-byte input
  snapshot, then reads its selected byte from the distinct range
  `+0x4c..+0x53`. Snapshot initialization clears `+0x54`; the following input
  build reads it while deriving profile state. Therefore the marker does not
  force or overwrite AIR's current sampled byte.
- Input samples can replace retained fine index `+0x40` only when the current
  table rank is at least the retained rank. Before the pivot, the classifier
  returns nonaccepting index 1 while still retaining such input. At/after the
  pivot it returns retained center index 6 immediately; otherwise it emits a
  retained result when its rank is greater than the current timing location,
  emits a currently sampled nonworse result, or waits at index 1. If no center
  wins, reaching the outer upper endpoint produces index 11.
- The rank order for fine indices 0 through 11 is
  `0,1,4,6,8,10,11,9,7,5,3,2`. Indices 2 through 11 have nonzero acceptance;
  0 and 1 do not. Coarse tiers are `0,0,0,1,2,3,4,3,2,1,0,0`, and side codes
  are `0,0,1,1,1,1,0,2,2,2,2,2`.
- Ordinary completion additionally requires the pivot to have been reached.
  Forced-result mode can supply input and its selected result at the external
  forced point, but it still crosses the shared early/pivot gates. Final
  coarse/side data is canonicalized back to a fine index after the active
  result remap.
- A completed result sets local phase 3 and submits through the shared handler.
  Direction codes 0 through 2 use source category 7; codes 3 through 5 use 8.
  Both map to category 3 and are aggregate-authoritative for a valid result
  byte unless the shared terminal route is already active, in which case the
  event is observer-only.
- The shared handler requests base state 2 while current state 1 remains. Commit
  and manager removal occur on the next normal note tick. Root-before-secondary
  vector order allows a root result to activate terminal routing before its AIR
  result in the same pass; AIR can also remain active after its root is removed.

## Reasoning

The parser match and factory append order close the apparent missing type-3
factory case: type 3 is secondary metadata, not a root family. RTTI and the
complete vtable connect that secondary allocation to load, candidate, outcome,
terminal, and destruction paths. The checker instructions and fixed table
separately establish retention, strict/equality boundaries, and result
metadata. Distinct snapshot offsets disprove the initial possibility that the
pre-read marker directly forced current AIR input.

## Alternatives and falsifiers

- Competing explanation: an AIR command creates or judges a standalone lane
  note, or the `+0x54` marker itself is the accepted AIR input.
- Evidence that would disprove this claim: a type-3 root allocation path, a
  non-sentinel AirNote candidate, a current-profile read from `+0x54`, a phase
  write outside construction/outcome paths that changes terminal semantics, or
  a root lifetime dereference required by AirNote judgement.

## Unknowns

- `claim.input.touch-photo-input-framing` and
  `claim.input.snapshot-profile-synthesis` close the photo-sensor source
  ordering, complete profile synthesis, marker effect, and history ownership.
  External conditioning and motion values remain parameters.
- External checker endpoints, corrections, global/per-profile offsets, forced
  timing value, defaults, units, and player-facing mode names remain parameters.
- Parsed property `+0xa0` selects presentation state but has no recovered
  outcome effect. Its authored meaning is unresolved.
- Player-facing judgement names for fine indices, coarse tiers, side codes, and
  categories are not established.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/notes/air.md`, `spec/c2s.md`, `spec/input.md`,
  `spec/matching.md`, `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `AirCommand`, `AirDirectionCode`, `AirProfileSnapshot`,
  `AirTimingState`, `update_air_timing`, and AIR helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/air_judgement_test.cpp`.

## Verification

Parser success/failure paths, mirror tables, factory default and secondary
branches, root/secondary append order, RTTI, constructor/load reset, profile
selection, anchor selection, candidate virtual, checker input and forced paths,
snapshot marker writer/initializer/consumer, retained table, result category,
shared terminal request, and relevant sibling virtuals were checked
independently. Focused tests cover mirror mapping, all profile groups, marker
separation, strict early boundary, pre-pivot retention, noncenter decay,
center-at-pivot completion, late endpoint, candidate absence, terminal phase,
and shared category routing.
