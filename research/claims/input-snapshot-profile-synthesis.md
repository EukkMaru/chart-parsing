# Claim: snapshot synthesis closes all gameplay input fields and consumers

- ID: `claim.input.snapshot-profile-synthesis`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `input.logical_state`, `input.buffering`, `note.air`, `note.other_variants`, `state.ownership`, `config.external`, `audit.indirect_calls`
- Last reviewed: 2026-07-26

## Statement

The input manager deterministically derives the complete gameplay-consumed
`0x58`-byte snapshot: physical and logical levels/edges, sustain marker,
conditioned and residual photo masks, bounded scalar motion, eight profile
bytes, and the AIR sampling marker. It retains at most 301 chronological
snapshots, and the closed accessor reference set contains no additional
gameplay input representation.

## Anchors

- `game.exe @ RAM:00c2dfa0, FUN_00c2dfa0, complete synthesis and eviction path, hash 5bc6e3d9b686d09b202e6115514710ef07a2a47a3db3d1db4b92f8da66d25e87`
- `game.exe @ RAM:00c2dbf0, FUN_00c2dbf0, reset/configuration/seed path, hash a0341bd76f1b344c6b0749dfa49b9c87d93c64a9d56774ead6592e22b26cf1de`
- `game.exe @ RAM:00c2f020, FUN_00c2f020, snapshot append, hash 812ffde85489eb247b070f364532e5aa6228e17cae1f813ebabe2aa734345432`
- `game.exe @ RAM:00c2db20, FUN_00c2db20, history selector, hash 28b57244b72cff06f216f1f83e38a38e6b90d3ed45417940bacfa36ce4b45fb5`
- `game.exe @ RAM:00c2ddf0, FUN_00c2ddf0, profile-byte accessor, hash fab620190ee2187bf629cafdd4577c5359ecf9014cfd85ebdbc98e00eb8e5921`
- `game.exe @ RAM:00c2de10 and RAM:00c2de70, logical level/edge accessors, hashes b858cd08cd021d2d83c21cd4bc76432b071816c04d0e2b78108758fce26f5680 / 9255366ab836b30008992dc57839d9ad6311898d64ae2a3f8859e9c507580227`
- `game.exe @ RAM:00c2ded0 and RAM:00c2df50, physical level/marker accessors, hashes 963ad056251e795f6670c957587f250b19b323bdf2a3f95a5b2ad996de464954 / 79f7b60d80a4b0cf1091f351d87bde3fb1d7da8a1337b6e8445a45593bbbb995`
- `game.exe @ RAM:00c2f200 and RAM:00c2f250, AIR and sustain marker writers, hashes 9333b9ca52fd8f590b2e712f183e539b8c89d3c496457b63ca0c797a563341a5 / 464c0a2a240df6241952b2158a667caf668045e005062eee626fcd1be68e32a0`

## Observations

- Reset appends one zero snapshot whose scalar position is 65. Each later
  build copies the 40-byte source record and derives physical current at
  `+0x28`, physical rising at `+0x2c`, logical current at `+0x30`, and logical
  rising at `+0x34`. The two physical banks are folded only after per-source
  edge detection.
- The 32-source sustain marker occupies `+0x38`. Snapshot initialization
  clears it; HOLD, Slide, and HeavenHold can set bits in the newest snapshot.
- For each of six photo inputs, the builder counts the conditioned bit at
  `+0x3c` over the newest `min(configured window, available history)`
  snapshots. A count below the configured requirement clears the current
  conditioned bit. A sufficient count sets it unless the previous snapshot's
  AIR marker at `+0x54` is set; an already-set bit remains set through that
  marker case. The residual mask at `+0x40` is current photo input AND NOT the
  resulting conditioned mask.
- A nonempty residual mask maps to a scalar position. The two outer two-sensor
  patterns extrapolate to 75/95 and 185/205; every other pattern uses
  `90 + floor(20 * mean(asserted sensor index))`. With no residual input, the
  new position is 65 when the previous position is below 140 and 215
  otherwise.
- The stored position delta is new minus previous, except values outside the
  inclusive `[-50, 50]` interval become zero.
- Profile 7 is true exactly when the position lies within one externally
  configured inclusive range. Profiles 0 through 6 independently accumulate
  absolute, positive, and negative-magnitude delta over the current snapshot
  and up to their configured count of newest deltas. Each becomes true when
  any corresponding external threshold is met. Profile 7 does not use its
  stored threshold triplet.
- Before append, a history count greater than 300 loses its oldest entry.
  Append then restores a steady-state count of 301. Selector 0 returns the
  newest entry.
- Complete thunk xrefs close the readers: profile bytes feed AIR and
  AirHold/AirSlide/AirLadder paths plus one presentation consumer; logical
  current feeds MNE and FLK plus key-beam presentation; logical rising feeds
  the shared start gate and FLK; physical current feeds HOLD, Slide, FLK, and
  HeavenHold; sustain markers feed HOLD, Slide, and HeavenHold. No gameplay
  accessor reads the conditioned/residual masks or scalar fields directly;
  their gameplay effect is through profile bytes.

## Reasoning

The reset path establishes a valid previous snapshot before the synthesis
routine can run. Field writes, exact getter offsets, and the complete getter
xref sets connect each reconstructed field to its consumers. The synthesis
routine's only data-dependent loops are bounded by 32 touch sources, six photo
sources, eight profile records, and the owned snapshot history, so the
algorithm closes without an unresolved indirect dispatch.

## Alternatives and falsifiers

- Competing explanation: profile 7 also uses its configured motion thresholds,
  or the AIR marker directly forces the current profile byte.
- Evidence that would disprove this claim: a reference to a snapshot field
  through a non-enumerated accessor, a writer that bypasses synthesis/marker
  helpers, motion-threshold evaluation for profile 7, or history growth beyond
  the append/eviction invariant.

## Unknowns

- Conditioning counts, seven active motion-window/threshold triplets, the
  profile-7 position range, defaults, units, and player-facing meanings are
  externally loaded. Their selection locations and exact consumers are
  recovered, but their runtime values remain parameters.
- The owner stores an eighth motion-window/threshold record and a maximum
  window field; neither affects the recovered profile-7 synthesis branch.

## Consequences

- Ghidra mutations: synthesis/configuration comments at `FUN_00c2dfa0` and
  `FUN_00c2dbf0`, plus source/consumer boundary comments.
- Spec sections: `spec/input.md`, `spec/configuration.md`, and AIR-family note
  sections that consume profiles.
- Reconstruction code: `InputProfileSynthesisConfig`,
  `InputSynthesisSnapshot`, `residual_photo_position`,
  `synthesize_input_snapshot`, and bounded append helpers in
  `include/chart/reconstruction.hpp`.
- Tests: `tests/input_profile_test.cpp`.

## Verification

The profile getter, logical-level getter, logical-edge getter, physical-source
getter, sustain-marker getter, and both marker-writer thunk xref sets were
enumerated to their non-thunk callers. The resource-load/reset path was checked
independently from synthesis. Focused tests cover conditioning, the AIR-marker
exception, all scalar-position branches, delta clamping, motion-window
off-by-one behavior, profile-7 isolation, two-bank folding, and steady-state
history capacity.
