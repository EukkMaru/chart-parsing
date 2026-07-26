# Agent operating contract

## Mission

Recover all gameplay-affecting behavior between `.c2s` chart ingestion and
judgement outcome from the exact local `game.exe` snapshot. Produce an auditable
behavioral specification and a clean-room C++ reconstruction. Fidelity is at
the game's apparent tick granularity.

Read, in order:

1. `docs/SCOPE.md`
2. `docs/WORKFLOW.md`
3. `docs/EVIDENCE.md`
4. `docs/GHIDRA.md`
5. `docs/COMPLETION.md`
6. `research/STATUS.md` and `research/COVERAGE.tsv`

Then run `python3 scripts/harness.py doctor`, `validate`, and `next`.

## Non-negotiable rules

- Static analysis only. Do not execute `game.exe`, use Wine, attach a debugger,
  emulate the whole program, or attempt to bypass its boot checks. Isolated
  Ghidra P-code reasoning is allowed only when it does not execute the game or
  mutate source artifacts.
- Do not run Git commands or modify Git state. This is a local research
  workspace even though a `.git` directory exists.
- Treat `game.exe`, `music.zip`, `music/`, and the Ghidra project as local source
  material. Do not copy raw game data, assembly listings, decompiler dumps, or
  mechanically translated game code into workspace documents or source files.
- Public information is a lead, never proof. Confirm behavior in this binary.
- Constants may be externally loaded and absent from the executable. Distinguish
  the location/selection logic from the unavailable runtime value.
- The chart corpus comes from this snapshot but contains backward-compatible
  charts authored under older resource versions. Do not mistake chart version
  variation for executable version variation.
- Never describe a behavior as verified because it merely looks conventional.
- Do not broaden work into rendering, UI, accounts, skills, or scoring unless a
  traced dependency can alter gameplay generation or judgement.

## Ghidra write policy

Ghidra renames, types, enums, structs, comments, and bookmarks are encouraged
when they improve persistent analysis. Apply them only through Ghidra/MCP, not
by editing `chart.rep` files.

- Tentative role: keep the default symbol; add a hypothesis bookmark/comment.
- Supported role: rename only after compatible callers, callees, and data use
  establish the function's responsibility.
- Type or structure: apply only when observed field accesses, widths, calling
  convention, and relevant call sites agree.
- Before a broad or batch mutation, inspect a representative sample and ensure
  the operation is reversible inside Ghidra.
- Record meaningful mutations in the active session note and relevant claim.

## Work loop

1. Check binary identity and MCP health with `harness.py doctor`.
2. Select work from `harness.py next`; do not chase an unrelated interesting
   string while a blocking call-path question is active.
3. Mark the target `investigating` and name its owner in `COVERAGE.tsv`.
4. Trace both upstream and downstream control/data flow. Inspect negative/error
   paths, indirect calls, state ownership, reset paths, and timing dependencies.
5. Write compact claims using `research/templates/CLAIM.md`. Cite stable Ghidra
   anchors and explain reasoning without pasting raw output.
6. Update Ghidra annotations using the confidence gate above.
7. Update the relevant clean-room spec. Add C++ only for reconstructed behavior;
   add tests when an observable rule can be stated.
8. Run `harness.py validate`, build, and tests. Update coverage and status.
9. End every session with a handoff using `research/templates/SESSION.md`.

Use the hybrid research strategy in `docs/WORKFLOW.md`: maintain the broad
gameplay map, then complete vertical note-type slices. One agent owns Ghidra
writes at a time; parallel agents, when explicitly requested, perform bounded
read-only investigations with disjoint targets.

## Definition of done

Do not claim the grand goal is met until every gate in `docs/COMPLETION.md`
passes. “No more hidden logic” must be supported by closed entry-to-exit call
paths, resolved indirect behavior, complete state ownership/reset analysis,
cross-note interaction coverage, corpus compatibility, and an independent
contradiction audit. A high decompiler-completeness score alone is insufficient.
