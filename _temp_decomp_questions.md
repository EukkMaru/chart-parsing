# Temp handoff: questions for the next decomp session (2026-08-07)

Temporary note, not normative. Delete once the questions are answered or
recorded as claims. Context: a six-domain faithfulness audit of the viewer
against the 2ce3ff6 claims (18 confirmed divergences, GitHub issues #6-#11),
an owner footage-verification pass over targets selected by a corpus trigger
scan of all 1,837 reference `_03` charts, and a fix batch applied to
`scripts/c2s-viewer.html`. Issue #12 is the standing ledger of viewer behavior
that is implemented from inference or observation and is waiting for binary
backing — it is a "probably right, please verify" list, not a bug list.

## The headline question: DCM query scope

`FUN`-level question: what exact times and entities does the source-order DCM
projection-factor query consume, at materialization and at render?

Evidence that the current model (query at the note's own scrolled event time,
first covering interval in source order, +1 shift) is incomplete, from
`2442_03` m49-50 (owner-verified footage):

- `DCM 49 193 144 100.0` covers exactly the three earlier hold ends of the
  four `HLD 49 192 ...` records (durations 48/96/144/192; the fourth ends at
  the interval's edge). Footage and viewer agree the holds vanish one by one.
- The full-width slide chain (`SXC 50 0 0 16 384 6 4 SLD DW`) begins 47 ticks
  AFTER the interval ends. Under the current model it takes no factor — yet
  the owner observed the viewer deferring its appearance while footage shows
  it approaching normally. The divergence mechanism is NOT located; it may
  involve materialization eligibility being factor-scaled differently from
  drawn projection, or a different query anchor entirely. Reproduce before
  theorizing: the observation predates the endpoint-key/cap fixes.

## Verification requests for issue #12 (implemented, probably right)

1. Presentation-side consumers of the endpoint SLA tag (+0x80) for types 1,
   10, 13 — the viewer now projects every sustain endpoint through its own
   key, recovered only for type 2 and ALD.
2. The type-5/6 (air hold/slide) vertical transform — the viewer now anchors
   all air heights at authored value 5 = ordinary air height, recovered only
   for ALD (stream-1 threshold 15.574 = (5-1)*3.8934999).
3. Air-arrow lean construction (`render.air`) — ADL/ADR lean was flipped on
   one footage observation (2517_03 m71).
4. The lazy slide endpoint allocator's width argument — post-shrink width
   kept on two observations (2905_03 m11, 2106_03 m33) against one older
   contrary one (2891_03 bar 53).
5. The meter/grid vector builder's field consumers — unit-then-count is still
   corpus-inferred.

## Enumerations that would close standing blind spots

- The 91-entry descriptor registry, and specifically the exact 45 group-3
  `T_` statistics spellings — the viewer suppresses `T_*` by wildcard today,
  which would hide an unregistered spelling (the SLP lesson, issue #11).
- Family-specific secondary match rejection (orphan AIR/AHD/ASD records):
  what exactly fails to construct, so the viewer can stop drawing orphans.

## Owner-verified since your last session (for calibration)

The endpoint-key and cap-transform divergences were confirmed in footage
(0961_03 m105, 2310_03 m106, 2898_03 m27) and are fixed; AHX was absent from
the viewer's family table (issue #6, fixed, 207 reference charts affected);
the ALD per-chart height rescale was confirmed at 2121_03 and replaced by the
fixed value-5 anchor. The slide presentation-classes and scroll-transform
recoveries survived a 509,056-vector differential test unchanged.
