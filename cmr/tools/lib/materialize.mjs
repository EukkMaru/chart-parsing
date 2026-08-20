// cmr AST -> viewer record pack, directly. Deployment-safe: this is the
// module that lets the shipped viewer build its model from cmr with no
// other chart format involved. The record objects carry exactly the fields
// the reference ingest produces; verify_corridor and verify_viewport hold
// the two paths equal.
import { R } from "./ast.mjs";

const STYLE_TABLE = [
  "DEF","RED","ORN","YEL","LIM","GRN","AQA","CYN",
  "DGR","BLU","PPL","VLT","PNK","GRY","BLK","NON",
];
const SLIDE_TABLE = ["SLD", "HLD", "GRN"];
const FEEDBACK_TABLE = ["UP","DW","CE","RC","LC","RS","LS","BS"];
const idx = (table, tok) => { const i = table.indexOf(tok || ""); return i < 0 ? 0 : i; };
const toTenths = v => Math.trunc(v * 10 + 0.5) * 0.1;
const clampW = v => Math.max(1, Math.min(16, v | 0));
const FAMILY_WORD = {
  TAP: "TAP", EXTAP: "CHR", FLICK: "FLK", DAMAGE: "MNE",
  HOLD: "HLD", EXHOLD: "HXD", SLIDE: "SLD", EXSLIDE: "SXD",
  CRUSH: "ALD", TRACE: "ASO", HEAVEN: "HHD", EXHEAVEN: "HHX",
};
const AIR_WORD_OF = { U: "AIR", UL: "AUL", UR: "AUR", D: "ADW", DL: "ADL", DR: "ADR" };
const KIND = {
  TAP: "tap", CHR: "tap", FLK: "tap", MNE: "tap",
  HLD: "hold", HXD: "hold", SLD: "slide", SXD: "slide", SLC: "slide", SXC: "slide",
  AIR: "air", AUL: "air", AUR: "air", ADW: "air", ADL: "air", ADR: "air",
  AHD: "ahold", AHX: "ahold", ASC: "aslide", ASD: "aslide",
  ALD: "ald", ASO: "asolid", HHD: "heaven", HHX: "heaven", SLA: "region",
};
const MIRROR_AIR = { AUL: "AUR", AUR: "AUL", ADL: "ADR", ADR: "ADL" };

const REGION_DEF = {
  STP: { label: "Stop",        color: "#e8a33d", factor: false, key: false },
  SFL: { label: "Scroll",      color: "#e8a33d", factor: true,  key: false },
  SLP: { label: "Scroll keyed",color: "#ffcc77", factor: true,  key: true  },
  DCM: { label: "Factor span", color: "#bb88dd", factor: true,  key: false },
  SFE: { label: "Rejected",    color: "#777788", factor: true,  key: false, dead: true },
  CLK: { label: "Click",       color: "#8899aa", factor: false, key: false, point: true },
};

export function materialize(ast, mirror) {
  const head = { resolution: 384, bpmDef: 150, metNum: 4, metDen: 4, creator: "", version: "" };
  const counts = new Map();
  const bump = cmd => counts.set(cmd, (counts.get(cmd) || 0) + 1);
  const META_TO_CMD = {
    "version": "VERSION", "music": "MUSIC", "sequence": "SEQUENCEID",
    "difficulty": "DIFFICULT", "level": "LEVEL", "creator": "CREATOR",
    "bpm-default": "BPM_DEF", "meter-default": "MET_DEF",
    "resolution": "RESOLUTION", "clock": "CLK_DEF",
    "progjudge-bpm": "PROGJUDGE_BPM", "progjudge-aer": "PROGJUDGE_AER",
    "tutorial": "TUTORIAL",
  };
  for (const { key, value } of ast.meta) {
    const cmd = META_TO_CMD[key];
    if (!cmd) continue;
    bump(cmd);
    if (key === "bpm-default") head.bpmDef = Number.parseFloat(value) || 0;
    else if (key === "meter-default") {
      const [count, unit] = value.split("/");
      head.metNum = Number.parseInt(unit, 10) || 0;
      head.metDen = Number.parseInt(count, 10) || 0;
    }
    else if (key === "creator") head.creator = value.replace(/[\t ]+/g, " ");
    else if (key === "version") head.version = value.split(/[\t ]+/)[0] || "";
  }
  for (const [name] of ast.statsOverrides) bump(name);

  const bpmEvents = [], metEvents = [], regions = [];
  const pos = p => [Math.floor(p / R), p - Math.floor(p / R) * R];
  for (const e of ast.events) {
    bump(e.cmd);
    const [m, t] = pos(e.p);
    if (e.cmd === "BPM") { bpmEvents.push({ m, t, bpm: e.bpm }); continue; }
    if (e.cmd === "MET") { metEvents.push({ m, t, n: e.unit, d: e.count }); continue; }
    const def = REGION_DEF[e.cmd];
    regions.push({
      cmd: e.cmd, def, sourceOrder: regions.length, m, t,
      dur: def.point ? 0 : e.dur,
      factor: def.factor ? e.factor : null,
      key: def.key ? e.key : 0,
    });
  }

  const rawNotes = [];
  let order = 0;
  const rec = (cmd, p, lane, width, extra) => {
    const [m, t] = pos(p);
    const n = {
      cmd, kind: KIND[cmd], color: undefined, sourceOrder: order++,
      m, t, lane, width: clampW(width),
      dur: 0, endLane: null, endWidth: null, h0: 0, h1: 0, style: "", root: "",
      ...extra,
    };
    if (mirror) {
      n.lane = (16 - n.lane - n.width) | 0;
      if (n.endLane !== null && n.endLane !== undefined &&
          (n.kind === "slide" || n.kind === "aslide" || n.kind === "ald" ||
           n.kind === "asolid" || n.kind === "heaven")) {
        n.endLane = (16 - n.endLane - (n.endWidth ?? n.width)) | 0;
      }
      if (n.kind === "air" && MIRROR_AIR[n.cmd]) n.cmd = MIRROR_AIR[n.cmd];
    }
    bump(cmd);                       // pre-mirror cmd counted? reference counts post-parse cmd; bump uses original arg
    rawNotes.push(n);
    return n;
  };
  const airs = (list, p, lane, width, ownerWord) => {
    for (const a of list || []) {
      rec(AIR_WORD_OF[a.dir], p, lane, width,
          { root: FAMILY_WORD[ownerWord] ?? ownerWord, style: a.style ?? "" });
    }
  };
  const slideExtras = (cmd, style, fx) => {
    const code = idx(SLIDE_TABLE, style);
    const ex = cmd === "SXD" || cmd === "SXC";
    return {
      style: style ?? "",
      slideStyleCode: code,
      slideExtendedRoot: ex,
      slideFeedbackCode: ex ? idx(FEEDBACK_TABLE, fx) : 0,
      slideRuntime: code === 1 ? "heaven_hold" : "slide",
      slideResultTableIndex: 0xff,
    };
  };

  for (const it of ast.items) {
    switch (it.k) {
      case "note":
        rec(FAMILY_WORD[it.family], it.p, it.lane, it.width, {});
        airs(it.airs, it.p, it.lane, it.width, it.family);
        break;
      case "hold":
        rec(FAMILY_WORD[it.family], it.p, it.lane, it.width,
            { dur: it.endP - it.p, style: it.fx ?? "" });
        airs(it.airs, it.endP, it.lane, it.width, it.family);
        break;
      case "slide": {
        let prev = { p: it.p, lane: it.lane, width: it.width };
        for (const s of it.steps) {
          const ex = s.ex ?? it.headEx;
          const cmd = ex ? (s.marked ? "SXD" : "SXC") : (s.marked ? "SLD" : "SLC");
          const w = s.sevenField ? s.width : prev.width;
          const style = s.style === undefined ? undefined
            : s.style;                          // resolved token or undefined
          const fx = ex ? s.fx : undefined;
          rec(cmd, prev.p, prev.lane, prev.width, {
            dur: s.p - prev.p, endLane: s.lane, endWidth: clampW(w),
            ...slideExtras(cmd, s.sevenField ? style : undefined,
                           s.sevenField ? fx : undefined),
          });
          airs(s.airs, s.p, s.lane, w, it.headEx ? "EXSLIDE" : "SLIDE");
          prev = { p: s.p, lane: s.lane, width: w };
        }
        break;
      }
      case "ahold": {
        let prev = it.p, first = true;
        for (const s of it.steps) {
          const cmd = s.action ? "AHX" : "AHD";
          rec(cmd, prev, it.lane, it.width, {
            root: first ? (FAMILY_WORD[it.on] ?? it.on) : "AHD",
            dur: s.p - prev,
            ignoredTrailingLabel: (first ? it.note : s.note) ?? "",
            isAction: s.action,
          });
          prev = s.p; first = false;
        }
        break;
      }
      case "aslide": {
        let prev = { p: it.p, lane: it.lane, width: it.width, h: it.h0 };
        let prevAction = null, first = true;
        for (const s of it.steps) {
          const cmd = s.action ? "ASD" : "ASC";
          const h0 = s.from !== undefined ? s.from : prev.h;
          rec(cmd, prev.p, prev.lane, prev.width, {
            root: first ? (FAMILY_WORD[it.on] ?? it.on) : (prevAction ? "ASD" : "ASC"),
            h0, dur: s.p - prev.p,
            endLane: s.lane, endWidth: clampW(s.width), h1: s.h1,
            style: (s.style ?? "") || "",
            styleCode: Math.max(0, STYLE_TABLE.indexOf(s.style ?? "")),
            isAction: s.action,
          });
          prev = { p: s.p, lane: s.lane, width: s.width, h: s.h1 };
          prevAction = s.action; first = false;
        }
        break;
      }
      case "crush": {
        let prev = { p: it.p, lane: it.lane, width: it.width, h: it.h0 };
        for (const s of it.steps) {
          const style = (s.style ?? "") || "";
          const code = Math.max(0, STYLE_TABLE.indexOf(style));
          rec("ALD", prev.p, prev.lane, prev.width, {
            interval: it.interval, h0: toTenths(prev.h),
            dur: s.p - prev.p, endLane: s.lane, endWidth: clampW(s.width),
            h1: toTenths(s.h1), style, styleCode: code,
            aldRuntime: it.interval === 0 && code === 15 ? "heaven_hold" : "air_ladder",
          });
          airs(s.airs, s.p, s.lane, s.width, "CRUSH");
          prev = { p: s.p, lane: s.lane, width: s.width, h: s.h1 };
        }
        break;
      }
      case "trace": {
        let prev = { p: it.p, lane: it.lane, width: it.width, pA: it.pA0, pB: it.pB0 };
        for (const s of it.steps) {
          const style = (s.style ?? "") || "";
          rec("ASO", prev.p, prev.lane, prev.width, {
            pA0: toTenths(prev.pA), pB0: toTenths(prev.pB),
            dur: s.p - prev.p, endLane: s.lane, endWidth: clampW(s.width),
            pA1: toTenths(s.pA1), pB1: toTenths(s.pB1),
            style, asoStyleCode: Math.max(0, STYLE_TABLE.indexOf(style)),
          });
          airs(s.airs, s.p, s.lane, s.width, "TRACE");
          prev = { p: s.p, lane: s.lane, width: s.width, pA: s.pA1, pB: s.pB1 };
        }
        break;
      }
      case "heaven": {
        const cmd = FAMILY_WORD[it.family];
        let prev = { p: it.p, lane: it.lane, width: it.width, h: it.h0 };
        for (const s of it.steps) {
          const disc = it.disc;
          rec(cmd, prev.p, prev.lane, prev.width, {
            h0: toTenths(prev.h), dur: s.p - prev.p,
            endLane: s.lane, endWidth: clampW(s.width), h1: toTenths(s.h1),
            heavenDiscriminator: disc, heavenParity: disc % 2 === 1,
            heavenExtended: cmd === "HHX",
            heavenFeedbackCode: cmd === "HHX" ? idx(FEEDBACK_TABLE, it.fx) : 0,
            heavenRuntime: "heaven_hold",
          });
          airs(s.airs, s.p, s.lane, s.width, "HEAVEN");
          prev = { p: s.p, lane: s.lane, width: s.width, h: s.h1 };
        }
        break;
      }
      case "keyzone":
        rec("SLA", it.p, it.lane, it.width, { dur: it.endP - it.p, key: undefined, tag: it.key });
        break;
      case "orphanAir":
        rec(AIR_WORD_OF[it.dir], it.p, it.lane, it.width,
            { root: it.rootRaw ?? "", style: it.style ?? "" });
        break;
    }
  }
  return { __pack: true, head, bpmEvents, metEvents, rawNotes,
           markers: [], regions, counts, unknown: new Map() };
}
