# GPT-6.1 Sol: whole-project Android → PC implementation plan

Prepared 2026-10-03 from [the tested repository audit](ANDROID_PC_TRANSFER.md).
This is an implementation handoff for the **entire PC reconstruction**. It
supersedes the earlier AI-focused draft. The AI and parser experiments are
worked examples of a general transfer process, not the project's scope.

The user activated indefinite continuation with validated checkpoints every half
hour and direct pushes to main on opensagadev/saga-pc. This remains a
whole-project mandate; a passing transfer checkpoint does not finish the goal.
See config/transfer-execution.md for current released families and retained
provider frontiers.

## Immediate priority after the runtime audit (2026-10-04)

Whole-code fuzzy/exact comparison and runtime progress are separate records.
The user clarified on 2026-10-04 that matching takes priority over running.
Reconstruct startup through the same native identity, ABI, complete-extent and
retail-comparison checks as other families. Runtime tests check correctness;
they do not authorize an alternate entry path or bypassing missing providers.
The similarity percentage does not measure how much of the game works. The source
entry path stops before loading a level; no current live window/input/device
producer or recurring update/render loop establishes a playable application.

Prioritize a connected50–100KB candidate wave around boot to one installed
level, releasing genuine closed subsets continuously:

1. Recover native WinMain/window/configuration/hardware dispatch, input owner
   creation and D3D9 device/reset/presentation. Verify a persistent real window,
   responsive message pump and repeated presented frames.
2. Connect the existing implode decoder to actual compressed DAT read groups,
   with producer-backed buffer bounds. Complete permanent configuration/fixups,
   scene/material/shader/texture ingestion using actual installed assets. Verify
   an installed scene produces nontrivial displayed pixels.
3. Complete LoadPerm1/2, FixUpLevels, area/character and WorldInfo stream/load/
   activation/reset families; preserve true worker behavior. The native synchronous
   loader option is available for bounded tests while the real worker is recovered.
   Verify a populated world/player/camera from real loaders.
4. Reconnect already-real suppliers in UpdateGameObjects and recover native private
   main update/render helpers and recurring transitions. Verify consecutive rendered
   frames with keyboard movement, collision and camera response. Do not remove
   initialization guards or breaks without their actual replacement control flow.

Mapped-only spine anchors already total21,139 PC bytes; private frame candidates
and PC services add further work. Completing already-compared partial parents
earns zero new unique-byte credit. These footprints are not an estimate of all
remaining work. Audio, menus, transitions and level-specific gaps stay explicit.

Keep generated evidence bounded: one active baseline/report pair, shared object
references, small native/ABI receipts and current meaningful fixtures. Do not
retain repeated graph exports, whole SDK copies or every abandoned compiler
snapshot. Target total generated workspace1.5GB; inspect storage at checkpoints.
The removed50GB archive is not required to resume honest source reconstruction.

## 1. Mandate and destination

Reconstruct the complete PC game from source, using the existing Android
reconstruction wherever the PC binary supports that correspondence. Systematically
map the remaining code, transfer reusable source in substantial families, recover
shared PC types and data, implement the actual PC-specific services, connect the
full game lifecycle, and continue toward exact matching of the agreed executable
scope. Include engine, gameplay, resources, UI, rendering, audio, input, platform,
startup, shutdown, static data and compiler/runtime boundaries.

Use GPT-6.1 Sol as lead and for all workers, with as many useful delegated tasks
as necessary and up to eight workers concurrently in this environment. Reuse
workers across waves. This is not a request to stop at 24 AI functions, the
existing mapped backlog, 500 KB, or one playable level. Those are possible
checkpoints inside a whole-project programme.

Run symbol discovery, source transfer, shared contract recovery, PC services,
runtime integration and exactness work concurrently. Establish sufficient PC
identity and local contracts before accepting a transferred family. Do not make
complete whole-program mapping or recursive implementation of every dependency a
prerequisite for compiling the next family.

The retail PC executable remains authoritative. Android provides bodies,
structure, names and hypotheses; it does not establish PC equivalence by itself.
Follow `.agents/skills/saga-pc-decomp/SKILL.md` and `CONTRIBUTING.md`. Use this plan
to set the implementation priority rather than reopening the completed audit or
following an older research queue mechanically.

## 2. Starting position and first session

Audited PC HEAD: `680ca578092fafa147d5887da14104b1cc85f8da`.

| Measure | Audited baseline |
| --- | ---: |
| Original PC `.text` | 3,471,246 bytes |
| Unique original code represented in comparisons | 388,082 bytes |
| Native objdiff denominator, including overlap | 388,137 bytes |
| Whole-PC similarity-weighted coverage | 6.7873055371% |
| Exact original code | 47,697 bytes |
| Exact function / data report rows | 828 / 2,257 |
| Accepted mapped code | 574,658 bytes |
| Mapped, unimplemented code | 186,576 unique bytes |

The Android bank contains 452 standalone matching translation units and 12,090
body definitions including textually included files. Including header definitions
gives 12,842 AST definitions. Both reference binaries are 32-bit x86. This is a
substantial reusable source bank, but Android coverage is not a PC completion
forecast. Even implementing the entire currently mapped backlog would leave most
PC code outside the comparison set.

At implementation start:

1. Inspect current HEAD, status, source and matching report. Preserve all existing
   edits. At audit handoff, `config/identification-decisions.jsonl` had three
   pre-existing additions; the audit and this plan were new files.
2. Save the baseline report and exact-result sets under an ignored execution
   directory. Recompute starting deltas if another task has advanced the project.
3. Check the actual input fingerprints of reused experiments. The Android checkout
   advanced during the audit without changing the captured relevant source; HEAD
   alone is insufficient for determining whether evidence is stale.
4. Read the audit's findings and select independent families across the project.
   Start copying and compiling source immediately from accepted candidates while
   a mapping worker replenishes the next waves. Do not repeat every audit test as
   a prerequisite to a first edit.

The audit already passed `make check` (665 tests, three skips), object checks and
a fresh complete comparison. The initial Wine sandbox issue was resolved and
is not an outstanding code failure. These are baseline results, not validation
of any future implementation.

## 3. Whole-project scope

Maintain a frontier for every subsystem, including those with no accepted
unimplemented symbols yet. The following groups are a coverage checklist, not a
required serial order or an assumption that all Android bodies are reusable.

| Subsystem | Transfer and reconstruction work | Main shared contracts |
| --- | --- | --- |
| Math and geometry | Remaining vector, matrix, quaternion, collision and spatial routines; preserve existing exact bodies | Floating-point operations, constants, alignment, helper context |
| Memory and containers | Allocators, pools, lists, lookup tables, strings and ownership utilities | Allocation sizes, lifetime, strides and private runtime helpers |
| Files, archives and parsers | File abstraction, archive access, decompression, text/binary parsers and serialization | Handle representations, formats, wide characters and PC I/O |
| Resource and asset loading | Textures, materials, models, scenes, animation assets and resource registries | Actual producer layouts, relocations, pointer tables and ownership |
| Animation | Clip evaluation, skeletal animation, blending and game animation dispatch | Skeleton/pose layouts, frame timing and callback conventions |
| Worlds, areas and levels | Loading, unloading, visibility, scene ownership and transitions | Resource relationships, level state and teardown |
| Objects, characters and AI | Object creation, state, movement, collision, AI dispatch and actions | PC object fields, callback packets and physical calling conventions |
| Actions, gizmos, items and effects | Triggers, puzzles, interactions, pickups, effects and associated state machines | Registries, action tables, shared world/object services |
| UI, menus and persistence | HUD, menus, localization, settings, save/load and error flows | Strings, UI state, serialization formats and PC settings/input |
| Audio and music | Shared event/selection logic plus actual PC sound and music providers | PC handles, channel state, streaming and shutdown |
| Rendering | Shared scene/geometry producers, submission, culling and effects; actual PC graphics backend | Vertex/material layouts, device state, buffers and Direct3D interfaces |
| Input and controls | Shared action mapping and game controls; PC keyboard/mouse/controller services | Device interfaces, focus, polling and binding state |
| Startup and lifecycle | Entry, configuration, service initialization, main loop, teardown and recovery | Ordering, global initialization and real service dependencies |
| OS and other platform services | Win32 timing, threads, synchronization, windowing, video and any present network services | Imported APIs, SDK/runtime boundaries and platform-specific ownership |
| Static data and C++/compiler support | Literals, constant tables, vtables, RTTI, constructors, exception/runtime support and original helper boundaries | Exact ownership, initializers, extents, relocations and compiler ABI |

Classify at function or coherent-family level: reusable, structurally reusable
with PC contracts, version-divergent, platform-specific, compiler/library-owned,
or still unknown. A file's category is not proof for every body in it. For
example, Android's empty `NuSound3KillAllAudio` corresponds to real PC stop calls;
copying that no-op would lose required behavior. GLES/JNI and Android device
services similarly cannot stand in for the PC implementation.

## 4. Workstream A: systematic whole-program correspondence

Build on the existing PC graph, Android symbols/source AST, `config/symbols.csv`,
identification journal and compiled-object inventory. Sweep the whole source
bank and PC function inventory to produce a concise frontier by subsystem:

- accepted identity and extent, with corresponding source;
- accepted identity with a specific local ABI/type/data gap;
- source-present candidates needing identity or extent confirmation;
- Android-only, PC-only or changed-version candidates;
- compiler/library, inline/alias or unresolved-boundary cases;
- drafted, compiled, integrated, partial and exact implementations.

Join the existing evidence instead of hand-reviewing known facts again. Keep
candidate hypotheses out of accepted symbol maps. Unmapped candidates should
retain enough reason and location for a worker to investigate them without
repeating a global search. Never count speculative identities as compiled or
exact coverage.

Use the existing graph propagation together with registries, callback tables,
strings, constants, global references, qualified method names, RTTI, callers,
callees and neighbouring family structure. Recover full PC boundaries and the
actual entry convention; a logical Android prototype does not establish the PC
physical ABI of a private optimized helper.

Mapping comes first for a family's accepted implementation. It runs alongside
other families' source work. Start with the 186,576-byte backlog, then continuously
replenish it across all subsystems. The audit already found 178,292 PC bytes with
accepted unimplemented crosswalk pairs and source present; source availability
still does not establish complete readiness.

Require a concrete blocked family or a measured failure on known controls before
adding another general matching algorithm. The existing holdout recovered 140
of 305 hidden known labels with no disagreements among recoveries; that is useful
support for the current pipeline, not a claim of universal unknown-match accuracy.
Avoid repeated whole-graph exports when inputs relevant to the decision have not
changed.

## 5. Workstream B: recover shared PC types and data once

For each active family, collect required member uses, global references, direct
and indirect calls, and the producers and consumers of the relevant objects.
Build the smallest sufficient PC contract for that family and reuse it for all
its bodies. This is where intelligent copying should amortize the work.

Use named fields with verified offsets and appropriate types. Check access width,
signedness, alignment, bit masks, pointer indirection, wide-character width and
calling convention, not just offset assertions. A struct can pass an `offsetof`
check while an access still has the wrong width. Preserve opaque unknown regions
where valid; require full size/stride evidence where allocation, arrays, copies
or serialization depend on it. Do not use a known prefix as an asserted full size.

Keep Android-to-PC member correspondence explicit. Do not assume one constant
layout delta or change packing globally to silence assertions. The bulk trial
found large real layout differences, especially in object/AI structures.

Batch literal and data work by family: strings, constants, local statics,
callback tables, initializer arrays, vtables and resource records. Recover actual
bytes, ownership, extents and relocations. An interior reference is not a new
independent object. Keep a sole production writer for `src/data.cpp`; delegate
nonconflicting recovery work and merge its concrete initializer packets.

As the repository skill permits, compiled data whose original extent is unknown
may remain mapped without a size and receive no coverage or exact-match credit.
Verified bounded accesses can proceed without recovering the whole allocation;
require capacity evidence wherever storage, arrays, copying or serialization
actually depend on it. Do not invent a size or allocation to clear a gate.

Use practical reviewed declarations to let callers compile while providers are
implemented in parallel. A caller need not wait for every descendant's source,
but production still needs real bindings and cannot acquire fabricated providers
or stub return values to satisfy the linker. Hold the affected body when a real
semantic or interface uncertainty remains; do not freeze unrelated families.

## 6. Workstream C: bulk source transfer across families

Operate rolling waves spanning several independent translation units. An initial
wave may aim at roughly 50–100 KB of original PC scope where available, then grow
as the process proves reliable. This is a scheduling target, not an acceptance
threshold or a reason to combine unrelated functions into one compiler context.

For every family:

1. Select bodies using accepted correspondence and capture source fingerprints,
   PC entries/extents, original TU ownership and relevant private helpers.
2. Extract/copy the existing Android bodies with their original expressions,
   branches and helper context. Avoid rewriting readable logic from disassembly
   where source already exists.
3. Apply a small reviewed substitution set for compiler syntax, PC type/field
   names, linkage and bindings. Record actual version/platform semantic changes
   separately from mechanical adaptations.
4. Compile a scratch family early with the real VC8 toolchain. Unresolved external
   declarations are acceptable in isolated experimental objects; fake implemented
   dependencies and accidental production stubs are not.
5. Resolve required data and bindings, then compare complete original and emitted
   functions using existing relocation-aware tooling. Include helper emissions,
   literals, size differences and missing mappings in the result. Keep low scores
   visible instead of dropping inconvenient functions from the denominator.
6. Correct wrong behavior, ABI and layouts immediately. Defer harmless codegen
   tuning when it would block substantial verified source expansion. Similarity
   alone is not proof of correct behavior; there is no arbitrary minimum fuzzy
   score that makes a body valid.
7. Integrate ready subsets into their proper production TUs continuously. Rebuild
   and compare the actual final TU; scratch `.cpp` evidence does not establish
   results in a production `.c` file or with different private helpers.
8. Feed concrete unresolved items to the mapping, contract or backend worker and
   select the next ready family. Preserve a short reason for every held subset.

Automate repetitive extraction, reviewed substitutions, field-use collection,
literal preflight, object selection and batch comparison through the existing
tools. Build only the thin missing automation justified by current transfers.
Do not spend the first implementation phase constructing another comprehensive
framework or universal readiness database.

The experiments support this approach: ten of sixteen sample Android TUs
compiled after shared mechanical adaptations, and substantial bodies transferred
with little logic editing. Their many unresolved mappings and layout issues also
show why compilation must be followed by real PC contract and object checks.

## 7. Workstream D: implement actual PC services concurrently

Keep a dedicated lane for the PC-specific providers that Android cannot supply.
Recover their real interfaces and implement them from the retail PC evidence:
Win32 services, graphics/device management, audio, input, file access, timing,
threads, windowing, video and other present platform services.

Separate shared engine producers from platform consumers at actual interfaces.
For example, shared scene/geometry logic can advance with a reviewed PC graphics
contract while another worker reconstructs device submission. Both sides must
ultimately use the PC layouts, resources and state transitions.

Classify imported APIs and statically linked SDK/CRT/library code explicitly.
Determine which code is reconstructed, reproducibly supplied by a verified
library/toolchain, imported externally, or still unexplained. Do not conceal
unmatched executable code by calling it a platform exclusion or removing it from
metrics. Respect the repository's intended matching scope and document any
justified scope decision.

## 8. Workstream E: assemble and exercise the complete game

Use startup and the main lifecycle as the integration spine, while keeping broad
source transfer parallel. Do not turn one startup dependency chain into the sole
project queue. Link success alone does not demonstrate a working game.

Advance observable runtime milestones:

1. Real process/window/service initialization, configuration and archive access.
2. Real asset producers and resource tables, with loading paths connected beyond
   any current safe early exit.
3. A representative level running through the actual frame loop with rendering,
   input and audio.
4. Gameplay systems operating together: objects, movement, AI, collision, actions,
   effects, scripts and UI.
5. Level transitions, reloads, persistence, settings, error handling and shutdown.
6. The remaining levels, modes, content paths and less common feature combinations
   needed to cover the game rather than one demonstration scene.

Record the concrete scenario and observed result at each stage. Add focused tests
or trace comparisons where they resolve meaningful behavioral uncertainty. Keep
partial implementations and unavailable runtime observations explicit. Never use
an early exit, fabricated asset/provider or omitted required subsystem to claim
full lifecycle success. If the current environment cannot exercise a scenario,
prepare a reproducible run procedure and report it as unverified until executed.

## 9. Workstream F: exact matching throughout, then completion

Track source coverage, functional integration and exact matching separately.
Preserve existing exact bodies and fix semantic discrepancies during expansion;
allocate more workers to exactness as broad reconstruction and integration mature.
Do not require every new body to be exact before the next family can land.

Tune family context before isolated expressions: original TU grouping, private
linkage, helper address-taking, optimized private ABI, C/C++ differences, local
extern declarations, floating-point expressions, constants and initializer
layout. The audit demonstrated that moving a helper or changing its exposure can
turn exact code into a mismatch even when its body is unchanged.

Use full original extents and relocation-aware byte comparisons. No truncation,
padding to fake equality, copying binary code into source, masked mismatches,
fabricated symbol extents, omitted difficult functions or denominator changes.
Data, static initialization, library/compiler support and relevant padding/layout
must remain accounted for according to the repository's matching rules. Fuzzy
scores guide investigation; they cannot override known incorrect semantics.

## 10. Sol team and ownership

Use a lead plus eight concurrent GPT-6.1 Sol workers by default. Reassign them as
bottlenecks change; the following is a balanced starting allocation, not a fixed
subsystem queue. Reuse existing agents and avoid nested agent trees.

| Role | Responsibility |
| --- | --- |
| Lead | Own integration, production shared maps/headers/reports, evidence decisions and checkpoints; implement a family alongside coordination |
| Source workers 1–4 | Four independent bulk-transfer families, normally different production TUs and preferably different subsystems |
| Mapping worker | Whole-program frontier, bounded identity/boundary recovery and supply of the next source waves |
| Contract/data worker | Shared layouts, literal/table initializers and the concrete blockers of active families |
| PC backend worker | Actual PC providers and platform interfaces, prioritizing integration blockers |
| Runtime/review worker | Lifecycle integration, meaningful runtime scenarios, independent evidence checks or exact refinement of a ready family |

Use all slots when work is independent. If source families are ready, give most
capacity to transferring them. If platform or shared contracts become the real
bottleneck, temporarily move workers there. Do not leave source workers idle
behind a single global reviewer or wait for a full wave before integrating a
ready subset.

One writer owns each production TU at a time. Only one writer owns `src/data.cpp`;
the lead coordinates shared headers, symbol maps, journal and aggregate reports.
Workers targeting the same TU prepare separate scratch packets, and its owner
integrates them while retaining original helper context. Assign ownership before
edits and reassign it explicitly. Do not overwrite another worker's changes.

Each handoff includes owned functions and original ranges, source/input hashes,
concrete source changes, header/map/data additions, compiler/object results,
complete comparisons and the specific remaining uncertainties. Keep this compact
and machine-readable where useful; link existing proof rather than rewriting a
long report per function. Shared acceptance is performed once, with independent
checking concentrated on new assumptions and consequential risks.

Bound compiler/job concurrency separately from agent count. Freeze one integrated
checkpoint for aggregate verification while other agents work in isolated scratch
outputs; avoid running competing report writers or heavy graph jobs together.

## 11. Verification, progress and intervention rules

During development, compile affected units and run checks that exercise the actual
change. At coherent production checkpoints, run the required release gates:

```sh
make check
make -j"$(nproc)" build check-objects
make -j"$(nproc)" match
```

Inspect all affected full-function/data results and account for lost exact matches.
Real unresolved definitions, relocation bindings or material semantic gaps in the
selected behavior and local interfaces remain acceptance blockers. This does not
require universal proofs across every input and transitive dependency before
integrating a verified family. A successful link or a scratch comparison does not waive the
production object gate. Keep tool failures distinct from code failures.

Batch shared evidence updates before generating the final matching report to
avoid repeatedly invalidating its input fingerprints. Reuse cached original
binary analysis. Rerun expensive graph work only for relevant changed inputs or
a concrete algorithm question, not automatically after source-only changes.
The already implemented `/GS`, startup and helper fixes must not become repeat
research projects.

At each checkpoint report:

- newly compared unique original PC bytes and total whole-PC coverage;
- whole-PC weighted progress and exact-code/data gains or regressions;
- substantive behavior connected, remaining partials and runtime observations;
- subsystem frontier changes: accepted, integrated, held and next owner;
- the principal current blocker and what the next wave will do about it.

Count unions of original ranges, not emitted bytes, aliases, source lines or
Android report scores. The audit found 83% of newly compared bytes in the first
five broad source releases, with much smaller later releases. That supports
larger source waves; commit counts and timestamps do not establish wasted hours
or a measured causal productivity ratio.

Use +50 KB, 500 KB total, 1 MB total and later coverage thresholds as review
checkpoints, never as the final scope. Replenish identities early enough that the
initial backlog does not exhaust the pipeline. Do not invent throughput or a
completion date from small scratch pilots.

If two bounded work cycles produce no new compared source, identify the specific
contract/platform/tooling obstacle, resolve one representative case, and resume
transfer or move to another ready cohort. Do not reflexively add global research
infrastructure. Do not bypass evidence requirements merely to satisfy a byte
quota. A blocked body or family should have an explicit local disposition, not
stop the whole project.

## 12. Project completion criteria

The project is complete only when the intended reconstruction and matching scope
is satisfied, rather than when an intermediate coverage number looks good:

- Every relevant executable region and required data object has an accepted role,
  source/library/import disposition and ownership. Unknown executable regions and
  unsupported comparisons remain visible until resolved or explicitly justified
  within the project's accepted scope.
- Required game behavior, callback/indirect paths, PC providers, initialization,
  resource lifetimes, transitions and shutdown are implemented and connected.
- A source-built executable exercises the full required lifecycle and breadth of
  game functionality without required stubs, fabricated providers or safe exits.
  Representative runtime success is evidence, not proof of all behavior.
- All required checks pass. Full code/data comparisons and intended layout/ABI
  requirements meet the project's exact-matching acceptance rules. Functional
  reconstruction and exact-match completion are reported separately if reached
  at different times.
- Remaining differences, deliberate exclusions and untested scenarios are stated
  precisely; none are silently removed from accounting or rounded into success.

## 13. Optional first worked packets, not a scope limit

All paths in the table below are beneath the ignored local directory
`build/analysis/transfer-audit-20261003/`. If those files are unavailable in another
checkout, regenerate only the selected evidence packet from current inputs.

| Retained input | Use |
| --- | --- |
| `mapping/accepted-backlog.json` | Whole accepted unimplemented backlog |
| `android-inventory/waves.json`, `families.json`, `qualified-crosswalk.json` | Cross-project source-present families and correspondence |
| `abi-layout/function-contracts.json` | Existing PC field-use evidence; offsets alone do not prove access widths |
| `source-pilot/new-backlog/pc_contract/pilot.cpp`, `saga.h`, `results.json` | Two genuinely new AI callback drafts and their local PC contracts |
| `source-pilot/parser_exact_c/pilot.c`, `source-pilot/report.md` | Four independently checked exact parser candidates and the parser mask finding |
| `mapping/first-wave-24-aisys.json` | A convenient small example family, not the general work programme |
| `root/parser-exact-independent.json`, `root/backlog-independent.json` | Independent byte/relocation/literal checks |
| `bulk-ingest/report.md`, `workflow/report.md` | Bulk compile limits and measured workflow costs |

The AI scratch candidates are `Action_CircleLocator` at `0x456080` (517 original
bytes) and `Action_GoToOriginalPath` at `0x44e580` (724). Together they cover 1,241
previously unimplemented PC bytes at approximately 71.45% weighted similarity.
They still require real production data definitions and final-TU integration.
Apply the proposed header changes as a reviewed diff, never by replacing the
live header with the pilot's copy.

The four parser candidates cover 1,138 already-compared bytes and become exact
in the saved VC8 C experiment: `NuGetChar` (`0x4fc2d0`, 137), `NuGetWChar`
(`0x4fc360`, 41), `NuFParGetLineW` (`0x4fc390`, 544) and `NuFParGetLine`
(`0x4fc5b0`, 416). They improve exactness, not source coverage. The separate
`NuFParGetWord` mask discrepancy must be resolved according to retail behavior;
an incorrect variant scored better, demonstrating the limits of fuzzy scores.

Use these packets to validate the integration process quickly if still current.
At the same time, transfer independent resource/world/object/action or other ready
families and advance PC services. No part of the full project depends on starting
with AI specifically.

## 14. Execution instruction for the implementing Sol session

> Execute `SOL_IMPLEMENTATION_PLAN.md` as the whole-project PC reconstruction
> programme, using GPT-6.1 Sol and as many useful subagents as available. Map the
> remaining cross-platform correspondence, bulk-transfer reusable Android families,
> recover shared PC types/data, implement real Windows backends, connect the full
> game lifecycle, and continue toward exact matching. Work concurrently with clear
> ownership and verified production checkpoints. Reuse the completed audit and
> existing backlog; compile substantial source early and let concrete differences
> drive further research. AI/parser packets are optional first examples, not the
> scope or endpoint. Coverage milestones are checkpoints. Preserve existing user
> edits, exact results and the repository's full-byte verification requirements.

## Runtime checkpoint after connected archive/action/device-lifecycle transfer

The source executable still exits before a level; no playable milestone is
established. This batch completes actual compressed archive reads and existing
resource/gameplay call sites, and introduces the genuine graphics object
lifecycle. The 224 normal-object subsystem checks establish their stated domains;
they do not establish a window, input hardware, graphics device, world or frame
loop. See the dated checkpoint in `config/transfer-execution.md` for measurements.

The next priority is one connected PC startup cohort: genuine window and input
setup, full keyboard state constructor and configuration/file helpers with their
real data producers. Then close graphics device/default-resource creation and
the actual loader/loop dependencies. Keep the current early-exit guard until its
producers exist; no dummy state, fake device or forced frame bypass. Existing
caller/provider restoration takes priority over unused source leaves.
