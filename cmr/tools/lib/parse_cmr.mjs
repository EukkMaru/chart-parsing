// Strict cmr text parser -> AST (the same shape assoc.mjs builds).
// Deployment-safe: this is the parser the viewer port will grow from.
// SPEC §1: unknown keyword, malformed field, or out-of-place line is a
// hard error with file/line context.
import { R, FX_TOKEN, META_CMD } from "./ast.mjs";

const NOTE_WORDS = new Set(["TAP","EXTAP","FLICK","DAMAGE"]);
const HOLD_WORDS = new Set(["HOLD","EXHOLD"]);
const CHAIN_WORDS = new Set(["SLIDE","EXSLIDE","AIRHOLD","AIRSLIDE","CRUSH","TRACE","HEAVEN","EXHEAVEN"]);
const EVENT_WORDS = new Set(["BPM","METER","SCROLL","STOP","KEYZONE","DEPTH","SCROLL-DEAD","CLICK"]);

export function parseCmr(text) {
  const lines = text.split(/\r?\n/);
  const ast = { meta: [], statsOverrides: [], events: [], items: [],
                lineTicks: new Map(), lineInfo: new Map() };
  const CHAIN_KIND = { slide: "slide", ahold: "air", aslide: "air",
                       crush: "air", trace: "trace", heaven: "heaven" };
  let section = null, lineNo = 0, sawPragma = false;
  let blockM = 0, currentChain = null, inOverrides = false;
  const err = msg => { throw new Error(`cmr line ${lineNo}: ${msg}`); };

  const parseMt = tok => {
    if (tok === undefined) err("missing position");
    if (tok.includes(":")) {
      const [m, t] = tok.split(":");
      if (!/^-?\d+$/.test(m) || !/^\d+$/.test(t)) err(`bad position "${tok}"`);
      return Number(m) * R + Number(t);
    }
    if (!/^\d+$/.test(tok)) err(`bad tick "${tok}"`);
    return blockM * R + Number(tok);
  };
  const num = tok => {
    const v = Number.parseFloat(tok);
    if (Number.isNaN(v)) err(`bad number "${tok}"`);
    return v;
  };
  const int = tok => {
    if (!/^[+-]?\d+$/.test(tok ?? "")) err(`bad integer "${tok}"`);
    return Number(tok);
  };
  const hVal = tok => {
    if (!tok?.startsWith("h")) err(`expected h<value>, got "${tok}"`);
    return num(tok.slice(1));
  };
  // trailing clause scan: [note T] [style S] [fx W] [from hV] [plain|ex]
  // [every N] [disc N] [on F] [key N] [until P] [end] [+air:D [style S]]
  const clauses = (toks, start) => {
    const c = { airs: [], Ls: [], ws: [], hs: [], hAs: [], hBs: [] };
    let i = start;
    while (i < toks.length) {
      const w = toks[i];
      if (w === "note") { c.note = toks[++i]; i++; }
      else if (w === "style") { c.style = toks[++i]; i++; }
      else if (w === "fx") { const t = toks[++i]; c.fx = t === "none" ? "none" : (FX_TOKEN.get(t) ?? t); i++; }
      else if (w === "from") { c.from = hVal(toks[++i]); i++; }
      else if (w === "until") { c.until = parseMt(toks[++i]); i++; }
      else if (w === "every") { c.every = int(toks[++i]); i++; }
      else if (w === "disc") { c.disc = int(toks[++i]); i++; }
      else if (w === "on") { c.on = toks[++i]; i++; }
      else if (w === "key") { c.key = int(toks[++i]); i++; }
      else if (w === "hA") { c.hAs.push(num(toks[++i])); i++; }
      else if (w === "hB") { c.hBs.push(num(toks[++i])); i++; }
      else if (w === "plain") { c.exFlag = false; i++; }
      else if (w === "ex") { c.exFlag = true; i++; }
      else if (w === "end") { c.end = true; i++; }
      else if (w === "orphan") { c.orphan = true; i++; }
      else if (w.startsWith("+air:")) {
        const air = { dir: w.slice(5) };
        if (toks[i + 1] === "style") { air.style = toks[i + 2]; i += 2; }
        c.airs.push(air); i++;
      }
      else if (/^L-?\d/.test(w)) { c.Ls.push(int(w.slice(1))); i++; }
      else if (/^w-?\d/.test(w)) { c.ws.push(int(w.slice(1))); i++; }
      else if (/^h-?[\d.]/.test(w)) { c.hs.push(num(w.slice(1))); i++; }
      else err(`unexpected token "${w}"`);
    }
    return c;
  };
  const one = (arr, what) => {
    if (!arr.length) err(`missing ${what}`);
    return arr[0];
  };
  // resolve step clause against header token (text -> resolved AST tokens)
  const inherit = (tok, headTok) =>
    tok === undefined ? headTok : (tok === "none" ? undefined : tok);

  for (let raw of lines) {
    lineNo++;
    const isStep = /^[ \t]+(~>|->)/.test(raw);
    let line = raw;
    const stringMeta = section === "meta" && /^(creator|difficulty|version):/.test(raw.trim());
    if (!stringMeta) {
      const c = line.indexOf("//");
      if (c >= 0) line = line.slice(0, c);
    }
    if (!line.trim()) continue;
    const trimmed = line.trim();

    if (!sawPragma) {
      if (trimmed !== "cmr 1") err(`missing or unknown version pragma (got "${trimmed}")`);
      sawPragma = true; continue;
    }
    if (trimmed === "[meta]" || trimmed === "[events]" || trimmed === "[notes]") {
      section = trimmed.slice(1, -1); inOverrides = false; continue;
    }
    if (!section) err(`content before any section: "${trimmed}"`);

    if (section === "meta") {
      if (trimmed === "stats: derived") { inOverrides = false; continue; }
      if (trimmed === "stats.overrides:") { inOverrides = true; continue; }
      if (inOverrides && /^[ \t]/.test(line)) {
        const m = trimmed.match(/^(\S+):\s*(.*)$/);
        if (!m) err(`bad override line "${trimmed}"`);
        ast.statsOverrides.push([m[1], m[2]]);
        continue;
      }
      inOverrides = false;
      const m = line.match(/^([a-z-]+(?:\.[a-z-]+)?):\s?(.*)$/);
      if (!m) err(`bad meta line "${trimmed}"`);
      if (!META_CMD.has(m[1])) err(`unknown meta key "${m[1]}"`);
      ast.meta.push({ key: m[1], value: m[2] });
      continue;
    }

    if (section === "events") {
      const toks = trimmed.split(/[ \t]+/);
      const word = toks[0];
      if (!EVENT_WORDS.has(word)) err(`unknown event keyword "${word}"`);
      const p = parseMt(toks[1]);
      ast.lineTicks.set(lineNo, p);
      if (word === "BPM") ast.events.push({ cmd: "BPM", p, bpm: num(toks[2]) });
      else if (word === "METER") {
        const m2 = toks[2]?.match(/^(-?\d+)\/(-?\d+)$/);
        if (!m2) err(`bad meter fraction "${toks[2]}"`);
        ast.events.push({ cmd: "MET", p, count: Number(m2[1]), unit: Number(m2[2]) });
      }
      else if (word === "CLICK") ast.events.push({ cmd: "CLK", p });
      else if (word === "STOP") {
        const c = clauses(toks, 2);
        if (c.until === undefined) err("STOP needs until");
        ast.events.push({ cmd: "STP", p, dur: c.until - p });
      }
      else if (word === "KEYZONE") {
        const [lane, width] = lw(toks, 2);
        const c = clauses(toks, 4);
        if (c.until === undefined || c.key === undefined) err("KEYZONE needs until and key");
        // grammar-accepted in [events]; canonical home is [notes]
        ast.items.push({ k: "keyzone", p, lane, width, endP: c.until, key: c.key });
      }
      else {
        if (!toks[2]?.startsWith("x")) err(`expected x<factor>, got "${toks[2]}"`);
        const factor = num(toks[2].slice(1));
        const c = clauses(toks, 3);
        if (c.until === undefined) err(`${word} needs until`);
        const cmd = word === "DEPTH" ? "DCM" : word === "SCROLL-DEAD" ? "SFE"
          : (c.key !== undefined ? "SLP" : "SFL");
        const e = { cmd, p, dur: c.until - p, factor };
        if (cmd === "SLP") e.key = c.key;
        ast.events.push(e);
      }
      continue;
    }

    // [notes]
    if (trimmed.startsWith("@")) {
      if (!/^@\d+$/.test(trimmed)) err(`bad measure header "${trimmed}"`);
      blockM = Number(trimmed.slice(1));
      currentChain = null;
      continue;
    }
    const toks = trimmed.split(/[ \t]+/);
    if (isStep || toks[0] === "~>" || toks[0] === "->") {
      if (!currentChain) err("step line outside a chain block");
      const arrow = toks[0];
      if (arrow !== "~>" && arrow !== "->") err(`bad arrow "${arrow}"`);
      const p = parseMt(toks[1]);
      ast.lineTicks.set(lineNo, p);
      const it = currentChain;
      const prevP = it.steps.length ? it.steps[it.steps.length - 1].p : it.p;
      // step: checkpoint lines highlight but never steer the follow-scroll
      ast.lineInfo.set(lineNo, { kind: CHAIN_KIND[it.k], p0: prevP, p1: p, step: true });
      const c = clauses(toks, 2);
      if (it.k === "slide") {
        const sevenField = c.ws.length > 0;
        if (!sevenField && (c.style !== undefined || c.fx !== undefined || c.note !== undefined)) {
          err("a step without w cannot carry style/fx/note (six-field form)");
        }
        const ex = c.exFlag ?? it.headEx;
        it.steps.push({
          p, lane: one(c.Ls, "L<lane>"), width: sevenField ? c.ws[0] : null,
          sevenField, marked: arrow === "->", ex,
          style: sevenField ? inherit(c.style, it.headStyle) : undefined,
          fx: sevenField && ex ? inherit(c.fx, it.headEx ? it.headFx : undefined) : undefined,
          end: c.end, airs: c.airs,
        });
      } else if (it.k === "ahold") {
        it.steps.push({ p, action: arrow === "->", note: c.note, end: c.end });
      } else if (it.k === "aslide") {
        it.steps.push({
          p, lane: one(c.Ls, "L"), width: one(c.ws, "w"), h1: one(c.hs, "h"),
          from: c.from, action: arrow === "->",
          style: inherit(c.style, it.headStyle), end: c.end, airs: c.airs,
        });
      } else if (it.k === "crush") {
        it.steps.push({ p, lane: one(c.Ls, "L"), width: one(c.ws, "w"), h1: one(c.hs, "h"),
          style: inherit(c.style, it.headStyle), end: c.end, airs: c.airs });
      } else if (it.k === "trace") {
        it.steps.push({ p, lane: one(c.Ls, "L"), width: one(c.ws, "w"),
          pA1: one(c.hAs, "hA"), pB1: one(c.hBs, "hB"),
          style: inherit(c.style, it.headStyle), end: c.end, airs: c.airs });
      } else if (it.k === "heaven") {
        it.steps.push({ p, lane: one(c.Ls, "L"), width: one(c.ws, "w"), h1: one(c.hs, "h"),
          end: c.end, airs: c.airs });
      } else err(`steps not allowed under ${it.k}`);
      if (c.end) currentChain = null;
      continue;
    }

    const word = toks[0];
    currentChain = null;
    if (toks[1] !== undefined && /^\d|^-|:/.test(toks[1])) {
      try { ast.lineTicks.set(lineNo, parseMt(toks[1])); } catch { /* headers */ }
    }
    if (word.startsWith("AIR:")) {
      const p = parseMt(toks[1]);
      const c = clauses(toks, 2);
      const [lane, width] = [one(c.Ls, "L"), one(c.ws, "w")];
      if (!c.orphan) err("AIR: diagnostic line must end with orphan");
      ast.lineInfo.set(lineNo, { kind: "air", p0: p, p1: p });
      ast.items.push({ k: "orphanAir", dir: word.slice(4), p, lane, width,
                       rootRaw: c.on, style: c.style });
      continue;
    }
    if (NOTE_WORDS.has(word)) {
      const p = parseMt(toks[1]);
      const c = clauses(toks, 2);
      ast.lineInfo.set(lineNo, {
        kind: { TAP: "tap", EXTAP: "extap", FLICK: "flick", DAMAGE: "damage" }[word],
        p0: p, p1: p });
      ast.items.push({ k: "note", family: word, p,
                       lane: one(c.Ls, "L"), width: one(c.ws, "w"),
                       note: c.note, airs: c.airs });
      continue;
    }
    if (HOLD_WORDS.has(word)) {
      const p = parseMt(toks[1]);
      const c = clauses(toks, 2);
      if (c.until === undefined) err(`${word} needs until`);
      ast.lineInfo.set(lineNo, { kind: "hold", p0: p, p1: c.until });
      ast.items.push({ k: "hold", family: word, p,
                       lane: one(c.Ls, "L"), width: one(c.ws, "w"),
                       endP: c.until, fx: c.fx === "none" ? undefined : c.fx,
                       airs: c.airs });
      continue;
    }
    if (word === "KEYZONE") {
      const p = parseMt(toks[1]);
      const c = clauses(toks, 2);
      if (c.until === undefined || c.key === undefined) err("KEYZONE needs until and key");
      ast.items.push({ k: "keyzone", p, lane: one(c.Ls, "L"),
                       width: one(c.ws, "w"), endP: c.until, key: c.key });
      continue;
    }
    if (!CHAIN_WORDS.has(word)) err(`unknown keyword "${word}"`);
    const p = parseMt(toks[1]);
    const c = clauses(toks, 2);
    const [lane, width] = [one(c.Ls, "L"), one(c.ws, "w")];
    let item;
    if (word === "SLIDE" || word === "EXSLIDE") {
      item = { k: "slide", family: word, p, lane, width,
               headStyle: c.style, headFx: c.fx === "none" ? undefined : c.fx,
               headEx: word === "EXSLIDE", steps: [] };
    } else if (word === "AIRHOLD") {
      if (c.on === undefined) err("AIRHOLD needs on <FAMILY>");
      item = { k: "ahold", p, lane, width, on: c.on, note: c.note, steps: [] };
    } else if (word === "AIRSLIDE") {
      if (c.on === undefined) err("AIRSLIDE needs on <FAMILY>");
      item = { k: "aslide", p, lane, width, h0: one(c.hs, "h<v>"), on: c.on,
               headStyle: c.style, steps: [] };
    } else if (word === "CRUSH") {
      if (c.every === undefined) err("CRUSH needs every <n>");
      item = { k: "crush", p, lane, width, h0: one(c.hs, "h<v>"),
               interval: c.every, headStyle: c.style, steps: [] };
      if (c.until !== undefined) {
        item.single = true;
        if (c.Ls.length < 2 || c.ws.length < 2 || c.hs.length < 2) {
          err("inline CRUSH needs end L/w/h after until");
        }
        item.steps.push({ p: c.until, lane: c.Ls[1], width: c.ws[1],
                          h1: c.hs[1], style: c.style, airs: c.airs });
      }
    } else if (word === "TRACE") {
      item = { k: "trace", p, lane, width, pA0: one(c.hAs, "hA"),
               pB0: one(c.hBs, "hB"), headStyle: c.style, steps: [] };
      if (c.until !== undefined) {
        item.single = true;
        if (c.Ls.length < 2 || c.ws.length < 2 || c.hAs.length < 2 || c.hBs.length < 2) {
          err("inline TRACE needs end L/w/hA/hB after until");
        }
        item.steps.push({ p: c.until, lane: c.Ls[1], width: c.ws[1],
                          pA1: c.hAs[1], pB1: c.hBs[1], style: c.style,
                          airs: c.airs });
      }
    } else {   // HEAVEN / EXHEAVEN
      if (c.disc === undefined) err(`${word} needs disc <n>`);
      item = { k: "heaven", family: word, p, lane, width, h0: one(c.hs, "h<v>"),
               disc: c.disc, fx: c.fx === "none" ? undefined : c.fx, steps: [] };
      if (c.until !== undefined) {
        item.single = true;
        if (c.Ls.length < 2 || c.ws.length < 2 || c.hs.length < 2) {
          err("inline HEAVEN needs end L/w/h after until");
        }
        item.steps.push({ p: c.until, lane: c.Ls[1], width: c.ws[1],
                          h1: c.hs[1], airs: c.airs });
      }
    }
    item.__line = lineNo;
    ast.lineInfo.set(lineNo, { kind: CHAIN_KIND[item.k], p0: p,
      p1: item.single && item.steps.length ? item.steps[0].p : p });
    ast.items.push(item);
    if (!item.single) currentChain = item;
    continue;
  }
  // chain headers highlight for their whole span
  for (const it of ast.items) {
    if (it.__line !== undefined && it.steps?.length) {
      const info = ast.lineInfo.get(it.__line);
      if (info) info.p1 = it.steps[it.steps.length - 1].p;
    }
  }
  return ast;
}
