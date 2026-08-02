# Claim: an adversarial same-investigator pass found no new closed-path contradiction

- ID: `claim.audit.adversarial-saturation-review`
- State: superseded
- Maturity: supported
- Confidence: medium
- Owner: codex-root
- Coverage rows: `audit.closure`
- Last reviewed: 2026-07-27

## Statement

This claim is superseded by `claim.audit.independent-closure-review`. Its
same-investigator review missed a live contradiction: SLA-selected tags reach
the pending-note materialization predicate through STP/SFL/SLP schedules. It
also omitted HXD's external checker-profile selection and the group-1 schedule
producers from the normative reconstruction.

## Binary identity and coverage snapshot

- Target: `game.exe`
- SHA-256:
  `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`
- Ghidra project/program: local `chart` / `game.exe`
- Ledger before this report: 28 rows; 26 `mapped`, one `unknown`
  (`input.transport`), and this one `investigating`.
- Claim inventory before this report: 62 active claims (46 reconstructed,
  12 supported, four verified) and three preserved superseded claims.
- Every active claim file was referenced by at least one coverage row; no
  coverage row referenced a superseded claim.

## Corpus compatibility snapshot

`scripts/harness.py corpus` found 2,140 song directories, 7,752 readable charts,
2,140 metadata files, 2,140 jacket files, and zero unreadable charts. Declared
versions are `0.00.00`, `1.01.00`, `1.07.00`, `1.08.00`, `1.10.01`,
`1.11.00`, `1.12.00`, and `1.13.00`.

The aggregate vocabulary has 111 spellings. Binary registry and lookup
evidence partitions it into 91 live exact descriptors plus twenty ignored
legacy `T_PROG_00..95` commands. Direct aggregate checks reconfirmed that
`ASO`, `HHD`, and `HHX` are absent from the corpus; their binary-supported
compatibility paths remain separately documented.

## Entry and exit boundaries reviewed

- Entry begins with the process catalog's selected chart path, literal
  `.xml`-to-`.c2s` replacement, destructive chart reset, exact descriptor
  lookup, fixed parser passes, and successful-load postprocessing.
- Accepted parsed records enter an ordered pending-index queue. Lazy
  materialization occurs after the current outer update's input/note substeps;
  the factory/type switch and twelve reachable runtime vtables are enumerated.
- Each ordinary substep derives input before candidate/note work. Result
  submission reaches the two-stage category routing, aggregate and terminal
  producers, active-gameplay exit predicate, and post-active drain.
- State `0x13` is the first boundary that stops the complete outer update and
  tears down both runtime-note vectors. Final result persistence precedes
  later scene reset and report presentation.

## Indirect-call rewalk

Representative instruction inventories were repeated for
`FUN_011c7980`, `FUN_011c8870`, `FUN_00da62c0`, `FUN_00b28cc0`,
`FUN_00da9820`, `FUN_00da8730`, and `FUN_00b2b690`.

- The parser orchestrator and materializer have no object-computed calls in
  their controlling loops. The group-2 handler's indirect absolute calls are
  imported runtime helpers on conversion/error paths, not event dispatch
  targets; its complete parsed-type switch remains `0..6, 8..13`.
- The materialization factory's two object calls are the already-enumerated
  load/setup slots on a factory-selected runtime class.
- The outer update has one object-computed call, the already-bounded external
  catalog/resource read whose return is exported one-way to a scene setter.
  Its 44 direct callees and nine registered scene callers are closed
  separately.
- The ordinary substep loop's only absolute indirect call is an out-of-line
  invalid-parameter path after function return, not a gameplay callback.
- The active-note manager's object-computed calls are the documented primary
  candidate slots, common runtime-note tick/destruction slots, and their
  secondary-vector counterparts. All twelve factory-reachable vtables were
  already enumerated; this rewalk found no fourth runtime vector or alternate
  dispatch column.

## Contradiction and traceability search

- Active claim IDs are unique. All claim identifiers referenced by
  specifications, reconstruction comments, tests, status, and coverage resolve
  to claim files.
- The only claim IDs intentionally not referenced by those active artifacts are
  the three superseded claims:
  `claim.pipeline.c2s-load-failure-lifecycle`,
  `claim.note.mine-contact-judgement`, and
  `claim.judgement.shared-result-routing`.
- No reconstruction, specification, test, or coverage evidence cites a
  superseded claim. Active successor claims retain explicit supersession
  history.
- Two stale unknown statements discovered during the pass were repaired:
  music-load failure propagation now points to the materialization/lifecycle
  successor, and pre-active catch-up now points to the closed AutoScan
  modulo-`2^32` counter claim.
- The apparent corpus contradiction around `ASO` was rejected by direct
  aggregate search: the inventory contains adjacent `ASC` and `ASD`, not
  `ASO`.

## Remaining parameterized values

Values absent from this workspace remain explicit inputs rather than guessed
constants. The main groups are:

- per-family timing windows, pivots, offsets, gap thresholds, completion
  thresholds, and malformed-ordering domains;
- runtime result-byte maps, category contributions, active-result remaps,
  skill thresholds/lifetimes, forced-result configuration, and terminal-rule
  vectors;
- PlayOption timing-correction records, runtime speed/projection inputs, and
  chart-region projection schedules;
- Mine contact requirements, HOLD/Slide/Air-family generated-path endpoints,
  and retained-profile checker records;
- controller/alternate-mode terminal fields whose interfaces and consumers
  are known but whose external record values are unavailable.

Their sources, selectors, lifetimes, defaults where present, and consumers are
split across `config.external`, judgement, timing, and note-family claims.
Unavailable numeric values alone are not treated as executable logic.

## Residual uncertainty

The following can still affect gameplay and prevents closure. The first two
items recorded by this audit were subsequently closed by
`claim.input.touch-photo-input-framing` and
`claim.input.snapshot-profile-synthesis`; they no longer remain blockers.

- Active-vector construction order, unequal/equal cross-family candidate
  fanout, simultaneous result ordering, and remaining shared-marker cycles
  need an independent interaction pass.
- Several family-specific external-mode selectors, endpoint builders,
  malformed path domains, active-result remaps, and absent-corpus
  compatibility paths remain only mapped.
- Remaining configuration virtuals and manager-field reset/use asymmetries
  require adversarial review.
- Completion gate 12 explicitly requires a fresh independent investigator.
  This report is by the same investigator who produced several audited claims
  and therefore cannot satisfy that gate.

## Alternatives and falsifiers

- Competing explanation: an unresolved indirect call, superseded dependency, or
  hidden compatibility command still feeds gameplay despite the current maps.
- Evidence that would disprove this claim: an active artifact citing a
  superseded claim, a descriptor/corpus spelling outside the documented
  partition, a factory-reachable vtable outside the twelve-class inventory, a
  new outer-update caller/callee alias, or an unclassified manager indirect
  target that writes gameplay state.

## Unknowns

- This report intentionally inherits every residual item listed above.
- It does not claim independent closure, saturation, or grand-goal completion.

## Consequences

- Ghidra mutations: none for this report.
- Spec sections: no new behavior; stale timing and lifecycle cross-references
  were corrected in their owning claims.
- Reconstruction code: none.
- Tests: no new behavior-specific test; the complete suite is the regression
  check.

## Verification

`harness.py doctor`, `validate`, and `corpus` succeeded. The CMake build
succeeded and all 28 tests passed. Claim-state/reference searches, coverage
counts, superseded-ID searches, direct corpus absence checks, and the
representative Ghidra indirect-call inventories were performed independently
within this pass. A different investigator must still repeat the closure audit.
