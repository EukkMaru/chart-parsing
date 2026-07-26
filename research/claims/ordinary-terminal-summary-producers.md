# Claim: ordinary terminal summaries have two ordered producers

- ID: `claim.judgement.ordinary-terminal-summary-producers`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `judgement.miss`,
  `state.ownership`, `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-20

## Statement

Each ordinary valid aggregate evaluation starts with clear summary bytes. A
matched configured terminal rule can zero the computed contribution and set
the primary summary byte. Only while that byte remains clear can a later
runtime end-threshold comparison zero the contribution and set both primary
and secondary bytes. The configured rule therefore has precedence and produces
primary-only termination when both conditions would otherwise apply.

The aggregate owner latches either nonzero summary byte in its corresponding
owner flag until reset. The executable-owned suppression byte read by both
producer branches is initialized to zero and has no recovered nonzero writer.

## Anchors

- `game.exe @ RAM:00b95870, FUN_00b95870, ordinary event aggregation entry`
- `game.exe @ RAM:00b96b30, FUN_00b96b30, summary consumption and owner latches`
- `game.exe @ RAM:00b92640, FUN_00b92640, ordered summary production`
- `game.exe @ RAM:00b92ff0, FUN_00b92ff0, configured terminal-rule evaluation`
- `game.exe @ RAM:00b94ac0, FUN_00b94ac0, rule-vector population`
- `game.exe @ RAM:00b93d90, FUN_00b93d90, rule-record construction`
- `game.exe @ RAM:00b946f0, FUN_00b946f0, rule-record copy`
- `game.exe @ RAM:00b90ea0, FUN_00b90ea0, rule-vector construction`
- `game.exe @ RAM:00b93f90, FUN_00b93f90, rule-vector reset`
- `game.exe @ RAM:00b8ef70, FUN_00b8ef70, end-metric getter`
- `game.exe @ RAM:00b901a0, FUN_00b901a0, end-metric update`
- `game.exe @ RAM:00b61ad0, FUN_00b61ad0, track-skip option dispatch`
- `game.exe @ RAM:00b95f80, FUN_00b95f80, aggregate-owner reset`
- `game.exe @ RAM:00b96700, FUN_00b96700, external primary-flag setter`
- `game.exe @ RAM:01c78bc4, initialized producer-suppression byte`

## Observations

- The evaluator clears its output double and both adjacent summary bytes before
  applying configured rule records.
- Rule records are owned by a vector with `0x60`-byte elements. The type-3 load
  path constructs and copies these records, and the result owner has explicit
  construction and reset paths for the vector.
- Each type-3 record is evaluated by `FUN_00b92ff0`, which returns whether the
  record matched and separately reports whether it requests terminal routing.
  A matched record is marked consumed and counted even when it does not request
  termination. Its five conditions and same-evaluation gauge dependency are
  closed by `claim.judgement.ordinary-configured-terminal-rule`.
- A requested rule termination, while the suppression byte is zero, clears the
  computed double and sets only summary byte zero. Evaluation can continue,
  but the later end-threshold producer requires that byte still be clear.
- The end producer excludes several runtime modes and applies additional
  runtime gates before selecting one of two externally backed threshold paths
  from the live `PlayOptionTrackSkipID`. Its exact selection and unsigned
  comparison are closed by
  `claim.judgement.ordinary-terminal-end-threshold`. It compares against an
  aggregate metric read through `FUN_00b8ef70`. That metric is reset by the
  result owner and updated as a separately scaled and rounded value by
  `FUN_00b901a0`.
- When the end threshold is reached, the suppression byte is zero, and primary
  remains clear, the evaluator clears the computed double and writes both
  summary bytes to one.
- `FUN_00b96b30` stores the returned double, latches a primary value of one in
  owner byte `+0x2a8`, and latches any nonzero secondary value in `+0x2a9`.
  `FUN_00b95f80` clears both owner bytes. `FUN_00b96700` can independently
  replace the primary owner byte.
- The suppression byte is initialized to zero. Its direct references consist
  of the evaluator reads and apparent references that disassemble as padding
  in the gap before the next function; no applicable nonzero writer was found.

## Reasoning

The primary-clear condition on the later branch establishes producer
precedence independently of the unresolved rule semantics and threshold-table
values. Separating the evaluator's per-call summary from the aggregate owner's
latched flags also closes the lifetime: producer output is temporary, while
later result dispatch observes owner state until its explicit reset.

## Alternatives and falsifiers

- Competing explanation: the secondary flag is another independent rule result
  that can coexist with a configured-rule terminal request.
- Evidence that would disprove this claim: a path to the end producer that does
  not require primary to be clear, a nonzero suppression-byte writer in the
  resolved runtime graph, or a result-owner reset that preserves either flag.

## Unknowns

- The player-facing identities of the two summary flags are not assigned.
- Semantic names, units, and external values for the configured type-3 fields,
  external end-threshold rows, and the scaled aggregate metric remain
  unresolved. The type-3 comparisons are closed by
  `claim.judgement.ordinary-configured-terminal-rule`; the end-selector target
  and arithmetic are closed by
  `claim.judgement.ordinary-terminal-end-threshold`.
- A computed-address or untraced external write to the suppression byte would
  falsify the direct-reference absence result; none is statically supported.

## Consequences

- Ghidra mutations: none; GhidraMCP was unavailable and analysis used the
  temporary project clone.
- Spec sections: `spec/judgement.md`.
- Reconstruction code: `OrdinaryTerminalSummary`,
  `evaluate_ordinary_configured_terminal_rule`,
  `ordinary_terminal_end_threshold_reached`, and
  `produce_ordinary_terminal_summary` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/shared_result_test.cpp` covers the ordinary, configured-rule,
  end-threshold, and simultaneous-condition cases.

## Verification

The rule-vector lifecycle, record evaluator, both producer branches, output
consumption, owner reset, external setter, end-metric reset/update/getter, and
all direct suppression-byte references were inspected separately. The focused
test preserves the observed primary-only precedence when both input predicates
are true.
