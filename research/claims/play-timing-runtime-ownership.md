# Claim: gameplay clock rate and correction are selected by separate process-owned objects

- ID: `claim.timing.play-timing-runtime-ownership`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `timing.clock`
- Last reviewed: 2026-07-21

## Statement

Normal in-executable gameplay leaves the accumulated-clock rate at `1.0`,
while the manager subtracts the current `PlayOptionSet` record's externally
loaded play-timing offset, narrowed from double to float; a missing or
out-of-range offset record contributes zero.

## Anchors

- `game.exe @ RAM:00c320d0, FUN_00c320d0, ViewTimingManager construction and scalar defaults`
- `game.exe @ RAM:018da0f8, ViewTimingManager RTTI complete-object locator`
- `game.exe @ RAM:00b26de0, FUN_00b26de0, ViewTimingManager singleton allocation`
- `game.exe @ RAM:00b27210, FUN_00b27210, ViewTimingManager singleton destruction`
- `game.exe @ RAM:00da6850, FUN_00da6850, guarded accumulated-clock rate selection`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, selector lookup, narrowing, and subtraction`
- `game.exe @ RAM:00b57600, FUN_00b57600, initial enabled PlayOptionSet selection`
- `game.exe @ RAM:00b6cff0, FUN_00b6cff0, bounded PlayOptionSet assignment`
- `game.exe @ RAM:00433564, FUN_00433564, bounded play-timing-offset lookup`
- `game.exe @ RAM:0113ea70, FUN_0113ea70, table path selection and load dispatch`
- `game.exe @ RAM:01140be0, FUN_01140be0, 0x38-byte record reader`
- `game.exe @ RAM:0113ee00, FUN_0113ee00, table singleton cleanup`

## Observations

- RTTI identifies the large singleton allocated by the gameplay manager as
  `projView::ViewTimingManager`. Its constructor clears the enable word and
  initializes the five scalar floats at offsets `+0x0c` through `+0x1c` to
  `1.0`.
- The accumulated-clock consumer reads `+0x0c * +0x10` only when byte `+0x04`
  is nonzero. The manager-position and view consumers similarly guard `+0x14`,
  `+0x18`, and `+0x1c` with the same byte.
- A complete literal-reference inventory for the singleton pointer contains
  only manager construction/destruction and those three read-only consumers.
  The reset helper is called only by construction and clears the trailing
  sample storage, not the enable byte or scalar fields. No executable producer
  for a nonzero enable byte or non-unit scalar was recovered.
- The options constructor chooses the first enabled entry among the first four
  `PlayOptionSetTableRecord` rows, falling back to ID zero. Later assignment
  stores a requested ID only when it is below that table's current record
  count. Scene and menu paths can explicitly select another valid set,
  including set 3.
- The judgement manager passes that set ID to a distinct
  `PlayOptionPlayTimingOffsetTableRecord` singleton. Its getter checks the ID
  against the current `0x38`-byte record count and reads the double at record
  offset `+0x10`; failure of the check returns zero. The caller narrows the
  result to float before subtracting it from manager time.
- The generic table entry names the external resource
  `PlayOptionPlayTimingOffsetTableRecord.bin`. Loading prefers an existing file
  under an optional override directory and otherwise uses the supplied base
  directory. The reader clears the prior vector before opening the file.
- The table singleton is lazy and process-owned. Its dedicated cleanup deletes
  it and clears the global pointer; chart reset, runtime-note teardown, and
  gameplay-manager reset do not own it. Neither the workspace nor `music.zip`
  contains the named table file, so its values are unavailable.

## Reasoning

Constructor defaults plus the closed singleton-reference set make the guarded
rate branch dormant under normal control flow in this executable snapshot; the
effective accumulated-clock rate is therefore `1.0`. The correction's RTTI,
table-entry name, binary reader, set-ID writers, bounded getter, and immediate
manager consumer close its ownership and type-conversion path without assuming
any absent table value.

## Alternatives and falsifiers

- Competing explanation: an internal path enables ViewTimingManager through an
  alias or hidden setter.
- Evidence that would disprove this claim: a reachable executable write to the
  singleton's enable/scalar fields, or a call that passes its address to a
  mutating routine not represented in the literal-reference inventory.
- Competing explanation: the correction selector is an unrelated integer.
- Evidence that would disprove this claim: a manager path deriving the selector
  independently of the bounded PlayOptionSet owner, or a correction lookup
  indexed by another field.

## Unknowns

- The player-facing meanings of individual PlayOptionSet IDs are not assigned.
- External table values and directory roots are absent and remain parameters.
- Corrupt or partially readable table-file behavior is not reconstructed
  beyond the observed clear-before-load and bounded-consumer fallback.
- Deliberate out-of-process memory modification is outside the recovered normal
  executable control flow.

## Consequences

- Ghidra mutations: no live-project mutation because GhidraMCP is unavailable;
  a function boundary at `RAM:0113ee00` was recovered only in the temporary
  analysis clone.
- Spec sections: `spec/timing.md`.
- Reconstruction code: `include/chart/reconstruction.hpp`.
- Tests: `tests/play_clock_test.cpp`.

## Verification

The rate path was checked independently through constructor defaults, all
literal singleton references, reset callers, and three consumers. The
correction path was checked through both external table identities, selector
construction/assignment, record parsing, bounded lookup, and manager
subtraction. Focused tests cover disabled/enabled rate selection, double-to-
float correction selection, and out-of-range fallback.
