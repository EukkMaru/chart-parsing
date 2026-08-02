# Claim: SLA tags select projection schedules that can change note materialization time

- ID: `claim.parser.sla-materialization-selection`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `pipeline.boundaries`, `timing.clock`, `interactions.cross_note`
- Last reviewed: 2026-07-27

## Statement

`SLA` creates a parsed type-12 chart-time/lane region carrying an integer tag.
Queries fully contained by overlapping regions receive the greatest positive
tag. The region itself has no runtime-note factory case, but its selected root
and endpoint tags choose keyed STP/SFL/SLP schedules in the far-path
materialization predicate and can therefore change when an ordinary note first
becomes able to participate in candidates, input, and judgement.

## Anchors

- `game.exe @ RAM:011c1030, FUN_011c1030, integer-lane region query, hash bfec6950b674ce8edc1606194153e5c8bdd0bd24a832858be27caed9b68dfd22`
- `game.exe @ RAM:011c1330, FUN_011c1330, float-lane region query`
- `game.exe @ RAM:011c0e80, FUN_011c0e80, root/end tag assignment, hash b0192e96f90af45da2fc66b85bc746dec398d59c9727524e1db6497c23c90d7d`
- `game.exe @ RAM:011c6720, FUN_011c6720, keyed schedule integration, hash b2799e1cd7b3106c6e20599a1e99eed1465844c510ca38b9c565642acd38bd75`
- `game.exe @ RAM:00b29c90, FUN_00b29c90, materialization tag consumers, hash 88a81914b32218d202582c3aa46579ad51e4dec0f6a32da750c57cce674a7584`
- `game.exe @ RAM:00da62c0, FUN_00da62c0, retain/construct/erase decision, hash cb798253647b9f10eed6cd3d3aeaa4417cbc410ea3682481f762a20829cfa27b`

## Observations

- SLA has six integer arguments after the command: major position, minor
  position, lane, width, duration, and tag. Width is clamped to 1 through 16;
  mirrored parsing replaces lane with `16 - lane - width`. The normalized end
  uses `(major, minor + duration)`. Those subtractions and the addition wrap at
  signed 32-bit width before position normalization.
- Both region queries add exact float `1/192` to chart position, use a
  half-open time interval, require full lane-span containment, and retain only
  a strictly greater tag starting from zero. The AirLadder float-span variant
  additionally uses initialized `-0.00001F/+0.00001F` lane tolerances. Integer
  query and region lane ends use wrapped 32-bit additions; the float-span
  query converts the wrapped region end to float.
- Postprocessing writes selected tags to parsed roots and supported endpoints,
  including fields `+0x7c` and `+0x80`. It skips SLA records themselves.
- The primary factory omits parsed type 12, so an SLA record is consumed
  without allocating a runtime note.
- For other pending records whose raw delta is at least `30.0F`,
  `FUN_00b29c90` reads a nonnegative root or endpoint tag, passes it as the
  exact keyed-schedule lookup key, and uses the transformed target in its
  inclusive far projection bounds.
- A failed far bound retains the pending record; a passed bound constructs and
  erases it. The established runtime-note dispatch permits candidate and
  type-specific gameplay only on later manager substeps.

## Reasoning

The region selector, tag assignment, materialization read, exact keyed lookup,
and retain/construct branch form one continuous data path. SLA does not alter
the manager clock or timing-window classifier, but that narrower fact does not
exclude gameplay impact: delaying or advancing construction changes whether a
runtime note exists on a later input/judgement substep.

## Alternatives and falsifiers

- Competing explanation: the keyed transform affects only drawing after a note
  already exists.
- Evidence that would disprove this claim: a distinct presentation-only caller
  rather than `FUN_00b29c90`, or a queue branch that ignores the transformed
  eligibility result.

## Unknowns

- Player/editor-facing names for SLA tags remain intentionally unassigned.
- Nonfinite keyed-schedule ordering and materialization branches are closed by
  `claim.timing.projection-schedule-materialization`.

## Consequences

- Ghidra mutations: the schedule comparator function boundary at
  `RAM:011c3c60` was recovered during the same audit; no SLA rename was made.
- Supersedes: `claim.parser.sla-region-selection`.
- Spec sections: `spec/c2s.md`, `spec/timing.md`.
- Reconstruction code: `C2sSlaRegion`, both tag selectors,
  `C2sProjectionSchedule`, and
  `runtime_materialization_probe_from_schedule`.
- Tests: `tests/sla_region_test.cpp`,
  `tests/projection_schedule_test.cpp`.

## Verification

Focused tests preserve the parser, overlap, boundary, mirroring, and
float-tolerance rules, cover every malformed 32-bit wrap site, and add an
end-to-end synthetic case in which an SLA tag selects a full-stop SLP interval
and changes far-path materialization eligibility. Independent corpus
aggregation found 41 charts containing both SLA and SLP and 40 with shared
keys; a conservative scan found nine charts with ordinary roots assigned to
matching nonidentity SLP schedules.
