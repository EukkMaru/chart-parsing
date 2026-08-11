# MNE note

## Construction, schedule, and candidate behavior

The `MNE` event family maps to parsed type 11 and constructs a runtime
`projView::MineNote`. Shared loading supplies its scheduled time, raw start and
width, and the common bounded logical-lane interval within lanes 0 through 15.
The common width and extent rules are specified in `spec/notes/tap.md` and
proven by `claim.parser.common-lane-width-encoding`. Mine
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

## Root and terminal presentation

Mine owns one width-indexed root model. Clamp `decoded_width - 1` to `[0, 15]`
and subtract from 145, yielding executable resource rows 145 through 130. The
rows select external model records. Position and scale use the shared root
rules: lateral center `4 * start_lane + 2 * decoded_width - 32`, initial depth
`-10000`, lateral scale `decoded_width / external_native_width` with a `1.0`
fallback below one, and executable depth-axis scale `1.0` or `1.3`.

Load explicitly makes the model visible. During phase 0, each presentation
update forces it visible and replaces depth with the shared scheduled-time
projection including base offset and positive-delta DCM. Phases 1 and 2 skip
that update without issuing a hide, so the last root state persists until the
maintenance/reset path explicitly hides it or ownership is destroyed.

After result remapping, only result byte zero emits the Mine success effect.
For an ordinary result, its lateral source coordinate is the arithmetic mean
of currently held covered lane indices when that set is nonempty; otherwise it
is `start_lane + decoded_width / 2`. Forced success always uses that span
center. The coordinate becomes `(coordinate - 8) * 4`, and depth is exactly
the common judgment plane `-65`.

The success constructor receives the executable-owned selector
`max(clamp(decoded_width, 0, 16) - 1, 0)` and the chart start lane. A registered
startup initializer constructs the exact 17-entry table
`[0,0,1,2,...,15]`; this value is not external configuration. The constructor
conditionally emits shared effect kinds 8 and 9 only when their external
handles exist; the primary constructor receives lifetime input 100. The effect
assets remain external. A nonzero remapped result follows the
fixed shared-feedback `(4, 0)` path instead; zero selects the external cue
object with fixed selector 7 and parameter 0. These numeric protocols are
binary facts; no asset or audio semantics are inferred.

Preload step zero stages all sixteen root rows, step one stages feedback group
17, and step two prewarms the success-effect constructor for every width.
Readiness is set after step 28. Reconstruction:
`mine_model_resource_row`, `mine_root_model_update_enabled`, and the
`mine_success_effect_*` helpers; tests: `tests/mine_presentation_test.cpp`;
evidence: `claim.presentation.mine-root-success-effect`.

Mine's shared-feedback `+0x48` wrapper separately applies the same active
result-control remap to its incoming result byte before the common one-position
wrapper. That second application is idempotent after the ordinary producer-side
remap and also covers the forced-result route. It leaves the feedback position
unchanged; the held-average/span-center calculation above is exclusive to the
zero-result success-effect constructor.

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
