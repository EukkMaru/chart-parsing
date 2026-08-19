// Differential audit: the viewer's group-1 parser, schedule builder, and
// chart-region transforms against independent references transcribed from
// spec/c2s.md and spec/timing.md. Extracts the live function bodies from
// scripts/c2s-viewer.html so the test exercises shipped code, not a copy.
// Run: node scripts/verify_scroll_schedule.mjs
//
// Scope (GitHub issue 1 close-out, VIEWER_ROADMAP queue item 1):
//   - actual STP/SFL/SFE/SLP/DCM/CLK parse-and-build separation
//   - STP/SFL/SLP additive interval transform semantics
//   - DCM source-order projection-factor query
//   - SLA tag selection (integer-span and tolerant float-span)
// Out of scope, reported separately: parse-time numeric conversion, wrapped
// 32-bit minor+duration on hostile inputs, camera calibration.

import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "c2s-viewer.html"), "utf8");

// ---- extract the shipped implementations -------------------------------

function extractFunction(name) {
  const m = html.match(new RegExp(`  function ${name}\\([^)]*\\) \\{[\\s\\S]*?\\n  \\}`));
  if (!m) throw new Error(`cannot extract ${name}`);
  return m[0];
}
function extractSelectSlaKey() {
  const m = html.match(/const selectSlaKey = \(p, lane, width[\s\S]*?\n    \};/);
  if (!m) throw new Error("cannot extract selectSlaKey");
  return m[0];
}
function extractMatch(pattern, label) {
  const m = html.match(pattern);
  if (!m) throw new Error(`cannot extract ${label}`);
  return m[0];
}

// The functions close over `show`, `chart`, and `slas`; supply those.
const shell = `
  ${extractMatch(/  const REGION = \{[\s\S]*?\n  \};/, "REGION")}
  ${extractMatch(/  const c2sInt =[^;]*;/, "c2sInt")}
  ${extractMatch(/  const c2sFloat = value => \{[\s\S]*?\n  \};/, "c2sFloat")}
  ${extractFunction("snapshotMsvcSort")}
  ${extractFunction("parseProjectionRecord")}
  ${extractFunction("buildProjectionSchedules")}
  const show = { applySlp: true };
  let chart = null;
  let slas = [];
  ${extractFunction("scrolledTime")}
  ${extractFunction("scrollFactor")}
  ${extractSelectSlaKey()}
  return {
    scrolledTime,
    scrollFactor,
    selectSlaKey: (...a) => selectSlaKey(...a),
    parseProjectionRecord,
    buildProjectionSchedules,
    setChart: c => { chart = c; },
    setSlas: s => { slas = s; },
  };
`;
const viewer = new Function(shell)();

// ---- reference implementations, from spec/timing.md --------------------

// "For a forward query, start with the target milliseconds and visit every
//  sorted interval whose scheduled start is strictly before target. When
//  `from` is strictly before the interval end, add
//      (min(target, interval_end) - max(from, interval_start))
//          * (interval_factor - 1)
//  when the overlap endpoints are ordered. Every overlapping interval
//  contributes; this is not last-wins. A missing key or backwards
//  materialization query leaves the target unchanged."
function refScrolledTime(intervalsByKey, targetMs, fromMs, key) {
  if (targetMs < fromMs) return targetMs;
  const list = intervalsByKey.get(key || 0);
  if (!list) return targetMs;
  let out = targetMs;
  for (const iv of [...list].sort((a, b) => a.start - b.start)) {
    if (!(iv.start < targetMs)) continue;   // strictly before target
    if (!(fromMs < iv.end)) continue;       // from strictly before end
    const lo = Math.max(fromMs, iv.start);
    const hi = Math.min(targetMs, iv.end);
    if (hi > lo) out += (hi - lo) * (iv.factor - 1);
  }
  return out;
}

// "DCM intervals remain in source order. Shift the query by exactly 1.0F,
//  stop at the first interval whose start is later than the shifted value,
//  and return the first nonzero factor whose end is strictly later. The
//  fallback is 1.0F."
function refScrollFactor(dcmSourceOrder, targetMs) {
  const q = targetMs + 1.0;
  for (const iv of dcmSourceOrder) {
    if (iv.start > q) break;
    if (iv.end > q && iv.factor !== 0) return iv.factor;
  }
  return 1.0;
}

// "Both region queries add exact float 1/192 to chart position, use a
//  half-open time interval, require full lane-span containment, and retain
//  only a strictly greater tag starting from zero. The AirLadder float-span
//  variant additionally uses initialized -0.00001F/+0.00001F lane
//  tolerances."  (1/192 position scalar = 0.5 tick on the fixed 384 grid.)
function refSelectSlaKey(slaRegions, p, lane, width, floatSpan = false) {
  const q = p + 0.5;
  let key = 0;
  for (const s of slaRegions) {
    if (!(s.tag > key)) continue;
    if (!(q >= s.p && q < s.p + s.dur)) continue;
    const tol = floatSpan ? 0.00001 : 0;
    if (lane < s.lane - tol) continue;
    if (lane + width > s.lane + s.width + tol) continue;
    key = s.tag;
  }
  return key;
}

// Group-1 descriptor/handler table transcribed independently from
// spec/c2s.md. SFE is registered but rejected; CLK is a point record; DCM is
// factor-only. Only STP/SFL/SLP can enter a keyed interval set.
const REF_GROUP1 = {
  STP: { disposition: "keyed", factor: false, key: false },
  SFL: { disposition: "keyed", factor: true, key: false },
  SFE: { disposition: "rejected", factor: true, key: false },
  SLP: { disposition: "keyed", factor: true, key: true },
  DCM: { disposition: "factor", factor: true, key: false },
  CLK: { disposition: "point", factor: false, key: false },
};

const refInt = value => Number.isNaN(Number.parseInt(value, 10))
  ? 0 : Number.parseInt(value, 10);
const refFloat = value => {
  const parsed = Number.parseFloat(value);
  return Number.isNaN(parsed) ? 0 : parsed;
};

function parseGroup1With(parser, text) {
  const records = [];
  for (const line of text.split(/\r?\n/)) {
    const tokens = line.split(/[\t ]+/).filter(Boolean);
    if (!tokens.length) continue;
    const record = parser(tokens[0], tokens.slice(1), records.length);
    if (record) records.push(record);
  }
  return records;
}

function refParseProjectionRecord(cmd, fields, sourceOrder) {
  const def = REF_GROUP1[cmd];
  if (!def) return null;
  return {
    cmd, disposition: def.disposition, sourceOrder,
    m: refInt(fields[0]), t: refInt(fields[1]),
    dur: def.disposition === "point" ? 0 : refInt(fields[2]),
    factor: def.factor ? refFloat(fields[3]) : null,
    key: def.key ? refInt(fields[4]) : 0,
  };
}

// Schedule separation is independent of BPM. Materialize the exact parsed
// chart positions into an identity time domain so this test isolates the
// parser/builder boundary while the existing transform sweep covers timing.
function materializeProjectionRecords(records, resolution = 384) {
  for (const record of records) {
    record.p = record.m * resolution + record.t;
    record.time = record.p;
    record.endTime = record.dur > 0 ? record.p + record.dur : record.p;
  }
  records.sort((left, right) => left.time - right.time);
  return records;
}

function refBuildProjectionSchedules(records) {
  const regionsByKey = new Map();
  for (const record of records) {
    if (record.disposition !== "keyed" || record.endTime <= record.time) continue;
    const key = record.key || 0;
    const factor = record.cmd === "STP" ? 0 : record.factor;
    if (!regionsByKey.has(key)) regionsByKey.set(key, []);
    regionsByKey.get(key).push({
      p: record.p, start: record.time, end: record.endTime, factor,
    });
  }
  for (const list of regionsByKey.values()) {
    list.sort((left, right) => left.p - right.p);
  }
  const dcm = records
    .filter(record => record.disposition === "factor")
    .sort((left, right) => left.sourceOrder - right.sourceOrder)
    .map(record => ({
      start: record.time, end: record.endTime, factor: record.factor,
    }));
  return { regionsByKey, dcm };
}

function canonicalSchedule(schedule) {
  return JSON.stringify({
    regionsByKey: [...schedule.regionsByKey.entries()]
      .sort((left, right) => left[0] - right[0]),
    dcm: schedule.dcm,
  });
}

// ---- comparison harness -------------------------------------------------

let checks = 0, failures = 0;
function expectEqual(label, got, want) {
  checks++;
  const same = Number.isNaN(want) ? Number.isNaN(got) : Math.abs(got - want) < 1e-9;
  if (!same) {
    failures++;
    console.error(`FAIL ${label}: viewer=${got} reference=${want}`);
  }
}

function expectScheduleEqual(label, got, want) {
  checks++;
  const gotText = canonicalSchedule(got);
  const wantText = canonicalSchedule(want);
  if (gotText !== wantText) {
    failures++;
    console.error(`FAIL ${label}: viewer=${gotText} reference=${wantText}`);
  }
}

let mutationChecks = 0;
function expectMutationDetected(label, mutant, reference) {
  checks++;
  mutationChecks++;
  if (canonicalSchedule(mutant) === canonicalSchedule(reference)) {
    failures++;
    console.error(`FAIL mutation ${label}: deliberate builder bug was not detected`);
  }
}

function runScrolled(byKey, target, from, key) {
  viewer.setChart({ regionsByKey: byKey, dcm: [] });
  expectEqual(`scrolledTime(t=${target},f=${from},k=${key})`,
    viewer.scrolledTime(target, from, key),
    refScrolledTime(byKey, target, from, key));
}
function runFactor(dcm, target) {
  viewer.setChart({ regionsByKey: new Map([[0, []]]), dcm });
  expectEqual(`scrollFactor(t=${target})`,
    viewer.scrollFactor(target), refScrollFactor(dcm, target));
}
function runSla(regions, p, lane, width, floatSpan) {
  viewer.setSlas(regions);
  expectEqual(`sla(p=${p},l=${lane},w=${width},fs=${!!floatSpan})`,
    viewer.selectSlaKey(p, lane, width, floatSpan),
    refSelectSlaKey(regions, p, lane, width, floatSpan));
}

// ---- synthetic edge vectors ---------------------------------------------

const IV = (start, end, factor) => ({ start, end, factor });
const oneKey = list => new Map([[0, list]]);

// boundary strictness: start == target, from == end
runScrolled(oneKey([IV(100, 200, 3)]), 100, 0, 0);     // start==target: excluded
runScrolled(oneKey([IV(100, 200, 3)]), 100.001, 0, 0); // just inside
runScrolled(oneKey([IV(100, 200, 3)]), 500, 200, 0);   // from==end: excluded
runScrolled(oneKey([IV(100, 200, 3)]), 500, 199.999, 0);
// partial overlaps from both sides, containment, exact cover
runScrolled(oneKey([IV(100, 200, 3)]), 150, 120, 0);
runScrolled(oneKey([IV(100, 200, 3)]), 150, 0, 0);
runScrolled(oneKey([IV(100, 200, 3)]), 500, 150, 0);
runScrolled(oneKey([IV(100, 200, 3)]), 200, 100, 0);
// accumulation across overlapping same-key intervals (not last-wins)
runScrolled(oneKey([IV(100, 300, 2), IV(200, 400, 5)]), 500, 0, 0);
runScrolled(oneKey([IV(100, 300, 2), IV(100, 300, 0.5)]), 250, 120, 0);
// STP dead stop (factor 0), identity factor 1, huge wall factor
runScrolled(oneKey([IV(100, 160, 0)]), 400, 0, 0);
runScrolled(oneKey([IV(100, 160, 1)]), 400, 0, 0);
runScrolled(oneKey([IV(100, 158, 6553.600098)]), 400, 0, 0);
// backwards query, missing key, empty schedule
runScrolled(oneKey([IV(100, 200, 3)]), 50, 100, 0);
runScrolled(oneKey([IV(100, 200, 3)]), 400, 0, 7);
runScrolled(new Map(), 400, 0, 0);
// keyed separation
runScrolled(new Map([[0, [IV(0, 100, 2)]], [1, [IV(0, 100, 9)]]]), 300, 0, 1);

// DCM: shifted-query boundaries (q = t + 1)
runFactor([IV(101, 300, 2)], 100);   // start == q: visited
runFactor([IV(102, 300, 2)], 100);   // start >  q: break -> 1
runFactor([IV(0, 101, 2)], 100);     // end == q: not strictly later -> 1
runFactor([IV(0, 101.5, 2)], 100);
// zero factor skipped, later nonzero wins
runFactor([IV(0, 500, 0), IV(0, 500, 1.5)], 100);
// source-order early exit can skip a later matching entry
runFactor([IV(200, 900, 2), IV(0, 900, 3)], 100);  // first start(200) > 101: break -> 1
runFactor([IV(0, 900, 3), IV(200, 900, 2)], 100);  // reversed source order -> 3
// nested/overlapping in source order: first match wins
runFactor([IV(0, 900, 1.5), IV(50, 600, 0.75)], 100);

// SLA: query shift and half-open bounds (q = p + 0.5)
const SLA = (p, lane, width, dur, tag) => ({ p, lane, width, dur, tag });
runSla([SLA(1000, 4, 4, 100, 3)], 999.5, 4, 4);   // q == region start: inside
runSla([SLA(1000, 4, 4, 100, 3)], 999.4, 4, 4);   // q just below: outside
runSla([SLA(1000, 4, 4, 100, 3)], 1099.5, 4, 4);  // q == region end: outside
runSla([SLA(1000, 4, 4, 100, 3)], 1099.4, 4, 4);  // q just inside end
// full lane containment, both edges
runSla([SLA(0, 4, 4, 1000, 3)], 100, 4, 4);
runSla([SLA(0, 4, 4, 1000, 3)], 100, 3, 4);
runSla([SLA(0, 4, 4, 1000, 3)], 100, 5, 4);
runSla([SLA(0, 4, 4, 1000, 3)], 100, 4, 5);
// greatest positive tag among overlaps; zero/negative tags never selected
runSla([SLA(0, 0, 16, 1000, 2), SLA(0, 0, 16, 1000, 7), SLA(0, 0, 16, 1000, 5)], 100, 2, 4);
runSla([SLA(0, 0, 16, 1000, 0), SLA(0, 0, 16, 1000, -3)], 100, 2, 4);
// float-span tolerance boundaries
runSla([SLA(0, 4, 4, 1000, 3)], 100, 3.999995, 4, true);
runSla([SLA(0, 4, 4, 1000, 3)], 100, 3.99997, 4, true);
runSla([SLA(0, 4, 4, 1000, 3)], 100, 4, 4.000005, true);

// Builder mutation sentinels required by issue 17. They prove the comparison
// rejects the three historical/credible category leaks instead of merely
// comparing a green implementation to itself.
const builderSentinelText = [
  "STP\t0\t0\t96",
  "SFL\t0\t96\t96\t2",
  "SFE\t0\t192\t96\t3",
  "SLP\t0\t288\t96\t0.5\t7",
  "DCM\t1\t0\t96\t4",
  "CLK\t1\t96",
  "SLP\t2\t0\t0\t9\t8",
].join("\n");
const sentinelActualRecords = materializeProjectionRecords(
  parseGroup1With(viewer.parseProjectionRecord, builderSentinelText));
const sentinelReferenceRecords = materializeProjectionRecords(
  parseGroup1With(refParseProjectionRecord, builderSentinelText));
const sentinelActual = viewer.buildProjectionSchedules(sentinelActualRecords);
const sentinelReference = refBuildProjectionSchedules(sentinelReferenceRecords);
expectScheduleEqual("group1 builder sentinel", sentinelActual, sentinelReference);

function copySchedule(schedule) {
  return {
    regionsByKey: new Map([...schedule.regionsByKey.entries()].map(
      ([key, list]) => [key, list.map(interval => ({ ...interval }))])),
    dcm: schedule.dcm.map(interval => ({ ...interval })),
  };
}
const dcmLeak = copySchedule(sentinelActual);
if (!dcmLeak.regionsByKey.has(0)) dcmLeak.regionsByKey.set(0, []);
dcmLeak.regionsByKey.get(0).push({ p: 384, ...dcmLeak.dcm[0] });
expectMutationDetected("DCM enters key-0 schedule", dcmLeak, sentinelReference);

for (const [command, label] of [["SFE", "SFE accepted"], ["CLK", "CLK interval"]]) {
  const leak = copySchedule(sentinelActual);
  const record = sentinelActualRecords.find(candidate => candidate.cmd === command);
  if (!leak.regionsByKey.has(0)) leak.regionsByKey.set(0, []);
  leak.regionsByKey.get(0).push({
    p: record.p, start: record.time,
    end: record.endTime > record.time ? record.endTime : record.time + 1,
    factor: record.factor ?? 0,
  });
  expectMutationDetected(label, leak, sentinelReference);
}

// ---- corpus sweep -------------------------------------------------------

import { readdirSync, existsSync } from "node:fs";
const musicRoot = process.env.C2S_MUSIC_ROOT ?? join(here, "..", "music");
let corpusCharts = 0, corpusQueries = 0;
let builderCharts = 0, builderRecords = 0;
if (existsSync(musicRoot)) {
  const R = 384;
  for (const dir of readdirSync(musicRoot)) {
    const chartDir = join(musicRoot, dir);
    let files = [];
    try { files = readdirSync(chartDir).filter(f => f.endsWith(".c2s")); }
    catch { continue; }
    for (const file of files) {
      const text = readFileSync(join(chartDir, file), "utf8");
      const hasGroup1 = /^(STP|SFL|SFE|SLP|DCM|CLK)[\t ]/m.test(text);
      const hasSla = /^SLA[\t ]/m.test(text);
      if (!hasGroup1 && !hasSla) continue;
      corpusCharts++;
      if (hasGroup1) {
        const actualRecords = materializeProjectionRecords(
          parseGroup1With(viewer.parseProjectionRecord, text));
        const referenceRecords = materializeProjectionRecords(
          parseGroup1With(refParseProjectionRecord, text));
        const actualSchedule = viewer.buildProjectionSchedules(actualRecords);
        const referenceSchedule = refBuildProjectionSchedules(referenceRecords);
        expectScheduleEqual(`${dir}/${file} group1 builder`,
                            actualSchedule, referenceSchedule);
        builderCharts++;
        builderRecords += actualRecords.length;
      }
      // minimal parse: BPM schedule + region records, mirroring the viewer's
      // reading of the recovered field shapes
      const bpm = [];
      const regs = [];
      const dcm = [];
      const slaRegions = [];
      let order = 0;
      for (const line of text.split(/\r?\n/)) {
        const f = line.split(/[\t ]+/).filter(Boolean);
        if (!f.length) continue;
        const p = () => (+f[1] || 0) * R + (+f[2] || 0);
        if (f[0] === "BPM") bpm.push([p(), +f[3] || 120]);
        else if (f[0] === "STP") regs.push({ p: p(), dur: +f[3] || 0, factor: 0, key: 0 });
        else if (f[0] === "SFL") regs.push({ p: p(), dur: +f[3] || 0, factor: +f[4] || 0, key: 0 });
        else if (f[0] === "SLP") regs.push({ p: p(), dur: +f[3] || 0, factor: +f[4] || 0, key: +f[5] || 0 });
        else if (f[0] === "DCM") dcm.push({ p: p(), dur: +f[3] || 0, factor: +f[4] || 0, order: order++ });
        else if (f[0] === "SLA") slaRegions.push({ p: p(), lane: +f[3] || 0, width: +f[4] || 1, dur: +f[5] || 0, tag: +f[6] || 0 });
      }
      bpm.sort((a, b) => a[0] - b[0]);
      if (!bpm.length) bpm.push([0, 240]);
      const seg = [];
      let acc = 0;
      for (let i = 0; i < bpm.length; i++) {
        const msPerTick = 240000 / (bpm[i][1] * R);
        if (i > 0) acc += (bpm[i][0] - seg[i - 1].p) * seg[i - 1].msPerTick;
        seg.push({ p: bpm[i][0], ms: acc, msPerTick });
      }
      const timeAt = p => {
        let s = seg[0];
        for (let i = 1; i < seg.length && seg[i].p <= p; i++) s = seg[i];
        return s.ms + (p - s.p) * s.msPerTick;
      };
      const byKey = new Map();
      for (const r of regs) {
        if (r.dur <= 0) continue;
        const iv = IV(timeAt(r.p), timeAt(r.p + r.dur), r.factor);
        if (iv.end <= iv.start) continue;
        if (!byKey.has(r.key)) byKey.set(r.key, []);
        byKey.get(r.key).push(iv);
      }
      for (const list of byKey.values()) list.sort((a, b) => a.start - b.start);
      const dcmIv = dcm
        .sort((a, b) => a.order - b.order)
        .map(r => IV(timeAt(r.p), timeAt(r.p + r.dur), r.factor));

      const spanEnd = timeAt(Math.max(0, ...regs.map(r => r.p + r.dur),
                                      ...dcm.map(r => r.p + r.dur)) + R * 4);
      const keys = [0, ...byKey.keys()];
      for (let i = 0; i <= 200; i++) {
        const target = (spanEnd * i) / 200;
        const from = target * ((i * 7919) % 100) / 100; // deterministic spread
        for (const key of keys) {
          runScrolled(byKey, target, from, key);
          corpusQueries++;
        }
        runFactor(dcmIv, target);
        corpusQueries++;
      }
      for (let p = 0; p < Math.max(...slaRegions.map(s => s.p + s.dur), 1); p += 97) {
        for (const [lane, width] of [[0, 4], [4, 4], [11, 4], [2, 12], [7, 2]]) {
          runSla(slaRegions, p, lane, width, false);
          runSla(slaRegions, p + 0.25, lane, width, true);
          corpusQueries += 2;
        }
      }
    }
  }
}

console.log(`charts with regions: ${corpusCharts}`);
console.log(`group1 builder charts: ${builderCharts} (${builderRecords} records)`);
console.log(`mutation sentinels:   ${mutationChecks}`);
console.log(`total comparisons:   ${checks} (${corpusQueries} corpus, ${checks - corpusQueries} synthetic)`);
console.log(failures === 0 ? "PASS: viewer matches the spec reference on every vector"
                           : `FAIL: ${failures} divergences`);
process.exit(failures === 0 ? 0 : 1);
