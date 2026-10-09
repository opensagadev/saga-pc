# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline WW is [93b3548](https://github.com/opensagadev/saga-pc/commit/93b3548f5fee4ab8e4fcf58918aa915575e8875a), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37883640294) passing all 13 steps. XX exact-revision CI is pending publication.

Checkpoint XX restores the native LoadPerm1 call to DebrisSetup and its complete
43-byte wrapper. The wrapper is byte exact and forwards the six verified inputs
and native additional constants to existing DebrisSetup2 and its real providers.
The existing LoadPerm1 body grows from 781 to 813/native 1,351 bytes and remains
partial; restoring its caller earns zero additional compared range credit.
The 15-byte particle path equals native, but its original owning extent stays
unknown and receives zero coverage/exact-data credit. Header and shared data are
unchanged.

Whole-PC fuzzy coverage is 29.476569%, up
0.002320 percentage points from WW. Exact code is
233,512/3,471,246 bytes (6.727037%), up 43; exact data remains 637,833 bytes.
Every previous exact code/data result and all 13 named member rows survive.
Among 16,597 old complete physical outputs, 16,596 retain identical relocated
bytes; only the reviewed LoadPerm1 call changes. All 237 other outputs in the two
changed owners and all 340 unaffected object hashes are preserved.
Required checks pass: 796 tests (three existing skips), LLVM18 formatting and
342-unit analysis, normal source link/object checks and full retail matching.

The latest ordinary source executable exits 5 in the backward-copy read of
PCByteCopy. Its complete 84-byte source body equals retail. Fault source5160a0,
native4ef6a0, reads BYTE [ESI-1] at invalid address02def4bf. This run does not
establish the caller or the copy arguments; archive/resource producers are the
next investigation. The earlier NULL DebMat[5] failure did not recur after
restoring genuine DebrisSetup. No guard, replacement material or invented
successful result is added.

One ordinary source-game run exited naturally after 5.800 seconds. All 342
objects, 455 frozen inputs and linked-image/report pins remained unchanged.
Wine crash suppression was set and queried; no owned processes or windows
remained after cleanup. Broad class-name window samples are unclassified.
No successful frame, GPU presentation, loaded world or gameplay is established;
the source build remains not playable.

1. Trace the genuine archive/resource producer and caller at the observed exact-copy startup fault.
   Preserve native control, callbacks, returned values and named field offsets;
   do not add a standalone counter reset, force focus or enlarge storage to hide
   the crash.
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
