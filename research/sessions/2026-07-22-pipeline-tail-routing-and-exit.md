# Session 2026-07-22: aggregate routing and gameplay-exit tail

- Investigator: codex-root
- Ghidra writer: none
- Coverage rows claimed: `pipeline.boundaries`
- Binary hash checked: yes
- MCP health checked: yes; HTTP endpoint not listening and no GhidraMCP tools exposed

## Goal

Continue the P0 chart-load-to-teardown edge audit from the end of the complete
outer gameplay update. Close the periodic aggregate tail, verify shared-result
category routing rather than inheriting a single-map assumption, reconstruct
the active-state exit predicate, and determine the first state that actually
stops runtime-note updates.

## Findings

- Added `claim.judgement.periodic-aggregate-reevaluation`: every complete outer
  update ends with a kind-2 aggregate evaluation using the recovered
  single-precision position-to-tick conversion. Common contribution/promotion
  and the later end threshold remain eligible without a new note result.
- Superseded `claim.judgement.shared-result-routing` with
  `claim.judgement.shared-result-two-stage-routing`. The sole dispatcher uses
  independent progress and aggregate fields from the original dispatch record;
  all fourteen dispatch categories enter both valid domains for a runtime-valid
  result byte.
- Superseded `claim.note.mine-contact-judgement` with
  `claim.note.mine-contact-aggregate-judgement`. MNE source category 17 selects
  dispatch category 11, progress category 0, and aggregate category 8; it is an
  authoritative ordinary result, not a nonaggregate dispatch.
- Added `claim.pipeline.active-gameplay-exit-gate`: the registered active
  callback completes the full outer update before its ordinary versus alternate
  exit predicates. Result-count completion is enabled only for a nonzero
  expected total and uses unsigned inclusive comparison.
- Added `claim.pipeline.post-active-gameplay-drain`: structural states `0x10`,
  `0x11`, and `0x12` each continue the full outer gameplay update. Runtime-note
  teardown begins only on state-`0x13` entry; state `0x14` is final report/exit
  handling with no outer update.
- Added `claim.pipeline.final-result-persistence-order`: state-`0x13` clears
  only two already-consumed configured-rule notification masks, and state
  `0x14` resets the destination slot before deep-copying the complete current
  result object. The last drain update survives to retained result state.

## Ghidra mutations

- Live project: none. GhidraMCP was unavailable and the configured endpoint was
  not listening, so the confidence-gated live rename/comment/type policy could
  not be exercised.
- Temporary clone only: missing callback thunks/functions needed for static
  closure were materialized in `/tmp/chart-readonly-20260720`, including the
  active callback thunk and state-`0x12` release/cleanup targets. No names,
  comments, types, structures, enums, or bookmarks were applied to the live
  project.

## Validation

- `python3 scripts/harness.py doctor`: binary/project/corpus/tool identity OK;
  MCP HTTP not listening; existing GUI project lock observed and not touched.
- Independent headless checks covered the sole result dispatcher caller, both
  second-stage lookup arguments and table fields, active callback registration,
  reset/setup/enable completion chain, state callbacks `0x0f` through `0x14`,
  runtime-note teardown, and report/final callbacks.
- `cmake -S . -B build` and `cmake --build build -j2`: passed.
- `ctest --test-dir build --output-on-failure`: 28/28 passed.
- `python3 scripts/harness.py validate`: validation OK, 28 coverage rows and 10
  required files.
- `python3 scripts/harness.py next`: continue `pipeline.boundaries`, P0,
  investigating, owner codex-root.

## Unresolved and contradictions

- The prior single-map routing model was falsified. Its two affected claims are
  explicitly superseded; current specs, status, code, tests, and coverage point
  to the replacement claims.
- Player-facing meanings for the three result category domains, controller and
  scene selectors, nested state/ready fields, and structural states `0x10`
  through `0x14` remain unassigned.
- Post-active presentation conditions determine the number of additional full
  gameplay updates. Their interfaces and branch order are recovered, but their
  unavailable runtime timing values are not guessed.
- The live Ghidra database still lacks annotations from this session because
  MCP access is an external blocker; durable workspace claims carry the stable
  anchors meanwhile.

## Handoff

Keep `pipeline.boundaries` owned by codex-root. The state-`0x13` contradiction
pass is closed by `claim.pipeline.final-result-persistence-order`: the narrow
chain `00b984e0 -> 00b966d0 -> 00b949c0` clears only dynamic rule masks, while
`00da1d20 -> 00bd7ef0 -> 00bd6be0` persists the complete current result. The
registered post-active path is
`00da2d30` (state `0x10`), `00da2e20` (state `0x11`), `00da2f30` (state
`0x12`), `00da1c00`/`00da2ff0` (state `0x13`), and
`00da1d20`/`00da3140` (state `0x14`). Rewalk the remaining `00da9820` direct
callees for any unowned write alias rather than reopening already bounded
presentation sinks.
