# Claim: six presentation table families have manager-fed paths and closed checked-accessor fallbacks

- ID: `claim.configuration.external-presentation-table-provenance`
- State: active
- Maturity: reconstructed
- Confidence: high
- Owner: codex-root
- Coverage rows: stage-one `audit.closure`; viewer rows
  `config.external_presentation`, `render.feedback_layering`,
  `render.playfield_projection`, and `render.air_ladder`
- Last reviewed: 2026-08-18

## Statement

The executable statically registers `NotesEffectTableRecord`,
`NotesCharaEffectTableRecord`, `ModelTableRecord`, `ModelSetTableRecord`,
`TextureTableRecord`, and `FieldLineFileTableRecord` with the common
`earth::TableManager`. The shared table loader composes the selected database
directory, a backslash, the registered record name, and `.bin`; the effective
basenames are therefore `NotesEffectTableRecord.bin`,
`NotesCharaEffectTableRecord.bin`, `ModelTableRecord.bin`,
`ModelSetTableRecord.bin`, `TextureTableRecord.bin`, and
`FieldLineFileTableRecord.bin`. Each family then has an opcode-identical loader
that creates its typed singleton, copies that manager-supplied path, and invokes
the table's virtual binary reader. The typed row strides, checked row accessors,
invalid-row fallbacks, and first presentation consumers are closed. The
selected database directory, deployed row values and strings, and referenced
resource contents remain external inputs.

## Anchors

- `game.exe @ RAM:00525cd0, InitializeTableManagerEntry, common manager registration, hash 71955744358db4f586565296697a65a36a72f8372a62b97712122649ba6f4840`
- `game.exe @ RAM:010e4660, FUN_010e4660, common database-directory plus registered-name plus .bin path composition and table load, hash 1d86f9e4b75354ce4a347cbb09a04d91a8c3738a2c37361cc580bb6dc3ddd5c3`
- `game.exe @ RAM:00525a10 / 00525ab0 / 00525c90 / 00525d30 / 00525e30 / 005241b0, six typed table-entry initializers, common normalized hash 651521fa28a55a364e5ee37dd9e1d03fe140e74ed0d97909b19705db6ec684bd`
- `game.exe @ RAM:010e7180 / 010ea130 / 010f1a90 / 010f4310 / 010f6c30 / 010875d0, six manager-path loaders, common normalized hash 022493c8733df441927b352b2cc79da648fcfc7912ebd1b463680fc7efd6d92b`
- `game.exe @ RAM:010e8e80 / 010e93b0 / 010e9420, NotesEffect checked resource fields, hashes ace31f5ff47787a84b2a13448e7da55685677d1f7850ddcd9fdf35f296892acd, f144a53cfe51d83802fa6d98fbe8a9f3c4b5b8d3aa608ea4e33f65e89f4da9ce, and ff63023715fd32c1ec2a5a888a67c68fb0f0837b7ad7e78af5beb6436116ca7f`
- `game.exe @ RAM:010ebe70 / 010ebc30 / 010ebdc0 / 010ebf80, NotesCharaEffect checked fields, hashes 546961109c285a51f355c0c9f18b7fbc5e088d3b837f94df22e47ecba21c766e, d5ebcc2e73f8880c9d06acee1e9040ada43cc6334cd7cf2b02a7cf62dd232991, 22b2ed5cfedb98a2642d75d812733028f8061f35517fd90cd909be78111d2131, and b418d0d726d0c54ea72498411ddb1416b1c7c6657c28a0f5a08a0d5bb5cf8003`
- `game.exe @ RAM:010f36b0 / 010f3790 / 010f3850, Model count/path/auxiliary accessors, hashes fce36bd0fa687895c353ffdd7108ae33b1e7c0201649fd0913bbc95e8ffa7ab2, ca06825b87dea4ed7161e36a14e80b9ad8cc0a7ca39d51207e5b17b984c0a4c4, and d0b85d5bddf0498f08dbf1a3540b6470260b7ce6512412aa1fb1f8658f14e212`
- `game.exe @ RAM:010f5e20 / 010f60d0 / 010f61a0 / 010f6060, ModelSet count and three checked ID fields, hashes fce36bd0fa687895c353ffdd7108ae33b1e7c0201649fd0913bbc95e8ffa7ab2, 2cb1ca5de19bfd14a2480aea08d575e77aac3d779de512263ffde3ee1b847c87, b6cbdcb0d402e497cd27a1bc1befece3b5053ec017826986d04203a6a66ae738, and 9265a4748dfe865831ace5df61e35b9ef7d056d97449f6f3a3fe34ae2f1951c8`
- `game.exe @ RAM:010f89b0 / 010f8a40 -> RAM:00c331b0, Texture count/path and resource wrapper load, hashes 6facedf463c49e8f96f433ac7f67510ae92aed84b36449da7c615b3b66d87c9b, 487925ed07b27f31ff7905ebabdbf8a02698511de634935ba5cbb886e9ddfc38, and e3cc0bfdb95450f4d9d09ea6dfaeda35ffb75ae1c2ff98840f56b4485fad3195`
- `game.exe @ RAM:010891c0 / 01088fe0 / 01089030 / 01089080, FieldLineFile count/key/model/auxiliary accessors, hashes b4fe2c734088efaee62fc2949c93a17927dbb6f86c51a70fd98c93116ebfb79b, b67107d800a03fc8ad5f117b2226e9f41400dca14d2f1f96d346e7a1c15d59c7, 7781d8228724dcf5fd590a1079bc63b39948ebeea1e393ac20f5cc73d2d3b430, and 141ef3a9269eb21b1f5570c569587a3b3fedbda88c5b66fef0e3ef4adf4ce3`

## Observations

- Every static initializer installs an `earth::TableEntry<typed record>`
  vtable and its exact record-class string in the shared registry. The manager
  initializer separately installs the `earth::TableManager` manager entry.
- The common loader combines the externally selected database directory with
  `\\`, the registered record-class string, and `.bin`. The six basenames are
  exact consequences of their registration strings; no per-family filename
  guess is involved. The directory prefix remains external.
- The six family loaders are structurally identical. Each lazily allocates a
  typed 0x48-byte table singleton, copies the path owned by the manager entry,
  and calls the typed table virtual reader. The row deserializers and string
  pools are family-specific, but the source path remains the manager argument.
- Row strides are exact: NotesEffect `0x44`, NotesCharaEffect `0x28`, Model
  `0x14`, ModelSet `0x30`, Texture `0x10`, and FieldLineFile `0x20` bytes.
- NotesEffect's checked integer accessors return `-1` for an invalid row.
  Fields `+0x0c/+0x1c/+0x20/+0x28/+0x2c/+0x30/+0x34` supply the span buckets
  for ordinary feedback kind 0; `+0x38` and `+0x3c` supply kinds 1 and 2. The
  consumers reject negative, zero, and out-of-resource-range IDs before
  submission. This closes the table provenance behind
  `claim.presentation.shared-result-feedback` without assigning the row values.
- NotesCharaEffect's resource fields `+0x0c` and `+0x14` return `-1` on an
  invalid row and feed Slide extended-feedback kinds 6 and 7. Its associated
  `+0x20/+0x24` parameters return zero on invalid rows. Caller gates and
  nonzero resource tests remain independent.
- Model's `+0x0c` string accessor resolves the row through the table string
  pool and returns the shared empty-string sentinel on invalid indices;
  `+0x10` returns zero when invalid. ModelSet's checked signed-ID accessors
  return `-1`; downstream preload/load callers validate selected IDs against
  the target table count and also reject zero before resolving paths/resources.
- Texture's `+0x0c` string accessor has the same checked empty-string fallback.
  The resource-wrapper loader first validates the row against the exact count,
  then copies the selected path into the resource loader. Invalid selection
  constructs the wrapper with an empty path rather than dereferencing a row.
- FieldLineFile supports both ordered-index and associative-key lookup. Missing
  keys and invalid rows return `-1`. The checked `+0x1c` field is consumed as a
  ModelTable row ID and reaches the same checked Model path accessor; another
  signed ID at `+0x18` also returns `-1`, but its player-facing field name is
  intentionally left unresolved.

## Reasoning

RTTI-backed table entry registration identifies each family without guessing
from a consumer. The generic composer proves how that registered name becomes
the effective `.bin` basename, and the opcode-identical family loaders connect
the composed manager-owned path to the typed singleton. Record pointer
arithmetic establishes the stride and exact field; invalid branches establish
fallback independently of any deployed row. Finally, the presentation
consumers validate those outputs and either select a resource path/player or
suppress the request. This is a closed source-to-consumer provenance chain even
though external table contents are absent.

## Alternatives and falsifiers

- Competing explanation: the resource IDs, filenames, or row defaults are
  compiled constants in `game.exe`.
- Evidence that would disprove this claim: a consumer bypassing the typed table
  singleton for a compiled row, a family loader that ignores the manager path,
  or an invalid-row path that dereferences a record instead of returning the
  documented sentinel.

## Unknowns

- The selected database directory, deployed record values, and referenced
  resources are absent external data. The exact basename convention is proved
  by the common composer; the full installed path cannot be recovered without
  the external directory selection.
- ModelTable `+0x10`, FieldLineFile `+0x18`, and several ModelSet fields have
  closed widths/fallbacks but not trustworthy player-facing names. They remain
  opaque typed parameters until their complete consumer slice supports names.
- External models, textures, animations, materials, scene pass rows, and final
  pixels remain excluded source assets. Their absence does not reopen the
  recovered selector/fallback behavior.

## Consequences

- Ghidra mutations: supported names for the manager/table initializers, six
  manager-path loaders, table counts, Model/Texture checked path accessors, and
  the FieldLineFile model-ID accessor; compact comments on checked opaque fields.
- Spec sections: `spec/presentation.md` external presentation table boundary.
- Reconstruction/tests: existing shared-feedback and AirLadder reconstruction
  already encode the gameplay-visible selectors. No external row value or
  asset is added to the clean-room code.

## Verification

Registration vtables/strings, all six loader bodies, table record counts and
pointer strides, invalid branches, and representative downstream consumers
were inspected independently. The accessor reference set reaches shared result
feedback, model/model-set preload/load paths, Texture-backed AirLadder resource
loading, and FieldLine model selection. No inspected path feeds these external
presentation values back into chart generation, input, candidate selection,
judgement classification, result dispatch, or terminal state.
