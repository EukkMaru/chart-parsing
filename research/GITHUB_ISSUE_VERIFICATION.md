# GitHub issue and temporary-handoff verification

Issues 1-12 were verified against the exact local `game.exe` snapshot on
2026-08-07. The requested updated-issue pass rechecked issues 13-15, 17, and 18
on 2026-08-18. Issue 16 is deliberately excluded because the owner designated
it as human-only; this document makes no finding about it. GitHub state is
inventory metadata, not an evidence grade. “Binary answered” means a closed
producer/consumer path exists in this snapshot; “product partial” means the
current offline viewer still does not implement all of that answer.

GitHub issue 12 was rechecked on 2026-08-09. Its body is unchanged, it has no
comments, and its GitHub `updated_at` value remains 2026-08-07 08:00:13 UTC.
It is the same issue included in this twelve-issue pass, not a new thirteenth
issue or a new binary question. The active claims cited below still answer
each requested verification item against the unchanged binary hash.

The old twelve-issue inventory below is retained for continuity and the five
newly reviewed issues follow it. States are the GitHub states observed on
2026-08-18; they do not change local evidence maturity.

| Issue | GitHub state | Binary verdict | Viewer/product state | Remaining boundary |
| ---: | --- | --- | --- | --- |
| [1](https://github.com/EukkMaru/chart-parsing/issues/1) SLP vocabulary | closed | answered | implemented and differentially covered | None for SLP grammar/schedule; broader camera is separate. |
| [2](https://github.com/EukkMaru/chart-parsing/issues/2) profile 7 physical meaning | open | interface answered; proposed meaning unprovable | parameterized | Inclusive range synthesis and profile index 7 are exact. External range values and “upward exit present/absent” label are unavailable. |
| [3](https://github.com/EukkMaru/chart-parsing/issues/3) ALD judged checkpoints | open | answered; issue premise is stale | implemented | Result-state simulation and external checkpoint resources remain separate viewer work. |
| [4](https://github.com/EukkMaru/chart-parsing/issues/4) Slide checkpoint classes | closed | answered | implemented for ordinary Slide; HLD style routes to the separate HeavenHold path | Gameplay-state-driven result transitions and external resources remain product boundaries. |
| [5](https://github.com/EukkMaru/chart-parsing/issues/5) playfield proportions | closed | not a binary closure | fitted and owner-accepted baseline | Shared camera/viewport and final resource geometry remain open. |
| [6](https://github.com/EukkMaru/chart-parsing/issues/6) AHX missing | open | answered | fixed | None for recognition; AirHold final pixels/resources remain open. |
| [7](https://github.com/EukkMaru/chart-parsing/issues/7) sustain endpoint scroll | open | answered | endpoint keys and DCM scope implemented | Common camera/clipping and family-specific final resource pixels remain open. |
| [8](https://github.com/EukkMaru/chart-parsing/issues/8) ALD normalization | open | answered | fixed for bounded widths, integer-tenth verticals, sequential first-compatible chaining, endpoint keys, and fixed transform | External type-9 resources/result-state integration remain open. |
| [9](https://github.com/EukkMaru/chart-parsing/issues/9) Slide chains/width | open | answered | source-order first-compatible chains, six-field legacy width inheritance, post-shrink endpoint width, and HLD-to-Heaven routing fixed | Phase/result simulation and external materials/final pixels remain. |
| [10](https://github.com/EukkMaru/chart-parsing/issues/10) timing/meter | open | answered | fake BPM seed removed; zero behavior, meter steps, and snapshot three-way introsort implemented | Hostile numeric failure parity and seek-equivalent playback state remain broader parser/product work. |
| [11](https://github.com/EukkMaru/chart-parsing/issues/11) parser robustness | open | answered for listed blind spots | exact registry, prefix numbers, legacy Slide width fallback, and orphan suppression pass the full corpus browser audit | JS exception/range behavior and every hostile numeric domain are not yet a complete clean-room parser replacement. |
| [12](https://github.com/EukkMaru/chart-parsing/issues/12) verification ledger | open | all listed questions answered | canonical inputs and type-13 authored mesh implemented; asset substitutes labeled | External resources, replacement-glyph shape/angle, runtime phase integration, and owner pixels remain open rows. |
| [13](https://github.com/EukkMaru/chart-parsing/issues/13) meter-grid alignment | closed | answered, including the later zero-component question | record-anchored bar/beat generation and terminal zero anchor implemented | A continuously visible cabinet field/grid must come from a static/external layer; the recovered generated vectors stop. |
| [14](https://github.com/EukkMaru/chart-parsing/issues/14) World's End acceptance gauntlet | open | not one binary claim; its reported divergences were routed to owning traces | full parser audit passes; group-1 builder is now independently differential-tested; AirHold replacement root restored | End-to-end milestone and visual acceptance remain owner work. |
| [15](https://github.com/EukkMaru/chart-parsing/issues/15) bounded decomp work order | open | all listed static questions answered | AirHold fields/resources, zero-MET vector, Heaven NON selector absence, Slide center color, and per-endpoint DCM adopted | External resource contents and human visual review only. |
| [17](https://github.com/EukkMaru/chart-parsing/issues/17) group-1 builder differential | open | acceptance is harness/product coverage, not a new binary rule | actual viewer parser/builder matches an independent reference over 595 group-1 charts/19,890 records; all three deliberate mutations fail comparison | Node-only standalone verifier could not run locally; the browser-hosted equivalent passed. |
| [18](https://github.com/EukkMaru/chart-parsing/issues/18) viewer provenance backlog | open | all 42 suggested sources checked; stale premises corrected | every live rule is labeled binary, product, or mixed; ledger: `research/VIEWER_PROVENANCE_AUDIT.md` | No unresolved provenance item remains in the issue's listed scope. |

## Handoff answers

### DCM query scope

Materialization first applies the root or endpoint's keyed STP/SFL/SLP
schedule. For a positive adjusted delta it queries DCM at:

```text
(manager_position + adjusted_delta) * 16.666666F
```

Endpoint retry recomputes this from the endpoint schedule and endpoint key.
The common active-presentation helper uses the same keyed-adjusted entity time
before adding the projection base offset. `FUN_011c58f0` then shifts the query
by `+1.0F`, scans DCM records in source order, stops at the first future start,
and returns the first nonzero covering factor, else 1. There is no manager-only,
root-only, or interval-relative anchor on these paths.

The older `2442_03` note reporting a later Slide affected after the DCM interval
does not match this binary path and predates other viewer corrections. It is
retained only as a stale/unreproduced observation; it does not justify a second
query rule. Sustained Hold, Slide, AirHold, AirSlide, AirLadder, AirSolid, and
HeavenHold updates call that helper independently for every authored endpoint;
interpolation/clipping happens only after those endpoint projections. Evidence:
`claim.timing.projection-schedule-materialization` and
`claim.presentation.sustain-endpoint-dcm-projection`.

### Issue 12 verification requests

1. **Endpoint SLA consumers for types 1, 10, and 13:** verified. HOLD,
   AirSolid, and HeavenHold presentation each project stored endpoints with
   their independently selected endpoint key. Types 2 and 9 were already
   verified. Evidence:
   `claim.presentation.sustain-endpoint-sla-selection`.
2. **Type-5/8 vertical transform:** verified and the former universal value-5
   anchor is rejected. The common origin is `(value-1)*3.8934999`; AirSlide
   action resources add `0.14999962`. Ordinary AirHold retains value 1 and
   therefore origin zero unless a type-13 root supplies a final value.
   External resource extent around the origin is unavailable. Evidence:
   `claim.presentation.common-air-transform`.
3. **AIR direction lean:** the binary verifies the mirror pairing, not a
   geometric angle. AUL/code 2 and ADR/code 4 negate lateral resource scale;
   AUR/1 and ADL/5 retain positive scale. The viewer's 30-degree replacement
   glyph remains labeled product choice.
4. **Slide endpoint width:** verified as the ending/post-shrink width.
   Generated `+0x14` is decoded by the lazy allocator; preceding width is at
   `+0x10`.
5. **MET fields:** verified as unit then count. The producer computes
   `beat=384/unit` and `bar=count*384/unit`; `4 3` yields 96 and 288 ticks.

### Complete descriptor registry

All 91 case-sensitive names and IDs are now enumerated in `spec/c2s.md` and
`c2s_command_descriptor_names`. IDs `0x2e..0x5a` are the already recovered 45
group-3 statistics. The viewer no longer suppresses arbitrary `T_*` names by
prefix. Only exact registered group-3 names and the exact 24 rejected legacy
metadata names take their proven inert/discarded paths.

### Failed secondary association

AIR-family commands do not create standalone parsed notes.

- AIR/AUR/AUL/ADW/ADR/ADL require an earlier unused root with matching
  requested type and current-endpoint position, lane, and width.
- Initial AHD/AHX and ASD/ASC attachment supports root parsed types
  0, 1, 2, 4, 6, and 11 with the same current-endpoint geometry match and
  unused slot.
- AHD/AHX continuation requires a saved type-5 endpoint match.
- ASD/ASC continuation additionally requires the prior control's ASD marker to
  match the referenced ASD/ASC family.

A failed search follows the parser diagnostic path and appends no record. The
viewer now processes these associations in source order, reserves one root
secondary slot, reports rejected associations, and does not draw orphans.
For the six-data-field legacy Slide form, control construction inherits the
root width; seven or more data fields overwrite it with field 7. This exact
compatibility branch is required before valid legacy AIR/AHD endpoint matches
can be evaluated.

### Corpus/browser verification

`python3 scripts/harness.py viewer-audit` ran the actual Firefox-hosted viewer
parser across all 7,752 local charts. It reported zero parse errors, unknown
records, rejected associations, or harness errors. A separate 20-chart
coverage-spread headless render batch completed successfully with populated,
materially different outputs and empty unknown/rejection panels; focused
legacy and modern AIR-family renders were inspected after the compatibility
fix. The refreshed audit also covered all 595 charts containing any group-1
record and all 19,890 such records: actual viewer keyed schedules and
source-order DCM lists matched the independent reference with zero mismatches.
Deliberate DCM-in-key-0, accepted-SFE, and interval-CLK mutations were all
detected. These checks establish parser/builder compatibility and product-path
stability, not visual fidelity or owner acceptance.

## Product conclusion

The reviewed issue set, except deliberately excluded human-only issue 16, has
no listed static-analysis question left unanswered. That is not the project
completion boundary. Remaining work includes binary saturation outside the
issue list, external-table loader/selector/fallback provenance, viewer state
integration, hostile numeric failure parity, seek-equivalent playback, absent
resource/material content, and owner visual/gameplay acceptance.
