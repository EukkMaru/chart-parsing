# Research workflow

## Strategy: map, slice, audit

Use a hybrid strategy.

### 1. Maintain the broad map

Locate boundaries for chart loading, parsing, chart-object construction, the
gameplay update loop, device-to-logical-input conversion, note dispatch,
judgement production, and teardown/reset. A boundary may remain a hypothesis,
but unknown calls and shared state must appear in `research/COVERAGE.tsv`.

### 2. Complete vertical slices

Start with the simplest well-supported note type, normally TAP, and trace it
from parsed event through final judgement. Then expand to stateful and compound
types. Each slice must account for:

- construction and ownership;
- scheduling and ordering;
- update state machine and reset/destruction;
- input matching/consumption;
- early/late/miss boundaries;
- simultaneous and overlapping notes;
- shared helpers and external configuration;
- observable output passed downstream.

Do not assume helpers behave identically across note types. Promote shared
logic only after comparing call paths.

### 3. Periodically audit sideways

After each vertical slice, revisit the broad map for newly discovered indirect
calls, modes, flags, alternate constructors, error paths, or configuration.
Update coverage before choosing the next slice.

## Investigation unit

Each bounded target has one row in `research/COVERAGE.tsv` and one or more claim
files under `research/claims/`. Prefer a question that can be closed, such as
“how are simultaneous TAP candidates ordered?” over “understand judgement.”

Allowed statuses are `unknown`, `mapped`, `investigating`, `reconstructed`,
`verified`, and `excluded`. Allowed confidence values are `none`, `low`,
`medium`, and `high`.

- `mapped`: entry/owner is located, behavior is not reconstructed.
- `reconstructed`: control/data flow is expressed as a clean-room algorithm and
  no known branch inside the target remains unexplained.
- `verified`: reconstruction is checked by an independent call path, corpus
  property, focused test, or separately derived invariant. Static analysis can
  verify structure even when unavailable constants prevent numeric validation.

## Session protocol

At session start, read the latest status and handoff. Take ownership in coverage
before mutating Ghidra. At session end, update:

- claims and clean-room specs;
- Ghidra names/types/comments made with high confidence;
- coverage status, evidence path, spec path, and test path;
- `research/STATUS.md` with blockers and the next best questions;
- a dated handoff based on `research/templates/SESSION.md`.

## Multiple agents

Default to one active investigator. When parallel work is explicitly requested:

- appoint exactly one Ghidra writer;
- give other agents disjoint, read-only questions;
- record ownership in coverage before work begins;
- merge evidence in repository notes before the writer applies annotations;
- never let two agents rename/type the same function or structure concurrently.
