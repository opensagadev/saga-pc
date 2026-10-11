# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline BBB is [c1e87df](https://github.com/opensagadev/saga-pc/commit/c1e87dfe7901cb4b383e9b3a4465a51a28e9f2be), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37892772484) passing all 13 steps. CCC exact-revision CI will be checked after publication.

Checkpoint CCC transfers nine complete functions / 1,685 newly compared PC bytes:
occlusion initialization (three / 720), startup material creation (one / 469),
and input mapping/configuration (five / 496). Five functions / 203 bytes are
byte exact. The other four retain complete nonexact comparisons and independently
reviewed PC contracts. Three original hardware sites now call real providers;
the partial hardware parent receives no duplicate transferred-byte credit.

Whole-PC fuzzy coverage is 29.547906%, up 0.034794
percentage points. Exact code is 234,248/3,471,246 bytes
(6.748240%), up 203; exact data remains 637,833 bytes.
Every prior exact contract survives: 2,451 code copies, 5,712 data copies and
13 named members. Of 16,636 prior complete outputs, 16,633 stay identical.
The three changes are the intended hardware parent and two specific previously
reviewed camera/particle compiler-output pairs, with complete bytes/references
and their stable-memory/floating-point qualifications retained.

The existing occlusion manager's complete 368-byte initializer remains unchanged
and native-equal. Six new scalar/array views total 36 typed bytes in ordinary
sections; all complete emitted initializers equal native, but original owning
extents stay blank and receive zero data credit. Three material class statics
remain in original occlusion.cpp. No new constructor or dynamic initializer is
introduced. Existing central initialization remains unchanged.

All required local checks pass: LLVM18 format/tidy, 796 tests (three existing
skips), source build/link, 343-owner object coverage and full retail matching.
The initial sandboxed match attempt could not start Wine IPC; the unchanged
normal-environment retry passed. No compiler flags, source guards, comparison
masks or extents were changed to obtain these results.

CCC has not been run as a game. The latest full-game observation remains BBB's
60-second deadline termination, with no fault recorded and no verified native
game window, recurring frames, loaded world or playability. The earlier AAA
declaration-cache failure and unproved original allocation remain unresolved.
No cache size, counter, search or guard is changed. Occlusion keeps native
default capacity zero and its zero-capacity early return.

Next is the producer-backed viewport family (two / 213 candidate bytes), with
genuine 64-byte viewport views and storage review before activation. Lighting
and resource registry providers remain separate bounded holds. Candidate bytes
receive no comparison credit. Cleanup follows publication before further source
integration; originals, current tools, active proofs and human edits are retained.

1. Transfer the producer-backed viewport213 family with canonical 64-byte views
   and genuine storage, then continue lighting and resource-provider closure.
   Recover original declaration-cache storage and the preceding write separately;
   do not enlarge arrays, reset counters or add guards to bypass that failure.
2. Transfer connected gameplay, movement, resource and loader families in parallel
   where their PC contracts are established. Reattach real providers to partial
   callers and record remaining void omissions.
3. Integrate shared named layouts, mappings and data once for each coherent wave.
   Preserve every prior exact contract and investigate changed complete outputs.
4. Verify actual source/object/report agreement, run required checks and meaningful
   bounded runtime tests, then publish a coherent checkpoint. Verify CI on its
   exact pushed revision before using it as the next baseline.

Use [strategy.md](strategy.md), [identification-plan.md](identification-plan.md)
and [transfer-execution.md](transfer-execution.md) for selection and validation.
Current runnable milestones are in [the roadmap](../docs/roadmap.md).
The earlier [execution plan and release history](../docs/history/progress-plan-through-2026-10-03.md)
remain available without serving as today's task queue.
