// Reference model: the viewer's own parseC2s + association pass, executed
// verbatim (owner decision 2026-08-11: the viewer parser is the reference).
// DEV-SIDE ONLY — this module touches c2s and never ships to deployment.
import { readFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const here = dirname(fileURLToPath(import.meta.url));
const html = readFileSync(join(here, "../../scripts/c2s-viewer.html"), "utf8");
const script = html.split(/<script>/)[1].split(/<\/script>/)[0].split("\n");
const open = script.findIndex(l => l.trim() === "(() => {");
const end = script.findIndex(l => l.includes("document.getElementById"));
if (open < 0 || end <= open) throw new Error("cannot extract viewer parser");
const body = script.slice(open + 1, end)
  .filter(l => l.trim() !== '"use strict";').join("\n");

const ref = new Function(`
  const GUIDE_MERGE_MS = 8;
  ${body}
  return { parseC2s };
`)();

export const parseC2s = ref.parseC2s;

// Canonical comparable projection of a parsed chart (cmr SPEC §7 enumerated
// model). Construct-structural, order-insensitive where order is not
// load-bearing; unknown commands and rejected associations are counted
// separately, never compared (spec §8: the binary appends nothing either).
const num = v => (typeof v === "number" && Number.isFinite(v)) ? +v.toFixed(6) : v;

export function modelOf(chart) {
  const sig = n => [n.parsedType, n.p, n.lane, n.width, num(n.dur)].join(",");
  const chainKeyOf = new Map();   // chain object -> stable signature
  const chainKey = (chain, kind) => {
    if (!chainKeyOf.has(chain)) {
      const root = chain.root?.note ?? chain.root?.segment ?? chain.root ?? chain.segments?.[0];
      chainKeyOf.set(chain, kind + "@" + (root ? sig(root) : "?"));
    }
    return chainKeyOf.get(chain);
  };
  const recs = [];
  for (const n of chart.notes) {
    const r = {
      type: n.parsedType, p: n.p, lane: n.lane, width: n.width,
      dur: num(n.dur ?? 0),
      endLane: n.endLane ?? null, endWidth: n.endWidth ?? null,
      h0: num(n.h0 ?? 0), h1: num(n.h1 ?? 0),
    };
    switch (n.kind) {
      case "slide":
        r.styleCode = n.slideStyleCode; r.fb = n.slideFeedbackCode;
        r.marked = n.cmd === "SLD" || n.cmd === "SXD";
        r.ex = n.slideExtendedRoot;
        r.chain = chainKey(n.slideChain, "sld");
        r.ord = n.slideChain.segments.indexOf(n);
        break;
      case "heaven":
        r.fb = n.heavenFeedbackCode; r.disc = n.heavenDiscriminator;
        r.ex = n.heavenExtended;
        r.chain = chainKey(n.heavenChain, "hvn");
        r.ord = n.heavenChain.segments.indexOf(n);
        break;
      case "ald":
        r.styleCode = n.styleCode; r.interval = n.interval;
        r.chain = chainKey(n.aldChain, "ald");
        break;
      case "asolid":
        r.styleCode = n.asoStyleCode;
        r.pA0 = num(n.pA0); r.pB0 = num(n.pB0); r.pA1 = num(n.pA1); r.pB1 = num(n.pB1);
        r.chain = chainKey(n.asoChain, "aso");
        r.ord = n.asoChain.segments.indexOf(n);
        break;
      case "air":
        r.dir = n.cmd;
        r.owner = sig(n.attachedRoot.note);
        r.ownerType = n.attachedRoot.type;
        break;
      case "ahold":
        r.action = !!n.isAction;
        r.chain = chainKey(n.secondaryChain, "ahd");
        r.ord = n.secondaryChain.segments.indexOf(n);
        r.owner = n.secondaryChain.root ? sig(n.secondaryChain.root.note) : null;
        break;
      case "aslide":
        r.action = !!n.isAction; r.styleCode = n.styleCode;
        r.chain = chainKey(n.secondaryChain, "asl");
        r.ord = n.secondaryChain.segments.indexOf(n);
        r.owner = n.secondaryChain.root ? sig(n.secondaryChain.root.note) : null;
        break;
      case "region":
        r.tag = n.tag;
        break;
    }
    recs.push(r);
  }
  recs.sort((a, b) => a.p - b.p || a.type - b.type ||
    JSON.stringify(a).localeCompare(JSON.stringify(b)));

  const regions = {};
  for (const [key, list] of chart.regionsByKey) {
    regions[key] = list.map(r => ({
      p: r.p, start: num(r.start), end: num(r.end), factor: num(r.factor) }));
  }
  return {
    head: {
      resolution: chart.head.resolution, bpmDef: num(chart.head.bpmDef),
      metNum: chart.head.metNum, metDen: chart.head.metDen,
      // whitespace-run collapse: the direct text path tokenizes on runs,
      // the corridor row path preserves raw bytes (more faithful); the
      // model compares the collapsed form
      creator: chart.head.creator.replace(/[\t ]+/g, " "),
      version: chart.head.version,
    },
    seg: chart.seg.map(s => ({ p: s.p, bpm: num(s.bpm) })),
    meters: chart.meters.map(m => ({ p: m.p, n: m.n, d: m.d })),
    beats: chart.beats.map(b => [b.p, b.bar ? 1 : 0]),
    records: recs,
    regionsByKey: regions,
    dcm: chart.dcm.map(d => ({ start: num(d.start), end: num(d.end), factor: num(d.factor) })),
    rejects: chart.associationRejects.length,
  };
}

// [meta] extension: header values the viewer parser does not retain
// (spec §7 — DIFFICULT/CREATOR etc. compare via this side channel).
export function metaOf(text) {
  const keys = ["VERSION","MUSIC","SEQUENCEID","DIFFICULT","LEVEL","CREATOR",
    "BPM_DEF","MET_DEF","RESOLUTION","CLK_DEF","PROGJUDGE_BPM",
    "PROGJUDGE_AER","TUTORIAL"];
  const raws = new Set(["DIFFICULT", "CREATOR", "VERSION"]);
  const out = {};
  for (const raw of text.split(/\r?\n/)) {
    const cmd = raw.split(/[\t ]+/)[0];
    if (!keys.includes(cmd)) continue;
    let rest = raw.slice(raw.indexOf(cmd) + cmd.length).replace(/^[\t ]+/, "")
      .replace(/[\t ]+$/, "");
    if (!raws.has(cmd)) {
      // numeric header values compare by VALUE (SPEC: value equality is
      // what round-trips, not digit strings)
      rest = rest.split(/[\t ]+/)
        .map(t => { const v = Number.parseFloat(t); return Number.isNaN(v) ? t : String(v); })
        .join(" ");
    } else {
      rest = cmd === "CREATOR" ? rest : rest.replace(/[\t ]+/g, " ");
    }
    out[cmd] = rest;      // last duplicate wins, matching header state
  }
  return out;
}
