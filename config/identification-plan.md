# Incremental code and data identification plan

Prepared 2026-09-30. This implements the discovery work in
[progress-plan.md](progress-plan.md), independently of gameplay reconstruction.
The authoritative accepted map remains [symbols.csv](symbols.csv); the existing
[identification guide](identification.md) defines PC identity, ABI and extent rules.

The implementation and checked local commands are documented in
[identification-tools.md](identification-tools.md). The maintained pipeline uses
`identification_run.py` for frozen inputs, `identification_graph.py` for typed raw
graphs, and `identification_joint.py` for orchestration around the existing function
scorer. `identification_review.py` and `identification_tables.py` retain independent
review statuses and bounded pointer/RTTI evidence. The small
[review journal](identification-decisions.jsonl) includes the verified Customiser
chain and a rejected random-state identity; it does not change the accepted CSV.
Actual experiment results and remaining limitations are recorded in the runbook.

Memory correction from the first full pilot: raw JSONL exports remain authoritative,
but their query indices use local SQLite through Python's standard library. Full
Python graph copies and duplicated candidate reference lists caused unacceptable
memory pressure. Identification now runs one job at a time under a default 1 GiB
address-space limit; incomplete or failed runs are never published. This local
disk index replaces the original all-in-memory indexing choice without adding a
service or external dependency.

## Outcome and tool choices

Build a repeatable, inspectable mapping pipeline for the original PC executable
and named Android library. It must identify functions and data together, retain
alternatives and unresolved regions, and improve as verified mappings accumulate.
It must distinguish identifying a symbol from reconstructing its implementation.

Use the installed Ghidra and existing Python tooling. The verified local setup is
Ghidra **12.1.2_DEV** at `/opt/ghidra`, with Java **21.0.12.1**. Both originals are
32-bit x86. Headless analysis, Version Tracking and BSim are bundled. BSim will
use its local H2 database; ordinary graph state will use JSON/JSONL and CSV with
Python standard-library indices. No graph database, server, new web UI, learned
model or alternative disassembler is required.

BinDiff is an optional bounded experiment after the baseline exists. Use it only
if a packaged Linux executable and compatible Ghidra BinExport extension work
with this installation, or the extension builds unmodified through its documented
build. Do not replace Ghidra, patch a dependency stack or build the full BinDiff
engine just for this experiment. Other analysis stacks require a demonstrated
gap in this pipeline before being considered.

Initial inspection found no BinDiff executable or BinExport extension. During
implementation the packaged BinDiff 8 executable installed locally, and official
BinExport sources built unmodified against this Ghidra installation. The pilot
added no top-five retrieval gain over graph plus BSim; it remains optional.
Pinned hashes and checked commands are recorded in the operator runbook.
See the [official BinExport releases](https://github.com/google/binexport/releases)
and [BinDiff releases](https://github.com/google/bindiff/releases).

## Fixed architecture

```text
Original PC PE + Android ELF
          |
     Ghidra exports + existing ELF/PE readers
          |
     immutable raw graph snapshot
          |
     map snapshot + reviewed decisions + optional clean source overlay
          |
     function/data candidates <--- BSim candidates
          |                     <--- optional BinDiff candidates
     evidence packets + competing identities
          |
     agent verification of PC identity / ABI / extent
          |
     reviewed decisions; qualified rows integrated into symbols.csv
          |
     recompute affected input overlays and rescore
```

The state is incremental: preserve exports and reviewed decisions between runs.
Initially rescore the complete candidate set deterministically after an anchor
change. The current scorer depends on global rarity weights, competing candidates
and one-to-one reservations; a neighbor-only update could silently leave stale
rankings. Optimize rescoring only if measurement shows it is a bottleneck, and
require exact agreement with a full replay before enabling that optimization.

## Work packages and release order

| Package | Concrete deliverable | Completion gate |
| --- | --- | --- |
| 1. Freeze inputs and baseline | Explicit run context, saved map, versioned manifest, baseline reports | Same frozen inputs reproduce existing scoring; active edits cannot enter a running experiment |
| 2. Preserve code/data graph | Ghidra export script, raw-address/data-object model, adapters for retained exports | Raw edges survive without named anchors; address conversion and function ranges validated |
| 3. Joint matching, first useful release | Data candidates, reviewed data-to-function propagation, evidence packets | Reproduce Customiser chain; measure known-data holdouts and review the first candidate families |
| 4. BSim comparison | Local Android signature database and PC query adapter | Measure new retrieval beyond package 3 on the identical evaluation split |
| 5. Tables and missing graph edges | Pointer tables, validated RTTI/vtables, tail calls and bounded gap review | Demonstrate reviewed discoveries unavailable to direct calls alone |
| 6. Persistent review and whole-program inventory | Decisions/rejections replay, stale-state handling, remaining-region reports | Full replay equivalence, no silent acceptance, repeatable code/data inventory |
| Optional. BinDiff | Compatible export + one comparison + imported candidate evidence | Keep only if it provides useful additional reviewed candidates at low setup cost |

Packages 1-3 are the first implementation milestone. Package 4 can run in
parallel once package 2 establishes program identity and address normalization.
Packages 5-6 follow measured results; basic decision recording exists from package
1, while package 6 completes its replay and reporting workflow.

## 1. Freeze inputs and establish the comparison baseline

Refactor `tools/identification.py` to take an explicit run context instead of
module-global output paths and live-map reads. Extend `tools/check.py` with
explicit map/output arguments while keeping the current `--identify` defaults.
Snapshot the map once and use it for feature generation, seeding, holdouts and
provenance. Do not use monkeypatches as the production snapshot mechanism.

Each run manifest records both original hashes, map hash, exporter/scorer version,
Ghidra build, language/compiler specification, analysis settings, program image
base, address transformation, graph hashes and source-overlay fingerprints if used.
Separate raw extraction from map-derived labels so changing the CSV does not
require redisassembling either binary. Build outputs in a temporary directory and
publish a complete manifest atomically; incomplete runs are not reusable caches.

Use the retained frozen binary-only run as the initial regression input:
1,165 seeds, 1,207 tentative pairs, and 181 recovered labels from 292 hidden pairs.
These figures belong to its saved map; they are not assertions about a later map.
Preserve the existing function scorer as the baseline mode throughout evaluation.

Completion checks: reproducible candidate ordering, identical baseline decisions
on the same inputs, rejected incompatible caches, and a test changing the live
map during a run without changing that run's result.

## 2. Export a graph that preserves anonymous data

Promote the retained `/tmp/SagaPcCandidates.java` approach into a maintained
`tools/ghidra/ExportReferenceGraph.java`. Reuse the analyzed PC project at
`build/analysis/ghidra/saga-pc.gpr`. Validate the Android analyzed program before
reuse; its existing project uses a documented +0x10000 rebase. Normalize every
exported address to the original ELF VA, and verify the transform using memory
block/file mappings and known symbol bytes. Do not assume the rebase from a path
or program name. Import the original ELF into an isolated analysis project if a
compatible verified database cannot be reused.

An Android project candidate found in recent logs is
`/tmp/saga-plus-two-ghidra.Ip19bO/matching-focused.gpr`, program `/libTTapp.so`.
Its original hash and address base still need validation; the temporary location
is not a durable pipeline dependency. Do not confuse other ARM/aarch64 Android
projects on this machine with this x86 reference.

Export existing projects read-only with analysis disabled. Any analysis changes,
function-boundary repair or review annotations belong in a dedicated working copy,
not an active shared database. Use one writer per Ghidra project; never bypass a
project lock. Keep all projects, exports and caches under ignored `build/analysis/`.

Use a versioned manifest plus deterministically ordered `nodes.jsonl` and
`edges.jsonl`. Stable IDs include the binary hash, address space, normalized
address and entity kind. Keep aliases as separate symbol records attached to the
same entity; a demangled basename must not merge file-local symbols.

| Record | Required information |
| --- | --- |
| Function | Entry, complete list of analyzed body ranges, body bytes, thunk target, raw/demangled aliases, name provenance, boundary status |
| Raw data site | Referenced VA and memory block, initialized/zero-filled state, owner candidates; extent unknown unless independently established |
| Data object | Symbol/owner identity, base, optional extent, aliases, section, known interior offsets and evidence for the boundary |
| Call edge | Caller, callsite, target or unresolved expression, direct/indirect/tail classification, site order; retain repeated calls |
| Access edge | Function, instruction/operand, raw target, read/write/read-write/address-taken role, width if proven, owner and offset if known |
| Pointer edge | Source object or slot address, target, relocation/type evidence, array index/stride if established |
| Evidence | Binary/source/analysis origin, producer version, source site, extraction certainty, required upstream observations |

Inspect instruction P-code for access widths where it can be tied unambiguously
to the reference; otherwise store unknown. An address-taking operation is not a
load from the addressed object. Source field types and Ghidra inferred types are
advisory metadata, not proven PC access widths or ABI contracts.

Extend `tools/reference_features.py` to retain every raw data edge and function
with data uses, even when no named global or string exists. Its v1 adapters remain
available for baseline comparison. Use existing `tools/binary.py` and Android ELF
readers for file translation and relocation evidence; do not duplicate PE/COFF
relocation logic. Normalize imports/PLT/GOT while retaining unresolved cases.

Completion checks: reproduce retained raw edge counts where inputs are identical;
explicitly explain newly discovered edges; retain READ_WRITE; preserve aliases,
interior pointers, unknown widths, disjoint functions and empty-reference nodes.
An export must never invent object sizes from adjacent addresses.

## 3. Implement joint function/data matching

Keep normalization, validation and graph indices in a small new module,
`tools/identification_graph.py`; keep candidate scoring in
`tools/identification.py`. Use the same graph on both sides, with adapters for
legacy exports. No separate matching command or build orchestrator is needed.

First generate data candidates by the set of **confirmed paired function users**.
At least two distinct paired users is the initial broad shortlist rule. Rank by
weighted overlap, rare users, read/write roles, known offsets and distinctive
initializer/table structure. Keep zero-filled objects neutral on initializer
similarity. A separate rare-initializer route may propose singleton-user objects,
but must explain the independent evidence. Multiple instructions in one function
do not count as independent users.

Preserve competing data bases and field-offset interpretations. Co-used globals
with identical user sets must remain tied until roles or other evidence separate
them. Cross-binary sizes and offsets are weak features, not equality requirements.
Pointer tables compare translated target identities and structure, not absolute
pointer bytes. Duplicate/aliased objects must not be forced into a bijection.

Feed **reviewed** data correspondences back into function candidate generation.
Retain the current caller/callee/string channel and add separately attributed data
evidence. Tentative data propagation can be evaluated as an opt-in experiment;
it cannot silently acquire the weight or status of a reviewed anchor. Track the
original verified supports so a cycle of tentative pairs cannot reinforce itself
into apparent independent evidence. Keep at least five alternatives where present.

First review cohorts, using the current map to skip work already completed:

- Customiser state: the `0x00955894` hypothesis and `0x005930c0` function lead;
  check full PC behavior and boundaries beyond the already inspected entry guards.
- Random/seed state: withhold known seed globals and retrieve them from setters,
  getters and reset routines; identical arithmetic with different state is a
  required ambiguity case.
- File/DAT state and cheat state: exercise co-used globals, pointer owners and
  interior fields; treat `0x00950da8` as a site until its owner is established.

Review the top 30 previously unknown data addresses from these and adjacent
families, retain rejected/ambiguous outcomes, and inspect functions newly reached
through verified data. Do not enlarge an acceptance count by guessing extents.
The first release must demonstrate a complete function -> data -> function chain
and pass known-data retrieval tests; the number of accepted symbols is measured,
not promised.

Every review packet contains both addresses, aliases/source location, callers and
callees, data users/roles/sites, distinguishing constants, competing candidates,
proposed identity, and separate ABI/type/extent status. Agent binary review can
promote an identity; `symbols.csv` integration follows the existing mapping rules.
Unknown data extents remain blank; inferred inline helpers receive no fabricated
standalone PC entry. The scorer never writes the accepted CSV automatically.

## 4. Add BSim as an independent candidate source

Use `/opt/ghidra/support/bsim`, the bundled H2 backend and `medium_nosize` template.
Generate signatures for the named Android original and query selected PC
functions. Reuse Ghidra's built-in signature generator/query APIs through a small
`tools/ghidra/ExportBSimCandidates.java` adapter; do not implement new P-code
similarity. Record Ghidra build, signature settings, similarity, significance,
candidate rank and decompiler failures. Normalize result addresses through the
same manifest as the graph exports.

Run a smoke comparison first, then a pilot of 100 known PC functions withheld
from mapping seeds, stratified across at least five families, plus 50 unknown
functions chosen before viewing BSim results. Include graph-disconnected entries
and difficult larger functions; report tiny wrappers separately. Compare graph
alone, BSim alone and their combined shortlist on this exact set. BSim scores
remain separate evidence; do not average incomparable raw scores blindly.

Retain this channel if it adds independently reviewed correct identities or
improves top-five retrieval on the fixed test set without increasing false
promotions under the same review policy. Report ambiguous queries and failures
in the denominator. No bulk import of Android types, signatures or labels into
the PC project. Publish the measurements even if BSim provides no useful gain.

## 5. Recover tables and missing edges

Extend the same exporter/adapters with pointer references from data, validated
MSVC RTTI/vtables and known callback/keyword tables. Start with the established
flow callback table, one AI callback family and the Fade class. Normalize
Itanium/MSVC metadata by verified class/method relationships; slot numbers and
destructor variants are platform-specific.

Enumerate address-taken functions and pointer assignments before proposing
indirect-call targets. Mark possible targets separately from proven targets.
Capture tail jumps only when the control-flow evidence supports a function
transfer; do not classify every external branch as a tail call.

Maintain a gap queue for valid entries absent from the retained PC function
export, ambiguous split bodies, and referenced but unowned data sites. Examine
one bounded family at a time in a working Ghidra project and publish new graph
snapshots with changed-function lists. Do not use sweeping speculative function
creation to make the inventory appear complete. Runtime-library identification
uses the already pinned VC8 COFF libraries as an additional anchor source when
it addresses a concrete gap.

## 6. Persist decisions and expose the remaining work

Store a versioned decision journal next to each analysis workspace, keyed by
binary identities and entity IDs. Distinguish candidate, identity-reviewed,
rejected, ambiguous, inline-only and alias relations; track type/ABI and extent
review independently. Include reason, supporting evidence IDs, map revision and
the decision it supersedes. Persist negative decisions so the same bad pairing
does not return on every run.

On map/export changes, reload raw caches only when their own inputs changed,
invalidate affected derived evidence, and run deterministic scoring from the
remaining valid reviewed anchors. An explicit rejection removes dependent
tentative hypotheses; a changed analysis boundary flags relevant reviews for
revalidation. A changed original hash starts a separate analysis identity.

Outputs: function candidates CSV, data candidates CSV, evidence JSONL, review
Markdown, and a summary of named entries, unresolved alternatives, unowned data
sites, missing/split function boundaries and unknown extents. Report examined
function bytes only where extents are defensible. Keep mapping metrics separate
from `matching.json` reconstruction metrics.

Use Ghidra's existing call-tree, references, listing and decompiler views for
inspection. Provide per-family adjacency exports and evidence packets; a custom
interactive graph viewer is outside the first implementation milestone.

## Evaluation, tests and integration

The implemented public interface stays under `tools/check.py --identify`, with
`--analysis-dir`, `--symbols`, `--joint-graph`, `--reviewed-pairs` and
`--holdout-mode {pair,family,data}`. Keep expensive extraction as an explicit
operation. For a family holdout, supply an explicit selection whose family labels
match the chosen held families:

```sh
python3 tools/check.py --identify \
  --analysis-dir build/analysis/identification/run-001 \
  --symbols config/symbols.csv \
  --joint-graph build/analysis/identification/graph-pair.json \
  --reviewed-pairs build/analysis/identification/reviewed-pairs.jsonl \
  --holdout-selection build/analysis/identification/family-selection.json \
  --holdout --holdout-mode family
```

`graph-pair.json` identifies the two immutable graph manifests. The run snapshots
the supplied symbols and decisions before reading features. Existing invocations
without these options retain their current behavior.

The maintained exporter launcher isolates its Java script and exports both
originals sequentially. Use the pinned configuration described in the runbook
and a fresh output directory:

```sh
python3 tools/reference_graph.py \
  --config build/analysis/identification/current-graphs/export-config.json \
  --output build/analysis/identification/graphs-next
```

The isolated XDG directories avoid writes to the user's normal Ghidra settings.
Validate the exporter and its arguments on a fixture before the original program.

Expand `tools/test_identification.py` with meaningful synthetic cases: anonymous
data retention, code/data distinction, read-write and address-taking semantics,
interior/unknown extents, duplicate aliases, co-used-global ties, zero-filled
ambiguity, wrong-global identical getters, split bodies, inference cycles,
rejections, snapshot isolation and deterministic replay. Test a small Ghidra
export against synthetic fixtures locally; Python validation remains independent
of Ghidra installation and retail inputs in CI.

Use a fixed evaluation manifest containing function-family splits, data-owner
holdouts, negative examples and withheld labels. Remove held labels from **all
map-derived channels**, including accepted-symbol caches, annotated Ghidra names,
source/name crosswalks and decision journals. Keep real binary references and
literal content. Keep original Android names as candidate labels; remove the
withheld cross-platform correspondence, not the available Android inventory.
For a held data owner, withhold its aliases/interior labels too.
Report functions and data separately, with per-family top-one/top-five recall,
proposal precision, unresolved fraction, boundary status, decompiler failures,
reviewed unknown discoveries and review time. Tune on training families, not the
final evaluation set; preserve difficult controls rather than dropping failures.

After each coherent tooling/map implementation batch, run the repository-required
checks in the single aggregate lane:

```sh
make check
make -j"$(nproc)" build check-objects
make -j"$(nproc)" match
```

Inspect any lost exact matches and refresh reports only from a verified frozen
state. Original binaries, extracted bytes, graphs, Ghidra projects and BSim
databases remain ignored. Commit only tooling, synthetic fixtures, documentation,
reviewed symbol rows and required report updates. This planning change itself
does not require builds or modify matching results.

## Ownership and immediate next checkpoint

The main agent owns CLI/run context, schema decisions, scorer integration,
accepted-map changes and aggregate verification. One worker owns the Ghidra
exporter and adapters; one owns data-candidate logic after schema agreement;
one owns the bounded BSim adapter/pilot after normalized exports are available.
Assign test ownership alongside each change. Shared files have one writer; all
research workers hand off proposals rather than concurrently editing the map.

Implement package 1, then freeze the minimal schema. Complete package 2 and the
Customiser/random/file-state slice in package 3. The next checkpoint delivers
reproducible exports, ranked data hypotheses with sites and alternatives, one
verified function-data-function discovery chain, and a benchmark against the
unchanged function-only baseline. That checkpoint is useful independently of
BSim or BinDiff.

No tools were installed and no matching implementation was changed while writing
this plan. Local feasibility and external-tool details are recorded in the
supporting notes under `build/analysis/graph-feasibility/`.
