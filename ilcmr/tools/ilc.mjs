#!/usr/bin/env node
// The ilcmr compiler: ilcmr text -> cmr AST (-> cmr text). Deployment-safe.
import { readFileSync, writeFileSync } from "node:fs";
import { emitCmr, META_CMD, FX_TOKEN } from "../../cmr/tools/lib/ast.mjs";
import { parseCmr } from "../../cmr/tools/lib/parse_cmr.mjs";

const R = 384;
const gcdOf = (a, b) => b ? gcdOf(b, a % b) : a;
const GLYPH_DIR = { "^": "U", "^<": "UL", "^>": "UR", "v": "D", "v<": "DL", "v>": "DR" };
const EVENT_KEYWORDS = new Set(["BPM","METER","SCROLL","STOP","DEPTH","SCROLL-DEAD","CLICK","KEYZONE"]);
const KEYWORDS = new Set(["crush","trace","heaven","heavenx","keyzone"]);

export function compile(text, diag = { warn: () => {} }) {
  const ast = { meta: [], statsOverrides: [], events: [], items: [] };
  let lineNo = 0;
  const err = msg => { throw new Error(`ilcmr line ${lineNo}: ${msg}`); };

  // exact rational stream position, in ticks
  let posN = 0, posD = 1;                    // pos = posN/posD ticks
  let division = 4;
  const advance = () => {
    // += R/division
    const d = posD * division;
    posN = posN * division + R * posD;
    posD = d;
    const g = gcdOf(posN, posD); posN /= g; posD /= g;
  };
  const tickHere = () => {
    if (posN % posD === 0) return posN / posD;
    const t = Math.round(posN / posD);
    diag.warn(`line ${lineNo}: onset snapped to tick ${t} ` +
      `(drift ${(posN / posD - t).toFixed(3)})`);
    return t;
  };
  const durTicks = spec => {
    const m = spec.match(/^\[(\d+):(\d+)\]$/);
    if (!m) err(`bad duration "${spec}"`);
    const val = Number(m[2]) * R / Number(m[1]);
    if (!Number.isInteger(val)) {
      const t = Math.round(val);
      diag.warn(`line ${lineNo}: duration ${spec} snapped to ${t} ticks`);
      return t;
    }
    return val;
  };

  // ---- gimmick state channels --------------------------------------------
  const open = new Map();          // channel -> {p, factor, key, cmd}
  const chanChange = (name, cmd, key, factor, p) => {
    const o = open.get(name);
    if (o) {
      const e = { cmd: o.cmd, p: o.p, dur: p - o.p, factor: o.factor };
      if (o.cmd === "SLP") e.key = o.key;
      ast.events.push(e);
      open.delete(name);
    }
    if (factor !== null) open.set(name, { p, factor, key, cmd });
  };
  const finishChannels = p => {
    for (const name of [...open.keys()]) {
      diag.warn(`unclosed ${name} state closed at stream end`);
      chanChange(name, null, null, null, p);
    }
  };

  // ---- escapes ------------------------------------------------------------
  let escapeRun = [];
  const flushEscapes = () => {
    if (!escapeRun.length) return;
    const noteLines = [], eventLines = [];
    for (const l of escapeRun) {
      const w = l.split(/[ \t]+/)[0];
      (EVENT_KEYWORDS.has(w) && w !== "KEYZONE" ? eventLines : noteLines).push(l);
    }
    if (eventLines.length) {
      const sub = parseCmr("cmr 1\n[events]\n" + eventLines.join("\n") + "\n");
      ast.events.push(...sub.events);
      ast.items.push(...sub.items);
    }
    if (noteLines.length) {
      const sub = parseCmr("cmr 1\n[notes]\n" +
        noteLines.map(l => /^(~>|->)/.test(l) ? "  " + l : l).join("\n") + "\n");
      ast.items.push(...sub.items);
    }
    escapeRun = [];
  };

  // ---- note-token scanner -------------------------------------------------
  const scanToken = (tok, p) => {
    let i = 0;
    const peek = () => tok[i];
    const take = re => {
      const m = tok.slice(i).match(re);
      if (!m || m.index !== 0) return null;
      i += m[0].length; return m[0];
    };
    const takeSpan = () => {
      const m = take(/^(\d+)-(\d+)/);
      if (!m) err(`expected span in "${tok}" at offset ${i}`);
      const [a, b] = m.split("-").map(Number);
      if (b <= a) err(`bad span ${m} (right edge must exceed left)`);
      return { lane: a, width: b - a };
    };
    const takeDur = () => {
      const m = take(/^\[\d+:\d+\]/);
      if (!m) err(`expected [div:count] in "${tok}" at offset ${i}`);
      return durTicks(m);
    };
    const takeTag = () => {
      const m = take(/^\.[A-Z0-9_-]*/);
      return m === null ? undefined : m.slice(1);   // "." -> "" empty tag
    };
    const takeHeight = () => {
      const m = take(/^@-?[\d.]+/);
      return m === null ? undefined : Number(m.slice(1));
    };

    const sigil = take(/^(hx|sx|x|f|d|h|s)(?=\d)/) || "";
    const span = takeSpan();
    const items = [];
    let owner;

    if (sigil === "" || sigil === "x" || sigil === "f" || sigil === "d") {
      const family = { "": "TAP", x: "EXTAP", f: "FLICK", d: "DAMAGE" }[sigil];
      let tag = takeTag();
      let note;
      if (family === "EXTAP") note = tag === undefined ? "UP" : (tag === "" ? undefined : tag);
      else if (family === "FLICK") note = tag === undefined ? "L" : (tag === "" ? undefined : tag);
      else if (tag !== undefined) err(`${family} takes no tag`);
      owner = { k: "note", family, p, lane: span.lane, width: span.width,
                note, airs: [] };
      items.push(owner);
    } else if (sigil === "h" || sigil === "hx") {
      const dur = takeDur();
      const tag = takeTag();
      if (sigil === "h" && tag !== undefined) err("HOLD takes no tag");
      owner = { k: "hold", family: sigil === "hx" ? "EXHOLD" : "HOLD",
                p, lane: span.lane, width: span.width, endP: p + dur,
                fx: tag === "" ? undefined : tag, airs: [] };
      items.push(owner);
    } else {          // s / sx
      const headEx = sigil === "sx";
      owner = { k: "slide", family: headEx ? "EXSLIDE" : "SLIDE",
                p, lane: span.lane, width: span.width,
                headStyle: undefined, headFx: undefined, headEx, steps: [] };
      let cursor = p;
      for (;;) {
        const dur = takeDur();
        const arrow = take(/^[>~]/);
        if (!arrow) err(`expected > or ~ leg in "${tok}"`);
        const target = takeSpan();
        cursor += dur;
        owner.steps.push({ p: cursor, lane: target.lane, width: target.width,
                           sevenField: true, marked: arrow === ">", ex: headEx,
                           style: undefined, fx: undefined, airs: [] });
        if (!/^\[/.test(tok.slice(i))) break;
      }
      items.push(owner);
    }

    // suffixes: air glyphs, ^h/^hx, ^s
    const ownerPoint = () => {
      if (owner.k === "note") return { p: owner.p, lane: owner.lane, width: owner.width };
      if (owner.k === "hold") return { p: owner.endP, lane: owner.lane, width: owner.width };
      const last = owner.steps[owner.steps.length - 1];
      return { p: last.p, lane: last.lane, width: last.width };
    };
    const airHostOf = () => owner.k === "slide"
      ? owner.steps[owner.steps.length - 1] : owner;
    while (i < tok.length) {
      if (peek() === "^" && (tok[i + 1] === "h")) {
        i += 2;
        const action = tok[i] === "x" ? (i++, true) : false;
        const tag = takeTag();
        const q = ownerPoint();
        const item = { k: "ahold", p: q.p, lane: q.lane, width: q.width,
                       on: owner.family, note: tag === "" ? undefined : tag,
                       steps: [], ownerItem: owner };
        let cursor = q.p;
        for (;;) {
          cursor += takeDur();
          item.steps.push({ p: cursor, action: false });
          if (peek() === "~") { i++; continue; }
          break;
        }
        item.steps[item.steps.length - 1].action = action;
        items.push(item);
        continue;
      }
      if (peek() === "^" && tok[i + 1] === "s") {
        i += 2;
        const q = ownerPoint();
        const h0 = takeHeight() ?? 1;
        const item = { k: "aslide", p: q.p, lane: q.lane, width: q.width,
                       h0, on: owner.family, headStyle: undefined,
                       steps: [], ownerItem: owner };
        let cursor = q.p, h = h0;
        for (;;) {
          const dur = takeDur();
          const arrow = take(/^[>~]/);
          if (!arrow) err(`expected > or ~ air-slide leg in "${tok}"`);
          const target = takeSpan();
          const nh = takeHeight();
          if (nh !== undefined) h = nh;
          cursor += dur;
          item.steps.push({ p: cursor, lane: target.lane, width: target.width,
                            h1: h, from: undefined, action: arrow === ">",
                            style: undefined });
          if (!/^\[/.test(tok.slice(i))) break;
        }
        items.push(item);
        continue;
      }
      const glyph = take(/^(\^<|\^>|v<|v>|\^|v)/);
      if (glyph) {
        airHostOf().airs.push({ dir: GLYPH_DIR[glyph] });
        continue;
      }
      err(`unexpected suffix at "${tok.slice(i)}" in "${tok}"`);
    }
    return items;
  };

  // ---- keyword-form scanner ----------------------------------------------
  const scanKeyword = (words, p) => {
    const word = words[0];
    let i = 1;
    const next = () => words[i++];
    const spanAt = tok => {
      const m = tok.match(/^(\d+)-(\d+)(@(.*))?$/);
      if (!m) err(`expected span[@h] got "${tok}"`);
      return { lane: Number(m[1]), width: Number(m[2]) - Number(m[1]),
               h: m[4] };
    };
    const glyphSplit = tok => {
      // strip trailing air glyphs from a leg token
      const m = tok.match(/(\^<|\^>|v<|v>|\^|v)+$/);
      if (!m) return [tok, []];
      const glyphs = m[0].match(/\^<|\^>|v<|v>|\^|v/g).map(g => ({ dir: GLYPH_DIR[g] }));
      return [tok.slice(0, tok.length - m[0].length), glyphs];
    };
    if (word === "keyzone") {
      const s = spanAt(next());
      if (next() !== "key") err("keyzone needs key <n>");
      const key = Number(next());
      const dur = durTicks(next());
      return [{ k: "keyzone", p, lane: s.lane, width: s.width,
                endP: p + dur, key }];
    }
    if (word === "crush" || word === "trace") {
      const head = spanAt(next());
      const item = word === "crush"
        ? { k: "crush", p, lane: head.lane, width: head.width,
            h0: Number(head.h), interval: 0, headStyle: undefined, steps: [] }
        : { k: "trace", p, lane: head.lane, width: head.width,
            pA0: Number(head.h.split(":")[0]), pB0: Number(head.h.split(":")[1]),
            headStyle: undefined, steps: [] };
      let cursor = p, pendingStyle;
      while (i < words.length) {
        const w = next();
        if (w === "every") { item.interval = Number(next()); continue; }
        if (w === "style") {
          const v = next();
          if (!item.steps.length && item.headStyle === undefined && pendingStyle === undefined) {
            item.headStyle = v === "none" ? undefined : v;
            if (item.steps.length === 0) { pendingStyle = null; continue; }
          }
          const lastStep = item.steps[item.steps.length - 1];
          if (lastStep) lastStep.style = v === "none" ? undefined : v;
          continue;
        }
        if (/^\[/.test(w)) {
          const dur = durTicks(w);
          if (next() !== ">") err(`${word} legs use ">"`);
          const [legTok, airs] = glyphSplit(next());
          const s = spanAt(legTok);
          cursor += dur;
          const step = word === "crush"
            ? { p: cursor, lane: s.lane, width: s.width, h1: Number(s.h),
                style: item.headStyle, airs }
            : { p: cursor, lane: s.lane, width: s.width,
                pA1: Number(s.h.split(":")[0]), pB1: Number(s.h.split(":")[1]),
                style: item.headStyle, airs };
          item.steps.push(step);
          continue;
        }
        err(`unexpected "${w}" in ${word} form`);
      }
      if (item.steps.length === 1) item.single = true;
      return [item];
    }
    // heaven / heavenx
    const family = word === "heavenx" ? "EXHEAVEN" : "HEAVEN";
    const headTok = next();
    const tagM = headTok.match(/^(.*?)(\.([A-Za-z0-9_-]+))?$/);
    const head = spanAt(headTok.split(".")[0]);
    const item = { k: "heaven", family, p, lane: head.lane, width: head.width,
                   h0: Number(head.h), disc: 0, fx: undefined, steps: [] };
    let cursor = p;
    while (i < words.length) {
      const w = next();
      if (w === "disc") { item.disc = Number(next()); continue; }
      if (w.startsWith(".")) { item.fx = FX_TOKEN.get(w.slice(1)) ?? w.slice(1); continue; }
      if (/^\[/.test(w)) {
        const dur = durTicks(w);
        if (next() !== ">") err("heaven legs use \">\"");
        const [legTok, airs] = glyphSplit(next());
        const s = spanAt(legTok);
        cursor += dur;
        item.steps.push({ p: cursor, lane: s.lane, width: s.width,
                          h1: Number(s.h), airs });
        continue;
      }
      err(`unexpected "${w}" in ${word} form`);
    }
    if (item.steps.length === 1) item.single = true;
    return [item];
  };

  // ---- event parser -------------------------------------------------------
  const handleEvent = (body, p) => {
    const w = body.split(/[ \t]+/);
    if (/^-?[\d.]+$/.test(w[0])) { ast.events.push({ cmd: "BPM", p, bpm: Number(w[0]) }); return; }
    switch (w[0]) {
      case "bpm": ast.events.push({ cmd: "BPM", p, bpm: Number(w[1]) }); return;
      case "meter": {
        const m = w[1].match(/^(-?\d+)\/(-?\d+)$/);
        if (!m) err(`bad meter "${w[1]}"`);
        ast.events.push({ cmd: "MET", p, count: Number(m[1]), unit: Number(m[2]) });
        return;
      }
      case "click": ast.events.push({ cmd: "CLK", p }); return;
      case "stop": ast.events.push({ cmd: "STP", p, dur: durTicks(w[1]) }); return;
      case "speed": {
        const keyed = w[2] === "key" || w[3] === "key";
        const key = keyed ? Number(w[w.indexOf("key") + 1]) : null;
        const name = keyed ? `speed key ${key}` : "speed";
        const cmd = keyed ? "SLP" : "SFL";
        if (w[1] === "off") chanChange(name, cmd, key, null, p);
        else chanChange(name, cmd, key, Number(w[1]), p);
        return;
      }
      case "depth": {
        if (w[1] === "off") chanChange("depth", "DCM", null, null, p);
        else chanChange("depth", "DCM", null, Number(w[1]), p);
        return;
      }
      case "scroll-dead": {
        if (w[1] === "off") chanChange("scroll-dead", "SFE", null, null, p);
        else chanChange("scroll-dead", "SFE", null, Number(w[1]), p);
        return;
      }
    }
    err(`unknown event "(${body})"`);
  };

  // ---- main loop ----------------------------------------------------------
  for (let raw of text.split(/\r?\n/)) {
    lineNo++;
    if (raw.startsWith("!")) { escapeRun.push(raw.slice(1).trim()); continue; }
    flushEscapes();
    if (raw.startsWith("&")) {
      const m = raw.match(/^&([a-z-]+)[ \t]?(.*)$/);
      if (!m) err(`bad header line "${raw}"`);
      if (!META_CMD.has(m[1])) err(`unknown header key "&${m[1]}"`);
      ast.meta.push({ key: m[1], value: m[2] });
      continue;
    }
    const c = raw.indexOf("//");
    const line = (c >= 0 ? raw.slice(0, c) : raw);
    if (!line.trim()) continue;
    const lexemes = line.match(/\([^)]*\)|,|\|/g) !== null
      ? (line.match(/\([^)]*\)|,|\||[^\s,|]+/g) || []) : (line.match(/[^\s]+/g) || []);
    let li = 0;
    while (li < lexemes.length) {
      const lex = lexemes[li];
      if (lex === ",") { advance(); li++; continue; }
      if (lex === "|") {
        if (!(posN % posD === 0 && (posN / posD) % R === 0)) {
          err(`bar check failed at position ${posN / posD} ticks`);
        }
        li++; continue;
      }
      if (lex === "/") { li++; continue; }   // slot-construct separator
      if (/^\{\d+\}$/.test(lex)) { division = Number(lex.slice(1, -1)); li++; continue; }
      if (lex.startsWith("(")) { handleEvent(lex.slice(1, -1).trim(), tickHere()); li++; continue; }
      if (KEYWORDS.has(lex)) {
        const group = [];
        while (li < lexemes.length && lexemes[li] !== "," &&
               lexemes[li] !== "|" && lexemes[li] !== "/") {
          group.push(lexemes[li]); li++;
        }
        ast.items.push(...scanKeyword(group, tickHere()));
        continue;
      }
      // note chord
      for (const part of lex.split("/")) {
        ast.items.push(...scanToken(part, tickHere()));
      }
      li++;
    }
  }
  flushEscapes();
  finishChannels(posN % posD === 0 ? posN / posD : Math.round(posN / posD));
  return ast;
}

const isMain = process.argv[1] && import.meta.url.endsWith(process.argv[1].split("/").pop());
if (isMain) {
  const file = process.argv[2];
  if (!file) { console.error("usage: ilc.mjs <chart.ilcmr> [-o out.cmr]"); process.exit(2); }
  const warnings = [];
  const ast = compile(readFileSync(file, "latin1"), { warn: w => warnings.push(w) });
  for (const w of warnings) console.error(`warn: ${w}`);
  const out = emitCmr(ast);
  const o = process.argv.indexOf("-o");
  if (o >= 0) writeFileSync(process.argv[o + 1], out, "latin1");
  else process.stdout.write(out);
}
