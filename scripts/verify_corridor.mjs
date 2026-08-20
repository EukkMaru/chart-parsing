// Corridor equivalence gate: the viewer's cmr-corridor load path must build
// the same model as parsing the c2s directly. Run: node scripts/verify_corridor.mjs [charts...]
import { readFileSync, existsSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";
import { modelOf } from "../cmr/tools/reference.mjs";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "c2s-viewer.html"), "utf8");
const script = html.split(/<script>/)[1].split(/<\/script>/)[0];
const lines = script.split("\n");
const open = lines.findIndex(l => l.trim() === "(() => {");
const close = lines.length - 1 - [...lines].reverse().findIndex(l => l.trim() === "})();");
const body = lines.slice(open + 1, close)
  .filter(l => l.trim() !== '"use strict";').join("\n");

const noop = () => {};
function stubContext() {
  const gradient = { addColorStop: noop };
  return new Proxy({}, {
    get: (t, k) => k === "measureText" ? () => ({ width: 10 })
      : k === "createLinearGradient" || k === "createRadialGradient" ? () => gradient
      : k === "getImageData" ? () => ({ data: new Uint8ClampedArray(4) })
      : typeof k === "string" ? noop : undefined,
    set: () => true,
  });
}
function stubElement() {
  return {
    style: {}, classList: { add: noop, remove: noop, toggle: noop, contains: () => false },
    setAttribute: noop, getAttribute: () => null, addEventListener: noop,
    removeEventListener: noop, appendChild: noop, focus: noop, click: noop,
    getContext: () => stubContext(),
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
    if (!elements.has(id)) elements.set(id, stubElement());
    return elements.get(id);
  },
  createElement: () => stubElement(),
  createTextNode: () => ({}),
  addEventListener: noop, removeEventListener: noop,
  body: stubElement(), documentElement: stubElement(),
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
   return { load, getChart: () => chart, parseC2s };`)(
  documentStub, windowStub, noop, { userAgent: "gate" }, { now: () => 0 },
  function X() { this.open = noop; this.send = noop; this.addEventListener = noop; },
  { getItem: () => null, setItem: noop }, windowStub.location,
  () => Promise.reject(new Error("no network")), () => ({ getPropertyValue: () => "" }));

function firstDiff(a, b, path = "$") {
  if (a === b) return null;
  if (typeof a !== typeof b || a === null || b === null || typeof a !== "object") {
    return `${path}: ${JSON.stringify(a)} != ${JSON.stringify(b)}`;
  }
  for (const k of new Set([...Object.keys(a), ...Object.keys(b)])) {
    const d = firstDiff(a[k], b[k], `${path}.${k}`);
    if (d) return d;
  }
  return null;
}

const home = process.env.HOME;
const files = process.argv.slice(2).length ? process.argv.slice(2)
  : [`${home}/Downloads/music/music1086/1086_03.c2s`].filter(existsSync);
let fail = 0;
for (const file of files) {
  const id = file.split("/").pop();
  try {
    const text = readFileSync(file, "latin1");
    harness.load(text);                       // corridor path
    const corridor = modelOf(harness.getChart());
    const direct = modelOf(harness.parseC2s(text));
    const d = firstDiff(direct, corridor);
    if (d) { fail++; console.log(`FAIL ${id}: ${d}`); }
    else if (files.length <= 10) console.log(`ok   ${id}`);
  } catch (error) {
    fail++;
    console.log(`ERR  ${id}: ${error.message.split("\n")[0]}`);
  }
}
console.log(fail ? `${fail} failures` : `${files.length} charts: corridor model-equal`);
process.exit(fail ? 1 : 0);
