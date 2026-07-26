# Claim: gameplay setup freshly selects its source chart path from the content catalog

- ID: `claim.pipeline.source-chart-path-selection`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `pipeline.boundaries`, `parser.discovery`
- Last reviewed: 2026-07-21

## Statement

Each gameplay setup initializes the controller's three selected-content strings
and freshly assigns its source chart path from the current content-catalog
record. The path is available only when that record has at least six 64-byte
selector entries. The requested one-byte selector is used when it is below the
external selector-table count and is otherwise clamped to that count minus one.

## Anchors

- `game.exe @ RAM:00da06c0, FUN_00da06c0, gameplay setup builds the selection descriptor and supplies controller +0x5b4 as the third output`
- `game.exe @ RAM:00b4e220, FUN_00b4e220, selected-content string initializer and source-path selector`
- `game.exe @ RAM:00b4e6a0, FUN_00b4e6a0, content-record lookup with an empty process-default fallback`
- `game.exe @ RAM:00d81e40, FUN_00d81e40, only other selector-helper caller and non-gameplay consumer of the first two outputs`
- `game.exe @ RAM:008c2430, FUN_008c2430, content-record default constructor`
- `game.exe @ RAM:009072f0, FUN_009072f0, external-resource record construction/deserialization/copy path`
- `game.exe @ RAM:008cf7f0, FUN_008cf7f0, content-record destructor including the 64-byte-entry vector`

## Observations

- `FUN_00da06c0` constructs a small selection descriptor, then calls
  `FUN_00b4e220` with controller strings at `+0x584`, `+0x59c`, and `+0x5b4`.
  It copies `+0x5b4` only after that call and immediately applies the separately
  recovered `.xml` to `.c2s` transformation.
- `FUN_00b4e220` first initializes all three outputs to empty strings. It looks
  up the current content record through `FUN_00b4e6a0`; a missing key returns a
  process-owned default record whose constructor initializes the selector
  vector empty.
- The third output remains empty unless the selected record's begin/end pair
  describes more than five 64-byte entries. When present, the descriptor byte
  at `+4` is compared with the one-byte result of `FUN_0106e120`. An in-range
  selector is retained; an out-of-range selector becomes the table count minus
  one. The string at offset `+4` in that selected 64-byte entry is assigned to
  the third output.
- The selection helper has exactly two callers. `FUN_00d81e40` passes temporary
  outputs, consumes the first two in a content-transition helper, and destroys
  all three; only `FUN_00da06c0` persists and transforms the third output for
  chart loading.
- The selected record type's default constructor zeros its selector-vector
  begin/end/capacity fields. `FUN_009072f0` constructs a temporary record,
  deserializes it from an external resource, copies it into the process catalog,
  and destroys the temporary. The destructor walks and frees the same 64-byte
  vector. No gameplay setup/reset writer to that catalog vector was recovered.

## Reasoning

The call order proves that controller `+0x5b4` is a fresh output of catalog
selection rather than stale state inherited from a previous gameplay session.
The fixed element stride, vector-size threshold, selector comparison, and
single string assignment establish the exact supported-domain index rule. The
constructor/deserializer/destructor chain places ownership in the external
process catalog, outside chart state and runtime-note teardown.

## Alternatives and falsifiers

- Competing explanation: controller `+0x5b4` is the authoritative owner and the
  helper merely decorates an already selected path.
- Evidence that would disprove this claim: a path that writes `+0x5b4` after
  `FUN_00b4e220` but before the transformation, a hidden indirect gameplay
  caller with a different selector contract, or evidence that the third output
  aliases an input instead of being initialized and assigned by the helper.

## Unknowns

- Exact player-facing names of the selector entries are external-resource
  semantics and are not assigned here.
- The binary performs no observed check for an external selector count of zero
  or for the clamped index exceeding the selected record's vector. Those
  malformed external-table domains would cause unchecked access; the
  clean-room interface reports them explicitly instead of guessing a fallback.

## Consequences

- Ghidra mutations: none in the live project; GhidraMCP remains unavailable and
  analysis used the temporary read-only clone.
- Spec sections: `spec/c2s.md` chart source selection and path derivation.
- Reconstruction code: `include/chart/reconstruction.hpp`.
- Tests: `tests/chart_path_test.cpp`.

## Verification

The helper's complete direct caller set was checked through its sole thunk. The
content record's constructor, external deserialization/copy path, and destructor
were independently traced. Focused tests cover direct selection, clamp to the
external last entry, a smaller external table, the six-entry availability
threshold, zero external count, catalog/table mismatch, and the downstream
literal path replacement.
