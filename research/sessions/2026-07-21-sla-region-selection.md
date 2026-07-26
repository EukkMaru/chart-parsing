# Session 2026-07-21: event fallback and SLA region selection

- Investigator: codex-root
- Ghidra writer: none; temporary-clone analysis only
- Coverage rows claimed: `parser.events` (advanced, still investigating)
- Binary hash checked: yes
- MCP health checked: yes; unavailable to this client and HTTP endpoint not listening

## Goal

Close unknown-command, event-arity, and malformed numeric behavior, then take
the simplest well-supported unclosed event family through parsing and all
downstream consumers.

## Findings

- Added reconstructed claim `research/claims/event-token-fallback.md`.
- Added reconstructed claim `research/claims/sla-region-selection.md`.
- Established that both event-handler callers disable the dormant descriptor
  arity validator. Unknown commands are dropped before dispatch; recognized
  commands use missing-field zero fallback, prefix-accepting numeric conversion,
  and uncaught conversion/range errors.
- Reconstructed SLA's six-field region, width clamp, mirror rule, normalized
  end, shifted half-open time containment, integer and tolerant float lane
  containment, and maximum-positive-tag overlap rule.
- Closed the SLA scope boundary: type 12 has no runtime factory entry. Selected
  tags propagate into note/path display state and tag-keyed scroll projection,
  with no read in the independently closed gameplay clock or judgement paths.
- Added clean-room region parsing/selection and focused tests, and made the SLA
  grammar normative in `spec/c2s.md`.

## Ghidra mutations

None in the live project. GhidraMCP remained unavailable. One missing thunk was
created only in `/tmp/chart-readonly-20260720` to resolve the type-10 virtual
loader in preparation for the next target; source artifacts were not modified.

## Validation

- `python3 scripts/harness.py doctor` confirmed binary/project/corpus identity;
  MCP HTTP remained unavailable.
- Local corpus aggregation found 1,186 SLA records across 41 charts. Every
  record has six arguments and a positive tag in the range 1 through 500.
- Focused tests cover missing/extra/malformed fields, width clamps, mirroring,
  normalization, overlap and boundary selection, nonpositive tags, and float
  lane tolerance.
- CMake configure/build succeeded and all 23 CTest tests passed.
- `python3 scripts/harness.py validate` passed with 28 coverage rows and 10
  required files.

## Unresolved and contradictions

- The presentation name/effect of the SLA integer tag is not assigned; only its
  parser algorithm, scroll-projection consumer, and judgement exclusion are
  claimed.
- No local chart contains ASO, so its next slice must rely on exact-binary
  support and synthetic tests rather than corpus examples.
- An initial exact-tolerance float test exposed host single-precision rounding;
  the fixture was moved strictly inside the recovered inclusive boundary. This
  does not change the executable comparison or its specified exact constant.

## Handoff

`parser.events` remains owned by codex-root. Resume shared ingestion at
tokenizer `RAM:011d03f0`, caller pair `RAM:011c78e0`/`RAM:011c7980`, and event
handler `RAM:011c8870`. SLA resumes at query helpers `RAM:011c1030` and
`RAM:011c1330`, tag postprocessor `RAM:011c0e80`, factory `RAM:00b28cc0`, and
projection helper `RAM:011c6720`. The next target is absent-corpus ASO: begin at
the type-10 branch in `RAM:011c8870`, factory case 10 `RAM:00b28cc0`,
constructor `RAM:00c163d0`, virtual loader `RAM:00c16fa0`, then close its update
vtable, state ownership, and any candidate/result path before assigning names.
