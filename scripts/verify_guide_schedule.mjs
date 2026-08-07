// Fixture test for the guide-click schedule. Extracts the shipped
// buildGuideEvents and collapsedGuides bodies from scripts/c2s-viewer.html and
// checks the owner-specified cue mapping (2026-08-07) and the merge-window
// collapse. Run: node scripts/verify_guide_schedule.mjs

import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "c2s-viewer.html"), "utf8");

function extract(name) {
  const m = html.match(new RegExp(`  function ${name}\\([^)]*\\) \\{[\\s\\S]*?\\n  \\}`));
  if (!m) throw new Error(`cannot extract ${name}`);
  return m[0];
}

// The merge window is a fixed constant in the shipped file; read it from
// there so the fixtures below verify the value that actually runs.
const mergeDecl = html.match(/const GUIDE_MERGE_MS = (\d+);/);
if (!mergeDecl) throw new Error("cannot find GUIDE_MERGE_MS");
const MERGE_MS = Number(mergeDecl[1]);

const harness = new Function(`
  let chart = null;
  const GUIDE_MERGE_MS = ${MERGE_MS};
  let guideCache = { win: -1, src: null, list: [] };
  ${extract("buildGuideEvents")}
  ${extract("collapsedGuides")}
  return {
    mergeMs: GUIDE_MERGE_MS,
    build: c => buildGuideEvents(c),
    collapse: events => {
      chart = { guideEvents: events };
      guideCache = { win: -1, src: null, list: [] };
      return collapsedGuides();
    },
  };
`)();

let checks = 0, failures = 0;
function expect(label, got, want) {
  checks++;
  const g = JSON.stringify(got), w = JSON.stringify(want);
  if (g !== w) { failures++; console.error(`FAIL ${label}: got ${g}, want ${w}`); }
}

// ---- cue mapping fixtures ----------------------------------------------

const N = (kind, cmd, time, extra = {}) => ({ kind, cmd, time, endTime: time, ...extra });

const chart = {
  notes: [
    N("tap", "TAP", 100),
    N("tap", "CHR", 110),
    N("tap", "FLK", 120),
    N("tap", "MNE", 200),                                    // silent
    N("hold", "HLD", 300, { endTime: 500 }),                 // root + end
    N("slide", "SXC", 600, { endTime: 640, isRoot: true, isFinal: false,
                             marker: false, endpointResource: false }),
    N("slide", "SXC", 640, { endTime: 680, isRoot: false, isFinal: false,
                             marker: false, endpointResource: false }),  // shaper: silent
    N("slide", "SXD", 680, { endTime: 700, isRoot: false, isFinal: false,
                             marker: true, endpointResource: true }),    // marked ckpt
    N("slide", "SXC", 700, { endTime: 800, isRoot: false, isFinal: true,
                             marker: false, endpointResource: true }),   // forced final
    N("air", "AIR", 900),
    N("ahold", "AHD", 950, { endTime: 990, isAction: false }),           // sustain: silent
    N("ahold", "AHX", 950, { endTime: 1000, isAction: true }),           // air action
    N("aslide", "ASC", 1040, { endTime: 1080, isAction: false }),        // shaper: silent
    N("aslide", "ASD", 1080, { endTime: 1100, isAction: true }),         // air action
    N("asolid", "ASO", 1150, { endTime: 1250 }),                          // trace: silent
    N("ald", "ALD", 1190, { endTime: 1230 }),        // note itself silent; samples cue
    N("region", "SLA", 1300),                                             // silent
  ],
  aldSamples: [{ time: 1202 }, { time: 1200 }, { time: 1201 }],           // unsorted on purpose
};

expect("cue mapping + sort order", harness.build(chart),
  [100, 110, 120, 300, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1201, 1202]);

// ---- collapse fixtures --------------------------------------------------

expect("shipped merge window", harness.mergeMs, 8);
// chain suppression relative to the last EMITTED click
expect("merge chain", harness.collapse([0, 5, 9, 20]), [0, 9, 20]);
// exact duplicates always collapse
expect("exact dups", harness.collapse([100, 100, 100, 140]), [100, 140]);
// interval-1 crush cadence at 219 BPM (~2.28ms) is rate-limited to ~1/window
const crush = Array.from({ length: 50 }, (_, i) => i * 2.283);
const merged = harness.collapse(crush);
expect("crush rate cap",
  merged.every((t, i) => i === 0 || t - merged[i - 1] > MERGE_MS), true);
expect("crush first kept", merged[0], 0);
// authored rhythm above the window is untouched
expect("rhythm preserved", harness.collapse([0, 100, 200, 300]), [0, 100, 200, 300]);

console.log(`${checks} checks, ${failures} failures`);
process.exit(failures ? 1 : 0);
