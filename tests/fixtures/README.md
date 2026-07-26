# Synthetic fixtures only

Fixtures in this directory must be minimal and authored from the clean-room
specification. Do not copy or trim a chart from `music/`. Each fixture should
state which claim/spec rule it tests and isolate one boundary or interaction.

Prefer programmatically constructed events until the `.c2s` grammar itself is
reconstructed. Never make a corpus-derived observation silently become an
expected gameplay result.
