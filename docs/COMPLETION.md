# Completion and saturation gates

The grand goal is met only when the evidence supports perfect gameplay
reconstruction for this snapshot at apparent tick granularity. “It seems
complete” is not a gate.

## Required gates

1. **Closed boundaries** — Every path from accepted chart input to gameplay
   outcome is mapped; excluded downstream code receives no value that feeds back.
2. **Parser completeness** — Every corpus command/version variant is classified,
   including ignored, invalid, default, and backward-compatibility behavior.
3. **Time completeness** — Clock source, tick conversion, rounding, tempo changes,
   update order, discontinuities, and boundary inclusivity are reconstructed.
4. **Note completeness** — Every constructible note/variant has construction,
   state transitions, input needs, judgement, miss, reset, and destruction rules.
5. **Input completeness** — HID-to-logical state, edge/level semantics, lane/width,
   buffering/compensation, matching, consumption, and priority are resolved.
6. **Interaction completeness** — Simultaneous, overlapping, compound, and
   cross-note behavior has explicit ordering and focused tests.
7. **Configuration completeness** — Every gameplay-affecting constant has a
   source/consumer/default/selection description. Unavailable external numeric
   values are parameterized and clearly distinguished from recovered logic.
8. **Control-flow saturation** — Relevant indirect calls, vtables, callbacks,
   alternate modes, error paths, and reset paths are either resolved or proven
   unable to affect gameplay.
9. **Reconstruction traceability** — Each C++ behavior links to a clean-room spec,
   claim, Ghidra anchor, and test; no implementation is justified by convention.
10. **Corpus compatibility** — The parser/reconstruction accounts for all local
    chart versions without silently discarding unknown gameplay commands.
11. **Contradiction audit** — No active claims conflict; superseded findings have
    had their dependent names, types, specs, and tests reviewed.
12. **Independent closure review** — A fresh pass attempts to find omitted state,
    calls, modes, and boundary cases and records why each candidate is covered.

## Blocking conditions

Any `unknown`, `mapped`, or `investigating` gameplay row in coverage blocks
completion. So does any undocumented gameplay-relevant indirect target, inferred
field with incompatible accesses, unexplained branch, unparameterized external
constant, or test that encodes an assumption absent from the evidence.

Externally unavailable values do not block reconstruction of the logic when the
configuration interface and all consumers are fully recovered. They do block a
claim that exact numeric judgement outcomes are known for the missing profile.

## Final audit record

The closure review must create a dedicated claim/report listing binary identity,
coverage snapshot, remaining parameterized values, corpus command inventory,
entry/exit boundaries, indirect-call audit, contradiction search, test results,
and explicit residual uncertainty. If residual uncertainty can affect gameplay,
the project is not complete.
