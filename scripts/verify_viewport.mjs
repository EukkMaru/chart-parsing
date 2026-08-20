// Viewport equivalence gate: the corridor-loaded chart and the directly
// parsed chart must produce IDENTICAL canvas call streams across the whole
// timeline (field, bird's-eye, and strip). This is draw-call-level, one
// step below pixels: same calls + same args = same pixels.
// Run: node scripts/verify_viewport.mjs [charts...]
import { readFileSync, existsSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";
import { createHash } from "node:crypto";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "c2s-viewer.html"), "utf8");
const script = html.split(/<script>/)[1].split(/<\/script>/)[0];
const lines = script.split("\n");
const open = lines.findIndex(l => l.trim() === "(() => {");
const close = lines.length - 1 - [...lines].reverse().findIndex(l => l.trim() === "})();");
const body = lines.slice(open + 1, close)
  .filter(l => l.trim() !== '"use strict";').join("\n");

// ---- recording canvas context ----
const log = [];
let recording = false;
const fmtArg = a => typeof a === "number" ? (Math.round(a * 1e6) / 1e6).toString()
  : typeof a === "string" ? a : String(a);
function recordingContext(canvasId) {
  const gradient = { addColorStop: (...a) => { if (recording) log.push(`${canvasId}.grad:${a.map(fmtArg)}`); } };
  return new Proxy({}, {
    get: (t, key) => {
      if (key === "measureText") return () => ({ width: 10 });
      if (key === "createLinearGradient" || key === "createRadialGradient") {
        return (...a) => { if (recording) log.push(`${canvasId}.${key}:${a.map(fmtArg)}`); return gradient; };
      }
      if (key === "getImageData") return () => ({ data: new Uint8ClampedArray(4) });
      if (typeof key !== "string") return undefined;
      return (...a) => { if (recording) log.push(`${canvasId}.${key}:${a.map(fmtArg)}`); };
    },
    set: (t, key, v) => {
      if (recording && typeof key === "string") log.push(`${canvasId}.set ${key}=${fmtArg(v)}`);
      return true;
    },
  });
}
const noop = () => {};
function stubElement(id) {
  return {
    style: {}, classList: { add: noop, remove: noop, toggle: noop, contains: () => false },
    setAttribute: noop, getAttribute: () => null, addEventListener: noop,
    removeEventListener: noop, appendChild: noop, focus: noop, click: noop,
    getContext: () => recordingContext(id),
    getBoundingClientRect: () => ({ left: 0, top: 0, width: 1280, height: 720 }),
    value: "0", textContent: "", innerHTML: "", max: "1", files: [],
    width: 1280, height: 720, hidden: false, dataset: {},
    parentElement: { style: {}, addEventListener: noop,
      getBoundingClientRect: () => ({ width: 1280, height: 720 }) },
  };
}
const elements = new Map();
const documentStub = {
  getElementById: id => {
    if (!elements.has(id)) elements.set(id, stubElement(id));
    return elements.get(id);
  },
  createElement: () => stubElement("dyn"),
  createTextNode: () => ({}),
  addEventListener: noop, removeEventListener: noop,
  body: stubElement("body"), documentElement: stubElement("html"),
};
const windowStub = {
  addEventListener: noop, removeEventListener: noop,
  requestAnimationFrame: noop, devicePixelRatio: 1,
  location: { search: "", href: "" },
};
const harness = new Function("document", "window", "requestAnimationFrame",
  "navigator", "performance", "XMLHttpRequest", "localStorage", "location",
  "fetch", "getComputedStyle",
  `${body}
   return {
     load, sync, setNow: v => { now = v; }, getChart: () => chart,
     loadDirect: text => {
       chart = parseC2s(text);
       computeTrims(chart.notes);
       markSlideBoundaries(chart.notes);
       chart.guideEvents = buildGuideEvents(chart);
       maxH = Math.max(1, ...chart.notes.map(n => Math.max(n.h0 || 0, n.h1 || 0)));
       now = 0;
     },
     setEye: (on, speed) => { eyeOn = on; if (speed) eyeSpeed = speed; },
   };`)(
  documentStub, windowStub, noop, { userAgent: "vp" }, { now: () => 0 },
  function X() { this.open = noop; this.send = noop; this.addEventListener = noop; },
  { getItem: () => null, setItem: noop }, windowStub.location,
  () => Promise.reject(new Error("no network")), () => ({ getPropertyValue: () => "" }));

function callStream(loader, text, steps, keepStep) {
  loader(text);
  harness.setEye(true, 1.0);
  const duration = harness.getChart().duration || 1;
  const hashes = [];
  let kept = null;
  for (let i = 0; i <= steps; i++) {
    harness.setNow(duration * (i / steps));
    log.length = 0;
    recording = true;
    harness.sync();
    recording = false;
    if (keepStep === i) kept = log.slice();
    hashes.push(createHash("sha256").update(log.join("\n")).digest("hex"));
  }
  return { hashes, kept };
}

const home = process.env.HOME;
const defaults = [
  `${home}/Downloads/music/music1086/1086_03.c2s`,
  `${home}/Downloads/music/music2582/2582_03.c2s`,
  `${home}/Downloads/music/music2194/2194_03.c2s`,
  `${home}/Downloads/music/music8302/8302_05.c2s`,
  `${home}/Downloads/music/music8135/8135_05.c2s`,
  `${home}/Downloads/music/music2493/2493_02.c2s`,
  `${home}/Downloads/music/music8315/8315_05.c2s`,
  `${home}/Downloads/music/music2699/2699_03.c2s`,
];
const files = process.argv.slice(2).length ? process.argv.slice(2)
  : defaults.filter(existsSync);
let fail = 0;
for (const file of files) {
  const id = file.split("/").pop();
  try {
    const text = readFileSync(file, "latin1");
    const steps = Number(process.env.VP_STEPS || 240);   // whole-chart sweep
    const corridor = callStream(harness.load, text, steps).hashes;
    const direct = callStream(harness.loadDirect, text, steps).hashes;
    let diverged = -1;
    for (let i = 0; i < corridor.length; i++) {
      if (corridor[i] !== direct[i]) { diverged = i; break; }
    }
    if (diverged >= 0) {
      fail++;
      console.log(`FAIL ${id}: draw streams diverge at step ${diverged}/${steps}`);
      const a = callStream(harness.load, text, steps, diverged).kept;
      const b = callStream(harness.loadDirect, text, steps, diverged).kept;
      for (let j = 0; j < Math.max(a.length, b.length); j++) {
        if (a[j] !== b[j]) {
          for (let k = Math.max(0, j - 2); k < j + 4; k++) {
            console.log(`   ${k} | ${(a[k] ?? "").slice(0, 68).padEnd(68)} | ${(b[k] ?? "").slice(0, 68)}`);
          }
          break;
        }
      }
    } else if (files.length <= 12) {
      console.log(`ok   ${id} (${steps + 1} frames identical)`);
    }
  } catch (error) {
    fail++;
    console.log(`ERR  ${id}: ${error.message.split("\n")[0]}`);
  }
}
console.log(fail ? `${fail} failures` : `${files.length} charts: viewport call streams identical`);
process.exit(fail ? 1 : 0);
