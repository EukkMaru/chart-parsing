# Claim: HOLD start judgement reuses the TAP gate

- ID: `claim.note.hold-construction-start-gate`
- State: active
- Maturity: supported
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `matching.candidates`, `matching.priority`, `judgement.miss`, `note.hold`, `state.ownership`, `audit.indirect_calls`
- Last reviewed: 2026-07-27

## Statement

Parsed record type 1 constructs a runtime `projView::HoldNote` whose unresolved
start component alone exposes lane candidates and resolves through the same
rising-edge, selected-candidate, timing-window, and late-expiration gate used by
TAP.

## Anchors

- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed-type runtime factory`
- `game.exe @ RAM:00c297f0, FUN_00c297f0, HoldNote construction`
- `game.exe @ RAM:018d9abc, HoldNote primary vtable`
- `game.exe @ RAM:01c326d0, RTTI class-name storage`
- `game.exe @ RAM:00c2adc0, FUN_00c2adc0, HOLD record load`
- `game.exe @ RAM:00c1d1d0, FUN_00c1d1d0, shared note-field load`
- `game.exe @ RAM:00c29c10, FUN_00c29c10, HOLD candidate preparation`
- `game.exe @ RAM:00c2a300, FUN_00c2a300, HOLD start update`
- `game.exe @ RAM:00c1af30, FUN_00c1af30, shared start judgement gate`
- `game.exe @ RAM:00c1cce0, FUN_00c1cce0, shared result wrapper`

## Observations

- Factory case 1 allocates `0x35c` bytes and calls the constructor at
  `RAM:00c297f0`. The installed primary vtable has RTTI naming the class
  `projView::HoldNote`.
- The shared loader copies start lane and table-decoded width, bounds the lane
  extent to the 0-through-15 domain, and scales the record start time by
  `0.06`. The HOLD loader also scales record field `+0x3c` as end time, applies
  one runtime offset to both endpoints, copies `0x20`-byte checkpoint records,
  and initializes two component-phase fields to zero.
- The HLD/HXD parser's first five integers are major, minor, lane, width, and
  duration. It canonicalizes the start from `(major, minor)` and the end from
  `(major, minor + duration)`. Duration addition and both optional mirror
  subtractions wrap at signed 32-bit width. The end triple is stored at parsed
  `+0x34`; its scheduled-millisecond member is `+0x3c`.
- HOLD loading passes the parsed `+0x34` end position together with the
  already-decoded lane and width to the SLA endpoint selector. The selected
  end tag is stored separately from the shared start tag. Thus `+0x34` is not
  an unidentified lane field.
- Candidate preparation first clears the base candidate array. It calls the
  shared lane-window candidate builder only while the start phase is not 4.
- The start update calls the same gate used by TAP. That gate checks covered
  lanes, newest logical rising input, manager-selected candidate equality,
  configured windows, and late expiration.
- The HOLD loader also reaches the shared checker initializer after chart
  postprocessing, so nearby-record per-lane endpoint trimming applies to HOLD's
  start windows under the same rules as TAP. Evidence is bounded separately by
  `claim.judgement.tap-adjacent-window-adjustment`.
- When the gate reports completion, the HOLD forwards the start result through
  its shared result-wrapper slot and sets its start phase to 4. The completion
  report covers either accepted input or the gate's expiration path.

## Reasoning

The factory and RTTI establish runtime identity independently of event naming.
The HOLD virtual slots delegate candidate preparation and start resolution to
the same helpers already closed for TAP, while the phase guard removes the
start component from later candidate reductions.

## Alternatives and falsifiers

- Competing explanation: HOLD uses a visually similar but independently
  evaluated start path.
- Evidence that would disprove this claim: a type-1 factory branch selecting a
  different runtime class, or a HOLD start call path bypassing the shared
  candidate equality/rising-edge gate.

## Unknowns

- Forced-result behavior inside the shared gate is covered by
  `claim.judgement.forced-result-mode`; its exact producer, enable source, and
  lifetime are closed. The enable's player-facing label is unassigned and the
  external timing value remains a parameter.
- Player-facing names for the forwarded start result remain unresolved.

## Consequences

- Ghidra mutations: none in the live project; temporary read-only analysis
  only.
- Spec sections: `spec/notes/hold.md`, `spec/matching.md`.
- Reconstruction code: `parse_c2s_hold_geometry`; shared TAP
  input/classification and lifecycle helpers model the reused gate components.
- Tests: existing TAP gate primitives are covered by `tests/tap_input_test.cpp`,
  `tests/tap_window_test.cpp`, and `tests/tap_lifecycle_test.cpp`;
  `tests/hold_variant_test.cpp` covers start/end geometry and mirroring.

## Verification

Factory allocation, RTTI, construction, load, candidate, start update, shared
gate, result wrapper, and phase write were followed as separate paths. The
start-phase guard was checked in both candidate and update functions. The
parser's duration-derived end triple and the loader's independent end-time and
SLA-endpoint consumers were rewalked from producer to use.
