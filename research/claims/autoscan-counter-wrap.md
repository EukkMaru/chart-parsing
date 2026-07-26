# Claim: ordinary catch-up uses a modulo-2^32 successful-scan sequence

- ID: `claim.timing.autoscan-counter-wrap`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `timing.tick_conversion`, `timing.discontinuity`,
  `input.buffering`
- Last reviewed: 2026-07-26

## Statement

The counter that bounds ordinary gameplay catch-up is a 32-bit sequence
initialized to zero and incremented once for each successful newly available
AutoScan update. It reaches the gameplay input owner without arithmetic
conversion. The ordinary substep loop first performs 32-bit modular
`previous - current + 1`, interprets those result bits as signed, clamps to
`[-1, 0]`, and iterates through zero.

Counter wrap is therefore seamless for adjacent scans, and every discontinuity
still produces either the zero substep alone or the bounded `-1, 0` pair.

## Anchors

- `game.exe @ RAM:01043a90, FUN_01043a90, AutoScan construction and sequence
  zero, hash f3a78de6c598a70b0f2a242ba782d9b3ed017051d841ebc37ff651adc0a6b5df`
- `game.exe @ RAM:01043c20, FUN_01043c20, successful-scan increment,
  hash 10235301424bc215abf28cb8e6b5baa82ac6cbcb5c7034b9643aa7f345df4939`
- `game.exe @ RAM:01043d20, FUN_01043d20, sequence getter,
  hash bce13b731521a8fabb0ea242d9385167450cc79830b578d5deeb2af417bd52d3`
- `game.exe @ RAM:0103c250, FUN_0103c250, BoardCtrl update copy,
  hash c0806f34513fb006114d075bb64f12415561aa91ebb068cfc3c0400114ba9ba2`
- `game.exe @ RAM:0103c3b0, FUN_0103c3b0, BoardCtrl sequence adapter,
  hash cd5eb578d06253353b8a0651322b642e45ac72e5a7d8ad486aab99f11455747e`
- `game.exe @ RAM:00a80240, FUN_00a80240, input-owner sequence copy,
  hash 21334e295d3606f56aeceff0ff7a8c80c6767915b51a937ade61d1735cbd0436`
- `game.exe @ RAM:00a7fed0, FUN_00a7fed0, gameplay sequence getter,
  hash c8c0547a3f3ddd0f4f1172b7fc96bf595b8731d6af3064fa93e2f28f001eb7a3`
- `game.exe @ RAM:00da8730, FUN_00da8730, modular signed clamp and substep
  loop, hash ed2d269bf3ba4bfcefd8300792569c3c467773fc2b2a543636e5db55b918712a`

## Observations

- AutoScan construction writes zero to object offset `+0x144`.
  `FUN_01043c20` increments that dword with one native 32-bit `INC` only after
  the scan backend reports a newly available successful update. There is no
  overflow test, saturation, reset, or wider carry.
- Whole-program operand search found the constructor zero, increment, and
  getter as the only instruction references to this `+0x144` field apart from
  unrelated allocation-size constants.
- BoardCtrl's update copies the getter result into its dword at `+0xe0`; its
  adapter returns that value. The input update copies it into the input owner
  at `+0x4c`, and the gameplay getter returns it unchanged.
- The ordinary loop subtracts current from the stored previous dword, adds one,
  and only then performs signed conditional clamps to `-1` and zero. It stores
  current after processing the final zero offset.
- The exact wrap from `0xffffffff` to zero yields a modular difference of zero,
  so it produces the ordinary zero substep rather than an extreme lag.

## Reasoning

The complete field-reference inventory fixes the sequence's initialization and
sole increment. The getter/copy chain has no narrowing because every stage is
already one dword. Finally, the loop's arithmetic precedes its signed
comparisons, proving that modulo wrapping occurs before signed clamp semantics;
an unbounded mathematical subtraction is not equivalent at half-range
discontinuities.

## Alternatives and falsifiers

- Competing explanation: the counter is a gameplay-frame counter, saturates at
  overflow, or is widened before the catch-up calculation.
- Evidence that would disprove this claim: another writer/reset of AutoScan
  `+0x144`, an update path that increments without a new successful scan, a
  transformed copy in the BoardCtrl/input chain, or a widened subtraction
  before the ordinary loop's signed clamp.

## Unknowns

- The hardware/backend condition that makes a scan newly available is outside
  chart and judgement ownership; only its successful-update edge is relevant
  here.
- Stable alternate-mode gameplay bypasses this catch-up loop, so this counter's
  value does not create alternate-mode substeps.

## Consequences

- Ghidra mutations: added plate comments at `RAM:01043c20` and
  `RAM:00da8730`.
- Spec sections: `spec/timing.md`, `spec/input.md`.
- Reconstruction code: modular `first_catch_up_offset` in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/play_clock_test.cpp`.

## Verification

The AutoScan field's complete instruction-reference set, the independent
BoardCtrl/input copy chain, and the ordinary loop's instruction order were
checked separately. Focused tests cover adjacent forward updates, ordinary
lag, full wrap, reverse wrap, and a half-range case that distinguishes native
32-bit arithmetic from the previous mathematical 64-bit reconstruction.
