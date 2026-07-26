# Session 2026-07-26: outer, parser, timing, and input closure

- Investigator: codex-root
- Ghidra writer: codex-root during bounded mutations; released at session end
- Coverage rows claimed: `pipeline.boundaries`, `parser.events`,
  `timing.tick_conversion`, `audit.closure`, and `input.logical_state`;
  `input.transport` was closed within the input slice; all are `mapped` with
  no owner at session end
- Binary hash checked: yes
- MCP health checked: yes; live `chart` project and `game.exe` program used

## Goal

Continue from the final documentation handoff without stopping at the first
finding: close the remaining outer-update direct callees, resolve parser
descriptor/corpus vocabulary disagreement, finish clock/counter conversion
domains, perform a same-investigator adversarial audit, and then recover the
complete gameplay input framing, snapshot synthesis, history, and consumer
inventory.

## Findings

- `research/claims/pre-active-zero-base-priming.md` and
  `research/claims/outer-update-direct-callee-closure.md` close the registered
  scene-state callers and direct callees of the outer gameplay update. States
  `0x0a..0x0e` run the full update with a zero base/input mode; `0x0f..0x12`
  run live updates; `0x13/0x14` do not.
- `research/claims/legacy-t-prog-command-exclusion.md` resolves the apparent
  111-versus-91 command contradiction. Exactly 91 descriptors and constructor
  calls form the live vocabulary; the 20 corpus-wide `T_PROG_00..95` spellings
  belong to a separate unused string table and are dropped by exact lookup.
- `research/claims/music-cursor-base-time.md` closes audio cursor conversion,
  failure sentinel handling, base-time sampling, the QPC-backed modulo
  millisecond timer, and the two separate float multiplications used for chart
  position.
- `research/claims/autoscan-counter-wrap.md` closes successful-scan counter
  ownership and the native modulo-`2^32` catch-up calculation, including
  adjacent wrap.
- `research/claims/adversarial-saturation-review.md` records the representative
  indirect-call, corpus, claim-reference, supersession, and contradiction
  rewalk. It found no conflict but explicitly does not satisfy the independent
  completion gate.
- `research/claims/touch-photo-input-framing.md` closes the gameplay source
  record as two ordered 16-byte TouchSlider Y banks plus six
  `photo_sensor_6..1` bits. Ordinary updates use thresholded history offsets
  `-1/0`; the alternate builder uses the current thresholded sample.
- `research/claims/input-snapshot-profile-synthesis.md` closes every
  gameplay-consumed field of the `0x58` input snapshot, the AIR-marker effect,
  photo conditioning/residual scalar algorithm, profiles 0 through 7,
  oldest-first history eviction, and all accessor consumers. Profile 7 is
  position-range-only; its stored eighth motion-threshold record is bypassed.
- The clean-room reconstruction now includes source/profile configuration,
  scalar-position and signed-motion synthesis, music cursor/timer/counter
  conversions, outer-update mode mapping, and live descriptor vocabulary.
  Focused coverage increased from 28 to 29 tests.

## Ghidra mutations

- Created missing function boundaries after representative inspection at
  `RAM:00da2c80`, `00da2f30`, `00da3150`, `00da3140`, `00da33f0`, and
  `00da3420`.
- Added confidence-gated comments to the outer-update registered callbacks and
  direct-callee boundaries, the 91-entry descriptor constructor/exact lookup/
  group-2 dispatch, the audio cursor producer, chart-position conversion,
  AutoScan counter increment, and ordinary catch-up arithmetic.
- Added input comments at `RAM:00da8860` and `00da70c0` for ordinary/alternate
  source framing, `RAM:00a7b730` and `00a7d170` for TouchSlider/photo-sensor
  selection, `RAM:00c2dbf0` for reset/configuration/seed ownership, and
  `RAM:00c2dfa0` for complete synthesis and capacity.
- Every broad or persistent mutation was dry-run first. No tentative function
  was renamed and no raw project files were edited. `game.exe` was saved
  successfully through Ghidra MCP.

## Validation

- `python3 scripts/harness.py doctor`: passed at session start with the exact
  binary identity and live MCP program.
- `python3 scripts/harness.py corpus`: 2,140 directories, 7,752 readable
  charts, eight observed version strings, and 111 aggregate spellings.
- Descriptor constructor count, exact-lookup references, accessor thunks,
  non-thunk consumers, input reset/append paths, and representative indirect
  calls were enumerated independently from the large synthesizer decompile.
- `python3 scripts/harness.py validate`: passes with 28 coverage rows and 11
  required files after all ledger changes.
- CMake configure/build: passes.
- CTest: 29/29 passes, including the new
  `tests/input_profile_test.cpp`.

## Unresolved and contradictions

- Grand-goal completion remains open because `docs/COMPLETION.md` requires a
  fresh independent contradiction audit. This investigator's saturation pass
  cannot satisfy that gate.
- External input conditioning counts, seven active motion records, profile-7
  range, device thresholds/bindings, judgement windows, and other resource
  values remain parameters. Their executable selection and consumers are
  closed; the absent numeric values are not treated as hidden executable logic.
- The initial visual reading of adjacent corpus tokens as `ASO` was false.
  Direct aggregate searches showed zero `ASO`, `HHD`, and `HHX` corpus lines;
  no claim or reconstruction was based on the misread.
- A tempting interpretation applied the stored eighth motion-threshold record
  to profile 7. Rewalking the resolved-flag loop showed profile 7 bypasses that
  evaluation and uses only its inclusive position range; the reconstruction
  and test explicitly enforce this.

## Handoff

No coverage row is owned. A fresh investigator should claim `audit.closure`
and repeat the completion checklist without relying on this session's
conclusions: entry-to-exit paths, indirect targets, all state owners/resets,
cross-note marker/candidate/result interactions, corpus compatibility, active
versus superseded claim references, and clean-room/spec agreement.

Start from `research/claims/adversarial-saturation-review.md` as an audit
inventory, not as proof. For input, the strongest contradiction anchors are
`FUN_00c2dfa0` at `RAM:00c2dfa0`, configuration/reset at `RAM:00c2dbf0`,
source builders at `RAM:00da8730`/`00da70c0`, and the six accessor thunk xref
sets enumerated in `claim.input.snapshot-profile-synthesis`. If the independent
pass agrees, update `audit.closure` only as allowed by
`docs/COMPLETION.md`; otherwise reopen the exact contradicted rows.
