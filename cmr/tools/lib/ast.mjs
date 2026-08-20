// cmr AST <-> cmr text. The AST is the shared shape between c2cmr's
// association pass (assoc.mjs) and the emitters. This module is
// deployment-safe: it knows only cmr.
export const R = 384;

export const FX_WORD = new Map([
  ["UP","up"],["DW","down"],["CE","center"],["RC","right-center"],
  ["LC","left-center"],["RS","right-side"],["LS","left-side"],["BS","both-sides"],
]);
export const FX_TOKEN = new Map([...FX_WORD].map(([t, w]) => [w, t]));

export const META_KEY = new Map([
  ["VERSION","version"],["MUSIC","music"],["SEQUENCEID","sequence"],
  ["DIFFICULT","difficulty"],["LEVEL","level"],["CREATOR","creator"],
  ["BPM_DEF","bpm-default"],["MET_DEF","meter-default"],
  ["RESOLUTION","resolution"],["CLK_DEF","clock"],
  ["PROGJUDGE_BPM","progjudge-bpm"],["PROGJUDGE_AER","progjudge-aer"],
  ["TUTORIAL","tutorial"],
]);
export const META_CMD = new Map([...META_KEY].map(([c, k]) => [k, c]));
const STRING_META = new Set(["difficulty", "creator", "version"]);

export const fmt = v => {
  if (typeof v !== "number") return String(v);
  // minimal spelling; guard float artifacts from tenths arithmetic
  const r = Math.round(v * 1e6) / 1e6;
  return String(r);
};
export const mt = p => {
  const m = Math.floor(p / R), t = p - m * R;
  return `${m}:${String(t).padStart(3, "0")}`;
};
const fxOut = tok => FX_WORD.get(tok) ?? tok;

// clause helper: emit `word tok` per the presence/inheritance rule
const clause = (word, stepTok, headTok) => {
  if (stepTok === undefined) return headTok === undefined ? "" : ` ${word} none`;
  if (stepTok === headTok) return "";
  return ` ${word} ${word === "fx" ? fxOut(stepTok) : stepTok}`;
};
const airOut = airs => (airs || []).map(a =>
  ` +air:${a.dir}${a.style !== undefined ? ` style ${a.style}` : ""}`).join("");

export function emitCmr(ast) {
  const out = ["cmr 1", "", "[meta]"];
  for (const { key, value } of ast.meta) out.push(`${key}: ${value}`);
  out.push("stats: derived");
  if (ast.statsOverrides.length) {
    out.push("stats.overrides:");
    for (const [name, value] of ast.statsOverrides) out.push(`  ${name}: ${value}`);
  }
  out.push("", "[events]");
  for (const e of ast.events) {
    switch (e.cmd) {
      case "BPM": out.push(`BPM ${mt(e.p)} ${fmt(e.bpm)}`); break;
      case "MET": out.push(`METER ${mt(e.p)} ${fmt(e.count)}/${fmt(e.unit)}`); break;
      case "SFL": out.push(`SCROLL ${mt(e.p)} x${fmt(e.factor)} until ${mt(e.p + e.dur)}`); break;
      case "SLP": out.push(`SCROLL ${mt(e.p)} x${fmt(e.factor)} until ${mt(e.p + e.dur)} key ${fmt(e.key)}`); break;
      case "STP": out.push(`STOP ${mt(e.p)} until ${mt(e.p + e.dur)}`); break;
      case "DCM": out.push(`DEPTH ${mt(e.p)} x${fmt(e.factor)} until ${mt(e.p + e.dur)}`); break;
      case "SFE": out.push(`SCROLL-DEAD ${mt(e.p)} x${fmt(e.factor)} until ${mt(e.p + e.dur)}`); break;
      case "CLK": out.push(`CLICK ${mt(e.p)}`); break;
    }
  }
  out.push("", "[notes]");
  let blockM = null;
  const tick = p => {
    const m = Math.floor(p / R);
    if (blockM === null || m > blockM) { out.push(`@${m}`); blockM = m; }
    return m === blockM ? String(p - m * R).padStart(3, "0") : mt(p);
  };
  for (const it of ast.items) {
    switch (it.k) {
      case "note": {
        out.push(`${it.family} ${tick(it.p)} L${it.lane} w${it.width}` +
          (it.note !== undefined ? ` note ${it.note}` : "") + airOut(it.airs));
        break;
      }
      case "hold": {
        out.push(`${it.family} ${tick(it.p)} L${it.lane} w${it.width} until ${mt(it.endP)}` +
          (it.fx !== undefined ? ` fx ${fxOut(it.fx)}` : "") + airOut(it.airs));
        break;
      }
      case "slide": {
        out.push(`${it.family} ${tick(it.p)} L${it.lane} w${it.width}` +
          (it.headStyle !== undefined ? ` style ${it.headStyle}` : "") +
          (it.headFx !== undefined ? ` fx ${fxOut(it.headFx)}` : ""));
        it.steps.forEach((s, i) => {
          const last = i === it.steps.length - 1;
          let line = `  ${s.marked ? "->" : "~>"} ${mt(s.p)} L${s.lane}`;
          if (s.sevenField) {
            line += ` w${s.width}`;
            line += clause("style", s.style, it.headStyle);
            if (s.ex) line += clause("fx", s.fx, it.headEx ? it.headFx : undefined);
          }
          if (s.ex !== it.headEx) line += s.ex ? " ex" : " plain";
          if (last) line += " end";
          line += airOut(s.airs);
          out.push(line);
        });
        break;
      }
      case "ahold": {
        out.push(`AIRHOLD ${tick(it.p)} L${it.lane} w${it.width} on ${it.on}` +
          (it.note !== undefined ? ` note ${it.note}` : ""));
        it.steps.forEach((s, i) => {
          const last = i === it.steps.length - 1;
          out.push(`  ${s.action ? "->" : "~>"} ${mt(s.p)}` +
            (s.note !== undefined ? ` note ${s.note}` : "") + (last ? " end" : ""));
        });
        break;
      }
      case "aslide": {
        out.push(`AIRSLIDE ${tick(it.p)} L${it.lane} w${it.width} h${fmt(it.h0)} on ${it.on}` +
          (it.headStyle !== undefined ? ` style ${it.headStyle}` : ""));
        it.steps.forEach((s, i) => {
          const last = i === it.steps.length - 1;
          let line = `  ${s.action ? "->" : "~>"} ${mt(s.p)} L${s.lane} w${s.width} h${fmt(s.h1)}`;
          if (s.from !== undefined) line += ` from h${fmt(s.from)}`;
          line += clause("style", s.style, it.headStyle);
          if (last) line += " end";
          out.push(line);
        });
        break;
      }
      case "crush": {
        if (it.single) {
          const s = it.steps[0];
          out.push(`CRUSH ${tick(it.p)} L${it.lane} w${it.width} h${fmt(it.h0)} ` +
            `every ${it.interval} until ${mt(s.p)} L${s.lane} w${s.width} h${fmt(s.h1)}` +
            (it.headStyle !== undefined ? ` style ${it.headStyle}` : "") + airOut(s.airs));
          break;
        }
        out.push(`CRUSH ${tick(it.p)} L${it.lane} w${it.width} h${fmt(it.h0)} every ${it.interval}` +
          (it.headStyle !== undefined ? ` style ${it.headStyle}` : ""));
        it.steps.forEach((s, i) => {
          const last = i === it.steps.length - 1;
          out.push(`  ~> ${mt(s.p)} L${s.lane} w${s.width} h${fmt(s.h1)}` +
            clause("style", s.style, it.headStyle) + (last ? " end" : "") + airOut(s.airs));
        });
        break;
      }
      case "trace": {
        if (it.single) {
          const s = it.steps[0];
          out.push(`TRACE ${tick(it.p)} L${it.lane} w${it.width} hA ${fmt(it.pA0)} hB ${fmt(it.pB0)} ` +
            `until ${mt(s.p)} L${s.lane} w${s.width} hA ${fmt(s.pA1)} hB ${fmt(s.pB1)}` +
            (it.headStyle !== undefined ? ` style ${it.headStyle}` : "") + airOut(s.airs));
          break;
        }
        out.push(`TRACE ${tick(it.p)} L${it.lane} w${it.width} hA ${fmt(it.pA0)} hB ${fmt(it.pB0)}` +
          (it.headStyle !== undefined ? ` style ${it.headStyle}` : ""));
        it.steps.forEach((s, i) => {
          const last = i === it.steps.length - 1;
          out.push(`  ~> ${mt(s.p)} L${s.lane} w${s.width} hA ${fmt(s.pA1)} hB ${fmt(s.pB1)}` +
            clause("style", s.style, it.headStyle) + (last ? " end" : "") + airOut(s.airs));
        });
        break;
      }
      case "heaven": {
        const fx = it.fx !== undefined ? ` fx ${fxOut(it.fx)}` : "";
        if (it.single) {
          const s = it.steps[0];
          out.push(`${it.family} ${tick(it.p)} L${it.lane} w${it.width} h${fmt(it.h0)} ` +
            `disc ${it.disc}${fx} until ${mt(s.p)} L${s.lane} w${s.width} h${fmt(s.h1)}` +
            airOut(s.airs));
          break;
        }
        out.push(`${it.family} ${tick(it.p)} L${it.lane} w${it.width} h${fmt(it.h0)} disc ${it.disc}${fx}`);
        it.steps.forEach((s, i) => {
          const last = i === it.steps.length - 1;
          out.push(`  ~> ${mt(s.p)} L${s.lane} w${s.width} h${fmt(s.h1)}` +
            (last ? " end" : "") + airOut(s.airs));
        });
        break;
      }
      case "keyzone": {
        out.push(`KEYZONE ${tick(it.p)} L${it.lane} w${it.width} until ${mt(it.endP)} key ${it.key}`);
        break;
      }
      case "orphanAir": {
        out.push(`AIR:${it.dir} ${tick(it.p)} L${it.lane} w${it.width} on ${it.rootRaw}` +
          (it.style !== undefined ? ` style ${it.style}` : "") + " orphan");
        break;
      }
    }
  }
  out.push("");
  return out.join("\n");
}
