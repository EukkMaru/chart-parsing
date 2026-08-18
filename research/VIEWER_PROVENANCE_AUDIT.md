# Viewer provenance audit

Last reviewed: 2026-08-18

This is the local verification ledger for GitHub issue 18. It classifies all
42 listed viewer rules after checking the suggested source against the active
claim/spec text. `Recovered` means the rule is supported by this executable;
`product` means it is an asset-free browser convention and is not presented as
cabinet behavior; `corrected` means the issue described an older implementation
or premise that is no longer true. Source comments are beside every live rule
in `scripts/c2s-viewer.html`; retired rules are cited at their replacement.

## Tables and parsing (24)

| ID | Rule | Disposition and source |
|---|---|---|
| T1 | AirLadder unresolved result byte is `0xff` | Recovered: `claim.note.air-ladder-precalc-presentation`. |
| T2 | Eight-entry Slide/Heaven feedback order | Recovered: `claim.note.slide-presentation-classes` and `claim.presentation.heaven-hold-authored-mesh`. |
| T3 | Heaven mirror/mode tables and fallback | Recovered: `claim.presentation.heaven-hold-authored-mesh`. |
| T4 | Initial Air-path root whitelist | Corrected: the initial set is types 0, 1, 2, 4, 6, and 11. Direct type 13 is not an initial candidate; a compatible type-2 HLD path is retyped later. Sources: both AirHold/AirSlide secondary claims. |
| T5 | Unknown exact enum text maps to row zero | Recovered: `claim.parser.event-token-fallback` plus each family decoder. |
| T6 | Source-order, earlier-root, single-secondary association | Recovered: `spec/matching.md` and the three Air secondary claims. |
| T7 | Slide continuation key | Recovered: `claim.note.slide-path-sustain-judgement` and `spec/notes/slide.md`. |
| T8 | Heaven continuation key | Recovered: `claim.presentation.heaven-hold-authored-mesh`. |
| T9 | Family-specific attachment endpoint/time | Recovered: the three Air secondary claims and their note specs. |
| T10 | Type-13 Air-path base vertical | Corrected with T4: the reachable HLD-retyped path writes value 1 to root/control scalars, so the common transform origin is zero. Direct HHD/HHX is not in the initial attachment whitelist. Sources: `claim.presentation.common-air-transform` and both AirHold/AirSlide presentation claims. |
| T11 | ASD/ASC continuation marker match | Recovered: `claim.note.air-slide-secondary-judgement`. |
| T12 | SLA excluded from primary root/runtime factory | Recovered: `claim.parser.sla-materialization-selection`. |
| T13 | Attached-secondary HOLD far-marker selector | Recovered selector, external appearance unavailable: `claim.presentation.hold-root-body-transform`. The asset-free glyph does not invent a visual difference between missing external rows. |
| T14 | Header reset defaults | Recovered: `claim.parser.header-default-dispatch`. BPM default state does not synthesize a BPM event. |
| T15 | Unknown-command position marker | Product/corpus heuristic. It is explicitly not parser proof and affects only the diagnostic marker layer. |
| T16 | Legacy six-field Slide inherits root width | Recovered: `claim.parser.event-token-fallback` and `spec/c2s.md`. |
| T17 | Slide endpoint unresolved byte is `0xff` | Recovered: `claim.note.slide-presentation-classes`. |
| T18 | AHX/ASD alone create authored action checkpoints | Recovered: the AirHold and AirSlide secondary claims. |
| T19 | Zero-interval `NON` ALD becomes HeavenHold | Recovered: `claim.note.heaven-hold-judgement` and `claim.presentation.heaven-hold-authored-mesh`. |
| T20 | Tempo lookup half-tick query shift | Recovered: `claim.timing.tempo-measure-schedule`. |
| T21 | MET field meanings, steps, restart, and zero stop | Recovered: `claim.timing.tempo-measure-schedule` and `claim.pipeline.meter-grid-render-boundary`. |
| T22 | SLA query adds half one fixed-resolution tick | Recovered: `claim.parser.sla-materialization-selection`. |
| T23 | AirLadder SLA float-span tolerance | Recovered exact `-0.00001/+0.00001`: `claim.parser.sla-materialization-selection`; it is not fitted. |
| T24 | Sustain endpoint owns its SLA key | Recovered for types 1, 2, 9, 10, and 13: `claim.presentation.sustain-endpoint-sla-selection`. |

## Association and schedules (5)

| ID | Rule | Disposition and source |
|---|---|---|
| A1 | Missing origin MET is synthesized from header meter | Recovered: `claim.parser.header-default-dispatch` and `claim.timing.tempo-measure-schedule`. |
| A2 | Duplicate-position MET keeps the first record after snapshot sort | Recovered: `claim.timing.tempo-measure-schedule`. The viewer uses the reconstructed snapshot sort, not browser stable sort. |
| A3 | Eight-measure minimum grid and two-measure trailing pad | Product canvas-extent choice. The binary meter claims do not define a finite browser extent. |
| A4 | Repeated active meter values omitted from UI labels | Product display choice. It does not alter the recovered meter-vector anchors. |
| A5 | Prime-factor subdivision levels and palette | Product review overlay. No executable/cabinet provenance is claimed. |

## Transforms and camera (5)

| ID | Rule | Disposition and source |
|---|---|---|
| C1 | `AIR_PLANE = 1.0` is a ceiling | Corrected/product: it is a fitted-camera normalization of 15.574 render units, not a recovered asset extent. `claim.presentation.air-hold-model-path` explicitly classifies those values as attachment coordinates. |
| C2 | Negative lift clamps to the lane surface | Product camera choice. The recovered common transform permits negative attachment coordinates. |
| C3 | One DCM factor per sustained entity | Corrected and retired. Every sustained endpoint now calls the projection helper independently; source: `claim.presentation.sustain-endpoint-dcm-projection`. |
| C4 | Guide cues stay on authored due times | Mixed: cue/audio policy is owner/product behavior; manager time and judgement classifiers are not replaced by projection schedules. Sources: `claim.timing.projection-schedule-materialization` and `claim.parser.sla-materialization-selection`. |
| C5 | Every fourth lane divider is emphasized | Product display choice; no cabinet texture/divider cadence is claimed. |

## Draw dispatch (5)

| ID | Rule | Disposition and source |
|---|---|---|
| D1 | HOLD/Slide terminal resources and cap-before-root ordering | Resource ownership is recovered from the Hold and Slide presentation claims. Browser cap-before-root ordering is a product choice because `claim.presentation.cross-family-update-composition` leaves concrete external sort keys unavailable. |
| D2 | Same 8-ms/lane cap suppression | Product deduplication choice. No binary timing bucket is claimed. |
| D3 | AirSlide root glyph height/type-13 base | Recovered scale input from `claim.presentation.air-slide-model-path` and the common transform; issue's direct-type-13 premise is corrected by T4/T10. The replacement glyph remains product art. |
| D4 | Air-family objects omitted from window overlay | Product diagnostic scope. It does not imply absence of their recovered attached judgement. |
| D5 | AirLadder stream-1 attenuation | Recovered exact piecewise expression: `claim.note.air-ladder-precalc-presentation`. |

## UI, audio, and panels (3)

| ID | Rule | Disposition and source |
|---|---|---|
| U1 | Authored-time guide scheduling | Same mixed boundary as C4; the one shared source comment covers audio and visual guide consumers. |
| U2 | Type-13 Air glyph base | Corrected by T4/T10/D3; no separate uncited implementation remains. |
| U3 | AirLadder legend says three streams and excludes `NON` | Recovered inventory: `claim.note.air-ladder-precalc-presentation` and `claim.presentation.heaven-hold-authored-mesh`. The legend duplicates only those already-recovered selectors. |

## Result

All 42 issue items now have a checked disposition. Twenty-five are direct
binary-backed rules, eight are explicitly product-only conventions, and nine
are mixed boundaries or corrected stale premises. No product convention is cited as
binary evidence, and no unresolved provenance item remains in this issue's
listed scope.
