# Identification tools operator runbook

Checked setup and continuation: 2026-10-01. Run commands from the repository root. These tools
produce research evidence; they never add accepted rows to `config/symbols.csv`
or establish reconstruction matching coverage. The identity, ABI/type and
complete-extent rules in [identification.md](identification.md) still apply.

Identification jobs run sequentially under a repository lock and a default
**1 GiB address-space limit**. `--memory-limit-mb` changes the limit explicitly;
The current pipeline fits the default; an explicit budget up to about 4 GiB is
available when a larger reviewed batch needs it. Measure peak RSS and keep heavy
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

`tools/repair_graph.py` makes the independent copy, pins the reviewed packet and
executed script, checks a fresh repair report, and verifies the saved function
extents in the new export before publishing a graph pair. Use a fresh output
under `build/analysis`; the packet schema and required range hashes are described
in `tools/ghidra/RepairReviewedFunctions.java`. Neither script clears conflicting
data or code, infers signatures, or analyzes the whole program.

```sh
python3 tools/repair_graph.py \
  --config build/analysis/identification/current-graphs/export-config.json \
  --packet build/analysis/identification/continuation/callback-repair-packet.json \
  --reuse-pair build/analysis/identification/current-graphs/graph-pair.json \
  --output build/analysis/identification/callback-repaired-graphs
```

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

Optional joint inputs are `--reviewed-pairs JOURNAL.jsonl`, repeated
`--similarity-candidates CANDIDATES.json`, and `--holdout` with
`--holdout-mode pair`, `family` or `data`. Use `--holdout-selection FILE` for a
frozen explicit evaluation selection; keep its labels out of training inputs.
BSim and BinDiff scores retain their producer meaning rather than becoming
automatic identity approvals. Data candidates use distinct confirmed paired
function users; repeated accesses in one function do not count as independent
users. Tied co-used globals and competing owner interpretations require review.

Each run freezes the symbol map, optional decisions and source overlay, and
fingerprints raw inputs. It publishes a staged directory atomically only after
validation. `run-manifest.json` has schema `identification-run-v1` and status
`complete`; it fingerprints every published output. A staging directory or a
modified output is not a reusable completed run. Concurrent runs against the
same output directory are rejected. Prefer named run directories to preserve
comparisons.

Joint research outputs include `hypotheses.csv` (functions),
`function-candidates.jsonl` (full function evidence), `data-candidates.csv`,
`evidence.jsonl`, `review.md`, `inventory.json`,
`state.json`, and `delta.json`; `summary.json` and any `holdout.json` report
this run's actual counts. These are separate from
`matching.json` and reconstruction coverage.

Persistent function state contains compact summaries and an exact digest of each
full evidence row. Replay streams legacy state row by row, so an old 100 MiB
state does not allocate another complete evidence tree. Data rows retain full
evidence with a 16 MiB aggregate canonical payload limit; individual rows are
limited to 1 MiB. Oversized inputs fail explicitly. Tentative state is used only
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

The latest checkpoint `make -j2 check` passed 297 tests (three optional Ghidra fixture skips), source
formatting and Clang-tidy. The object check passed 1,638 function and 2,604 data
definitions across 123 objects. Final compilation/matching checks use two Make
workers to keep resource use bounded.
Retail matching preserves all 801 exact functions and every pre-existing
comparison row. Fuzzy code coverage stays at 6.212%; only mapping coverage rises.
The first pushed tool snapshot exposed a test relying on a local ignored file;
the fixture now isolates both repository roots and its job lock, so the required
legacy-default inputs are entirely synthetic.
