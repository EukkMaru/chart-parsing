# Claim: gameplay chart-load failure is destructive and setup continues empty

- ID: `claim.pipeline.c2s-load-failure-lifecycle`
- State: superseded
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.discovery`, `parser.events`, `state.ownership`
- Last reviewed: 2026-07-21

## Supersession

`claim.pipeline.runtime-note-materialization-order` supersedes this claim. The
destructive load/reset and teardown findings below remain supported, but the
state-0 loop was misidentified as the runtime factory loop. It actually fills
an ordered queue of parsed-record indices; `FUN_00da62c0` performs later lazy
materialization only after the current outer update's manager substeps.

## Statement

The gameplay load entry clears parsed chart state before file validation. A
file-precheck or tokenizer failure therefore leaves zero parsed records, while
successful tokenization makes record-handler rejection skip-only. The scene
setup caller continues without a load-status branch and constructs zero runtime
notes after failure. Runtime-object teardown is owned by manager construction,
scene teardown, full reset, and destruction rather than by the chart loader.

## Anchors

- `game.exe @ RAM:011c7040, FUN_011c7040, parsed-chart reset and record destruction`
- `game.exe @ RAM:011c77d0, FUN_011c77d0, file precheck and tokenizer-wrapper selection`
- `game.exe @ RAM:011c7810, FUN_011c7810, file tokenizer lifetime and failure return`
- `game.exe @ RAM:011c7c50, FUN_011c7c50, buffer tokenizer lifetime and failure return`
- `game.exe @ RAM:011c7980, FUN_011c7980, chart reset, ordered parser passes, and success return`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, gameplay destructive load boundary and success-only configuration`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, state-0 scene setup and runtime factory loop`
- `game.exe @ RAM:00da24a0, FUN_00da24a0, state-0 callback-table binding`
- `game.exe @ RAM:00da2d10, FUN_00da2d10, state-0 forward transition to state 1`
- `game.exe @ RAM:00b26de0, FUN_00b26de0, manager/chart construction and full reset`
- `game.exe @ RAM:00b28a00, FUN_00b28a00, two runtime-vector teardown and result-container reset`
- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, full manager reset`
- `game.exe @ RAM:00da04d0, FUN_00da04d0, scene-state runtime teardown entry`
- `game.exe @ RAM:00da1c00, FUN_00da1c00, alternate scene-state runtime teardown entry`
- `game.exe @ RAM:00b27210, FUN_00b27210, manager destructor and three-vector ownership closure`
- `game.exe @ RAM:00ace460, FUN_00ace460, external full-reset sequence`

## Observations

- The parsed-chart reset destroys every 0x174-byte parsed record, empties its
  record vector and associated containers, resets the root identifier to -1,
  and clears chart parameter storage.
- The gameplay loader calls that reset unconditionally before the file wrapper.
  It writes forced-mode bytes and constructs runtime timing/profile records only
  after the wrapper returns true.
- The file wrapper first performs a path/stream precheck. Its tokenizer wrapper
  constructs a temporary token owner, returns false when tokenization fails,
  and destroys that temporary on both branches. The buffer wrapper has the same
  token-owner cleanup shape.
- Successful tokenization calls the parse orchestration. That orchestration
  resets the chart again, executes its descriptor/event passes and fixed
  postprocessor sequence, and returns true. Individual handler rejection can
  suppress a record/count contribution but has no whole-load failure return.
- Scene state 0 calls the void gameplay loader, then immediately continues
  post-load setup. It calculates the runtime build count as parsed-vector bytes
  divided by 0x174 and executes the factory loop that many times. No conditional
  branch consumes the wrapper result; failed load therefore produces count zero.
- The scene callback table binds state 0 to this setup entry. Its update writes
  pending state 1. The recovered scene updates advance among later states; no
  reachable in-scene writer requests state 0 after initial entry.
- Manager construction constructs the chart and invokes full reset. Full reset
  first deletes the two runtime pointer vectors, resets parsed chart state, then
  clears forced-result, input/source-map, timing/result, and configuration state
  plus a third owned pointer vector.
- Two later scene entry callbacks invoke the two-vector runtime teardown. The
  manager destructor independently deletes all three owned pointer vectors,
  releases their storage and other containers, and destroys the chart owner.

## Reasoning

The ordering in the gameplay loader proves that a failed file/token boundary
cannot preserve parsed records. The wrappers close their temporary ownership
on both returns, and the only parse-orchestration exit after tokenization is
success, separating tokenization failure from skip-only record rejection. The
void call followed by an unconditional parsed-count factory loop establishes
the observable failed-load result: setup continues empty. Callback-table and
state-transition evidence rules out an ordinary in-scene state-0 retry, while
constructor/reset, later scene entries, and the destructor account for runtime
object lifetime outside the chart loader.

## Alternatives and falsifiers

- Competing explanation: a hidden retry preserves old parsed or runtime notes,
  or a rejected record aborts the complete tokenized load.
- Evidence that would disprove this claim: a failure path bypassing the
  gameplay loader's initial chart reset, a false return from parse orchestration
  after tokenization, a state transition that re-enters state 0 with live
  runtime vectors, or another runtime pointer-vector owner not closed by reset
  or destruction.

## Unknowns

- The presentation/error response to a zero-note failed setup is outside
  gameplay generation/judgement scope and was not traced.
- Shared header/event numeric malformed-field behavior is closed by
  `claim.parser.header-default-dispatch` and
  `claim.parser.event-token-fallback`. Family-specific association and domain
  rejection conditions remain owned by their event claims.
- The non-gameplay caller of the lower file wrapper does not perform the same
  outer pre-reset; its product behavior is outside this gameplay-facing claim.

## Consequences

- Ghidra mutations: none; GhidraMCP remained unavailable, so analysis used the
  temporary static project clone.
- Spec sections: `spec/c2s.md`.
- Reconstruction code: `C2sInputOutcome`, `GameplayChartLoadAttempt`, and
  `reconstruct_gameplay_chart_load_attempt`.
- Tests: `tests/chart_load_lifecycle_test.cpp`.

## Verification

The file and buffer wrappers were compared for symmetric temporary cleanup.
The gameplay loader's reset ordering was checked independently against the
state-0 caller's unconditional parsed-count factory loop. The state callback
table, forward state-0 update, both scene teardown entries, full reset, and the
manager deleting destructor were traced as independent lifetime paths. The
focused test and complete suite pass.
