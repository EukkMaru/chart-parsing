# Session 2026-07-20: alternate-meter auxiliary lifetime

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: continuing ownership of `pipeline.boundaries`, `judgement.types`, `note.tap`, and `note.hold`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not listening

## Goal

Close the producer, selection rule, guard, reset paths, and consumers of the
alternate terminal meter's auxiliary field at subobject offset `+0x48`, without
inventing unavailable player-facing identities.

## Findings

- Extended `research/claims/alternate-terminal-meter.md` with the bounded
  participant-count derivation and complete capture/reset lifetime.
- Initial gameplay setup captures the count, participant limit, and contribution
  sums and sets a guard byte. A later periodic setup path requests a refresh but
  cannot overwrite the guarded capture.
- Setup modes 1, 2, and 4 restrict selection to the local identity. Other modes
  can admit flagged non-local records; those contribute only with nonzero
  effective current. The local identity is omitted when the meter zero latch is
  already set.
- Construction and configuration load clear the guard and auxiliary fields.
  The terminal predicate and presentation paths read the captured count; no
  result-time decrement exists in the traced snapshot.
- Added clean-room participant-selection and capture-lock helpers and expanded
  `tests/alternate_meter_test.cpp`.

## Ghidra mutations

None in the live project. GhidraMCP was unavailable. Missing functions and
thunks were created/disassembled only in `/tmp/chart-readonly-20260720`; source
artifacts were not modified.

## Validation

- Binary identity and workspace health were checked by `harness.py doctor` at
  session start.
- Audited all direct `+0x48` accesses, all callers of both meter-subobject
  getters, the initial and periodic owner call sites, and every method in the
  contiguous meter-function range.
- CMake configure/build succeeded; CTest passed all 14 tests, including the
  expanded `chart.alternate_meter` selection and lock cases.
- `python3 scripts/harness.py validate` passed with 28 coverage rows and 10
  required files.

## Unresolved and contradictions

- Player-facing identities for the auxiliary mode, setup modes, participant
  flags, and contribution fields remain unavailable.
- The count is frozen for an initialized gameplay setup; this contradicts the
  earlier tentative interpretation of `+0x48` as a per-result decrementing
  counter. The specification now calls it a setup-derived participant count.
- External configuration values remain parameters.

## Handoff

Resume the count path at participant selection `RAM:00da8cf0`, guarded capture
`RAM:00da8a20`, initial locked caller `RAM:00da7b30`, periodic caller
`RAM:00da9820`, predicate `RAM:00c41520`, and presentation consumer
`RAM:00ce14b0`. The next bounded target is FLK's candidate asymmetry at
`RAM:00c1f6a0`/`RAM:00c20340` against manager reduction `RAM:00b2b690`.
