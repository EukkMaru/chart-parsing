# Completion gates

## Stage one: exact-snapshot gameplay reconstruction — correction pending re-audit

The stage-one goal was perfect reconstruction of gameplay-affecting behavior
from `.c2s` ingestion to judgement outcome at apparent tick granularity. The
following twelve gates are preserved because historical claims and the final
independent audit cite them:

1. **Closed boundaries** — Every path from accepted chart input to gameplay
   outcome is mapped; excluded downstream code receives no value that feeds
   back.
2. **Parser completeness** — Every corpus command/version variant is
   classified, including ignored, invalid, default, and compatibility behavior.
3. **Time completeness** — Clock source, tick conversion, rounding, tempo
   changes, update order, discontinuities, and inclusivity are reconstructed.
4. **Note completeness** — Every constructible note/variant has construction,
   state transitions, input, judgement, miss, reset, and destruction rules.
5. **Input completeness** — HID-to-logical state, edges/levels, lane/width,
   buffering, matching, consumption, and priority are resolved.
6. **Interaction completeness** — Simultaneous, overlapping, compound, and
   cross-note behavior has explicit ordering and focused tests.
7. **Configuration completeness** — Every gameplay-affecting constant has a
   source, consumer, default, and selection description; unavailable values are
   explicit parameters.
8. **Control-flow saturation** — Relevant indirect calls, vtables, callbacks,
   alternate modes, errors, and reset paths are resolved or excluded.
9. **Reconstruction traceability** — Each C++ behavior links to a clean-room
   spec, claim, binary anchor, and test.
10. **Corpus compatibility** — All local chart versions are accounted for
    without silently discarding unknown gameplay commands.
11. **Contradiction audit** — Active claims agree and superseded findings have
    had all dependents reviewed.
12. **Independent closure review** — A fresh investigator attempts to find
    omitted state, calls, modes, and boundaries.

Twenty-seven stage-one coverage rows are currently verified. The ALD
authored/generated-vector, accepted-event identity, terminal schedule, and main
presentation ownership contradictions were corrected in
`claim.note.air-ladder-generated-checkpoints`,
`claim.note.air-ladder-precalc-presentation`, and
`claim.audit.primary-ald-precalc-correction`; the three affected behavioral
rows are verified again by focused/full tests and aggregate corpus coverage.
The former authoritative record
`research/claims/independent-post-fix-closure-review.md` is preserved as
superseded because it missed that distinction. `audit.closure` remains open
until a fresh independent investigator repeats gate 12 against the corrected
workspace. Viewer observations alone still cannot reopen gameplay evidence,
but exact-binary contradictions can.

## Stage two: canonical offline C2S gameplay viewer — active

The current grand goal is not met until all gates below pass. The existing
viewer is a valuable baseline, not proof of fidelity: it was initially built
from repository documents without binary access and is known to diverge from
gameplay footage, especially in shapes and presentation rules.

1. **Asset-independent product boundary** — Releases contain only original
   code, UI, geometry, and presentation. No real charts, game media, footage,
   extracted resources, server code, or player records are embedded or needed.
2. **Arbitrary-file parser** — A user can load a `.c2s` file outside this
   repository. Every registered command, supported compatibility shape,
   unknown command, malformed field, and diagnostic follows the clean-room
   parser rules without relying on `music/` paths or chart IDs.
3. **Single normalized model** — Renderer and inspector consume a named,
   source-located chart model. Raw token indexing is confined to the parser;
   continuation, mirroring, defaults, derived records, and ownership are not
   duplicated in drawing code.
4. **Canonical timing and playback** — BPM/MET, projection schedules, scroll,
   materialization, generated paths, ordering, update cadence, pause, seek, and
   playback speed are deterministic and agree with the verified logic.
5. **Complete note-family presentation** — Every constructible note and command
   variant has explicit authored/generated geometry, lifetime, path continuity,
   layering, checkpoint class, and diagnostic behavior. No family falls back
   silently to a generic or guessed shape.
6. **Gameplay-state fidelity** — Deterministic playback follows the recovered
   note lifecycle. Any exposed logical input/judgement simulation agrees with
   the reconstruction and remains local/nonpersistent; scoring services and
   player records are not introduced.
7. **Binary-backed presentation closure** — Every claimed reference behavior,
   including geometry relationships, transforms, visible classes, lifetime,
   motion, layering, and feedback triggers, has a closed exact-binary producer-
   to-consumer path. The GitHub issues are seed cases, not the boundary.
   Resource-only values have closed lookup/selection/consumer logic and remain
   configurable and labeled rather than guessed as canonical.
8. **Zero-ambiguity reference suite** — A large coverage-selected set from
   `music/` covers every observed keyword/field shape, note/command variant,
   legacy/current form, tempo/projection extreme, simultaneous/compound case,
   near/mid/far position, outlier, and a holdout set. Every reviewed chart has
   zero undefined keywords, heuristic/fallback primitives, appearance-only
   constants, or unclassified footage differences. Feature saturation and the
   owner—not a fixed count—decide whether the set is sufficient.
9. **Robustness and performance** — Malformed or extreme input produces a
   diagnostic or modeled source-failure disposition, not a browser hang or
   unsafe allocation. Representative large charts remain interactive during
   load, seek, and playback.
10. **Offline privacy** — Loading and viewing a chart requires no network,
    account, upload, telemetry, or local corpus installation. The UI clearly
    communicates local-only file handling.
11. **Regression and corpus compatibility** — Synthetic parser/model/renderer
    tests, all reconstruction tests, coverage validation, and aggregate parsing
    of all 7,752 local charts pass without copying corpus content into outputs.
    Every observed corpus keyword has a binary-proven registered, compatible,
    rejected, or ignored disposition; none is dismissed or corrected as a typo.
12. **Independent binary saturation audit** — A fresh investigator starts from
    factories, presentation managers, parsed fields, vtables/callbacks, state
    ownership, and configuration selectors in both producer-to-consumer and
    consumer-to-producer directions. No relevant reachable path, indirect
    target, field, branch, reset, or external-parameter selection remains
    unexplained merely because the known issue list is empty.
13. **Owner gameplay review** — The owner reviews the finished workflow against
    real gameplay footage and gameplay experience across the documented matrix,
    tests arbitrary files, and explicitly accepts or rejects all residual
    fitted asset-independent parameters and presentation deviations. Automated
    tests and agent self-review cannot pass this gate.

## Stage-two blocking conditions

Completion is blocked by any of the following:

- a known chart family or command rendered by an unexplained fallback;
- parser/model behavior implemented only in canvas/UI code;
- hard-coded chart/song IDs or dependence on repository-local `music/` paths;
- copied charts, artwork, audio, video, fonts, or extracted game resources;
- a runtime chart upload, telemetry, account, or server dependency;
- an unlabeled guessed/fitted value presented as recovered or canonical;
- a reproducible footage discrepancy with no classification or next action;
- a reviewed corpus chart with any undefined keyword, unexplained visible
  configuration, heuristic fallback, appearance-only constant, or unclassified
  divergence;
- a gameplay/spec contradiction “fixed” only by visual tuning;
- a canonical renderer behavior justified only by convention, corpus pattern,
  footage, the existing viewer, or an unclosed decompiler fragment;
- any applicable `research/VIEWER_COVERAGE.tsv` row whose binary status is not
  `closed`, whose product status is not `verified`, or whose owner review is not
  `accepted`, plus a missing fresh saturation audit;
- inability to seek/replay deterministically or safely handle malformed input;
- missing tests for a rule that can be stated without copyrighted fixtures.

Pixel identity with proprietary assets is not a goal and cannot be required.
Canonical fidelity means behavior, timing, layout relationships, path/note
shape rules, and motion are recovered or transparently calibrated, while the
actual presentation assets and branding remain original.

## Final stage-two audit record

The final product report must list parser/command coverage, note-family/render
coverage, arbitrary-file and corpus results, known fitted parameters and their
comparison matrix, Ghidra-backed issue resolutions, asset/license inventory,
offline/network checks, build/test results, performance limits, and explicit
residual differences. Any residual that can materially change chart-gameplay
behavior or shape without a documented parameter blocks completion.

Only the owner can pass gate 13. Agents continue productive binary and product
work until all preceding gates are supported and a deliberate saturation audit
finds no further in-scope behavior to explore; they then prepare the human
review rather than declaring experiential fidelity themselves.
