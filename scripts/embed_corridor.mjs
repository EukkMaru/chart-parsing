#!/usr/bin/env node
// Regenerates the CMR CORRIDOR block inside c2s-viewer.html from the
// canonical tool modules. The tools stay the single source of truth;
// the viewer embeds a generated copy. Run after any tool change:
//   node scripts/embed_corridor.mjs
import { readFileSync, writeFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const root = join(here, "..");
const MODULES = [
  "cmr/tools/lib/ast.mjs",
  "cmr/tools/lib/c2s_read.mjs",
  "cmr/tools/lib/assoc.mjs",
  "cmr/tools/lib/parse_cmr.mjs",
  "cmr/tools/lib/emit_c2s.mjs",
  "cmr/tools/c2cmr.mjs",
  "ilcmr/tools/cmr2ilc.mjs",
];

let seenR = false;
const parts = [];
for (const rel of MODULES) {
  let code = readFileSync(join(root, rel), "utf8");
  code = code.replace(/^#!.*\n/, "");
  // strip CLI tail
  const cli = code.indexOf("const isMain =");
  if (cli >= 0) code = code.slice(0, cli);
  const lines = code.split("\n").filter(l => !/^import /.test(l));
  const out = [];
  for (let l of lines) {
    l = l.replace(/^export (const|function|let)/, "$1");
    if (/^const R = 384;/.test(l)) {
      if (seenR) continue;
      seenR = true;
    }
    out.push(l);
  }
  parts.push(`  // ---- from ${rel} ----\n` + out.join("\n").trim());
}

const block = [
  "  // ==== CMR CORRIDOR (generated from cmr/tools + ilcmr/tools by",
  "  // scripts/embed_corridor.mjs — DO NOT EDIT BY HAND; edit the tools and",
  "  // re-run the generator; the corpus gates certify the tools) ====",
  "  const CMR = (() => {",
  parts.join("\n\n"),
  "  return { convert, parseCmr, emitCmr, emitC2sRows, decompile,",
  "           decompileLineTicks: () => decompile.lastLineTicks };",
  "  })();",
  "  // ==== END CMR CORRIDOR ====",
].join("\n");

const viewerPath = join(root, "scripts/c2s-viewer.html");
let html = readFileSync(viewerPath, "utf8");
const begin = html.indexOf("  // ==== CMR CORRIDOR");
const end = html.indexOf("  // ==== END CMR CORRIDOR ====");
if (begin >= 0 && end > begin) {
  html = html.slice(0, begin) + block +
    html.slice(end + "  // ==== END CMR CORRIDOR ====".length);
} else {
  const anchor = html.indexOf('"use strict";');
  if (anchor < 0) throw new Error("no use-strict anchor");
  const insertAt = html.indexOf("\n", anchor) + 1;
  html = html.slice(0, insertAt) + "\n" + block + "\n" + html.slice(insertAt);
}
writeFileSync(viewerPath, html, "utf8");
console.log("corridor embedded:", block.split("\n").length, "lines");
