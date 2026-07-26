# AirSolid (ASO) boundary

## Gameplay status

`ASO` creates parsed type 10 and runtime `projView::AirSolidNote`. It is a
projected presentation path, not a judged note in this snapshot. It emits no
lane candidate, reads no gameplay input, submits no result category, and has no
recovered terminal-state request. Rendering and resource details beyond this
exclusion boundary are outside scope.

Evidence: `claim.note.air-solid-nonjudgement`.

## Record fields

The twelve fields after `ASO` are:

| Field | Structural interpretation |
| ---: | --- |
| 1 | start major position, integer |
| 2 | start minor position, integer |
| 3 | start lane, integer |
| 4 | start width, integer clamped to 1 through 16 |
| 5 | start anonymous property A, float converted to integer tenths |
| 6 | start anonymous property B, float converted to integer tenths |
| 7 | duration in minor-position units, integer |
| 8 | end lane, integer |
| 9 | end width, integer clamped to 1 through 16 |
| 10 | end anonymous property A, float converted to integer tenths |
| 11 | end anonymous property B, float converted to integer tenths |
| 12 | case-sensitive color/style string |

For ordinary finite values in the integer domain, each anonymous property is:

```text
integer_property = trunc_toward_zero(float_property * 10.0 + 0.5)
```

The semantic names and authored domains of these four properties are not
established. They must remain structural parameters.

The end position is normalized from `(major, minor + duration)`. Mirrored
parsing transforms the start and end lanes independently using their own
clamped widths:

```text
mirrored_lane = 16 - lane - endpoint_width
```

The color/style table maps the following exact strings to codes 0 through 15
in order:

```text
DEF RED ORN YEL LIM GRN AQA CYN DGR BLU PPL VLT PNK GRY BLK NON
```

Missing, empty, or unknown style text maps to code 0. Shared missing and
malformed numeric behavior follows `spec/c2s.md`.

## Chaining

An ASO segment extends the first existing type-10 chain satisfying every
condition below:

- the existing chain has at least one endpoint;
- color/style codes are equal;
- the previous end lane and width equal the new start lane and width;
- both previous end integer-tenth properties equal the new start properties;
- the two chart-position scalars differ by at most `1/192`, inclusively.

If no chain matches, parsing appends a new type-10 record. A match appends the
new endpoint, regenerates the derived projected samples, and replaces the
chain's end position.

Reconstruction: `C2sAsoSegment`, `parse_c2s_aso_record`, and
`c2s_aso_segments_connect`. Focused test: `tests/aso_parser_test.cpp`.

## Runtime boundary

Accepted source order supplies a sequence key. Before runtime-note construction,
the scene setup converts each type-10 parsed path into a sequence-keyed
projected configuration. AirSolidNote loads the corresponding configuration
and its scheduled callback updates projected resource state.

Candidate collection writes the negative sentinel to all 16 lanes. A separate
AirSolid virtual compares a stored end-position scalar with manager current
position, but it is not the adjacent slot invoked by the recovered gameplay
manager. That manager slot is a shared no-op whose return is discarded. No
caller of the separate comparison and no ASO-specific terminal transition were
recovered. The ordinary active-vector teardown still destroys the object and
its owned path data at scene cleanup.
