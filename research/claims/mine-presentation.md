# Claim: Mine has one persistent width model and an exact zero-result effect protocol

- ID: `claim.presentation.mine-root-success-effect`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.mine`, `render.playfield_projection`,
  `render.feedback_layering`, `config.external_presentation`
- Last reviewed: 2026-08-10

## Statement

Mine's root resource selection, transform, explicit visibility writes,
reset/destruction, preload, and terminal feedback trigger are closed. Widths
1..16 select rows 145..130. Phase 0 refreshes the shared projected transform;
terminal phases 1/2 do not hide the root themselves. After active result
remapping, only byte zero emits the Mine success-effect pair, at a binary-defined
held-average or span-center lateral position and depth -65.

## Anchors

- `game.exe @ RAM:00c1f940, FUN_00c1f940, root load/setup, hash 8cb20909cc9ec471e437ddf003bbf14aff1b2dd395284570e42664ba437a6a5e`
- `game.exe @ RAM:00c1fac0, FUN_00c1fac0, judgement/presentation order, hash 22d52cd71f40ed597fdb6b20ec118a0ab5df7850419e1e9ac83833115130e8d9`
- `game.exe @ RAM:00c1ee00, FUN_00c1ee00, contact/result and success-effect producer, hash 3ef2a30cc3e7cd3b9020b1b3c62e2822a9e8cc1537bdfb34fe38a3df924442a2`
- `game.exe @ RAM:00c1f3b0, FUN_00c1f3b0, root active transform, hash e22583340cceaa437738f5b9ec0ef6bad398a5983929fd580de4f8b3f637358a`
- `game.exe @ RAM:00c1f690, FUN_00c1f690, terminal table query, hash 47c78518f05916a9c55ab80c06ca47d261434cdadf32a44673dbd559f517a1c3`
- `game.exe @ RAM:00c1f590, FUN_00c1f590, explicit root hide/reset, hash 348b3b584de6fa7f4f63450e80bb933aeb8db68e9e9fdcc1c13736c5914609db`
- `game.exe @ RAM:00c1f7a0, FUN_00c1f7a0, preload state machine, hash cc7ef0ebaf87a6ed669a9b8fd0ad3c67937ecac1af9b9637a4fe469fb51393e9`
- `game.exe @ RAM:00c1f6a0, FUN_00c1f6a0, terminal feedback split, hash 7a69ec1f6e5bdff615b3c419853cc967636669c680e34670bc2f8e108098e47e`
- `game.exe @ RAM:00b29920, FUN_00b29920, startup width-table consumer, hash b251f17a1f082bb2b0e897608dda6aa736f59d02b199288ab0bb500d5053bd79`
- `game.exe @ RAM:004be800, FUN_004be800, width-selector startup initializer, hash b66514c28137fb3aea44a5832b36b95eda9e18d8ec698f44bfbef5bd115b33fe`
- `game.exe @ RAM:016f1b28, initializer-table entry for FUN_004be800`
- `game.exe @ RAM:00b1ad60, FUN_00b1ad60, conditional two-effect constructor, hash 3a2d53566192dec9b93bda3a7f9b98427d2520f9506b92d1efcd4794947f5806`
- `game.exe @ RAM:00c1eda0, FUN_00c1eda0, owned-state destruction, hash f046569f32ad1afe2b24c15516d747bac3da410e58b9f34f8ce83d3dcd1e0514`
- `game.exe @ RAM:00b28870 and RAM:00b28890, common lateral/depth transforms, hashes 1cc795d4bdfb7e1e4f971672fd21b7eaa2a3ef012b82b2bc753e726b14909e68 and 19696db1083c59e7e5324f7e5dcc4866796038ba33244d08ddd4b650966ceae8`

## Observations

- Root load clamps `decoded_width - 1` to `[0,15]` and indexes the embedded
  descending external-resource row sequence `145..130`. Its translation and
  scale are the common Tap-family model transform. Load additionally writes
  `1.0` to field `+0x24` on two optional model-owned subobjects and explicitly
  makes the root visible; no unsupported semantic name is assigned to those
  subobjects.
- The active wrapper calls the complete contact/result function before root
  presentation. The presentation half queries the fixed phase table
  `{0,1,1}` over valid phases 0..2. Phase 0 forces visibility, computes
  scheduled delta through the common timing adjustment, and writes common
  base-offset/positive-DCM projected depth. Phases 1/2 issue no model write.
  Reset is the explicit hide, and destruction releases the owned state.
- Ordinary contact processing initializes a lateral source to the chart-span
  center. If at least one covered lane is held in the resolving sample, it
  replaces that source with `sum(held_lane_indices)/held_count`. Forced success
  uses the span center. The producer applies the common lane-edge conversion
  `(source-8)*4` and calls common projection with zero delta and base-offset
  disabled, which is exactly depth `-65`.
- Active result remapping occurs before the feedback split. Remapped byte zero
  invokes a fixed cue selector 7/parameter 0 and the success constructor;
  nonzero invokes shared feedback inputs 4/0 and does not invoke that
  constructor.
- Mine's `+0x48` feedback wrapper applies the same active result-control remap
  to the incoming result byte before the common one-position wrapper. This is
  idempotent after the ordinary producer-side remap and covers the forced
  result route. It does not calculate the held-average success-effect position.
- The success path maps decoded width with an inclusive clamp `[0,16]` into a
  17-entry executable startup table. Zero-initialized entries 0 and 1 remain
  zero; the initializer writes entries 2..16 as 1..15. The exact selected
  value is therefore `max(clamped_width - 1, 0)`. That value, the computed
  position, chart start lane, and runtime/preload flag feed a shared
  constructor. It emits kind
  8 only when one external handle exists and kind 9 only when a second handle
  exists; the first call carries fixed lifetime input 100. These identities
  and gates are recoverable even though external effect contents are not.
- Preload step 0 stages all 16 root rows under resource group 11, step 1 stages
  feedback group 17, and step 2 calls the same success constructor for every
  width in preload mode at a judgment-plane seed. Readiness is set only after
  step 28. Preload reset hides its owned objects.

## Reasoning

The runtime vtable and wrapper order connect Mine's state producer and model
consumer. The row and terminal tables close selection and valid lifecycle.
Both ordinary and forced result branches converge on the same result wrapper
and expose their exact position construction before the same effect call. The
preloader independently exercises every row and width-selected effect path.

## Alternatives and falsifiers

- Competing explanation: Mine success is always centered on its chart span.
- Evidence that would disprove this claim: absence of the held-index
  accumulation/division or a later overwrite restoring the span center.
- Competing explanation: Mine hides immediately on success/failure.
- Evidence that would disprove this claim: a hide call in the terminal branch
  of the active model update rather than only maintenance/reset.

## Unknowns

- Root meshes/materials/native widths, two effect handles, fixed secondary
  effect position, and cue contents are
  external resource/configuration data. The clean-room renderer must provide
  original substitutes while preserving selectors and gates.
- Final camera/viewport conversion and cross-family draw order remain in their
  dedicated rows.

## Consequences

- Ghidra mutations: plate comments at `00c1f940`, `00c1f3b0`, `00c1fac0`,
  `00c1f690`, `00c1f590`, `00c1f7a0`, `00c1f6a0`, `00b29920`, `00b1ad60`,
  and `00c1eda0`; created the missed default-named initializer at `004be800`
  after dry-run and documented its startup-table ownership.
- Spec section: `spec/notes/mine.md`.
- Reconstruction code: `mine_model_resource_row`,
  `mine_root_model_update_enabled`, and `mine_success_effect_*`.
- Tests: `tests/mine_presentation_test.cpp`.

## Verification

Constructor/load, model update, terminal query, wrapper order, ordinary and
forced result branches, explicit reset, destruction, all row-table entries,
startup width table and clamp, both conditional effect-handle branches, and the complete
preload schedule were checked. Focused tests cover row and width boundaries,
every phase, ordinary held-average and fallback positions, forced position,
result trigger, fixed depth, effect kinds, and lifetime input.
