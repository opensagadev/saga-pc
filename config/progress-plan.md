# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline UU is [ea790699](https://github.com/opensagadev/saga-pc/commit/ea790699cb68bc616345d52f874124aba5093cd6), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37876166258) passing all 13 steps. VV exact-revision CI is pending publication.

The VV source batch adds 20 complete PC functions / 3,481 unique compared bytes:
transform, geometry, indexed-draw and debris handlers, eight real device methods,
the shader-binding provider and the private Z-mode helper. Twelve are byte exact,
adding 755 exact code bytes. Whole-PC fuzzy coverage is 29.396037%, up 0.089480
percentage points from UU. Every previous exact function/data copy and all 13
named member rows remain intact. Nine canonical four-byte state views have native
initializers; their unknown owning extents remain blank and earn no data credit.

Seven native stream/shader calls are restored in the existing renderer. Its full
1,465-byte native extent is compared against the 1,169-byte candidate; it remains
partial and nonexact. This restoration adds no new compared range. Nineteen of
the 20 observed draw callbacks now have compiled source. The remaining material
handler needs the genuine 4,472-byte shader-parameter dispatcher and its providers.
DrawScene, DrawItems, handler-table publication and frame/hardware caller activation
remain held until that connected chain is available. No independent scene-count
reset or guessed scene-array enlargement has been added.

The frozen normal-object CPU fixture passes 86 checks with zero failures, using
all 342 genuine engine objects and no replacements. It verifies complete matrix
cache results/restoration, real shader/uniform helpers and selected cache-hit,
unbound, NULL and zero-count paths. Final output preservation keeps those selected
CPU results applicable after the seven renderer changes; the renderer itself was
not exercised. Wine crash dialogs were disabled and queried, the process exited
naturally, and owned cleanup was empty. Native vector-tail reads in other paths
remain qualified and untested. There is no device, GPU, presentation, installed
asset or gameplay claim. The source game remains not playable.

1. Reconnect the genuine render and message flow at the observed startup frontier.
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

The latest ordinary source run naturally exits 5 in display-list scene packaging.
A bounded debugger watch proves that initialization sets the first-list pointer,
then the exact NuRndrEndScene scene copy overwrites it at index 32. The omitted
native renderer resets the scene count after processing its scene loops. Original
array capacity is unproved; it must not be inferred from the fault. CPU checks and
matching do not establish device presentation or gameplay. Use
[matching.json](../matching.json) for comparisons.

Use [strategy.md](strategy.md), [identification-plan.md](identification-plan.md)
and [transfer-execution.md](transfer-execution.md) for selection and validation.
Current runnable milestones are in [the roadmap](../docs/roadmap.md).
The earlier [execution plan and release history](../docs/history/progress-plan-through-2026-10-03.md)
remain available without serving as today's task queue.
