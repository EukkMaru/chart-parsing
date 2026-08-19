# HeavenHold note

## Parser and class selection

`HHD` and `HHX` have command IDs `0x2c` and `0x2d` and resolve to parsed type
13. Their exact data fields are root measure/tick/lane/width, root path scalar,
duration, ending lane/width, ending path scalar, and integer presentation
selector. The two scalars are rounded to integer tenths and later divided by
ten. HHD clears the command-form byte. HHX sets it and decodes one additional
token through the exact `UP/DW/CE/RC/LC/RS/LS/BS` table. Compatible commands
extend a saved type-13 chain; the parser requires matching command form, HHX
extra value when present, integer selector and parity, and prior endpoint
position/lane/span/scalar.

Type 13 also has a post-parse Slide origin. An SLD/SXD/SLC/SXC chain whose
field-8 exact style is `HLD`/code 1 is first constructed as type 2, then changed
to type 13 before generated-path construction. The rewrite stores integer 10
in the root/control path-scalar slots, so precompute consumes scalar `1.0`, and
preserves the Slide command-form flag. It does not write parsed `+0xb0`, the
separate presentation selector; that field retains constructor-default zero.
SLD/SLC preserve command-form zero; SXD/SXC preserve one. The local chart
snapshot contains two such lines in one chart, both SXD with the extended flag.

The same runtime class also has one type-9 entry:

```text
ALD sampling_interval == 0 && style_code == 15 (exact string NON)
```

All type-13 records, including rewritten Slide chains, and that exact ALD
exception allocate `0x348` bytes and construct RTTI-identified
`projView::HeavenHoldNote`. Missing ALD style is not `NON`: the out-of-range
token accessor returns an empty string and style lookup maps it to code 0, so
legacy one-short records construct ordinary AirLadder. Evidence:
`claim.note.slide-hld-heaven-retyping`, `claim.note.heaven-hold-judgement`, and
`claim.note.air-ladder-generated-checkpoints`.

## Generated path construction

Parsed-record initialization clears the primary generated vector at `+0x158`.
The chart postprocessor populates it for type 13 but not for exceptional type-9
ALD. The latter therefore loads an empty HeavenHold path queue by design.

For type 13, generation covers one root-to-final-end span on the shared
384-tick major-unit grid. The initial cursor is one tempo-adaptive step after
the root. After every sample, the next step is selected from that sample's
scheduled value. Strictly interior records are kind 0 and enabled by default.
When positive `TUTORIAL` enables the parser owner's exclusion feature,
membership in a key-zero selector-zero open interval clears an interior
record's emission flag. The final endpoint is appended as enabled kind 1 and
is not interval-filtered.

The duration token used to form the type-13 final endpoint is signed, and the
parser performs no range check. Grid conversion uses `CVTTSS2SI`; tick
subtraction/addition wraps at 32 bits. The wrapped endpoint delta and sampling
offset are compared as unsigned. A delta above `INT32_MAX` creates a very large
source range regardless of signed endpoint ordering; a zero adaptive step can
also leave the loop unable to progress. The clean-room API reports
`source_large_unsigned_span_expansion` plus the shared zero-step disposition
and does not attempt those unsafe source loops. Reconstruction:
`evaluate_heaven_hold_path_generation`.

Grid and schedule conversion plus BPM selection are specified in
`spec/timing.md`; path interpolation remains an explicit interface. Reconstruction:
`generate_heaven_hold_path_records`; tests:
`tests/heaven_hold_judgement_test.cpp`.

## Start checker and candidates

HeavenHold's start component is the shared TAP-style lane checker. Candidate
preparation first leaves every lane at the negative sentinel, then exposes the
embedded checker's candidates while start phase is not 4. The shared center-
window adjustment applies before manager reduction.

Start profile selection is:

```text
exceptional ALD, HHD, or rewritten SLD/SLC: 0 + input_variant_at_least_two
HHX or rewritten SXD/SXC:                    2 + input_variant_at_least_two
```

The actual windows and corrections are external parameters. A resolved start
submits source category 0 or 1 according to the standard variant-enabled and
variant-suppressed flags, performs feedback, and sets start phase 4. These map
to distinct shared categories 0 and 4.

## Sustain input and inactive-gap state

Every logical lane in the note span reads two physical banks in order. For
each lane:

1. check admission only for asserted banks, stopping at the first success;
2. if one succeeds, latch each asserted bank;
3. if none succeeds, latch each deasserted bank; and
4. after the scheduled start, mark a bank active only when both its raw source
   and retained latch are set.

Pre-start updates can change the latches but clear current activity. Forced
activity is ORed into the aggregate supplied to the shared inactive-gap
tracker; it does not overwrite per-bank activity. Reconstruction:
`HeavenHoldLaneContactState` and `update_heaven_hold_lane_contact`.

The path phase is 0 before its gate, 2 while the current gap classifier is
index 0, 3 for another current gap index, and 4 when the generated queue is
empty. Consequently exceptional ALD completes the path component at its first
eligible path update, while its start component can remain unresolved.

## Generated results and completion

Only the due front record is processed in one gameplay substep. Kind 0 submits
source category 2; kind 1 submits category 3. Both map to authoritative shared
category 1 under valid runtime bounds.

In ordinary mode, a due record is classified from the retained maximum and the
maximum is reset before emission is tested. A disabled record is consumed and
resets state but submits nothing. Forced selection bypasses that ordinary
classification/reset. It does not bypass record consumption or enable a
disabled record. Reconstruction:
`update_heaven_hold_generated_checkpoint` and
`heaven_hold_generated_source_category`.

The once-only adjusted-end branch chooses indexed feedback from the maximum
gap, but submits no gameplay result and changes no component phase. HeavenHold
is terminal only when both start phase and path phase equal 4; the main update
then requests the standard deferred transition. Construction clears phases,
the queue, bank latches/activity, and the feedback once flag. Destruction
releases the queue, checker, and resources. Reconstruction:
`heaven_hold_path_phase`, `heaven_hold_exposes_candidate`, and
`heaven_hold_is_terminal`.

## Authored presentation path

All three origins own the same two presentation resources: one root resource
and one body resource. Type-13 precompute emits one `0x34` presentation span
for every authored `0x24` control and a root-plus-authored-endpoint point
vector. The generated judgement queue never supplies body geometry. Every
authored endpoint independently selects its SLA key; presentation projects it
with that key rather than inheriting the root key.

Direct HHD/HHX's integer selector at parsed `+0xb0` drives two embedded tables:

```text
selector             0  1  2  3   4   outside 0..4
mirror endpoints     0  1  0  1   0   0
geometry mode        0  0  1  1  83h  83h
```

A mirror flag reflects every authored endpoint span about the root lane
center. A nonzero geometry mode, after any raw-relative-zero crossing trim,
reverses the surviving path and reflects projected, lateral, vertical, and
resource-coordinate values about the first/final midpoint. Retyped HLD and
zero/`NON` ALD retain selector zero, so neither transformation applies.

`NON` is consumed only by the earlier class-selection rule. HeavenHold preload
loads every width row of the ordinary root/body and attached-secondary body
tables, and runtime lookup branches only on width, command-form/alternate-root
state, and attached-secondary state. It does not read parsed type 9, style code
15, or the `NON` text. The executable therefore has no NON-specific null-
resource selector. External table contents remain unavailable: body
invisibility is independently explained by the exceptional ALD empty-path
phase reaching completion, while an absent start image in footage cannot be
attributed to a code-side NON selector without external-resource evidence.

The shared builder requires matching projected/path-vector cardinalities,
clips every span to projected range `[-600, 50]`, and emits one six-vertex
single-sided quad per surviving span. Vertices are `0x18` bytes. Lateral half
extent is decoded width times two render units, exactly the authored lane span
under the four-units-per-lane transform. The body stream uses topology 4.
HeavenHold has one body stream, not
Slide's or AirLadder's three streams.

The root resource is visible while start phase is not 4. The body resource is
visible while path phase is not 4. Path phase 2 selects animated mode 1; after
start completion, path phase 3 selects alternate mode 2 and the other
nonterminal states select mode 1; earlier states use base mode 0. Modes 0 and 1
use exact white `0xffffffff`; mode 1 also animates resource scale. Mode 2 uses
exact gray `0xff666666`. Root/body resource identities, textures, materials,
and final camera/pixels are external. Their selection and lifetime paths are
closed. Evidence:
`claim.presentation.heaven-hold-authored-mesh` and
`claim.presentation.sustain-endpoint-sla-selection`.
