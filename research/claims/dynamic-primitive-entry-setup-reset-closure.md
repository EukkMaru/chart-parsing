# Claim: dynamic-primitive entry setup and reset are closed

- ID: `claim.presentation.dynamic-primitive-entry-setup-reset-closure`
- State: active
- Maturity: verified
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `state.ownership`, `audit.indirect_calls`,
  `audit.closure`; viewer row `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

Both dynamic-primitive vtables use one common configuration, pending-pointer
access, backend-count reset, and finalizer sequence. Configuration stores the
four observed inputs, resolves external vertex stride, computes the exact
32-bit vertex-byte count, acquires one write pointer, and mirrors selector and
topology into submission state. Reset clears only the renderer backend's
recorded vertex count. No hidden owner-local clear or allocation release occurs
in these common slots.

## Anchors

- `game.exe @ RAM:0045b226 -> RAM:006db950, SetDynamicPrimitiveEntryConfiguration, hash bfc3e770c571e2aee2dcb10e663a3014452f3e8b067cb6449e56686e06d1b017`
- `game.exe @ RAM:00455bf5 -> RAM:006c6cb0, GetDynamicPrimitiveWritePointerSlot, hash 1500b0a0e079f696964bcaf15e78023be5308f1156e2df300ceff4af93660942`
- `game.exe @ RAM:004673d2 -> RAM:006db9d0, ResetDynamicPrimitiveBackendVertexCount, hash 0fc97865f5775aa4e1c6a98772d7f162000fbe3a97d627dbe734652525ad6762`
- `game.exe @ RAM:00449625 -> RAM:006c6e70, SetDynamicPrimitiveCollector, hash 98eeba85cbe66e45fa6e8f0f53a2ab90b97d0a2a08921b7bd1da1377d5c80f86`
- `game.exe @ RAM:0066e630, ResolveDynamicPrimitiveVertexStride, hash 1db8aa33eeb73fbc670772c0e150d10f35460d2dc38ff3e615a6aa388888cc57`
- `game.exe @ RAM:0066d340 / RAM:0066d360, AllocateDynamicPrimitiveVertexStorage wrapper and implementation, hashes 1568aa1d49b733e0bbdf2e91047962e87df6bec760b478fad94267e57ebd4702 / e645a4bedfb5d1e6e982dba0343c3a6bdf8eeb3e7dc1515ad44e2bd46a553349`
- `game.exe @ RAM:0066d470, ClearDynamicPrimitiveBackendVertexCount, hash 41dd05e7b5f98367192603ae87352fed66f178aa5db389032e2872d3bb255924`
- `game.exe @ RAM:0186b4a0 / RAM:0186d3f0, complete dynamic-util and embedded-entry-helper vtables`
- `game.exe @ RAM:019c9eac -> RAM:01c146b4 / RAM:019cbc14 -> RAM:01c146e4, RTTI complete-object locator and type-descriptor chains`

## Observations

- DynamicPrimitiveUtil and DynamicPrimitiveEntryHelper install the same
  accessor at vtable `+0x04`, configuration at `+0x08`, reset at `+0x0c`, and
  finalizer at `+0x10`. Only standalone DynamicPrimitiveUtil has the sixth
  `+0x14` collector setter; the embedded helper ends at object size `0x158`.
- Configuration writes layout selector `+0x08`, topology/mode `+0x0c`, vertex
  count `+0x10`, and byte flag `+0x1c`; obtains the selector stride at `+0x14`;
  computes `count * stride` at `+0x18`; mirrors selector/mode to `+0x90/+0x94`;
  and stores the allocator result at `+0x150`.
- The accessor returns `this + 0x150`; line, triangle, Sprite, and Joint
  builders dereference that slot before populating their observed vertex
  layouts. Finalization later zeros the contained pointer.
- The stride resolver returns zero when the shared backend is absent. With a
  backend, it reduces the signed selector by 17 and queries the selected
  external layout entry. Known gameplay selectors are nonnegative and in the
  observed table domain; no additional validation is present in this helper.
- The allocator records selector at backend `+0x1b8`, vertex count at `+0x1bc`,
  and topology/mode at `+0x1c0`. It rounds the current byte offset upward to a
  stride boundary, adds two stride units when mode is exactly 4, allocates the
  prefix plus `count*stride`, and returns the address after the prefix.
- The reset virtual ignores its owner object and forwards to a helper that
  writes zero only to backend `+0x1bc` when the backend exists.
- The configuration thunk has two vtable data references plus direct calls
  from the already excluded WindManager and AuraScene producers. Chart-side
  Joint/Sprite builders reach the same slot through their embedded entry
  helper; their exact callers and tuples are independently closed.

## Reasoning

The two dynamic vtables bound every common virtual target, while direct body
xrefs account for specialized stack producers. Field writes establish the
input roles without relying on guessed engine enum names: the third argument
is multiplied by resolved per-vertex stride and matches every emitted vertex
cardinality, while the second controls allocator mode and is mirrored to
submission state. The independent finalizer and destructor passes establish
the pointer's post-submit clear and exclude teardown ownership from setup/reset.

## Alternatives and falsifiers

- Competing explanation: reset also invalidates owner-local payload, or an
  unlisted configuration target changes chart vertex cardinality.
- Evidence that would disprove this claim: a third dynamic vtable target, an
  owner-local write in the reset body, a setup field consumer inconsistent
  with selector/topology/count roles, or a chart builder bypassing both common
  vtables.

## Unknowns

- External layout-table rows determine concrete selector formats and strides;
  only lookup, selection, arithmetic, and consumers are executable-owned.
- Backend allocation storage, frame-buffering index selection, and failure
  consequences beyond the observed zero/no-backend return remain renderer
  boundaries and are not guessed.
- Semantic enum labels for selector and topology/mode values remain unnamed;
  claims retain exact numeric tuples.

## Consequences

- Ghidra mutations: supported names and compact comments for configuration,
  pending-pointer access, collector setter, stride resolver, allocation
  wrapper, backend-count reset virtual, and reset implementation.
- Spec sections: `spec/presentation.md`, dynamic-primitive finalization and
  teardown.
- Reconstruction code: exact configuration field update and 32-bit byte-count
  arithmetic in `include/chart/reconstruction.hpp`.
- Tests: Sprite tuple configuration, external stride injection, mirrored
  selector/mode, acquired pointer, flag, and 32-bit multiplication wrap in
  `tests/scene_camera_presentation_test.cpp`.

## Verification

All entries of both dynamic vtables were resolved through their thunks, then
cross-checked against direct callers, the common geometry convenience helpers,
Joint, Sprite, Wind, and Aura. The reset implementation was followed through
the global backend write; the setup allocation was followed through stride
lookup and prefix arithmetic. Focused tests reconstruct every owner-visible
write while keeping external stride and storage explicit inputs.
