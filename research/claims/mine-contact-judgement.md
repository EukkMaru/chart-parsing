# Claim: MNE integrates held contact and terminates when its threshold is reached or impossible

- ID: `claim.note.mine-contact-judgement`
- State: superseded
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `input.logical_state`, `matching.candidates`, `judgement.types`, `judgement.miss`, `note.other_variants`, `state.ownership`, `config.external`
- Last reviewed: 2026-07-22

Superseded by `claim.note.mine-contact-aggregate-judgement`. The contact state
machine below remains supported, but its downstream boundary stopped after the
first category conversion. Source category 17 reaches dispatch category 11,
which `FUN_00b97730` converts again to aggregate category 8; it is therefore an
authoritative aggregate event when its result byte is valid.

## Statement

Parsed type 11 constructs `projView::MineNote`, which exposes no lane
candidate and, after an external activation start, accumulates the clipped
previous-to-current timing interval whenever any covered logical lane is held.
It terminates successfully when accumulated contact reaches an external
requirement, or unsuccessfully once accumulated contact plus all remaining
time can no longer reach that requirement.

## Anchors

- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed type-11 factory case`
- `game.exe @ RAM:00c1ec90, FUN_00c1ec90, MineNote construction and state initialization`
- `game.exe @ RAM:018d9878, MineNote vtable, gameplay dispatch family`
- `game.exe @ RAM:01c32620, RTTI type descriptor, projView::MineNote identity`
- `game.exe @ RAM:00c1f940, FUN_00c1f940, load/reset path`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, candidate output remains all sentinels`
- `game.exe @ RAM:00c1ee00, FUN_00c1ee00, ordinary/forced contact and result state machine`
- `game.exe @ RAM:00c2de10, FUN_00c2de10, logical held-level input query`
- `game.exe @ RAM:00c1f690, FUN_00c1f690, local terminal-state lookup`
- `game.exe @ RAM:00c1f8f0, FUN_00c1f8f0, Mine result remap wrapper`
- `game.exe @ RAM:00c1c340, FUN_00c1c340, shared result and deferred terminal request`
- `game.exe @ RAM:00d7f030, FUN_00d7f030, source category 17 to mapped category 11`

## Observations

- The type-11 factory allocates a `0x1b0`-byte object and calls the constructor
  whose vtable is associated with the `projView::MineNote` RTTI descriptor.
  Construction and successful load both initialize local phase `+0xe8` and
  accumulated float `+0xec` to zero.
- Shared loading supplies the scheduled time, raw start/width, and bounded
  logical lane range. Mine's candidate virtual fills all 16 outputs with the
  negative sentinel. The load path does not initialize the shared TAP timing
  checker, and the gameplay update does not read a selected manager candidate.
- The update derives current and previous deltas from manager fields `+0x300`
  and `+0x304`, subtracting scheduled time, configuration field `+0x720`, and
  the same selected runtime correction from both.
- Once `current >= config[+0x724]`, the update scans the bounded lanes with the
  logical held-level query. If any lane is held, it adds
  `min(current, config[+0x728]) - max(previous, config[+0x724])` only when that
  difference is strictly positive. Lane count and held duration within the
  substep are not sampled; one or more held lanes credit the entire interval.
- In that same activation-start branch, resolution begins only when
  `current >= 0`. If `config[+0x72c] > accumulated +
  max(config[+0x728] - current, 0)`, phase becomes 2. Otherwise phase becomes 1
  when `accumulated >= config[+0x72c]`; equality therefore succeeds, while
  equality with the best possible total is not an early failure.
- Local phases 1 and 2 are terminal; phase 1 supplies provisional result byte
  0 and phase 2 byte 4. The result is sent through Mine's active-result remap
  wrapper with fixed source category 17 and a finalization flag. The shared
  handler requests base state 2 while base state 1 is current, so commit and
  manager removal follow the existing next-substep deferred lifecycle.
- Source category 17 maps to category 11. It reaches ordinary dispatch but is
  outside the authoritative aggregate's accepted mapped range 0 through 8. If
  terminal routing was already active, it instead follows the shared
  observer-only path.
- With manager forced-result state active, Mine waits for the separately
  configured forced point, presets phase 2, selects/submits a forced result,
  and changes local phase to 1 only when the selected byte is zero. A nonzero
  selected byte leaves phase 2. This branch reads no lane input.
- The remaining Mine-specific virtuals manipulate resources, feedback, or
  presentation vectors and do not write local phase, accumulated contact,
  candidate values, manager input, or authoritative aggregate state.

## Reasoning

The constructor/RTTI/factory chain identifies the object family, while its
vtable closes load, candidate, update, terminal, result, and destruction paths.
Direct comparison and branch inspection establishes the strict/equality
boundaries independently of decompiler condition recovery. The shared result
and base state paths establish the downstream category and lifetime behavior;
resource siblings have no backward write into the reconstructed state.

## Alternatives and falsifiers

- Competing explanation: Mine counts held lanes or input samples rather than
  elapsed contact interval.
- Evidence that would disprove this claim: a phase/accumulator write outside
  the recovered load/update paths, a physical-bank or rising-edge query in the
  ordinary outcome path, a non-sentinel candidate produced by Mine, or a source
  category other than 17 reaching the shared handler.

## Unknowns

- Values, units, ordering guarantees, defaults, and mode selection for
  configuration fields `+0x720`, `+0x724`, `+0x728`, `+0x72c`, and the forced
  timing field `+0xb04` are external and remain parameters.
- Player-facing names for local phases, result bytes, and source/mapped
  categories are not established.
- The active-result remap can transform the submitted result byte; its existing
  aggregate-boundary effect is irrelevant to category 17, but other observer
  or presentation consequences remain outside this claim.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/notes/mine.md`, `spec/input.md`, `spec/matching.md`,
  `spec/judgement.md`, `spec/configuration.md`, `spec/c2s.md`.
- Reconstruction code: `MinePhase`, `MineContactState`, and Mine helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/mine_contact_test.cpp`.

## Verification

The family lookup, factory case, RTTI, complete vtable, constructor/load resets,
current/previous timing math, logical input query, instruction-level float
branches, forced path, terminal table, result wrapper, shared category map,
base-state request, manager removal loop, destructor, and resource-only sibling
callbacks were checked independently. Focused tests cover clipping, activation
and zero gates, strict impossibility, equality success, terminal stability,
forced zero/nonzero selection, candidate absence, and source-category routing.
