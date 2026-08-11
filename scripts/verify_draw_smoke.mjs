// Draw-loop smoke test: run the actual viewer script under a stub DOM/canvas,
// load real charts, and step playback across their full duration so every
// draw path executes. Catches closure/scope crashes (like drawHeavenHold's
// depthAt) that no parser-only audit can see.
// Run: node scripts/verify_draw_smoke.mjs [chart.c2s ...]
// With no arguments it covers the known rare-path charts plus dense WE picks.
import { readFileSync, existsSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "c2s-viewer.html"), "utf8");
const script = html.split(/<script>/)[1].split(/<\/script>/)[0];
const lines = script.split("\n");
// strip the IIFE wrapper: opening "(() => {" + "use strict", closing "})();"
const openIndex = lines.findIndex(l => l.trim() === "(() => {");
const closeIndex = lines.length - 1 - [...lines].reverse().findIndex(l => l.trim() === "})();");
if (openIndex < 0 || closeIndex <= openIndex) throw new Error("cannot unwrap viewer IIFE");
const body = lines.slice(openIndex + 1, closeIndex)
  .filter(l => l.trim() !== '"use strict";').join("\n");

// ---- stub DOM ------------------------------------------------------------
const noop = () => {};
function stubContext() {
  const gradient = { addColorStop: noop };
  return new Proxy({}, {
    get: (target, key) => {
      if (key === "measureText") return () => ({ width: 10 });
      if (key === "createLinearGradient" || key === "createRadialGradient") return () => gradient;
      if (key === "getImageData") return () => ({ data: new Uint8ClampedArray(4) });
      return typeof key === "string" ? noop : undefined;
    },
    set: () => true,
  });
}
function stubElement() {
  const el = {
    style: {}, classList: { add: noop, remove: noop, toggle: noop, contains: () => false },
    setAttribute: noop, getAttribute: () => null, addEventListener: noop,
    removeEventListener: noop, appendChild: noop, focus: noop, click: noop,
    getContext: () => stubContext(),
    getBoundingClientRect: () => ({ left: 0, top: 0, width: 1280, height: 720 }),
    value: "0", textContent: "", innerHTML: "", max: "1", files: [],
    width: 1280, height: 720, hidden: false, dataset: {},
  };
  return el;
}
const elements = new Map();
const documentStub = {
  getElementById: id => {
    if (!elements.has(id)) elements.set(id, stubElement());
    return elements.get(id);
  },
  createElement: () => stubElement(),
  addEventListener: noop, removeEventListener: noop,
  body: stubElement(), documentElement: stubElement(),
};
const windowStub = {
  addEventListener: noop, removeEventListener: noop,
  requestAnimationFrame: noop, devicePixelRatio: 1,
  location: { search: "", href: "" },
  // no AudioContext: ensureAudio degrades to silent, matching CI machines
};

const harness = new Function("document", "window", "requestAnimationFrame",
  "navigator", "performance", "XMLHttpRequest", "localStorage", "location",
  "fetch", "getComputedStyle",
  `${body}
   return { load, sync, setNow: v => { now = v; }, getChart: () => chart,
            setMirror: v => { mirrorOn = v; } };`)(
  documentStub, windowStub, noop,
  { userAgent: "smoke" }, { now: () => 0 },
  function XMLHttpRequestStub() { this.open = noop; this.send = noop; this.addEventListener = noop; },
  { getItem: () => null, setItem: noop },
  windowStub.location,
  () => Promise.reject(new Error("no network in smoke test")),
  () => ({ getPropertyValue: () => "" }));

// ---- chart selection -----------------------------------------------------
const home = process.env.HOME;
const defaults = [
  // the three heaven-hold-bearing charts (all rare draw paths)
  `${home}/Downloads/music/music2194/2194_03.c2s`,
  `${home}/Downloads/music/music2493/2493_02.c2s`,
  `${home}/Downloads/music/music2699/2699_03.c2s`,
  // gimmick-dense World's End picks: keys/negatives, DCM, ALD walls
  `${home}/Downloads/music/music8302/8302_05.c2s`,
  `${home}/Downloads/music/music8135/8135_05.c2s`,
  `${home}/Downloads/music/music8315/8315_05.c2s`,
  // an ordinary dense chart and the original reference chart
  `${home}/Downloads/music/music2582/2582_03.c2s`,
  `${home}/Downloads/music/music1086/1086_03.c2s`,
];
const files = process.argv.slice(2).length ? process.argv.slice(2)
  : defaults.filter(f => existsSync(f));

let failures = 0;
for (const file of files) {
  const id = file.split("/").pop();
  for (const mirror of [false, true]) {
    try {
      harness.setMirror(mirror);
      harness.load(readFileSync(file, "latin1"));
      const duration = harness.getChart().duration || 1;
      // step through the whole chart: every 250 ms plus the exact start/end
      const stepCount = Math.min(2000, Math.max(64, Math.ceil(duration / 250)));
      for (let i = 0; i <= stepCount; i++) {
        harness.setNow(duration * (i / stepCount));
        harness.sync();   // sync() drives draw()
      }
      console.log(`ok   ${id}${mirror ? " (mirrored)" : ""}`);
    } catch (error) {
      failures++;
      console.error(`FAIL ${id}${mirror ? " (mirrored)" : ""}: ${error.message}`);
      console.error(error.stack.split("\n").slice(1, 4).join("\n"));
    }
  }
}
harness.setMirror(false);
console.log(failures ? `${failures} failures` : "all draw paths clean");
process.exit(failures ? 1 : 0);
