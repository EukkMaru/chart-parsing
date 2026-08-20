// cmr AST -> c2s text. DEV-SIDE ONLY: exists to certify the converters
// against the private corpus (SPEC §7); never ships, never runs at runtime.
import { R, fmt } from "./ast.mjs";

const WORD_TOKEN = {
  TAP: "TAP", EXTAP: "CHR", FLICK: "FLK", DAMAGE: "MNE",
  HOLD: "HLD", EXHOLD: "HXD", SLIDE: "SLD", EXSLIDE: "SXD",
  CRUSH: "ALD", TRACE: "ASO", HEAVEN: "HHD", EXHEAVEN: "HHX",
};
const AIR_CMD = { U: "AIR", UL: "AUL", UR: "AUR", D: "ADW", DL: "ADL", DR: "ADR" };

const pos = p => [Math.floor(p / R), p - Math.floor(p / R) * R];

// Token-row form: an array of rows, each row an array of string tokens.
// emitC2s joins them into c2s text; the viewer corridor consumes the rows
// directly so no c2s text is ever materialized on the cmr->model path.
export function emitC2sRows(ast) {
  const out = [];
  const line = toks => toks.map(fmt);
  const METACMD = {
    "version": "VERSION", "music": "MUSIC", "sequence": "SEQUENCEID",
    "difficulty": "DIFFICULT", "level": "LEVEL", "creator": "CREATOR",
    "bpm-default": "BPM_DEF", "meter-default": "MET_DEF",
    "resolution": "RESOLUTION", "clock": "CLK_DEF",
    "progjudge-bpm": "PROGJUDGE_BPM", "progjudge-aer": "PROGJUDGE_AER",
    "tutorial": "TUTORIAL",
  };
  for (const { key, value } of ast.meta) {
    const cmd = METACMD[key];
    if (key === "meter-default") {
      const [count, unit] = value.split("/");
      out.push([cmd, unit, count]);                      // re-inverted
    } else if (key === "creator") {
      out.push([cmd, value]);                            // raw bytes
    } else {
      out.push([cmd, ...value.split(/ +/)]);
    }
  }
  for (const [name, value] of ast.statsOverrides) {
    out.push([name, ...value.split(/ +/)]);
  }
  out.push([]);
  for (const e of ast.events) {
    const [m, t] = pos(e.p);
    switch (e.cmd) {
      case "BPM": out.push(line(["BPM", m, t, e.bpm])); break;
      case "MET": out.push(line(["MET", m, t, e.unit, e.count])); break;
      case "SFL": out.push(line(["SFL", m, t, e.dur, e.factor])); break;
      case "SLP": out.push(line(["SLP", m, t, e.dur, e.factor, e.key])); break;
      case "STP": out.push(line(["STP", m, t, e.dur])); break;
      case "DCM": out.push(line(["DCM", m, t, e.dur, e.factor])); break;
      case "SFE": out.push(line(["SFE", m, t, e.dur, e.factor])); break;
      case "CLK": out.push(line(["CLK", m, t])); break;
    }
  }
  out.push([]);

  // resolve a step's inherited clause token
  const resolved = (stepTok, headTok) =>
    stepTok === "none" ? undefined : (stepTok !== undefined ? stepTok : headTok);

  const emitAirs = (airs, p, lane, width, ownerWord) => {
    for (const a of airs || []) {
      const [m, t] = pos(p);
      const toks = [AIR_CMD[a.dir], m, t, lane, width, WORD_TOKEN[ownerWord] ?? ownerWord];
      if (a.style !== undefined) toks.push(a.style);
      out.push(line(toks));
    }
  };

  for (const it of ast.items) {
    const [m, t] = pos(it.p);
    switch (it.k) {
      case "note": {
        const toks = [WORD_TOKEN[it.family], m, t, it.lane, it.width];
        if (it.note !== undefined) toks.push(it.note);
        out.push(line(toks));
        emitAirs(it.airs, it.p, it.lane, it.width, it.family);
        break;
      }
      case "hold": {
        const toks = [WORD_TOKEN[it.family], m, t, it.lane, it.width, it.endP - it.p];
        if (it.fx !== undefined) toks.push(it.fx);
        out.push(line(toks));
        emitAirs(it.airs, it.endP, it.lane, it.width, it.family);
        break;
      }
      case "slide": {
        let prev = { p: it.p, lane: it.lane, width: it.width };
        for (const s of it.steps) {
          const ex = s.ex ?? it.headEx;
          const marked = s.marked;
          const cmd = ex ? (marked ? "SXD" : "SXC") : (marked ? "SLD" : "SLC");
          const [pm, pt] = pos(prev.p);
          const toks = [cmd, pm, pt, prev.lane, prev.width, s.p - prev.p, s.lane];
          const width = s.sevenField ? s.width : prev.width;
          if (s.sevenField) {
            toks.push(s.width);
            const st = resolved(s.style, it.headStyle);
            const fx = ex ? resolved(s.fx, it.headEx ? it.headFx : undefined) : undefined;
            if (st !== undefined || fx !== undefined) toks.push(st ?? "");
            if (fx !== undefined) toks.push(fx);
          }
          out.push(line(toks));
          emitAirs(s.airs, s.p, s.lane, width, it.headEx ? "EXSLIDE" : "SLIDE");
          prev = { p: s.p, lane: s.lane, width };
        }
        break;
      }
      case "ahold": {
        let prev = it.p, first = true;
        for (const s of it.steps) {
          const cmd = s.action ? "AHX" : "AHD";
          const rootTok = first ? (WORD_TOKEN[it.on] ?? it.on) : "AHD";
          const [pm, pt] = pos(prev);
          const toks = [cmd, pm, pt, it.lane, it.width, rootTok, s.p - prev];
          const label = first ? it.note : s.note;
          if (label !== undefined) toks.push(label);
          out.push(line(toks));
          prev = s.p; first = false;
        }
        break;
      }
      case "aslide": {
        let prev = { p: it.p, lane: it.lane, width: it.width, h: it.h0 };
        let prevAction = null, first = true;
        for (const s of it.steps) {
          const cmd = s.action ? "ASD" : "ASC";
          const rootTok = first ? (WORD_TOKEN[it.on] ?? it.on)
            : (prevAction ? "ASD" : "ASC");
          const [pm, pt] = pos(prev.p);
          const h0 = s.from !== undefined ? s.from : prev.h;
          const toks = [cmd, pm, pt, prev.lane, prev.width, rootTok, h0,
                        s.p - prev.p, s.lane, s.width, s.h1];
          const st = resolved(s.style, it.headStyle);
          if (st !== undefined) toks.push(st);
          out.push(line(toks));
          prev = { p: s.p, lane: s.lane, width: s.width, h: s.h1 };
          prevAction = s.action; first = false;
        }
        break;
      }
      case "crush": {
        let prev = { p: it.p, lane: it.lane, width: it.width, h: it.h0 };
        for (const s of it.steps) {
          const [pm, pt] = pos(prev.p);
          const toks = ["ALD", pm, pt, prev.lane, prev.width, it.interval,
                        prev.h, s.p - prev.p, s.lane, s.width, s.h1];
          const st = resolved(s.style, it.headStyle);
          if (st !== undefined) toks.push(st);
          out.push(line(toks));
          emitAirs(s.airs, s.p, s.lane, s.width, "CRUSH");
          prev = { p: s.p, lane: s.lane, width: s.width, h: s.h1 };
        }
        break;
      }
      case "trace": {
        let prev = { p: it.p, lane: it.lane, width: it.width, pA: it.pA0, pB: it.pB0 };
        for (const s of it.steps) {
          const [pm, pt] = pos(prev.p);
          const toks = ["ASO", pm, pt, prev.lane, prev.width, prev.pA, prev.pB,
                        s.p - prev.p, s.lane, s.width, s.pA1, s.pB1];
          const st = resolved(s.style, it.headStyle);
          if (st !== undefined) toks.push(st);
          out.push(line(toks));
          emitAirs(s.airs, s.p, s.lane, s.width, "TRACE");
          prev = { p: s.p, lane: s.lane, width: s.width, pA: s.pA1, pB: s.pB1 };
        }
        break;
      }
      case "heaven": {
        const cmd = WORD_TOKEN[it.family];
        let prev = { p: it.p, lane: it.lane, width: it.width, h: it.h0 };
        for (const s of it.steps) {
          const [pm, pt] = pos(prev.p);
          const toks = [cmd, pm, pt, prev.lane, prev.width, prev.h,
                        s.p - prev.p, s.lane, s.width, s.h1, it.disc];
          if (it.family === "EXHEAVEN" && it.fx !== undefined) toks.push(it.fx);
          out.push(line(toks));
          emitAirs(s.airs, s.p, s.lane, s.width, "HEAVEN");
          prev = { p: s.p, lane: s.lane, width: s.width, h: s.h1 };
        }
        break;
      }
      case "keyzone": {
        out.push(line(["SLA", m, t, it.lane, it.width, it.endP - it.p, it.key]));
        break;
      }
      case "orphanAir": {
        const toks = [AIR_CMD[it.dir], m, t, it.lane, it.width, it.rootRaw];
        if (it.style !== undefined) toks.push(it.style);
        out.push(line(toks));
        break;
      }
    }
  }
  out.push([]);
  return out;
}

export function emitC2s(ast) {
  return emitC2sRows(ast).map(r => r.join("\t")).join("\r\n");
}
