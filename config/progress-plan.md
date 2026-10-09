# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline XX is [d1695b6](https://github.com/opensagadev/saga-pc/commit/d1695b6f9004d105f4fbf500dc9bdc069a93aa4b), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37885753616) passing all 13 steps. YY exact-revision CI is still pending. ZZ exact-revision CI will be checked after publication.

Checkpoint ZZ adds six complete post-effect/light member methods / 219 newly
compared PC bytes; four methods / 47 bytes are exact. The genuine generic
post-filter cohort is consolidated in nupostfilter.cpp; related deferred/light
methods share nucore.cpp. All 17 old functions and three old hardware initializers
retain their complete outputs. The deferred light array/API now names the actual
NuDynamicLight object type; its pointer ABI is unchanged. Named shadow-list fields
preserve the existing 0x380 light-record layout. No class instances, constructors,
vtables or higher renderer activation are fabricated.

Whole-PC fuzzy coverage is 29.498644%, up
0.003541 percentage points from YY. Exact code is
233,733/3,471,246 bytes (6.733404%), up 47; exact data remains 637,833 bytes.
All 2,443 previous exact code copies, 5,709 data copies and 13 named members
survive. Of 16,618 prior complete outputs, 16,614 are byte identical; three
unchanged-source Camera/Init/Particle outputs reproduce specific independently
reviewed historical pairs. The remaining change is the explicit native-zero
cache prefix extension. No general compiler-variation waiver is used.

Required checks pass: 796 tests (three existing skips), LLVM18 formatting and
343-unit analysis, normal source linking/object checks and full retail matching.
The 89 natural SDK outputs retain identical admission descriptors; only header
recipe pins change, with fresh complete link/archive/reference proof and zero
PC credit. A sandbox Wine IPC denial was resolved by rerunning unchanged checks
with the authorized build permissions.

The latest ordinary source run passed the earlier vertex-cache copy failure and
exited 5 after 7.879 seconds at a new NULL-list read. A separate headless capture
proves NuMtlDestroy passes NULL g_overrideList to NuLstGetNext. That complete
40-byte list function is byte exact; its NULSTHDR.head read at +8 faults. The
canonical list pointer has no producer in current source. Hardware startup still
omits native NuMtlInitOverride at 0x5e4eb0 with arguments (512, buffer, end).
That genuine list initializer is the next connected transfer. The separate
shader-cache initializer at 0x60ded0 does not produce this list.

The vertex-declaration cache now supplies a reviewed four-record minimum view
for four verified native descriptors. All 24 insertion orders fit this prefix;
its complete 32-byte zero initializer matches native and preserves the old
16 bytes. The counter, getter, search and copy bodies remain unchanged. This is
a minimum accessed prefix, with original owning extent blank and zero data or
allocation credit. Full native capacity and all-startup sufficiency remain
unproved. Passing the earlier crash is an observation, not a full capacity proof.

Both runs preserve all 343 objects, 456 input pins and frozen image/report
artifacts. Wine crash suppression was set and queried; cleanup left no owned
processes, debuggers or windows. The headless debugger captured registers/stack
and then killed only its debuggee: debugger exit zero is not natural game exit.
Filter/resource lifetimes, full cache capacity, recurring frames, GPU presentation,
loaded world and gameplay remain unverified. The source build is not playable.

1. Restore the genuine material-override list initializer and its native hardware-startup call. Keep full vertex-cache capacity unresolved until original evidence establishes it.
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
