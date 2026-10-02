# Identification tools operator runbook

Checked setup and continuation: 2026-10-01. Run commands from the repository root. These tools
produce research evidence; they never add accepted rows to `config/symbols.csv`
or establish reconstruction matching coverage. The identity, ABI/type and
complete-extent rules in [identification.md](identification.md) still apply.

Identification jobs run sequentially under a repository lock and a default
**1 GiB address-space limit**. `--memory-limit-mb` changes the limit explicitly;
Production scoring fits the default. The broader registry-family holdout needs
an explicit 1.5 GiB cap and peaks at about 999 MiB RSS; its 1 GiB attempt aborts
without publishing a completed run. A budget up to about 4 GiB is available when
a larger reviewed batch needs it. Measure peak RSS and keep heavy
jobs serialized. A memory-limit failure aborts publication.
Do not run raw graph loaders concurrently or bypass this guarded entry point for
full originals. An earlier unbounded parallel pilot exhausted memory; its partial
outputs are not valid results. Raw graph extraction and bounded synthetic tests
remain independently verified.

The raw JSONL files remain authoritative. Local SQLite indices provide bounded
node, edge and incidence queries; they are derived caches rather than accepted
symbol maps. The SQLite implementation is part of Python's standard library.

Unique map-to-ELF data names remain advisory labels and holdout controls. Function
propagation uses explicit identity-reviewed or alias data pairs from the journal.
This separates cross-binary name hypotheses from reviewed observations and avoids
generating uninformative Cartesian products for widely used globals.

Measured original-binary validation: the disk-backed loader peaked at 121 MiB;
the fixed-selection graph/BSim/BinDiff run at 765 MiB; the Customiser/data holdout
at 642 MiB. All ran sequentially under the limit. The same 100 hidden labels gave
graph-only top-five retrieval 68, BSim 68, and graph-plus-BSim 85. Adding BinDiff
kept top-five retrieval at 85 and reduced top-one from 68 to 65, so BinDiff remains
optional. These are retrieval measurements on existing labels, not verified new
PC mappings. See the ignored `fixed-selection/`, `customiser-chain/` and
`method-comparison.json` artifacts under `build/analysis/identification/`.

The data holdout hid 204 name/review labels: 95 were retrievable, 66 had the
expected top-one pair, and 85 had it in the top five. Fifty pairs passed the
advisory shortlist filter; 48 agreed with the existing labels. The two mismatches
remain review work, and no proposals enter the accepted map. The reviewed
accessory-state anchor recovered `Customiser_DumpAccessories` at `0x005930c0`
in the first round with two function roots and the reviewed data root.

Holding out the complete `nu2api/nufile/nufile.cpp` family hid 44 function labels.
The first round retrieved 23 in the top five; propagation proposed 20, all
consistent with the hidden labels. That run peaked at 763 MiB. The journal's
verified 211-byte function extent increased the research inventory from 913,001
to 913,212 reviewed image bytes; its unknown data extent added no bytes. These
research extents do not change the accepted CSV or matching report.

The original data-only run replayed against its legacy state with zero added,
removed or changed function/data candidates, peaking at 694 MiB. Its compact
state was 29.6 MB, and its empty semantic delta was 150 bytes. Full function
evidence remains in the separate JSONL ledger.

Replaying with the full reviewed journal then added 30 data candidates and
changed 26 function evidence rows (two added, three removed), at 695 MiB peak.
The reviewed Customiser function left the tentative ledger and became a seed;
the random-state rejection persisted. This demonstrates anchor-driven replay
without promoting old tentative candidates to confirmed roots. The retained
data-holdout metrics are in `customiser-data-holdout-holdout.json`; the latest
`customiser-chain/` contains the full-journal replay.

## Reviewed mapping continuation

The first bounded review release accepted five functions totaling 1,064 PC code
bytes: Customiser dumping, character-icon drawing and scene lookup, plus the
animation buffer loader and its wrapper. The Customiser state global received
an identity-only row with blank type and size. See the accepted
[mapping notes](symbols.md#reviewed-graph-and-bsim-mapping-release-october-2026).
There are 4,690 address rows plus one inline record, covering 914,065 image bytes.
These additions have no compiled definitions and claim no reconstruction gain.

The reviewed matrix disagreement is a valid semantic duplicate: two separate PC
allocations correspond to one Android object. The render-state disagreement is
an interior field mistaken for its aggregate base; its exact base pairing is
rejected. Alias-connected data records now count as one independent support and
contribute their maximum weight. Repeated storage correspondences cannot satisfy
the two-root propagation rule by themselves.

Data and family holdouts quarantine entire alias components and report extra
excluded pairs. Held-object rejection decisions also stay out of evaluation
training. An explicit two-object check recovered both expected bases in the top
five, but ranked the render-state field first without the withheld rejection.
It excluded the sibling matrix alias and kept only the unrelated random-state
rejection. This is a useful remaining ownership limitation, rather than a new
identity claim. The check peaked at 642 MiB under the 1 GiB guard.

Five already-mapped callback entries, totaling 244 bytes, were repaired in an
independent copy of the PC Ghidra project. Restricted disassembly and exact
reviewed bodies restored `Condition_Timer`, `Condition_Random`,
`Condition_AlwaysTrue`, `CheckOutputConditionFlowBox` and `ProcessActionFlowBox`.
The saved copy was reopened read-only and exported; all existing function bodies
were preserved. Original project file hashes and metadata were unchanged. The
Android export was reused unchanged. This adds graph evidence, with no new map
bytes or reconstruction credit. Java used a 512 MiB heap and peaked at 756 MiB
RSS; small reviews used 256 MiB Python caps.
The incremental replay gained five accepted function seeds (1,176 total),
removed five obsolete alternatives for `Condition_Random`, and preserved every
data candidate. It peaked at 697 MiB under the 1 GiB Python guard. The repaired
PC graph has 7,844 functions; the Android graph remains at 13,242.

A subsequent repair includes four more reviewed Flow callbacks, for nine entries
and 537 already-mapped bytes total. It preserves every existing function body,
restores the direct `ProcessActionFlowBox` → `PerformActionFlowBox` connection,
and retains unresolved table dispatches. The PC graph now has 7,848 functions.
This larger repair peaked at 737 MiB RSS with the same 512 MiB heap.

Original review then accepted `MovePlayer_DIRECTIONAL` (10,904 bytes),
`SetLevelSfxBits` (8,845 bytes), and three four-byte level-pointer cells. Their
19,761 bytes raise mapped image coverage to 933,826 / 40,067,072 bytes (2.331%).
The functions remain unimplemented and their complete aggregate layouts remain
unreviewed. The new pointer-cell contracts do not establish pointee layouts.
Replay uses 1,182 accepted function seeds and seven reviewed data correspondences,
with 1,243 tentative function pairs and 1,731 data candidates. Relative to the
five-callback replay, data rows gained 72, changed 214 and removed 18; function
evidence rows gained 114, changed 835 and removed 167. These are research deltas,
not acceptance or accuracy metrics. Peak Python RSS was 695 MiB.

Joint mode now needs only its pinned graph pair, originals, symbol map and
optional reviewed journal. It does not capture legacy function/reference TSVs,
the old Android call graph, accepted-symbol cache or feature caches. Explicit
pair paths can relocate the original; the supported PC hash and image/code
bounds remain mandatory, and inventory reads that same verified original.
Legacy mode keeps its existing input defaults.

Without the optional Android source-tree ELF reader, a valid ELF32 i386 original
still permits call/data matching. Android literal observations are then disabled
with an explicit reason in `literal_features`; missing reader code is never
replaced with guessed labels. Malformed ELF headers/table bounds still fail.
Fresh-repository tests exercise this path with a genuine minimal ELF, absent
legacy files/default original, and invalid stale caches.

Five further reviewed level-pointer cells add 20 mapped bytes, bringing accepted
image coverage to 933,846 bytes (2.331%). The portable full replay used 12 reviewed
data correspondences, retained every data candidate, and changed three function
evidence rows without new tentative pairs. Its peak RSS was 697 MiB under the
1 GiB guard. No unused legacy input roles occur in its completed manifest.

The next connected review accepted `PartKill_ForceThrow` (371 PC bytes),
`ZipUp_MoveCode` (3,774), `Action_RemoveThrownForceObjects` (75), the four-byte
CloudCityTrap_B pointer cell, and the eleven-byte GrapWindLp character array.
These five rows add 4,235 mapped bytes, bringing image coverage to 938,081 bytes
(2.341%). Their ABI/storage contracts follow original callers, callback dispatch
and initializer rows; full aggregate layouts remain unresolved. They add no
compiled definitions or reconstruction credit.

The removal callback's original named table row supplied its missing entry seed.
A subsequent disposable function repair recovered its 75-byte node while
preserving all 7,848 prior function bodies and the two corrected data owners.
The new graph has 7,849 PC functions. The combined replay used 1,185 accepted
function seeds and fourteen reviewed data correspondences, with 1,243 tentative
function pairs and 1,759 data candidates. It reports zero stale decisions.
Compared with the portable replay, data rows gained 37, changed 185 and removed
nine; function evidence rows gained eighteen, changed 400 and removed thirty.
These remain hypothesis deltas. Peak Python RSS was 695 MiB under the serialized
1 GiB guard; the callback Ghidra repair peaked at 748 MiB with a 512 MiB heap.

A following original registry/literal wave accepts four action callbacks
(1,016 PC bytes) and three ZipUp sound arrays (32 bytes), adding 1,048 mapped bytes.
Accepted image coverage is now 939,129 bytes (2.344%). Two Play registry names
share one physical callback in both original binaries; the reconstructed Android
source's NULL entries do not override that original evidence. Independent
original-byte review confirms the callback dispatcher, used arguments, complete
reachable bodies and stack balance. Full aggregate layouts remain unreviewed.

GrapSwing had the same preceding-scalar ownership error, this time beside a
four-byte float. Explicit float32 repair support recreates that scalar and the
ten-byte literal, preserving all original bytes and 7,849 existing functions.
The subsequent three-function repair preserves both typed owners and all prior
functions, giving 7,852 PC graph functions. Peak Ghidra RSS was 761 MiB for data
repair and 763 MiB for function repair, each with a 512 MiB heap.

The resulting replay uses 1,189 accepted function seeds and seventeen reviewed
data correspondences; it has 1,242 tentative function pairs, 1,763 data candidates,
and zero stale decisions. Compared with the preceding replay, four data rows are
added; none change or disappear. Function evidence changes 63 rows and removes
twelve, with none added. Peak Python RSS was 694 MiB under the 1 GiB guard.
The repaired GrapWindLp owner has two accepted PC users, so it contributes no
new candidate data roots; this is expected. A bounded post-replay audit finds no
duplicate site/owner observations and confirms accepted ZipUp is excluded from
tentative endpoints.

The next obstacle review accepts five complete function identities (669 PC
bytes) and six character-ID storage identities with unknown allocation extents.
The latter have blank CSV sizes and ctypes, false type/extent review flags and
zero coverage credit. Accepted image coverage becomes 939,798 bytes (2.346%).
These inventory additions have no compiled implementations or reconstruction
gain; the original PC readers/writers and independent allocation limitations
are recorded in [symbols.md](symbols.md).

An AI-family preflight exposed 43 missing PC nodes among 70 confirmed map/ELF
pairs. Independent original name/callback records and full CFG reviews recovered
all 43 complete entries (11,448 original bytes) in three disposable repair batches,
preserving every previous function extent and both corrected scalar/literal
owner pairs. The final PC graph has 7,895 functions; Android remains at 13,242.
One retained two-byte instruction at `0x00534f4e` incorrectly split the original
four-byte `LEA ESP,[ESP]` at `0x00534f4c`. The first batch-three attempt rolled
back and published no graph pair. Its explicitly reviewed replacement verifies
the exact old instruction bounds/hash before clearing only that unit in the
copy. All other code/data/function conflicts remain fatal. Successful Ghidra
jobs peaked at 749--762 MiB with 512 MiB heaps.

The family correspondence holdout freezes the original accepted map/journal,
all 74 Android family variants and 70 unambiguous labels. Before repair, graph
coverage was 27/70, final recovery 20/27 represented pairs or 20/70 overall, and
round-one top-five retrieval 24/27. After repair, all 70 inputs are represented:
final recovery is 25/70 (all 25 proposals correct on these labels), round-one
top-one 27/70 and top-five 43/70. Every earlier final recovery survives. The five
new registry callbacks recover only 1/5, counting the two Play aliases once.
Data-name labels quarantined from training increase from 24 to 31, while the
same seventeen independently checked reviewed anchors and 1,162 function seeds
remain training. Original binary names/literals are allowed; this is not a
registry-name-blind experiment or probability calibration. Independent audits
verify both frozen input sets, partitions and denominators. Peak Python RSS is
767 MiB before and 796 MiB after repair under the 1 GiB guard.

The following production replay uses the current map/journal: 1,237 accepted
function seeds, 23 reviewed data correspondences, 1,239 tentative function pairs,
1,776 data candidates and zero stale decisions. Compared with the preceding
registry replay, data rows add fifteen, change 79 and remove two; function
evidence adds two, changes 176 and removes 207. Those are research changes,
not new verified symbols. Peak RSS is 630 MiB. Shared topology and the common
`false` literal leave many flag callbacks ambiguous. Reviewed original registry
records are the next retrieval channel; raw DWORD/byte masks and field offsets
must not be equated across platforms.

The original registry diagnostic now extracts 261 PC and 262 Android records
from independently reviewed complete tables. Original initialization and dispatch
code establishes separate game/API namespaces: PC has 207 game records versus
Android's 208, and both have 54 API records. `DontPush`, `SetLocator` and
`FollowPlayer` occur in both namespaces; a name alone cannot identify them.
The Android pointer addends already contain original VAs. No second relocation
or graph address adjustment is applied. The PC and Android table bounds, null
terminators, names, callbacks and namespace proofs are independently checked.

A following reviewed wave adds `GizObstacles_RegisterGizmo` and sixteen AI
callbacks, totaling 2,995 complete PC bytes, plus an obstacle type-ID identity
with unknown allocation extent and zero coverage credit. Full aggregate types
remain unreviewed. Image mapping becomes 942,793 bytes (2.353%); source matching
does not increase. The registration repair adds one missing graph entry, and
the AI repair adds fifteen; the sixteenth callback already has a retained node.
All 7,895 previous function bodies and corrected scalar/literal owners survive.
The current PC graph has 7,911 functions. These Ghidra jobs peaked below 761 MiB
with 512 MiB heaps.

The current production replay uses 1,254 accepted function seeds and 24 reviewed
data anchors, with 1,241 tentative function pairs, 1,775 data candidates and zero
stale decisions. Its separate registry diagnostic yields 131 physical candidate
pairs, no ambiguous joins and 130 unresolved names: 129 missing PC function nodes
and one Android-only name, `CharClipToBlobShadows`. Alias records count once per
physical callback pair. Peak Python RSS was 632 MiB under the 1 GiB limit.
The diagnostic finds the counterparts of all seventy frozen AI labels using
original registry names. This is lookup coverage, not a new holdout score: the
graph correspondence result remains 25/70. Registry outputs neither seed nor
change graph scoring, and no diagnostic candidate is automatically accepted.

The next reviewed wave adds 33 AI callbacks, three obstacle callbacks and one
registration-name literal: 7,366 complete function bytes and twelve data bytes.
Image mapping reaches 950,171 bytes (2.371%). Six AI bodies already existed;
the remaining 27 AI entries and three obstacle entries are repaired in disposable
projects. The PC graph has 7,941 functions. A retained one-byte `INC ESI` at `0x455345` split the
original two-byte `JLE` at `0x455344`; the initial repair rolled back. Independent
original CFG review and an exact listing/hash proof authorize replacing only
that stale instruction. The successful repair rechecks the source listing and
retained-project preservation, and peaks below 763 MiB with a 512 MiB heap.

The preceding incremental replay freezes the previous completed state and reviewed
map/journal. It uses 1,289 accepted function seeds and 25 reviewed data anchors,
with 1,236 tentative function pairs, 1,795 data candidates and zero stale
decisions. Data hypotheses add twenty, change thirteen and remove none; function
evidence changes 200 and removes 130, with none added. The separate registry
diagnostic now has 158 physical candidate pairs, no ambiguous joins and 103
unresolved names: 102 missing PC entries and the same Android-only name. None
of those diagnostic pairs alter scoring or acceptance. Peak Python RSS is
678 MiB under the 1 GiB guard. Mapping increases; reconstruction does not.

A separate bounded review identifies sixteen original AI registry name-storage
pairs, but holds their full allocation sizes/types and journal integration:
Android graph endpoints are absent. First-NUL content and rounded Ghidra string
owners alone do not establish complete allocation extents. The obstacle
output-count callback also remains held for missing original PC caller ABI
proof. These concrete limitations remain in ignored review packets rather than
becoming speculative coverage.

`tools/repair_graph.py` makes the independent copy, pins the reviewed packet and
executed script, checks a fresh repair report, and verifies the saved function
extents in the new export before publishing a graph pair. Use a fresh output
under `build/analysis`; the packet schema and required range hashes are described
in `tools/ghidra/RepairReviewedFunctions.java`. Function repair refuses conflicting
data or code except exact, explicitly listed `superseded_instructions` inside a
reviewed body. Each optional row pins `start`, integer `size` (1--15 bytes) and
`range_sha256`; duplicate/overlapping rows, entry-instruction replacement and
existing function bodies are forbidden. A fresh report must identify precisely
those cleared units. Missing reports fail even when headless exits successfully.
Repair does not infer signatures or analyze the whole program.

Complete function extents include compiler-owned local tables and internal
alignment after terminal instructions. Optional `excluded_ranges` pin these
bytes by start/size/hash and kind `padding` or `switch-table32`; they stay outside
restricted disassembly. Tables additionally require aligned four-byte cells,
explicit original targets and `guard_reviewed: true`. Every stored target must
equal its reviewed value and resolve to a code instruction in the function.
An existing defined table owner requires `existing_data: {data_type_path: ...}`
with exact matching start, size and type. No new data definition is inferred.

Optional `replacement_body` pins the exact old analyzed ranges, each original
range hash and total `body_bytes`. It authorizes a strict extension to the full
reviewed extent. The saved report must echo this metadata and mark the extension
`extended-reviewed`; every other prior function remains identical. Reopened
exports must retain the new full bodies and all explicitly pinned data owners.
The obstacle output/name bodies require extensions from 147/49 to 168/72 bytes;
`SetSide` extends from 1,463 instruction bytes to 1,496 bytes including one NOP
and two guarded four-cell tables. Following INT3 alignment is excluded. Exact
old body proofs and eighteen four-byte `/pointer` owners constrain these three
extensions; no broader extent or owner replacement is permitted.

The real mixed-body repair succeeds with a 512 MiB heap and 744 MiB peak RSS.
The reopened graph retains all 7,941 prior functions, changing only those three
explicit extents, all eighteen pointer owners and both corrected scalar/literal
pairs. The corrected incremental replay uses 1,290 accepted function seeds and
25 reviewed data anchors, with 1,235 tentative function pairs, 1,795 data
candidates and zero stale decisions. Relative to the preceding replay, twelve
data hypotheses and sixteen function evidence rows change, one tentative
function disappears, and none are added. The registry diagnostic remains at
158 physical candidates and 103 unresolved names. Peak Python RSS is 679 MiB
under the 1 GiB limit. All original reconstruction comparison rows and 123
object hashes remain unchanged; mapped bytes rise without source matching gain.

The following wave adds 31 complete AI callback identities (7,783 PC bytes)
and sixteen name-storage identities with unknown allocation extents/types.
The storage rows have blank CSV size/ctype and false extent/type/ABI flags,
without complete ranges or coverage credit. Mapping reaches 957,954 bytes
(2.391%). Game `SetLocator` retains a distinct reconstruction name from its API
namesake, and SetPath retains its complete 388-byte span despite a 382-byte sparse
analysis body. Twenty-six missing callback entries are repaired in two bounded
PC jobs, preserving all earlier function bodies, corrected data owners and the
three explicit mixed extents. The PC graph now has 7,967 functions.

Original name-lookup consumers and exact name-pointer records independently
establish those sixteen storage identities. A copied Android project receives
sixteen reviewed original-VA data-site annotations during read-only export;
no definitions, users or edges are invented. The 512 MiB heap attempt stops
without publication; an explicit 1 GiB heap retry succeeds at 1,266 MiB RSS.
All 266,598 preceding Android node records and the exact 248,675-edge export
are preserved, with only sixteen new sites. The original project is unchanged.
Full allocation extents remain unproved; resolving graph endpoints does not
resolve those allocation/type holds or establish ranking-quality improvement.

The production replay uses 1,321 accepted function seeds and 41 reviewed data
anchors, with 1,230 tentative function pairs, 1,824 data candidates and zero
stale decisions. Data hypotheses add 29, change 32 and remove none; function
evidence changes 45 and removes 95, with none added. The registry diagnostic
has 184 physical candidates, no ambiguous joins and 77 unresolved names:
76 missing PC callbacks plus `CharClipToBlobShadows` on Android only. Its outputs
remain separate from scoring and acceptance. Peak Python RSS is 674 MiB under
the 1 GiB limit. No source reconstruction gain is claimed.

The next wave adds 24 complete function identities (14,057 PC bytes) and sixteen
more unknown-extent name-storage identities, bringing mapping to 972,011 bytes
(2.426%). Sixteen AI callbacks and seven original GizObstacle callbacks have
independent pointer/namespace, PC consumer ABI and complete-extent reviews.
PunchCode adds a full 3,000-byte extent including its seven-cell switch table;
its ctype remains blank. Its exact old 2,972-byte body and seven typed pointer
owners authorize the mixed extension. Other broad movement identities and
private ABIs remain explicit holds. The PC graph now has 7,983 functions.

This review also corrects previously swapped GizmoActivate/GizmoSetVisibility
bindings, their runtime slots and the activation condition. Twelve source calls
are independently checked against fourteen original E8 sites: ten names change
and two retain their identity. All 801 previous exact function rows remain
identical. Other old comparison rows remain identical except the two corrected
wrapper bindings and ProcessGizmoFlowBox's corrected call target. Header changes
recompile all 123 objects, whose full COFF hashes change. Comparison hashes and
extents, rather than raw object equality, verify preservation here. Whole-code
fuzzy coverage changes from 6.211618663% to 6.211660511%; no newly compared
original code bytes are added.

Every repair now pins the prior PC manifest and streams all existing function
extents into a bounded disk SQLite comparison before publication, including
ordinary missing-entry packs. Changed, dropped or duplicate old bodies fail;
only exact independently reviewed mixed extensions are allowed. Reopened program,
original transform and graph hashes are checked in every mode. Four synthetic
ordinary-repair regressions raise the wrapper suite to 41 tests.

The second Android export carries all 32 reviewed name annotations. All 266,614
preceding nodes and all 248,675 edges remain identical; only sixteen unknown-size
sites are added. No allocation, type, definition, user or edge is inferred. Its
1 GiB heap export peaks at 1,272 MiB RSS; the original project is unchanged.
PC jobs peak below 760 MiB RSS with 512 MiB heaps. The successful replay peaks
at 675 MiB RSS under the 1 GiB Python address-space limit. Replay validation
rejects incomplete baseline publications and noncanonical binary hash roles
before scoring; the final run uses a complete verified baseline and canonical
original-hash-bound journal.

The production replay has 1,345 accepted function seeds, 57 reviewed data
anchors, 1,224 tentative function pairs, 1,887 data candidates and zero stale
decisions. Data hypotheses add 74, change 193 and remove 11; function hypotheses
add 13, change 486 and remove 80. Registry diagnostics report 194 physical
candidates, no ambiguous joins and 67 unresolved names: 66 missing PC callbacks
plus the Android-only CharClipToBlobShadows. These diagnostics remain separate
from scoring and acceptance. No identification-quality gain is established.

The following checkpoint accepts 31 complete function identities (19,052 PC
bytes) and three data allocations (45,416 bytes), bringing mapping to
1,036,479 / 40,067,072 bytes (2.587%). Two registry batches, the AddGizmos family
and TerrainPlayer retain independent original identity, ABI and extent proofs.
TerrainPlayer's floating helper dependencies require a separately bounded review;
its return type stays unknown. The particle array's initially held tail is
affirmatively proved by original reads and writes, then explicitly superseded in
the journal. Aggregate declarations remain research, and three incorrect
count/pointer/interior pairings become explicit exclusions.

Nine then thirteen missing callback entries are repaired on independent copies.
The latest graph has 8,005 PC functions, preserving all 7,992 prior extents in
the last job and all explicitly reviewed scalar, literal and switch-table owners.
Both 512 MiB heap jobs peak below 760 MiB RSS. The Android graph is reused without
another export. Incremental replay uses a complete verified preceding run and
peaks at 741 MiB under the 1 GiB Python limit. It has 1,376 accepted function seeds,
60 reviewed data anchors, five rejected pairs and zero stale decisions. There are
1,427 tentative function pairs and 1,923 data candidates. Full function candidate
rows grow from 16,686 to 34,284: 19,798 added, 5,415 changed and 2,200 removed;
data candidates add 70, change 255 and remove 34. These are evidence deltas,
not accepted pairs or a retrieval-quality improvement. Registry diagnostics have
216 physical candidates and 45 unresolved names: 44 missing PC callbacks and the
Android-only CharClipToBlobShadows. Diagnostics remain outside scoring.

All 123 COFF objects, all 1,941 previous function rows and all 2,528 previous data
rows remain identical. There are 1,972 function and 2,531 data inventory rows.
All 801 exact functions and 45,746 exact bytes survive; whole-code fuzzy coverage
remains 6.211660511%. No additional original code bytes are compiled or compared.
Optional reviewed PC labels in the bounded inspector are presentation only;
the AddGizmos depth-two view uses 31 MiB RSS and preserves raw graph evidence.

A subsequent original review adds 22 complete function identities (12,644 PC
bytes), ten complete four-byte data cells and four data identities with unknown
extents. Mapping reaches 1,049,163 / 40,067,072 bytes (2.619%). Six terrain/shadow
helpers retain unknown formal types despite reviewed physical contracts; the
sixteen registry callbacks retain independently proved namespace identities.
Fourteen missing callback entries are repaired in a copied project, preserving
all 8,005 prior functions and reviewed data/table owners. The resulting graph
contains 8,019 PC functions. The job uses a 512 MiB Java heap and peaks at about
759 MiB RSS. A weights pairing based on immediate flag constants becomes an
explicit exclusion. See the [mapping notes](symbols.md#callback-terrain-helper-and-state-slot-continuation)
for the accepted storage and unresolved allocations.

The broader family evaluation freezes the preceding production run's map and
journal snapshots before later additions. Its predeclared family contains all
261 physical Android registry callbacks, including the 70 original gold labels.
It hides 193 known function pairs and retains 1,183 training seeds. Propagation
proposes 72 held pairs; all agree with their existing labels. First-round
retrieval connects 176 queries, with 60 expected top-one and 91 expected top-five
pairs. Within the original fixed 70-label subset, 26 are recovered correctly;
per-query ranks for that subset are not published. The historical 25/70 result
used different graphs and training anchors, so the difference does not isolate
an improvement from graph repairs. Existing-label retrieval does not establish
precision on previously unknown symbols or calibrated probabilities.

The successful family run uses a 1.5 GiB address-space cap, taking 59.53 seconds
and peaking at 999 MiB RSS. A subsequent partition-only reproduction uses the
same pinned inputs and original seed/split machinery, stopping before scoring.
It confirms that all 261 family endpoints stay out of training and that every
fixed-70 gold pair is held. Explicit family data, incident data and alias closure
quarantine 89 of 873 data labels, leaving 784 labels and 17 reviewed anchors.
Five unrelated exclusions remain; blocked owners/interiors, endpoint decisions
and held PC names are filtered. This later reproduction is separate from the
earlier run's runtime capture. Frozen `registry-wave5-family-holdout-result-audit`
and `registry-wave5-family-partition-audit` artifacts retain the counts,
endpoints, hash pins and limitations. The partition probe peaks at 205 MiB RSS
and performs no scoring or original-code decoding.

The next production replay has 1,398 accepted function seeds, 74 reviewed data
anchors, six exclusions and zero stale decisions. It records 1,464 tentative
function pairs, 31,047 full function candidate rows and 1,972 data candidates.
Function evidence adds 835 rows, changes 15,476 and removes 4,072; data evidence
adds 97, changes 203 and removes 48. Registry diagnostics independently report
230 physical pairs and 31 unresolved names: 30 missing PC callback entries and
the Android-only callback. These remain diagnostics and hypotheses, with no
retrieval-quality or reconstruction gain inferred from their counts.

An initial incremental attempt fails safely because the preceding run's full
data cohort exceeds the 16 MiB retained-state bound, at only 221 MiB RSS. Both
candidate cohorts now stream into bounded digest summaries; separately hashed
JSONL ledgers retain every full row. Real legacy-state migration succeeds under
256 MiB, and independent synthetic review passes 86 targeted tests. Evidence-only
changes, legacy formats, corruption, cohort/row limits and unpropagated
`round=null` candidates are covered. The final production job takes 70.24 seconds,
peaks at 718 MiB RSS under the unchanged 1 GiB cap and publishes a verified
completed run. Its state shrinks from 41,182,703 to 15,741,922 bytes despite added
anchors, with 18,330,080 bytes of full data evidence retained separately.

The following map-only batch accepts seventeen functions (12,002 PC bytes) and
two data allocations (608 bytes), reaching 1,061,773 image bytes (2.650%).
Camera/path callbacks and five terrain leaves retain independent identity,
physical ABI and complete-range proofs. A larger terrain query remains held
because fifteen opaque calls leave ABI effects unknown. Accessory's 32-byte
record cohort stays research; the accepted allocation has blank aggregate type.
Five new interior/literal pairings become explicit exclusions.

Four missing callback entries are repaired on a project copy, preserving every
one of 8,019 prior functions and all explicit mixed-body/table/data owners.
The resulting graph has 8,023 PC functions; its 512 MiB heap job peaks at
753 MiB RSS. Production replay uses the complete verified digest-v2 baseline,
takes 50.72 seconds and peaks at 712 MiB RSS under the 1 GiB cap. It has 1,415
accepted function seeds, 76 reviewed data anchors, eleven exclusions and zero
stale decisions. It records 1,471 tentative function pairs, 30,952 full function
candidate rows and 2,077 data candidates. Function evidence adds 129, changes 290
and removes 224 rows; data evidence adds 124, changes 196 and removes 19.
Registry diagnostics report 234 physical candidates, no ambiguous joins and
27 unresolved names, including the Android-only callback. These counts establish
neither retrieval-quality gains nor newly compared code.


```sh
python3 tools/repair_graph.py \
  --config build/analysis/identification/current-graphs/export-config.json \
  --packet build/analysis/identification/continuation/callback-repair-packet.json \
  --reuse-pair build/analysis/identification/current-graphs/graph-pair.json \
  --output build/analysis/identification/callback-repaired-graphs
```

Typed-data repair uses the same wrapper with an
`identification-reviewed-data-v1` packet, documented in
`tools/ghidra/RepairReviewedData.java`. Authorizations include original-byte
bounds and hashes, exact superseded data starts/sizes/type paths, and the new
four-byte `float32`, eight-byte `float64`, or NUL-terminated `char-array` definitions. It clears only the explicitly
superseded definitions in a disposable project, rejects every other code/data
conflict, and performs the entire replacement in one transaction. Original
memory bytes remain unchanged. Reopening and exporting must recover the exact
new owner sizes/types, remove superseded owners, and preserve every prior
function's entry, ranges and byte count before the new graph pair is published.
The function comparison streams raw nodes into a small disk-backed SQLite table;
it does not load either graph into Python memory.

This closes a real ownership error around `GrapWindLp`: the original string
starts at `0x0075bf90`, while the prior Ghidra string at `0x0075bf8f` absorbs the
preceding double's last byte. A correct reviewed interior site has no independent
user incidence while access edges belong to that incorrect owner. Pairing the
wrong parent would authorize a different storage extent. The repair instead
creates the verified eight-byte double at `0x0075bf88` and eleven-byte character
array at `0x0075bf90`; alignment remains outside both definitions.

The first real typed-data repair saved and reopened both exact owners, removed
the erroneous owner, and preserved all 7,848 prior function bodies. All three
original access sites now resolve to their correct storage objects. Retained
project content and captured metadata remain unchanged; Android's pinned graph
was reused unchanged. With a 512 MiB Java heap, peak process RSS was 759 MiB.
Original bytes in the explicitly reviewed twenty-byte window were verified
before and after the transaction.

A tiny unique-GOT-pointer fixture now verifies exact interior-site incidence
across live, indexed and annotated graph views. Both existing representations
resolve the exact target site, with offset zero; indexed reconstruction now
calculates the offset from target addresses rather than hardcoding that result.
This introduces neither containing-owner fallback nor additional automatic data
anchors.

## Operand context and false address observations

The exporter now adds `identification-reference-operand-v1` facts to access
observations. These record the exact reference operand index, object form,
raw scalar bits and bounded instruction P-code context. A scalar value never
receives the address normalization delta. Operand-local P-code computes an
operand; consuming operations come from whole-instruction P-code. Ghidra's
ADDRESS flag alone is insufficient because attaching an analysis reference can
set it on a scalar operand.

`address_reference_policy()` excludes an exactly bound OR/AND/XOR/TEST scalar-bit
operand from derived address incidence. Every raw edge, target, owner, site,
role and width remains in the exported evidence. A memory operand at the same
numeric address remains eligible. PUSH/MOV/LEA, direct memory and PIC/GOT address
candidates retain their support. CMP, unknown, unbound, truncated and legacy
observations remain explicitly weak candidates; these local facts cannot
establish pointer identity, allocation extent or type.

In-memory graphs, SQLite normalized incidence, literal inputs and data support
use the same eligibility rule. Manifest schemas and operand indices are checked
strictly; a declared schema with missing access facts fails validation. The
derived index schema and scorer version change, while older raw exports remain
readable with unclassified legacy support. Do not rebind old index markers to a
new producer hash.

A compiled Ghidra fixture checks 24 synthetic instructions and 20 data-reference
observations: fourteen remain eligible and six scalar operands are excluded.
Independent review verifies Java/Python parity, equal-valued memory and scalar
operands, raw-bit normalization, comparison recall and malformed schema/index
rejection. Twelve dedicated tests and 85 targeted graph/tool tests pass under an
external 256 MiB process limit.

Fresh original exports preserve byte-identical nodes, every raw edge after
removing the added facts, all function ranges and the explicit mixed/table/data
owners. They exclude 250 PC and 548 Android scalar-bit observations from derived
support. Five exact original-cache controls independently confirm OR/TEST/AND
exclusion and weak CMP retention. The serialized exports take 37.03 seconds and
peak at 1,262 MiB RSS, using 512 MiB PC and 1 GiB Android Java heaps. Read-only
Ghidra rotates byte-identical project index files and updates housekeeping
metadata; all retained project file contents, sizes and modes remain unchanged.
These checks establish classification and preservation, not a retrieval-quality
or unknown-symbol precision gain.

Two further missing creature entries have independently reviewed identities and
complete extents and are repaired for graph analysis, preserving all 8,023 prior
functions and explicit owners. Their ABI holds remain outside the accepted map.
The graph now contains 8,025 PC and 13,242 Android functions. The next production
replay takes 78.68 seconds and peaks at 761 MiB RSS under the unchanged 1 GiB cap.
It records 1,416 accepted function seeds, 88 identity-reviewed data anchors,
eleven exclusions, zero stale decisions, 1,490 tentative function pairs, 30,915
full function candidates and 2,039 data candidates. The registry diagnostic has
236 physical pairs; none is automatically accepted.

An isolated control uses the same current producer, frozen accepted inputs and
explicit 89-label quarantine for both arms. Its 193-function family partition,
1,183 training function seeds, 784 training data labels, seventeen reviewed data
anchors and five rejections are identical. Both arms exclude all 261 selected
Android family endpoints from training and hold out all fixed seventy queries.
The old arm uses the same raw observations without operand facts; the new arm
adds only the facts and derived eligibility policy. Complete output manifests
and 193 per-query rank records are retained locally.

Both arms recover 74 of 193 functions with no incorrect proposals. Round one
connects 176 queries, ranks 59 correct candidates first and 89 in the top five.
The fixed seventy subset recovers 27, connects 67, ranks 26 first and 42 in the
top five. Every recorded rank and proposal is identical between arms. This
control establishes no retrieval gain or loss for this family and cannot measure
unknown-symbol precision or exhaustive address recall. Earlier results from
different repaired graphs are historical context, not this control's baseline.
The serialized old/new runs take 91.45/66.32 seconds and peak at 1,010/1,046 MiB
RSS under the same 1.5 GiB process cap; timings alone do not establish a general
performance improvement.

## Further registry and allocation review

The next bounded registry review establishes six complete PC callback extents.
Only `SetLapTime` closes its qualified normal ABI and enters the accepted map;
five specific callee-effect holds remain. All six missing endpoints are repaired
for analysis, preserving every previous function range and the explicit
mixed/table/data packets. A separate namespace review recovers the already mapped
API `DontPush` and `SetLocator` graph endpoints; their game-registry counterparts
are distinct. These two repairs add no mapping bytes. `SetLocator`'s new transitive
ABI review remains held and does not claim a new contract.

The graph now contains 8,033 PC and 13,242 Android functions. The two serial
repair/export jobs take 21.66 and 21.64 seconds, each using a 512 MiB Java heap
and peaking at about 748 MiB RSS. Every prior function and the explicit reviewed
mixed/table/data owners are preserved. Preparing the new PC SQLite cache takes
10.49 seconds and peaks at 61 MiB under an external 256 MiB limit.

The allocation review accepts the 560-byte private camera, the 48,128-byte
particle-definition arena and the separate 128-byte scene-pointer table. Exact
PC initialization, independent capacities, literal bases and allocation-end
observations establish their extents; aggregate types remain unresolved. Three
wrong particle bindings remain rejected. One superseding rejection corrects
lifecycle wording without changing the exclusion. Trusted PE section metadata
establishes the large original zero initializer without reading a 48-KiB array.
The static tables stay distinct from the dynamic particle buffer. `InitParts`'s
631 owned bytes lie in three ranges within a 643-byte span; no missing tail is
invented from that count.

A cache-only view of `SetLapTime` compares two bounded call/reference
neighborhoods with explicit reviewed PC labels. It takes 1.13 seconds and peaks
at 31 MiB; its depth, fanout, node, edge and output limits stay enforced. Labels
are presentation annotations and do not turn raw graph observations into
identity or ABI proofs. The ignored `setlaptime-call-reference-tree.{json,md}`
retain raw versus normalized incidence and explicit truncation.

The terrain Mask review closes its qualified normal physical ABI after composing
37 independently checked owner contracts and seven original runtime interfaces.
All 82 call bindings verify, including private `ftol` inputs and 39 `NuFsqrt`
calls with no caller-owned floating values. The complete 1,984-byte function
enters the map with blank ctype and unreviewed formal types. Valid runtime
memory/state/domain, direction flag and normal return remain conditions; hooks,
exceptions and exact internal CRT floating peak are outside the claim.

The production replay takes 46.39 seconds and peaks at 750 MiB under the unchanged
1 GiB cap. It freezes the exact accepted map and journal, records 1,418 function
seeds, 91 reviewed data anchors, fourteen exclusions and zero stale decisions.
Its full ledgers contain 1,503 tentative pairs, 16,649 function candidates and
2,039 data candidates. Registry diagnostics resolve 244 physical pairs, with
sixteen missing PC endpoints and one Android-only name. These changed candidate
counts are research results on changed graphs and anchors, not retrieval-quality
or reconstruction gains. The preserved complete baseline and hashed outputs
retain the semantic delta. An initial launch was refused by the shared lock
while the baseline copy ran; the identical completed baseline was revalidated
before the successful serial replay.

The next fixed four-callback review repairs 680 original PC bytes in a fresh
project copy, preserving all 8,033 prior function bodies and the explicit mixed
table/data-owner guards. The graph now contains 8,037 PC functions. Java uses
a 512 MiB heap, takes 21.15 seconds and peaks at 760 MiB with zero tool swaps.
Only `PartyCanBeUnderCover` passes independent ABI review and adds 94 accepted
code bytes; the other three complete graph spans remain held on callee effects.
No aggregate declarations or compiled definitions are added.

The corresponding replay takes 53.73 seconds and peaks at 742 MiB under the
1 GiB cap. It has 1,419 seeds, 91 data anchors, fourteen exclusions, zero stale
decisions, 1,503 tentative pairs, 16,661 full function rows and 2,039 data rows.
Registry diagnostics resolve 248 physical pairs, leaving twelve missing PC
endpoints and one Android-only name. Independent audit verifies all 38 outputs
across the completed run and its complete previous baseline, live input pins
and streamed ledger counts. Counts do not establish identification accuracy.

### Bounded static-CRT archive adapter

`tools/crt_archive.py` supplies exact member selection from the already installed
MSVC `libcmt.lib`, complementing Android graph identities. It reads archive
headers and linker/name metadata once, then reads only explicitly selected
member payloads. Defaults cap cumulative metadata at 2 MiB and both per-member
and cumulative payload at 16 KiB. Limits are checked before reads. Duplicate
symbol alternatives remain ambiguous until an exact selector resolves them.

Use `CRTArchive.find()` for exact alternatives, `select()` for one exact symbol,
member name or header offset, and its `coff()` context for the existing
`tools.binary.COFF` parser. The context stages a fixed safe temporary filename;
existing extraction and relocation APIs retain all their failure gates. There
is no new relocation implementation, inferred function extent or automatic map
acceptance. Archive index names are hypotheses; complete original identity,
ABI, ownership and full relocated byte equality require separate review.

Independent review verifies the pinned library and exact calibration selectors
without supplying object code. A synthetic alternate-file open exposed a
provenance race before the payload pilot. The adapter now checks the opened
descriptor's device, inode, size and timestamps before and after parsing or
payload reads, alongside path checks. The regression rejects the alternate
descriptor before charging or reading selected payload. Seventeen synthetic
tests cover this case, exact selection, ambiguity, malformed indices, limits,
delegation and changed relocation destinations.

The fixed original pilot reads three selected members once (9,294 payload
bytes), supplies 210 original PC code bytes and extracts 130 tool-code bytes
including the failed relocation attempt. The complete 52-byte `__allmul`
calibration is exactly equal at its original address. This is an existing
identity calibration, not a new mapping or reconstructed source definition.
The 13-byte `_srand` extraction succeeds, but relocation refuses the missing
`__getptd` binding. Both FTOL entry extractions refuse native shared-section
ownership without auxiliary function sizes; no flags or extents are invented.
Actual unresolved-relocation and unequal-extent controls are recorded. Actual
changed-target and alias controls were unavailable in this pilot; synthetic
adapter controls remain separately identified. Frozen
`pc-crt-coff-pilot-next1` records the exact inputs, hashes and failures.

### Directed views and identity anchors with ABI holds

Graph inspection accepts `--direction out`, `in`, or `both`; the default remains
`both`. Outgoing expansion follows source/owner references to targets and their
normalized/GOT observations. Incoming expansion follows target/user references
to sources and owners. Direction controls traversal expansion: raw owner-access
observations remain retained, including peer-source observations that are not
queued. Existing 32-node, 256-edge, per-relation fanout, output and deadline
limits remain enforced. Raw and normalized targets retain distinct meanings.
Six new synthetic cases independently verify direction, shared-callee clutter,
cycles, limits, malformed options and default behavior; 31 inspector tests pass.

The retained `raceopponent-outgoing-call-reference-tree` is an advisory depth-two
view with explicit truncations and optional presentation-only CSV labels. It
contains 31 PC nodes/34 edges and 32 Android nodes/78 edges, takes 0.93 seconds
and peaks at 31 MiB. Presence, labels and graph extents do not establish identity
or calling-interface acceptance. The CLI refuses implicit index preparation.

The next four-callback graph repair preserves all 8,037 prior functions and the
explicit mixed/data guards, bringing the PC graph to 8,041 functions. Its
512 MiB Java heap peaks at 744 MiB in 20.51 seconds with zero tool swaps.
Those four original identities and complete spans total 830 research bytes;
the journal retains ABI/type flags false and the CSV gains no rows or bytes.
An independent particle-count/normal disagreement adds one rejected data pair.

The graph/exclusion replay takes 53.56 seconds at 742 MiB under the 1 GiB cap.
A second replay isolates the four identity-anchor additions on identical graphs,
using the complete first run as its previous baseline. It takes 42.44 seconds
at 689 MiB, with 1,423 seeds, 91 data anchors, fifteen exclusions, zero stale
decisions, 1,313 tentative pairs, 16,865 full function rows and 2,039 data rows.
The inferred-pair decrease is not an accuracy claim. Independent audit verifies
all 57 outputs across three complete runs, both state links, streamed ledger
counts and final live input pins. An initial anchor append stopped before any
mutation because its duplicate check did not handle legacy graph node IDs;
the first replay remained valid for its graph/exclusion scope before the
corrected append and isolated anchor replay.

## Installed tools and input identity

The installed Ghidra is `/opt/ghidra`, version `12.1.2_DEV`, with Java
`21.0.12.1`. BSim, its local H2 backend and the `medium_nosize` template are
bundled. Python commands use the existing interpreter and standard library.
No server or replacement disassembler is needed.

| Original | SHA-256 | Analyzed language/compiler | Normalization |
| --- | --- | --- | --- |
| PC `LEGOStarWarsSaga.exe` | `239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113` | `x86:LE:32:default` / `windows` | analyzed VA unchanged; image base `0x00400000` |
| Android `libTTapp.so` | `30d3fe55503416139a1d0f9b25fc73924aab4b12dd731aafc08b4a1c9bd950d5` | `x86:LE:32:default` / `gcc` | subtract `0x10000` from the inspected project's analyzed VA |

The Android normalization is specific to the validated project. Verify the
original hash, image base and executable file-byte anchors before reusing it.
Another import may use a different base. The PC original is available through
ignored `res/LEGOStarWarsSaga.exe`; the inspected Android original is
`/home/fabian/git/saga/res/libTTapp.so`.

All projects, databases, exports and tool binaries stay under ignored `build/`.
Every headless command sets project-local `XDG_CONFIG_HOME` and
`XDG_CACHE_HOME`; it does not install extensions or write settings to the user's
normal Ghidra configuration. Use one operation per Ghidra project at a time.
Never bypass a project lock. Export retained databases with `-readOnly
-noanalysis`; analysis repair belongs in a separate working copy.
The maintained exporters and repair launcher share the identification job lock.
Set a job's `java_heap_mb` explicitly to change the 512 MiB default (128–4096 MiB).
Java heap is not total RSS, so measure the process. Launch Java separately from
Python jobs with finite address-space limits: JVM virtual reservations exceed
its heap, and the launchers reject inherited limits rather than remove them.

Ghidra can exit successfully despite script failure. Check the expected output,
its manifest, and the script log. A pre-existing output is not proof that the
latest command succeeded. Use fresh output paths and retain previous evidence.

## Raw reference graphs

`tools/ghidra/ExportReferenceGraph.java` takes:

```text
output-directory original-file signed-address-delta [reviewed-data-evidence.jsonl]
```

It verifies the analyzed original SHA-256, normalized image base and executable
file-byte controls. The signed delta is **added** to analyzed addresses: PC `0`,
this Android project `-0x10000`. Use a separate script directory because Ghidra
compiles a directory's scripts as an OSGi bundle.

```sh
mkdir -p build/analysis/identification/graph-runtime/scripts \
  build/analysis/identification/graph-runtime/config \
  build/analysis/identification/graph-runtime/cache
cp tools/ghidra/ExportReferenceGraph.java \
  build/analysis/identification/graph-runtime/scripts/
XDG_CONFIG_HOME="$PWD/build/analysis/identification/graph-runtime/config" \
XDG_CACHE_HOME="$PWD/build/analysis/identification/graph-runtime/cache" \
/opt/ghidra/support/analyzeHeadless "$PWD/build/analysis/ghidra" saga-pc \
  -process LEGOStarWarsSaga.exe -readOnly -noanalysis \
  -scriptPath "$PWD/build/analysis/identification/graph-runtime/scripts" \
  -postScript ExportReferenceGraph.java \
    "$PWD/build/analysis/identification/graphs-run-002/pc" \
    "$PWD/res/LEGOStarWarsSaga.exe" 0
```

The inspected Android project was
`/tmp/saga-plus-two-ghidra.Ip19bO/matching-focused.gpr`, program `libTTapp.so`.
For its export replace the project-directory/name/program above with those
values, the original with `/home/fabian/git/saga/res/libTTapp.so`, and the delta
with `-0x10000`. A temporary project is a current input location, not a durable
installation dependency. Preserve a verified paired `.gpr`/`.rep` copy under
`build/analysis/` while the source project is closed before relying on long-term
reuse.

The maintained `tools/reference_graph.py` launcher exports both sides in order,
isolates the script bundle, checks fresh manifests and publishes
`graph-pair.json`. Its `--config` JSON has schema
`export-reference-graph-config-v1` with exactly `jobs.pc` and `jobs.android`.
Each job contains `project` (existing `.gpr`), `program`, `original`, pinned
`sha256`, signed additive `delta`, and optional `reviewed_evidence`. Paths are
absolute or relative to the configuration file. For example:

```json
{
  "schema": "export-reference-graph-config-v1",
  "jobs": {
    "pc": {
      "project": "/home/fabian/git/saga-pc/build/analysis/ghidra/saga-pc.gpr",
      "program": "LEGOStarWarsSaga.exe",
      "original": "/home/fabian/git/saga-pc/res/LEGOStarWarsSaga.exe",
      "sha256": "239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113",
      "delta": 0
    },
    "android": {
      "project": "/tmp/saga-plus-two-ghidra.Ip19bO/matching-focused.gpr",
      "program": "libTTapp.so",
      "original": "/home/fabian/git/saga/res/libTTapp.so",
      "sha256": "30d3fe55503416139a1d0f9b25fc73924aab4b12dd731aafc08b4a1c9bd950d5",
      "delta": -65536
    }
  }
}
```

Save this under ignored analysis, then use:

```sh
python3 tools/reference_graph.py \
  --config build/analysis/identification/export-config.json \
  --output build/analysis/identification/graphs-run-002
```

The retained checked pair is
`build/analysis/identification/current-graphs/pair.json`; a new launcher run
writes its own `graph-pair.json`. Do not overwrite retained graphs used by an
active scoring run.

The output is `manifest.json`, `nodes.jsonl`, `edges.jsonl`. Its schema is
`identification-reference-graph-v1`. The manifest includes binary hashes,
address transform, language/compiler, exporter hash, verification results,
memory-block source mappings, record counts and JSONL hashes. Entity IDs combine
binary SHA-256, address space, normalized VA and kind. Nodes preserve analyzed
function ranges and raw data sites. Edges preserve calls, accesses and pointer
relationships, with evidence attribution and uncertainty.

An analyzed data size is a hypothesis. A referenced interior address is not
automatically an owner base. Unknown widths/extents stay unknown; disjoint
function ranges are not filled into one contiguous extent. The optional fourth
argument supplies separately reviewed data/pointer evidence, rather than
authorizing inferred data boundaries. Each JSONL annotation includes the
original `binary_sha256` and normalized `address`; its remaining fields carry
the reviewed evidence and are attached to that raw site. The exporter verifies
the hash/address and fingerprints this evidence file in the manifest.

Create a graph-pair JSON next to the two graph directories:

```json
{
  "schema": "identification-graph-pair-v1",
  "pc": {
    "manifest": "pc/manifest.json",
    "binary_sha256": "239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113"
  },
  "android": {
    "manifest": "android/manifest.json",
    "binary_sha256": "30d3fe55503416139a1d0f9b25fc73924aab4b12dd731aafc08b4a1c9bd950d5"
  }
}
```

Manifest paths resolve relative to this pair file. The loader verifies graph
hashes and identities before scoring.

## Baseline and joint scoring

The existing binary-only baseline remains available:

```sh
python3 tools/check.py --identify --holdout \
  --symbols config/symbols.csv \
  --analysis-dir build/analysis/identification/baseline-run-002
```

This mode uses retained legacy function/reference exports and feature caches.
`--refresh-features` explicitly regenerates those features. A diagnostics-clean
Android AST overlay can be supplied with `--source-graph PATH` in baseline mode.
Source overlays and legacy feature refresh are currently rejected in joint mode.

Joint mode consumes the normalized raw graph pair:

```sh
python3 tools/check.py --identify \
  --joint-graph build/analysis/identification/graphs-run-002/graph-pair.json \
  --symbols config/symbols.csv \
  --analysis-dir build/analysis/identification/joint-run-002
```

Use `--max-rounds N` (1–64, default 4) to extend function propagation in either
mode. Summaries record the configured limit, completed rounds and whether the
run stopped at a fixed point (`no-new-pairs`) or its limit (`round-limit`). More
rounds do not relax the score, margin, independent-root or mutual-best rules.
Joint runs also publish `function-seeds.snapshot.jsonl`, the exact accepted
function crosswalk used for retrieval. Its origin distinguishes reviewed-journal
seeds from accepted-map unique-name seeds; it does not approve new mappings.

A controlled retained-graph experiment with 1,444 seeds produced 1,304 tentative
pairs in four rounds and 1,326 in seven rounds, then stopped adding pairs.
The additional rounds added 22 hypotheses; they do not establish 22 identities.
An independent direct-call pilot found 13 previously unmapped review leads from
shared accepted caller signatures. Both results require original-PC identity,
physical ABI and complete-boundary review before insertion into the symbol map.
A disposable Ghidra repair adds the independently reviewed code entries
`0x0049b960` / 98 bytes and `0x0049ca10` / 108 bytes, preserving all 8,043
previous PC function extents and reusing the Android export. Their former
nonfunction call targets could not participate in function propagation. Boundary
repair alone does not approve a historical name, type or source implementation.
The confirmed-user data scorer still uses accepted function pairs only. Add
`--tentative-data-feedback` to an ordinary joint run to publish the separate
`tentative-function-data-candidates.jsonl` experiment. This channel combines
current inferred-mutual-top function observations with accepted seeds, intersects
ancestry with accepted function roots and reviewed data components, and counts
at least two distinct users and two distinct ancestry roots. Repeated descendants
cannot manufacture independent roots; their per-root contributions use maxima.
Its scores are research rankings, not comparable with the normal data ranking.

These rows never enter normal data outputs, state, function retrieval, table
scoring or the accepted map. They retain function observation status, round,
score and ancestry. Rejected/reserved pairs and work/provenance limits apply;
owner, offset, type and allocation ambiguities still need original review.
Holdout and similarity-evaluation modes reject this option until a separate
training-only evaluation is available. The option defaults off, and reruns clear
any stale tentative ledger before publication.

The production experiment retains the normal scorer's 10,000-row limit and
uses a separate 50,000-row limit for the ancestry experiment, with the same
500,000-work limit and a hard 1 GiB process limit. The first experimental run
hit the smaller retained-row guard and published no partial rankings; the
explicit larger experimental budget completed with zero swaps. These limits
are work controls, not score or identity approvals.

With the five newly reviewed identities, 1,449 accepted seeds yield 1,307
tentative function pairs in four rounds and 1,329 at the seven-round fixed point.
The separate ancestry channel reports 9,892 and 9,949 data-candidate rows,
respectively: a net increase of 57 rows, not 57 verified or necessarily new
unique identities. The normal data scorer retains 2,072 candidate pairs in both
runs. Disabled/enabled sixteen-round runs preserve the normal function, data
and state outputs; delta bookkeeping differs when only the control starts from
a prior state. These experiments do not measure unknown-data accuracy.

Optional joint inputs are `--reviewed-pairs JOURNAL.jsonl`, repeated
`--similarity-candidates CANDIDATES.json`, and `--holdout` with
`--holdout-mode pair`, `family` or `data`. Use `--holdout-selection FILE` for a
frozen explicit evaluation selection; keep its labels out of training inputs.
BSim and BinDiff scores retain their producer meaning rather than becoming
automatic identity approvals. Data candidates use distinct confirmed paired
function users; repeated accesses in one function do not count as independent
users. Tied co-used globals and competing owner interpretations require review.

Add `--registry-descriptors FILE` to a joint run for a separate original-registry
diagnostic. The file has schema `identification-registry-descriptors-v1`, `pc`
and `android` descriptor arrays, and `review_proofs` entries containing `side`,
`path` and `sha256`. Each descriptor pins the original binary SHA-256, namespace
and explicit 12-byte layout (`stride: 12`, name/callback/flags offsets 0/4/8).
Selected rows require individual start/size/hash proofs. Complete tables require
the base, count excluding the terminator, a bound spanning `(count + 1) * 12`
bytes with its SHA-256, and a twelve-zero-byte terminator. No expected names or
address transforms are accepted. Names are read through their first original
NUL, and callback endpoints must retain their exact original VA.

Only `namespace_status: "reviewed-original"` enables joins, with at least one
pinned review proof per reviewed side. Unknown namespaces fail closed. The run
pins descriptor, proof, original and producer inputs; it reuses the literal
readers and pins the optional Android reader when present. An unavailable
Android reader produces an explicit degraded diagnostic retaining PC evidence;
malformed originals still fail. Limits are 32 descriptors, 4,096 records,
64 KiB per table and 2 MiB for the descriptor file. `registry-evidence.json`
and `registry-comparison.json` preserve unresolved callbacks, conflicting names
and alias groups. Their candidates are review inputs with unproved body/ABI
extents; `summary.json` records `scoring_used: false` and promotion disabled.

Each run freezes the symbol map, optional decisions and source overlay, and
fingerprints raw inputs. It publishes a staged directory atomically only after
validation. `run-manifest.json` has schema `identification-run-v1` and status
`complete`; it fingerprints every published output. A staging directory or a
modified output is not a reusable completed run. Concurrent runs against the
same output directory are rejected. Prefer named run directories to preserve
comparisons.

Joint research outputs include `hypotheses.csv` (functions),
`function-candidates.jsonl` and `data-candidates.jsonl` (full evidence),
`data-candidates.csv` (selected review fields),
`evidence.jsonl`, `review.md`, `inventory.json`,
`state.json`, and `delta.json`; `summary.json` and any `holdout.json` report
this run's actual counts. These are separate from
`matching.json` and reconstruction coverage.

For a small paired call/reference view, use the cache-only inspector after a
guarded joint run has prepared and validated both graph indices:

```sh
python3 tools/identification_inspect.py \
  --pair build/analysis/identification/registry-wave2-reviewed-conflict-repaired-graphs/graph-pair.json \
  --pc 0x455320 --android 0x1867b0 --depth 2 \
  --output build/analysis/identification/continuation/walk-backwards-inspection
```

It streams manifest/raw-export/index hashes, checks the exact cache identity and
current producer, and queries SQLite directly without constructing a full graph
or preparing a missing index. Outputs are a fresh JSON/Markdown pair under ignored
`build/analysis`. Depth defaults to one and is capped at two, with 32 nodes per
side, 256 total edges, sixteen edges per relation, a five-second traversal
deadline, 2 MiB SQL caches and a 256 MiB CLI address-space limit. Verification
time is separate from the traversal deadline. Missing function entries stay
missing; cycles/shared nodes are references and all truncation is explicit.
Raw targets, owners, roles and widths remain separate from advisory GOT-derived
normalized targets. Analysis body ranges never establish reviewed extents.
The first real `WalkBackwards` depth-two probe used 28 MiB RSS and 0.73 seconds,
with explicit node/fanout truncation. This inspection has no scoring, acceptance,
state, journal or source overlay effects.

Add `--symbols config/symbols.csv` to show reviewed PC map names in a separate
presentation layer. This explicit input is read once into a private immutable
snapshot, parsed with the normal map parser, and pinned by hash and file identity
through traversal. Its limit is 2 MiB. Only exact retained PC addresses receive
labels, capped at four names of 96 characters; parents and interior sites are not
rebound. Original nodes, edges, owners and unknown extents remain unchanged, and
no Android name or cross-binary identity is inferred. Mermaid labels use a fixed
safe character alphabet. The real PunchCode probe used 28 MiB RSS; the inspector
suite has 25 synthetic tests, including changed-input and hostile-label cases.

Persistent function and data state contains compact summaries and an exact
digest of each full evidence row, using `candidate-digest-v2` storage. Full
evidence remains in separate JSONL files fingerprinted by the completed-run
manifest. Replay streams legacy state row by row, including earlier
`function-digest-v1` state with full data rows. An old large state never allocates
another complete evidence tree. Individual decoded rows are limited to 1 MiB;
each retained compact cohort and other member is limited to 16 MiB. Oversized or
malformed inputs fail explicitly. Evidence-only changes remain visible through
their canonical digests. Tentative state is used only
to report changes, never as confirmation anchors. The current implementation
recomputes candidates from confirmed roots rather than updating only neighbors.

## Review journal

`--reviewed-pairs` takes append-only JSONL with schema
`identification-decision-v1`. Each decision needs a unique `id`, `pc`, `android`,
`kind` (`function` or `data`), `status`, nonempty `reason`, and
`binary_sha256` containing both `pc` and `android` hashes. The pair fields refer
to normalized graph identities/addresses, not Android analyzed rebased VAs.

Statuses are `candidate`, `identity-reviewed`, `rejected`, `ambiguous`,
`inline-only`, and `alias`. The independent optional booleans `abi_reviewed`,
`type_reviewed`, `extent_reviewed` do not follow automatically from identity.
An update to the same pair must set `supersedes` to that pair's latest decision
ID; a first decision has no predecessor. Optional `graph_fingerprints` marks
reviews stale when their underlying analysis changes. Rejections remain
exclusions across analysis-only changes. Different original binaries reject
the journal entirely.

Replay regenerates deterministic candidates and reports changed evidence.
An identity review alone does not establish examined byte coverage. Only
separately reviewed complete PC ranges qualify; unknown data extents remain
blank in an eventual accepted-map proposal. The scorer never writes the map.

## BSim local H2

`tools/bsim.py export CONFIG` launches the maintained Ghidra adapter with
read-only processing, no analysis, isolated scripts/config/cache and explicit
project identity. Index configs contain `mode: "index"`, absolute `output`,
`database_url`, `database_file`, and `program`. The `program` object contains
`original_path`, `sha256`, analyzed `image_base`, `language_id`, `compiler_id`
and signed additive `address_delta`.

For example, a database URL `file:/ABS/android-v2` resolves the H2 file
`/ABS/android-v2.mv.db`. Set `database_file` to that exact file. Index creation
refuses an existing database, and the driver refuses an existing export output.
Use fresh database/output paths when repeating an index.

The actual checked pilot config examples are under ignored
`build/analysis/bsim-pilot/index-config-v2.json` and
`query-config-v3.json`. Edit copies to fresh outputs before repeating:

```sh
python3 tools/bsim.py export build/analysis/bsim-pilot/index-config-v2.json \
  --project-directory /tmp/saga-plus-two-ghidra.Ip19bO \
  --project-name matching-focused --program-name libTTapp.so \
  --runtime build/analysis/bsim-pilot/runtime-v2
python3 tools/bsim.py export build/analysis/bsim-pilot/query-config-v3.json \
  --project-directory build/analysis/ghidra \
  --project-name saga-pc --program-name LEGOStarWarsSaga.exe \
  --runtime build/analysis/bsim-pilot/query-runtime-v3
python3 tools/bsim.py evaluate build/analysis/bsim-pilot/candidates-v3.json \
  build/analysis/bsim-pilot/selection.json \
  --output build/analysis/bsim-pilot/evaluation-v3.json
```

Query configs add `mode: "query"`, `index_manifest`, normalized PC `addresses`,
`max_candidates`, `similarity_threshold`, `significance_threshold`, and optionally
`selection_sha256`. Their database URL points at the indexed Android database.
The query program is PC with delta `0`; the index program is Android with delta
`-65536` for the inspected project.

Outputs use `saga-bsim-v1`; metadata pins template `medium_nosize`, settings,
Ghidra/generator version, binary identities and normalization. Queries retain
similarity, significance, rank, status and failures. Evaluation uses
`saga-bsim-evaluation-v1`, includes failed/missing queries in denominators, and
separates tiny wrappers from larger functions. The frozen pilot selection is
100 known functions plus 50 unknown functions. Retrieval against existing labels
is a biased benchmark, not reviewed unknown discovery.

The initial pilot database/output was superseded while correcting signature
vector transfer. Do not use the original `android` database or initial
`candidates.json`; use the corrected `index-v2.json` / `android-v2.mv.db` and
`candidates-v3.json` artifacts. The v3 query accepts numerical cosine roundoff
within `1e-12` and retains complete provenance. The fixed pilot retrieved
53/100 known labels at top one, 68/100 at top five, and 77/100 at top ten,
with no hard query errors. Its eight tiny functions retrieved 0/8, 2/8 and 3/8
respectively. These are retrieval results against existing labels. Read final
signature/failure counts from the corrected index manifest: this pilot indexed
13,052 signatures from 13,432 attempted bodies, with 380 skipped thunks and
no hard indexing exceptions. Queries returned 146/150 shortlists and four
no-candidate results (two known, two unknown), with no hard query errors.
The 48 unknown shortlists remain unreviewed. Native warnings for query
`0x00727654` remain in the log; a signature does not certify function boundaries.
Similarity is raw cosine and significance reflects feature weights; neither is
a probability. Candidates sort by similarity, significance, normalized VA and
name, and the pilot used zero query thresholds.
No Android labels or types are imported into the PC program.

## Optional BinDiff

The bounded pilot succeeded using the official
[BinDiff v8 Linux package](https://github.com/google/bindiff/releases/tag/v8)
unpacked with `dpkg-deb -x` into `build/tools/bindiff/package`.
Its package SHA-256 is
`821990e3974a9df6733794370e485486ac276ba5d777a06b4ebe575f092f4dda`;
the executable reports `BinDiff 8 (@568181968, Sep 25 2023)`.

The published BinExport extension targets Ghidra 11.0.3. Instead, official
[BinExport source](https://github.com/google/binexport) commit
`b56dea7b0a8baff0971466d422030c496464b6b6` built **unmodified** against this
installed Ghidra with Gradle `9.7.1`, Java 21 and detected protobuf `4.31.0`:

```sh
GRADLE_USER_HOME="$PWD/build/tools/bindiff/gradle-home" \
GHIDRA_INSTALL_DIR=/opt/ghidra \
gradle --no-daemon -p build/tools/bindiff/source/java
```

The built ZIP is under `build/tools/bindiff/source/java/dist/`. Its hash and
source artifact provenance are in
`build/analysis/identification/bindiff/pilot-manifest.json`. The extension was
unpacked into this isolated settings path:

```text
build/analysis/identification/bindiff/config/ghidra/ghidra_12.1.2_DEV/Extensions/BinExport
```

Use `tools/ghidra/ExportBinDiffCandidates.java` with one output `.BinExport`
argument. It discovers the installed exporter through Ghidra's class registry,
preserves analyzed addresses/names and refuses replacement of an existing export.
Copy it to an isolated script directory before launch:

```sh
mkdir -p build/analysis/identification/bindiff/scripts
cp tools/ghidra/ExportBinDiffCandidates.java \
  build/analysis/identification/bindiff/scripts/
XDG_CONFIG_HOME="$PWD/build/analysis/identification/bindiff/config" \
XDG_CACHE_HOME="$PWD/build/analysis/identification/bindiff/cache" \
/opt/ghidra/support/analyzeHeadless "$PWD/build/analysis/ghidra" saga-pc \
  -process LEGOStarWarsSaga.exe -readOnly -noanalysis \
  -scriptPath "$PWD/build/analysis/identification/bindiff/scripts" \
  -postScript ExportBinDiffCandidates.java \
    "$PWD/build/analysis/identification/bindiff/pc-new.BinExport"
```

Export Android with its validated project similarly. Then run:

```sh
build/tools/bindiff/package/opt/bindiff/bin/bindiff \
  --primary="$PWD/build/analysis/identification/bindiff/pc.BinExport" \
  --secondary="$PWD/build/analysis/identification/bindiff/android.BinExport" \
  --output_dir="$PWD/build/analysis/identification/bindiff"
python3 tools/bindiff.py \
  build/analysis/identification/bindiff/pc_vs_android.BinDiff \
  build/analysis/identification/bindiff/candidates-new.json \
  --secondary-offset 0x10000
```

Unlike the graph/BSim additive delta, BinDiff adapter `--primary-offset` and
`--secondary-offset` are **subtracted**. Defaults are zero. The adapter opens
the SQLite result read-only; records database hash, metadata, embedded input
hashes, raw and normalized addresses, algorithm, similarity and confidence;
and emits schema version `1` with `evidence_channel: "bindiff"` and candidate
status. It exposes `import_candidates()` for scorer integration. Compare
embedded original hashes with graph manifests before combining channels.

The checked cross-binary pilot produced 7,585 pairs, overall similarity 12.10%
and confidence 32.55%. It also emitted 7,345 engine missing-basic-block warnings
and 196 PC exporter entry-point warnings. The fixture self-diff matched 15/15
functions but scored 99.27% and emitted a missing-block warning. Retain these
limitations when reviewing results. BinDiff's chosen assignment is not a full
alternative shortlist. Annotated PC names may contain accepted-map labels, so
this saved pilot is not a leakage-free holdout evaluation. No pair was accepted
automatically and no identification-quality gain is established by setup success.

A later retrospective check queries only 30 original-reviewed entries from two
AI callback batches and the AddGizmos family. Eight have saved BinDiff selections;
all eight select different Android counterparts. The other 22 have no selected
pair, which does not establish their absence from BinExport. This fixed sample,
saved warnings and annotated inputs do not establish general accuracy. They do
reinforce keeping BinDiff assignments advisory and using original registries,
callers and complete-body review as authority. The ignored
`continuation/reviewed-new-callback-bindiff-diagnostic` pins the originals and
records every selected counterpart and normalization.

## Extending a verified function across missing code

A reviewed `replacement_body` may describe an exact strict subset of the complete
new function range, including a missing code gap with no padding or table
exclusions. Every old range retains its original-byte hash and exact byte count.
The same entry, complete new extent, old-body equality, instruction boundaries,
conflicting-owner checks and preservation audit remain mandatory. Existing
mixed code/table exclusions retain their separate ownership rules.

The original CRT candidate at `0x6f0395` supplies the real validation: its
retained 116-byte split body omitted reachable POP ECX and XOR ESI,ESI bytes
at `0x6f03fd..0x6f0400`. Its independently reviewed complete PC extent is
119 bytes through `0x6f040c`. A disposable-project negative with an incorrect
old-range hash was refused before publication. The correctly pinned positive
restored those three bytes and preserved all 8,043 prior function entries,
allowing only this exact declared extension, plus the four explicit owner
guards and both previously repaired runtime helpers. The source project
remained unchanged. The pinned native CRT member instead has a 131-byte body;
identity, ABI and type remain held, with no accepted mapping credit.

Before that extension, two already accepted functions were absent from the
analysis graph: GetCharacterGoalSpeed at `0x43c130` (112 bytes) and
GetNamedAPIObject at `0x43c220` (375). Their independently verified extents
were added in a disposable project while preserving all prior 8,041 function
entries and the four owner guards. These 487 bytes add no mapping credit.
Lookup formatter and arbitrary callback-replacement effects remain unresolved.

Both successful Ghidra jobs used a 512 MiB Java heap, unrestricted Java address
space and the shared heavy-job lock. They ran sequentially at roughly 753 MiB
and 760 MiB peak RSS, reporting zero swaps. Frozen
`registry-wave13-helper-owner-preservation`,
`registry-wave13-crt-gap-root-validation` and the independent tooling audit
retain the actual proofs and bounds.

## Verification and boundaries

Fixture exports and adapter tests verify serialization, normalization and
provenance handling. They do not establish exhaustive disassembly, function
boundaries, data ownership or cross-platform semantic identity. Review reports
must retain failures and unresolved candidates in their denominators.

The main integration lane runs the repository checks from
[CONTRIBUTING.md](../CONTRIBUTING.md), including original PC matching after
tooling/map changes. Keep generated graphs, original bytes, H2 databases and
downloaded binaries ignored. Only tooling, synthetic tests, reviewed proposals
and documentation belong in tracked changes.

The preceding checkpoint `make -j2 check` passed 434 tests (three optional Ghidra
fixture skips), source formatting and Clang-tidy. The object check passed 1,638
function and 2,606 data definitions across 123 objects. Compilation and matching
use two Make workers. The preceding checkpoint preserved all prior comparison
rows and added 128 exact page-ID data bytes; its pushed commit `4abd113` passed
CI Build 36843877620.

That checkpoint maps CompleteLevel's complete 280-byte body and the 32-byte
page occupancy gates, adding 312 mapped bytes. CompleteLevel's reviewed normal
physical ABI retains explicit runtime, table and queue-count qualifications;
formal types stay unknown and ctype is blank. No compiled callback definition
or code comparison gain is claimed. The array retains its original zero
initializer. Mapping coverage rounds to 2.786%; fuzzy code remains 6.212%.

The frozen `joint-registry-wave13-run` uses the repaired 8,043-function PC and
unchanged 13,242-function Android graphs. It contains 1,427 function seeds,
93 reviewed data anchors, 15 exclusions and zero stale decisions. CompleteLevel
adds one identity; two already accepted identities become usable function seeds
through the missing-helper graph repairs. The 1,311 inferred pairs, 16,858
function candidate rows and 2,041 data candidate pairs remain advisory. These
counts do not establish accuracy improvement. All 20 files from the complete
previous run were copied and hash-validated before replay.

The 1024 MiB address-space limit and shared heavy-job lock remained enforced;
production took 52.29 seconds at 705,204 KiB peak RSS (about 689 MiB), with
zero swaps reported by that job. Root integration preserves the corrected
CompleteLevel record stride of `0x130` (304 bytes), explicit queue range 0..8
and the occupancy gates' zero/one contract. Source implementations, broader
CRT identities and arbitrary-input safety remain separate work.

The first pushed tool snapshot exposed a test relying on a local ignored file;
the fixture now isolates both repository roots and its job lock, so the required
legacy-default inputs are entirely synthetic.


## CompleteLevel source release and incremental replay

Four independently reviewed helper identities supply 286 additional mapped code
bytes, and the complete NewCutInfoCount supplies four mapped data bytes. The
nine new CSV rows include four unknown queue/flag/string access views; their
blank size/type cells earn no extent or exact credit. Source implementations
of these four helpers and the previously mapped callback cover 566 newly
compared original code bytes. All source definitions and relocations resolve:
1,643 functions and 2,611 data definitions across 124 objects.

The callback scores 98.175255%; whole-PC fuzzy coverage increases to
6.224200609% from 6.211660511%. Native objdiff averages 61.1476% across 353,427
original code bytes, including the existing 55-byte overlap. All previous 801
exact functions and 45,746 exact code bytes remain unchanged. None of the five
new routines is exact. CompleteLevel's genuine status lookup is inlined in
its 162-byte rebuilt body and scores 0.125% against the complete 97-byte retail
body; this remains visible in the denominator. The counter adds four exact
initializer bytes. Mapping reaches 1,116,538 / 40,067,072 (2.786672308%).

The frozen `joint-registry-wave14-run` reuses the same 8,043 PC and 13,242 Android
function graphs. It contains 1,431 function seeds, 95 independently reviewed data
anchors, 15 exclusions and zero stale decisions. Its 1,309 inferred function
pairs, 16,852 candidate rows and 2,042 data candidates remain advisory. New
anchor counts do not prove scoring accuracy or allocation recovery.

All 20 prior completed-run files were copied and hash-validated in the frozen
`registry-wave14-baseline` and new run directory before scoring. The baseline
and `registry-wave14-run-copy` records describe this pre-score copy; they are
not hashes of subsequently changed completed-run outputs. The new run's
previous-state snapshot matches the frozen baseline state. Manifest output and
input hashes independently establish current completed-run integrity.

Production ran for 43.12 seconds at 706,068 KiB peak RSS (about 690 MiB) with the
1024 MiB address-space cap and shared heavy-job lock enforced. That job reported
zero swaps. Compilation and retail comparison used two Make workers. The initial
sandboxed full-check attempt's seven compiler fixture failures all reported
Wine socket-bind permission errors; the log remains preserved separately from
the required run with local Wine socket access. No PC boundary, formal type or
complete allocation is inferred from a successful source build.


The required run with local Wine socket access passed all 434 tests in
154.550 seconds (three optional Ghidra fixture skips), formatting for 126
source/header files and Clang-tidy for 124 units. The source EXE links and the
object check resolves every compiled mapping/relocation. The frozen
`registry-wave14-run-independent-audit` independently checks manifest outputs,
input hashes, unchanged graphs, the prior-state snapshot and exactly six
journal additions. Separate source/compiled-contract audits preserve formal
type, allocation and runtime-domain limitations. The next source cohort is
RaceOpponent's existing movement closure; speed-control dependencies retain
specific field and callee holds.


## Race and animation-speed checkpoint

The next source transfer implements two previously accepted callbacks covering
976 original code bytes. Independent current-object audits verify required
float32 rounding, double distance/lateral operations, NaN and clamp ordering,
actual call arguments and normal dependency effects. RaceOpponent scores
91.99448% and SetAnimSpeedMul 85.79032%; neither is exact. Whole-PC fuzzy coverage
reaches 6.249405124% from 6.224200609%, with native objdiff averaging 61.226074%
over 354,403 original bytes including the unchanged 55-byte overlap. Every old
exact function and data result remains unchanged: 801 exact functions and
45,746 exact code bytes. Mapped extent remains 1,116,538 / 40,067,072; five new
literal bindings have unknown complete original allocations and receive zero
extent/exact credit.

The append-only journal explicitly supersedes the previous SetAnimSpeedMul
same-pair decision, refining its normal parser/source contract while keeping
formal types false and clearing the prior source-convention CSV signature.
The first replay correctly rejected the initially missing supersedes ID.
Root verified all 20 copied baseline files remained unchanged before retry;
preserved failure logs and a retrospective root command record distinguish
that observed pre-retry check from later completed-run integrity.

The complete `joint-registry-wave15-run` reuses the unchanged 8,043 PC and
13,242 Android function graphs. Counts remain 1,431 seeds, 95 reviewed data
anchors, 15 exclusions, zero stale decisions, 1,309 inferred function pairs,
16,852 function candidate rows and 2,042 data candidates. These remain advisory;
source transfer and improved reviewed contracts do not imply a scoring accuracy
gain. All 20 prior completed-run files were frozen and hash-verified before
dependent replay. The run finished in 42.34 seconds at 706,168 KiB peak RSS
(about 690 MiB) under the 1024 MiB address-space cap and shared heavy-job lock;
that job reported zero swaps.

`make -j2 check` passed 434 tests in 154.480 seconds with three optional Ghidra
fixture skips, formatting for 126 files and Clang-tidy for 124 units. The source
EXE links and 1,645 function / 2,616 data definitions resolve across 124 objects.
Matching and compilation use two workers. Complete owner/type and broader
runtime safety claims remain held. Next investigate the original CurrentStart
provider blocking UseCurrentSpeed; PC dogfight-level and zero-speed behavior
must replace Android-specific assumptions.


### Current-speed checkpoint replay

The next accepted seed is CurrentStart499ca0/288, independently reviewed
against original Android153840/412. Both this helper and the existing
UseCurrentSpeed44c2a0/336 now have genuine C, independently checked source
contracts and actual MSVC COFF contracts. Two new literal access views retain
blank original sizes/types; the callback signature is explicitly superseded
to a blank historical type. No Android aggregate or allocation size transfers.

Before any changes, all 20 completed wave15 run files, the committed matching
report and the 124 report-listed object hashes were frozen for wave16. All
20 baseline files were then copied and hash-verified before scoring. The
scorer uses its internal shared heavy-job lock and a 1,024 MiB address-space
limit. A redundant outer lock caused an immediate guard rejection; the error
and timing logs were preserved, all 20 outputs were verified unchanged before
retry, and a pre-retry root verification record was saved. Removing the
redundant outer lock retains the scorer's normal internal serialization.

The successful replay took 42.43 seconds, peaked at 704,556 KiB RSS (about
688 MiB), and reported zero swaps. Graph counts remain 8,043 PC and 13,242
Android functions with unchanged graph fingerprints. The new reviewed seed
replaces one inferred hypothesis: 1,432 name seeds, 1,308 inferred pairs and
16,851 function candidate rows. Data candidate rows remain 2,042, reviewed
data anchors 95, rejections 15 and stale decisions zero. Those counts describe
research, not accuracy or source coverage.

Required local checks pass: 434 tests (three skipped), formatting for 126
files, static analysis for 124 units, source-only link and complete object
checks. There are 1,647 compiled functions and 2,618 data definitions across
124 reported objects. Matching adds 624 original code bytes, reaches
6.265641761481748% whole-PC fuzzy coverage and preserves all 801 exact
functions/45,746 exact code bytes. Source-only linking still does not
establish reconstructed gameplay. The separate SetCurrentSpeed indirect
lookup remains held; the next source packet is SetScaleOverride687.


### Scale-override checkpoint replay

The accepted SetScaleOverride44c750/687 inventory callback now has genuine
C and independent source/actual compiled-contract approval. The 645-byte
VC8 output preserves timed-target F32 narrowing, rounded rate storage/reload,
extended weighting and ordered/unordered predicates. Historical signature
remains blank through an explicit supersession; the reset literal view has
unknown original extent/type and receives no coverage or exact credit.

Before integration, all 20 completed wave16 run files, the committed report
and 124 report-listed objects were frozen. All 20 files were copied and
hash-verified before scoring. The scorer used its internal heavy-job lock
and a 1,024 MiB address-space limit; no duplicate outer lock was added. The
replay passed in 42.69 seconds, at 704,144 KiB peak RSS (about 688 MiB), with
zero swaps. Independent single-pass audit verifies all output/input hashes,
previous-state equality, exact map/journal changes and baseline preservation.

Graph counts/fingerprints and research counts are unchanged: 8,043 PC and
13,242 Android functions, 1,432 name seeds, 1,308 inferred pairs, 16,851
function rows, 2,042 data rows, 95 reviewed data anchors, 15 rejections and
zero stale decisions. Research counts establish no accuracy or source credit.
The source transfer adds 687 compared original bytes, reaches 6.276357013318698%
whole-PC fuzzy coverage and preserves all 801 exact functions/45,746 bytes.
Known mapped image bytes remain 1,116,826/40,067,072.

Required checks pass: all 434 tests (three skipped), formatting 126 files,
static analysis 124 units, source-only link and object coverage. There are
1,648 compiled functions and 2,619 data definitions across 124 reported objects.
The full original callback remains nonexact at 54.141594%; source-only linking
does not establish playable gameplay. Next bounded helper work targets the
SnapToPosition/SnapToSockPosition family; optional InitSurfaceInfo callback
951030 and source/query dependencies remain held rather than replaced.
## Structural propagation and bounded data review (2026-10-01)

The candidate-only data channel now exports a compact
`tentative-function-data-candidates.csv` alongside the full JSONL evidence.
Scalar user/root counts support bounded selection; `evidence_record`,
`evidence_byte_offset` and `evidence_byte_length` locate the exact UTF-8 JSONL
record. Review selected records with bounded seeks rather than loading the
whole evidence ledger. Counts alone do not establish independent ancestry,
identity, original allocation or types. Disabled reruns remove both exports.

The seven function and three data identities grow the longer run from 1,329 to
1,528 tentative function pairs, with 1,456 accepted function seeds. This batch
was tested together; the contribution of each anchor is not isolated. Round yields are
763, 437, 200, 80, 33, 12, 3 and 0: convergence takes eight rounds. Normal data
candidates number 2,059; the separate experimental channel has 10,835 rows.
Its CSV is 3,667,180 bytes and full evidence is 315,702,666 bytes. These counts
describe hypotheses, not verified mappings or measured accuracy.

The expanded experiment safely stopped at its original 1,024 MiB address-space
limit without publishing a completed run. A separate retry under 1,536 MiB
completed in 71.20 seconds at 1,055,132 KiB peak RSS with zero swaps. Heavy jobs
remain serialized. Both the failed capture and successful retry are retained.

Global accepted-caller discovery supplies fifteen unmapped review leads before
this release. All published lead signatures use direct calls on both sides;
the broader competitor inventory includes resolved indirect calls. The missing
target audit separates eleven direct edges to nine local text targets from
twenty computed calls to sixteen synthetic external targets. Those external
targets are not missing local functions.

A further disposable repair restores the already accepted
`AIPathCnxSetTemporaryBlock` at `0x0052d5f0` / 192 bytes. Independent replay
preserves all 8,045 previous function IDs/extents and verifies exactly one new
function, for 8,046 PC functions. The retained project is unchanged. This is
graph completeness progress with zero new mapping or source credit; the
1,528-pair experiment above predates this repair.

The next structural release reviews ten more functions (681 known code bytes).
Three missing single-parent setter targets are repaired with complete extents
16, 41 and 41; trailing alignment is excluded. Independent scalar preservation
keeps every prior function ID/extent, bringing the PC graph to 8,049 functions.
With the new identities, 1,467 accepted function seeds yield 1,522 inferred
pairs and 2,060 normal data candidates at an eight-round fixed point. Reviewed
data anchors remain 99 and stale decisions zero. The run finishes in 48.71
seconds at 757,524 KiB RSS, under a 1,024 MiB address-space cap, with zero swaps.
Do not interpret fewer inferred rows as lost accepted identities; compare
pair sets separately when candidates become reviewed seeds. Original identity,
physical caller effects, full PC extent and historical types remain distinct
gates. Mapping identities adds no source reconstruction or exact/fuzzy credit.

The combined release also reviews the creature creation/reset pair and all ten
fixed experimental data leads through original users/operands. It totals twelve
function identities / 3,920 code bytes and ten blank-extent data identities,
without source or exact-data credit. The final run has 1,469 accepted function
seeds, 1,522 inferred pairs, 2,063 normal data candidates and 109 reviewed data
anchors. It reaches an eight-round fixed point in 47.36 seconds, at 746,344 KiB
RSS under the 1,024 MiB address-space limit, with zero swaps. The original
ten-function counterfactual remains frozen separately; six of its hypotheses
became seeds and all remaining pairs survived unchanged.

A separate single-parent context prototype uses ordered neighboring accepted
calls and reviewed data-role profiles to propose 102 unmapped leads. It retains
global competitors and excludes inferred anchors. Known-pair controls recover
six of twenty exact withheld pairs, and five of twenty with two same-parent
neighbors withheld; no wrong-known-neighbor control is recovered. These tests
show fragility, not unknown-pair precision or an accuracy gain. All prototype
leads require original identity, physical ABI and complete PC boundary review.
Candidate-only diagnostic publication must remain separate from accepted
scoring and state. Bounded cache pinning and atomic publication are required
before the prototype becomes an operator tool.

## Bounded ordered-call context diagnostic

`tools/identification_context.py` turns the single-parent prototype into a
candidate-only operator tool. It compares the ordered intervals between accepted
callee anchors inside paired accepted callers, retaining same-context competitors
before filtering mapped entries. Canonical reviewed data access roles provide
additional evidence; unresolved references and missing roles remain explicit
debts. Native names are attached after selection. Inferred pairs cannot supply
anchors, and the result never enters scorer state or the symbol map.

The CLI requires `--graph-pair`, `--accepted-seeds`, `--journal`, `--symbols`,
`--input-pins` and a new `--output` filename. The pin file is a JSON object with
exact SHA-256 values keyed `graph_pair`, `accepted_seeds`, `journal`, `symbols`.
Use the accepted `function-seeds.snapshot.jsonl` from the matching frozen run;
never substitute hypotheses. Graph exports and existing immutable SQLite caches
are stream-verified; missing caches require separate guarded preparation.

The diagnostic takes the internal heavy-job lock and caps address space at
256 MiB before imports. It bounds SQL work, projections, metadata, fanout and
publication, with atomic exclusive output creation. Descriptor captures reject
replacement/growth before parsing; the shared inspection metadata reader uses
the same bounded approach. Sixteen synthetic tests cover competition, withheld
anchors, missing access roles, stale inputs, growth races, no partial publication
and absence of aggregate-test memory-limit side effects.

The frozen preaccept run retains twenty review candidates from 179 compatible
pairs in 1.31 seconds at 44,224 KiB RSS, with zero swaps. After the seven new
function mappings, the diagnostic retains twenty from 180 compatible pairs in
1.29 seconds at 44,116 KiB RSS, also with zero swaps. It charges 2,725,025 metadata
bytes, with no original code/data reads. These are candidate yields, not measured
precision. Missing-role handling is stricter than the old prototype; the old
holdout result and lead count do not establish this tool's accuracy.

The next counterfactual freezes three runs on the same 8,049-function PC export:
the previous baseline, five new qualified CSV bindings, and those bindings plus
two journal-only identity anchors whose ABI/type flags remain false.

| Run | Accepted function anchors | Inferred function pairs | Data candidate pairs |
| --- | ---: | ---: | ---: |
| Baseline | 1476 | 1515 | 2087 |
| Five mapped functions | 1481 | 1512 | 2118 |
| Plus two research anchors | 1483 | 1510 | 2136 |

Exact pair comparisons distinguish promotion from discovery. The five mappings
convert four inferred pairs into anchors and produce one new inferred pair;
there are no other inferred-pair losses. The research anchors convert two more
inferred pairs and produce no new function inference. Data candidates change by
31 additions with no losses in the first stage, then 24 additions and six losses
in the second. Across both stages, that is 55 additions and six losses, rather
than 49 newly discovered identities. Data candidates still require original
instruction and ownership review; ambiguous counter/global neighbors are not
accepted automatically.

The newly inferred pair is candidate DieRumble, PC 0x0054a9b0 against native
Android 0x004894b0. SphereSphereOverlap is the new accepted pair that was not
previously inferred. The union of accepted and inferred function pairs gains
exactly two pairs across both stages and loses none; promotion alone does not
count as finding another function.

Both runs reach a fixed point in eight rounds with the sixteen-round cap. Their
maximum resident sizes are 735,740 and 735,220 KiB, with zero swaps. The final
ordered-call diagnostic takes 1.28 seconds at 44,060 KiB RSS; it retains twenty
candidates from 178 compatible pairs. Its eighteen common retained pairs keep
the same evidence and holds. Two now-mapped initializer candidates leave the
list and two lower-ranked pairs enter it. These are retrieval changes, not
precision or reconstruction gains. Frozen manifests and independent membership
audits remain under ignored analysis.

## Positive intervals across multiple accepted callers

Pass `--context-mode multi-positive` to the same bounded context CLI to inspect
shared helpers that the default single-caller mode excludes. It requires matching
positive intervals around known callee anchors in at least two distinct accepted
parent pairs. Repeated calls from one parent count as one root. Call ordinal,
interval length and missing calls remain descriptive debts; absent or inlined
edges do not reject a pair. All observed interval rivals, including mapped,
one-root and hub rivals, participate before the output limit. Hubs are held;
fanout overflow aborts publication rather than silently hiding alternatives.

This optional mode retains the 256 MiB address-space limit, bounded immutable
SQL caches/work, original input pins, internal heavy-job lock and exclusive
publication. It accepts no inferred anchors and writes no scorer state or map.
The default mode preserves the prior results. Twenty-six synthetic tests cover
both modes; independent controls check asymmetric parent support and hub rivals.

With the same 1,483 accepted parents as the final counterfactual, the first real
multi-parent run inspects 1,069 observed rival pairs in 1,192 comparisons. Of 42
pairs supported by multiple parents, three are eligible unmapped review leads:

| Candidate | PC entry | Native Android entry | Distinct accepted parent roots |
| --- | --- | --- | ---: |
| AISysGetPathPos2 | 0x0043e040 | 0x00181410 | 3 |
| GameCam_Judder | 0x00549de0 | 0x004d8370 | 3 |
| Player_ResetContexts | 0x0055e4e0 | 0x0046b240 | 3 |

All three were already inferred by the normal matcher. This adds review evidence
for shared helpers, not three newly discovered identities. Their reviewed data
profiles are empty, so equality of those profiles is not strong data evidence.
The run takes 1.25 seconds at 45,192 KiB RSS with zero swaps. A same-input default
control reproduces the previous complete result except the producer file hash.
Original bodies, caller contracts and complete extents still require review.

After seven further qualified function bindings, a journal-only AddFade identity
and the unknown-extent terrain-counter identity, the normal matcher has 1,491
accepted function anchors, 1,507 inferred pairs, 2,356 data candidates and 110
reviewed data anchors. Four old inferred pairs convert to anchors; four other
accepted pairs were previously candidates. Candidate PC 0x00578e10 against
native 0x0030f230 moves from baseline rank two into the inferred set. This is a
new inferred membership, not a newly generated raw candidate. All old accepted
or inferred pairs survive, and their union gains five pairs. Data candidates gain
225 pairs and lose five, for a net increase of 220. These are retrieval changes,
with no automatic identity, allocation or source credit. The run again reaches
a fixed point in eight rounds, taking 48.46 seconds at 742,244 KiB maximum RSS
with zero swaps and the same 1,024 MiB address-space cap.

After reviewing the three shared-helper leads, correcting the debris initializer
rival and adding the journal-only collision identity, five previous inferences
become accepted anchors. The normal matcher now has 1,496 accepted anchors,
1,324 inferred pairs and 2,380 data candidates. All 1,491 previous accepted
anchors survive. Five other existing raw candidates become inferred, while
183 unconverted old inferences lose that status: 180 remain raw candidates and
three leave the candidate pool. The accepted/inferred union falls from 2,998 to
2,820. This is an advisory-confidence regression, not a loss of verified maps.
The explicitly rejected debris-parser pair was absent from both raw candidate
pools and is not a direct member of those losses.

This exposes a nonmonotonic mechanism in the propagation policy: an inferred
pair carries inherited roots, while its promotion to an accepted seed gives it
one fresh root. Descendants may then lose the multiple-root support required
for inference. Specific causal paths still require an evidence audit; the
membership counts alone do not attribute every loss to that mechanism. Data
candidates gain 28 pairs and lose four, for a net increase of 24. The run settles
in seven rounds, takes 45.85 seconds at 692,192 KiB maximum RSS with zero swaps,
and retains the 1,024 MiB address-space cap. Original review, complete extents
and qualified physical contracts remain separate from advisory propagation.

The final multi-parent diagnostic retains no eligible leads: its three former
leads are now mapped. It inspects 1,080 observed rivals in 1,198 comparisons,
with 37 multi-parent compatible pairs. Ten held examples remain; one gains three
PC rivals. This does not establish that the remaining ambiguities are exhausted.
The diagnostic takes 1.47 seconds at 45,016 KiB RSS with zero swaps. An initial
attempt supplied the JSON symbol inventory where the accepted-seed JSONL was
required; the format guard rejected it before publication. The failed and
successful runs together charge 2,991,059 metadata bytes, below eight MiB.

## Caller, callee and data neighborhoods

The optional `--context-mode neighbor-positive` diagnostic compares explicit
PC and Android query cohorts against accepted caller/callee anchors and reviewed
data identities. Supply one or more `--query-pc` and `--query-android` entries,
using the same graph, accepted-seed JSONL, journal, symbol CSV and input-pin
arguments as the other context modes. For example, append:

```sh
--context-mode neighbor-positive --query-pc 0x0062a6a0 --query-android 0x003c678b
```

Distinct accepted function pairs count as independent roots; repeated calls and
caller/callee directions do not multiply a root. Data aliases form one component.
Known data access roles, widths and offsets can support a comparison; GOT address
loads alone cannot establish the accessed object's width. Two function roots, or
one function root plus a compatible reviewed data component, provide stronger
review evidence. These thresholds do not establish identity or a PC contract.

The initial query signatures freeze the retrieval vocabulary. All observed
users of those anchors, including mapped, weak and rejected alternatives, remain
in the rival pool. Secondary endpoints have only that vocabulary inspected and
remain held because their complete signatures are unknown. Induced call edges
between hypotheses provide descriptive witnesses without creating accepted
roots. Missing calls remain unknown because the compilers inline differently.

The first retained trial queries AnimMiscFlags against the frozen preacceptance
registry: 1,496 anchors produce 71 PC and 65 Android endpoints, with 3,132 positive
pairs and 15 stronger query-connected pairs. It takes 2.15 seconds at 57,284 KiB
maximum RSS with zero swaps. No pair is eligible: the conservative competition
rule includes every pair sharing even one positive token. This exposes a need
for full-signature ranking; the result is not an accuracy measurement or proof
that the query identity is wrong. Original review separately verifies the query.

Three preceding attempts abort without publishing partial results: recursive
endpoint growth, unrelated hub-profile fanout, then excessive component work.
Their combined metadata charge is 5,897,481 bytes. Freezing the query vocabulary,
restricting rival profiles and indexing induced edges by source lets the retained
trial finish without increasing the limits. Its positive component witnesses
preserve physical call-site multiplicity and ordering.

The mode permits at most 64 queries and 128 retrieved endpoints per side, 16,384
endpoint pairs, 4,096 induced edges per side and 200,000 component comparisons.
It retains the 256 MiB address-space limit, eight MiB metadata cap, bounded SQL
work and exclusive publication. Overflow aborts rather than hiding rivals.
Fifty-six context tests cover the new mode and existing controls, including
normalized GOT rivals, alias roots, exact-query scope and genuine work overflow.
It changes neither the normal scorer nor the symbol map automatically.

After seven original-reviewed function additions and three data identities, the
normal matcher has 1,503 accepted anchors, 1,319 inferred pairs and 113 reviewed
data anchors. Six previous inferences become accepted. AnimMiscFlags becomes
accepted from a prior rank-one candidate; PC 0x004a4900 against native Android
0x0015f830 becomes inferred from another prior rank-one candidate. All previous
accepted and inferred membership survives, so their union grows by two pairs.
Data candidates fall from 2,380 to 2,365: the fifteen removals include the three
newly reviewed identities, which leave the candidate pool. No new data candidate
appears. This run settles in seven rounds, taking 46.95 seconds at 690,676 KiB
maximum RSS with zero swaps and the unchanged 1,024 MiB address-space limit.

The next version adds a separate `review_candidates` ranking without changing
conservative eligibility or removing rivals. It ranks distinct common function
roots and known data components, then weights positive evidence by inverse
global anchor fanout. Each function root or alias component contributes at most
once. Distinct induced target pairs provide a final comparison feature only
when both endpoints already have direct support; repeated sites and unrooted
cycles cannot manufacture evidence. Equal scores share a rank group. Signature
equality within the inspected scope is descriptive, never an absent-edge penalty
or proof of whole-function identity. Sixty-four context tests cover these rules.

On the same frozen AnimMiscFlags inputs, all earlier candidates, holds, rivals
and witnesses survive unchanged. The new ranking puts the real query in its
first group alongside five unassigned alternatives. It distinguishes 120
incidental one-root rivals from fourteen stronger rivals, but resolves no
identity. This trial takes 2.26 seconds at 63,968 KiB maximum RSS with zero swaps.

A new SetForceBack query, using 1,503 anchors, inspects eight PC and ten Android
endpoints, sixty positive pairs and seventeen holds. Four review rows remain;
the real pair ties the PIC thunk on one function root. Its reviewed normalized
data reference lacks compatible known access widths, so the current strong-data
ranking does not use that identity link. Reference identity and access-contract
evidence need separate treatment. This trial takes 1.56 seconds at 41,452 KiB
RSS with zero swaps. A preceding two-function query exceeds the unchanged
128-endpoint limit and aborts without output, using 40,572 KiB RSS. These are
bounded retrieval/ranking observations, not accuracy or source-coverage gains.

The focused mode separates reviewed data-reference identity from a verified
memory-access contract. A canonical reference contributes at most once per
reviewed alias component, with inverse global user count as advisory weight.
An unknown-width normalized GOT reference remains weak evidence: it supplies
neither a four-byte object read nor strong support, type or eligibility credit.
On the same frozen SetForceBack inputs, the real pair now ranks ahead of the PIC
thunk (weights 3/8 versus 1/8); the already assigned ResetForceBack remains a
competing pair of equal weight. All sixty positive pairs, seventeen holds and
previous witnesses remain. The trial uses 41,768 KiB RSS with zero swaps.

`--context-mode data-root-cohort --data-root-pc 0x87b8ec` compares all function
users of the selected reviewed data component. Selectors may repeat for up to
eight components. Incidence includes raw target, allocation owner and normalized
target while retaining the physical source function. Each cohort member receives
its accepted caller/callee and reviewed-data profile. Global fanout uses scalar
distinct-user counts; profiles of outside functions remain uninspected. Every
row consequently retains an outside-rivalry hold and cannot become an automatic
match. Alias components, call sites and direction tokens cannot multiply roots.

The position-global trial inspects exactly four PC and four native Android
functions, sixteen positive pairs and sixteen held rows. Four unassigned review
rows remain; the true ForcedBackCode pair ranks first with six distinct accepted
callee roots. This finishes in 3.08 seconds at 43,336 KiB RSS with zero swaps,
under unchanged limits. Seventy-one context fixtures cover weak-reference
ranking, complete selected cohorts, scalar fanout, reserved rivals, outside
holds, alias deduplication and overflow. This mode narrows review work; it does
not establish global uniqueness or reconstruct source automatically.

`tools/ghidra/ExportSelectedFunctionShapes.java` adds selected-function summaries
from existing analyzed programs. Invoke it with `analyzeHeadless -readOnly
-noanalysis -postScript ExportSelectedFunctionShapes.java OUTPUT ORIGINAL DELTA
SELECTORS`; a `selected-function-shape-selectors-v1` JSON record supplies `side`,
`program`, `binary_sha256`, `delta` and a list of canonical original `entries`.
Use the existing identification heavy lock, local Ghidra configuration/cache and
sequential jobs. The tested PC/native jobs use 512 MiB/1 GiB Java heaps. Validate
each result with `python3 tools/identification_shapes.py SELECTORS OUTPUT
tools/ghidra/ExportSelectedFunctionShapes.java`.

The exporter checks original/program body hashes and emits instruction classes,
Pcode, conservative memory summaries and analyzed intrabody CFG relationships.
It permits sixteen functions, 128 KiB total selected body, 4,096 instructions per
function and a two MiB output. It never emits raw code or grants identity, ABI,
extent, type or complete-runtime-CFG review. Direct memory storage varnodes are
counted alongside intrinsic LOAD/STORE operations; flow operands and embedded
LOAD/STORE storage operands are excluded with explicit debt counters. These are
incomplete raw-Pcode occurrence summaries, not exact instruction-access counts.
Indirect flow operands can contain real data reads, so absence cannot prove an
object is unused. Fifteen checker fixtures cover accounting, pins, bounds and
import-safe resource limits; seven Java accounting fixtures run before export.

Actual exports of three PC and four native functions pass body and schema checks
at 376,068/383,756 KiB RSS with zero swaps. SetForceBack has five absolute PC and
five indirect native non-stack stores; the PIC helper has none. Nevertheless,
its four PC CFG blocks become one native block through conditional moves.
AnimMiscFlags also differs in return/block counts across compilers. Shapes remain
advisory context; they are not new anchor roots or exact cross-platform CFG tests.
The earlier LOAD/STORE-only output omitted direct PC stores and is preserved as
a failed memory-accounting control under ignored analysis.

After five newly reviewed function identities and the actor data identity, the
normal matcher has 1,508 accepted function anchors, 1,495 inferred pairs and 114
reviewed data anchors. All five new function anchors were already inferences.
The exact two-run comparison retains all 1,503 earlier accepted anchors, finds
184 newly inferred memberships and three former inferences now held as rank-one
candidates. Accepted-plus-inferred membership grows from 2,822 to 3,003, a net
181 hypotheses rather than 181 verified identities. No existing pair is rerouted
to another endpoint. The three demotions remain available for review.

Data candidates grow from 2,365 to 2,400: 54 appear and nineteen leave, with
2,346 retained. The actor identity is the only new reviewed data anchor. Graphs,
original hashes, readers and normal scoring features remain unchanged; reviewed
decisions and the symbol inventory are the changed inputs. The run reaches its
fixed point in eight rounds, taking 48.15 seconds at 738,148 KiB RSS with zero
swaps and the unchanged 1,024 MiB address-space limit. This measures the spread
of structural hypotheses after review; it does not establish their accuracy.

A further exact historical comparison finds that 181 of the 184 added
inferences restore pairs demoted in wave 30. Three additions lie outside that
historical loss set; their presence in still older raw candidate pools remains
uninspected. Restored membership establishes neither correctness nor the cause
of recovery, and the larger total is not evidence of exponential mapping growth.

`tools/identification_shape_pairs.py` compares declared pairs from two validated
selected-shape exports. It requires six explicit input pins: PC/native shapes,
their selectors, exporter source and a `selected-positive-shape-pairs-v1` record
whose `pairs` contain canonical `pc` and `android` addresses. Supply these through
the corresponding CLI arguments and `--input-pins`; `--producer-pins` additionally
binds the expected module and checker source hashes. The worker captures producer
pins before checking and verifies them again before publishing. Changing either
producer aborts without a completed output, including a change after staging.

Each pair reports shared positive non-stack memory direction/width tokens and
Pcode operation presence. Absolute and register-relative access classes do not
prove the same object, and duplicate occurrences do not increase the weight.
There are no identity-specific motifs, names, absent-edge penalties or required
CFG equivalence. Missing selected bodies yield null comparisons and explicit
holds. Every row retains outside-scope rivalry and original-review holds, with
zero new roots and no scoring, identity, ABI, extent or type promotion. Limits
remain 256 MiB address space, eight MiB metadata, two MiB output, 64 KiB rows,
200,000 work units and 128 declared pairs. Twenty pair fixtures supplement the
existing fifteen shape-checker fixtures.

The complete six-pair AnimMiscFlags tie uses three PC and four native endpoints.
The true pair shares two non-stack read widths and fifteen Pcode operations;
the five alternatives share zero or one width and four to thirteen operations.
All six remain present and held. This separates the inspected alternatives with
generic positive features, not a global uniqueness or accuracy claim. The
maintained CLI reproduces the frozen trial's features, holds and accounting in
0.05 seconds at 21,664 KiB RSS with zero swaps. Its metadata charge is 50,579 bytes;
no original bytes or SQL queries are read by the pair comparison itself.
### Reference expansion and corrected camera cohort (2026-10-02)

The normal wave33 run uses the unchanged graph pair and scorer with the reviewed
Hint_SaveGameState and corrected GameCam_Blend mappings, three ForceBack data
identities, four specific crossed data rejections and one camera rejection.
It retains all 1,508 previous function anchors and adds two, with 117 reviewed
data anchors, 22 rejections and no stale decisions. It converges in seven rounds
to 1,317 tentative function pairs and 2,400 data hypotheses. The measured run
takes 45.84 seconds with 689,944 KiB peak RSS, zero swaps and a 1 GiB address-space
limit. Heavy jobs remain serialized.

Exact baseline comparison shows Hint converted from an inference to a reviewed
anchor; the correct camera pair was absent from the previous raw candidate pool.
There are no new inferred pairs and 177 unconverted inference losses. Of those,
174 are earlier wave30 losses that wave32 restored. The rejected private-helper
camera pair is absent from the new pool; exact candidate projections and ranks
for other losses remain in the audit. The accepted-plus-inferred union loses
177 pairs and gains the corrected camera pair. Data hypotheses have four gains
and four losses, with 2,396 common pairs. These are output transitions, with no
single-input causal attribution or inferred-identity accuracy claim. The two
functions add 397 inventory bytes; unknown data extents add no mapped bytes.

The independent main and lineage audits are frozen as
`structural-wave33-normal-control-independent-next1.json` and its `-lineage.json`
under ignored continuation analysis. A separate `-prose-correction.json`
preserves a wording correction without changing the frozen JSON results.

### Captured paths and larger child cohorts (2026-10-02)

`tools/identification_paths.py` records finite paired A-to-B-to-C paths from a
compact `anchored-child-capture-v1` input. It preserves caller context while
opening children behind a bridge that the direct-neighbor comparison could
miss. It does not manufacture independent roots by repeating the same path,
traversing a cycle or promoting a bridge. Its capture producer must pin the
graph pair, accepted crosswalk, journal, CSV and both indexes, and preserve all
captured outgoing incidences for every selected bridge.

Run `python3 tools/identification_paths.py --capture CAPTURE --pins PINS
--output NEW_OUTPUT`. The pins JSON contains exactly `input_sha256` and
`producer_sha256`. The standalone worker caps address space at 256 MiB before
imports; eight MiB metadata, two MiB output, 64 KiB rows, 200,000 work units,
128 declared child pairs, 64 children per bridge side and 4,096 edges per side
bound the job. Input and producer changes abort publication. Twenty-nine
fixtures cover paths, rivalry, cycles, aliases, limits and publication races.

A null target is permitted only with unknown role and target kind. It retains
its real source and site, supplies no positive path and is excluded only from
the known-address child set. Known addresses with unknown roles also supply no
positive path. Declared completeness means captured known incidences plus
recorded unresolved observations; runtime target completeness remains false.
Every candidate retains outside rivalry and original identity/ABI/extent/type
review holds. Qualified data-alias metadata is deduplicated, without establishing
object contracts or adding roots. Historical rows retain their prior input pin
and supply no current evidence.

The LoadPerm2-to-APILoadCharacterModels pilot preserves 165 captured control
occurrences, 19 PC and 17 native known children and ten unresolved targets.
Eleven already mapped child pairs serve as controls. All 48 combinations of the
remaining eight PC and six native children are retained, giving 59 declared
pairs. The accepted-map name joins for the two parent anchors remain explicit
limitations. All rows remain held; the diagnostic alone verifies no identities.
The V3 trial takes 0.05 seconds at 22,476 KiB RSS, with no original or SQL reads.
The maintained reader additionally bounds a read by remaining metadata capacity
plus one guard byte, charging failed reads before aborting. Its default-cap
replay preserves every path, unresolved observation and hold.

Selected Ghidra exports cover those fourteen unpaired endpoints, including the
nullsubs, PIC thunk and security-cookie rival. The 48-pair comparison runs in
0.05 seconds at 22,096 KiB RSS, charging 80,504 metadata bytes. Sequential
read-only PC/native exports peak at 385,556/386,940 KiB RSS with zero swaps.
The reset helper shares five positive memory direction/width tokens with its
reviewed counterpart, versus at most four with the inspected alternatives.
The two animation loaders each tie both native loaders at two memory tokens
and eighteen Pcode operations. Generic shape cannot distinguish those siblings;
their raw-buffer versus filename-reader dependency and caller arguments require
original-code review. No shape count is a probability or global uniqueness proof.

With the three private helpers independently reviewed for identity and complete
PC extent, the normal matcher retains every earlier accepted pair and reaches
1,513 accepted anchors, 1,316 tentative pairs and the same 2,400 data candidates.
Two accepted loaders convert earlier inferences; reset converts a rank-one
candidate. One other rank-one candidate (`0x00630b40` / `0x003cba8f`) becomes an
inference. Accepted-plus-inferred membership grows from 2,827 to 2,829, with no
losses or reroutes. All 117 reviewed data anchors and 872 map-name data labels
remain unchanged. The run takes 49.02 seconds at 688,876 KiB RSS with zero swaps,
under the unchanged 1 GiB limit. The new path diagnostic is separate from scoring.
These exact transitions do not establish accuracy or single-input causation.

### Shared state and field access refinement (2026-10-02)

The `apicharsys` shared-state frontier captures 52 PC and 67 native users,
including twenty already paired users. Its 32 by 47 unpaired alternatives
remain held. Both captured sides have one observed writer, but writer incidence
alone does not establish identity. Original instruction, caller and native
symbol review independently confirms APICharacterSysInit at PC `0x00630b40`
and native `0x003cba8f`, with a complete 541-byte PC body. The packed-buffer
dependency NuAnimData2LoadBuffFromPAK adds a separate 69-byte reviewed identity.
Provider effects and formal types remain unresolved; neither function enters
the CSV or contributes reconstruction coverage.

The writer and three reviewed helper reads identify the PC slot `0x02913104`
with the original native GLOBALOBJECT `0x0123f918`. The native GOT cell at
`0x006122f0` carries that address and remains distinct from the object. This
journal row establishes data identity only. Four-byte accesses and the native
four-byte symbol do not establish a complete PC allocation or a transferred
type. The existing source and CSV stay unchanged.

The normal wave35 run retains all previous accepted pairs and reaches 1,515
accepted anchors, 1,320 tentative pairs, 118 reviewed data anchors and the same
2,400 data hypotheses. Both new function identities convert previous inferences.
Six other functions become inferences, with no losses or reroutes, increasing
accepted-plus-inferred membership from 2,829 to 2,835. All six carry the new data
anchor in their evidence lineage. The run takes 45.60 seconds at 692,036 KiB
peak RSS, zero swaps and the unchanged 1 GiB address-space limit.

A controlled third run adds only the reviewed `apicharsys` data row to the
frozen baseline journal. Every other input hash stays fixed. It reproduces
exactly the same six new accepted-plus-inferred pairs, with no losses and
unchanged data hypotheses. Its 1,513 accepted and 1,322 inferred pairs differ
from the normal run only because the two function promotions remain inferences.
Thus this one data anchor causes the six output additions under these fixed
inputs; it does not establish their original identities or general accuracy.
The control takes 45.39 seconds at 692,304 KiB RSS with zero swaps. Independent
membership and composition proofs are frozen as
`structural-wave35-apicharsys-only-control-independent-next1.json` and its
`-composition.json` under ignored continuation analysis.

Field access refinement is the next diagnostic: follow a reviewed global slot
through its loaded pointer, field offsets and indexed accesses, preserving
reads, writes and caller context. The initializer and both animation loaders
form the finite pilot, including all four crossed loader alternatives. Existing
Pcode histograms tie the loaders; operands and value origins are needed to
distinguish their roles. This prototype remains separate from scoring until
its actual output is checked. Cross-platform field correspondence, unknown
call effects, partial register writes and unresolved targets remain explicit
limitations; repeated uses of one anchor do not create independent roots.

### Animation anchors and field provenance trial (2026-10-02)

Original-code review confirms Animate_CHARACTER at PC `0x004a8f20` and native
`0x0016ae60`, and UpdateMiniAnimPacket at PC `0x0062f0a0` and native
`0x003cf1e7`. Their complete PC bodies are 1,024 and 85 bytes. Both identities
enter the research journal; unresolved caller contracts and aggregate types keep
them out of the source map. The normal wave36 run converts their earlier
inferences to reviewed anchors, reaching 1,517 accepted and 1,318 inferred
pairs. Accepted-plus-inferred membership stays exactly 2,835, without losses or
reroutes. The 2,400 data hypotheses retain 2,395 pairs and exchange five pairs
in each direction. The new PC targets are interiors of existing ContextInfoTable
and WorldInfo owners; these proposals do not identify new standalone objects.
The run takes 45.60 seconds at 692,040 KiB RSS with zero swaps under 1 GiB.

The field trial exports raw Pcode operands and machine control flow for the
initializer and two animation loaders on each platform. Existing body checks
validate all six selected exports against the originals. Serialized read-only
Ghidra jobs take 4.41/4.31 seconds and peak at 468,464/397,432 KiB RSS, with zero
swaps. This scratch exporter preserves register storage and overlapping writes,
instruction-local unique values and branches, and distinct RAM and other address
spaces. It adds no roots or scoring inputs.

The bounded consumer follows a reviewed slot into loaded pointers and subsequent
field accesses. Store values are snapshots; events are collected after the
control-flow fixed point. The native GOT carrier supplies an address, separately
from a loaded object value. Unknown calls, unreviewed Pcode user operations and
unresolved computed flow keep downstream provenance conditional. Reviewed leaf
effects are qualified by their original-code contracts; they do not establish
cross-platform function identities. The pilot retains the writer control and
all four loader cross-pairs, with outside alternatives held. Generic roles may
support correspondence without equal numeric offsets or strides; neither raw
operand exports nor synthetic fixtures alone establish useful discrimination.

The actual five-pair consumer trial passes its source, input and publication
checks and takes 0.21 seconds at 30,320 KiB RSS with zero swaps under 256 MiB.
All four loader cross-pairs share the same single positive role. Each PC loader
has two positive and twenty-two conditional field events; each native loader
has one positive event. Distinct PC fields at offsets 36 and 44 collapse to
the same generic role, so this prototype does not distinguish the loaders.
The writer has twenty-three conditional PC events and no native field events,
with no shared roles. No pair is promoted. The next refinement must preserve
distinct field identities and learn correspondence from paired value-source and
access context, without requiring equal platform offsets.

Exact graph queries locate the first writer barriers at PC `0x00630b49`, calling
`0x0062a5e0`, and native `0x003cbaf3`, calling the `memset` PLT endpoint at
`0x000d8b80`. Complete writer direct-call lists retain six incidences on each
side. Original provider effects and the dynamic backend remain separate review
questions; an imported name alone supplies no preservation certificate.

Two more reviewed function identities, Tag_DrawIcon_Batman and AddGameMsg,
convert prior inferences in wave37. The normal run retains all 1,517 previous
accepted pairs and reaches 1,519 accepted and 1,317 inferred pairs. One former
rank-one candidate, PC `0x00570a00` / native `0x004c17a0`, becomes an inference;
there are no unconverted losses or reroutes. Accepted-plus-inferred membership
grows from 2,835 to 2,836. Data hypotheses have 27 gains and ten losses, with
2,390 common pairs and 2,417 total; reviewed data anchors and name labels stay
at 118 and 872. These transitions do not verify the new hypotheses or establish
single-input causation. The run stops after seven rounds with no new pairs,
taking 45.46 seconds at 692,160 KiB RSS, zero swaps and a 1 GiB limit.

Original provider review verifies the PC return-only target. The 122-byte PC
memset wrapper has an optimized tail outside the reviewed normal path; the
writer's first 56-byte call proves the normal-path domain, while its other three
counts are dynamic and remain held. The native endpoint is an exact ELF import
transport, whose backend is absent from the original library. Any preservation
under an external System V i386 ABI and normal-return assumption must use a
separate opt-in contract and leave downstream observations conditional. It is
not a verified leaf-effect certificate or an additional matching root.

The V3 field consumer passes 35 fixtures and a separate source review. Three
pinned trials retain strict behavior, add only reviewed PC effects, and then
add the conditional native import contract. Strict functions and pair rows
exactly match V2. PC effects recover nine positive writer events; the opt-in
native contract recovers nine conditional events and three conditional-inclusive
shared role classes. There are still no shared positive writer roles, and all
four loader cross-pairs remain unchanged and tied. The trials take 0.23/0.20/0.20
seconds at 30,820/30,860/30,948 KiB RSS, with zero swaps under 256 MiB. Event
output lacks store-value ancestry and CFG partial-order witnesses, so field
correspondence is still a hypothesis. The controls are frozen in
`structural-field-origin-actual-three-arm-independent-next1.json`.

Original shared-message pool and cursor identities add two data anchors in
wave38, with allocation extents and types held. The normal run retains all
1,519 accepted function pairs and adds four inferences, reaching 1,321 inferred
pairs and an accepted-plus-inferred union of 2,840. There are no function-pair
losses or reroutes. Data candidates decrease from 2,417 to 2,416: the only lost
pair was the PC active-byte member `0x00947cd8` paired with the entire native
pool `0x01298800`. Reviewed data anchors rise from 118 to 120; name labels stay
at 872. These are fixed-run transitions, with no inferred-identity accuracy or
individual-anchor causal claim. The run takes 45.86 seconds at 692,044 KiB RSS
with zero swaps under 1 GiB. Exact membership and root lineage are retained in
`structural-wave38-normal-control-independent-next1.json` and its supplements.

The next owner diagnostic must distinguish an indexed static reference base
from a runtime effective address. Reviewed member/site incidences can connect
that field family to its parent while preserving raw targets, offsets and access
widths. Unknown runtime indices and PC allocation boundaries remain held; no
arbitrary owner interval or extra independent root is introduced. The original
library's GOT carrier continues to represent address transport separately from
object or member access.
