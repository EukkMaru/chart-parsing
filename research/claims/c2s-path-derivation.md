# Claim: gameplay load derives the chart path by literal substring replacement

- ID: `claim.pipeline.c2s-path-derivation`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.discovery`
- Last reviewed: 2026-07-26

## Statement

During gameplay load initialization, the snapshot copies a selected path,
searches from offset zero for the first exact lowercase `.xml` substring, and,
only when found, replaces those four bytes with `.c2s`. It passes the resulting
string to the chart-load entry even when no replacement occurred.

## Anchors

- `game.exe @ RAM:00da06c0, FUN_00da06c0, gameplay load-state function containing the sole .c2s reference`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, downstream owner initialization and chart-load caller`
- `game.exe @ RAM:011c77d0, FUN_011c77d0, file/line-load wrapper reached by FUN_00b2a8c0`
- `game.exe @ RAM:00b4e220, FUN_00b4e220, immediate upstream source-path selector`

## Observations

- `FUN_00da06c0` is in the state-label neighborhood containing `LoadMusic`,
  `CheckMusic`, `GameSetup`, `Play`, and `PlayFinish`.
- It copies the path-like field at owner offset `0x5b4`, performs a standard
  string search for `.xml` from position zero, and branches on the not-found
  sentinel.
- The found branch replaces a length of four with `.c2s`; the not-found branch
  skips only the replacement. Both paths converge before the same call with the
  local path string.
- That call reaches `FUN_00b2a8c0`, which clears prior chart state through
  `FUN_011c7040` and invokes `FUN_011c77d0` with the path.

## Reasoning

The exact literals, standard-string operations, converged control flow, and
downstream file/line reader establish a path-selection rule rather than an
incidental display-string transformation. The not-found branch is explicit and
does not substitute a default chart filename.

## Alternatives and falsifiers

- Competing explanation: the replacement creates a logging-only name.
- Evidence that would disprove this claim: a reanalysis showing the converged
  path is not supplied to `FUN_00b2a8c0`/`FUN_011c77d0`, or that another branch
  rewrites it before the reader receives it.

## Unknowns

- The player-facing presentation of a failed/empty gameplay load is outside
  this path-selection claim. Destructive reset, file/token failure, skip-only
  record rejection, unconditional scene continuation, and later lazy
  materialization are closed by
  `claim.pipeline.runtime-note-materialization-order`.

## Consequences

- Ghidra mutations: none in the live project; analysis used a temporary clone
  because GhidraMCP was unavailable.
- Spec sections: `spec/c2s.md` chart-path derivation.
- Reconstruction code: `include/chart/reconstruction.hpp`.
- Tests: `tests/chart_path_test.cpp`.

## Verification

Focused tests cover a normal suffix, case mismatch, absence, first-match-only
replacement, and a non-suffix occurrence. Upstream field ownership and
selection are independently reconstructed by
`claim.pipeline.source-chart-path-selection`.
