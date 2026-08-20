#!/usr/bin/env node
// c2s -> cmr importer. DEV-SIDE ONLY: this is the one-time (per-chart)
// archival boundary crossing; deployment serves the .cmr it produces.
import { readFileSync, writeFileSync } from "node:fs";
import { readC2s, R } from "./lib/c2s_read.mjs";
import { associate } from "./lib/assoc.mjs";
import { emitCmr, META_KEY, fmt } from "./lib/ast.mjs";

export function convert(text, diag = { warn: () => {} }) {
  const src = readC2s(text, diag);

  // [meta]: last duplicate wins (the reference header state is
  // last-assignment); order of first appearance is preserved.
  const seen = new Map();
  for (const [cmd, rest] of src.meta) {
    if (seen.has(cmd)) diag.warn(`duplicate header ${cmd}; last wins`);
    seen.set(cmd, rest);
  }
  const meta = [];
  const done = new Set();
  for (const [cmd] of src.meta) {
    if (done.has(cmd)) continue;
    done.add(cmd);
    const rest = seen.get(cmd);
    const key = META_KEY.get(cmd);
    const toks = rest.split(/[\t ]+/).filter(s => s.length);
    let value;
    switch (key) {
      case "creator": value = rest; break;                    // raw to EOL
      case "difficulty": case "version": value = toks.join(" "); break;
      case "bpm-default": value = toks.slice(0, 4).map(t => fmt(Number.parseFloat(t) || 0)).join(" "); break;
      case "meter-default": {
        const unit = Number.parseInt(toks[0], 10) || 0;
        const count = Number.parseInt(toks[1], 10) || 0;
        value = `${count}/${unit}`; break;                    // un-inverted
      }
      default: value = toks.map(t => fmt(Number.parseFloat(t) || 0)).join(" ");
    }
    meta.push({ key, value });
  }

  const events = src.events.map(e => ({ ...e, p: e.m * R + e.t }));
  const { items, warnings, errors } = associate(src.records);
  for (const w of warnings) diag.warn(w);
  if (errors.length) {
    throw new Error("non-reproducible source ordering:\n  " + errors.join("\n  "));
  }
  const ast = { meta, statsOverrides: src.statLines, events, items };
  return { ast, text: emitCmr(ast), unknown: src.unknown, warnings };
}

const isMain = process.argv[1] && import.meta.url.endsWith(process.argv[1].split("/").pop());
if (isMain) {
  const args = process.argv.slice(2);
  const oIndex = args.indexOf("-o");
  const outFile = oIndex >= 0 ? args.splice(oIndex, 2)[1] : null;
  const file = args[0];
  if (!file) { console.error("usage: c2cmr.mjs <chart.c2s> [-o chart.cmr]"); process.exit(2); }
  const warnings = [];
  const diag = { warn: w => warnings.push(w) };
  try {
    const { text, unknown } = convert(readFileSync(file, "latin1"), diag);
    for (const w of warnings) console.error(`warn: ${w}`);
    for (const [cmd, n] of unknown) console.error(`warn: unknown command ${cmd} x${n} dropped`);
    if (outFile) writeFileSync(outFile, text);
    else process.stdout.write(text);
  } catch (error) {
    console.error(`error: ${error.message}`);
    process.exit(1);
  }
}
