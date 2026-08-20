// Source-order association over raw c2s records -> cmr AST [notes] items.
// Mirrors the viewer's normalizeEventAssociations (the reference pass):
// sequential first-match continuation, first-compatible-root attachment
// with slot consumption, ALD block grouping per buildAirLadderChains.
import { PARSED_TYPE, AIR_PATH_ROOT_TYPES } from "./c2s_read.mjs";

const AIR_DIR = { AIR: "U", AUL: "UL", AUR: "UR", ADW: "D", ADL: "DL", ADR: "DR" };
export const TYPE_WORD = {
  0: "TAP", 4: "EXTAP", 6: "FLICK", 11: "DAMAGE", 1: "HOLD",
  2: "SLIDE", 9: "CRUSH", 10: "TRACE", 13: "HEAVEN",
};

const sameStart = (a, b) =>
  a.p === b.p && a.lane === b.lane && a.width === b.width;

// Build the cmr [notes] AST from raw group-2 records (source order).
// Returns { items, warnings, errors } — errors are §7.4 non-reproducible
// orderings and abort conversion upstream.
export function associate(records) {
  const items = [], warnings = [], errors = [];
  const roots = [];                   // viewer parity: source-order root list
  const slideChains = [], heavenChains = [], asoChains = [], aldChains = [];
  const airHoldChains = [], airSlideChains = [];

  const endpointOf = n => ({
    p: n.p + (n.dur ?? 0),
    lane: n.endLane ?? n.lane,
    width: n.endWidth ?? n.width,
  });
  const rootAttachmentPoint = root => {
    if (root.chain) {          // types 2/10/13 chains: current final control
      return { ...root.chain.last };
    }
    if (root.type === 1 || root.type === 9) {
      return endpointOf(root.note);
    }
    return { p: root.note.p, lane: root.note.lane, width: root.note.width };
  };
  const compatibleRoot = (requestedType, n) =>
    roots.find(root =>
      root.type === requestedType && root.secondaryType === null &&
      sameStart(rootAttachmentPoint(root), n));

  for (const n of records) {
    switch (n.cmd) {
      case "TAP": case "CHR": case "FLK": case "MNE": {
        const family = { TAP: "TAP", CHR: "EXTAP", FLK: "FLICK", MNE: "DAMAGE" }[n.cmd];
        const item = { k: "note", family, p: n.p, lane: n.lane, width: n.width,
                       note: n.note, airs: [] };
        items.push(item);
        roots.push({ type: PARSED_TYPE[n.cmd], note: n, secondaryType: null,
                     chain: null, item, airHost: item });
        break;
      }
      case "HLD": case "HXD": {
        const item = { k: "hold", family: n.cmd === "HXD" ? "EXHOLD" : "HOLD",
                       p: n.p, lane: n.lane, width: n.width,
                       endP: n.p + n.dur, fx: n.fx, airs: [] };
        items.push(item);
        roots.push({ type: 1, note: n, secondaryType: null, chain: null,
                     item, airHost: item });
        break;
      }
      case "SLD": case "SXD": case "SLC": case "SXC": {
        let chain = slideChains.find(c =>
          c.styleCode === n.styleCode && sameStart(c.last, n));
        if (!chain) {
          const item = { k: "slide", family: n.ex ? "EXSLIDE" : "SLIDE",
                         p: n.p, lane: n.lane, width: n.width,
                         headStyle: n.style, headFx: n.fx, headEx: n.ex,
                         steps: [] };
          items.push(item);
          chain = { styleCode: n.styleCode, item, last: null, root: n };
          slideChains.push(chain);
          roots.push({ type: 2, note: n, secondaryType: null, chain, item });
        }
        const step = { p: n.p + n.dur, lane: n.endLane,
                       width: n.sevenField ? n.endWidth : null,
                       sevenField: n.sevenField,
                       marked: n.marked, ex: n.ex,
                       style: n.style, fx: n.fx, airs: [] };
        chain.item.steps.push(step);
        chain.last = endpointOf(n);
        chain.lastStep = step;
        break;
      }
      case "HHD": case "HHX": {
        let chain = heavenChains.find(c =>
          c.ex === n.ex && c.fbCode === n.fbCode && c.disc === n.disc &&
          sameStart(c.last, n) && c.last.vertical === n.h0);
        if (!chain) {
          const item = { k: "heaven", family: n.ex ? "EXHEAVEN" : "HEAVEN",
                         p: n.p, lane: n.lane, width: n.width, h0: n.h0,
                         disc: n.disc, fx: n.fx, steps: [] };
          items.push(item);
          chain = { ex: n.ex, fbCode: n.fbCode, disc: n.disc, item,
                    last: null, root: n };
          heavenChains.push(chain);
          roots.push({ type: 13, note: n, secondaryType: null, chain, item });
        }
        const step = { p: n.p + n.dur, lane: n.endLane, width: n.endWidth,
                       h1: n.h1, airs: [] };
        chain.item.steps.push(step);
        chain.last = { ...endpointOf(n), vertical: n.h1 };
        chain.lastStep = step;
        break;
      }
      case "ASO": {
        let chain = asoChains.find(c =>
          c.styleCode === n.styleCode && sameStart(c.last, n) &&
          c.last.pA === n.pA0 && c.last.pB === n.pB0);
        if (!chain) {
          const item = { k: "trace", p: n.p, lane: n.lane, width: n.width,
                         pA0: n.pA0, pB0: n.pB0, headStyle: n.style, steps: [] };
          items.push(item);
          chain = { styleCode: n.styleCode, item, last: null, root: n };
          asoChains.push(chain);
          roots.push({ type: 10, note: n, secondaryType: null, chain, item });
        }
        const step = { p: n.p + n.dur, lane: n.endLane, width: n.endWidth,
                       pA1: n.pA1, pB1: n.pB1, style: n.style, airs: [] };
        chain.item.steps.push(step);
        chain.last = { ...endpointOf(n), pA: n.pA1, pB: n.pB1 };
        chain.lastStep = step;
        break;
      }
      case "ALD": {
        // Association: each ALD record is its own type-9 root; block
        // grouping follows buildAirLadderChains (interval + style code +
        // matching point incl. vertical).
        let chain = aldChains.find(c =>
          c.interval === n.interval && c.styleCode === n.styleCode &&
          sameStart(c.last, n) && c.last.vertical === n.h0);
        let step;
        if (!chain) {
          const item = { k: "crush", p: n.p, lane: n.lane, width: n.width,
                         h0: n.h0, interval: n.interval, headStyle: n.style,
                         steps: [] };
          items.push(item);
          chain = { interval: n.interval, styleCode: n.styleCode, item,
                    last: null };
          aldChains.push(chain);
        }
        step = { p: n.p + n.dur, lane: n.endLane, width: n.endWidth,
                 h1: n.h1, style: n.style, airs: [] };
        chain.item.steps.push(step);
        chain.last = { ...endpointOf(n), vertical: n.h1 };
        roots.push({ type: 9, note: n, secondaryType: null, chain: null,
                     item: chain.item, airHost: step });
        break;
      }
      case "AIR": case "AUL": case "AUR": case "ADW": case "ADL": case "ADR": {
        const requestedType = PARSED_TYPE[n.root];
        const root = requestedType === undefined ? undefined
          : compatibleRoot(requestedType, n);
        if (!root) {
          warnings.push(`orphan ${n.cmd} at ${n.at}`);
          items.push({ k: "orphanAir", dir: AIR_DIR[n.cmd], p: n.p,
                       lane: n.lane, width: n.width, rootRaw: n.root,
                       style: n.style });
          break;
        }
        root.secondaryType = 3;
        const air = { dir: AIR_DIR[n.cmd], style: n.style };
        if (root.airHost) {
          root.airHost.airs.push(air);
        } else {
          // chain root (slide/trace/heaven): the suffix must ride the end
          // step; a mid-chain attachment is a non-reproducible ordering.
          root.airChain = { chain: root.chain, air,
                           attachedAt: { ...rootAttachmentPoint(root) },
                           at: n.at };
          root.chain.pendingAirs = root.chain.pendingAirs || [];
          root.chain.pendingAirs.push(root.airChain);
        }
        break;
      }
      case "AHD": case "AHX": {
        const requestedType = PARSED_TYPE[n.root];
        let chain = null;
        if (requestedType === 5) {
          chain = airHoldChains.find(c => sameStart(c.last, n));
        } else if (AIR_PATH_ROOT_TYPES.has(requestedType)) {
          const root = compatibleRoot(requestedType, n);
          if (root) {
            root.secondaryType = 5;
            const item = { k: "ahold", p: n.p, lane: n.lane, width: n.width,
                           on: TYPE_WORD[requestedType], onRaw: n.root, note: n.note,
                           steps: [], ownerItem: root.item };
            items.push(item);
            chain = { item, last: null };
            airHoldChains.push(chain);
          }
        }
        if (!chain) {
          warnings.push(`dropped orphan ${n.cmd} at ${n.at}`);
          break;
        }
        const step = { p: n.p + n.dur, action: n.isAction,
                       note: chain.item.steps.length ? n.note : undefined };
        if (!chain.item.steps.length && n.note !== undefined) chain.item.note = n.note;
        chain.item.steps.push(step);
        chain.last = { p: n.p + n.dur, lane: n.lane, width: n.width };
        break;
      }
      case "ASC": case "ASD": {
        const requestedType = PARSED_TYPE[n.root];
        let chain = null;
        if (requestedType === 8) {
          const referencedAction = n.root === "ASD";
          chain = airSlideChains.find(c =>
            sameStart(c.last, n) && c.lastAction === referencedAction);
        } else if (AIR_PATH_ROOT_TYPES.has(requestedType)) {
          const root = compatibleRoot(requestedType, n);
          if (root) {
            root.secondaryType = 8;
            const item = { k: "aslide", p: n.p, lane: n.lane, width: n.width,
                           h0: n.h0, on: TYPE_WORD[requestedType], onRaw: n.root,
                           headStyle: n.style, steps: [], ownerItem: root.item };
            items.push(item);
            chain = { item, last: null };
            airSlideChains.push(chain);
          }
        }
        if (!chain) {
          warnings.push(`dropped orphan ${n.cmd} at ${n.at}`);
          break;
        }
        const prevEnd = chain.lastH1;
        const step = { p: n.p + n.dur, lane: n.endLane, width: n.endWidth,
                       h1: n.h1, action: n.isAction, style: n.style,
                       from: prevEnd !== undefined && prevEnd !== n.h0
                         ? n.h0 : undefined };
        chain.item.steps.push(step);
        chain.last = { p: n.p + n.dur, lane: n.endLane, width: n.endWidth };
        chain.lastAction = n.isAction;
        chain.lastH1 = n.h1;
        break;
      }
      case "SLA": {
        items.push({ k: "keyzone", p: n.p, lane: n.lane, width: n.width,
                     endP: n.p + n.dur, key: n.tag });
        break;
      }
    }
  }

  // Resolve chain air suffixes: legal only at the chain's true final point.
  for (const chains of [slideChains, heavenChains, asoChains]) {
    for (const chain of chains) {
      for (const pending of chain.pendingAirs || []) {
        if (sameStart(pending.attachedAt, chain.last)) {
          chain.lastStep.airs.push(pending.air);
        } else {
          errors.push(`AIR attached at a mid-chain endpoint (${pending.at}) — ` +
            `non-reproducible ordering (SPEC §7.4)`);
        }
      }
    }
  }

  // Single-record CRUSH/TRACE/HEAVEN use the inline `until` form.
  for (const item of items) {
    if ((item.k === "crush" || item.k === "trace" || item.k === "heaven") &&
        item.steps.length === 1) {
      item.single = true;
    }
  }
  return { items, warnings, errors };
}
