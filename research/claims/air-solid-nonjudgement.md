# Claim: ASO constructs a presentation-only AirSolidNote with no judgement producer

- ID: `claim.note.air-solid-nonjudgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `parser.events`, `matching.candidates`, `note.other_variants`, `audit.indirect_calls`
- Last reviewed: 2026-08-03

## Statement

`ASO` parses and chains twelve-field, two-endpoint type-10 records, constructs
RTTI-identified `projView::AirSolidNote`, and drives a source-sequence-keyed
projected resource path. It contributes no lane candidate, reads no gameplay
input, submits no result, and makes no terminal-state request on the recovered
gameplay path.

## Anchors

- `game.exe @ RAM:011c8870, FUN_011c8870, parsed type-10 ASO branch`
- `game.exe @ RAM:011cfce0, FUN_011cfce0, 16-entry color/style lookup`
- `game.exe @ RAM:011b4e80, FUN_011b4e80, continuation position tolerance`
- `game.exe @ RAM:011c6a50, FUN_011c6a50, derived ASO sample generation`
- `game.exe @ RAM:00da06c0, FUN_00da06c0, parsed-type setup dispatch`
- `game.exe @ RAM:00b264e0, FUN_00b264e0, type-10 configuration insertion by source sequence`
- `game.exe @ RAM:00b236a0, FUN_00b236a0, parsed-path to projected configuration builder`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, type-10 runtime factory case`
- `game.exe @ RAM:01c324f8, RTTI type descriptor, projView::AirSolidNote identity`
- `game.exe @ RAM:018d945c, AirSolidNote vtable, complete runtime dispatch family`
- `game.exe @ RAM:00c16fa0, FUN_00c16fa0, runtime load and sequence-keyed configuration lookup`
- `game.exe @ RAM:00c17040, FUN_00c17040, type-specific scheduled update`
- `game.exe @ RAM:00c1b170, FUN_00c1b170, all-sentinel candidate output`
- `game.exe @ RAM:00c11c90, FUN_00c11c90, shared no-op manager-adjacent virtual`
- `game.exe @ RAM:00c16f70, FUN_00c16f70, separate pure end-position comparison virtual`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, candidate collection and active-vector update/removal`
- `game.exe @ RAM:00c16550, FUN_00c16550, type-specific destruction`

## Observations

- The ASO handler consumes twelve fields after the command: integer major,
  minor, start lane, and start width; two anonymous start floats; integer
  duration, end lane, and end width; two anonymous end floats; and a final
  color/style string. Both widths clamp to 1 through 16. Mirroring independently
  replaces each endpoint lane with `16 - lane - endpoint_width`.
- The end chart position is formed from `(major, minor + duration)`. Each
  anonymous float is converted to an integer by `int(value * 10.0F + 0.5F)`.
  Duration addition and both mirror subtractions wrap at signed 32-bit width.
  Direct scalar inspection establishes 10.0 and 0.5, and the final
  `CVTTSS2SI` maps NaN, infinities, and out-of-range values to `INT32_MIN`;
  the properties' semantic names are not established.
- The final string is compared case-sensitively against `DEF`, `RED`, `ORN`,
  `YEL`, `LIM`, `GRN`, `AQA`, `CYN`, `DGR`, `BLU`, `PPL`, `VLT`, `PNK`, `GRY`,
  `BLK`, and `NON`, yielding codes 0 through 15. Empty or unknown strings yield
  code 0.
- A compatible earlier type-10 record must have a nonempty endpoint vector,
  the same color/style code, a last endpoint lane and width equal to the new
  start, both last endpoint integer properties equal to the new start
  properties, and a last-position scalar within inclusive `1/192` of the new
  start. The first such record is extended; otherwise a new type-10 record is
  appended. Extension regenerates derived samples and replaces the stored end
  position.
- The ordinary parser stores accepted source-sequence order at parsed offset
  `+0x84`. During scene setup, type 10 builds a `0x78`-byte projected
  configuration from the parsed control points and inserts it into an ordered
  map under that sequence key. Runtime loading copies the key, performs the
  ordered lookup, and copies two three-field blocks plus an end-position scalar
  into AirSolidNote-owned state.
- The type-10 factory allocates `0x114` bytes and installs the vtable whose RTTI
  descriptor names `projView::AirSolidNote`. Its scheduled callback reaches
  the type-specific virtual at `+0x2c`, which only derives projected positions
  and updates resource state from the sequence-keyed configuration.
- Its candidate virtual fills all 16 lane outputs with the negative sentinel.
  The adjacent virtual invoked by the gameplay manager is the shared no-op at
  vtable offset `+0x34`; the manager discards its return before lane reduction.
  A separate AirSolid override at `+0x38` only compares the stored end-position
  scalar with manager current position. No gameplay-manager or scheduled-task
  call to that comparison was recovered.
- The remaining type-specific virtuals are no-ops or manipulate the same
  projected resource/configuration state. None queries logical or physical
  input, writes a result category, invokes the inherited shared result handler,
  or requests base terminal state. Destruction frees the owned point vector;
  scene teardown remains the closed deletion path. No ASO-specific terminal
  request was recovered.

## Reasoning

The handler establishes field order, quantization, matching, and chaining
without relying on corpus examples. The factory, RTTI, complete vtable, task
callback table, manager candidate loop, shared result path, and destructor
close the runtime boundary. The only ASO-specific state consumers are the
projected configuration builder and resource-update methods. Candidate absence
and the lack of any input/result/terminal producer distinguish ASO from a
judged note despite its presence in the active runtime-note vector.

## Alternatives and falsifiers

- Competing explanation: AirSolidNote is judged through an inherited generic
  callback that bypasses its type-specific update.
- Evidence that would disprove this claim: an AirSolid-reachable callback that
  queries gameplay input, writes non-sentinel lane candidates, invokes the
  shared result handler, or requests terminal state; or another observed
  consumer that uses the end-comparison return to alter judgement or lifetime.

## Unknowns

- Player-facing meanings and valid authored domains for the four integer-tenth
  properties are unknown. They remain structural fields.
- The local chart corpus contains no ASO records, so compatibility is supported
  by exact-binary control/data flow and synthetic focused tests rather than a
  corpus instance.
- Presentation resource identities, geometry semantics, and the reason the
  end predicate is retained despite its discarded observed return are outside
  the gameplay mission.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP was unavailable and
  analysis used the temporary static project clone only.
- Spec sections: `spec/c2s.md`, `spec/notes/air_solid.md`.
- Reconstruction code: `C2sAsoEndpoint`, `C2sAsoSegment`,
  `parse_c2s_aso_record`, color/style lookup, property quantization, and
  `c2s_aso_segments_connect` in `include/chart/reconstruction.hpp`.
- Tests: `tests/aso_parser_test.cpp`.

## Verification

The initialized 10.0/0.5 scalars and all 16 style strings were checked
independently of decompiler type recovery. The parser branch, continuation
comparison, source-sequence producer, setup switch, configuration insertion
and lookup, factory/RTTI/vtable, scheduled callback, candidate manager,
no-op manager slot, uncalled end-comparison slot, inherited result handler,
active-vector deletion predicate, and destructor were followed separately.
Focused tests cover field order, tenths conversion including integer-indefinite
nonfinite/range behavior, width clamps, wrapped mirror/duration arithmetic,
missing fields, prefix conversion, malformed conversion, exact style lookup,
and continuation
mismatches.

## Revision note

An independent vtable-slot audit on 2026-07-21 corrected one lifecycle detail:
the gameplay manager invokes AirSolid's shared no-op at `+0x34`, not the pure
end-position comparison at `+0x38`. The earlier wording conflated adjacent
slots. The end comparison has no recovered gameplay caller, so this correction
strengthens rather than weakens the no-judgement/no-terminal conclusion. The
claim statement, reconstruction, and tests are otherwise unchanged.
