#!/usr/bin/env node
// Round-trip verifier (SPEC §7.3): reference-model equality between the
// source c2s and cmr2c2s(c2cmr(source)), plus the [meta] extension compare.
// DEV-SIDE ONLY.
import { readFileSync } from "node:fs";
import { parseC2s, modelOf, metaOf } from "./reference.mjs";
import { convert } from "./c2cmr.mjs";
import { emitC2s } from "./lib/emit_c2s.mjs";
import { emitCmr } from "./lib/ast.mjs";
import { parseCmr } from "./lib/parse_cmr.mjs";

// first-divergence reporter for debugging
function firstDiff(a, b, path = "$") {
  if (a === b) return null;
  if (typeof a !== typeof b || a === null || b === null ||
      typeof a !== "object") {
    return `${path}: ${JSON.stringify(a)} != ${JSON.stringify(b)}`;
  }
  const keys = new Set([...Object.keys(a), ...Object.keys(b)]);
  for (const k of keys) {
    const d = firstDiff(a[k], b[k], `${path}.${k}`);
    if (d) return d;
  }
  return null;
}

export function checkChart(text) {
  const warnings = [];
  const { ast, text: cmrText } = convert(text, { warn: w => warnings.push(w) });
  // the round trip goes through the cmr TEXT: emit -> strict parse -> emit
  const reparsed = parseCmr(cmrText);
  const cmrText2 = emitCmr(reparsed);
  if (cmrText2 !== cmrText) {
    const a = cmrText.split("\n"), b = cmrText2.split("\n");
    let i = 0; while (i < a.length && a[i] === b[i]) i++;
    return { ok: false, warnings,
             modelDiff: `cmr text not idempotent at line ${i + 1}: "${a[i]}" vs "${b[i]}"`,
             metaDiff: null };
  }
  const emitted = emitC2s(reparsed);
  const srcModel = modelOf(parseC2s(text));
  const outModel = modelOf(parseC2s(emitted));
  const modelDiff = firstDiff(srcModel, outModel);
  const metaDiff = firstDiff(metaOf(text), metaOf(emitted));
  return { ok: !modelDiff && !metaDiff, modelDiff, metaDiff, warnings };
}

const isMain = process.argv[1] && import.meta.url.endsWith(process.argv[1].split("/").pop());
if (isMain) {
  const files = process.argv.slice(2);
  let pass = 0, fail = 0, warned = 0;
  for (const file of files) {
    const id = file.split("/").slice(-1)[0];
    try {
      const r = checkChart(readFileSync(file, "latin1"));
      if (r.ok) {
        pass++;
        if (r.warnings.length) { warned++; }
        if (files.length <= 8) {
          console.log(`ok   ${id}${r.warnings.length ? ` (${r.warnings.length} warnings)` : ""}`);
          for (const w of r.warnings.slice(0, 5)) console.log(`       warn: ${w}`);
        }
      } else {
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
  console.log(`${pass} pass, ${fail} fail`);
  process.exit(fail ? 1 : 0);
}
