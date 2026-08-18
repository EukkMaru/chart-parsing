# Claim: AirSolid builds a clipped two-surface path and terminates at its authored end

- ID: `claim.presentation.air-solid-path`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `render.air_solid`, `time.playback_seek`, `audit.indirect_calls`
- Last reviewed: 2026-08-18

## Statement

The accepted type-10 record is precomputed into a source-sequence-keyed
AirSolid path. At runtime, each authored endpoint is projected with its own SLA
selection, clipped to the executable's visible depth interval, split at the
first raw judgement-plane crossing, and emitted as an optional bridge quad plus
a four-quad shell. The note requests the shared deferred terminal state when
manager current position reaches its stored authored end. Resource identity
and final camera composition are external inputs; the static base-white color,
path math,
style coordinate, clipping, seam, topology, winding, and lifetime rule are
executable-owned.

## Anchors

- `game.exe @ RAM:00b24200, FUN_00b24200, type-10 precompute builder`
- `game.exe @ RAM:00b235f0, FUN_00b235f0, precompute span append`
- `game.exe @ RAM:00b265e0, FUN_00b265e0, accepted-sequence allocation and insertion`
- `game.exe @ RAM:00b234d0, FUN_00b234d0, sequence-keyed runtime lookup`
- `game.exe @ RAM:018d3e98, executable-owned 16-entry style-coordinate table`
- `game.exe @ RAM:00c16fa0, FUN_00c16fa0, runtime load and resource setup`
- `game.exe @ RAM:00c16900, FUN_00c16900, start projection and lifetime update`
- `game.exe @ RAM:00c16a90, FUN_00c16a90, active endpoint projection`
- `game.exe @ RAM:00c05ac0, FUN_00c05ac0, clipping, seam, and vertex emission`
- `game.exe @ RAM:00c16f70, FUN_00c16f70, authored-end predicate`
- `game.exe @ RAM:0042d394, FUN_0042d394, common deferred terminal request`
- `game.exe @ RAM:00c16e60, FUN_00c16e60, maintenance/finalization`
- `game.exe @ RAM:00c16f60 / 00c16f90 / 00c16e90, immediate preload family`
- `game.exe @ RAM:00c16550, FUN_00c16550, owned-resource destruction`

## Observations

- Scene setup allocates a `0x5c`-byte precompute object and inserts it under
  parsed accepted-source identity `+0x84`. Runtime load uses that same identity;
  this is not a positional or chart-type lookup.
- The precompute converts root and endpoint schedule scalars with the exact
  `0.06F` unit. It independently selects the root SLA tag and every authored
  endpoint SLA tag. Lane spans use the common decoded lane-center transform.
  The two integer-tenth endpoint properties are restored with `* 0.1F`.
- The normalized style code selects one of sixteen embedded coordinates:
  `{0.1561999917, 0.9688000083, 0.9061999917, 0.8436999917,
  0.7811999917, 0.7186999917, 0.6561999917, 0.5938000083,
  0.5311999917, 0.4688000083, 0.4061999917, 0.3438000083,
  0.2811999917, 0.09399998188, 0.03100001812, 0}`. Codes 0 through
  14 use an externally supplied primitive resource identity. Code 15 (`NON`)
  stores the missing-resource sentinel instead. The preload state is ready
  immediately; the primitive wrapper is prepared per precompute/runtime load.
  A valid checked resource handle assigns topology 3. An invalid index leaves
  the default zero handle and is rejected by the common Joint callback, so
  every submitted AirSolid primitive uses topology 3.
- Runtime retains raw-relative and SLA-adjusted projected schedules in separate
  arrays. Each endpoint's lane center becomes `(coordinate - 8) * 4`; each
  property becomes `(value - 1) * 3.8934999`; decoded width remains a width
  until vertex construction.
- The mesh builder rejects a segment only when both projected endpoints are
  below `-600` or both are above `50`. Otherwise, when absolute projected span
  is at least `0.000001`, it independently clips either endpoint to those
  boundaries and linearly interpolates lateral position, both vertical
  properties, decoded width, and both neutral coordinates. Raw-relative values
  are not rewritten by projected clipping.
- The first segment satisfying `raw_start < 0.000001` and
  `raw_end > -0.000001` receives a seam. Its interpolation fraction is
  `clamp(abs(raw_start) / max(abs(raw_end - raw_start), 0.000001), 0, 1)`.
  The seam uses the cached common projection of raw position zero and
  interpolates lateral position, both vertical properties, width, and neutral
  coordinates. The builder duplicates the segment around this seam and clears
  the seam boundary flags used by the adjacent primitive portions.
- Each neutral vertex is exactly `0x18` bytes: lateral, vertical, projected
  depth, exact startup color `0xffffffff`, coordinate U, and coordinate V. For
  this precompute, U is `0.5` and V is the selected embedded style coordinate.
  Left/right half-extent is exactly `2 * decoded_width`.
- One enabled segment can emit a six-vertex bridge across end-property-A and
  start-property-B. Its shell is four six-vertex quads (24 vertices) joining
  the start/end left/right edges of property A and property B. The binary
  reverses triangle winding only when end projected depth is strictly less
  than start projected depth; equality, unordered comparisons, and increasing
  projected depth use the other winding.
- The scheduled callback calls the start/lifetime update and then the geometry
  update. The former compares stored authored end with manager current position
  through the `+0x38` virtual. On `end <= current`, it invokes the common
  request that changes active state 1 to requested state 2. The geometry update
  then observes the terminal request and does not rebuild vertices that tick.
- Candidate output remains all sentinels and the manager-adjacent judgement
  virtual remains a shared no-op. The terminal edge is therefore a lifetime
  transition, not a judgement result. Maintenance finalizes the keyed primitive
  resource; destruction releases the owned vertex vector and shared base.

## Reasoning

The parsed source identity is followed through precompute insertion, runtime
lookup, active scheduling, endpoint projection, mesh generation, terminal
request, maintenance, and destruction. The two anonymous parsed properties are
not assigned speculative player-facing names: their exact recovered role is
the two vertical surfaces of the AirSolid shell. Separating raw and projected
schedule explains why clipping and judgement-plane seam placement use different
values without treating either as an approximation.

## Alternatives and falsifiers

- Competing explanation: `NON` selects a different embedded material.
  Falsifier: an executable path that replaces the observed missing-resource
  sentinel for normalized style code 15.
- Competing explanation: the `+0x38` comparison is dormant. Falsifier: removal
  of the direct call from the scheduled lifetime update, or evidence that the
  common terminal thunk is unreachable from that call.
- Competing explanation: either anonymous property is non-geometric metadata.
  Falsifier: a different active consumer that supersedes the observed two
  independent vertical-coordinate conversions and shell-corner use.

## Unknowns

- The player-facing names and intended authored ranges of the two vertical
  properties remain unavailable. Their executable use is exact and retained
  structurally as property A and property B.
- The primitive resource identity is externally loaded. Its selection site is
  known, but its runtime contents are not copied into the clean-room
  reconstruction. The vertex color is the exact executable startup constant
  closed by `claim.presentation.shared-static-colors`.
- Shared final camera, layer ordering, blend/material behavior, and scene
  composition remain in the cross-family presentation rows. Therefore
  `render.air_solid` remains partial even though the family-local mesh and
  lifecycle path are closed.

## Consequences

- Ghidra mutations: supported plate comments were added at `00b24200`,
  `00b265e0`, `00c16fa0`, `00c16900`, `00c16a90`, `00c05ac0`, `00c16f70`,
  `00c16e60`, `00c16550`, and `0042d394`.
- Spec sections: `spec/notes/air_solid.md`.
- Reconstruction code: `AirSolidGeometryEndpoint`,
  `AirSolidGeometrySegment`, style/lifetime/seam/clipping helpers, and
  `build_air_solid_vertices` in `include/chart/reconstruction.hpp`.
- Tests: `tests/air_solid_presentation_test.cpp`.

## Verification

Normalized opcode hashes for the principal anchors are:

- `00b24200`: `194ae8e96b81a8b8dc091f4e8dbefe6571d7c6f2bc82a9ff77dd67ce2e445185`
- `00b235f0`: `78b29810c0639a560105d9c803a70c2887700e90c45ff9674f20688c1fbed80d`
- `00b265e0`: `efa429109d3c34187052d275ed712881977af139a93d4c4f280bee752c730bef`
- `00c16fa0`: `f8ff648290220d54c0495177825a360f6edbf3318be76aec1368cb4c1f64de84`
- `00c16900`: `93a3fe975a9c45430d14a857a465bae97827d3bfc9853a1e0bad03d10df9c002`
- `00c16a90`: `a63e8c23215750c97ca1121fa86f66dcfd756e4e13d3423b75feeeaced018c24`
- `00c05ac0`: `0ef38695a6ee89607db46366d486fece940ff86a63b81e29c1793d530fe096bb`
- `00c16f70`: `372f9e6643cd25d1751704eb0ab51937143b241b2c7bc828e60b646651b78348`
- `0042d394`: `b291eba04ee7171fc1126e41cb02d22f4536858bcdabd0ff59b2fbaeec1fb87f`
- `00c16e60`: `a947ff989d8572e6854faff7f371e667b2ff0195e5a458937d951f24be014716`
- `00c16550`: `a7249977d40320298cd78e18e8c778739469cbcb9b0d3123714d806c9bb0b2c3`

Focused tests cover style selection, `NON`, terminal equality and NaN behavior,
raw-origin detection and fraction, endpoint interpolation, near/far clipping,
raw/projected separation, half-extents, exact base-color propagation, both
winding branches, optional bridge/shell cardinality, equal depth, and unordered
depth. The full 42-test suite passed after reconstruction.
