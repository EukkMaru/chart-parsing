# Claim: FLK combines a timed rising edge with two-bank lateral motion

- ID: `claim.note.flick-motion-judgement`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `input.logical_state`, `matching.candidates`, `matching.priority`, `judgement.types`, `judgement.windows`, `judgement.miss`, `note.flick`, `state.ownership`, `config.external`, `interactions.cross_note`, `audit.indirect_calls`
- Last reviewed: 2026-07-20

## Statement

Parsed type 6 constructs `projView::FlickNote`. In ordinary play it accepts an
initiating logical rising edge through an adjusted TAP-style timing checker,
then tracks lateral held-source centroids for the union and each of two physical
source banks. Reaching a configured travel distance before a second configured
late bound completes with a negative or positive direction; missing the edge or
motion deadline completes through distinct terminal states.

## Anchors

- `game.exe @ RAM:00b28cc0, FUN_00b28cc0, parsed type-6 factory case`
- `game.exe @ RAM:00c1fae0, FUN_00c1fae0, FlickNote construction and shared forced-direction reset`
- `game.exe @ RAM:018d98f0, FlickNote vtable`
- `game.exe @ RAM:01c32648, FlickNote RTTI type descriptor`
- `game.exe @ RAM:00c200a0, FUN_00c200a0, record loading and two-checker initialization`
- `game.exe @ RAM:00c18800, FUN_00c18800, primary adjusted timing checker`
- `game.exe @ RAM:00c19190, FUN_00c19190, secondary motion timing checker`
- `game.exe @ RAM:00c1fd80, FUN_00c1fd80, awaiting-edge candidate exposure`
- `game.exe @ RAM:00c1fdb0, FUN_00c1fdb0, FLK center-window adjustment`
- `game.exe @ RAM:00c20340, FUN_00c20340, edge, motion, timeout, forced, and result state machine`
- `game.exe @ RAM:00c2de70, FUN_00c2de70, logical rising-edge read`
- `game.exe @ RAM:00c2ded0, FUN_00c2ded0, per-bank held-source read`
- `game.exe @ RAM:00c2de10, FUN_00c2de10, logical held-lane read`
- `game.exe @ RAM:00c17e00, FUN_00c17e00, primary/secondary fine-result lookup`
- `game.exe @ RAM:00c1fee0, FUN_00c1fee0, terminal-phase table lookup`
- `game.exe @ RAM:00c20050, FUN_00c20050, active-result remap and shared-result wrapper`
- `game.exe @ RAM:00b2b690, FUN_00b2b690, candidate/update/removal ordering`

## Observations

- Factory case 6 allocates a `0x230`-byte object and calls the constructor whose
  vtable RTTI identifies `projView::FlickNote`. Shared loading supplies decoded
  width, raw and bounded start/count, and scheduled time.
- Loading builds a primary checker at object `+0xec` through `FUN_00c18800`, so
  chart-derived adjacent-record trimming applies. A second checker at `+0x10c`
  is populated by `FUN_00c19190` from a separate external configuration group
  for the note's bounded lanes. Width-indexed external fields also supply an
  observation span and required travel distance.
- While phase is 0, candidate preparation exposes the scheduled value through
  the same per-lane timing eligibility helper as TAP. The following FLK-specific
  preparation step compares primary inner/middle endpoints with a strict
  binary `0.00001F` epsilon and, when near-equal, copies the inner endpoint to the center
  endpoint. Candidate exposure precedes this persistent mutation in the pass;
  input classification follows it.
- Ordinary edge processing begins only when the timing delta is at or above the
  secondary checker's overall lower bound. In phase 0, it expires at or above
  the primary checker's overall upper bound. Before that bound, it scans covered
  lanes in ascending order for a logical rising edge and a nonzero primary fine
  acceptance code. The update does not read or compare the manager-selected
  candidate, although FLK contributed candidates during the earlier reduction.
- The first accepted edge stores its primary fine index and changes phase 0 to
  phase 1. Motion processing then runs in the same update. Phase 1 expires at or
  above the secondary checker's overall upper bound.
- The motion scan expands the bounded lane interval by
  `(configured_span - bounded_count + 1) / 2` on each side with truncation
  toward zero and clamps it to lanes 0 through 15. It computes active counts and
  average lane centers for the union of both source banks, bank 0 alone, and
  bank 1 alone, in that order. A group's first nonempty sample initializes its
  retained minimum and maximum.
- If a note touches lane 0 or the right edge and that logical boundary lane is
  held, each currently nonempty group records a boundary status. If that group
  is empty on the next sample, status 1 contributes center `-0.5` and status 2
  contributes `16.5`, then clears. An empty ordinary group retains its previous
  center.
- Every initialized group lowers its retained minimum or raises its retained
  maximum. `abs(maximum - minimum) >= external_distance` completes immediately;
  a new lower minimum selects phase 4 and a new higher maximum phase 5. Union,
  bank-0, then bank-1 order decides the first simultaneous completion.
- Phase 2 is edge timeout, phase 3 is motion timeout, and phases 4/5 are the two
  directions. A static phase table treats 0/1 as nonterminal and 2 through 5 as
  terminal. Metadata integers are 16 for phases 2/3, 14 for phase 4, and 15 for
  phase 5.
- Directional completion combines the primary start fine record's coarse tier
  with the secondary completion fine record's coarse tier. Sums below 3 are
  raised to 3 and side code becomes 2; a fixed table maps the sum to control
  code 0, 1, 2, or 4, and code 4 zeros the side code. The provisional downstream
  result remains the start coarse tier and passes through the active result
  controller before the shared result wrapper. Timeouts submit fixed anonymous
  bytes 0/1 with side code 2.
- Active forced-result mode bypasses both edge and motion. After an external
  timing point, it selects the manager's forced result byte and alternates
  phases 4/5 through a single global flag. Construction writes that flag true;
  a forced completion consumes true as phase 4 and false as phase 5, toggling it
  after each use. Static instruction search found no other readers or writers.
- Manager removal occurs after the full active-note update pass through the
  common terminal predicate and destructor path. Loading reconstructs state
  from phase 0; no ordinary reset returns a live terminal FLK to phase 0.

## Reasoning

The parser type, factory allocation, constructor RTTI, virtual load, candidate
slots, two checker instances, input helpers, retained motion fields, phase
table, result wrapper, and manager cleanup form a closed construction-to-
outcome path. Separate reads of logical rising input and per-bank held input
distinguish the initiating edge from the directional gesture.

## Alternatives and falsifiers

- Competing explanation: lateral fields drive only animation after an ordinary
  TAP judgement.
- Evidence that would disprove this claim: result submission occurring before
  the travel/timeout states, per-bank centroids not controlling phases 4/5, or
  a hidden selected-candidate equality check on the FLK edge path.

## Unknowns

- Exact checker endpoints, observation spans, travel distances, forced timing,
  and mode selections are externally loaded and remain parameters.
- Physical meanings of the two source banks and player-facing result/direction
  names are unresolved.
- The active-result bridge used by FLK is one of the twelve closed callers of
  `claim.judgement.active-tier-zeroing`; no family-specific lifetime or
  precedence path remains.
- Cross-family priority consequences are closed by
  `claim.matching.flick-candidate-asymmetry`.

## Consequences

- Ghidra mutations: none in the live project; temporary analysis only.
- Spec sections: `spec/notes/flick.md`, `spec/input.md`, `spec/matching.md`,
  `spec/judgement.md`, `spec/configuration.md`.
- Reconstruction code: FLK phase, center-window, motion, result-composition,
  metadata, and forced-direction helpers in `include/chart/reconstruction.hpp`.
- Tests: `tests/flick_motion_test.cpp`.

## Verification

The type-6 factory and RTTI were checked independently. All FlickNote vtable
targets were resolved, the manager's candidate/update/removal order was traced,
computed phase jump targets were disassembled, the result tables were read by
field, and the shared forced-direction global was searched across all program
functions. The cross-family gate asymmetry was independently audited against
the manager and shared TAP/HOLD/Slide start path. The shared checker mutation
was independently reconciled with TAP in
`claim.judgement.tap-center-window-adjustment`. Focused tests cover center adjustment, each source grouping,
boundary continuation, threshold direction, same-tick edge/motion transition,
timeouts, result composition, metadata, and forced alternation.
