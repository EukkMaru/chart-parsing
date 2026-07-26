# Claim: a manager mode can force note results without physical input

- ID: `claim.judgement.forced-result-mode`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `input.logical_state`, `judgement.types`, `judgement.miss`, `note.tap`, `note.hold`, `state.ownership`, `config.external`, `interactions.cross_note`
- Last reviewed: 2026-07-20

## Statement

When a manager enable byte is set and its forced-result mode is nonzero, shared
start judgement can complete without physical input, HOLD sustain is treated as
active, and a manager-wide fixed, cycling, or random selector supplies anonymous
result bytes 0 through 4.

## Anchors

- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, manager reset`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, validated mode-enable setup`
- `game.exe @ RAM:00b29570, FUN_00b29570, forced-result byte selector`
- `game.exe @ RAM:00b29700, FUN_00b29700, companion forced-result metadata selector`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, shared start-gate forced branch`
- `game.exe @ RAM:00c29c50, FUN_00c29c50, HOLD forced sustain/checkpoint/end branches`
- `game.exe @ RAM:00ce9ab0, FUN_00ce9ab0, PlayMusicTutorialObject::Impl construction`
- `game.exe @ RAM:00ce9110, FUN_00ce9110, tutorial callback registration`
- `game.exe @ RAM:00ce9270, FUN_00ce9270, tutorial-step mode producer`
- `game.exe @ RAM:01193000, FUN_01193000, indexed tutorial-step flag reader`
- `game.exe @ RAM:01c36764, RTTI text Impl@PlayMusicTutorialObject@projView@@`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, manager setup reset and enable-byte caller`
- `game.exe @ RAM:00da1c00, FUN_00da1c00, gameplay teardown mode reset`
- `game.exe @ RAM:00da4540, FUN_00da4540, controller destruction mode reset`

## Observations

- Manager reset clears the two bytes at `+0x2d0/+0x2d1`, forced-result mode at
  `+0x35c`, companion selector at `+0x360`, and shared cycle counter at
  `+0x364`. A validated setup path copies two caller-supplied bytes into
  `+0x2d0/+0x2d1`.
- The forced-result selector returns 0 when the enable byte is clear. With it
  set, mode 1 returns byte 4, mode 2 returns 3, mode 3 returns 2, and mode 4
  returns 1.
- Mode 5 returns 4, 3, 2, 1 according to `counter & 3`, then increments the
  manager-wide counter. Mode 6 returns the same sequence plus 0 according to
  `random_value % 5`. Other mode values return 0.
- The selector has a second stack argument but does not read it in any branch
  in this snapshot. HOLD calls it with different values for start/end and
  checkpoint contexts without changing selection.
- The shared TAP/HOLD start gate enters its forced branch only when enable is
  set and mode is nonzero. After a runtime timing point, it chooses a forced
  byte and completes without lane rising-edge or candidate-equality tests. If
  that timing point has not arrived, the ordinary late bound can still produce
  terminal byte 0.
- HOLD initializes aggregate sustain activity to true under the same active
  condition. Its checkpoint and once-only end paths select the forced byte
  instead of classifying retained inactive gap.
- `projView::PlayMusicTutorialObject::Impl` registers `FUN_00ce9270` as an
  update callback. When an indexed tutorial step reaches its scheduled point,
  that callback reads a step-record flag and writes manager mode 2 if the flag
  equals one, otherwise mode 0. Because mode 2 selects byte 3, this is a
  concrete in-binary producer for the fixed byte-3 override.
- The tutorial callback writes the mode regardless of the enable byte. The
  override becomes active only when the manager's separately supplied enable
  byte is nonzero. `FUN_00b2a8c0` writes that byte and its companion only after
  validating caller-supplied setup state; `FUN_00da06c0` is its sole caller.
- General manager reset, gameplay setup, gameplay teardown, and controller
  destruction clear the mode. A full direct-offset scan found no other
  nonzero write to this manager field. Modes 1 and 3 through 6 remain supported
  selector cases but have no statically recovered in-binary producer. The
  companion integer at `+0x360` is reset and read but likewise has no recovered
  nonzero writer.

## Reasoning

The shared manager fields are checked directly in both start and HOLD sustain
paths, and the selector owns the only cycle-counter mutation. This establishes
a gameplay-result override rather than a presentation-only option. RTTI,
callback registration, indexed step lookup, and the direct mode write identify
the concrete mode-2 producer as tutorial control rather than chart data or
judgement configuration. Because the counter belongs to the manager,
cycling-mode calls from different notes share one sequence if such a mode is
supplied externally.

## Alternatives and falsifiers

- Competing explanation: the mode changes only feedback while ordinary input
  still determines result state.
- Evidence that would disprove this claim: a physical-input predicate on the
  active forced branch after its timing point, or a later mandatory gap/input
  reclassification replacing the selected byte before note result forwarding.

## Unknowns

- The upstream semantic names of the two enable/setup bytes remain unresolved.
- Player-facing names for selector modes remain unresolved. Only tutorial mode
  2 has a recovered nonzero producer; reachability of modes 1 and 3 through 6
  and nonzero companion values requires an untraced generic/indirect or
  external producer.
- Mode 6 RNG seed/state and reproducibility are not recovered; the
  reconstruction accepts the generated value as an input.
- Exact forced timing configuration values remain external parameters.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/configuration.md`, `spec/judgement.md`,
  `spec/notes/tap.md`, `spec/notes/hold.md`.
- Reconstruction code: `tutorial_forced_result_mode` and
  `select_forced_result_byte` in `include/chart/reconstruction.hpp`.
- Tests: `tests/forced_result_test.cpp`.

## Verification

Reset, setup, all selector jump-table cases, start-gate bypass, HOLD activity,
checkpoint, end consumers, tutorial RTTI/callback/step lookup, and direct field
writes were checked separately. Focused tests cover the tutorial mode mapping,
disabled, fixed, cycling, random-modulo, and out-of-range modes.
