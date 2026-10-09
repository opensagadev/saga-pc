# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified baseline SS is [2f3c7fdb](https://github.com/opensagadev/saga-pc/commit/2f3c7fdb38717759ab47bf35e3fef6bde895f833), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37866549493) passing all 13 steps.
Published SS is [2f3c7fdb](https://github.com/opensagadev/saga-pc/commit/2f3c7fdb38717759ab47bf35e3fef6bde895f833): 52 newly compared complete functions/6,422 unique PC bytes, including real shader publication and compiler/parser lifetime. Required local checks and 208 bounded shader CPU checks pass; every RR exact contract survives. Whole-PC fuzzy coverage is 29.202954%, with 231,692 exact code bytes. [SS exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37866549493) passed all 13 steps. Full qualifications and retained failures are in transfer-checkpoint.json.

The current TT frame/texture batch compares 13 new complete functions / 2,089
PC bytes. Nine are exact (+365 code bytes); whole-PC fuzzy coverage is 29.252102%
(+0.049148 percentage points). All SS exact code/data survives, with 12 new exact
data bytes. The real renderer is partial and its callers/table remain held.
NuFrameEnd now includes the native Windows message-pump gate. The next wave is
nine real draw callbacks plus NuVec4Add / 1,361 bytes, six shader providers / 917
bytes and the remaining draw callbacks. Nineteen callbacks lack compiled source.
The corrected lighting proposal matches native MAX/tie/unordered control but is
not yet compiled or tested. Original array capacity remains unknown. Terrain registration,
GameAudio/Menu IDs and the reviewed gameplay backlog remain connected later work;
full Bolt backing, CharFixUp and resource/device lifetime remain separate.

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
