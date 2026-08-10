# AirSolid (ASO)

## Gameplay status

`ASO` creates parsed type 10 and runtime `projView::AirSolidNote`. It is a
projected presentation path, not a judged note in this snapshot. It emits no
lane candidate, reads no gameplay input, and submits no result category. It
does have an ordinary scheduled lifetime: manager current position reaching
the authored end requests the shared deferred terminal state.

Evidence: `claim.note.air-solid-nonjudgement` and
`claim.presentation.air-solid-path`.

## Record fields

The twelve fields after `ASO` are:

| Field | Structural interpretation |
| ---: | --- |
| 1 | start major position, integer |
| 2 | start minor position, integer |
| 3 | start lane, integer |
| 4 | start width, integer clamped to 1 through 16 |
| 5 | start property A, float converted to integer tenths |
| 6 | start property B, float converted to integer tenths |
| 7 | duration in minor-position units, integer |
| 8 | end lane, integer |
| 9 | end width, integer clamped to 1 through 16 |
| 10 | end property A, float converted to integer tenths |
| 11 | end property B, float converted to integer tenths |
| 12 | case-sensitive color/style string |

For ordinary finite values in the integer domain, each property is:

```text
integer_property = trunc_toward_zero(float_property * 10.0 + 0.5)
```

The final conversion is the snapshot's `CVTTSS2SI`: NaN, infinity, and values
outside the signed 32-bit range produce integer-indefinite `INT32_MIN`.

The properties' exact runtime role is established below: they become the two
vertical surfaces of the rendered shell. Their player-facing names and intended
authored domains are not established, so the structural A/B names are
normative.

The end position is normalized from `(major, minor + duration)`. Mirrored
parsing transforms start and end lanes independently using their own clamped
widths:

```text
mirrored_lane = 16 - lane - endpoint_width
```

Duration addition and both mirror subtractions wrap at signed 32-bit width
before position normalization or lane storage.

The color/style table maps the following exact strings to codes 0 through 15:

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

## Precompute and runtime identity

Accepted source order supplies a sequence key. Before runtime-note construction,
scene setup converts each type-10 parsed path into a `0x5c`-byte sequence-keyed
projected configuration. AirSolidNote looks up that exact accepted-source key;
the lookup is not keyed by note type or chart position.

The precompute converts root and endpoint schedule scalars using `* 0.06`. The
root and every authored endpoint independently select an SLA projection tag.
Lane spans use the common decoded lane-center transform. The two integer-tenth
properties are restored with `* 0.1` and remain separate through geometry.

The style code selects this executable-owned coordinate table:

```text
0.1561999917  0.9688000083  0.9061999917  0.8436999917
0.7811999917  0.7186999917  0.6561999917  0.5938000083
0.5311999917  0.4688000083  0.4061999917  0.3438000083
0.2811999917  0.09399998188 0.03100001812 0
```

Codes 0 through 14 select an externally loaded primitive resource. Code 15,
`NON`, stores the missing-resource sentinel. The resource's contents are
external data; vertices use exact startup color `0xffffffff`. Preload reports
ready immediately, while primitive setup occurs during precompute/runtime load.

## Projection and geometry

The active update carries two schedule arrays:

- raw relative position, used to locate the judgement-plane seam;
- endpoint-SLA-adjusted position, used by the common depth projection.

For each endpoint:

```text
lateral = (decoded lane center - 8) * 4
vertical A = (property A - 1) * 3.8934999
vertical B = (property B - 1) * 3.8934999
half width = decoded width * 2
```

A segment is disabled when both projected endpoints are below `-600` or both
are above `50`. Otherwise, when projected span magnitude is at least
`0.000001`, either out-of-range endpoint is clipped independently. Clipping
linearly interpolates lateral position, both vertical properties, width, and
both neutral resource coordinates. It does not rewrite raw-relative position.

The first segment satisfying:

```text
raw start < 0.000001 and raw end > -0.000001
```

is split at the cached common projection of raw position zero. Its interpolation
fraction is:

```text
clamp(abs(raw start) /
      max(abs(raw end - raw start), 0.000001), 0, 1)
```

The seam interpolates lateral position, both vertical properties, width, and
both coordinates. Boundary flags at the duplicated seam are cleared so the
adjacent primitive portions do not both claim that boundary.

Every emitted vertex is six 32-bit fields (`0x18` bytes): lateral, vertical,
projected depth, exact packed color `0xffffffff`, coordinate U, and coordinate
V. This precompute uses U `0.5` and the selected style-table value for V. An
enabled segment can emit:

- an optional six-vertex bridge joining end surface A to start surface B;
- a 24-vertex shell made from four six-vertex quads around the two surfaces.

Triangle winding reverses only when end projected depth is strictly less than
start projected depth. Equality, increasing depth, and unordered comparisons
use the other winding.

Reconstruction: `AirSolidGeometryEndpoint`, `AirSolidGeometrySegment`,
`air_solid_*`, and `build_air_solid_vertices`. Focused test:
`tests/air_solid_presentation_test.cpp`.

## Judgement and lifetime boundary

Candidate collection writes the negative sentinel to all 16 lanes. The
gameplay manager's adjacent judgement slot is a shared no-op. A separate
AirSolid virtual compares stored authored end with manager current position,
and AirSolid's own scheduled lifetime update calls that predicate directly. At
`end <= current`, it invokes the common deferred request that changes active
state 1 to requested state 2. The subsequent geometry update observes that
request and skips rebuilding the mesh in the same tick.

Maintenance finalizes the sequence-keyed primitive resource. Destruction frees
the owned `0x18`-byte vertex storage and delegates shared base teardown. None of
these paths queries input or submits a result, so scheduled termination does
not change the no-judgement classification.

The endpoint SLA selection path is also covered by
`claim.presentation.sustain-endpoint-sla-selection`. Shared final camera,
layering, blend/material behavior, and scene composition remain cross-family
presentation work rather than family-local AirSolid ambiguity.
