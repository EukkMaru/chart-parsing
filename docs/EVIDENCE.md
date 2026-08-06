# Evidence, fidelity, and clean-room standard

## Two evidence tracks

Stage two uses the completed exact-binary reconstruction and a separate visual
product-validation track. Keep them distinct:

- **Gameplay/format evidence** answers what a C2S command means, how timing and
  generated state work, and how note logic updates. It comes from active
  claims, normative specs, clean-room reconstruction, tests, and focused static
  analysis.
- **Presentation evidence** answers how the offline viewer should visualize
  that state. It can use static rendering-code analysis, corpus invariants,
  written observations of gameplay footage, fitted measurements, and explicit
  original product choices.

A presentation observation cannot silently change a verified gameplay claim.
If it exposes a contradiction, return to the exact binary and use the stage-one
claim process.

## Stage-one claim maturity

| Level | Meaning | Permitted consequence |
|---|---|---|
| Observed | Directly located data, xref, call, field access, or branch | Record anchor; no semantic rename |
| Supported | Multiple compatible observations establish a likely role | Careful rename/comment; retain uncertainties |
| Reconstructed | All relevant paths form a clean-room algorithm | Add specification and provisional C++ |
| Verified | Independent evidence or a focused test confirms the rule | Mark coverage verified |

Confidence (`low`, `medium`, `high`) remains separate from maturity. Active
claims use `research/templates/CLAIM.md` and retain stable binary anchors,
reasoning, alternatives, unknowns, consequences, and verification.

## Product evidence labels

Every nontrivial viewer rule or constant should use one of these labels in code
comments, issues, or handoffs:

- **Recovered** — directly follows an active claim/spec/reconstruction rule.
- **Corpus-supported** — derived from aggregate/local chart structure without
  copying chart content; establishes compatibility, not game semantics alone.
- **Observed** — repeatable written observation from matched gameplay footage;
  useful for discovering and falsifying visual behavior, but insufficient by
  itself to define a canonical renderer rule.
- **Fitted** — numeric/visual value calibrated to observations because the
  original resource value is absent. Must stay configurable and carry the
  sample set/tolerance used.
- **Product choice** — intentionally original UI, color, accessibility,
  geometry, or interaction. Never imply it was recovered from the game.
- **Hypothesis** — plausible lead not yet strong enough to drive normative
  behavior; keep behind diagnostics or an explicit experimental option.

When labels mix, state which portion is recovered and which is fitted. For
example, the projection equation may be recovered while the viewport scale is
an original product choice.

## Visual comparison standard

A useful comparison matches chart, chart position/time, tempo region, playback
speed, viewer controls, and relevant options. Record observations in prose and
prefer measurements such as event counts, lane bounds, relative timing,
continuity, and direction over subjective “looks right.”

One chart or one player report is a lead. Promote fitted behavior only after it
survives multiple representative charts or after static evidence closes it.
Judgement outcomes, input semantics, and hidden state cannot be verified from
rendered footage alone.

For this project, “binary-backed” does not mean an unexplained decompiler
expression was copied into code. A canonical rule needs a closed producer-to-
consumer path, compatible field widths/ownership, relevant callers and
callees, state/reset lifetime, and competing explanations addressed. The claim
may contain human reasoning, but the implemented premise may not come solely
from convention, corpus correlation, footage, or the existing viewer.

If the executable only selects an external value, the binary-backed claim ends
at that value's loader/source identity, selection key/index, fallback, units
supported by consumers, and effect. A fitted numerical substitute is not
canonical binary evidence. It must be labeled, configurable, included in the
human review matrix, and listed
as residual unless the owner explicitly accepts it for the asset-independent
product.

An unfamiliar corpus keyword is evidence of an unresolved parser question,
not evidence of a typo. Close it to the exact command descriptor/registration,
handler and consumers, compatibility alias, or an exact proven ignore/drop
path. Even a no-op disposition needs binary evidence.

For render provenance, the required explanation has this form: the exact
record/configuration selects a traced constructor or resource-independent
primitive path; that path uses the documented fields and transforms; its update
and lifetime state produce the visible result. “It resembles the footage” can
falsify or validate the implementation after this trace, but cannot replace it.

Do not commit copyrighted screenshots, clips, audio, or extracted frames.
External/local footage may be referenced by a reproducible title/timecode when
appropriate, but repository evidence should remain original written analysis.

## Stable reverse-engineering references

Absolute addresses are valid only for the exact binary. Pair an address with
the program, current function name, call/data relationship, and function hash
when available:

```text
game.exe @ RAM:00abcdef, CurrentFunctionName, called by X, hash <value>
```

Never use a renamed symbol alone as evidence; the name is an interpretation.

## Clean-room and asset boundary

Repository artifacts may contain concise facts, original prose, reconstructed
equations, human-written implementations, synthetic C2S fixtures, original
render primitives, and aggregate corpus statistics. They must not contain:

- raw assembly sequences, decompiler dumps, or mechanically translated code;
- binary blobs, raw game tables, copied real charts, or large record excerpts;
- game textures, models, artwork, logos, fonts, audio, video, shader/effect
  resources, screenshots, or extracted presentation data;
- chart or footage data embedded in source as a shortcut for a parser/renderer;
- claims that fitted values or original styling are exact binary facts.

Synthetic fixtures must be minimal and authored from the specification. The
local corpus can be parsed in tests, but outputs must remain aggregate and must
not make releases depend on `music/`.

## Contradictions

Never tune the viewer around a contradiction. First identify whether the model
or renderer is wrong. If exact-binary evidence changes a gameplay fact, mark
the old claim `superseded`, link the replacement, and revisit dependent specs,
Ghidra annotations, reconstruction code, tests, and viewer behavior. If only a
visual fit changes, update its evidence label and comparison record without
rewriting verified gameplay history.

## Human acceptance

The owner is the final authority on experiential/visual acceptance. Present a
matrix of representative note families, command variants, distances, speeds,
tempo regions, interactions, and every fitted external parameter. Record the
owner's accepted deviations and rejected cases. Human approval does not turn a
fit into recovered evidence; it decides whether the documented clean-room
substitute is acceptable for release.
