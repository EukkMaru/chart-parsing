#!/usr/bin/env node
// The ilcmr fixed-point gate (SPEC intro): for chart C,
//   parse(cmr2c2s(ilc(cmr2ilc(C)))) == parse(cmr2c2s(C))
// at the cmr compared-model level. DEV-SIDE ONLY.
import { readFileSync } from "node:fs";
import { parseC2s, modelOf, metaOf } from "../../cmr/tools/reference.mjs";
import { convert } from "../../cmr/tools/c2cmr.mjs";
import { emitC2s } from "../../cmr/tools/lib/emit_c2s.mjs";
import { parseCmr } from "../../cmr/tools/lib/parse_cmr.mjs";
import { decompile } from "./cmr2ilc.mjs";
import { compile } from "./ilc.mjs";

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

export function checkChart(text) {
  const warnings = [];
  const diag = { warn: w => warnings.push(w) };
  const { text: cmrText } = convert(text, diag);
  const ast = parseCmr(cmrText);                 // through the real text layer
  const il = decompile(ast, diag);
  const ast2 = compile(il, diag);
  const back = emitC2s(ast2);
  const modelDiff = firstDiff(modelOf(parseC2s(text)), modelOf(parseC2s(back)));
  const m1 = metaOf(text), m2 = metaOf(back);
  const metaDiff = firstDiff(m1, m2);
  return { ok: !modelDiff && !metaDiff, modelDiff, metaDiff, warnings };
}

const isMain = process.argv[1] && import.meta.url.endsWith(process.argv[1].split("/").pop());
if (isMain) {
  let pass = 0, fail = 0;
  const escTotals = new Map();
  for (const file of process.argv.slice(2)) {
    const id = file.split("/").pop();
    try {
      const r = checkChart(readFileSync(file, "latin1"));
      for (const w of r.warnings) {
        const m = w.match(/escape fallback: (\S+) x(\d+)/);
        if (m) escTotals.set(m[1], (escTotals.get(m[1]) || 0) + Number(m[2]));
      }
      if (r.ok) { pass++; if (process.argv.length <= 10) console.log(`ok   ${id}`); }
      else {
        fail++;
        console.log(`FAIL ${id}`);
        if (r.modelDiff) console.log(`       model: ${r.modelDiff}`);
        if (r.metaDiff) console.log(`       meta:  ${r.metaDiff}`);
      }
    } catch (error) {
      fail++;
      console.log(`ERR  ${id}: ${error.message.split("\n")[0]}`);
    }
  }
  for (const [why, n] of escTotals) console.log(`escapes: ${why} x${n}`);
  console.log(`${pass} pass, ${fail} fail`);
  process.exit(fail ? 1 : 0);
}
