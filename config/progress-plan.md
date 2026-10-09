# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline AAA is [2825bf6](https://github.com/opensagadev/saga-pc/commit/2825bf60e08aa25797794f2d71d091d3d57ebf97), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37890834273) passing all 13 steps. BBB exact-revision CI will be checked after publication.

Checkpoint BBB transfers five complete startup functions / 436 newly compared
PC bytes. API initialization (112), wind initialization (57) and the real
particle vertex-buffer helper (108) are byte exact, adding 277 exact bytes.
Animation initialization (89) and particle arena initialization (70) retain
complete nonexact comparisons and independently reviewed source contracts.
The animation producer reserves the actual 16-byte header, 48-byte joints and
per-joint flags; old reader names and offsets remain unchanged, and header bytes
6/7 remain untouched. Native startup passes animation_count 160, not the video
parameter suggested by the old omission comment.

Seven real caller sites are restored: four hardware startup calls and three
sampler/post-effect reset calls in SwapScreen. The existing real buffer creators,
lockers, initial swap, callbacks and returned values are retained. No filter
instances, vtables, allocator providers, capacity guesses or success values are
fabricated. Both parents remain partial/nonexact; 16 SwapScreen omissions remain.

Whole-PC fuzzy coverage is 29.513112%, up 0.012891
percentage points. Exact code is 234,045/3,471,246 bytes (6.742392%),
up 277; exact data remains 637,833 bytes. All 2,448 previous exact code copies,
5,709 data copies and 13 named members survive. Of 16,625 old complete outputs,
16,619 are byte identical. The six changes are the two intended callers, two
specific independently reviewed nonexact Init/Particle pairs, and two complete
unknown-extent data-tail pairs. The declared shader array remains 157,210 bytes;
its full compiled region now includes two alignment zeros, which differ from
native bytes and remain explicitly nonexact with zero extent credit. No native
body or data mismatch is cropped or masked.

Two shared DWORDs are added in ordinary default sections; the private API default
stays in its original source owner. All three owning extents remain blank, with
zero new data credit. Required checks pass: LLVM18 format/tidy for 343 owners,
796 tests (three existing skips), normal source link/object coverage and full
retail matching. An initial system-LLVM22 formatting disagreement and two Wine
compiler timeouts are retained; unchanged-source checks with installed LLVM18
and the unchanged pinned build retry pass. All 89 natural SDK outputs retain
their descriptors, with refreshed header recipe pins and complete link proof.

The latest frozen source run remained active until its 60-second deadline.
The launcher then terminated it; natural exit was not observed. No fault or
exception was recorded, and no native game or crash window was attributed by
the broad window samples. This does not prove recurring frames, a loaded world,
playability, or that execution passed the earlier declaration failure.

The last attributed failure, in AAA, was exact PCVertexDeclarationFindSlot
receiving count 0x0200090a with descriptor 0x02000909. The source count occupied
cache+32, the key position of a hypothetical fifth record; insertion overwrite
was the leading explanation, with the preceding write unobserved. Native places
its counter eight bytes before its cache. Full original capacity remains unknown;
the earlier four-record view establishes only a minimum for a finite producer
domain. BBB changes no cache storage, counter, search, copy or guard. Retained Mac
catalogs contain no debug array declaration proving this PC allocation.

All 343 objects, 456 input pins and frozen report/image artifacts remain unchanged
during the observation. Crash-dialog suppression was set and queried; cleanup
left no owned processes, debuggers or windows. The 60.434 seconds include cleanup;
launcher exit -15 is deadline termination, not natural game exit. The source is
not playable. Device/resource lifetimes, persistent frames and world/gameplay
remain unresolved. Next is the connected occlusion720 and material/input965
source proposal; its 1,685 candidate bytes receive no reconstruction credit yet.

1. Transfer the connected occlusion720 and material/input965 families with real
   data owners and providers, then compile and compare all 1,685 candidate bytes.
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
