# Session 2026-07-20: AIR secondary ownership and judgement

- Investigator: codex-root
- Ghidra writer: none; temporary read-only project clone only
- Coverage rows claimed: `pipeline.boundaries`, `note.air`; advanced to `note.slide`
- Binary hash checked: yes
- MCP health checked: yes; configured endpoint unavailable

## Goal

Close parsed secondary type 3 from AIR-family parser attachment through runtime
object ownership, input selection, timing/result state, shared routing, and
deferred removal without treating it as a missing standalone root type.

## Findings

- Added `claim.note.air-secondary-judgement`.
- Added the normative vertical slice in `spec/notes/air.md` and integrated its
  parser, input, matching, judgement, and external-configuration consequences
  into the shared specifications.
- Added a clean-room direction/profile map, snapshot mark-before-read model,
  fixed AIR result table, retained timing transition, phase/candidate helpers,
  and result-category selector.
- Closed the apparent current-input ambiguity: helper `RAM:00c2f200` writes a
  separate snapshot marker at `+0x54`; AIR reads the selected derived profile
  byte at `+0x4c + profile`. The following snapshot builder reads the marker.
- Advanced active vertical-slice ownership from `note.air` to `note.slide`.

## Ghidra mutations

None in the live `chart` project. GhidraMCP was not exposed and its configured
HTTP endpoint was not listening. Static inspection used a temporary project
clone under `/tmp`; function creation there was temporary and did not mutate
source artifacts.

## Validation

- `python3 scripts/harness.py doctor`: binary identity and local source health
  passed; MCP endpoint remained unavailable.
- `python3 scripts/harness.py validate`: passed, 28 coverage rows and 10
  required files.
- CMake configure/build: passed.
- CTest: 13/13 passed, including new `chart.air_judgement` coverage for mirror
  codes, profile selection, marker separation, timing retention/boundaries,
  candidate/phase behavior, and source routing.
- Independently checked parser failure and success paths, root/secondary append
  order, RTTI and vtable family, external profile selection, input marker
  initializer/writer/consumer, retained table, forced branch, source category,
  base terminal request, and removal ordering.

## Unresolved and contradictions

- Physical meanings and full synthesis of derived input profiles remain open.
  The `+0x54` field is not a profile byte; the earlier direct-force hypothesis
  was rejected by offset and next-builder data flow.
- External AIR timing values, corrections, forced point, defaults, units, and
  mode identities are unavailable and remain parameters.
- Root types outside 0, 1, 2, 4, 6, 11, and 13 do not enter the recovered AIR
  profile initializer; whether malformed/backward-compatible charts can attach
  AIR to them needs corpus/schema audit.
- No persistent Ghidra names/comments/types were applied because the live
  write channel was unavailable.

## Handoff

Continue the claimed `note.slide` slice at factory `RAM:00b28cc0` type 2. Close
the `0x294`-byte object's constructor/RTTI/vtable, parsed path/control-point
load, candidate production, input and timing update, segment ownership, result
category, reset/error paths, and deferred removal. Cross-check its final point
fields against attached AIR load `RAM:00c1e730`. Keep broad active ownership
rows with `codex-root`; `note.air` is released as mapped.
