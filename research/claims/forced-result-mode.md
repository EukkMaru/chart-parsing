# Claim: a manager mode can force note results without physical input

- ID: `claim.judgement.forced-result-mode`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `input.logical_state`, `judgement.types`, `judgement.miss`, `note.tap`, `note.hold`, `state.ownership`, `config.external`, `interactions.cross_note`
- Last reviewed: 2026-07-27

## Statement

When a manager enable byte is set and its forced-result mode is nonzero, shared
start judgement can complete without physical input, HOLD sustain is treated as
active, and a manager-wide selector supplies anonymous result bytes. Although
the selector implements fixed, cycling, and random cases, exact-snapshot
producer closure makes tutorial mode 2 and its fixed byte 3 the only reachable
nonzero case.

## Anchors

- `game.exe @ RAM:00b2ae30, FUN_00b2ae30, manager reset, hash bd29062d3b65f1f97ae93fc82fb2ada49f46aa046abb6fb1af6034af02963fd7`
- `game.exe @ RAM:00b2a8c0, FUN_00b2a8c0, validated mode-enable setup, hash 3d3ca283af0a2b8c0a02013e185d1341c44f9b4a30d782a798f8fcf0f45bedf8`
- `game.exe @ RAM:00b29570, FUN_00b29570, forced-result byte selector, hash c5612ed134c344022e1639f915c0c15423fe81c89703de01fbe156f5a860b295`
- `game.exe @ RAM:00b29700, FUN_00b29700, companion forced-result metadata selector, hash 380e61ace412b0eef933c30cc3dec05f9c3abe879c6639aa4af54357b0cadf22`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, shared start-gate forced branch`
- `game.exe @ RAM:00c29c50, FUN_00c29c50, HOLD forced sustain/checkpoint/end branches`
- `game.exe @ RAM:00ce9ab0, FUN_00ce9ab0, PlayMusicTutorialObject::Impl construction`
- `game.exe @ RAM:00ce9110, FUN_00ce9110, tutorial callback registration`
- `game.exe @ RAM:00ce9270, FUN_00ce9270, tutorial-step mode producer, hash f5b7ab9a458164ed778c6decd190c9fa9e6e7450843194cb7d75da5408da3698`
- `game.exe @ RAM:01193000, FUN_01193000, indexed tutorial-step flag reader, hash 39f65507e07efb49e1cf1c31fb1c7633fab2f6bd86b8fd28d6f38c478ce980a6`
- `game.exe @ RAM:01c36764, RTTI text Impl@PlayMusicTutorialObject@projView@@`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, manager setup reset and enable-byte caller`
- `game.exe @ RAM:00b00be0, FUN_00b00be0, setup enable-byte source, hash 9c200851cfbde753419b83e765e4cf1631568ed1bcb47c95d7985a912d0b712d`
- `game.exe @ RAM:00da1c00, FUN_00da1c00, gameplay teardown mode reset, hash 03152b8d91e535c06dba12382f90c56f643b8398ca152a22352f707182cc1e8d`
- `game.exe @ RAM:00da4540, FUN_00da4540, controller destruction mode reset, hash 8087b1b0daf401dc0d1359417ddb580e951ce5e8b0a25e16abf94e44a1039156`

## Observations

- Manager reset clears the two bytes at `+0x2d0/+0x2d1`, forced-result mode at
  `+0x35c`, companion selector at `+0x360`, and shared cycle counter at
  `+0x364`. After chart validation succeeds, setup copies its current state
  object's inner byte at `+0x24` into enable `+0x2d0`; failed validation leaves
  the reset value.
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
  destruction clear the mode. The tutorial callback is the only nonzero
  writer. A whole-program instruction scan was intersected with every function
  that obtains the `DAT_01c77e64` manager singleton and with all manager-method
  callers. Within that owner set, `+0x35c` is never addressed separately or
  passed onward. Other textual `+0x35c` matches belong to unrelated large
  objects or stack locals. Modes 1 and 3 through 6 therefore have no producer
  or alias path and are unreachable in this exact snapshot.
- The same owner/write/address-escape audit leaves companion `+0x360` fixed at
  reset value zero. Its selector returns 0 while enable is clear and 1 while
  enable is set. Counter `+0x364` is reset and can be incremented only inside
  dormant mode 5, so it remains zero on the reachable mode-2 path.

## Reasoning

The shared manager fields are checked directly in both start and HOLD sustain
paths, and the selector owns the only cycle-counter mutation. This establishes
a gameplay-result override rather than a presentation-only option. RTTI,
callback registration, indexed step lookup, and the direct mode write identify
the concrete mode-2 producer as tutorial control rather than chart data or
judgement configuration. Complete writer and address-escape closure then
distinguishes dormant switch cases from reachable gameplay; no external mode
parameter reaches this internal field.

## Alternatives and falsifiers

- Competing explanation: the mode changes only feedback while ordinary input
  still determines result state.
- Evidence that would disprove this claim: a physical-input predicate on the
  active forced branch after its timing point, or a later mandatory gap/input
  reclassification replacing the selected byte before note result forwarding.

## Unknowns

- The player-facing semantic name of the setup state byte copied into enable
  `+0x2d0` remains unresolved. Its exact source field and validation gate are
  recovered.
- Player-facing names for dormant selector cases remain unresolved and do not
  affect reachable gameplay.
- Exact forced timing configuration values remain external parameters.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/configuration.md`, `spec/judgement.md`,
  `spec/notes/tap.md`, `spec/notes/hold.md`.
- Reconstruction code: `tutorial_forced_result_mode`,
  `forced_result_mode_has_snapshot_producer`,
  `select_reachable_forced_result_companion`, and
  `select_forced_result_byte` in `include/chart/reconstruction.hpp`.
- Tests: `tests/forced_result_test.cpp`.

## Verification

Reset, setup, all selector jump-table cases, start-gate bypass, HOLD activity,
checkpoint, end consumers, tutorial RTTI/callback/step lookup, singleton-user
inventory, direct writes, and address escapes were checked separately. Focused
tests distinguish the reachable mode set and fixed companion from the dormant
fixed, cycling, random-modulo, and out-of-range selector cases.
