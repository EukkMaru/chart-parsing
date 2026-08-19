# Claim: NotesPreCalcManager has exactly four accepted-record maps

- ID: `claim.presentation.notes-precalc-manager-map-closure`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: `state.ownership`, `audit.indirect_calls`,
  `audit.binary_saturation`
- Last reviewed: 2026-08-18

## Statement

`projView::NotesPreCalcManager` owns exactly four independent maps keyed by the
accepted-record identity stored at parsed offset `+0x84`: Slide at manager
`+0x04`, AirLadder at `+0x0c`, HeavenHold at `+0x14`, and AirSolid at `+0x1c`.
Fresh chart setup clears every map and dispatches only parsed types 2, 9, 10,
and 13 to their corresponding builders. Runtime family paths use checked
same-key lookups, and a missing key takes `std::_Xout_of_range`; there is no
fallback precompute or fifth family map.

## Anchors

- `game.exe @ RAM:00b1ed70, InitializeNotesPreCalcManager, four-map construction, hash b6ff9dbb6b5905bd81e1e4668bd757698d1b3a9e5faced1695f825ce91693911`
- `game.exe @ RAM:00da06c0 and RAM:00da1499, fresh chart load, map clear, and exact type switch, hash 0678d314e7aa6bbc63258739956ed62c800f8de9e915e805b011b2c593634604`
- `game.exe @ RAM:0044fd5e / 004167e3 / 0045d8b4 / 004285ba, recovered one-jump chart-load dispatch thunks for Slide / AirLadder / AirSolid / HeavenHold, normalized hash b291eba04ee7171fc1126e41cb02d22f4536858bcdabd0ff59b2fbaeec1fb87f`
- `game.exe @ RAM:00b267f0 / 00b264e0 / 00b265e0 / 00b266e0, four build-and-store functions, hashes aa3e7859911756e8d8c5526cc698e46f62ff2a55e8df2b86ea03d78fb44b7701, ca9889f09ac061485f9e4547783966d62972986c2a25004924d1563603d98acb, efa429109d3c34187052d275ed712881977af139a93d4c4f280bee752c730bef, and ff849ad5fbdbb5ac8ff5eb9f56e2cf1ca153bff8f52ed774cf2cfc92bef1b1a8`
- `game.exe @ RAM:00b23590 / 00b23470 / 00b234d0 / 00b23530, four checked lookup functions, hashes 7b730119714455853689209524b4d7010280f5e99f3b71385389ccad55e952f1, f0ee8059e6a782275f7157204f998c77b6497574a16fc06ca97551bfb3753ee6, 761cb9c08c6c48d1903d22f8ee0da9602e8d24d8d064310b741308a6fc7f335e, and e9727bc62e005f5622c5ffa4afd48413eb095f967019f71950d1b6342aa5cafe`
- `game.exe @ RAM:00b21ea0, ClearNotesPreCalcMaps, owned-entry destruction and sentinel reset, hash 4d824d2126b6017b48290957efdb03e3fc279921be9fdf1f2ba721ddb77f53ff`
- `game.exe @ RAM:00da1c00 / 00ace460, additional scene and gameplay-reset callers, hashes 03152b8d91e535c06dba12382f90c56f643b8398ca152a22352f707182cc1e8d and 93d81127bf2c0d278cea1ae8762863e474bfb90d4c55a089cd62613598a43b46`
- `game.exe @ RAM:01c77e60, complete NotesPreCalcManager singleton direct-reference set`

## Observations

- Construction allocates a 0x24-byte RTTI-identified manager and constructs
  four separate ordered-map owners at offsets `+0x04`, `+0x0c`, `+0x14`, and
  `+0x1c`. The constructor immediately calls the common clear routine.
- Fresh chart setup calls that clear routine, iterates the accepted parsed
  records, and switches on parsed type. The only precompute cases are:

  | Parsed type | Family | Manager map | Owned precompute size |
  | ---: | --- | ---: | ---: |
  | 2 | Slide | `+0x04` | `0x78` |
  | 9 | AirLadder | `+0x0c` | `0x78` |
  | 10 | AirSolid | `+0x1c` | `0x5c` |
  | 13 | HeavenHold | `+0x14` | `0x54` |

  Every other accepted type bypasses NotesPreCalcManager.
- Each case loads the singleton into ECX, passes the accepted parsed record,
  allocates the family-sized object, invokes the established family builder,
  and inserts ownership under parsed `+0x84` in the matching map.
- The four lookup functions are structurally identical except for map offset.
  They compare the key as unsigned and return the stored owned pointer only on
  an exact map hit. A missing key does not return null or a default family
  object; it calls `std::_Xout_of_range`.
- The complete direct-reference set partitions by family: Slide load/update/
  maintenance paths use only the `+0x04` getter; AirLadder uses only `+0x0c`;
  HeavenHold uses only `+0x14`; AirSolid uses only `+0x1c`. No other runtime
  note family reads the singleton.
- Clear destroys each family with its own recovered destructor sequence,
  releases the map nodes, restores every map sentinel/root, and zeros every
  count. It is called by manager construction and its deleting-destructor path,
  fresh chart setup, a scene reset, and the common gameplay-system reset.
- The singleton global is constructed once but is not referenced by the
  system singleton-destruction routine. Its deleting destructor exists only
  behind the manager vtable in the recovered reference set. This is an exact
  owner-lifetime boundary, not an inferred hidden teardown.

## Reasoning

The chart-load switch closes the upstream producer set. The four allocation,
builder, key-read, insertion, getter, and family-consumer chains then close
each map independently. Constructor layout and the single clear routine close
cross-family ownership and reset behavior. Whole-global references exclude a
second producer, a fifth map, another consuming family, or an unobserved
singleton teardown path.

## Alternatives and falsifiers

- Competing explanation: every runtime note type has a precompute entry.
- Evidence that disproves it: the accepted-record switch has only four cases;
  all other types bypass the manager and their runtime paths do not read its
  singleton.
- Competing explanation: a family can recover from a missing or stale key by
  selecting another entry.
- Evidence that disproves it: all four exact-key getters share the checked
  out-of-range failure path and expose no nearest, first, null, or default
  return.
- Evidence that would disprove this claim: another append into any map, a
  fifth map field, a consumer whose family differs from its getter, another
  chart-load switch target, or a second singleton writer/reset path.

## Unknowns

- Player-facing names for every internal field of the four precompute object
  layouts remain family-local. Their gameplay-visible consumers are specified
  in the Slide, AirLadder, AirSolid, and HeavenHold claims.
- Why the system singleton-destruction routine omits this 0x24-byte owner is
  not encoded as a semantic label. Map contents are still explicitly cleared
  on every recovered chart/scene/gameplay reset path.

## Consequences

- Ghidra mutations: supported manager/build/getter names at `00b1ed70`,
  `00b1f870`, `00b21ea0`, `00b267f0`, `00b264e0`, `00b265e0`, `00b266e0`,
  `00b23590`, `00b23470`, `00b234d0`, and `00b23530`; four previously undefined
  one-jump chart-load thunks recovered and named at `0044fd5e`, `004167e3`,
  `0045d8b4`, and `004285ba`; compact ownership comments at the manager,
  builder, getter, and switch boundaries.
- Spec sections: `spec/presentation.md` cross-family precompute ownership.
- Reconstruction code: existing family-specific precompute builders and
  runtime consumers; the clean-room design need not clone the original map
  containers to preserve their observable rules.
- Tests: existing Slide, AirLadder, AirSolid, and HeavenHold focused tests.

## Verification

The chart-load switch blocks were explicitly disassembled and their four jump
thunks recovered so the former anonymous calls resolve to the four builders.
The complete singleton direct-reference set was partitioned into construction,
chart/scene/gameplay clears, four family builders, four exact-key getters, and
their family consumers. Every owned-object destructor path and map reset was
walked inside the common clear routine.
