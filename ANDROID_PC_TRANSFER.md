# Android → PC transfer audit and execution plan

2026-10-03. PC checkpoint: `680ca578092fafa147d5887da14104b1cc85f8da`.
Android source checkpoint: `117c86450763e25391704ec99357b182ae9d0cf9`, including the
separately fingerprinted dirty checkout. This is an analysis and experiment
deliverable, not a production source release. The existing PC journal edits were
preserved. Detailed experiments and reproducible scripts are under
`build/analysis/transfer-audit-20261003/`.

The whole-project implementation handoff is
[SOL_IMPLEMENTATION_PLAN.md](SOL_IMPLEMENTATION_PLAN.md). Its scope extends from
systematic correspondence and bulk source transfer through PC services, the full
game lifecycle and exact matching; the experiments below are starting examples.

## Decision

**The process needs to spend substantially more of its capacity compiling and
transferring existing source. Another broad mapping-tool project is not the next
step.** The repository already has enough accepted identities to attempt a
meaningful increase in source coverage. The earlier broad-transfer plan was
sound in this respect, but later execution returned to small releases and
extensive identification infrastructure.

The intuition that much of this should be copied intelligently is supported by
actual compiler experiments. Once PC interfaces and field layouts are supplied,
large Android bodies can be copied with little or no body editing. Four copied
parser functions compile to exact PC bytes where the current rewrites do not.
There are also real barriers: platform/version changes, field layout, calling
conventions, literals, compiler dialect, and private-helper optimization context.
These need to be solved once for a family, rather than independently for each
function or transitively for the whole game before compiling a draft.

The immediate change is **source-first experiments inside an already mapped
family**: extract bodies, apply reviewed interface substitutions, compile early,
compare complete functions, and let concrete discrepancies determine the next
research task. Preserve production link and full-byte verification requirements.
Keep incomplete experimental objects outside the production source tree.

## What the repository actually contains

The fresh retail comparison reproduced every tracked function result, data
result, object hash and reconstruction-progress figure. The comparison did not
update `matching.json` or the README. The supported PC executable hash was checked
by the existing matcher.

| Measure | Current result | Interpretation |
| --- | ---: | --- |
| Full PC `.text` | 3,471,246 bytes | Fixed denominator |
| Unique compiled/comparison coverage | 388,082 bytes; 11.18% | Actual PC scope represented by compiled functions |
| Native objdiff denominator | 388,137 bytes | Includes 55 overlapping bytes; do not call this unique coverage |
| Whole-PC fuzzy coverage | 6.787306% | About 235,604 similarity-weighted bytes |
| Native similarity within compared functions | 60.715378% | Not whole-game progress |
| Exact PC code | 47,697 bytes; 1.374060% | Full byte equality, union of extents |
| Accepted mapped code | 574,658 bytes | Includes unimplemented source |
| Mapped, unimplemented backlog | 424 rows; 186,576 unique bytes | 421 rows have known sizes |
| Backlog with journal identity/extent/physical-ABI review | 250 rows; 128,683 bytes | Does not establish all fields, data or implementation readiness |
| Current identity/inventory union | 600,126 bytes | Includes existing uncommitted journal records |

The tracked report's identity/inventory union is 597,397 bytes. Its journal hash
predates the user's three existing journal additions; the fresh run incorporates
them. This changes identification accounting, not any compiled-code result.

Even perfecting every currently compared function would cap whole-PC fuzzy
coverage at 11.18%. Conversely, transferring every mapped backlog byte would
still cover only about 16.55% of `.text` before similarity weighting. We need
source expansion now and additional discovery later; neither alone solves the
whole project.

### Android is a substantial source library, not a 69%-complete PC executable

A fresh matching-target AST extraction covered all 452 Android translation units
with zero diagnostics: 12,842 definitions, 54,110 calls and 54,726 global
references. The wider tree has 490 units; the other 38 are outside the matching
target as standalone units, although four of those files are textually included
by target owners and contribute 261 definitions. Including those gives 12,090
target-file bodies and 752 header definitions. The source inventory found 189
bodies with explicit stub/unimplemented markers,
484 empty bodies and 1,138 return-only bodies. Some small/empty bodies are genuine
no-ops; none of these counts proves completeness or incompleteness by itself.

Android's captured report is 68.82301% fuzzy over 4,722,419 function-code bytes,
with 526,280 bytes / 11.144289% credited as matched. The installed Android
reporter awards that credit at a score of 100; it does not independently apply
this PC project's complete-byte/extent equality test. Calling those Android
bytes proven exact would be wrong. Its source layout, compiler output and
denominator differ from PC. The old strategy AST
was stale across 233 source/build files. This audit extracted a new graph rather
than treating old field/dependency inventories as current. Another Android chat
advanced HEAD during this audit; the final check still matched all 452 extractor
fingerprints and all observed source/include/build/report content hashes. Use
the captured content hashes, not a moving branch name, to reproduce the evidence.

The Android binary provenance discrepancy was checked: the documented APK's x86
library and the local rewritten ELF have identical allocated section payloads,
including code, data and relocations. Nonallocated metadata differs, including
removal of twenty unnamed section symbols. Function/data symbol identities agree.
Keep the actual ELF hash pinned for each binary analysis; do not interchange
whole-file hashes just because executable contents agree.

Both reference binaries are 32-bit little-endian x86. The central problem is not
an ARM-to-x86 instruction translation. It is recovering the PC version's
interfaces and behavior while preserving reusable source and VC8 code generation.

## Where the work went

The history audit examined all 64 report-changing checkpoints after `ba24830`.

| Checkpoint category | Count | New unique compared bytes |
| --- | ---: | ---: |
| Source releases | 24 | 209,883 |
| Tooling plus inventory | 24 | Combined with row below: 1 |
| Inventory/documentation | 16 | The one byte is an extent correction |

The first five substantial source releases added **174,608 bytes, 83.19% of all
new scope**. The median source release added only 1,836 bytes; after the first
five, the remaining nineteen had a 976-byte median. The six latest
source releases added 16,760 bytes together. The forty non-source checkpoints
added 116,210 mapped-code bytes but essentially no compiled scope. Some of that
mapping is valuable; its return now depends on implementing it.

Changed-line activity reinforces the imbalance: game source added 27,740 lines,
tools/tests 17,407, and documentation/plans 7,828. Report and badge churn added
another 83,985 lines and removed 39,146; that is mechanical output, not additional
reconstruction. These are activity and output measurements, **not active hours**.
Commit timestamps do not measure effort, and easier early families or larger
batching could explain part of the difference. No honest speedup or completion
date can be inferred from these records alone.

The observable imbalance is that researched identities and dependencies are
accumulating faster than source. My diagnosis is that the handoff into source
implementation has become too restrictive and fragmented; this is an inference
from the backlog and experiments, not a measured causal decomposition of effort.
The current joint mapper is already at a fixed
point on unchanged inputs. Repeating normal graph controls and expanding its
diagnostics at every small source checkpoint has not supplied corresponding
source output. The existing skill already says to use minimal field cohorts and
not wait for full type/backend reconstruction; execution needs to follow that.

### Verification cost is real, but not the entire explanation

Fresh measured commands:

| Command | Wall time | Result |
| --- | ---: | --- |
| `make check` outside the Wine-restricted sandbox | 231.53 s | 665 tests pass, 3 skips; LLVM 18 formatting and static analysis pass |
| `make -j16 build check-objects` | 14.27 s | Current objects/executable accepted; not a clean rebuild benchmark |
| `python3 tools/match.py` | 27.68 s | Full comparison reproduced tracked code/data results |

The initial sandbox check failed seven compiler fixtures because Wine could not
bind its local socket. The permitted rerun passed. Small compiler experiments ran
concurrently, so these are observed command durations, not isolated benchmark
claims. Production lint checked 128 units and formatting checked 130 source/header
files. The three skips are described in the retained log.

A concrete matcher inefficiency was reproduced: looking up all 5,215 map rows
rescans every object's definitions. One hot-cache pass took 17.307 seconds.
A scratch name index produced the **same ordered definitions for every query**
in roughly 0.006 seconds including construction, preserving duplicate labels.
This is a small, worthwhile fix, not a claim of a comparable end-to-end speedup.
Object parsing itself took about 0.056 seconds; toolchain-tree hashing about
0.126 seconds. Replacing the compiler or toolchain verifier is not justified.

Retained wave-52 command durations total about 581 seconds across build, checks,
per-function diffs, two full matches and graph control, for 3,561 newly compared
bytes. Some timings include lock waits; they are not additive active-work hours.
Verification should be batched and triggered by relevant input changes. Its
fixed overhead should not be paid repeatedly for single tiny additions.

## Experiments: what copying does and does not solve

### Whole source ingestion

Sixteen representative **whole Android translation units** were compiled,
preserving unresolved external references in scratch COFF objects. Raw source
compiled in only one case; the other fifteen first stopped at VC8's missing
`stdint.h`. Shared mechanical changes for fixed-width types/assertion spelling,
GNU attributes, scalar `return {}`, `override`, and callback alias syntax allowed
**ten of sixteen** to compile. They emitted 203 function and sixty data
definitions from 83,412 source bytes, including compiler-generated methods.
One compiled APK unit emitted no functions; a compiled-file count alone misleads.

Enabling the copied Android layout assertions reduces this to nine of sixteen.
The AI unit reports at least one hundred failed layout checks. A direct COFF
initializer probe measures APIOBJECT's `identity_mask` at `0x1e8` under default
VC8 packing versus Android's asserted `0x1e4`, and `exclusion_mask` at `0x2a0`
versus `0x298`. `/Zp4` restores many Android offsets, but establishes nothing
about the PC version's actual layout or original packing settings. Do not
globally change packing or disable assertions to make an import appear sound.

The ten draft objects fail the unchanged production checker on 185 issues:
134 missing definition mappings and 51 missing relocation bindings. The strict
gate correctly quarantines these drafts. Some broader units remain blocked by
Android/POSIX header coupling; no fake platform services were substituted.

These are shared compiler/include compatibility tasks. Device APIs, aggregate
layouts and changed algorithms require separate PC evidence. An object-only
draft can retain a real unresolved call without inventing a callee body or
claiming reconstruction coverage. This experiment proves substantial bulk
compilation is possible, not that those compiled functions are correct PC ports.

### Complete-body comparison controls: 8,058 existing PC bytes

These controls deliberately use already established PC interfaces. They measure
source reuse and representation, **not previously unimplemented byte throughput**.
All comparisons use the existing relocation engine, complete accepted PC extents
and pinned VC8/objdiff tools. Variants overlap and must not be summed.

| Cohort | PC bytes | Adaptation and result |
| --- | ---: | --- |
| `NuBridgeUpdate` | 3,073 | One approved record alias; unchanged Android body. 88.33% fuzzy with original inline abs, versus current 85.63%. Extra emitted helpers block production acceptance of that variant. |
| Twelve quaternion functions | 3,078 | Existing PC types/trig contracts plus one checked arccos rewrite. 91.68% weighted similarity, 1,762 exact bytes; some siblings regress. |
| Six parser functions | 1,907 | Thirteen approved field/type renames. Four full bodies become exact; complete-object checks pass. |

The four parser exact candidates are `NuGetChar` (137 bytes), `NuGetWChar`
(41), `NuFParGetLineW` (544) and `NuFParGetLine` (416): **1,138 bytes** total.
Root independently relocated and compared every byte. They also remain exact
under the production C front end. These are scratch integration candidates;
the production report was not changed and they add no new compared scope.

There is a stronger lesson than a score improvement. Current PC
`NuFParGetWord` omits the initial line-position mask that exists in both retail
PC and Android. A deliberately incorrect unmasked control scores higher than
the masked direct copy. Restoring the actual `NuStrChr` body beside the caller
improves the masked candidate from 5.78% to 44.99%, but changes its sibling's
score too. **Fuzzy score alone can reward a wrong implementation.** Preserve
the source's operations; investigate actual PC differences before rewriting for
the score. Correct the verified mask discrepancy during parser integration.

### A genuinely unimplemented source slice: 1,241 PC bytes

To avoid relying only on already solved positive controls, a second experiment
copied two actions from the mapped-but-unimplemented backlog:

| Function | Full original extent | Scratch compiled extent | Similarity |
| --- | ---: | ---: | ---: |
| `Action_CircleLocator` | 517 bytes | 491 bytes | 70.07051% |
| `Action_GoToOriginalPath` | 724 bytes | 690 bytes | 72.43636% |

The scratch objects pass the existing object/relocation checker using a frozen
map plus four literal aliases verified from actual PC operands and string bytes.
Root independently reproduced their complete relocated streams and verified the
six literal addresses occur in actual PC operands and contain the expected bytes.
All real call providers and the global callback already have mappings. The
drafts preserve genuine actions and do not invent successful dependency stubs.

This was not merely a rename. It needed a named owner-pointer view with an
offset assertion, PC creature/position fields, and a real flag translation:
Android uses mask `0x02` in a byte at packet `+0x1e5`; retail PC uses mask `0x200` in
the existing DWORD at `+0x1e8`. PC disassembly, not compilation success, supplied
that correction. That is precisely the kind of focused work that belongs between
copying a body and accepting it.

The resulting 886.70 similarity-weighted scratch bytes establish a small
new-backlog experiment, not production coverage or complete semantic proof.
Shared field/constant ownership, actual callback/TU integration, linking and full
production comparisons remain release work. A two-function sample cannot
establish the yield or effort of a 50–100 KB wave.

### Compiler/TU hypotheses were retested

The previously recommended `/GS` change, security-cookie bindings, static startup
helper and full `GAMEOBJECT` stride are already implemented. Repeating those
recommendations as future work would be stale advice.

Fresh controls reproduce the 59-byte startup helper exactly when static in its
caller unit. Moving it into an external unit gives 66.75% similarity. The
86-byte AI path-reset helper is exact in its current static closure; the external
variant has the same length and 96.33% similarity but differs at sixty byte
positions. Taking a static helper's address also suppresses the private-ABI
optimization in a fixture. A global prototype is not harmless bookkeeping.

`/GS` independently reproduces the exact 75-byte file-copy and 130-byte bad-word
filter controls. A large camera body remains 27.20% with either cookie flag,
so flags alone do not repair source representation. Of 43 GameAnim functions,
42 compare byte-identically under C and C++; the remaining C++ copy fails
relocation because a local dependency declaration gains C++ mangling.

A field-use extraction pilot over four current units found 200 functions and
2,690 distinct per-function member requirements, with no unresolved named-member
offsets in that sample. The shared header already contains 1,883 `offsetof` and
215 `sizeof` assertions. Reuse this investment. Extracting an offset from an
existing type is not proof that the Android field belongs at that PC offset;
the PC access evidence still supplies that correspondence.

## Mapping first: use a broad crosswalk, then transfer continuously

The current normal run has 1,574 accepted cross-platform seeds and 1,299 inferred
function pairs. Names, source owners and field requirements can be joined to
those results, but the production joint output does not currently carry that
source-readiness information. This missing handoff is more immediately useful
to fix than adding another global similarity feature.

A fresh family-disjoint test withheld 305 known movement/GameObject/AI labels
and their co-used data anchors. It recovered 140 labels by propagation, all
consistent with the hidden known labels; recovery differed substantially by
family. It reproduced the normal candidate/data/state artifacts unchanged.
This validates a useful retrieval aid, **not unknown-candidate precision**.
Do not weaken tie margins or automatically promote inferred pairs into the CSV.

For each family, retain five separate facts:

1. **Identity and extent:** original PC entry, complete ranges, corresponding
   Android symbol, positive evidence and rejected rivals.
2. **Physical interface:** arguments, return use, stack/register convention and
   private/static context needed by this caller/body.
3. **Access contract:** just the fields, widths, strides, constants and globals
   the selected bodies use, with PC evidence and assertions.
4. **Draft state:** copied/adapted source, actual compiler result, emitted
   definitions and unbound relocations.
5. **Release state:** complete comparison, semantic discrepancies, dependency
   implementations, link and aggregate-check status.

Do not conflate an unknown historical typedef or full allocation layout with
an unusable local physical contract. Only ten backlog rows mark all types
reviewed, but treating that as a hard implementation gate would be incorrect.
Conversely, an ABI-reviewed flag does not authorize copying an entire Android
structure or treating indirect calls as resolved.

Ordinary drafts do not need universal proofs of all child functions' x87,
exception, callback or malformed-input behavior. Investigate such effects when
they affect the actual local contract. A documented terrain mapping previously
required 37 provider contracts and 82 target bindings; such work can matter to
the eventual implementation, but should not be a universal prerequisite for
recording identity or obtaining first compiler feedback.

## Execution plan: turn the existing backlog into code

### 1. Start with an explicitly enumerated, previously unimplemented slice

The audit generated 424 local dependency packets. A conservative filter finds
185 bodies / 38,164 PC bytes with reviewed identity/extent/ABI and no unmapped
exported direct callees. AI actions account for 111 bodies / 26,777 bytes.
Within AI, **87 bodies / 18,919 bytes have all exported direct callees already
implemented**. The largest 24 cover **11,145 bytes** and form the first proposed
source session. The explicit task list is
`build/analysis/transfer-audit-20261003/mapping/first-wave-24-aisys.json`.
Seven bodies retain indirect-interface holds and four have source-only same-TU
helper questions. Fields, data and those calls still require local review;
these are filtered candidates for implementation, not a ready-to-merge claim.

Examples include `Action_GoToOriginalPath` (`0x0044e580`, 724 bytes) and
`Action_CircleLocator` (`0x00456080`, 517 bytes), whose observed direct providers
already exist. Use the saved task list, PC graph and current source together;
the Android body may contain version-specific behavior even at a correct name.

The immediate implementation work is to integrate the two demonstrated AI
callbacks with their field/literal proposals, then continue the rest of this
named slice. Do not repeat their completed first-draft experiment. The four
parser exact candidates and mask correction are a separate small integration
opportunity; they must not substitute for source expansion. No additional global
graph feature or readiness database is a prerequisite for starting these bodies.

### 2. Prepare several larger, independent families concurrently

| Accepted backlog family | Existing unimplemented PC bytes | Main shared work |
| --- | ---: | --- |
| AI actions/system | 37,910 | Action packet/process/locator fields, callback tables and literal bindings |
| Gizmo action dispatch | 10,397 | Locator/trigger/obstacle records and genuine callbacks |
| API object helpers | 8,580 | Object prefixes, producer initialization and private helper context |
| Character movement | 13,975 | Contact/path/transform field cohorts and version differences |
| World/area/level | 17,085 | Parser tables, resource producers and actual file/data dependencies |
| Terrain | 13,937 | Geometry/query contracts; more expensive follow-on family |

The first five rows provide roughly **88 KB of already mapped backlog**. Prepare
the queue together, then release confirmed subsets continuously. Do not spend
another mapping wave rediscovering these addresses. The current Android census
also supplies two independently grouped preparation queues of 63,356 and 71,520
PC bytes; these overlap this table and are alternatives, not additive opportunity.

Use several source owners across these families, one shared layout/data owner,
one bounded discovery/review owner and root integration. Keep one writer per
production TU. Multiple workers can prepare disjoint scratch packets for a large
AI unit, but one owner assembles and compares its final TU. Keep most capacity
on bodies and their concrete blockers; do not let eight workers all become
general-purpose mapping researchers.

### 3. Make mechanical transfer the default draft operation

- Extract full Android bodies using their actual build language and source
  fingerprints. Preserve expressions, control flow, signedness and local-variable
  structure until PC evidence requires a change.
- Apply explicit, reviewed type/member/symbol substitutions to a scratch cohort.
  Reuse asserted PC fields. Keep unknown fields opaque; never import Android
  layouts wholesale or replace field assertions with byte-offset casts.
- Compile with real VC8 immediately. Preserve unresolved external declarations
  in experimental objects rather than fabricating successful dependency bodies.
- Compare complete bodies wherever every relocation has a verified binding.
  An unavailable binding stops that comparison, not the entire family's drafting.
  Reuse `tools/binary.py`; no new relocation engine or host-game target.
- Preserve real helper/caller context. Check all emitted definitions, including
  unwanted inline helpers and literals. Do not invent original addresses for
  reconstruction-only code just to satisfy object checks.
- Integrate verified dependency closures into production, retaining fatal link
  errors, explicit partial actions and the existing complete comparison rules.

No new orchestration framework is required for the first source slice. The
scratch experiments already demonstrate the necessary compiler/comparison path.
A small object-selection/batch-output option in the existing matcher can follow
if repeated manual selection becomes a measured obstacle.

### 4. Shorten the loop without weakening acceptance

During drafting, run the affected compiler/object/comparison checks. At a
coherent production checkpoint, run the required full check, build/link/object
validation and full match once, freeze the report, and inspect exact regressions.
Group related header edits so that every individual field does not force a full
rebuild. Existing tests still run at release; a small source change does not
justify writing new scaffolding that only mirrors its implementation.

Run global graph replay when its relevant evidence or algorithm changes, or
when selecting the next discovery queue. An ordinary source-body change with
unchanged graph inputs should not trigger another fixed-point experiment.
Keep a full unchanged-input control for an actual mapping algorithm change.
Land the proven definition-name index as a bounded tool improvement if desired;
do not turn it into another large matcher rewrite.

### 5. Use output gates that prevent drift back into research-only work

The following are proposed management limits, not measured throughput promises:

- First source session: integrate the demonstrated new callbacks and continue
  the 11,145-byte AI packet; obtain new complete comparisons or enumerate specific
  blockers. Log active source/research/tool
  time separately from waiting and elapsed wall time.
- Across the next several sessions: target **50 KB of new unique compared code**,
  drawing from several families. If two bounded source sessions add no new
  comparisons, change cohort or unblock named fields; do not expand general
  research tooling without a concrete byte-unlocking argument.
- Maintain 50–100 KB of prepared family scope, with only a bounded amount of
  work waiting on root/header integration. A blocked major body should not hold
  every independent sibling or other subsystem.
- For repeated zero-score large bodies, inspect representative identity, source
  omissions, fields and TU context once; assign a bounded representation task
  and continue source elsewhere. Do not chase register ordering across the batch.
- Measure new unique PC bytes, net similarity-weighted bytes, exact regressions,
  semantic corrections and remaining partial actions. Report inventory-only
  work separately. A merge, test pass or candidate count is not source progress.

The next scope milestone is **500,000 compared PC bytes**, requiring 111,918 new
bytes; then 1,000,000. A hypothetical 100 KB new family at 70% similarity adds
about 2.02 whole-PC percentage points. That is arithmetic, not a forecast.
Reaching 40% or 50% whole-PC fuzzy at an 80% average would require about 1.74 MB
or 2.17 MB compared in total. The current backlog is sufficient to test a much
better workflow; it cannot by itself establish those larger outcomes.

## Evidence and limits

The detailed local reports are `workflow/report.md`, `mapping/report.md`,
`source-pilot/report.md`, `abi-layout/report.md`, `android-inventory/report.md`,
`bulk-ingest/report.md` and `critique/report.md` under the audit directory.
Machine-readable backlog/contract packets, source inventories, compiler commands,
object checks, field manifests, holdout results and per-checkpoint histories are
beside them. Root validation logs and independent exact-parser comparisons are
in `root/`.

These experiments do not prove the game runs, establish semantic equivalence
from fuzzy scores, validate every candidate identity, or measure a large
previously unimplemented transfer wave's yield. Scratch success is labeled
separately from production acceptance. The actionable result is a tested source
reuse method, a concrete existing backlog, identified unnecessary gate coupling,
and an implementation-first experiment that can now succeed or fail visibly.
