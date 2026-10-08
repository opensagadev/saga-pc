# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
The latest published source checkpoint is II,
[`9c78272c5847aef790e82320b0a2401c20906c52`](https://github.com/opensagadev/saga-pc/commit/9c78272c5847aef790e82320b0a2401c20906c52),
with [exact-revision CI passing](https://github.com/opensagadev/saga-pc/actions/runs/37710401246).
The current JJ source wave integrates pushing, ledges, teleport, title/credits
callbacks and geometry readers: 23 functions/16,291 PC bytes, with required local
checks and 156 bounded CPU checks passing. Its complete qualifications are in
[transfer-checkpoint.json](transfer-checkpoint.json). II remains the last full-game
runtime observation. Later shadow proposals remain candidates. Use [matching.json](../matching.json) for current comparisons.

1. Close genuine shader-manager publication, parser/compiler lifetime and device
   dependencies at the observed startup frontier. Preserve true constructors,
   callbacks and returned values; do not force a pointer or skip initialization.
2. Transfer connected gameplay, movement, resource and loader families in parallel
   where their PC contracts are established. Reattach real providers to partial
   callers and record remaining void omissions.
3. Integrate shared named layouts, mappings and data once for each coherent wave.
   Preserve every prior exact contract and investigate changed complete outputs.
4. Verify actual source/object/report agreement, run required checks and meaningful
   bounded runtime tests, then publish a coherent checkpoint. Verify CI on its
   exact pushed revision before using it as the next baseline.

Keep source, storage, ABI, native-boundary and runtime claims distinct. A working
access view does not prove allocation size; an accepted source packet does not
prove asset loading, device presentation or gameplay. One held family must not
stop independent reviewed source from progressing.

Use [strategy.md](strategy.md) for family selection,
[identification-plan.md](identification-plan.md) for mapping work and
[transfer-execution.md](transfer-execution.md) for ownership, validation and runtime
policy. Current runnable milestones are in [the roadmap](../docs/roadmap.md).
The complete earlier [execution plan and release history](../docs/history/progress-plan-through-2026-10-03.md)
remain available without serving as today's task queue.
