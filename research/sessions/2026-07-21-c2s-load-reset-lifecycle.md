# Session 2026-07-21: C2S load failure and manager reset lifecycle

> Subsequent correction (2026-07-21): the state-0 parsed-count loop below
> initializes a pending-index queue; it is not the runtime factory loop.
> `claim.pipeline.runtime-note-materialization-order` supersedes that part of
> this handoff and records the later post-manager lazy factory path. The
> destructive failure/reset and teardown findings remain valid.

- Investigator: codex-root
- Ghidra writer: none; GhidraMCP unavailable
- Coverage rows claimed: `pipeline.boundaries`
- Binary hash checked: yes
- MCP health checked: yes; no client tools and no listener at the configured endpoint

## Goal

Close gameplay C2S precheck/tokenizer/record-handler failure propagation,
partial cleanup, retry/reload state-machine ownership, runtime object teardown,
full manager reset, and manager destruction.

## Findings

- Added `claim.pipeline.c2s-load-failure-lifecycle`: gameplay load clears parsed
  chart state before file validation. Precheck/tokenizer failure leaves an empty
  chart; tokenized record rejection is skip-only rather than whole-load failure.
- The gameplay load function returns no status to scene setup. Setup continues,
  derives its pending-index count from the now-empty parsed vector, and queues
  no runtime notes after failure. Runtime construction occurs later.
- Token-file and token-buffer wrappers always destroy their temporary token
  owners. The parser orchestration performs its own reset before fixed ordered
  passes and returns success after tokenization.
- State 0 owns load/setup, advances directly to state 1, and has no recovered
  in-scene re-entry. The loader itself does not delete runtime notes.
- Manager construction invokes full reset. Two later scene entries delete both
  runtime pointer vectors; full reset additionally clears the third owned
  pointer vector and all parsed/forced/input/timing/result state. The manager
  destructor deletes all three vectors and releases the chart owner.
- Added normative load-lifecycle specification, clean-room decision helper, and
  a focused failure/empty/partially-accepted test.

## Ghidra mutations

None. The live project could not be reached through GhidraMCP. Static analysis
continued in `/tmp/chart-readonly-20260720`; no workspace `chart.rep` file was
edited. Several missing state callback boundaries were created only in that
temporary clone to resolve their direct control flow.

## Validation

- Binary identity remained the expected SHA-256
  `4e492489fb8e63c5b3ffad5267e78b02fcf8d74a261b058fc296a7e968516520`.
- File/buffer failure cleanup, gameplay outer-reset ordering, parser success
  propagation, unconditional state-0 build count, callback-table state flow,
  two scene teardowns, full reset, and manager destruction were independently
  compared.
- `python3 scripts/harness.py validate` passed with 28 coverage rows and 10
  required files.
- CMake configure/build passed. All 20 CTest targets passed, including
  `chart.load_lifecycle`.

## Unresolved and contradictions

- Exact per-command malformed-field predicates and alternate parser mode remain
  open; only failure propagation is closed here.
- The lower file wrapper has another non-gameplay caller without the gameplay
  outer reset. It does not alter the scoped gameplay result.
- Presentation/error behavior after an empty setup remains outside scope.
- No live Ghidra names, comments, types, or bookmarks could be applied.
- A later pipeline audit contradicted the immediate-factory interpretation;
  the dependent claim, spec, reconstruction helper, tests, coverage rows, and
  status were reviewed and corrected together.

## Handoff

The top-level gameplay load/reset/error path is closed at parsed reset
`RAM:011c7040`, file wrapper `RAM:011c77d0`, parser orchestration
`RAM:011c7980`, gameplay loader `RAM:00b2a8c0`, state-0 setup
`RAM:00da06c0`, runtime teardown `RAM:00b28a00`, full reset
`RAM:00b2ae30`, and manager destructor `RAM:00b27210`. Release
`pipeline.boundaries`; select the next highest-value unknown coverage row.
