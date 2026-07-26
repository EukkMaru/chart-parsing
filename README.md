# Chart gameplay reconstruction

This workspace supports static reverse engineering of `game.exe` to recover the
gameplay logic driven by external `.c2s` chart files. The target is behavioral
equivalence at the game's apparent tick granularity: parsing, scheduling, note
state machines, input matching, judgement, misses, and compensation behavior.

This is not an executable reimplementation project. Boot, accounts, song
selection, graphics, score multipliers, skills, and result presentation are out
of scope unless they directly alter gameplay judgement.

## Start here

Agents must read [AGENTS.md](AGENTS.md), then run:

```bash
python3 scripts/harness.py doctor
python3 scripts/harness.py validate
python3 scripts/harness.py next
```

For a one-line continuation handoff, start a fresh session with:
“Read [_temp_prompt.md](_temp_prompt.md) and follow it.”

The durable state of the investigation is [research/STATUS.md](research/STATUS.md)
and [research/COVERAGE.tsv](research/COVERAGE.tsv). Ghidra's local `chart`
project is the authoritative analysis database. The current clean-room
reconstruction is header-only under `include/`; focused tests belong under
`tests/`.

## Build the reconstruction skeleton

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

No game binary, chart corpus, raw assembly, or decompiler dump should be copied
into the documentation or reconstruction code.
