# Claim: HeavenHold combines a TAP-style start with a generated hold path, while exceptional ALD has no generated queue

- ID: `claim.note.heaven-hold-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `judgement.types`, `note.other_variants`, `state.ownership`, `config.external`, `audit.indirect_calls`
- Last reviewed: 2026-08-03

## Statement

HHD/HHX parsed type 13, HLD-styled Slide chains postprocessed to type 13, and
ALD's selector-zero/style-`NON` exception construct the same RTTI-identified
`projView::HeavenHoldNote`. It resolves a candidate-exposing TAP-style start
and a separate inactive-gap path. Type 13 receives generated sample/final
records, but exceptional parsed type-9 ALD deliberately receives an empty
generated queue. Only both component phases reaching 4 makes the note terminal.

## Anchors

- `game.exe @ RAM:00528b60, FUN_00528b60, HHD/HHX command IDs 0x2c/0x2d and type-13 descriptors`
- `game.exe @ RAM:011c8870, FUN_011c8870, type-13 parse and chain branch`
- `game.exe @ RAM:011c4af0, FUN_011c4af0, HLD-styled Slide retyping branch`
- `game.exe @ RAM:00da3710, FUN_00da3710, parsed-record vector initialization`
- `game.exe @ RAM:011bda60, FUN_011bda60, primary generated-record postprocessor`
- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, type-9 exception and type-13 factory cases`
- `game.exe @ RAM:018d93e8, HeavenHoldNote gameplay vtable`
- `game.exe @ RAM:01c324c8, RTTI type descriptor, projView::HeavenHoldNote identity`
- `game.exe @ RAM:00c15cd0, FUN_00c15cd0, load and primary-vector copy`
- `game.exe @ RAM:00c18800, FUN_00c18800, start-checker profile construction`
- `game.exe @ RAM:00c147e0, FUN_00c147e0, start resolution and source 0/1 submission`
- `game.exe @ RAM:00c13f90, FUN_00c13f90, sustain input, gap, checkpoint, and path phases`
- `game.exe @ RAM:00c13e30, FUN_00c13e30, candidate preparation`
- `game.exe @ RAM:00c158f0, FUN_00c158f0, concrete terminal predicate`
- `game.exe @ RAM:00c1b800, FUN_00c1b800, indexed end-feedback boundary`

## Observations

- HHD and HHX have command IDs `0x2c` and `0x2d` and enter parsed type 13.
  HHD stores discriminator zero; HHX stores one and parses an additional
  table-selected field. Continuation requires a compatible type-13 chain,
  discriminator, saved endpoint fields, and command-specific metadata.
- Slide field 8 maps exact `HLD` to style code 1. Before generated-record
  construction, the ordinary postparser changes matching completed type-2
  chains to type 13 and normalizes their root/control discriminator slots to
  code 10. The SLD/SLC versus SXD/SXC command-form flag remains clear/set.
- The common parsed-record constructor clears both `+0x158` and `+0x168`
  vectors. The chart postprocessor populates primary vector `+0x158` for root
  types 1, 2, and 13. Parsed type 9 bypasses every primary-vector producer.
- The type-13 producer samples a single root-to-final-end span on the shared
  384-tick grid. Cadence starts one adaptive step after the root and is updated
  from every generated sample. Ordinary samples have kind 0 and default
  enabled; enabled key-zero selector-zero open intervals can clear them. The
  final endpoint has kind 1 and remains enabled.
- The type-13 duration accessor returns a signed integer, the parser adds it to
  the start position without a range check, and grid conversion uses
  `CVTTSS2SI`. Tick subtraction/addition wraps at 32 bits before the producer
  compares offset and delta as unsigned. A delta with its high bit set expands
  toward a very large sampling range regardless of signed endpoint ordering.
  Depending on adaptive cursor progress, source execution performs
  pathological vector growth, reaches the vector length failure, or does not
  reach the unsigned bound; a zero adaptive step is a separate nonprogress
  domain. The clean-room evaluators report these cases without running unsafe
  loops.
- The factory always sends type 13 to the `0x348`-byte HeavenHold class. Type 9
  reaches the same constructor only for sampling interval zero and style code 15
  (`NON`). Load copies parsed primary vector `+0x158` to runtime `+0x168`.
- Legacy ALD records that omit the style token receive the accessor's empty
  string. Style lookup returns code 0 for an empty string, so those records do
  not satisfy the HeavenHold exception.
- Candidate preparation first writes all negative sentinels, then exposes the
  embedded start checker's candidates while start phase `+0xec` is not 4.
  The center-window adjustment is the shared lane-checker rule.
- HHD, exceptional ALD, and rewritten SLD/SLC select start profile 0 or 1 from
  the clamped runtime input variant. HHX and rewritten SXD/SXC select profile 2
  or 3. External timing records supply the actual windows and corrections.
- Start resolution uses the common checker and submits source category 0 or 1
  from the standard variant/suppression flags, then sets start phase 4.
- For each covered lane, sustain reads two physical banks. Admission checks
  asserted banks in order and stops at the first success. On success, asserted
  banks latch; with no success, deasserted banks latch. Current activity
  requires scheduled start, asserted raw input, and the retained bank latch.
  Forced activity is ORed only into the aggregate passed to the shared
  inactive-gap tracker.
- At most one due generated front is consumed per substep. Kind 0 selects
  source category 2 and kind 1 selects 3. Ordinary mode classifies and resets
  the retained maximum before checking the emission flag, so a disabled due
  sample still changes state but submits nothing. Forced selection bypasses
  ordinary classification/reset. Categories 2 and 3 both map to shared
  category 1.
- The once-only adjusted-end branch converts the gap class to indexed feedback
  category 1, but does not call the result dispatcher or alter either phase.
  An empty queue sets path phase 4 at the first path update after its gate.
- Constructor state clears phases, latches, activity, the queue, and the end-
  feedback once flag. The concrete terminal predicate requires both start and
  path phases equal 4; the normal update then requests deferred transition.
  Destruction releases the copied queue, embedded checker state, and resources.

## Reasoning

The factory and RTTI prove that three parser origins share one concrete gameplay
class. Constructor initialization plus the complete postprocessor type split
distinguishes an intentionally empty type-9 queue from a missing producer. The
load copy, two component updates, virtual result submissions, phase writes,
candidate method, and terminal predicate then form a closed parser-to-outcome
path. The separate indexed feedback helper has no return or call path into the
authoritative result dispatcher.

## Alternatives and falsifiers

- Competing explanation: exceptional ALD receives generated records through a
  second postprocessor, or its end feedback is an implicit checkpoint.
- Evidence that would disprove this claim: another writer to parsed type-9
  `+0x158`, a result-dispatch edge from the end-feedback resource path, a
  candidate consumer after start phase 4, or a terminal path that bypasses one
  of the two phase comparisons.

## Unknowns

- Exact external timing windows, gap gates/floor/end, adjusted-end values,
  forced timing, corrections, and resource identities remain parameters.
- HHD/HHX are absent from the local corpus, so their direct authored parser
  support cannot be sampled. The local corpus does exercise the alternate
  type-13 origin with two extended-form SXD/HLD lines in one chart.
- The player-facing meanings of the HHX extra table field, the runtime input
  variant that chooses the member of each profile pair, and result resources
  remain unresolved.

## Consequences

- Ghidra mutations: none.
- Spec sections: `spec/notes/heaven_hold.md`, `spec/c2s.md`, `spec/input.md`,
  `spec/timing.md`, `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: `HeavenHold*`, `generate_heaven_hold_path_records`,
  `update_heaven_hold_lane_contact`, and the HeavenHold phase/result helpers.
- Tests: `tests/heaven_hold_judgement_test.cpp`.

## Verification

The parsed-record constructor and every postprocessor primary-vector branch
were checked independently of the factory/load path. The start and path result
calls were compared with the shared source-category map and the feedback-only
helper. The type-13 parse branch was rechecked against the generated-record
loop to prove that signed negative durations reach the unsigned comparison.
Corpus aggregation independently confirmed six current-schema zero/`NON` ALD
lines, 10,638 one-short legacy ALD lines, no HHD/HHX lines, and two SXD/HLD
lines in one chart. Focused tests cover the wrapped small-boundary delta,
high-bit large-span expansion, zero-step nonprogress, CVTTSS2SI grid behavior,
and exceptional type-9 empty-queue path in addition to normal generated-record
and judgement behavior. Full-suite verification is tracked by the current
session handoff.
