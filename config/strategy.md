# PC decompilation strategy review

Research date: 2026-09-30. Production checkpoint: `ba24830e42d83d8d1106794a97c914bd19ce85d7`.

This review pauses implementation and evaluates how to transfer much more of the Android reconstruction into the matching PC project. It combines eight parallel subsystem investigations, two deeper compiler/strategy reviews, and independent root analyses and reproductions. It changes no compiler setting, accepted symbol, game source, matching report, or badge. Scratch objects, executable experiments, exports and detailed reports remain under ignored `build/analysis/strategy/`; their results are not additional project matches.

## Decision

The current approach is too incremental for the stated objective. The main constraint is the amount of original PC code entering comparison, followed by the quality of the transferred representation. There is substantial reusable Android logic, but our process spends too much effort resolving and integrating one function at a time, and sometimes changes the compilation-unit structure that MSVC needs to reproduce it.

The recommended approach is **bulk transfer of reviewed source families**, with a continuous mapping/layout queue feeding several independent source owners. Group functions by likely original compilation unit, private helper linkage, shared PC field cohorts and data dependencies. Preserve that organization during transfer. Use NuMain to discover relationships and test integration, rather than making its entire platform-dependent closure the order of work.

Three changes have direct experimental support:

1. Preserve local helper/caller compilation units. A real startup helper previously treated as an irreproducible private ABI became an exact 59-byte match in ordinary VC8 C when made `static` beside its caller. No assembly or `/GL` was needed.
2. Correct the `/GS-` baseline after integrating real cookie bindings and running production checks. An isolated build of all 93 current units with `/GS` added two exact functions and lost none. This corrects a systematic mismatch, but its measured coverage gain is small.
3. Refresh and strengthen the mapping pipeline, then review its proposals by source family. A clean, correctly configured Android source graph produces 572 stronger hypotheses covering approximately 404 KB of candidate PC spans. Those are a practical research queue, not accepted mappings or transferred bytes.

A first broad review wave is already identifiable: movement, GameObject and AI system source families contain **81 stronger hypotheses, 66 with candidate spans totaling 89,407 unique PC bytes, and 15 with unresolved boundaries**. None of those sized candidate entry addresses is already accepted in the current map. This is a credible wave size to investigate; it is not a promise that all 89 KB is portable or will score well.

## 1. What the numbers actually say

### Current PC baseline

The configured GOG executable has SHA-256 `239f6c09b73e5764ced7469f158b621ff34c9984b5b0a850fe77248add259113`. The pinned compiler is VC8 RTM `14.00.50727.42`; objdiff is `3.8.1`. The source-only Windows executable links using the original compiler/linker and static CRT. Successful linking does not establish a reconstructed or playable game.

| Measure | Current value | Meaning |
| --- | ---: | --- |
| Original `.text` | 3,471,246 bytes | Fixed whole-code denominator |
| Known mapped reconstruction code | 243,793 bytes, about 7.02% of `.text` | Inventory, including code without an implementation |
| Native objdiff comparison denominator | 178,253 bytes | Original extents supplied to the CLI, including duplicate copies/aliases |
| Unique compared original code | 178,198 bytes, 5.134% of `.text` | Deduplicated address coverage of current compiled functions |
| Native objdiff fuzzy similarity | 54.914806% | Similarity within the compared subset |
| Whole-code fuzzy coverage | **2.8183615%** | Similarity-weighted original bytes divided by all `.text` |
| Exact original code | 31,483 bytes, **0.9069654%** | Full equal extents and bytes, deduplicated |
| Known mapped image space | 695,188 / 40,067,072 bytes, **1.7350607%** | Code and data inventory against the entire virtual image |
| Current source | 93 units: 85 C, 8 C++ | 1,030 function definitions in the current units |
| Mapped functions in the report | 1,213 rows, 1,212 distinct entries | Includes unimplemented functions and aliases |

Even making every currently compared byte exact would cap whole-code fuzzy coverage at **5.134%**. More instruction tuning within that subset cannot solve the overall coverage problem.

The current whole-code fuzzy numerator is about **97,832 similarity-weighted bytes**. A newly compared 10 KB family scoring 80% adds about 0.230 percentage points. A 100 KB family scoring 70% adds about 2.02 points, provided its intervals are genuinely new and disjoint. These are arithmetic examples, not acceptance or similarity forecasts.

The mapped-but-unimplemented function backlog is approximately 66 KB. It is useful immediate work, but insufficient for a large percentage by itself. The accepted map must grow alongside source transfer.

### Why mapping looks particularly low

The PC image contains these virtual section extents:

| Section | Virtual bytes | Known CSV extent union | Coverage within that section |
| --- | ---: | ---: | ---: |
| `.text` | 3,471,246 | 244,987 | 7.058% |
| `.rdata` | 646,766 | 5,841 | 0.903% |
| `.data` | 35,372,200 | 444,360 | 1.256% |
| `.rsrc` | 568,196 | 0 | 0% |

The raw CSV `.text` union includes import thunks; the reconstruction-code measure excludes them, explaining its slightly smaller 243,793-byte value. The very large zero-filled `.data` region dominates image mapping. Recovering a large buffer can move the mapping badge significantly without transferring any code. Conversely, a productive code batch can barely move the image badge. Keep that badge, but add section-level/code inventory figures to contributor reports rather than interpreting image mapping as the primary decompilation metric.

### Android's roughly 65% is a useful source signal, not a PC forecast

The Android report compares the original ELF against a whole linked reconstructed library. At the initial investigation snapshot it reported approximately **65.7–65.8% native objdiff fuzzy similarity over 4,722,419 function bytes and 13,459 report functions**. Exactness was much lower: approximately 522,510 code bytes, or 11.06% of that reported code denominator. Android's quoted progress is not “65% exact source” and is not directly comparable to PC whole-`.text` fuzzy coverage.

The Android source graph was frozen at commit `3ade622b189cc1592fdda31f16f2ef9bfbd04545`, generated at `2026-09-30T11:37:44Z`. Another Android team continued working during this review; earlier notes reference `c037a40e`, and later checkout edits are outside this frozen graph. Use the recorded graph fingerprint, not whichever HEAD happens to be current later.

The original Android ELF hash remains `30d3fe55503416139a1d0f9b25fc73924aab4b12dd731aafc08b4a1c9bd950d5`. Both originals are little-endian x86 with 32-bit pointers. That removes a major architecture barrier, but not compiler, C++ ABI, platform, source-version or layout differences.

### What a 40–50% PC target requires

At 80% mean similarity across compared PC code:

| Whole-PC fuzzy target | Required compared original bytes | Additional bytes beyond today |
| --- | ---: | ---: |
| 40% | about 1.736 MB | about 1.557 MB |
| 50% | about 2.170 MB | about 1.991 MB |
| 65% | about 2.820 MB | about 2.642 MB |

At 90% average, the additional requirements are about 1.365 MB and 1.750 MB for 40% and 50%. At today's roughly 55% average, they are about 2.346 MB and 2.978 MB. A 65% whole-PC target would be impossible at that average even if all `.text` were compared.

These scenarios do not prove that 40–50% is unattainable. They show why copying a handful of helpers per checkpoint cannot get there, and why the Android percentage alone does not establish an achievable PC percentage.

## 2. Program structure and source availability

The retained Ghidra export has **7,839 PC function candidates** and approximately **800 multi-range bodies**. The Android ELF graph has **13,043 distinct function ranges**. These are analysis inventories, not an authoritative census or a one-to-one correspondence: aliases, inlining, shared tails, compiler helpers, mobile-only code and library code differ.

The root direct-call graph contains approximately 21,985 distinct caller/callee edges. Starting at NuMain reaches **3,727 PC entries**, of which 847 are mapped and 684 implemented in the graph snapshot. That is a useful discovery frontier, but it misses indirect callbacks, virtual calls and uncalled routines. Its nominal body spans must not become matching extents automatically.

Strongly connected components are useful for detecting cycles, but do not generate useful bulk work by themselves. The full PC direct graph has **7,739 SCCs, only 16 nontrivial**. Many of the largest are runtime/library code. Most gameplay nodes are singletons. Group by original TU, private calls, shared layouts and semantic subsystem; use SCCs only to avoid splitting a real cycle.

The fresh Android graph covers the **452 translation units named by its Bazel matching manifest**:

- 452 clean units, zero diagnostics;
- 12,833 source definitions;
- 53,047 calls;
- 53,359 global references;
- 7,071 string literals;
- 207 unresolved call records;
- graph SHA-256 `8afbfdc3837deed0d6a2c9bb39897d615a7b3e120685ff1f5fa606a8d13f88b6`.

Clang 22 was used for research AST extraction with the i686 Android target and NDK r8e headers. This is source evidence, not a replacement for Android's actual GCC code generation. All per-TU source/header fingerprints were current at the end of extraction.

The old graph was significantly stale: 48 source/header paths changed relative to its snapshot, including 20 headers. It also treated every `.c` file as C, while the Android Bazel configuration compiles **18 `.c` units as C++**. Correcting those modes and using Clang's own compatible intrinsic headers made the full target extraction clean. File extensions alone cannot define the historical language mode.

The source pool is substantial in exactly the areas worth investigating. Android's per-family report associates about 2.550 MB of original function code with `legoapi`, 864 KB with `nu2api`, and 555 KB with `gameapi`. Their respective fuzzy averages are approximately 61.0%, 75.9% and 72.4%. The `numath` subfamily accounts for about 101 KB at 99.9% Android fuzzy, and `nufile` about 50 KB at 96.1%. Those are strong source priorities but **Android bytes**, not PC opportunity estimates. Gameplay render, world, character and object/item families each have several hundred KB of Android function bodies.

The 490-TU wider source tree includes 38 units outside the Android matching target, mostly host/harness code. The target also includes mobile input and Android-specific rendering/device units that cannot become PC implementations. A textual scan found STUBBED markers in 62 target units; some denote known no-ops or diagnostics. Inspect the actual body rather than inferring completeness from its file suffix or Android similarity.

There is therefore enough source to support bulk investigation. What remains unknown is the amount of PC code with a truly corresponding, sufficiently reconstructed Android body. That requires family-level source/PC classification; source lines and clean AST counts cannot answer it.

## 3. Why work has been slow

### A. Too little code reaches comparison

Only 5.134% of original `.text` is currently compared. The project has been optimizing a small subset while discovering and integrating missing names, types and data one at a time. The evidence queue is not supplying enough complete PC contracts to feed many source owners.

Retained checkpoints show compared bytes rising from roughly 111.6 KB to 130.7 KB, 160.7 KB and 178.3 KB. Whole-code fuzzy rose despite the compared-set average declining. Broader structural transfer was working; it was simply too small in scale.

### B. Source organization sometimes destroys the required optimization context

Extracting each helper into a public function in its own unit changes more than filenames. It can force a stack ABI, prevent inlining and hide relationships from VC8's optimizer. A source family that is structurally right can then produce materially different callers and callees.

The startup-helper experiment below is a concrete counterexample to the assumption that private register calls require assembly or LTCG. Reconstructing original TU topology is part of source reconstruction.

### C. Shared layouts are repeatedly solved from individual consumers

Workers need the same object/world/AI fields, constants and declarations. If every function owner derives them independently, the shared header becomes a serial review queue and causes repeated rebuilds. If the header changes while source owners compile, results concern different contracts.

A whole Android structure is not a shortcut: PC layouts have localized insertions, removals and semantic changes. Recover a named PC field cohort once, assert it, and release all functions using that cohort.

### D. Graph evidence is useful but currently lossy

The scorer discards call order and multiplicity, collapses data references, has limited support for indirect calls and C++ name correspondence, and may propagate later hypotheses through earlier inferred pairs. Its ranking values are not calibrated probabilities.

A stale overlay or a false early identity can contaminate a whole review queue. Conversely, many good source leads do not become seeds because qualified MSVC and Itanium method names normalize differently.

### E. Some transferred representations produce a zero-score floor

Large ANI3 bodies currently produce much more code than retail. They can contain real matched instructions and still receive 0% because objdiff's accumulated insertion/deletion penalty saturates. Continuing to copy sibling functions in the same representation can add source without moving the main metric.

That calls for a bounded family-level identity/layout/TU/source-shape audit, not another prolonged sequence of tiny instruction edits or a claim that the matcher is broken.

### F. Integration and verification are shared lanes

The repository has one shared header, one symbol CSV, one main data unit, a Wine prefix and aggregate comparison outputs. Concurrent writers or aggregate builds can invalidate each other's evidence. More agents help when they have disjoint, contract-ready work; they do not remove the shared integration stage.

Existing timing logs do not separate active research, waiting, rework and tool time. A recorded synthetic test run took 156.741 seconds and a scratch TU compile 5.51 seconds; roughly four-to-five-minute CI figures are entire job durations, not compiler time. Full local build/check/match costs have not been measured under a controlled common workload. Commit gaps are not effort measurements. The next waves must measure that distinction before claiming a speedup or predicting completion time.

## 4. Compiler and compilation-unit experiments

### Real private ABI reproduced without `/GL`

`StartupParseArguments` at `0x00401100`, complete size 59, is called from retail InitOnce using ECX for count and EAX for the argument pointer. Its current public, separate-TU `__cdecl` reconstruction forces ordinary stack arguments.

An isolated fixture placed the real loop in a `static` C helper beside a caller and compiled it with pinned VC8 `/O2 /Gy /GS- /MT`, without `/GL`:

| Source arrangement | Result |
| --- | --- |
| Separate public helper/caller units | Stack argument ABI |
| Same-TU static helper, existing final-store order | Correct private register ABI; 59 bytes, three relocations; eight differing byte positions |
| Same-TU static helper, PC final-store order | **Exact 59/59 bytes with all three real relocations** |
| Static helper merged into current partial InitOnce | Helper remains exact; caller uses ECX/EAX with no argument pushes |

The root independently reproduced the comparison using existing `COFF.extract_function`, `relocate_function` and `PE.read_va`. Nothing was masked or truncated. Retail bytes were used only as the comparison target. InitOnce itself remains incomplete and nonmatching.

This finding falsifies the blanket claim that ordinary VC8 C cannot emit the observed private ABI. It does not prove that the entire retail program avoided LTCG. It means same-TU static closure is the first test, and that a global declaration should not make a truly local helper externally callable.

Production integration would remove the obsolete external declaration/definition, keep the emitted local symbol mapped, and inspect both helper and caller. If another unit calls the helper or its address escapes, that changes the hypothesis and requires another probe. Consolidate by verified original relationships, not merely because two files have similar names.

### `/GS` is a supported correction, with limited immediate yield

Retail has 443 cookie references from 440 distinct caller functions to `0x00829570`. Our `/GS-` build cannot generate those stack-cookie sequences.

A frozen scratch build covered **all 93 source units and 1,030 compiled functions**, adding only verified scratch bindings for:

- `___security_cookie` at `0x00829570`;
- `@__security_check_cookie@4` at `0x006e693a`.

With `/GS`, 40 bodies changed across 19 units, covering 29,175 original bytes. Two additional functions became exact: **NuFileCopy, 75 bytes**, and **NuStringFilterBadWords, 130 bytes**. No previously exact function changed or regressed. Compiled-definition exact counts and report-row exact counts differ because copies/aliases are counted differently; do not conflate them.

Of 33 changed functions with scored alignments, 27 improved and six regressed; their net weighted gain was approximately 242.8 bytes. Seven changed bodies had no scored alignment in that scratch calculation and must remain zero/unavailable in any whole-code summary, not disappear from its denominator. This is a small whole-project gain, not the explanation for missing tens of percentage points.

The isolated all-source `/GS` objects also linked successfully to a 328 KiB Windows executable with the pinned linker, static CRT and existing import libraries. This checks the compiler-generated cookie reference closure without adding replacement implementations. Production mapping/object/match gates still remain part of integration. The camera fixture is instructive: enabling `/GS` alone left its body unchanged, while retail has a cookie. The current source's local-variable/frame representation therefore still differs from retail.

### Avoid broad flag guesses

Controlled tests found:

| Probe | Baseline | Alternative |
| --- | ---: | ---: |
| NuMtxMul, 706-byte retail body | 95.50% fuzzy | `/arch:SSE`: 41.99%, candidate grows to 960 bytes |
| Same matrix function | 95.50% | `/fp:fast`: 93.97% |
| Same matrix function | 95.50% | `/O1`: 91.23% |
| GameAnimSet_Play | 81.46% | `/Ob0`: 77.05% |

Keep `/O2`, `/Gy`, `/MT` and default scalar x87 behavior. Use explicit SSE where PC code proves packed SIMD. Test C versus C++, floating-point settings and local-buffer shape by original-TU cohort when there is specific evidence. A single arithmetic C/C++ probe produced identical bytes; that does not establish equivalence for methods, constructors, exceptions or overloads.

The PE linker version and Rich-header build records support VC8, but do not recover every original switch. Do not replace the compiler or globally disable inlining because one family differs.

### LTCG is a contingency, not the next architecture rewrite

The pinned compiler accepts `/GL`, but its output is not ordinary COFF accepted by our matcher. A scratch `/LTCG` link failed because the minimal bundle lacks `msobj80.dll`. That is a tooling limitation, not evidence about retail flags.

A future LTCG experiment would need the complete verified toolchain and independently generated linked-function extents/symbol provenance/relocation handling. It cannot reuse retail addresses as candidate addresses. The current exact static-helper result provides a cheaper, proven route first. Microsoft's documentation describes the separate `/GL` and LTCG capabilities; modern documentation alone does not establish this game's historical settings. See the [official `/GL` reference](https://learn.microsoft.com/en-us/cpp/build/reference/gl-whole-program-optimization?view=msvc-170).

## 5. How much mapping can be accelerated

### Fresh measured retrieval

The private refreshed run used the current PC map, retained original exports, original ELF features and frozen clean source graph. Feature refresh took **105.46 seconds**, and baseline scoring **17.35 seconds**. Whole-binary redisassembly is not necessary for each review batch.

| Proposal pool | Pairs | Candidate extents known in analysis | Boundaries open | Candidate span union |
| --- | ---: | ---: | ---: | ---: |
| All inferred mutual-top pairs | 1,558 | 1,350 | 208 | 715,128 bytes |
| At least three seed supports and bilateral margins ≥0.10 | 572 | 451 | 121 | 404,216 bytes |
| At least five seed supports and bilateral margins ≥0.20 | 228 | 170 | 58 | 222,358 bytes |

There are 822 current name seeds. These thresholds select review leads; they are not confidence percentages. Candidate single-range extents remain Ghidra hypotheses until complete PC boundaries are checked.

The every-fourth-seed holdout hid 206 accepted labels. It proposed 122, of which 120 agreed with those labels; initial top-five retrieval found 145 of the 206. This is useful recovery on an already mapped, anchor-rich population. It does not demonstrate 98% precision among unknown functions or broad unseeded subsystems.

A harder, separately run source-family holdout removed all 122 current seed labels in six families: file core, game animation, SFX, AI script, GameObject and cutscenes. Of those, 85 had round-one candidate connections, 45 had a correct top-one result and 71 a correct top-five. Four-round selective propagation proposed **66 held-out pairs, 65 correct**: roughly 53% label recovery with one false accepted proposal. Root recomputed these totals from the six unscrubbed holdout runs' JSON and individual details; an intermediate worker summary incorrectly reported 65/65. The file-family false proposal passed ordinary ranking/support gates, demonstrating why they cannot authorize CSV insertion. A direct held-name data-token leakage check found none in the three tested larger families; scrubbed reruns were identical. This does not rule out every possible correlation or leakage path, and known-label families remain easier to evaluate than unknown PC identities.

The clean source overlay joins 11,723 Android ELF nodes and adds 676 direct calls. It leaves 1,105 definitions and 13,784 calls unjoined/indirect. Inline-only functions, function pointers, virtual dispatch and platform-specific calls must remain explicit missing evidence rather than invented standalone identities.

### Cross-ABI C++ names are a cheap improvement

A private crosswalk demangled mapped MSVC methods and compared fully qualified names plus normalized parameter/cv signatures against Android Itanium names. Of 24 initial unique signature intersections, **22 passed the scorer's uniqueness/alias gates**, raising seeds from 822 to 844. Inferred pairs rose from 1,558 to 1,564. This is modest additional reach, but fixes a real normalization loss.

Keep class/namespace, overload parameters and qualifiers. Never seed just `GetStatus`, `Update`, a destructor spelling or another unqualified generic method. Demangling does not prove PC layout, vtable slot or ABI.

### Representative manual checks

The new queue has useful large-family evidence, not merely small accessors:

- AICircle at `0x0051f0a0`: vector subtraction, angle calculation/rotation/addition and a `0.5f` reference align with Android; caller ABI still requires confirmation.
- NuPtInPoly at `0x005bf770`: branches to six corresponding axis-specific triangle predicates provide a strong family anchor.
- CutScenePlayer_Configure: the candidate at `0x0057e430` has matching `clip`, `intro`, `midtro`, `outro` and related configuration vocabulary. Another high-ranked address has different level/gizmo parsing behavior and is a false collision.
- NuMusic::Debug: distinctive track/status/class/volume/fade text and repeated font-print flow distinguish the candidate.

These checks show both the value of retrieval and the need for semantic review. Several strings from one parser are correlated evidence, not several independent proofs.

### Improvements ranked by likely return

1. Enforce source/header/config/tool/executable fingerprints and target TU language modes. Refresh only changed evidence; freeze a snapshot while reviewing it.
2. Preserve callsite order/multiplicity, reference widths/read-write roles and field-access cohorts. Retain rare anchors while discounting correlated parser vocabulary and ubiquitous math/CRT neighbors.
3. Distinguish original seed support from support inherited through inferred pairs. Keep alternatives and propagation provenance; prevent an inference chain from becoming an accepted fact automatically.
4. Expand RTTI/vtable and initializer/callback anchors. PC retains approximately 120 RTTI type-name strings. Recover PC method roles and slots independently; Itanium headers/destructor variants cannot supply MSVC slot numbers.
5. Add independent decompiler-derived retrieval where the reference graph is weak. Pilot before replacing the current pipeline.

BSim is a serious candidate for item 5 because its normalized decompiler data/control-flow features intentionally avoid register names, constants and types and support comparison across compiler/architecture changes. It complements our exact string/data and ABI evidence; it does not establish a function's source, complete extent or type. Its local H2 database can avoid a server for a small pilot. Start with blinded knowns and reviewed unknowns, log top-one/top-five retrieval, ambiguity and review time, and keep it only if it supplies accepted PC bytes the current scorer misses. See [Ghidra's BSim introduction](https://ghidra.re/ghidra_docs/GhidraClass/BSim/BSimTutorial_Intro.html) and [command-line workflow](https://ghidra.re/ghidra_docs/GhidraClass/BSim/BSimTutorial_BSim_Command_Line.html).

Version Tracking's reference correlators are another way to reuse existing analyzed projects and confirmed anchors. BinDiff overlaps more with our current graph features. A new custom whole-program P-code matcher should wait until a bounded BSim/Version Tracking test shows an actual unmet need.

## 6. Layout recovery that enables bulk source transfer

The right unit is a **PC-confirmed field cohort**, not a complete Android type and not numeric byte-pointer access.

For each needed member, retain `(function VA, owner hypothesis, displacement, width, signedness, read/write, index scale/stride, instruction evidence)`. Confirm its semantic role with a producer/initializer and consumers. Establish owner extent or array capacity from allocation/clear/iteration bounds before defining storage. Express it as a named field with `STATIC_ASSERT` checks; keep unreviewed gaps opaque.

### GameObject: substantial shared structure, several different deltas

The Android i686 record-layout probe gives GameObject size **0x10e4**. PC object-array stepping and owner-index arithmetic independently indicate **0x10d8**. The current PC declaration only exposes the observed prefix and does not yet assert the full storage stride. That existing research should be integrated rather than rediscovered.

| Field cohort | PC versus Android evidence | Transfer consequence |
| --- | --- | --- |
| Position/velocity/transform/bounds through `+0x1b4` | Several same-offset anchors, including position `+0x5c`, transform `+0xb8`, bounds near `+0x178` | Strong basis for movement, collision and transform families |
| Transform-to-bounds gap | Same boundary, but PC interior roles remain unproved | Do not infer two Android matrices merely from interval size |
| Vertical/contact state | Some equal offsets have different semantics; magnitude fields shift by four bytes | Recover PC floor/water/velocity roles independently |
| Suspension and character context | PC offsets include local `+0x10` shifts relative to Android | One global adjustment would be wrong |
| Spline and run-speed tail | PC has `-0x0c` shifts in another region | Preserve PC-specific asserted fields |

The reviewed object examples touch multiple cohorts: GameObjectRotation, MovePlayerSpline, GameObjectOrigin, GameObjectNearFloor and GetVehicleHoverHeight. Full owner size does not identify unknown matrix/joint fields or their capacities. Those remain separate tasks.

### WorldInfo: particularly favorable shared cohorts

Android WORLDINFO is **0x51b0**, while PC's independently known size is **0x51ac**. Level/progress/scene anchors around `+0x128..+0x15c` agree. PC Init copies exactly `0x2800` progress bytes to `+0x15c`, providing stronger evidence than the Android declaration alone. Several page/system pointers also align.

The sound/bolt tail differs locally by four bytes: PC fields at `+0x4710`, `+0x4b10`, `+0x4b14` correspond to Android positions four bytes later. Earlier fields agree, so subtracting four globally would corrupt the type.

WorldInfo_Load, Init and Dump total 3,722 already identified PC bytes and provide writer/consumer evidence for a much larger world/source queue. Solve the next small system-pointer and progress cohorts once, then release their consumers in parallel.

### ABI/type differences beyond member offsets

Both targets use four-byte pointers and 32-bit `long`, but that is not whole-ABI compatibility. Android i686 uses the Itanium C++ ABI and a four-byte `wchar_t`; VC8 Win32 uses MSVC class/method conventions and two-byte wide characters. Check real PC callers and initializers for wide text, overloads, virtual/destructor variants, aggregate returns, `this` adjustments, reference parameters, enum/boolean widths, packing/alignment and bitfields. C++11 syntax/library availability is another source adaptation issue, not a reason to change the pinned compiler.

A prototype inferred from Android is a hypothesis until PC stack/register use, cleanup and return consumers agree. Constant-width arithmetic should preserve the PC signedness and conversions. Stable field boundaries can support a partial PC type without asserting the meanings of its opaque interior. No cast through bytes may bypass that verification.

### Data research must target code blockers

Separate these tasks:

- constants/scalars with complete initializer evidence;
- pointer tables with real relocations and callback identities;
- writable owner structures;
- BSS arrays whose exact capacities come from PC producers;
- compiler literals, vtables and RTTI with real ownership/linkage.

A whole-binary sweep of every zero-filled byte is less valuable for fuzzy code than the few tables blocking a large source family. For example, retained NetMessage research proves a 512-entry pool of 0x4b4-byte records, **616,448 bytes total**, plus a 16-byte wrapper and bounded constructor/writer bodies. That can close a bolt/network dependency and improve inventory, but it is not yet integrated source or additional verified project coverage. Bolt_Init must still perform actual initialization; do not return an uninitialized allocation as successful creation.

## 7. Platform boundary: what can actually transfer

| Family | Reusable candidates | PC-specific work to reconstruct from retail |
| --- | --- | --- |
| Gameplay, movement, AI, world state | State transitions, selection, path/spline logic, configuration parsing, transforms | Version-specific fields, constants, callbacks and control-flow changes |
| Math, animation, geometry | Track decoding, interpolation, bounds, matrices, visibility | x87 ordering, packed formats, SIMD representation, local helper context |
| Rendering above the device | Culling, scene traversal, display-list production/sorting, state decisions | D3D9/D3DX resources, COM calls, reset/loss handling, uploads and draw submission |
| Audio above voices/devices | Routing, fade timing, attenuation, sample/voice selection | DirectSound buffers, capabilities, synchronization, hardware status/calls |
| File/archive core | DAT tables, metadata lookup, parsing, proven decompression | Win32 handles/read/seek semantics and PC path rules |
| Input above device polling | Game-level button transitions, dead zones, action mapping | DirectInput/XInput, scan codes, focus/acquire, PC controller layout |
| Cutscenes/video | Configuration, timing/state logic when PC correspondence is established | Bink, frame surfaces, audio synchronization, PC display modes |
| Background work | Queues, request/callback bookkeeping | Windows events, critical sections, thread creation and waits |

Android EGL/GLES, OpenSL, APK assets, touch interfaces and pthread adapters are not PC implementations. Neither are generic substitute backends. Transfer portable bodies up to real PC services; reconstruct those services as their own cohorts when evidence supports them.

NuBridgeUpdate is a useful positive control: a 3,073-byte gameplay body currently scores 85.63% without a complete backend. Areas_ConfigureList scores 75.77% over 1,853 bytes. GameObjectRotation is already a real 2,650-byte partial family at 40.77%. These are stronger expansion templates than NuMain's broad, heavily partial 4,332-byte body at 13.10%.

A playable boot remains a separate integration milestone. It can test source ordering and dependencies, but should not force reconstruction of all graphics/audio/input initialization before unrelated portable code can enter comparison.

## 8. Objdiff limitations and honest structural progress

The main metric should remain whole-PC fuzzy coverage. Its current low value is mathematically sensible. Do not replace it with compared-set similarity or shrink the original denominator to make it look larger.

The pinned algorithm aligns decoded opcode streams and applies mismatch penalties. Insertions/deletions cost much more than immediate/register differences; enough structural differences saturate the score at zero. The [objdiff-core 3.8.1 comparison source](https://docs.rs/crate/objdiff-core/3.8.1/source/src/diff/code.rs) defines this behavior. That source supports the scoring interpretation, not semantic correctness of a port.

For ANI_SimpleAni3PlayerV4Joint_Quat3:

- retail: 1,990 bytes, 614 decoded instructions;
- candidate: 3,503 bytes, 1,100 instructions;
- 145 alignment rows match, but 225 deletes and 711 inserts already exceed the penalty ceiling;
- both target/self and candidate/self comparisons score 100%.

The current 0% is therefore not a failed symbol pair or unsupported decoder. Eight ANI3 players show similar growth. The PC packed clip helper is also SSE, as is its candidate; its zero score reflects different spills, stack layout, loops and scheduling, not an x87/SSE architecture mismatch.

Conversely, an isolated changed external call destination can retain 100% fuzzy similarity while exact bytes differ. Full byte/extent equality remains the exactness gate, and PC semantic review remains necessary even at high fuzzy scores.

A current report-row breakdown, before copy/alias deduplication:

| Objdiff band | Functions | Original bytes |
| --- | ---: | ---: |
| 0% | 28 | 18,797 |
| Above 0%, below 25% | 40 | 21,247 |
| 25–50% | 101 | 39,316 |
| 50–75% | 143 | 37,591 |
| 75%, below 100% | 157 | 29,764 |
| 100% fuzzy | 560 | 31,484 |

These rows total 178,199 bytes; one duplicate compiled copy accounts for the extra 54 bytes in the native CLI denominator, and a same-address alias explains the remaining unique-union difference. Fuzzy 100% is not an exactness category.

A complete PC AST census found **64 functions containing 277 STUBBED markers**, covering a nonunique 48,468 original bytes and approximately 15,952 similarity-weighted bytes. No marker is emitted as implementation code. These functions are explicitly partial. Absence of a marker does not prove completeness, and diagnostics-only omissions differ from missing return-value-producing behavior.

Publish these distinct contributor measurements alongside the main badge:

1. accepted unique PC code bytes;
2. unique compared PC bytes;
3. whole-code fuzzy and exact bytes;
4. score-band distribution, including zero-floor bodies;
5. partial functions and their skipped actions;
6. candidate bytes awaiting identity, boundary, ABI, field or dependency review.

Never combine them into an opaque “structural match percentage” or credit speculative identity as implemented code.

## 9. Alternatives considered

| Strategy | Strength | Main problem | Recommended use |
| --- | --- | --- | --- |
| Follow NuMain depth-first | Clear runtime ordering and callers | Huge platform fanout; only covers direct reach; serial dependency discoveries | Discovery and bounded integration tests |
| Perfect current small functions first | Trustworthy exact gains | Maximum possible whole-code gain is limited by the current 5.134% compared scope | Fix actual semantic/type errors immediately; defer cosmetic byte tuning |
| Copy Android modules blindly | Fast source-line growth | Wrong layouts, platform code, private ABI and incomplete Android bodies can produce compiling but false source | Scratch candidate-generation experiments only |
| Require every type/backend to be complete first | Simple final interfaces | Delays portable functions unnecessarily; full layouts are enormous | Only where the next operation truly requires complete ownership/bounds |
| Group only by call-graph SCC | Preserves recursion cycles | Nearly all graph nodes are singletons; ignores shared fields and private TU context | Cycle safety check |
| Reviewed TU/field/dependency cohorts with a mapping queue | Preserves optimization context and amortizes shared research | Needs reliable interface packets and a deliberate integration lane | Main expansion strategy |

The recommended strategy is the last one, initially combining accepted portable backlog with newly reviewed high-byte candidates. It separates evidence discovery from implementation without making every cohort wait for an entire subsystem to be understood.

## 10. How to run broad waves

### A rolling evidence and source pipeline

Use these explicit states:

`candidate → identity reviewed → full boundary/ABI reviewed → fields/data/TU contract ready → source implemented or honestly partial → compiled/link checked → compared → exact/nonexact`

Each state records PC VA, full original interval, evidence and unresolved facts. Count original interval unions, not source lines or map rows. A function can leave the implementation queue when a return value, owner bound or control-flow-critical dependency is unknown while independent siblings proceed.

Create compact review packets from existing exports and disassembly rather than making every worker repeat whole-binary searches. Include:

- top alternatives and why the chosen identity fits;
- PC call sequence, constants/strings and global references;
- entry/caller stack/register contract and return use;
- full extent including internal gaps/tails;
- original-TU/static/helper/inlining relationships;
- named fields, widths, capacities and PC assertion evidence;
- required data initializers/relocations and emitted symbols;
- actual missing PC operations, separated from diagnostic gaps.

Mechanical tools can extract a source body, rename already approved identifiers, enumerate references, and compare signatures. They must not infer a layout or replace a platform implementation automatically. Keep candidates in research until reviewed.

The existing AST export captures globals/calls but not a complete field-use manifest. A useful extension is to collect member expressions, referenced record declarations and source types for each Android body, then join them to PC displacement/access facts. On the PC side, use bounded disassembly or Ghidra P-code with base-pointer provenance: a displacement from an arbitrary register is not automatically an object member. This can generate candidate field constraints and an exact list of which functions need each contract. It should shorten packet preparation and reveal reused cohorts without bypassing named fields or assertions.

Review can itself be parallel. Several PC evidence owners should inspect disjoint candidate families and hand over complete packets; root checks shared-contract conflicts and uncertain/contradictory cases, rather than serially redoing every accepted packet from scratch. Preserve their evidence so a questionable identity or boundary can be revisited. Automatic checks cover interval conflicts, ABI consistency, emitted symbols and relocations; they cannot certify semantic identity.

### Scale and concurrency

Start with **50–100 KB candidate-byte waves** spread across several related source units. This is a deliberate increase from tiny batches, not a rule that 100 KB must be accepted before anything moves. Release confirmed contract-ready subsets continuously; reject uncertain functions without holding the entire wave.

Use as many bounded Luna tasks as there are independent packets and source units, within the runtime's available slots. Keep no nested delegation. A practical fully occupied team can have two mapping/boundary reviewers, one data/layout researcher, several disjoint TU implementers, and the root performing concrete ABI/layout/source reconstruction plus shared integration. Roles should change as queues drain; an idle data researcher can own a ready TU.

There must be one writer per source TU and one owner for shared header/map/report integration. Root owns `src/saga.h` and accepted CSV changes; one delegated writer at a time may own `src/data.cpp`. Parallel data researchers can propose non-overlapping globals without concurrently rewriting that file.

Freeze small shared declaration bundles before the relevant workers compile. Integrate sibling requests together instead of causing a global header rebuild for every new function. Keep true private declarations local. Preserve one `saga.h` and one `externals.h` at the source root and the simple Makefile.

Isolated per-TU compiles and comparisons may run concurrently in distinct scratch outputs. Reserve one aggregate verification/report lane. Do not run destructive cleanup, two aggregate matchers, or simultaneous edits of shared inputs while claiming a frozen result.

### Acceptance and feedback

Correct immediately when comparison reveals a wrong identity, field, signedness, constant, branch, return value, FP operation order or calling convention. Defer pure register/order tuning once a family has coherent PC structure.

Before copying many siblings of a 0%-scoring large body, audit a few representatives for identity/extent, source omissions, typed widths/strides, static/TU context, local buffers, intrinsics and loop shape. If representation remains far from retail, route that family to a bounded representation task while other families continue expanding.

Do not invent stub callees or favorable return values to keep the EXE linkable. STUBBED markers belong at real missing actions and require an honest partial status. If removing an action makes a consumer's logic unknowable, return that function to research rather than fabricating its result.

After an actual coherent source/map batch, root runs:

```sh
make check
make -j"$(nproc)" build check-objects
make -j"$(nproc)" match
```

Inspect affected complete comparisons, preserve existing exact matches, update reports/badges from that frozen state, commit with the required Codex attribution, push under the existing authorization, and inspect CI. Commit roughly every 30 minutes of active implementation when a coherent verified checkpoint exists; do not manufacture a checkpoint out of unverified fragments.

## 11. Concrete expansion queue

Candidate bytes in this table are research extents from the refreshed three-support tier, not accepted coverage. Accepted backlog and already-compared exemplars are explicitly labeled. Rows share dependencies and must not be summed blindly.

| Priority/cohort | Available leverage | First PC contract to solve |
| --- | --- | --- |
| Startup/private helpers | Exact 59-byte ignored positive control, larger caller context | Same-TU static helper/public declaration removal; real caller ABI |
| Compiler baseline | All-current-source `/GS` experiment | Cookie aliases/CRT link, no exact regressions |
| Movement | 26 candidate pairs, 45,182 candidate bytes | Position/contact/spline fields; original helper grouping |
| GameObject state | 16 pairs, 23,049 candidate bytes | Full PC stride and state/transform field cohorts |
| AI system | 39 pairs, 21,176 candidate bytes; 11 boundaries open | AIPACKET/APIOBJECT/process fields, action callbacks and inline effects |
| AI game/core helpers | 10 pairs, 11,842 candidate bytes | Separate game-state logic from editor/debug/no-op identities |
| World/area lifecycle | Existing Load/Init/Dump extents and accepted unimplemented functions | Progress/page/system pointers, parser tables and initialization |
| Characters/models | Existing accepted loader backlog, including 3,370-byte LoadAreaCharacters | Model/list records, full owner extents and loaders |
| Parts/effects | 12 pairs, 11,561 candidate bytes | Particle records, pools, producer bounds; real rendering seam |
| Terrain/collision geometry | Two stronger candidates, 11,485 candidate bytes | Geometry/scene layout, raycast semantics; avoid fabricated collision results |
| Render command producers | 18 pairs, 8,799 candidate bytes | Scene/display records, culling/sort/state contracts; real D3D edges |
| Cutscene configuration | Four pairs, 7,041 candidate bytes | Parser anchors, clip/sequence records and version branches |
| Gizmos | Existing accepted backlog around 6.25 KB in an earlier selected queue | Per-family records and genuine world/object callbacks |
| File/DAT/parser core | Accepted unimplemented code plus existing partial NuDatOpenEx | Stream cursor/metadata counts, PC read/seek seam |
| Camera/geometry | Existing partial and newly evidenced bodies | Scene bounds/tail fields, local scratch/frame representation |
| Game animation and key decoding | Existing GameAnim family plus ANI3 source | Track/scale/table producers, inline helper topology |
| ANI3 packed evaluation | Roughly 14.5 KB already compared at the zero floor | One representation pilot before further siblings; code-growth diagnosis |
| Audio scheduling/music | Qualified C++ anchors and portable manager source | Manager/voice/track records, confirmed method ABI and DirectSound seam |
| Bolt/network closure | Retained constructor/writer/pool evidence and 2,551-byte Bolt_Init candidate | Actual serialization/owner index/init callbacks, no fake creation |
| PC device/backend services | Imports, COM methods, callback tables | Independent retail reconstruction; lowest direct Android reuse |

The large editor-tool candidate family also deserves classification: 11 stronger proposals cover 17,353 candidate bytes. Determine which editor/debug services actually exist and matter in the retail executable before making this a priority. Its high candidate size is not proof of easy game-logic transfer.

## 12. Next experiments and decision rules

### First: integrate the proven architectural corrections when implementation resumes

Test same-TU startup helper ownership and `/GS` as separate reviewable changes. Keep complete symbol maps and link checks. Their direct byte gains are small; their purpose is to remove known false assumptions before scaling source transfer.

### Second: execute a broad GameObject/movement/AI pilot

Review the 89 KB candidate wave using several evidence owners. Establish only the PC fields needed by its confirmed subsets; keep unknown matrix/joint regions separate. Preserve private helpers with their callers. Root should implement a concrete difficult layout/ABI-dependent member alongside source workers.

Measure:

- candidate bytes reviewed and accepted;
- accepted bytes made ABI/layout/TU-ready;
- new unique bytes actually compared;
- net whole-code fuzzy weighted bytes and percentage points;
- exact bytes gained/lost;
- partial functions/skipped actions;
- research, source, integration, tool and waiting time separately;
- shared-header changes, recompiles and rework causes.

Do not estimate identity probabilities from scorer margins. The realized acceptance and similarity of the first wave provide a better forecast than Android's headline percentage.

### Third: contrast a low-dependency resource/config wave

Use accepted backlog and strong parser/data anchors. Compare yield and active effort with the object-layout investment wave. This prevents committing the entire project to the first large family if its layouts or representation prove unusually expensive.

### Fourth: extend discovery where existing anchors do not reach

Run family-disjoint retrieval tests and a bounded independent-signature pilot. Prioritize accepted original bytes per review hour, not a larger tentative-pair count. Track complete boundary review separately from naming.

If eligible accepted bytes accumulate while implementation lags, add independent source owners. If workers wait on fields/identities, shift capacity to evidence/layout tasks. If compiled families repeatedly hit the zero floor, diagnose their representation while expanding other families. If aggregate verification dominates elapsed time, batch freezes and cache unchanged comparisons without weakening provenance or exact checks.

A 100 KB hypothetical wave at 70% acceptance, 70% implementation of accepted bytes and 70% similarity adds about 34.3 KB effective code, or 0.99 whole-PC points. Those rates are illustrative, not measurements. Replace them with observed family-specific rates. The current 715 KB inferred pool cannot alone supply the roughly 1.5–2 MB of additional compared code required by an 80%-mean 40–50% target, even under optimistic acceptance. Mapping discovery must continue beyond the initial ledger.

No completion-date estimate is defensible from the existing logs. The first goal is a demonstrable increase in net similarity-weighted bytes per active team hour across several coherent waves, with no hidden semantic or verification debt.

## 13. Evidence and reproducibility

Production inputs and current metrics are in `config/project.json`, `config/toolchain.json`, `config/tools.json`, `config/symbols.csv` and `matching.json`. No retail/tool binaries or extracted target instruction bytes are included in this document.

Detailed local research is retained under `build/analysis/strategy/`:

- `android-transfer.md`: Android match semantics, source inventory, family sizes and platform exclusions;
- `android-source-graph-current.json` and `.meta.json`: frozen target AST and input fingerprints;
- `mapping.md`, `mapping-score/`, `mapping-refresh/`, `mapping-family-holdout.json`: fresh proposals, holdout and C++ crosswalk experiments;
- `compiler-architecture-review.md` and its directory: private-ABI fixture, controls and independent full-byte comparison;
- `compiler.md`, `compiler-probes/`, `compiler-allgs/`: flag controls and all-source `/GS` experiment;
- `layouts-data.md` and record-layout/disassembly probes: PC versus Android field cohorts;
- `metrics-matcher.md`, `scratch/`: scoring sensitivity, ANI3/SSE full streams and self-comparison controls;
- `platform-render.md`: platform boundary and large-function examples;
- `gameplay-roadmap.md`: source-family backlog and graph review;
- `workflow.md`: timing/process audit;
- `strategy-review.md`: independent synthesis and critique;
- `root-inventory.json`, `root-graph-cuts.json`, `root-port-census.json`, `root-score-bands.json`: root inventories and independent measurements.

These supporting files are deliberately ignored. Earlier phase-one notes contain historical hypotheses; the integrated conclusions above use final experiments and distinguish confirmed PC observations from candidate extent/type interpretations. Re-run source fingerprint checks before using the frozen Android snapshot as evidence for later source edits. Root reproduced the exact helper comparison, independently verified both new `/GS` exact candidates and the scratch linked PE, corrected the family-holdout arithmetic from its machine output, and regenerated a complete 93-unit PC source census; no aggregate production matcher was rerun merely to write this analysis.
