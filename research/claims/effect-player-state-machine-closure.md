# Claim: feedback effect-player state, visibility, and terminal lifetime are closed

- ID: `claim.presentation.effect-player-state-machine-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer rows `render.feedback_layering`,
  `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

Every accepted `projView::EffectManager` submission constructs the same
0x84-byte `projView::EffectBase` wrapper. Its complete eight-slot vtable,
four-state callback table, pending/current-state distinction, start, live
external-instance test, stop, terminal predicate, visibility flag, position
setter, destruction, and list-removal timing are closed. External resource
payload determines the instance contents and when it disappears, but no
executable-owned wrapper transition remains opaque.

## Anchors

- `game.exe @ RAM:018d9164 through 018d9180, complete eight-slot EffectBase vtable; zero padding follows at 018d9184`
- `game.exe @ RAM:00bfd130 / 00bfd320 / 00bfd430, four-state callback registration, state-machine base construction, and EffectBase construction, hashes 757ba004e2104890755552392b0f7df2eda00f38587f6fe1f2f92025931238bf / e82fc06cc08d1e41a47411399adceaee75e5de4127a06cd44d07ce22919826d2 / d756c629fa77028df7a1a1cceb83f4bc2201521ab783149770b31fbab3cec734`
- `game.exe @ RAM:00bfd0b0..00bfd310, twelve state callbacks: ten one-byte no-ops share hash 68ad3fa3cd5f1c61843bd5049f8c2abc368a0f84a18ec4fb25932fa47efcdfaa; starting/live updates at 00bfd2c0 / 00bfd2d0 have hashes e70d1f93784d4d642b06b9376901bbae62cdb12da802f5411f3c0ca4f57a7f8f / 52cd558b01dfb8f608250648e91844dca56c3dc4d13d4e9db4cf2caf28c7f58a`
- `game.exe @ RAM:00bfd720 / 00bfd740 / 00bfd7e0 / 00bfd800 / 00bfd870 / 00bfd890, per-state exit/enter/update callback invocation and dispatch, hashes 4065bfe4b4b83a0fb87d3e51bfc504a95d9ab748f96b99666944d04f69687b52 / 7cba2cae8e5ec639d94f778d69c58a37bbcc253f5395a43bdd96eeb92e063dd6 / da3f25eca3eef2ff1e204e8a88f6ff0f81632fc64207070a65591fad1a254bbe / 88bf0530a069ee62424276a3782bb606c2d62e227079ce6652e03b45b30306c4 / 87b8e003350e5777a68f9fb3d1da55551c67efff0a2586f47887008468c02e9d / 343bdde6e50efefe8a6dfe7e1d1787023e1b05973d06dd2336d00ca34124b090`
- `game.exe @ RAM:00bfd900 / 00bfe110 / 00bfd6a0, pending-state transition, EffectBase update, and current-state terminal predicate, hashes dce59e6e37dc14e83edee0baff21a4997446dcdfc0c22854116878ecd00fcb56 / d22e98b354dd91d1ee457d59e57e2976a3205253247077584ba9f7da19903d7c / 963321046ef5e2c57977cf01c5ec3d5936a3532245f9a07836e02f7e5782e6bd`
- `game.exe @ RAM:00bfdec0 / 00bfdb50, start and idempotent stop, hashes 1bdbd89b14dd8b8474b61e2b2b2efc6df629587c91434cd1d2854c94694b8e68 / 86fc1e4aa8fe1b91dcbf10c9fc912db099325faec7daba1b7799362b8931ea63`
- `game.exe @ RAM:00bfdb00 / 00bfdd30 / 00bfdd60, retained visibility, external resource-entry selection, and translated identity-matrix setters, hashes d7dc67deb6ae6531637a134cf39b06c55a2c518d1b5c7dc24ddf93515cbcb3d7 / 1b90d6bcf11c34c5afbaf980439f0dd0ab52a22f30c2bb4815ff703352b9efb0 / bb389f9772b5d75cb86dea9afc9826e7edd6756bf3c592f53d447e795e421a59`
- `game.exe @ RAM:01232710, external player entry resolution and immediate/queued application, hash f14bcd8706f119ed42bfbdc31bf0a1feef5f1f7ae92f83e8297d65dfe668fcc7`
- `game.exe @ RAM:00c0ee40 / 00c0fe20, runtime Slide retained-handle control and Slide preload step, hashes 081634ec8e24c1d970ce8ee43104eaef0940b64500bd6703aeb622b8df0e673d / 278c5347b522ca3dec61a15baff28b8e725cdca839a586e6c8a4b60c55977448`
- `game.exe @ RAM:00c0c720 / 00c10720, Slide construction and parsed-record load latch initialization, hashes 454526ecc7488693d897290d545c2cd9d612f542d96b91017dbca85aeff2f503 / fd4d18a9ce086e341f48c7f7c5df26745aa1171b120f9154220ee42f8999559b`
- `game.exe @ RAM:00bfd4c0 / 00bfd5d0, destructor and deleting destructor, hashes 81920e23575953819b03eca08813b85527c0110d00a5f4640729390f6cc516a5 / c246561e3d852b3b7576af76fa47137ce3e9832796108369556f7d01247bc231`
- `game.exe @ RAM:00b1d7e0, list update and post-update terminal removal, hash bc211f0ecf124818a278bd589944dd31acaf331f7cd1152f2a21b14ab9321ada`

## Observations

- Construction sets current and pending state to `-1`, the state tick to zero,
  retained visibility to true, and the stopped byte to false. Start queues
  state 1 after external-player start; it does not directly replace current
  state.
- The state IDs are 0 idle, 1 starting, 2 live, and 3 terminal. Every enter
  and exit callback is a no-op. Idle and terminal update are no-ops. Starting
  update queues state 2. Live update preserves state 2 while the external
  player lookup returns an instance and queues state 3 otherwise.
- On an unpaused update, the wrapper increments its phase tick, applies a
  pending state by running exit, replacing current state, zeroing the tick,
  clearing pending state, and running enter, then runs the new current-state
  update callback. The constructed factory/fallback maps have no reachable
  registration writer, so the optional additional current-state handler stays
  absent on every EffectManager and retained Slide-handle path.
- Terminal testing reads current state, not pending state. Therefore a live
  update which first notices external disappearance queues 3 but survives that
  list update. The following update applies current state 3 and is then erased.
  A note-owned stop occurs before the ordinary all-list update, so that update
  can apply its queued state 3 and erase the effect in the same outer gameplay
  pass. Capacity eviction instead drops the list handle immediately.
- Stop is idempotent. On its first call it requests external stop when an
  instance exists, queues state 3, and latches the stopped byte. Destruction
  uses the same guarded stop before releasing state-machine storage.
- Visibility is retained at byte `+0x80`. True clears external instance flag
  `0x4`; false sets it. The setter applies the bit immediately, and every
  wrapper update reapplies it after state work. Construction defaults to true.
- The position setter resolves the current external instance and submits a
  4x4 identity-basis matrix with the supplied three floats in translation
  elements 12, 13, and 14. Its sole direct gameplay owner is the retained Slide
  feedback handle. Slide's visibility setter callers, completion stop, reset,
  and destruction paths account for all other direct wrapper-control calls.
- Slide construction and parsed-record loading both initialize a retained
  resource-entry latch to `-1`. While the retained handle exists, presentation
  phase 2 makes it visible, updates its position, and, if the latch is nonzero,
  selects external entry 0 and clears the latch. Other phases hide the handle
  without clearing it. No other reachable writer rearms the latch.
- The Slide preload step is the other complete owner of entry selection. At
  shared resource steps 11, 12, and 13 it walks every retained handle and
  applies visible plus entry 0, visible plus entry 1, then hidden plus entry 1.
  Steps 14 through 19 do no further handle work; step 20 latches the Slide
  preload object ready.
- Entry selection resolves the requested integer through the external player's
  loaded entry table and fallback. A valid entry is queued when player flag
  `0x8000` is set and applied immediately otherwise. The executable-owned
  request IDs and timing are exact even though the selected external payload is
  not available.

## Reasoning

The RTTI-bounded vtable fixes every indirect EffectBase operation. The state
parameter constructor supplies all twelve callback targets, including the ten
previously undefined one-byte functions; following the three dispatch layers
establishes their enter/update/exit positions. Upward xrefs from wrapper
setters and stop reach only the EffectManager list and retained Slide feedback
owners. Downward flow reaches external lookup/start/stop/transform calls but
returns to the closed pending/current state machine before list erasure.

## Alternatives and falsifiers

- Competing explanation: external disappearance removes an effect in the same
  list update in which it is first observed.
- Evidence that disproves it: live update writes pending state `+0x14`, while
  the list's following terminal predicate reads current state `+0x10`.
- Evidence that would disprove this claim: another EffectBase vtable slot, a
  writer of its callback/factory maps after construction, a terminal predicate
  reading pending state, another writer of Slide's resource-entry latch, or a
  reachable wrapper control call outside the enumerated EffectManager/Slide
  owners.

## Unknowns

- The loaded player controls proprietary mesh, animation, material, texture,
  and natural instance lifetime. The executable-owned reaction to presence or
  absence is exact; the payload and disappearance time remain external input.
- The external instance's other flag bits and matrix consumer semantics are
  not assigned product names. Only the exact bit-`0x4` transformation and
  exact submitted matrix are claimed.
- The loaded entry table determines what entries 0 and 1 contain. The snapshot
  proves their resolution/application path and every chart-side request, not a
  player-facing animation or effect name.
- The neighboring matrix-adjustment helper at `00bfdb90` has no code or data
  references in this snapshot. Its two exact transform branches are recorded
  in Ghidra but excluded from reachable gameplay unless a caller is found.

## Consequences

- Ghidra mutations: created and named the eleven previously undefined state
  callbacks, named the complete EffectBase state/visibility/lifetime chain,
  named the Slide runtime/preload entry-selection owners, and added supported
  comments at the vtable, state registration/update, start/stop, visibility,
  position, resource-entry, list-removal, and unreferenced-transform anchors.
- Spec section: `spec/presentation.md`, feedback effect-player wrapper.
- Reconstruction code: `FeedbackEffectPlayerState`, pending transition/update,
  idempotent stop, terminal predicate, visibility bit, position matrix, Slide
  retained-handle latch, and preload handle-control sequence.
- Tests: `tests/shared_feedback_presentation_test.cpp` covers start-to-live,
  delayed natural terminal, same-update explicit stop, pause, visibility bits,
  position matrix layout, Slide's one-shot entry-0 latch, and preload's exact
  visible/entry sequence.

## Verification

All eight vtable entries and all twelve state callbacks were followed at both
instruction and decompiler levels. Constructor, normal update, external-loss,
explicit stop, list eviction, list removal, Slide control, reset, and
destruction paths were checked in both directions. All four direct resource-
entry call sites and both latch initializers were independently enumerated;
the adjacent transform helper has zero references. Focused/full validation is
recorded in the active session handoff.
