#!/usr/bin/env node
// cmr AST -> ilcmr text (the decompiler). Priority: sugar > canonical
// keyword form > ! escape (ruling 13). Deployment-safe: knows only
// cmr and ilcmr.
import { readFileSync, writeFileSync } from "node:fs";
import { fmt, mt, FX_WORD } from "../../cmr/tools/lib/ast.mjs";
import { parseCmr } from "../../cmr/tools/lib/parse_cmr.mjs";
import { emitCmr } from "../../cmr/tools/lib/ast.mjs";

const R = 384;
const gcd = (a, b) => b ? gcd(b, a % b) : a;
const durTxt = ticks => {
  if (ticks <= 0) return "[1:0]";
  const g = gcd(ticks, R);
  return `[${R / g}:${ticks / g}]`;
};
const AIR_GLYPH = { U: "^", UL: "^<", UR: "^>", D: "v", DL: "v<", DR: "v>" };
const span = (lane, width) => `${lane}-${lane + width}`;

// families whose decoded codes match the bare-sugar defaults
const slideSugarable = it =>
  it.steps.every(s => {
    const st = s.sevenField ? (s.style ?? it.headStyle) : undefined;
    return st === undefined || st === "SLD";
  }) && (it.headStyle === undefined || it.headStyle === "SLD");
const aslideSugarable = it =>
  (it.headStyle === undefined || it.headStyle === "DEF") &&
  it.steps.every(s => (s.style ?? it.headStyle) === undefined ||
                      (s.style ?? it.headStyle) === "DEF");

// Recompute secondary ownership from the cmr attachment rules (SPEC §5.3):
// first compatible root in item order, one secondary slot, attachment at
// the root's endpoint (hold end, chain final, own time for points).
const ON_TYPE = { TAP: 0, EXTAP: 4, FLICK: 6, DAMAGE: 11,
                  HOLD: 1, EXHOLD: 1, SLIDE: 2, EXSLIDE: 2 };
function resolveOwners(ast) {
  const roots = [];
  const point = it => {
    if (it.k === "note") return { p: it.p, lane: it.lane, width: it.width };
    if (it.k === "hold") return { p: it.endP, lane: it.lane, width: it.width };
    // slide: chain final point with six-field width resolution
    let w = it.width, lane = it.lane, p = it.p;
    for (const s of it.steps) { w = s.sevenField ? s.width : w; lane = s.lane; p = s.p; }
    return { p, lane, width: w };
  };
  for (const it of ast.items) {
    if (it.k === "note" || it.k === "hold" || it.k === "slide") {
      // a suffix AIR consumes the root's single secondary slot too
      const hasAir = (it.airs?.length ?? 0) > 0 ||
        (it.steps ?? []).some(x => (x.airs?.length ?? 0) > 0);
      roots.push({ type: it.k === "note" ? ON_TYPE[it.family]
                     : it.k === "hold" ? 1 : 2,
                   it, used: hasAir });
      continue;
    }
    if ((it.k === "ahold" || it.k === "aslide") && !it.ownerItem) {
      const want = ON_TYPE[it.on];
      const root = roots.find(r => !r.used && r.type === want &&
        (() => { const q = point(r.it);
                 return q.p === it.p && q.lane === it.lane && q.width === it.width; })());
      if (root) { root.used = true; it.ownerItem = root.it; }
    } else if (it.ownerItem) {
      // assoc-built AST: mark the owner's slot used for parity
      const root = roots.find(r => r.it === it.ownerItem);
      if (root) root.used = true;
    }
  }
}

export function decompile(ast, diag = { warn: () => {} }) {
  resolveOwners(ast);
  // ---- slots: pos -> {events: [], notes: []} ------------------------------
  const slots = new Map();
  const slot = p => {
    if (!slots.has(p)) slots.set(p, { events: [], notes: [] });
    return slots.get(p);
  };
  const escapes = new Map();       // tick -> {pre: [], post: []}
  const escCount = new Map();
  // meta: highlight span info for the viewer — {kind, t0, t1} in ticks
  const escapeLines = (p, lines, why, post, meta) => {
    if (!escapes.has(p)) escapes.set(p, { pre: [], post: [] });
    escapes.get(p)[post ? "post" : "pre"].push(...lines.map(line => ({ line, meta })));
    escCount.set(why, (escCount.get(why) || 0) + 1);
    slot(p);                       // every escape gets a serializer cell
  };
  const itemSpanMeta = it => {
    const lastP = it.steps?.length ? it.steps[it.steps.length - 1].p
      : (it.endP ?? it.p);
    const kind = { slide: "slide", crush: "air", ahold: "air", aslide: "air",
                   orphanAir: "air", heaven: "heaven", trace: "trace",
                   hold: "hold", note: "note", keyzone: "keyzone" }[it.k];
    return { kind, t0: it.p, t1: lastP };
  };
  // re-emit one AST item as verbatim cmr lines via a single-item emit
  const itemCmrLines = it => {
    const mini = { meta: [], statsOverrides: [], events: [], items: [it] };
    const body = emitCmr(mini).split("\n");
    const notesAt = body.indexOf("[notes]");
    return body.slice(notesAt + 1).filter(l => l.trim() && !l.startsWith("@"))
      .map(l => {
        const t = l.trim();
        // escape lines are self-timed: force full m:t on the head line
        if (!t.startsWith("~>") && !t.startsWith("->")) {
          const toks = t.split(" ");
          if (!toks[1].includes(":")) toks[1] = mt(it.p);
          return toks.join(" ");
        }
        return t;
      });
  };
  const eventCmrLines = e => {
    const mini = { meta: [], statsOverrides: [], events: [e], items: [] };
    const body = emitCmr(mini).split("\n");
    const at = body.indexOf("[events]");
    return [body[at + 1]];
  };

  // ---- events -> state changes -------------------------------------------
  const channels = new Map();      // name -> spans[]
  const chan = k => { if (!channels.has(k)) channels.set(k, []); return channels.get(k); };
  for (const e of ast.events) {
    switch (e.cmd) {
      case "BPM": slot(e.p).events.push(`(${fmt(e.bpm)})`); break;
      case "MET": slot(e.p).events.push(`(meter ${fmt(e.count)}/${fmt(e.unit)})`); break;
      case "CLK": slot(e.p).events.push(`(click)`); break;
      case "STP":
        if (e.dur < 0) escapeLines(e.p, eventCmrLines(e), "negative-stop");
        else slot(e.p).events.push(`(stop ${durTxt(e.dur)})`);
        break;
      case "SFL": chan("speed").push(e); break;
      case "SLP": chan(`speed key ${e.key}`).push(e); break;
      case "DCM": chan("depth").push(e); break;
      case "SFE": chan("scroll-dead").push(e); break;
    }
  }
  for (const [name, spans] of channels) {
    const word = name.startsWith("speed key") ? `speed` : name;
    const keySuffix = name.startsWith("speed key") ? " key " + name.split(" ")[2] : "";
    let ok = true;
    for (let i = 0; i < spans.length; i++) {
      if (spans[i].dur < 0 ||
          (i > 0 && spans[i].p < spans[i - 1].p + spans[i - 1].dur)) { ok = false; break; }
    }
    if (!ok) {
      for (const e of spans) escapeLines(e.p, eventCmrLines(e), "event-overlap");
      continue;
    }
    for (let i = 0; i < spans.length; i++) {
      const e = spans[i];
      slot(e.p).events.push(`(${word} ${fmt(e.factor)}${keySuffix})`);
      const end = e.p + e.dur;
      if (!(i + 1 < spans.length && spans[i + 1].p === end)) {
        slot(end).events.push(`(${word} off${keySuffix})`);
      }
    }
  }

  // ---- notes -> tokens ----------------------------------------------------
  // Phase A: dispositions. A primary is escaped when its decoded tokens
  // exceed the sugar defaults; a secondary is escaped when it cannot be a
  // suffix OR its owner is escaped (the escape stream keeps owner-then-
  // secondary order so reattachment works on recompile).
  const isSecondary = it => (it.k === "ahold" || it.k === "aslide") && it.ownerItem;
  const escaped = new Map();
  const escapeWhy = new Map();
  // Continuation attachment is first-created-chain-match; the suffix form
  // moves initial records to owner order, which flips creation order when
  // two chains share a junction point. Escape every construct involved in
  // a shared point so verbatim cmr keeps the source resolution.
  const junctionCollide = new Set();
  // Slide-chain crossings: two chains sharing any path point (same decoded
  // style code) can swap continuations when emission order shifts (e.g.
  // one escapes while its mirror stays sugar). Escape both so verbatim
  // order keeps the source resolution.
  const slideGroups = [];
  {
    const SLIDE_STYLE = ["SLD", "HLD", "GRN"];
    const byPoint = new Map();
    for (const it of ast.items) {
      if (it.k !== "slide") continue;
      const code = Math.max(0, SLIDE_STYLE.indexOf(it.headStyle ?? ""));
      let w = it.width;
      const pts = [`${it.p},${it.lane},${it.width},${code}`];
      for (const st of it.steps) {
        w = st.sevenField ? st.width : w;
        pts.push(`${st.p},${st.lane},${w},${code}`);
      }
      for (const key of new Set(pts)) {
        if (!byPoint.has(key)) byPoint.set(key, new Set());
        byPoint.get(key).add(it);
      }
    }
    for (const set of byPoint.values()) {
      if (set.size > 1) slideGroups.push([...set]);
    }
  }
  // Initial-attachment contention: several secondaries claiming the same
  // (point, root-family) map to interchangeable roots by ARRIVAL ORDER;
  // suffix emission reorders arrivals, so escape the whole contention
  // group — verbatim lines keep source order.
  {
    const byAttach = new Map();
    for (const it of ast.items) {
      if ((it.k !== "ahold" && it.k !== "aslide") || !it.ownerItem) continue;
      const key = `${it.p},${it.lane},${it.width},${ON_TYPE[it.on]}`;
      if (!byAttach.has(key)) byAttach.set(key, []);
      byAttach.get(key).push(it);
    }
    for (const list of byAttach.values()) {
      if (list.length > 1) for (const it of list) junctionCollide.add(it);
    }
  }
  for (const kind of ["ahold", "aslide"]) {
    const byKey = new Map();
    for (const it of ast.items) {
      if (it.k !== kind || !it.ownerItem) continue;
      const seen = new Set();
      for (const st of it.steps) {
        const key = kind === "ahold"
          ? `${st.p},${it.lane},${it.width}`
          : `${st.p},${st.lane},${st.width},${st.action ? 1 : 0}`;
        if (seen.has(key)) continue;
        seen.add(key);
        if (!byKey.has(key)) byKey.set(key, []);
        byKey.get(key).push(it);
      }
    }
    for (const list of byKey.values()) {
      if (list.length > 1) for (const it of list) junctionCollide.add(it);
    }
  }
  // primaries
  for (const it of ast.items) {
    if (it.k === "slide") {
      const mixedEx = it.steps.some(x => (x.ex ?? it.headEx) !== it.headEx);
      const fbBad = it.steps.some(x => {
        const fx = (x.ex ?? it.headEx) ? (x.fx ?? (it.headEx ? it.headFx : undefined)) : undefined;
        return fx !== undefined && fx !== "UP";
      }) || (it.headEx && it.headFx !== undefined && it.headFx !== "UP");
      if (!slideSugarable(it) || mixedEx || fbBad) {
        escaped.set(it, true); escapeWhy.set(it, "styled-slide");
      }
      continue;
    }
    if (it.k === "orphanAir") { escaped.set(it, true); escapeWhy.set(it, "orphan-air"); continue; }
    if (!isSecondary(it) && (it.k === "ahold" || it.k === "aslide")) {
      escaped.set(it, true); escapeWhy.set(it, "ownerless-secondary");
    }
  }
  // slide-crossing escalation: a colliding group whose members are all
  // sugar keeps cmr item order in the stream and is safe; once any member
  // is escaped the group's emission order shifts, so the whole group goes
  // verbatim. Iterate: escaping a group can infect overlapping groups.
  for (let changed = true; changed;) {
    changed = false;
    for (const group of slideGroups) {
      if (!group.some(it => escaped.get(it))) continue;
      for (const it of group) {
        if (!escaped.get(it)) {
          escaped.set(it, true); escapeWhy.set(it, "slide-junction");
          changed = true;
        }
      }
    }
  }
  // secondaries
  for (const it of ast.items) {
    if (!isSecondary(it)) continue;
    let why = null;
    if (escaped.get(it.ownerItem)) why = "escaped-owner";
    else if (junctionCollide.has(it)) why = it.k + "-junction";
    else if (it.k === "ahold" && it.steps.slice(0, -1).some(x => x.action)) why = "mixed-airhold";
    else if (it.k === "aslide" && !aslideSugarable(it)) why = "styled-airslide";
    else if (it.k === "aslide" && it.steps.some(x => x.from !== undefined)) why = "airslide-height-jump";
    if (why) { escaped.set(it, true); escapeWhy.set(it, why); }
  }

  // suffix builders
  const suffixOf = new Map();
  const addSuffix = (owner, x) => {
    if (!suffixOf.has(owner)) suffixOf.set(owner, []);
    suffixOf.get(owner).push(x);
  };
  const airTxt = airs => (airs || []).map(a => AIR_GLYPH[a.dir]).join("");
  const tagTxt = t => t === undefined ? "" : `.${t}`;
  // span-aware token builder: text plus sub-span highlight metadata
  const tok = () => ({ text: "", subs: [] });
  const addT = (t, piece, meta) => {
    if (!piece) return;
    if (meta && meta.kind) {
      t.subs.push({ s: t.text.length, e: t.text.length + piece.length, ...meta });
    }
    t.text += piece;
  };
  for (const it of ast.items) {
    if (!isSecondary(it) || escaped.get(it)) continue;
    const lastStepP = it.steps[it.steps.length - 1].p;
    if (it.k === "ahold") {
      const last = it.steps[it.steps.length - 1];
      let prev = it.p;
      let txt = `^h${last.action ? "x" : ""}${tagTxt(it.note)}`;
      it.steps.forEach((x, i) => { txt += (i ? "~" : "") + durTxt(x.p - prev); prev = x.p; });
      addSuffix(it.ownerItem, { txt, t0: it.p, t1: lastStepP, kind: "air" });
    } else {
      let prev = { p: it.p, h: it.h0 };
      let txt = `^s@${fmt(it.h0)}`;
      for (const x of it.steps) {
        txt += durTxt(x.p - prev.p) + (x.action ? ">" : "~") +
          span(x.lane, x.width) + (x.h1 !== prev.h ? `@${fmt(x.h1)}` : "");
        prev = { p: x.p, h: x.h1 };
      }
      addSuffix(it.ownerItem, { txt, t0: it.p, t1: lastStepP, kind: "air" });
    }
  }

  // Phase B: ordered walk
  for (const it of ast.items) {
    if (escaped.get(it)) {
      const post = slots.has(it.p) && slots.get(it.p).notes.length > 0;
      escapeLines(it.p, itemCmrLines(it), escapeWhy.get(it), post, itemSpanMeta(it));
      slot(it.p);                     // ensure a grid cell exists at this tick
      continue;
    }
    if (isSecondary(it)) continue;      // emitted as a suffix on its owner
    const sfxAdd = t => {
      for (const x of suffixOf.get(it) || []) {
        addT(t, x.txt, { kind: x.kind, t0: x.t0, t1: x.t1 });
      }
    };
    // airs flash at the owner's attachment point
    const airsAdd = (t, airs, atP) =>
      addT(t, airTxt(airs), { kind: "air", t0: atP, t1: atP });
    switch (it.k) {
      case "note": {
        const sig = { TAP: "", EXTAP: "x", FLICK: "f", DAMAGE: "d" }[it.family];
        let tag = "";
        if (it.family === "EXTAP") tag = it.note === "UP" ? "" : tagTxt(it.note ?? "");
        if (it.family === "FLICK") tag = it.note === "L" ? "" : tagTxt(it.note ?? "");
        const kind = { TAP: "tap", EXTAP: "extap", FLICK: "flick", DAMAGE: "damage" }[it.family];
        const t = tok();
        addT(t, `${sig}${span(it.lane, it.width)}${tag}`, { kind, t0: it.p, t1: it.p });
        airsAdd(t, it.airs, it.p);
        sfxAdd(t);
        slot(it.p).notes.push(t);
        break;
      }
      case "hold": {
        const sig = it.family === "EXHOLD" ? "hx" : "h";
        const tag = it.family === "EXHOLD" && it.fx !== undefined ? tagTxt(it.fx) : "";
        const t = tok();
        addT(t, `${sig}${span(it.lane, it.width)}${durTxt(it.endP - it.p)}${tag}`,
             { kind: "hold", t0: it.p, t1: it.endP });
        airsAdd(t, it.airs, it.endP);
        sfxAdd(t);
        slot(it.p).notes.push(t);
        break;
      }
      case "slide": {
        let txt = (it.headEx ? "sx" : "s") + span(it.lane, it.width);
        let prev = { p: it.p, lane: it.lane, width: it.width };
        let airs = [];
        let finalP = it.p;
        it.steps.forEach((s, i) => {
          const w = s.sevenField ? s.width : prev.width;
          txt += durTxt(s.p - prev.p) + (s.marked ? ">" : "~") + span(s.lane, w);
          prev = { p: s.p, lane: s.lane, width: w };
          airs.push(...(s.airs || []));
          finalP = s.p;
        });
        const t = tok();
        addT(t, txt, { kind: "slide", t0: it.p, t1: finalP });
        airsAdd(t, airs, finalP);
        sfxAdd(t);
        slot(it.p).notes.push(t);
        break;
      }
      case "keyzone": {
        const t = tok();
        addT(t, `keyzone ${span(it.lane, it.width)} key ${it.key} ${durTxt(it.endP - it.p)}`);
        slot(it.p).notes.push(t);
        break;
      }
      case "crush": case "trace": case "heaven": {
        const kind = { crush: "air", trace: "trace", heaven: "heaven" }[it.k];
        const lastP = it.steps.length ? it.steps[it.steps.length - 1].p : it.p;
        const meta = { kind, t0: it.p, t1: lastP };
        const head = it.k === "crush"
          ? `crush ${span(it.lane, it.width)}@${fmt(it.h0)} every ${it.interval}` +
            (it.headStyle !== undefined ? ` style ${it.headStyle}` : "")
          : it.k === "trace"
          ? `trace ${span(it.lane, it.width)}@${fmt(it.pA0)}:${fmt(it.pB0)}` +
            (it.headStyle !== undefined ? ` style ${it.headStyle}` : "")
          : `${it.family === "EXHEAVEN" ? "heavenx" : "heaven"} ` +
            `${span(it.lane, it.width)}@${fmt(it.h0)} disc ${it.disc}` +
            (it.fx !== undefined ? tagTxt(it.fx) : "");
        const t = tok();
        addT(t, head, meta);
        let prev = it.p;
        for (const s of it.steps) {
          const legSpan = it.k === "trace"
            ? `${span(s.lane, s.width)}@${fmt(s.pA1)}:${fmt(s.pB1)}`
            : `${span(s.lane, s.width)}@${fmt(s.h1)}`;
          addT(t, ` ${durTxt(s.p - prev)} > ${legSpan}`, meta);
          airsAdd(t, s.airs, s.p);
          if (it.k !== "heaven") {
            const st = s.style ?? it.headStyle;
            if (st !== it.headStyle) {
              addT(t, st === undefined ? " style none" : ` style ${st}`, meta);
            }
          }
          prev = s.p;
        }
        sfxAdd(t);
        slot(it.p).notes.push(t);
        break;
      }

    }
  }

  // ---- serialize ----------------------------------------------------------
  const out = [], outTicks = [], outSpans = [];
  const push = (line, tick, spans) => {
    out.push(line); outTicks.push(tick ?? null); outSpans.push(spans ?? []);
  };
  for (const { key, value } of ast.meta) push(`&${key} ${value}`);
  push("");
  const positions = [...slots.keys()].sort((a, b) => a - b);
  const lastP = positions.length ? positions[positions.length - 1] : 0;
  const lastMeasure = Math.max(Math.floor(lastP / R),
    ...[...escapes.keys()].map(p => Math.floor(p / R)), 0);
  let division = null;
  let cursor = 0;                  // next unemitted position index
  for (let m = 0; m <= lastMeasure; m++) {
    const inMeasure = [];
    while (cursor < positions.length && positions[cursor] < (m + 1) * R) {
      inMeasure.push(positions[cursor]); cursor++;
    }
    let div = 1;
    if (inMeasure.length) {
      let g = R;
      for (const p of inMeasure) g = gcd(g, p - m * R);
      div = R / g;
    }
    // line builder: identical text to the old parts.join(" ") scheme, plus
    // char-offset highlight spans for the viewer panes
    let lb = null;
    const lbEnsure = () => { if (!lb) lb = { text: "", spans: [], tick: null }; };
    const lbPart = (piece, subs) => {
      lbEnsure();
      const base = lb.text.length + (lb.text.length ? 1 : 0);
      if (subs) for (const x of subs) {
        lb.spans.push({ ...x, s: x.s + base, e: x.e + base });
      }
      lb.text += (lb.text.length ? " " : "") + piece;
    };
    const flush = () => {
      if (lb && lb.text.length) push(lb.text, lb.tick, lb.spans);
      lb = null;
    };
    const escSpan = (line, meta) => meta && meta.kind
      ? [{ s: 0, e: line.length + 2, kind: meta.kind, t0: meta.t0, t1: meta.t1,
           step: /^(~>|->)/.test(line) }]
      : [];
    if (div !== division) { lbPart(`{${div}}`); division = div; }
    const step = R / div;
    for (let i = 0; i < div; i++) {
      const p = m * R + i * step;
      const esc = escapes.get(p);
      if (esc && esc.pre.length) {
        flush();
        for (const { line, meta } of esc.pre) push(`! ${line}`, p, escSpan(line, meta));
      }
      const s = slots.get(p);
      let cellText = "";
      const cellSubs = [];
      if (s) {
        cellText = s.events.join(" ");
        if (s.notes.length) {
          const joiner = s.notes.some(n => n.text.includes(" ")) ? " / " : "/";
          if (cellText) cellText += " ";
          s.notes.forEach((n, idx) => {
            if (idx) cellText += joiner;
            for (const x of n.subs) {
              cellSubs.push({ ...x, s: x.s + cellText.length, e: x.e + cellText.length });
            }
            cellText += n.text;
          });
        }
      }
      lbEnsure();
      if (lb.tick === null) lb.tick = p;
      lbPart(cellText + ",", cellSubs);
      if (esc && esc.post.length) {
        flush();
        for (const { line, meta } of esc.post) push(`! ${line}`, p, escSpan(line, meta));
      }
    }
    lbPart("|");
    flush();
  }
  push("");
  for (const [why, n] of escCount) diag.warn(`escape fallback: ${why} x${n}`);
  decompile.lastLineTicks = outTicks;
  decompile.lastSpans = outSpans;
  return out.join("\n");
}

const isMain = process.argv[1] && import.meta.url.endsWith(process.argv[1].split("/").pop());
if (isMain) {
  const file = process.argv[2];
  if (!file) { console.error("usage: cmr2ilc.mjs <chart.cmr> [-o out.ilcmr]"); process.exit(2); }
  const ast = parseCmr(readFileSync(file, "latin1"));
  const text = decompile(ast, { warn: w => console.error(`warn: ${w}`) });
  const o = process.argv.indexOf("-o");
  if (o >= 0) writeFileSync(process.argv[o + 1], text, "latin1");
  else process.stdout.write(text);
}
