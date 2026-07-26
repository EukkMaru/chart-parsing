# Claim: gameplay clock entry is based on the sampled music cursor

- ID: `claim.timing.music-cursor-base-time`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: `timing.clock`, `timing.tick_conversion`,
  `timing.discontinuity`
- Last reviewed: 2026-07-26

## Statement

Gameplay initialization and the final pre-active scene state sample the current
music playback cursor as integer milliseconds. A successful backend query
returns a nonnegative 64-bit cursor and a positive cursor-unit rate; the
adapter computes `trunc(double(cursor) / double(rate) * 1000.0)`. It returns
`UINT64_MAX` when the handle/query/range checks fail.

The scene narrows that unsigned result through double to float, resets a
QPC-backed elapsed timer, and later computes the raw gameplay clock in this
single-precision operation order:

```text
raw = float(music_cursor_milliseconds) * 0.06F
    + float(elapsed_milliseconds_modulo_2^32) * 0.06F
```

## Anchors

- `game.exe @ RAM:00a84a20, FUN_00a84a20, music-cursor query and millisecond
  conversion, hash dd8977fef96e7a9229f8728231e3733dfe20c4760a6c368ba08eff46f5e84ccf`
- `game.exe @ RAM:00e0a770, FUN_00e0a770, unsigned 64-bit-to-double adapter,
  hash a0de56fa782dd6c5375a392ec3afba511f01976107b8a82066a175f9057e5d60`
- `game.exe @ RAM:00da2160, FUN_00da2160, entry sample and timer reset,
  hash 78a615ec91c85ff2b36a16d65641931602c1b8dfa81ac9132335dc472867b086`
- `game.exe @ RAM:00da3420, FUN_00da3420, final pre-active sample and positive
  transition gate, hash 8cf57cb435089370db623e37f22f19ba5311c5c20b53299e4d7f90da85df8832`
- `game.exe @ RAM:00fe8820, FUN_00fe8820, resettable timer origin/capture reset,
  hash b0d03fbb0e83ec338e325c0d2aa98e3632dc273ba886aaaf55f05bfcbdaefb43`
- `game.exe @ RAM:00fe8840, FUN_00fe8840, captured-or-live modular elapsed
  subtraction, hash dce58e08b421adacdb444ce21bd6145b89fffe4b3ab88664d518066fa51aa624`
- `game.exe @ RAM:00ff6ee0, FUN_00ff6ee0, QPC-to-microsecond accumulator,
  hash 722151b17fa68fc6e2045a8d5e917f1fea67f11fd2bf1c843a93bd0fda1425fd`
- `game.exe @ RAM:00ff6fe0, FUN_00ff6fe0, low-32-bit millisecond conversion,
  hash 03fefaeb462efe04030448fc76447c70f6b4ef46c4927b253f7163395a23316e`
- `game.exe @ RAM:00da6850, FUN_00da6850, raw/smoothed/integrated clock,
  hash 3398b5e53561cdee93f8519a77c4d2db07a014427ac0b4108fa6e2469b9253eb`

## Observations

- The cursor adapter bounds a handle index, obtains its registered backend,
  and calls one virtual query. Failure, a cursor whose high word is negative,
  or a nonpositive returned rate retains the all-ones 64-bit sentinel.
- On success, the adapter converts the cursor as unsigned to double, divides by
  the returned integer rate, multiplies by the initialized double `1000.0`,
  and truncates through the executable's unsigned-double conversion helper.
- Gameplay initialization calls that adapter, converts its unsigned 64-bit
  return to double and then float, writes scene offset `+0x84`, clears the five
  clock fields, and resets the timer origin while preserving its running state.
- Structural state `0x0e` repeats the same cursor sample after its zero-base
  priming update. It stores the float and enters live state `0x0f` only when
  the sampled float compares greater than zero.
- The live clock reads a captured timer value when paused and the current timer
  otherwise. Both subtract the stored origin with a 32-bit `SUB`. The QPC
  owner accumulates monotonically observed time as microseconds, divides by
  1000, and exposes only the low 32-bit millisecond result.
- The clock converts the elapsed subtraction as unsigned and separately
  multiplies it and the sampled music cursor by `0.06F` before adding them.
- A whole-function field audit excluded apparent offset-`+0x84` accesses in
  `FUN_00da3890` and `FUN_00da7270`: they belong to independent copied record
  objects, not the gameplay scene. Scene construction initializes the actual
  clock-base field to zero.

## Reasoning

The cursor adapter's ratio and literal `1000.0` establish the sampled value's
millisecond unit without relying on an external API name. The two scene
writers, timer reset sequence, and later raw-clock reader close the value from
backend query through the judgement clock. The low-word timer return,
instruction-level subtraction, and unsigned conversion establish modulo
`2^32` elapsed behavior.

## Alternatives and falsifiers

- Competing explanation: scene offset `+0x84` is an external chart/configuration
  offset unrelated to playback, or the QPC timer alone defines absolute chart
  time.
- Evidence that would disprove this claim: another scene-field writer on the
  live path, a cursor adapter unit other than the observed ratio times 1000, a
  timer reset after the sample that uses a different origin, or a note-manager
  path supplied by an independent raw clock.

## Unknowns

- The player-facing name of the music handle and the backend virtual interface
  remain unassigned.
- Malformed cursor/rate combinations whose scaled double exceeds the unsigned
  64-bit conversion domain are outside the observed gameplay resource
  invariant.
- The alternate gameplay-mode global remains semantically unnamed.

## Consequences

- Ghidra mutations: added plate comments at `RAM:00a84a20` and
  `RAM:00da6850`.
- Spec sections: `spec/timing.md`.
- Reconstruction code: `music_cursor_milliseconds`,
  `music_cursor_base_time_value`, `elapsed_milliseconds_modulo`, and
  `make_raw_play_position` in `include/chart/reconstruction.hpp`.
- Tests: `tests/play_clock_test.cpp`.

## Verification

The cursor query arithmetic was checked at instruction level and independently
followed through both scene writers, the timer reset/capture methods, the QPC
accumulator, the raw-clock consumer, and the outer update's separate
monotonic-cursor retention. Focused tests cover ordinary conversion, zero,
failure guards, timer wrap, and final raw-position operation order.
