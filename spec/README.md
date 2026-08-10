# Clean-room gameplay specification

This directory will become the normative, implementation-independent description
of recovered gameplay. It starts empty of gameplay assertions intentionally.

Create focused documents as behavior is reconstructed:

- `c2s.md`: grammar, validation, defaults, and version compatibility
- `timing.md`: clocks, ticks, tempo, rounding, and update order
- `input.md`: HID-derived logical state, edges, buffering, and compensation
- `matching.md`: eligibility, priority, collision, and consumption
- `judgement.md`: outcome types, windows, boundaries, and misses
- `notes/<type>.md`: construction and state machine for each note family
- `configuration.md`: external parameters, defaults, selection, and consumers
- `presentation.md`: shared feedback, scene submission, layering boundaries,
  and external resource interfaces

Every normative rule must cite claim IDs, state boundary inclusivity and numeric
types/rounding where applicable, identify unknown parameters, and link focused
tests. Do not paste Ghidra output here.
