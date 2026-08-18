// Independent group-1 schedule reference for the browser corpus audit.
// Transcribed from spec/c2s.md and spec/timing.md; deliberately does not read
// the viewer's REGION table, parser helpers, or buildProjectionSchedules.
(() => {
  "use strict";

  const GROUP1 = Object.freeze({
    STP: { disposition: "keyed", factor: false, key: false },
    SFL: { disposition: "keyed", factor: true, key: false },
    SFE: { disposition: "rejected", factor: true, key: false },
    SLP: { disposition: "keyed", factor: true, key: true },
    DCM: { disposition: "factor", factor: true, key: false },
    CLK: { disposition: "point", factor: false, key: false },
  });

  const integer = value => {
    const parsed = Number.parseInt(value, 10);
    return Number.isNaN(parsed) ? 0 : parsed;
  };
  const floating = value => {
    const parsed = Number.parseFloat(value);
    return Number.isNaN(parsed) ? 0 : parsed;
  };

  function build(text, timeAt, resolution = 384) {
    const records = [];
    for (const line of text.split(/\r?\n/)) {
      const tokens = line.split(/[\t ]+/).filter(Boolean);
      if (!tokens.length) continue;
      const command = tokens[0];
      const shape = GROUP1[command];
      if (!shape) continue;
      const fields = tokens.slice(1);
      const position = integer(fields[0]) * resolution + integer(fields[1]);
      const duration = shape.disposition === "point"
        ? 0 : integer(fields[2]);
      const start = timeAt(position);
      const end = duration > 0 ? timeAt(position + duration) : start;
      records.push({
        command,
        disposition: shape.disposition,
        sourceOrder: records.length,
        position,
        start,
        end,
        factor: shape.factor ? floating(fields[3]) : null,
        key: shape.key ? integer(fields[4]) : 0,
      });
    }

    const regionsByKey = new Map();
    for (const record of records) {
      if (record.disposition !== "keyed" || record.end <= record.start) continue;
      const key = record.key || 0;
      const factor = record.command === "STP" ? 0 : record.factor;
      if (!regionsByKey.has(key)) regionsByKey.set(key, []);
      regionsByKey.get(key).push({
        p: record.position,
        start: record.start,
        end: record.end,
        factor,
      });
    }

    const dcm = records
      .filter(record => record.disposition === "factor")
      .sort((left, right) => left.sourceOrder - right.sourceOrder)
      .map(record => ({
        start: record.start,
        end: record.end,
        factor: record.factor,
      }));
    return { regionsByKey, dcm, recordCount: records.length };
  }

  // Keyed integration is additive, so equivalent-start interval order is not
  // observable. Canonicalize keyed entries as sets while preserving DCM's
  // explicitly source-ordered list.
  function canonical(schedule) {
    const regionsByKey = [...schedule.regionsByKey.entries()]
      .map(([key, intervals]) => [key, [...intervals].sort((left, right) =>
        left.p - right.p || left.start - right.start || left.end - right.end ||
        left.factor - right.factor)])
      .sort((left, right) => left[0] - right[0]);
    return JSON.stringify({ regionsByKey, dcm: schedule.dcm });
  }

  function clone(schedule) {
    return {
      regionsByKey: new Map([...schedule.regionsByKey.entries()].map(
        ([key, intervals]) =>
          [key, intervals.map(interval => ({ ...interval }))])),
      dcm: schedule.dcm.map(interval => ({ ...interval })),
    };
  }

  window.C2sScheduleReference = Object.freeze({ build, canonical, clone });
})();
