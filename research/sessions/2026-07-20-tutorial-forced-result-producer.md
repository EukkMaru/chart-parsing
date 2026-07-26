# Session 2026-07-20: tutorial forced-result producer

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: `note.hold` completed and released mapped; continuing ownership of `pipeline.boundaries`, `judgement.types`, and `note.tap`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not listening

## Goal

Recover the producer and reset lifetime of the manager forced-result mode and
distinguish executable producers from supported-but-unobserved selector cases.

## Findings

- Extended `research/claims/forced-result-mode.md` with the concrete producer.
- RTTI identifies the owner as `projView::PlayMusicTutorialObject::Impl`. Its
  registered update callback writes mode 2 when an indexed tutorial-step flag
  equals one and mode 0 otherwise at a scheduled step transition.
- Mode 2 selects anonymous result byte 3. The write is independent of the
  enable byte; the override activates only if validated manager setup supplied
  a nonzero enable.
- Manager reset plus gameplay setup, teardown, and controller destruction clear
  the mode. A direct-offset writer audit found no other nonzero mode producer.
  Modes 1 and 3 through 6 and nonzero companion-selector values remain
  supported interfaces with unresolved reachability.
- Added `tutorial_forced_result_mode` and focused integration coverage in the
  existing forced-result test.

## Ghidra mutations

None in the live project. A previously undefined thunk at `RAM:00424d34` was
created only in `/tmp/chart-readonly-20260720` to recover callback registration;
source artifacts were not modified.

## Validation

- Traced the tutorial implementation RTTI, callback table construction,
  scheduled update, indexed record flag, mode write, selector, consumers, and
  reset paths independently.
- Audited direct `+0x35c/+0x360/+0x364` accesses across executable functions.
- CMake build succeeded; CTest passed all 15 tests, including the tutorial-mode
  integration in `chart.forced_result`.
- `python3 scripts/harness.py validate` passed with 28 coverage rows and 10
  required files.

## Unresolved and contradictions

- Exact semantic names for enable bytes and tutorial step fields remain
  unresolved.
- The selector implements six nonzero modes, but only mode 2 has a recovered
  nonzero writer. Their mere switch presence is not treated as proof of normal
  reachability.
- Mode 6 RNG ownership remains outside the recovered path.

## Handoff

The mode-2 path is anchored at tutorial construction `RAM:00ce9ab0`, callback
registration `RAM:00ce9110`, callback `RAM:00ce9270`, step predicate
`RAM:01193000`, selector `RAM:00b29570`, and resets `RAM:00b2ae30`,
`RAM:00da06c0`, `RAM:00da1c00`, and `RAM:00da4540`. Next perform the TAP
contradiction/closure audit from candidate preparation through terminal removal.
