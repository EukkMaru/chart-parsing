# Scope and behavioral boundary

## Target pipeline

```text
C2S bytes/text
  -> format parsing and validation
  -> internal chart/event representation
  -> tempo, measure, and tick scheduling
  -> gameplay note/state construction
  -> tick/update processing
  -> HID-derived input state and edges
  -> candidate matching and priority
  -> note-specific judgement/state transitions
  -> gameplay outcome events
```

Every state, branch, clock, compensation rule, and cross-note interaction that
can change the outcome belongs in scope.

## Included

- Chart discovery where it selects or transforms gameplay chart data
- Every accepted `.c2s` header/event command and backward-compatibility path
- Tick/tempo/measure conversion and rounding
- Spawn/timeline construction, ordering, simultaneous events, and collisions
- Tap, hold, slide, air, flick, and any other gameplay-relevant note variants
- Device/HID transport only far enough to recover logical input state
- Input edges/levels, lane/width mapping, matching, consumption, and priority
- Judgement categories, windows, boundary inclusivity, misses, releases, and
  compensation behavior
- Update ordering, pause/resume, discontinuities, and frame/tick catch-up
- Externally loaded constants: identity, shape, consumers, defaults, and
  selection behavior, even when their runtime values are unavailable

## Excluded by default

- Boot/security checks, accounts, networking, song selection UI, rendering,
  animation, audio presentation, score display, skills, and result aggregation
- Faithful graphics, screen layout, scroll direction, resolution, and cabinet UI
- Running the Windows executable

An excluded subsystem becomes relevant only when a traced value or side effect
can alter the included pipeline. Document the narrow dependency; do not reverse
the entire subsystem.

## Intended consumers

The recovered logic must support both live human input and prerecorded input.
The immediate product is the logic/specification, not a playable frontend.
Tests should therefore prefer deterministic tick-stamped logical inputs.
