# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified baseline EEE is [9143206](https://github.com/opensagadev/saga-pc/commit/91432069da26694dfd12a63d4d8fbb2d55e554b0), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38138778480). FFF exact-revision CI is pending publication.

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

Work proceeds through connected source families and verified PC contracts:

1. Attribute the actual startup wait and repair the earliest verified native gap.
   Close genuine cache storage, resource loading and the remaining renderer providers, while
   preserving real shader publication and its constructor/parser/compiler. Verify real startup state and a persistent
   native window/input path without fabricated providers or initialization bypasses.
2. Close device/reset/resource lifetimes and presentation. Verify repeated real
   frames and nontrivial pixels from installed assets, keeping CPU and GPU evidence
   separate.
3. Complete permanent, area and world loaders, scene/material/model fixups and
   character/callback activation. Verify a populated world/player/camera from real
   input files and producers.
4. Complete the recurring update/render loop, movement, collision, menus, audio,
   saves and transitions. Exercise a playable installed level and then the remaining
   game content; an isolated level or transfer checkpoint does not finish the goal.
5. Continue complete-byte matching alongside functional reconstruction. Preserve
   prior exact results and resolve PC-specific behavior, ABI and layout differences
   before treating source similarity as correctness.

Release reviewed subsets continuously; a missing family does not block independent
correct source. Partial actions and runtime limitations remain explicit. Android
is source evidence, while PC callers, fields, native boundaries and behavior
establish the contracts.

[CONTRIBUTING.md](../CONTRIBUTING.md) covers setup and validation.
The [current work plan](../config/progress-plan.md) and
[execution policy](../config/transfer-execution.md) guide development.
The original [implementation handoff](development/implementation-plan.md) and
[historical evidence index](history/README.md) retain the full earlier plans and
checkpoint records.
