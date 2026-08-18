# Claim: Slide extended feedback has exact lane-overlap and global-cooldown admission

- ID: `claim.presentation.slide-extended-feedback-admission`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.slide`, `render.feedback_layering`,
  `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

For Slide source category 1 with a nonzero result and a valid selected
`NotesCharaEffectTableRecord`, `projView::CharaEffectManager` computes two
independent admission flags before the fixed kind-6 then kind-7 submissions.
Kind 6 uses a 32-subcell lane-overlap expiry map with exact width-dependent
footprints and one-unit durations. Kind 7 uses a single seven-unit global
cooldown. Both compare against the current NotesManager presentation time,
mutate their reservation state even when the other gate fails, and have closed
construction, reset, and destruction paths.

## Anchors

- `game.exe @ RAM:00b30660, InitializeCharaEffectManager, owner construction, hash f4587e8b21cd780585d31e55aa1a9e5a145a89cbe112c0df7c9e394ca0c2465a`
- `game.exe @ RAM:00b309d0, InitializeCharaEffectAdmissionDefaults, footprint/duration/cooldown constants and 32-cell allocation, hash afcf145b79d9cba7747bfc34098c94ebf4facf1647ee4a6f0f03e2b4f527828f`
- `game.exe @ RAM:00b30820, CheckAndReserveSlideCharaEffectLaneOverlap, lane gate and reservation, hash 30bc64250f8f3fe518575be86f9a18ede0ac30e943bb08cf65330f7249121693`
- `game.exe @ RAM:00b307d0, CheckAndReserveSlideCharaEffectGlobalCooldown, global gate and reservation, hash 3feae111b325ed6de865324e3259824914b2149f75a7a40ddf2c3e61c6bdd7a6`
- `game.exe @ RAM:00b30970, ResetCharaEffectAdmissionState, gameplay/reset clear, hash d8fb0937059db8957d4be82028cbc62d927813c0d02ddf82c65bec58963a7058`
- `game.exe @ RAM:00c1c340 -> RAM:00b1ab50, common result consumer and independently gated kind-6/kind-7 submitter, hashes 6d22a0d5b382d07b18f6280e0005284623e862a2b09397886156820a8f22c698 and 57ef1f4fb30391e1d35cd12765231f1a67a9ad85d4ab2c92f73dc26da22faf7f`
- `game.exe @ RAM:00acd1b0 / 00acd9f0 / 00ace460 / 00da27f0, singleton construction/destruction and both reset callers`

## Observations

- The RTTI-identified manager is a 0x98-byte singleton. It owns sixteen
  signed footprint entries, sixteen float durations, a seven-unit global
  cooldown duration, a 32-float expiry vector, and one next-global-admission
  float. Construction sizes the vector to 32 and zero-initializes every cell.
- Footprints for decoded widths 1 through 16 are exactly
  `{2,4,6,8,2,12,2,16,2,2,2,2,2,2,2,32}`. Every corresponding overlap
  duration is exactly `1.0`; the global duration is exactly `7.0`.
- The caller supplies runtime start lane `+0x84` and maps runtime decoded width
  `+0x80` to the established zero-based 1..16 width selector. Negative lanes
  and a lane-plus-width greater than 16 are rejected before subcell access.
- For decoded width `w`, lane `l`, and footprint `p`, the inclusive reserved
  endpoints before clamping are `2*l + w - p/2` and
  `2*l + w - 1 + p/2`. Each endpoint clamps to 0..31.
- Admission scans expiry cells from the first endpoint up to but excluding the
  last. Any cell whose expiry is strictly greater than current manager time
  rejects the request. Acceptance writes `current + duration[w-1]` from the
  first endpoint through the last endpoint inclusively. The asymmetric
  excluded-last scan/included-last write is executable behavior, not a
  transcription correction.
- The global gate rejects only when current manager time is strictly below the
  retained next-admission value. Otherwise it stores `current + 7.0` and
  accepts. The common feedback consumer evaluates the overlap gate and then
  the global gate without short-circuiting; kind 6 receives the first flag and
  kind 7 the second.
- Both gameplay reset and the final shader-preparation transition clear all 32
  expiry cells and the retained global value. System destruction deletes the
  same singleton and its vector. The singleton global has no other readers or
  writers; each admission function has only the common feedback call site.

## Reasoning

RTTI and the singleton construction/destruction references establish the
state owner. Constructor constants establish the complete tables independently
of any external resource. The common feedback call site supplies the note's
lane and width and calls both mutating gates in fixed order. The downstream
submitter uses the first flag only for kind 6 and the second only for kind 7.
The whole-global and function-reference closures exclude another producer,
consumer, or reset path.

## Alternatives and falsifiers

- Competing explanation: kinds 6 and 7 use only the effect-list capacity and
  cooldown already owned by `EffectManager`.
- Evidence that disproves it: the independent CharaEffectManager calls and
  separate flags passed before the EffectManager submission routine.
- Competing explanation: the overlap interval is a conventional symmetric
  half-open range.
- Evidence that disproves it: the last cell is excluded from the comparison
  loop but included in the write loop.
- Evidence that would disprove this claim: another live singleton reference,
  a second admission caller, a later rewrite of either flag, or another reset
  or duration-table writer.

## Unknowns

- The selected `NotesCharaEffectTableRecord` row values and kind-6/kind-7
  resource payloads remain external. Their table source, checked fallbacks,
  and submission consumers are closed separately.
- The binary exposes these durations only in NotesManager presentation-time
  units. No stronger player-facing unit name is assigned.

## Consequences

- Ghidra mutations: supported names and compact plate comments at `00b30660`,
  `00b30750`, `00b309d0`, `00b30820`, `00b307d0`, and `00b30970`.
- Spec sections: `spec/presentation.md` Slide extended-feedback admission.
- Reconstruction code: `SlideCharaEffectAdmissionState`, exact tables, both
  check-and-reserve functions, and reset helper.
- Tests: `tests/shared_feedback_presentation_test.cpp`.

## Verification

All references to the singleton global, both admission-function thunks, the
reset thunk, constructor, vtable deleting destructor, and downstream extended
submitter were enumerated. Focused tests cover every constant table entry,
normal rejection/re-admission, malformed lane/width guards, the asymmetric
last-cell boundary, the full-width 0..31 reservation, global cooldown
boundaries, nonfinite comparison behavior, and reset.
