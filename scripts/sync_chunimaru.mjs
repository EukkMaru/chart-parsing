#!/usr/bin/env node
// Regenerates ~/chunimaru/chart-viewer.html from the canonical viewer.
// Transform: chunimaru head + theme-alias palette + selector card + the
// cmr sample-chart fetch snippet. Run after canonical viewer changes:
//   node scripts/sync_chunimaru.mjs
import { readFileSync, writeFileSync, copyFileSync } from "node:fs";
import { buildCorridorBlock, DEPLOY_MODULES, DEPLOY_RETURN } from "./embed_corridor.mjs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const canonical = readFileSync(join(here, "c2s-viewer.html"), "utf8");
const outPath = "/home/etri/chunimaru/chart-viewer.html";

// ---- pieces of the canonical page ----
const style = canonical.split("<style>")[1].split("</style>")[0];
const bodyStart = canonical.indexOf('<div class="wrap">');
const bodyEnd = canonical.indexOf('<script src="./c2s-schedule-reference.js">');
const body = canonical.slice(bodyStart, bodyEnd).trimEnd();
let script = canonical.split("<script>")[1].split("</script>")[0];

// ---- deployment transforms: no other chart format exists on this page ----
// 1) swap the dev corridor block for the cmr-only build
{
  const begin = script.indexOf("  // ==== CMR CORRIDOR");
  const end = script.indexOf("  // ==== END CMR CORRIDOR ====") +
    "  // ==== END CMR CORRIDOR ====".length;
  if (begin < 0) throw new Error("corridor block not found");
  const block = buildCorridorBlock(DEPLOY_MODULES, DEPLOY_RETURN, [
    "  // ==== CMR CORRIDOR (cmr-only deployment build; generated) ====",
  ]);
  script = script.slice(0, begin) + block + "\n" + script.slice(end);
}
// 2) strip every DEV ONLY region, and the local drop/pick loader (the
//    deployment loads charts through chart.html's selector only)
for (const [beginMark, endMark] of [
  ["  // ==== DEV ONLY", "  // ==== END DEV ONLY ===="],
  ["  // ==== LOCAL LOAD", "  // ==== END LOCAL LOAD ===="],
]) {
  for (;;) {
    const a = script.indexOf(beginMark);
    if (a < 0) break;
    const b = script.indexOf(endMark);
    if (b < 0) throw new Error("unterminated region: " + beginMark);
    script = script.slice(0, a) + script.slice(b + endMark.length);
  }
}
// 3) the load path accepts cmr only
{
  const old = script.match(/    const cmrText = \(firstLine \|\| ""\).trim\(\) === "cmr 1"\n      \? text : CMR.convert\(text, diag\).text;/);
  if (!old) throw new Error("load convert branch not found");
  script = script.replace(old[0],
    `    if ((firstLine || "").trim() !== "cmr 1") {
      console.warn("this page loads cmr charts only");
      el("drop").classList.remove("hidden");
      return;
    }
    const cmrText = text;`);
}
// 5) scrub comments that mention the upstream format, then assert none left
script = script.split("\n").map(line => {
  if (!/c2s/i.test(line)) return line;
  const t = line.trim();
  if (t.startsWith("//")) return null;               // whole-line comment: drop
  const c = line.indexOf("//");
  if (c >= 0 && /c2s/i.test(line.slice(c))) return line.slice(0, c).replace(/\s+$/, "");
  return line;                                        // non-comment hit: caught below
}).filter(l => l !== null).join("\n");
if (/c2s/i.test(script)) {
  const hit = script.split("\n").find(l => /c2s/i.test(l));
  throw new Error("c2s survives in deploy script: " + hit.trim().slice(0, 100));
}

// ---- palette swap: site theme aliases replace the standalone palette ----
const ALIAS = `  :root {
    /* Page-local names aliased onto the shared theme tokens (css/theme.css).
       --panel, --line, --accent, --accent-soft and --shadow come straight
       from theme.css. The viewer stage itself (viewport, drop zone, canvas
       colors) is an always-dark surface by design and keeps its literals. */
    --ground: var(--bg);
    --panel-2: var(--panel);
    --ink: var(--text);
    --ink-2: var(--muted);
    --ink-3: var(--muted);
    --warn: var(--danger);
  }
`;
const palStart = style.indexOf("  :root {");
const lightBlock = style.indexOf(':root[data-theme="light"] {');
if (palStart < 0 || lightBlock < 0) throw new Error("palette blocks not found");
const palEnd = style.indexOf("\n  }\n", lightBlock) + "\n  }\n".length;
const themedStyle = style.slice(0, palStart) + ALIAS + style.slice(palEnd);
const deployStyle = themedStyle + `
  /* deployment: diagnostic rail cards stay in markup and code but do not
     render (owner, 2026-08-20) */
  #statCard, #legCard, #unkCard { display: none !important; }
  /* deployment: the cmr pane is parked for now -- still built and
     populated by the legacy code, just not rendered; the ilcmr pane
     takes the full double width (owner, 2026-08-20) */
  .cp-cols .cp:first-child { display: none; }
  /* deployment: slim site nav in place of the topbar */
  .sitenav { display: flex; gap: 8px; margin-bottom: 14px; }
  .sitenav a {
    display: inline-flex; align-items: center; min-height: 32px;
    padding: 0 16px; border: 1px solid var(--line); border-radius: 999px;
    background: var(--panel); color: var(--ink-2);
    font-size: 12px; font-weight: 700; text-decoration: none;
  }
  .sitenav a:hover { border-color: var(--accent); color: var(--ink); }
`;

// selector card removed for now (owner, 2026-08-20); local drop/pick only
let withCard = body;
// the deployed viewer has no site topbar: give it a slim nav row
{
  const navHtml = `<div class="sitenav">
    <a href="./chart.html" data-i18n="chart_viewer.nav.back_list">&larr; Back to list</a>
    <a href="./index.html" data-i18n="chart_viewer.nav.back_home">Back to ChuniMaru</a>
  </div>

  `;
  const at = withCard.indexOf('<div class="timeline">');
  if (at < 0) throw new Error("timeline anchor not found");
  withCard = withCard.slice(0, at) + navHtml + withCard.slice(at);
}
{
  const a = withCard.indexOf('<div class="drop" id="drop">');
  const b = withCard.indexOf("</div>", a) + "</div>".length;
  if (a < 0) throw new Error("drop zone not found");
  withCard = withCard.slice(0, a) + `<div class="drop" id="drop">
        <strong data-i18n="chart_viewer.drop.title">No chart loaded</strong>
        <span data-i18n="chart_viewer.drop.copy">Pick one from the chart list.</span>
        <a href="./chart.html"><button type="button" data-i18n="chart_viewer.drop.browse">Browse charts</button></a>
      </div>` + withCard.slice(b);
}

const SNIPPET = `
  // ---- chunimaru deployment: chart handoff from chart.html ---------------
  // chart.html links here as chart-viewer.html?chart=musicNNNN/chart_NN.cmr;
  // exactly one chart occupies the session -- each navigation replaces it.
  {
    const wanted = new URLSearchParams(location.search).get("chart");
    if (wanted && /^music\\d+\\/[\\w.-]+\\.cmr$/.test(wanted)) {
      const titleReady = fetch("./data/charts-catalogue.json", { cache: "no-cache" })
        .then(r => r.ok ? r.json() : null)
        .then(cat => {
          const folder = wanted.split("/")[0];
          const song = cat && cat.songs.find(x => x.folder === folder);
          if (song) pendingChartTitle = song.title;
        })
        .catch(() => {});
      Promise.all([window.ChartPack.file(wanted), titleReady])
        .then(([text]) => load(text))
        .catch(() => { el("drop").classList.remove("hidden"); });
    }
  }
`;
const closeAt = script.lastIndexOf("})();");
if (closeAt < 0) throw new Error("IIFE close not found");
const deployScript = script.slice(0, closeAt) + SNIPPET + script.slice(closeAt);

// ---- assemble ----
const page = `<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />
<script>try{var l=localStorage.getItem("chunimaru.language");if(l)document.documentElement.lang=l;var t=new URLSearchParams(location.search).get("theme")||localStorage.getItem("chunimaru.theme")||"auto";if(/^[1-9]$/.test(t))t="theme-"+t;if(t==="dark")t="theme-2";else if(t==="light")t="theme-1";var K={"theme-1":"light","theme-2":"dark","theme-3":"light","theme-4":"dark","theme-5":"light","theme-6":"dark","theme-7":"dark","theme-8":"light","theme-9":"dark"};if(t==="auto")t=(window.matchMedia&&matchMedia("(prefers-color-scheme: dark)").matches)?"theme-9":"theme-8";if(!K[t])t="theme-8";document.documentElement.dataset.theme=t;document.documentElement.dataset.scheme=K[t];var sh=localStorage.getItem("chunimaru.shape");if(sh==="square")document.documentElement.dataset.shape="square";else if(sh!=="legacy")document.documentElement.dataset.shape="mixed"}catch(e){}</scr` + `ipt>
<title data-i18n="chart_viewer.page_title">Chart Viewer | ChuniMaru</title>
<script src="./chart-pack.js"></scr` + `ipt>
<link rel="icon" type="image/svg+xml" href="./imgs/chunimaru.svg" />
<link rel="stylesheet" href="./css/theme.css" />
<link id="chunimaru-shape-styles" rel="stylesheet" href="./css/shape-common.css" />
<style>${deployStyle}</style>
</head>
<body data-page="chart-viewer">
${withCard}

<script>${deployScript}</scr` + `ipt>
<script defer src="./i18n.js"></scr` + `ipt>
</body>
</html>
`;
writeFileSync(outPath, page, "utf8");

// ---- served chart data ----
// cmr/demos is the canonical source; it mirrors into chunimaru's
// chart-src (repo-private plaintext) and the chart artifact builder
// generates what the site actually serves: the plaintext catalogue for
// chart.html and the per-song redacted pack blobs for the viewer. The
// chunimaru pre-commit hook verifies the artifacts stay in sync.
{
  const fs = await import("node:fs");
  const { execFileSync } = await import("node:child_process");
  const src = join(here, "../cmr/demos");
  const dstSrc = "/home/etri/chunimaru/chart-src";
  fs.rmSync(dstSrc, { recursive: true, force: true });
  fs.mkdirSync(dstSrc, { recursive: true });
  for (const folder of fs.readdirSync(src).sort()) {
    if (!/^music\d+$/.test(folder)) continue;
    fs.mkdirSync(join(dstSrc, folder), { recursive: true });
    for (const f of fs.readdirSync(join(src, folder)).sort()) {
      copyFileSync(join(src, folder, f), join(dstSrc, folder, f));
    }
  }
  execFileSync("node", ["scripts/build_charts.mjs"],
    { cwd: "/home/etri/chunimaru", stdio: "inherit" });
}

// ---- id coverage check ----
const refs = new Set([...deployScript.matchAll(/el\("([\w-]+)"\)/g)].map(m => m[1]));
const ids = new Set([...page.matchAll(/id="([\w-]+)"/g)].map(m => m[1]));
if (/c2s/i.test(page)) {
  const hit = page.split("\n").find(l => /c2s/i.test(l));
  throw new Error("c2s survives in deploy page: " + hit.trim().slice(0, 100));
}
const missing = [...refs].filter(id => !ids.has(id));
if (missing.length) throw new Error("script references missing ids: " + missing.join(", "));
console.log("chart-viewer.html regenerated;", refs.size, "ids referenced, all present");
