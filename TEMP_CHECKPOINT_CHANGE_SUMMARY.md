# Temporary checkpoint change summary

This temporary note summarizes changes from the first gameplay checkpoint
(`c2fb426`) through the completed checkpoint (`f1da97c`). It is not a normative
specification; use `research/STATUS.md` and the linked claims/specs for details.

- Expanded the clean-room C++ reconstruction from the initial gameplay map to
  closed `.c2s` ingestion, timing/materialization, input, note-family,
  candidate, judgement, result-routing, interaction, reset, and exit behavior.
- Corrected edge cases found by proof-checking, including parser compatibility,
  projection/SLA behavior, lane widths and result identifiers, exact BPM sort,
  malformed float conversion, and wrapped Air-family tick/cursor arithmetic.
- Added or revised the evidence claims and normative specs, then completed a
  fresh independent audit. All 28 gameplay coverage rows are now verified.
- Increased the CTest suite from 29 to 34 targets; the completed checkpoint
  passes all 34 tests, the coverage validator, and a separate UBSan run.
- Preserved the intermediate remote commit that added
  `scripts/c2s-viewer.html`, an offline chart/playfield inspection tool.

Overall diff before this note: 87 files changed, with 6,325 insertions and 633
deletions.
