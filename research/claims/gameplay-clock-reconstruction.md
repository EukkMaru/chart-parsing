# Claim: the judgement clock is a quantized, rate-integrated millisecond clock

- ID: `claim.timing.gameplay-clock-reconstruction`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `timing.clock`, `timing.tick_conversion`, `timing.discontinuity`, `note.tap`
- Last reviewed: 2026-07-26

## Statement

The time delivered to the active-note manager is derived from a sampled music
cursor plus a reset integer-millisecond timer at 0.06 chart units per
millisecond, passed through an exact mode-dependent float quantizer and
discrepancy correction, rate-integrated, then adjusted by a manager base term
and a selected runtime table value.

## Anchors

- `game.exe @ RAM:00da6850, FUN_00da6850, raw/smoothed/integrated clock update`
- `game.exe @ RAM:00da3a10, FUN_00da3a10, clock-field initialization`
- `game.exe @ RAM:00a84a20, FUN_00a84a20, music-cursor millisecond sample`
- `game.exe @ RAM:00da2160 and RAM:00da3420, music-base scene writers`
- `game.exe @ RAM:0042d61e, FUN_0042d61e, live-or-captured elapsed timer`
- `game.exe @ RAM:00ff6ee0, FUN_00ff6ee0, performance-counter accumulation`
- `game.exe @ RAM:00ff6fe0, FUN_00ff6fe0, integer millisecond conversion`
- `game.exe @ RAM:01013df0, FUN_01013df0, float floor helper`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, manager-time composition`
- `game.exe @ RAM:00af8200, FUN_00af8200, correction-selector owner path`
- `game.exe @ RAM:00b61060, FUN_00b61060, selected correction adapter`
- `game.exe @ RAM:00433564, FUN_00433564, runtime correction-table lookup`
- `game.exe @ RAM:00c320d0, FUN_00c320d0, ViewTimingManager defaults`
- `game.exe @ RAM:0113ea70, FUN_0113ea70, correction-table file selection`
- `game.exe @ RAM:01140be0, FUN_01140be0, correction-record binary reader`

## Observations

- The scene base is a sampled music cursor converted as
  `trunc(double(cursor) / double(rate) * 1000.0)`, narrowed through double to
  float. Gameplay initialization resets the elapsed timer after sampling; the
  final pre-active state repeats the sample and waits for it to be positive.
- The timer wrapper either subtracts a stored origin from a captured integer or
  from the current timer using 32-bit modular arithmetic. The current-timer
  path accumulates performance-counter time at microsecond scale, divides by
  1000, and exposes the low 32-bit millisecond result.
- The outer update converts the returned 32-bit value as unsigned, multiplies
  it and a base-time field separately by initialized float `0.06`, and adds the
  products into the raw position.
- The same object retains previous raw, previous smoothed, current smoothed,
  and accumulated positions. Its constructor initializes quantizer bias to
  `1.0` or `0.5`, discrepancy threshold to `0.1`, and slew step to `0.01`.
- In ordinary mode, for error `E = raw - smoothed`, the quantized value is
  `smoothed + 1` for `-0.5 <= E <= 2.5`,
  `smoothed + floor(E - 0.5)` above that range, and
  `smoothed + floor(E + 1.5)` below it.
- Alternate mode uses half-unit quantization and a distinct double-precision
  `+0.8` branch for `0.9 <= E < 1.25`. A paired runtime-flag path bypasses both
  quantizers and advances by the mode bias.
- Outside the bypass, a post-quantization discrepancy strictly greater than
  `0.1` moves the smoothed value toward raw by `0.01`, with the source's inner
  snap branch preserved. Equality with `0.1` does not trigger correction.
- Accumulated position adds the smoothed delta times either `1.0` or the product
  of two runtime record fields.
- The manager stores its prior time, then computes current time as manager base
  plus the supplied accumulated/substep position minus a double selected from
  a runtime record table. An out-of-range selector yields zero.
- RTTI identifies the optional rate owner as `projView::ViewTimingManager`.
  Manager construction clears its enable byte and initializes every scalar
  factor to `1.0`; the complete singleton-reference set contains no internal
  writer, so the optional product path is dormant in normal executable flow.
- The correction selector is the bounded current `PlayOptionSet` ID. It indexes
  external `PlayOptionPlayTimingOffsetTableRecord.bin`; the selected double is
  narrowed to float before subtraction, while absent/out-of-range data yields
  zero.

## Reasoning

The independently sampled music cursor and reset elapsed timer establish the
clock's two millisecond inputs, while the initialized `0.06` multiplication
establishes 60 chart units per second. The outer update's retained fields and
operation order show that note judgement does not consume raw wall time
directly. The manager subtraction is on the same path immediately before
candidate preparation and TAP delta calculation, so it is part of the
authoritative judgement position.

## Alternatives and falsifiers

- Competing explanation: the smoothed/integrated fields are visual-only clocks.
- Evidence that would disprove this claim: a note-manager caller supplying an
  independent gameplay time, or TAP delta calculation reading a different
  manager field on an active gameplay path.

## Unknowns

- The player-facing meaning of the alternate-mode global is not named. The
  paired ViewTimingManager bypass flags have no recovered nonzero producer.
- External correction values remain explicit reconstruction inputs, and
  PlayOptionSet player-facing identities are intentionally unassigned.
- Music-cursor/rate combinations whose scaled double exceeds the unsigned
  64-bit conversion domain, NaN, infinity, and non-default floating-point
  environment behavior are not claimed beyond the finite gameplay domain.

## Consequences

- Ghidra mutations: added plate comments at `RAM:00a84a20` and
  `RAM:00da6850`.
- Spec sections: `spec/timing.md`.
- Reconstruction code: `include/chart/reconstruction.hpp`.
- Tests: `tests/play_clock_test.cpp`.

## Verification

The music-cursor producer, both scene writers, timer reset/capture path,
quantizer instruction boundaries, constructor constants, manager correction
lookup, external record reader, set-ID owner, and TAP consumer were traced
independently. Focused tests cover cursor conversion and guards, timer wrap,
ordinary and alternate quantizer branches, the strict correction threshold,
rate selection, correction bounds, integration, and manager composition.
Producer detail is independently recorded in
`claim.timing.music-cursor-base-time`; ownership detail is recorded in
`claim.timing.play-timing-runtime-ownership`.
