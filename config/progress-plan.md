# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline VV is [d5faf138](https://github.com/opensagadev/saga-pc/commit/d5faf1388f5e5570df2bbfceefa14761109da4aa), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37879148887) passing all 13 steps. WW exact-revision CI is pending publication.

The WW batch adds 27 complete functions / 8,078 unique compared PC bytes across
material/shader parameters, display-list drawing, back-buffer/resource suppliers
and native presentation. Fourteen are byte exact, adding 463 exact code bytes.
Whole-PC fuzzy coverage is 29.474249%, up
0.078212 percentage points from VV; exact code is
233,469/3,471,246 bytes (6.725798%). Every previous exact code/data result
and all 13 named member rows survive.

All 20 observed display callbacks now have compiled source. All 31 new complete
initializers (840 typed bytes) equal native, including the 49-entry handler view
and its pointer: 27 NULL entries and 22 pointers to 20 distinct real callbacks.
Unknown owning extents remain blank and receive zero allocation or exact-data
credit. The old unimplemented DrawScene extent is corrected from 209 to 210 bytes:
209 cut off the final byte of its terminal jump. This is a boundary correction,
not additional transferred source.

Four genuine calls are restored: renderer DrawScene and native presentation,
then frame-end TimeBar(-1) and SwapScreenEx with the current callback. These
existing parents add zero new compared ranges; renderer 1,211/native 1,465 and
frame end 365/native 987 remain partial and nonexact. No separate scene counter
reset, enlarged array, forced focus/profiling or fabricated success is added.

Required checks pass: 796 tests (three existing skips), LLVM18 formatting and
342-unit analysis, normal source link/object checks and full retail matching.
Among 16,514 old complete outputs, 16,509 retain identical relocated bytes; two
parents change through the four reviewed calls, and three unchanged-source
Camera/Deb/Init outputs retain specifically reviewed historical finite pairs.
The 89 natural unused SDK outputs preserve their contracts and pass fresh
normal-link/archive admission with zero PC credit.

The latest ordinary source executable exits 5 in LoadPerm1 while reading
DebMat[5]->texture_id through a NULL material pointer. The preceding genuine
DebrisSetup call at native 48a6f8 is still omitted. Its native wrapper at 683530
and the existing DebrisSetup2 material/resource producers are the next connected
matching targets. No replacement material or direct DebMat assignment is added.

One actual source-game run exited naturally after 2.546 seconds. Its complete
781-byte LoadPerm1 body is verified against the linked source image; fault
source453f85/native48a7e7 reads WORD [EDX+74] with NULL DebMat[5]. All 342 normal
objects and 455 frozen inputs remained unchanged. Wine crash suppression was set
and queried, and owned process/window cleanup was empty. This is a failed startup
observation, not a passing frame, presentation, assets or gameplay test. The source
game remains not playable. Earlier CPU fixture results retain their original
bounded domains; they do not test the new material or live renderer paths.

1. Reconnect genuine DebrisSetup and its material/resource producers at the observed startup frontier.
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
