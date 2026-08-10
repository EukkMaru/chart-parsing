# GitHub issue and temporary-handoff verification

Verified against the exact local `game.exe` snapshot on 2026-08-07. GitHub
state is inventory metadata, not an evidence grade. “Binary answered” means a
closed producer/consumer path exists in this snapshot; “product partial” means
the current offline viewer still does not implement all of that answer.

GitHub issue 12 was rechecked on 2026-08-09. Its body is unchanged, it has no
comments, and its GitHub `updated_at` value remains 2026-08-07 08:00:13 UTC.
It is the same issue included in this twelve-issue pass, not a new thirteenth
issue or a new binary question. The active claims cited below still answer
each requested verification item against the unchanged binary hash.

There are exactly 12 repository issues. Issues 1, 4, and 5 are closed on
GitHub; issues 2, 3, and 6 through 12 are open as of this verification.

| Issue | GitHub state | Binary verdict | Viewer/product state | Remaining boundary |
| ---: | --- | --- | --- | --- |
| [1](https://github.com/EukkMaru/chart-parsing/issues/1) SLP vocabulary | closed | answered | implemented and differentially covered | None for SLP grammar/schedule; broader camera is separate. |
| [2](https://github.com/EukkMaru/chart-parsing/issues/2) profile 7 physical meaning | open | interface answered; proposed meaning unprovable | parameterized | Inclusive range synthesis and profile index 7 are exact. External range values and “upward exit present/absent” label are unavailable. |
| [3](https://github.com/EukkMaru/chart-parsing/issues/3) ALD judged checkpoints | open | answered; issue premise is stale | implemented | Result-state simulation and external checkpoint resources remain separate viewer work. |
| [4](https://github.com/EukkMaru/chart-parsing/issues/4) Slide checkpoint classes | closed | answered | implemented for ordinary Slide | HeavenHold-retyped Slide is a different presentation path. |
| [5](https://github.com/EukkMaru/chart-parsing/issues/5) playfield proportions | closed | not a binary closure | fitted and owner-accepted baseline | Shared camera/viewport and final resource geometry remain open. |
| [6](https://github.com/EukkMaru/chart-parsing/issues/6) AHX missing | open | answered | fixed | None for recognition; AirHold final pixels/resources remain open. |
| [7](https://github.com/EukkMaru/chart-parsing/issues/7) sustain endpoint scroll | open | answered | endpoint keys and DCM scope implemented | Common camera/clipping and family-specific final resource pixels remain open. |
| [8](https://github.com/EukkMaru/chart-parsing/issues/8) ALD normalization | open | answered | fixed for bounded widths, integer-tenth verticals, sequential first-compatible chaining, endpoint keys, and fixed transform | External type-9 resources/result-state integration remain open. |
| [9](https://github.com/EukkMaru/chart-parsing/issues/9) Slide chains/width | open | answered | source-order first-compatible chains, six-field legacy width inheritance, and post-shrink endpoint width fixed | HLD-style type-13/HeavenHold presentation is still not faithfully rendered. |
| [10](https://github.com/EukkMaru/chart-parsing/issues/10) timing/meter | open | answered | fake BPM seed removed; zero behavior, meter steps, and snapshot three-way introsort implemented | Hostile numeric failure parity and seek-equivalent playback state remain broader parser/product work. |
| [11](https://github.com/EukkMaru/chart-parsing/issues/11) parser robustness | open | answered for listed blind spots | exact registry, prefix numbers, legacy Slide width fallback, and orphan suppression pass the full corpus browser audit | JS exception/range behavior and every hostile numeric domain are not yet a complete clean-room parser replacement. |
| [12](https://github.com/EukkMaru/chart-parsing/issues/12) verification ledger | open | all listed questions answered | canonical inputs implemented; asset substitutes labeled | External resources, replacement-glyph shape/angle, camera, and type-13 presentation remain open rows. |

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
query rule. Evidence: `claim.timing.projection-schedule-materialization`.

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
fix. These checks establish parser compatibility and product-path stability,
not visual fidelity or owner acceptance.

## Product conclusion

The issue set is fully investigated, but it is not the project completion
boundary. Binary questions in the issue/handoff set are answered; known
remaining work is implementation or absent-resource/camera scope. The largest
canonical viewer gaps exposed by this pass are faithful type-13/HeavenHold
presentation, full JS numeric failure parity, seek-equivalent playback state,
the shared camera/viewport, and
external-resource-aware result-state presentation without copying assets.
