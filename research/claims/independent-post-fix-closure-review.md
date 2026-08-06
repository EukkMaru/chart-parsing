# Claim: the independent post-fix closure review passes every completion gate

- ID: `claim.audit.independent-post-fix-closure-review`
- State: superseded
- Maturity: verified
- Confidence: high
- Owner: closure_auditor
- Coverage rows: `audit.closure`
- Last reviewed: 2026-08-03

## Statement

Superseded on 2026-08-03 by the exact type-9 ALD producer-to-consumer traces in
`claim.note.air-ladder-generated-checkpoints` and
`claim.note.air-ladder-precalc-presentation`. The review did not distinguish
the `0x24` authored-control vector from the `0x20` generated-checkpoint vector,
nor identify the accepted-event precompute key and authored terminal schedule,
so its no-remaining-gameplay-uncertainty verdict and administrative promotion
are no longer current. The report remains preserved as historical audit
evidence; `audit.closure` requires a new contradiction review after correction.

A fresh read-only investigator audited the post-correction workspace against
all twelve gates in `docs/COMPLETION.md` and found no remaining
gameplay-affecting uncertainty. The exact-snapshot clean-room reconstruction is
closed at the game's apparent tick granularity. Residual unknowns are limited
to explicitly parameterized external values, non-gameplay presentation names
and resources, and faithfully reported malformed-input source failure domains.

## Binary identity and audit scope

- Audited binary: local `game.exe`, SHA-256
  `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`.
- Ghidra MCP was healthy against the exact local `chart` project and
  `game.exe`; the reviewer made no Ghidra or workspace mutations.
- The pre-promotion coverage snapshot contained 27 `mapped` gameplay rows and
  one `investigating` closure row. The review found all 28 eligible for
  `verified`; no `unknown`, `mapped`, or `investigating` behavior remains after
  the administrative promotion supported by this report.
- Traceability audit found 74 active and six superseded claims before this
  report, with no duplicate/undefined active ID, no uncited active claim, no
  superseded coverage evidence or implementation marker, and every one of the
  34 CMake test targets linked from the coverage ledger.

## Corpus and parser inventory

- The local corpus contains 7,752 readable charts across eight declared
  versions, with zero unreadable charts and 111 distinct command spellings.
- Exact descriptor lookup registers 91 spellings: 87 occur in the corpus and
  four (`SFE`, `ASO`, `HHD`, and `HHX`) are corpus-absent but structurally
  reconstructed from this binary and covered synthetically where applicable.
- The other 24 corpus spellings are the four legacy boundary summaries and
  twenty `T_PROG_00` through `T_PROG_95` names. They are absent from the exact
  registry and are proven discarded before every parser pass.
- Header defaults/errors, timing records, all event families, group-3
  destructive overwrite, skipped invalid records, backward-compatible shapes,
  and nonfinite/extreme numeric domains are explicitly classified.

## Entry, exit, state, and interaction boundaries

- The review repeated source-path selection, reset-before-load, ordered parser
  passes, postprocessing, pending-record materialization, primary/secondary
  factory order, preload, active substeps, candidate reduction, note updates,
  periodic aggregate reevaluation, active exit, post-active drain, teardown,
  and final-result persistence.
- Construction, reset, ownership, update, terminal, and destruction paths are
  closed for every constructible primary, attached secondary, type-13 origin,
  and deliberately nonjudging type. Error and empty-chart paths are included.
- HID transport framing through logical snapshots, history, photo profiles,
  edge/level selection, candidates, lane/width bounds, forced/tutorial modes,
  compound streams, simultaneous notes, same-pass rerouting, and cross-note
  sustain handoff have explicit ordering and focused tests.
- Clock ownership, catch-up order, float/tick conversion, projection schedules,
  exact compiled BPM sort, tempo changes, inclusivity, wrap arithmetic, and
  malformed Air cadence/path behavior are normative and reconstructed.

## Indirect calls and excluded downstream code

- The audit rechecked gameplay note vtables, factory dispatch, registered scene
  callbacks, alternate paths, result-control callers, reset/error paths, and
  the outer-update direct-callee inventory. Every gameplay-relevant target is
  resolved or proven dormant.
- The HOLD end-feedback path was rechecked through the fixed 51-element sound
  controller array and its sole `sound::Sound` vtable. Its reachable virtuals
  are audio/configuration operations and do not submit results, write note
  phases, or request gameplay transitions.
- Preload resources, render primitive counters, meter/grid markers, key beams,
  scene exports, and cue/report scheduling are one-way boundaries. Their
  complete owner/reference sets do not feed input, note generation,
  judgement, results, or the gameplay clock; the documented cue lifetime edge
  begins only after the authoritative gameplay update.

## External parameters and residual uncertainty

- Numeric rows absent from the snapshot remain clean-room inputs rather than
  guessed constants: judgement windows/corrections, gap gates/floors/ends and
  thresholds, photo-profile thresholds, option/skill/configuration table rows,
  result aggregation rules, endpoint profiles, exclusion intervals, and
  resource-selected timing values. Their executable source, default when
  executable-owned, selector, bounds, lifetime/reset, and every gameplay
  consumer are recovered.
- Player-facing labels for anonymous result bytes/categories and presentation
  resource identities are intentionally unassigned because they cannot alter
  the reconstructed numeric routing or outcome.
- Empty maps, invalid keyed paths, nonfinite conversions, high-bit wrapped Air
  spans, nonterminating cadence doubling, zero-step cursor nonprogress, and
  cursor-wrap expansion are reported as source behaviors/failure domains. The
  clean-room domain evaluators avoid executing unsafe allocations or loops
  without normalizing their effect.
- None of these residuals conceals a gameplay branch, target, state owner,
  reset, interaction, or unparameterized value. They therefore satisfy the
  configuration and residual-uncertainty rules in `docs/COMPLETION.md`.

## Contradiction and traceability audit

- The reviewer searched claims, specs, C++ markers, tests, coverage evidence,
  and status prose for stale assumptions from every superseded finding.
- It challenged parser vocabulary, lane/result field consumers, materialized
  sort ordering, signed wrap and `CVTTSS2SI` boundaries, Air adaptive zero
  steps, AirSlide cursor wrap, HOLD feedback virtuals, and the final
  entry-to-exit/control-flow inventory. All discrepancies found during the
  pass were corrected and then re-audited.
- The final substantive search found no active-claim conflict, conventional
  behavior used as proof, unexplained gameplay branch, unowned gameplay state,
  unresolved indirect target, or implementation rule lacking a claim, spec,
  anchor, and test.

## Validation results

- `python3 scripts/harness.py doctor`: exact hash, local source material,
  required tools, and MCP health passed.
- `python3 scripts/harness.py corpus`: 7,752 charts, zero unreadable, eight
  versions, and the 111-spelling inventory above.
- `python3 scripts/harness.py validate`: passed with 28 coverage rows before
  promotion and again after the final ledger updates.
- Fresh Release build and CTest: 34/34 passed.
- Fresh UndefinedBehaviorSanitizer build and CTest: 34/34 passed.
- The primary investigator's final normal build and CTest: 34/34 passed.

## Reasoning

The reviewer did not rely on the primary investigator's closure conclusion.
It performed a fresh read-only, gate-by-gate contradiction attempt after the
last corrective edits, including the previously missed parser, schedule,
configuration, indirect-call, malformed-arithmetic, and interaction classes.
Every candidate either followed a closed entry-to-outcome path or ended at a
documented one-way boundary. The passing verdict therefore supports promotion
of all 28 coverage rows and completion of the grand goal.

## Alternatives and falsifiers

- Competing explanation: an omitted branch or external value can still change
  gameplay without passing through a reconstructed interface.
- Evidence that would disprove this claim: a new exact-binary target, writer,
  selector, reset, command, or interaction that reaches generation,
  candidates, judgement, result routing, or terminal persistence without a
  linked claim/spec/test; or a reproducible active-claim contradiction.

## Consequences

- Ghidra mutations: none by the independent reviewer.
- Supersedes: `claim.audit.independent-closure-review` as the final closure
  verdict; the earlier report remains preserved as a valid historical failure.
- Spec sections: all normative files under `spec/`, with `spec/README.md` as
  the completion index.
- Reconstruction code: `include/chart/reconstruction.hpp`.
- Tests: all 34 targets declared by `CMakeLists.txt` and cited in
  `research/COVERAGE.tsv`.

## Verification

Independent technical verdict: **PASS** on closed boundaries, parser, time,
notes, input, interactions, configuration, control-flow saturation,
traceability, corpus compatibility, contradiction audit, and independent
closure review. The reviewer explicitly reported no remaining
gameplay-affecting uncertainty after the final Air and HOLD-feedback fixes.
