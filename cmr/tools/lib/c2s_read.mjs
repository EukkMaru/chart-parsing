// Raw c2s record reader for conversion. DEV-SIDE ONLY (touches c2s).
// Numeric rules per cmr SPEC §2 (recovered fallback): missing field = zero
// default; numeric prefix with trailing text converts WITH WARNING;
// no-conversion is a HARD ERROR. Width clamps to [1,16] with a warning.
// The reader keeps raw tokens so binary-ignored extras and AIRSLIDE raw
// float heights survive verbatim.

export const R = 384;

export const GROUP2 = new Set([
  "TAP","CHR","FLK","MNE","HLD","HXD","SLD","SXD","SLC","SXC",
  "AIR","AUL","AUR","ADW","ADL","ADR","AHD","AHX","ASC","ASD",
  "ALD","ASO","HHD","HHX","SLA",
]);
export const EVENTS = new Set(["BPM","MET","STP","SFL","SLP","DCM","SFE","CLK"]);
export const HEADER = ["VERSION","MUSIC","SEQUENCEID","DIFFICULT","LEVEL",
  "CREATOR","BPM_DEF","MET_DEF","RESOLUTION","CLK_DEF","PROGJUDGE_BPM",
  "PROGJUDGE_AER","TUTORIAL"];
export const PARSED_TYPE = {
  TAP: 0, HLD: 1, HXD: 1, SLD: 2, SXD: 2, SLC: 2, SXC: 2,
  AIR: 3, AUR: 3, AUL: 3, ADW: 3, ADR: 3, ADL: 3,
  CHR: 4, AHD: 5, AHX: 5, FLK: 6, ASD: 8, ASC: 8,
  ALD: 9, ASO: 10, MNE: 11, SLA: 12, HHD: 13, HHX: 13,
};
export const C2S_STYLE = [
  "DEF","RED","ORN","YEL","LIM","GRN","AQA","CYN",
  "DGR","BLU","PPL","VLT","PNK","GRY","BLK","NON",
];
export const SLIDE_STYLE = ["SLD", "HLD", "GRN"];
export const SLIDE_FEEDBACK = ["UP","DW","CE","RC","LC","RS","LS","BS"];
export const AIR_PATH_ROOT_TYPES = new Set([0, 1, 2, 4, 6, 11]);
export const DERIVED_CMDS = new Set([
  "T_REC_TAP","T_REC_CHR","T_REC_FLK","T_REC_MNE","T_REC_HLD","T_REC_SLD","T_REC_AIR","T_REC_AHD","T_REC_ALL",
  "T_NOTE_TAP","T_NOTE_CHR","T_NOTE_FLK","T_NOTE_MNE","T_NOTE_HLD","T_NOTE_SLD","T_NOTE_AIR","T_NOTE_AHD","T_NOTE_ALL",
  "T_NUM_TAP","T_NUM_CHR","T_NUM_FLK","T_NUM_MNE","T_NUM_HLD","T_NUM_SLD","T_NUM_AIR","T_NUM_AHD","T_NUM_AAC",
  "T_CHRTYPE_UP","T_CHRTYPE_DW","T_CHRTYPE_CE","T_CHRTYPE_RC","T_CHRTYPE_LC","T_CHRTYPE_RS","T_CHRTYPE_LS","T_CHRTYPE_BS",
  "T_LEN_HLD","T_LEN_SLD","T_LEN_AHD","T_LEN_ALL",
  "T_JUDGE_TAP","T_JUDGE_HLD","T_JUDGE_SLD","T_JUDGE_AIR","T_JUDGE_FLK","T_JUDGE_ALL",
]);
export const LEGACY_CMDS = new Set([
  "T_FIRST_MSEC","T_FIRST_RES","T_FINAL_MSEC","T_FINAL_RES",
  ...["00","05","10","15","20","25","30","35","40","45",
      "50","55","60","65","70","75","80","85","90","95"].map(s => "T_PROG_" + s),
]);

export const exactIndexOrZero = (table, value) => {
  const index = table.indexOf(value || "");
  return index < 0 ? 0 : index;
};
export const tenths = v => Math.trunc(v * 10 + 0.5) * 0.1;

export function makeReader(diag) {
  const int = (tok, where) => {
    if (tok === undefined || tok === "") return 0;
    const v = Number.parseInt(tok, 10);
    if (Number.isNaN(v)) throw new Error(`no-conversion integer "${tok}" at ${where}`);
    if (!/^[+-]?\d+$/.test(tok)) diag.warn(`numeric prefix "${tok}" at ${where}`);
    return v;
  };
  const float = (tok, where) => {
    if (tok === undefined || tok === "") return 0;
    const v = Number.parseFloat(tok);
    if (Number.isNaN(v)) throw new Error(`no-conversion float "${tok}" at ${where}`);
    return v;
  };
  const width = (tok, where) => {
    const v = int(tok, where);
    if (v < 1 || v > 16) diag.warn(`width ${v} clamped at ${where}`);
    return Math.max(1, Math.min(16, v));
  };
  return { int, float, width };
}

// Parse a full c2s text into { meta, statLines, events, records, unknown }.
// meta: ordered [key, rawValue]; statLines: ordered [name, rawValue] for
// registered T_* and legacy names; events/records keep source order.
export function readC2s(text, diag) {
  const rd = makeReader(diag);
  const meta = [], statLines = [], events = [], records = [];
  const unknown = new Map();
  let lineNo = 0;
  for (const raw of text.split(/\r?\n/)) {
    lineNo++;
    if (!raw.trim()) continue;
    const t = raw.split(/[\t ]+/).filter(s => s.length);
    const cmd = t[0], f = t.slice(1);
    const at = `line ${lineNo} (${cmd})`;
    if (HEADER.includes(cmd)) {
      // raw value: everything after the command token, tabs collapsed
      const rest = raw.slice(raw.indexOf(cmd) + cmd.length).replace(/^[\t ]+/, "")
        .replace(/[\t ]+$/, "");
      meta.push([cmd, rest]);
      continue;
    }
    if (DERIVED_CMDS.has(cmd) || LEGACY_CMDS.has(cmd)) {
      statLines.push([cmd, f.join(" ")]);
      continue;
    }
    if (EVENTS.has(cmd)) {
      const e = { cmd, m: rd.int(f[0], at), t: rd.int(f[1], at) };
      if (cmd === "BPM") e.bpm = rd.float(f[2], at);
      else if (cmd === "MET") { e.unit = rd.int(f[2], at); e.count = rd.int(f[3], at); }
      else if (cmd === "CLK") { /* point */ }
      else {
        e.dur = rd.int(f[2], at);
        if (cmd !== "STP") e.factor = rd.float(f[3], at);
        if (cmd === "SLP") e.key = rd.int(f[4], at);
      }
      events.push(e);
      continue;
    }
    if (GROUP2.has(cmd)) {
      const n = {
        cmd, at, sourceIndex: records.length,
        m: rd.int(f[0], at), t: rd.int(f[1], at),
        lane: rd.int(f[2], at), width: rd.width(f[3], at),
      };
      n.p = n.m * R + n.t;
      switch (cmd) {
        case "TAP": case "MNE": break;
        case "CHR": case "FLK":
          n.note = f[4];                       // binary-ignored extra, verbatim
          break;
        case "HLD": case "HXD":
          n.dur = rd.int(f[4], at);
          n.fx = f[5];                          // only HXD carries it in corpus
          break;
        case "SLD": case "SXD": case "SLC": case "SXC":
          n.dur = rd.int(f[4], at); n.endLane = rd.int(f[5], at);
          n.sevenField = f.length > 6;
          n.endWidth = n.sevenField ? rd.width(f[6], at) : n.width;
          n.style = f[7];
          n.styleCode = exactIndexOrZero(SLIDE_STYLE, n.style);
          n.ex = cmd === "SXD" || cmd === "SXC";
          n.fx = n.ex ? f[8] : undefined;
          n.marked = cmd === "SLD" || cmd === "SXD";
          break;
        case "AIR": case "AUL": case "AUR": case "ADW": case "ADL": case "ADR":
          n.root = f[4] || ""; n.style = f[5];
          break;
        case "AHD": case "AHX":
          n.root = f[4] || ""; n.dur = rd.int(f[5], at);
          n.note = f[6];                        // ignored trailing label
          n.isAction = cmd === "AHX";
          break;
        case "ASC": case "ASD":
          n.root = f[4] || "";
          n.h0raw = f[5] ?? "0"; n.h0 = rd.float(f[5], at);
          n.dur = rd.int(f[6], at);
          n.endLane = rd.int(f[7], at); n.endWidth = rd.width(f[8], at);
          n.h1raw = f[9] ?? "0"; n.h1 = rd.float(f[9], at);
          n.style = f[10];
          n.styleCode = exactIndexOrZero(C2S_STYLE, n.style);
          n.isAction = cmd === "ASD";
          break;
        case "ALD":
          n.interval = rd.int(f[4], at);
          n.h0 = tenths(rd.float(f[5], at));
          n.dur = rd.int(f[6], at);
          n.endLane = rd.int(f[7], at); n.endWidth = rd.width(f[8], at);
          n.h1 = tenths(rd.float(f[9], at));
          n.style = f[10];
          n.styleCode = exactIndexOrZero(C2S_STYLE, n.style);
          break;
        case "ASO":
          n.pA0 = tenths(rd.float(f[4], at)); n.pB0 = tenths(rd.float(f[5], at));
          n.dur = rd.int(f[6], at);
          n.endLane = rd.int(f[7], at); n.endWidth = rd.width(f[8], at);
          n.pA1 = tenths(rd.float(f[9], at)); n.pB1 = tenths(rd.float(f[10], at));
          n.style = f[11];
          n.styleCode = exactIndexOrZero(C2S_STYLE, n.style);
          break;
        case "HHD": case "HHX":
          n.h0 = tenths(rd.float(f[4], at));
          n.dur = rd.int(f[5], at);
          n.endLane = rd.int(f[6], at); n.endWidth = rd.width(f[7], at);
          n.h1 = tenths(rd.float(f[8], at));
          n.disc = rd.int(f[9], at);
          n.ex = cmd === "HHX";
          n.fx = n.ex ? f[10] : undefined;
          n.fbCode = n.ex ? exactIndexOrZero(SLIDE_FEEDBACK, n.fx) : 0;
          break;
        case "SLA":
          n.dur = rd.int(f[4], at); n.tag = rd.int(f[5], at);
          break;
      }
      records.push(n);
      continue;
    }
    unknown.set(cmd, (unknown.get(cmd) || 0) + 1);
  }
  return { meta, statLines, events, records, unknown };
}
