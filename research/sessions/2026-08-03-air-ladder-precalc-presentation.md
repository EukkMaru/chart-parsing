# Session 2026-08-03: AirLadder precompute and presentation split

- Investigator: codex-root
- Ghidra writer: codex-root
- Coverage rows claimed: `parser.events`, `judgement.types`,
  `state.ownership`, `audit.closure`; viewer row `render.air_ladder`
- Binary hash checked: yes
- MCP health checked: yes

## Goal

Close the exact owner/key, terminal schedule, authored-versus-generated
presentation split, and asset-independent type-9 geometry reached after the
first generated-checkpoint correction.

## Findings

- `claim.note.air-ladder-precalc-presentation` is the normative behavior claim.
- `claim.audit.primary-ald-precalc-correction` records the dependent correction
  without asserting a fresh independent closure verdict.
- Parsed `+0x84` is the accepted-event identity used by the checked type-9
  precompute lookup; it is not external configuration.
- The precompute walks authored controls, owns the final authored schedule and
  main path; generated samples independently own profile-7 checkers and
  checkpoint-effect records.
- Terminal inclusivity/unordered-float behavior, main clipping, all three
  neutral vertex streams, primitive-counter category order, generated-effect
  transform, and exact result-table-index expiry predicate are reconstructed.
  Authored roots and endpoints also own independently selected SLA projection
  keys, and the final vertex float comes from an embedded style table.
- The three streams belong to consecutively registered Joint graph children.
  Their callbacks run in order 0, 1, 2, resolve handles from checked
  `TextureTableRecord` rows, and reach the default dynamic-primitive command
  vector after compatible adjacent batching. External resource roles/material
  contents and final pixel compositing remain unavailable; viewer result-state
  integration remains open.

## Ghidra mutations

Added supported-role plate comments at `00b264e0`, `00b236a0`, `00b23470`,
`00c132f0`, `00c12d80`, `00c12350`, `00c13050`, `00c03c00`, `00c12120`,
`00c11ca0`, `0108e060`, and `011c1030`, plus a supported singleton comment at
`01c959d0`. Representative comments were dry-run before each batch. Default
names and types were retained; `00b236a0`'s comment was extended. Added
supported boundary comments at `00bfec90`, `00c033a0`, `00d7e550`, `00d7e350`,
`0060ada0`, `0066dbc0`, `00c322b0`, `00b2d160`, `00c331b0`, and `010f8a40`;
added exact tier-mapping comments at `00c196f0` and `00c18270`; corrected
`00c03c00` so its 7/9/8 calls are diagnostic counts, not submissions.

## Validation

- `python3 scripts/harness.py doctor`: exact target/project/tools/MCP passed.
- `python3 scripts/harness.py validate`: 28-row ledger and required structure
  passed after dependent correction.
- Normal build and CTest passed 35/35.
- Focused tests cover terminal edge/NaN behavior, authored path construction,
  clipping, stream geometry/extents/winding, and checkpoint resource-independent
  transform.
- Fifteen charts spread across the 1,627 corpus files containing `ALD`,
  including positive-interval `NON` and ALD-plus-SLA cases, loaded and rendered
  with zero headless failures. Four different outputs were visually inspected.
  No corpus content or screenshots were written to the repository.

## Unresolved and contradictions

- The prior primary ALD correction is superseded: its generated-checker
  cardinality remains correct, but its external terminal-threshold and
  generated-main-path premises do not.
- `render.air_ladder` remains partial for viewer result-state integration.
  External resource semantic roles, material behavior, and final pixel
  compositing are now an explicit unavailable-data boundary rather than an
  untraced executable path. The formerly auxiliary authored float is closed as
  the endpoint SLA projection key. Original canvas colors/camera/glyphs are not
  canonical claims.
- The viewer now represents each generated checkpoint with the exact unresolved
  `0xff` state and applies the exact visibility predicate for that state. It no
  longer fades unresolved checkpoints after the judgement line. It deliberately
  refuses to invent a resolved state without the external result-table count;
  the global Autoplay clip remains labeled product preview behavior.
- The former description of diagnostic categories 7, 9, 8 as submission/layer
  order was corrected after rechecking `FUN_00c32330 -> FUN_00c322b0`; that path
  only counts vertex triples. The actual Joint graph and command submission
  order is now closed separately; external materials determine final pixels.
- `audit.closure` remains open until a fresh independent investigator attempts
  to contradict the corrected stage-one workspace.

## Handoff

Connect the closed checkpoint result-index predicate to a binary-backed local
input/judgement state rather than another guessed time cutoff. The type-9
resource path is closed through TextureTable selection, Joint traversal, and
default command append; do not infer asset roles or final pixel overlap from
that order. Then resume the broader presentation manager/cross-family
inventory. Leave `audit.closure` mapped and `render.air_ladder` partial.
