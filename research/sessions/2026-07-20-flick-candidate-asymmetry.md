# Session 2026-07-20: FLK cross-family candidate asymmetry

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: continuing ownership of `pipeline.boundaries`, `judgement.types`, `note.tap`, and `note.hold`
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not listening

## Goal

Close tied and unequal FLK interactions with TAP/CHR/HOLD/Slide start
candidates, including whether the same substep can also alter terminal-result
routing.

## Findings

- Added reconstructed claim `research/claims/flick-candidate-asymmetry.md`.
- The manager finishes one all-note candidate-reduction pass before any note
  update. FLK contributes in phase 0 but its ordinary edge path ignores the
  selected minimum; the other four start families use the shared equality gate.
- For otherwise valid input, `FLK < gated` admits only FLK, `gated < FLK`
  admits both, and equality admits both. Storage order does not alter those
  candidate cases.
- Ordinary FLK edge acceptance only initializes phase-1 motion state. Its later
  result occurs after candidate exposure stops, so the asymmetry substep cannot
  also route an ordinary FLK terminal result. Forced mode is the explicit
  exception, but all shared start gates bypass candidate equality there and
  result routing follows vector order.
- Added `LaneCandidateGate` reconstruction and focused interaction tests.

## Ghidra mutations

None in the live project. All inspection used the temporary read-only clone;
source artifacts were not modified.

## Validation

- Rechecked manager, FLK candidate/update, shared start-gate, HOLD/Slide phase,
  result-wrapper, and forced-result paths independently.
- CMake configure/build succeeded; CTest passed all 15 tests, including new
  `chart.candidate_interaction` cases.
- `python3 scripts/harness.py validate` passed with 28 coverage rows and 10
  required files.

## Unresolved and contradictions

- A gated note suppressed by FLK can still accept a later edge or expire under
  its own rules; no player-facing outcome name is inferred.
- Full active-vector creation order across every parser family remains a
  separate audit concern for simultaneous results, not for candidate minima.

## Handoff

Resume at forced-result manager fields `+0x2d0/+0x35c/+0x360/+0x364`, their
reset at `RAM:00b2ae30`, and readers in `RAM:00c1af30`, `RAM:00c20340`, HOLD,
MNE, AIR, and Slide. The next question is the external selector producer and
whether it is gameplay setup, UI/debug control, or persistent configuration.
