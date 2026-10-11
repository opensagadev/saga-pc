# Current work plan

The [whole-project mandate](../SOL_IMPLEMENTATION_PLAN.md) remains active.
Verified baseline EEE is [9143206](https://github.com/opensagadev/saga-pc/commit/91432069da26694dfd12a63d4d8fbb2d55e554b0), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38138778480). FFF exact-revision CI is pending publication.

Checkpoint FFF restores the genuine sound configuration, SFX table producer,
backend initialization and loading-time pad reads: 15 functions / 6,052 complete
PC bytes, including 440 newly exact code bytes. Hardware and permanent-data
call sites now invoke these real providers in native order. This addresses the
missing producer behind EEE's attributed null-SfxInfo fault in GetSfxIdN; it does
not establish that the current runtime passed that fault.

The current unchanged source executable was observed under WineDbg for 60.20
seconds with empty arguments and installed assets. A four-byte write watch on the
current source SfxInfo pointer was verified installed, but captured no write and
no fault before the deadline. The test did not capture the final wait stack, so
it does not identify where startup waited or prove sound initialization completed.
The debugger was explicitly stopped, not naturally exited. All 344 objects and
frozen source/input pins remained unchanged; owned game/debugger processes and
sampled crash/game windows were absent after cleanup. Crash-dialog suppression
was set and queried before launch.

No completed nontrivial rendering, loaded world or playable recurring loop is
established. EEE's earlier cache-write/fault and separate black-window observations
remain historical evidence, not results of this new executable. Next, attribute
the actual startup wait and recover the earliest verified missing native provider.
In parallel, close real per-frame input polling and its cache reset before
claiming responsive menus or gameplay. Preserve native behavior and matching;
never substitute a null guard, fake table, success return or forced configuration.

1. Capture the actual startup wait or first fault with the unchanged source entry,
   installed assets and bounded, automatically cleaned diagnostic runs. Reconstruct
   the earliest attributed native dependency; verify the resulting path again.
2. Close the connected frame input producer at 0x4fb130, its genuine backend,
   mouse/math/record dependencies and native per-frame cache reset at 0x6d9610.
   Loading-time ReadPads alone does not establish live input. Preserve real
   DirectInput/XInput state, frame ordering and native reset conditions.
3. Close device/resource/presentation dependencies and verify completed real frames,
   then permanent/area/world loading and player/camera activation.
4. Complete recurring updates, movement, collision, menus, audio, saves and
   transitions. Release coherent source subsets, preserve prior exact results,
   validate against retail, push authorized checkpoints and verify their exact CI.

Whole-PC fuzzy coverage is 29.732275% (+0.140523 percentage points from EEE).
Exact code is 235,450/3,471,246 bytes (+440), exact data is 637,561 bytes (+24),
and compared PC code is 1,485,614 bytes (+6,052). Mapping covers code
1,517,547/3,471,246, data 646,738/36,018,966, and combined code/data
2,164,285/39,490,212 bytes (5.480561%). Unknown original data extents earn zero
allocation or coverage credit. Similarity and mapping do not measure playability.

All 16,682 prior complete outputs were reviewed, preserving 6,274 legitimate exact
rows and 8,182 exact owner copies. The final local run passed 796 tests (three
existing skips), LLVM 18 formatting/tidy, source build/link and object validation,
and authoritative full retail matching. The immutable report describes the source
snapshot before explanatory publication edits; no source, map, tool, member or
object changes followed that comparison.

See [execution evidence](transfer-execution.md), [strategy](strategy.md) and
[runnable milestones](../docs/roadmap.md).
