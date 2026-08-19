# Claim: every feedback effect-list virtual and reset edge is closed

- ID: `claim.presentation.effect-list-lifecycle-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer rows `render.feedback_layering`,
  `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

`projView::EffectManager` owns ten fixed `EffList` objects whose complete
twelve-slot vtable, normal and forced admission, active-capacity eviction,
cooldown, per-update budget, update, statistics, clear, and destruction paths
are closed. Capacity is not one predicate: normal admission limits submissions
since the previous list update, while non-forced append separately evicts the
oldest active effect when occupancy is already at capacity. Forced preload
append bypasses both eviction and cooldown restart and can exceed capacity.

## Anchors

- `game.exe @ RAM:018d3d2c through 018d3d58, complete EffList vtable; the next complete-object locator begins at 018d3d68`
- `game.exe @ RAM:00b18280 / 00b18a10 / 00b19720, InitializeFeedbackEffectList, DestroyFeedbackEffectList, and deleting destructor, hashes 6ec6b83f3b2f42a91ea7be7fec34574c7f9be91b95881a04ae2d98837fdece00 / 6363e8dbc19754f9101a84fbff83274ffda34d7e1d173df9ac198fb0d019c3c8 / 80752e09df0e8ac9a968009be4946500af800006c2b8c1d5e286fbf7f3ef8102`
- `game.exe @ RAM:00b1a860 / 00b1d7e0, ClearFeedbackEffectList and UpdateFeedbackEffectList, hashes 933e53f7e1e005f04d80cb62ceb12eaf5e23f5e6c431450b558f3dc473cfecda / bc211f0ecf124818a278bd589944dd31acaf331f7cd1152f2a21b14ab9321ada`
- `game.exe @ RAM:00b1a6c0 / 00b1d0e0, normal-admission composition and append transition, hashes 647e1363f22e25e667706bc4d8b8ba4c01d0359bd71401c6fb9bc780d7e9d049 / bad7be25c47a4a7c87f45fd1f2d2df634dd2c40c20db97cee28d7c9c77ca77cd`
- `game.exe @ RAM:00b1ce00 / 00b1ccb0 / 00b1ce20, active-count, maximum-occupancy, and accepted-total accessors, hashes d435c77af39050f253754ef203aff98c333e56e0fbf1c92496ec751f0b018576 / 77775f84edcb4af028ec944e815e96f4c04ec3501b5b7261406d6638cd0af118 / 4288e0b6a532b947551e2a9c37ba78dcf6c82566f30ed68825b5d8f0b45dc88f`
- `game.exe @ RAM:00b1a6f0 / 00b1a5f0 / 00b1a700 / 00b1a660, per-update budget, oldest-active eviction, cooldown-ready, and cooldown-restart slots, hashes 245bf5ff7cde7f82d39cf867386897d8609af41a518d4cf2d2f8d979431284c8 / 17dfb17db13cf08b3936dbaea2795bd6eb7f0cf6e404ab800c9d267c41bf8c2f / ab54acfb08e1d312350bedd0f88cc2d4d2782211ca5ff8ea141bc5c276077643 / 3c101fd68797d542581d1b2e974ec3c6220c7b7ce9e6bd77fb9b75b5c764205b`
- `game.exe @ RAM:00b18370 / 00b1d1f0 / 00b1d750 / 00b1b680, manager construction, submission, preload-mode entry, and exit, hashes 3bca2a30449ac2c5db72cc61bdc8ba15622423e84f1b645b85f232e0a9847420 / c9bbafd4f75839776dbfd3a4ad0c5bec7e58678f949dd38bed133f5bbf88fc35 / bd39d3bf4a93bc6d3c40e4284864461340c0abc08170dc9498bf7431d1c5c642 / 0dda1e72b39f4ffb62759f8a05e5ef47f4693230c07d3dd78fcf0e5a079314c6`
- `game.exe @ RAM:00b1a900 / 00b1d880, all-list clear and update dispatchers, hashes 41f589b0b0b8d4717c374382be638593752c7957c21d18373c884e3236987116 / e6500a80c166c7029bc3559937ee028e71f74430c84b8c6b2f0f8f0d8653518d`
- `game.exe @ RAM:00ace460 / 00da27f0 / 00da8730 / 00da9820, complete clear/update owners, hashes 93d81127bf2c0d278cea1ae8762863e474bfb90d4c55a089cd62613598a43b46 / 427bd55cf4b1ec8cfb33c319c74f42553f45126749af1ef8509600f927dc08b0 / ed2d269bf3ba4bfcefd8300792569c3c467773fc2b2a543636e5db55b918712a / ce8c5ca2608c3b0e7c39932b2e6042056865380a6824f611ea0342be79c90f1c`

## Observations

- Vtable slots are deleting destructor, clear, update, normal admission,
  append, active count, maximum occupancy, accepted total, submission-budget
  check, capacity eviction, cooldown-ready check, and cooldown restart. There
  is no thirteenth slot before the next RTTI record.
- Normal admission first checks `submissions_since_update < capacity`; only if
  true does it check the timer. Active occupancy is not read by admission.
- Non-forced append stops and marks the oldest active effect terminal, then
  removes its handle immediately when occupancy is at least capacity. It then
  restarts cooldown and appends the new effect. Forced append skips both steps.
  Every append increments active occupancy, the per-update counter, and
  accepted total with 32-bit wrap.
- Update visits active effects in stored order, removes each state-3 object,
  records the maximum of the prior maximum and post-removal occupancy, and
  zeroes the per-update counter. The ordinary and alternate gameplay paths
  update all ten lists once after note substeps.
- Clear releases all active handles, resets the cooldown timer, and zeros
  maximum occupancy and accepted total. It does not write the per-update
  counter. Its two callers are the common presentation-manager reset and the
  final shader-preparation transition; the latter updates first, so its counter
  is zero before clear.
- Preload mode clears its tuple set, deduplicates accepted parameter tuples,
  and forces accepted submissions. Leaving staged preload clears the mode byte.

## Reasoning

The RTTI-bounded vtable fixes every indirect list operation. Following each
slot downward separates the similarly named capacity states; following the
all-list update and clear dispatchers upward closes their gameplay/reset
owners. The exact field writes contradict the prior active-count description
without relying on effect appearance or conventional pool behavior.

## Alternatives and falsifiers

- Competing explanation: capacity admission is based on active occupancy and
  therefore never needs eviction.
- Evidence that disproves it: the admission slot reads `+0x14`, append
  increments it, update zeros it, while the eviction slot independently derives
  occupancy from the active handle vector.
- Evidence that would disprove this claim: an EffList vtable target beyond the
  twelve recovered slots, another all-list clear/update caller, or a writer of
  the per-update counter outside constructor/append/update.

## Unknowns

- Countdown time ultimately depends on the runtime millisecond source; the
  executable-owned 60/120 conversion and timer comparison are closed.
- The executable-owned EffectBase state, natural/explicit terminal timing,
  visibility bit, retained Slide position, and destruction are closed by
  `claim.presentation.effect-player-state-machine-closure`. External players
  still determine meshes, animations, materials, and final pixels.
- Diagnostic meanings of maximum occupancy and accepted total do not alter
  admission; their exact writes and reset are retained without product labels.

## Consequences

- Ghidra mutations: supported names/comments on the complete twelve-slot
  EffList vtable targets, manager construction/submission/preload functions,
  all-list update/clear dispatchers, and both clear owners.
- Spec sections: `spec/presentation.md`, effect-list ownership, lifetime, and
  submission.
- Reconstruction code: `FeedbackEffectListState`, append/update/clear state
  transitions, and corrected per-update admission semantics.
- Tests: `tests/shared_feedback_presentation_test.cpp` distinguishes active
  occupancy from per-update budget, covers normal eviction, forced overflow,
  update reset/maximum, and the clear slot's retained counter.

## Verification

Every vtable entry from the complete-object vtable start through the next RTTI
boundary was followed. Direct dispatcher callers and per-field writers were
enumerated independently, then the child EffectBase vtable and state callbacks
were closed separately. Focused/full build and validation are recorded in the
active session handoff.
