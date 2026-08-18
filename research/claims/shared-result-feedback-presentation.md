# Claim: shared result feedback has fixed pre-dispatch transforms, lane ownership, effect order, and scene submission

- ID: `claim.presentation.shared-result-feedback`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.feedback_layering`, `config.external_presentation`,
  `audit.cross_family_visual`
- Last reviewed: 2026-08-10

## Statement

All twelve runtime note vtables reach the same result-feedback consumer before
authoritative result dispatch. Their `+0x44` slots are identical. Ten `+0x48`
slots use the common one-position wrapper directly; Mine and Flick use
opcode-identical wrappers which apply active result-control remapping to the
incoming result byte and then call that same one-position wrapper. The common
consumer has an exact binary-owned order: select and adjust a transient
feedback result/variant, attempt fixed effect-list kinds 2, 0, and 1 at three
positions, update a 16-lane serial-group overlay for a closed set of source
categories, dispatch the result, conditionally emit a fixed post-result cue,
and finally attempt the two Slide extended-result effects. Accepted effect
players receive a monotonic creation key and are bound to the executable-
selected `MainScene` or `BgScene`; external resource rows, players, scene
composition, materials, and textures determine their unavailable final pixels.
The two Slide attempts additionally receive independent, mutating lane-overlap
and global-cooldown admission flags owned by `CharaEffectManager`.

## Anchors

- `game.exe @ RAM:00c1c340, FUN_00c1c340, common feedback and dispatch order, hash 6d22a0d5b382d07b18f6280e0005284623e862a2b09397886156820a8f22c698`
- `game.exe @ RAM:00c1cce0, FUN_00c1cce0, one-position wrapper, hash 0dcd4b4e45fbedd93333f7bede561b3d611d27b6f350020e05ec0b41f80bb086`
- `game.exe @ RAM:00c1f8f0, FUN_00c1f8f0, Mine result-remap wrapper, hash 0dd2a1a09d4343cf48d4f77762cd7be495ce8c2e459a8c9751c8f4f9241b729c`
- `game.exe @ RAM:00c20050, FUN_00c20050, opcode-identical Flick result-remap wrapper, hash 0dd2a1a09d4343cf48d4f77762cd7be495ce8c2e459a8c9751c8f4f9241b729c`
- `game.exe @ RAM:00c1a960, FUN_00c1a960, active result-control bridge, hash 2bbe5b3a1406189a885c2d14388841f58476c78210b6134d2c0e16f703077913`
- `game.exe @ RAM:00b93ba0, FUN_00b93ba0, loaded-profile first-unit threshold remap, hash c6a2236cbca7f998527eb863064d2cf0bb6a5faff609318923094d18872fde85`
- `game.exe @ RAM:00b1b370, FUN_00b1b370, effect-list kind 2, hash faa2ca139e18884f2145b5c616bfbf1e7eb39b1654b5a4c62eca7e04fa456dc3`
- `game.exe @ RAM:00b1af90, FUN_00b1af90, effect-list kind 0 and span resource selection, hash 6f4ec15e575c24443212af9336153603f120181267e632cff7d53db7890f7e38`
- `game.exe @ RAM:00b1b210, FUN_00b1b210, effect-list kind 1, hash 96ba9bb6aee301d106a4d04af40f3ba28a93c4b23985d894fc779d50fb2b59e5`
- `game.exe @ RAM:00b2f830, FUN_00b2f830, lane serial-group update, hash d7cb7a54a5ef94fd332a95723680995eff23798824bd5513d7728a41318a8fe7`
- `game.exe @ RAM:00b1ccc0, FUN_00b1ccc0, result/variant resource mode, hash 2b2aa550415147a010a10644ab7204c3ee9d4de428683168b39de3420a8e9af0`
- `game.exe @ RAM:00c1b6c0, FUN_00c1b6c0, Slide trailing selector, hash 5f21849fa87b589723f490b38a41f5afd17a12e3be60fd2022a2d4c7c1c46ef4`
- `game.exe @ RAM:00b1ab50, FUN_00b1ab50, two Slide extended effects, hash 57ef1f4fb30391e1d35cd12765231f1a67a9ad85d4ab2c92f73dc26da22faf7f`
- `game.exe @ RAM:00b18370, FUN_00b18370, ten fixed effect lists, hash 3bca2a30449ac2c5db72cc61bdc8ba15622423e84f1b645b85f232e0a9847420`
- `game.exe @ RAM:00b1d1f0, FUN_00b1d1f0, effect admission/construction, hash c9bbafd4f75839776dbfd3a4ad0c5bec7e58678f949dd38bed133f5bbf88fc35`
- `game.exe @ RAM:00b1d880, FUN_00b1d880, fixed-list update order, hash e6500a80c166c7029bc3559937ee028e71f74430c84b8c6b2f0f8f0d8653518d`
- `game.exe @ RAM:00bfdec0, FUN_00bfdec0, scene binding and sequence submission, hash 1bdbd89b14dd8b8474b61e2b2b2efc6df629587c91434cd1d2854c94694b8e68`
- `game.exe @ RAM:00b983e0, FUN_00b983e0, selected terminal predicate, hash 5188ac06b1986108cd01e2b395a641cb6d2d21c71bc02397bb5c6ed255d3173b`

## Observations

- All twelve mapped note vtables use `00c1c340` at slot `+0x44`.
  `00c1cce0` passes one position as all three effect positions. Ten vtables use
  it directly at `+0x48`; Mine uses `00c1f8f0` and Flick uses `00c20050`.
  Those two functions have the same normalized opcode hash and both call
  `00c1a960` on the incoming result byte before forwarding the remapped byte
  and unchanged remaining arguments to `00c1cce0`.
- The remap bridge copies the byte and reaches the loaded-profile controller
  described by `claim.judgement.active-tier-zeroing`: a valid result at or
  below the first valid configured threshold becomes zero. It does not read or
  write a position. Mine's held-lane average/span-center calculation belongs
  only to its separate zero-result success-effect producer. Ordinary Mine and
  Flick paths can already have applied the same remap before `+0x48`; applying
  this threshold-to-zero transform again is idempotent.
- The function samples the currently selected ordinary/alternate terminal
  predicate before feedback. A true predicate suppresses all three ordinary
  effect attempts. An optional manager byte further restricts kind 2 to
  source categories 2 through 6; kind 1 is independently disabled for those
  categories. Kind 0 has only the common terminal gate.
- Results 3/4 share one loaded setting selector, result 2 uses a second, and
  result 1 a third. Each selector clamps to `loaded_count - 1`; the nominal
  domain indexes seven adjacent embedded four-byte masks. The masks either
  clear the requested variant or remap result 1 through 4 to transient result
  5. Result 4 always clears the variant. A zero count or selected index beyond
  the four embedded entries is an unchecked malformed external domain.
- The adjusted result and variant map to resource mode exactly as reconstructed
  in `map_feedback_resource_mode`: the six supported result rows are
  `7`, `{4,5,6}`, `{1,2,3}`, `{0,8,9}`, `{0,13}`, and `{12,10,11}`.
- Kind 2 reads external source-row field `+0x3c`. Kind 1 reads `+0x38` and,
  under one manager setting, remaps resource IDs `0x18` to `0x3e` and `0x1c`
  to `0x3f`. Kind 0 chooses fields `+0x34`, `+0x30`, `+0x2c`, `+0x28`,
  `+0x20`, `+0x1c`, or `+0x0c` for spans 1, 2, 3, 4, 5..6, 7..8, or
  9..16. Negative, zero, and out-of-range resource IDs do not submit.
- Kind 0 scales X by `span / bucket_width` only for source categories 7..9.
  Kinds 0 and 1 select `BgScene` and flip X for source category 15; kind 2
  stays in `MainScene`.
- Lane feedback runs only for source categories 0, 1, 14, and 15, plus source
  category 17 when result is zero. Start clamps to zero, wrapped
  `start + span` clamps at 16, and an empty interval is ignored. A normal zero
  result is ignored; the category-17 case supplies the sole override. One
  serial is allocated per call. Before assigning a selected lane, every lane
  with that lane's prior nonzero serial is cleared, so overwriting part of a
  prior span clears the complete old group.
- Authoritative result dispatch occurs after the three ordinary effects and
  lane update. The fixed cue then re-reads the selected terminal predicate.
  Configured modes 1, 2, and 3 accept result bytes below 1, 2, or 3; other
  modes accept none. Therefore the current result can suppress its own cue by
  latching terminal during dispatch, while its pre-dispatch ordinary effects
  used the earlier terminal state.
- Slide source category 1 with nonzero result maps its trailing unsigned
  selector 0..7 through either embedded `{0,1,2,4,3,6,5,7}` or a writable
  runtime table. Values at least 8 become `-1`. A valid external result-effect
  row independently gates kind 6 and kind 7 submissions, in that order. Before
  the submitter, `CharaEffectManager` evaluates its lane-overlap reservation and
  then its global-cooldown reservation without short-circuiting. Kind 6 uses
  the first flag and kind 7 the second; their exact state machines are owned by
  `claim.presentation.slide-extended-feedback-admission`.
- The effect manager constructs ten lists in the order Bomb, Reaction, Text,
  Continue, AirRing, SonicBoom, CharaNote, CharaBG, Mine, MineBG. Their
  capacities are `24,24,24,32,16,16,16,4,48,4`; only CharaBG and MineBG use
  cooldown count 5. That count is converted with the selected 60/120 rate by
  rounded `(1000 / rate) * count`, producing 83 or 42 milliseconds.
- Normal gameplay admission compares submissions since the last list update,
  not active occupancy, with the fixed capacity and then checks the cooldown.
  A non-forced append at full active occupancy stops/removes the oldest effect
  before appending; forced preload appends bypass that eviction and cooldown
  restart. The note-view preload temporarily enables a separate
  tuple-deduplicating force mode and disables it at the end of the staged pass.
  Each accepted player receives the current global 32-bit sequence key, which
  is then incremented with wrap, before it is started and appended to its fixed
  list. List updates run in constructor order, erase state-3 objects, record
  maximum post-erase occupancy, and reset the per-update submission counter.
  Exact clear/reset callers and the clear slot's retained counter are closed by
  `claim.presentation.effect-list-lifecycle-closure`.
- The appended `EffectBase` wrapper is a closed four-state machine. Start queues
  state 1, its update queues state 2, and state 2 queues terminal state 3 after
  external-instance disappearance. Because terminal testing reads current
  state rather than pending state, natural disappearance is erased on the
  following list update. Exact stop, visibility-bit, translated-position, and
  destruction behavior is owned by
  `claim.presentation.effect-player-state-machine-closure`.

## Reasoning

The vtable inventory closes every note entry into the common consumer. Direct
control flow fixes the feedback-before-dispatch/cue-after-dispatch order, while
the effect helpers close every resource-field selector and scene flag. The
effect-manager constructor, admission routine, player initializer, update
loop, preload-mode callers, and cleanup path jointly establish ownership and
lifetime rather than inferring them from effect names. The player initializer
and wrapper state machine prove the executable-selected scene, creation key,
visible/terminal lifetime, and retained Slide controls, but delegate mesh,
material, animation, and final sorting behavior to the loaded player/scene
objects.

## Alternatives and falsifiers

- Competing explanation: every effect uses the post-dispatch result state or
  one common scene and generic span resource.
- Evidence that would disprove this claim: a mapped note vtable with another
  result-feedback target, a path that dispatches before the ordinary effect
  calls, another writer of the lane arrays outside the closed owner, or an
  accepted effect player that bypasses both named scene lookups.
- Competing explanation: the ten effect-list names are their cross-family draw
  order.
- Evidence that would prove that stronger claim: a downstream scene traversal
  that consumes the list order as a render sort key. The observed list order
  currently proves update/lifetime order only; the monotonic player key and
  external scene/material records are the known rendering inputs.

## Unknowns

- The source-row resource IDs, loaded selector counts/settings, writable Slide
  table, result-effect rows, and their resource-player contents are external
  runtime inputs. The snapshot proves their selectors, fallbacks, and
  consumers, not deployed values.
- `MainScene`/`BgScene` lookup and the monotonic key are exact. Their final
  camera transform, scene-relative sort, depth test/blend state, meshes,
  materials, textures, animation curves, and pixels remain downstream or
  external and are not inferred from list names.
- The fixed cue's resource construction is closed to hardcoded identifiers,
  but the audio/visual payload they resolve is external and unnamed here.

## Consequences

- Ghidra mutations: supported plate comments at `00c1c340`, `00c1cce0`,
  `00c1f8f0`, `00c20050`, `00c1a960`, `00b93ba0`, `00b1b370`, `00b1af90`, `00b1b210`, `00b2f830`, `00b1ccc0`,
  `00c1b6c0`, `00b1ab50`, `00b18370`, `00b1d1f0`, `00b1d6d0`, `00b1d880`,
  `00b1a900`, `00b1d750`, `00b1b680`, `00b1a660`, and `00bfdec0`.
- Spec sections: `spec/presentation.md` and `spec/configuration.md`.
- Reconstruction code: exact per-family `+0x48` result-remap selection,
  feedback mask/resource mapping, span resource
  selection, scene and lane gates, serial-group state, cue gate, effect-list
  descriptors/admission/cooldown/lifecycle, four-state player lifetime and
  visibility, sequence allocation, Slide feedback row, and the separately
  claimed CharaEffectManager admission state.
- Tests: `tests/shared_feedback_presentation_test.cpp`.

## Verification

The common consumer was checked at both assembly and decompiler levels to pin
the lane helper's hidden owner and stack argument order. The seven embedded
masks, embedded Slide table, effect-list descriptor records, both scene-name
branches, list virtual targets, preload mode callers, normal update/cleanup,
all eight EffectBase vtable slots, all twelve state callbacks, effect-object
terminal/visibility state, and both CharaEffectManager flags were independently
followed. Focused and full build/test validation are recorded in the session
handoff.
