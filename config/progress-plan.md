# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified published baseline TT is [1bbe21d4](https://github.com/opensagadev/saga-pc/commit/1bbe21d43f3cc25b9b41b4b396a6ddb37bca5308), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37872016080) passing all 13 steps. UU exact-revision CI is pending publication.

The current UU batch adds 16 complete PC functions / 2,278 unique compared bytes:
nine display-list callbacks, the vector helper and six shader providers. Five are
exact, adding 194 code bytes; whole-PC fuzzy coverage is 29.306557%, up 0.054455
percentage points. All prior exact code/data and 13 named members survive. Four
canonical state views use 28 minimum bytes; unknown owning extents receive zero
allocation or exact-data credit.

The actual-normal CPU run passes 43 checks through the real shader manager and
compiler. The retained first run stopped at 16 checks with one width failure:
PC semantic 0x49 is ps2ShineMtx, a four-vector matrix at packet +0x70. The reader
now names NUMTX and requires an initialized minimum 0xb0-byte view; its producer
and original owning extent remain unproved. No uniform metadata or guessed
padding was changed. The genuine signed32 camera local now emits native's
conversion helper; its complete body remains nonexact and positive camera/device
calls are excluded from CPU tests. Wine crash dialogs were set and queried off;
owned cleanup is empty. These checks establish no device or gameplay milestone.

Ten of 20 observed callbacks still lack compiled source. The next source wave is
BC transforms/indexed handlers, geometry/debris and their real device suppliers,
followed by material handling and the complete 4,472-byte shader-parameter
dispatcher. Preserve private helpers with their real caller cohort and keep the
old C owner unchanged. The renderer/table/frame caller remains held until these
consumed providers exist. Original array capacity is unproved; no standalone
scene-count reset, forced focus, fake provider or enlarged storage is introduced.

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
