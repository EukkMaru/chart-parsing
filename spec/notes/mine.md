# MNE note

## Construction, schedule, and candidate behavior

The `MNE` event family maps to parsed type 11 and constructs a runtime
`projView::MineNote`. Shared loading supplies its scheduled time, raw start and
width, and a bounded logical-lane interval within lanes 0 through 15. Mine
loading initializes local phase to 0 and accumulated contact to `0.0`.

MNE never exposes a runtime lane candidate. Its candidate preparation leaves
all 16 lane outputs at the negative sentinel, it has no TAP-style timing
checker, and its input update does not compare against the manager-selected
candidate. Consequently MNE neither constrains TAP/HOLD/FLK candidate reduction
nor depends on candidate ordering. Evidence:
`claim.note.mine-contact-aggregate-judgement`.

## Timing inputs

For each update, let:

- `current` be manager current position minus scheduled time, the external
  Mine base offset, and the selected runtime correction;
- `previous` be manager previous position minus the same three values;
- `start`, `end`, and `required` be external Mine contact parameters.

All are single-precision values. The external configuration values and
selected runtime correction remain parameters.

## Held-contact state machine

Phases are:

| Phase | Meaning in the clean-room model | Terminal |
| ---: | --- | --- |
| 0 | accumulating | no |
| 1 | threshold reached | yes |
| 2 | threshold unreachable | yes |

For a nonterminal note with finite inputs:

1. If `current < start`, do nothing.
2. Otherwise query the newest logical held level for every bounded covered
   lane. Physical source banks are already folded by this query.
3. If any covered lane is held, compute
   `overlap = min(current, end) - max(previous, start)`. Add `overlap` to the
   accumulator only when `overlap > 0`.
4. If `current < 0`, do not resolve a terminal phase yet.
5. Compute `remaining = max(end - current, 0)`. If
   `required > accumulated + remaining`, enter phase 2. Otherwise, if
   `accumulated >= required`, enter phase 1. Otherwise remain in phase 0.

Any positive number of covered held lanes credits the same whole clipped
substep interval. Rising edges, held-lane count, physical-bank identity, and
manager candidate values do not enter this state machine. The exact
reconstruction is `chart::reconstruction::update_mine_contact`; evidence:
`claim.note.mine-contact-aggregate-judgement`; tests:
`tests/mine_contact_test.cpp`.

## Result and lifetime

Phase 1 supplies anonymous provisional result byte 0; phase 2 supplies byte 4.
Both go through the active-result remap path with source category 17 and then
the shared result handler. Source category 17 selects dispatch category 11;
that record independently selects progress category 0 and aggregate category
8. A runtime-valid ordinary result therefore updates the authoritative
aggregate. An already-active terminal route instead sends the event to its
observer-only path.

Finalization requests base terminal state 2 while current base state 1 remains
active. The next normal note tick commits that request, after which the manager
removes and destroys the object. Unlike a completed TAP, the deferred MNE
cannot affect the intervening candidate pass because its candidate output is
always empty.

When manager forced-result state is active, MNE waits until its external forced
timing point, submits the selected forced byte through the same category path,
and enters phase 1 for byte 0 or phase 2 for any nonzero byte. It does not read
lane input in this branch. Evidence:
`claim.note.mine-contact-aggregate-judgement` and
`claim.judgement.forced-result-mode`.
