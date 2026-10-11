# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified source baseline DDD is [be647b8](https://github.com/opensagadev/saga-pc/commit/be647b859b259502f46569294b0cff7680ab7703), with [all 13 CI steps passing](https://github.com/opensagadev/saga-pc/actions/runs/38136446273). EEE exact-revision CI will be checked after publication.

The rebuilt source was tested with empty arguments and installed assets, first
under a correctly installed four-byte WineDbg write watch, then normally for
60 seconds. The debugger captured genuine declaration-count writes 1 and 2;
at a later fault the count was 13 and the cache contained real records. This run
passed the formerly observed four-record storage failure. It does not prove the
original cache capacity or establish a bound for every startup/gameplay path.

The new attributed fault is source `0x005347d5`, inside exact `GetSfxIdN`
(native `0x650e40`, 77 complete bytes), reading null `SfxInfo`. The recorded return
chain passes through particle loading. Current source omits the preceding native
sound configuration provider at `0x6521b0`, and never assigns `SfxInfo`.
Recovering that real producer and its consumed dependencies is the next priority;
a null guard or dummy table would hide the missing reconstruction.

The separate normal run reached its 60-second deadline without a logged source
fault; its PID-attributed window remained black in samples at 5.79, 21.44 and
55.81 seconds. Debugger timing changes scheduling, so these are distinct results.
No completed nontrivial rendering, world activation or playable game is established.
Both runs preserve all frozen inputs and objects, set and query crash-dialog
suppression, and leave no owned game/debugger process or sampled game/crash window.
The debugger was explicitly killed/quit after capture; the normal run was stopped
at its deadline. Neither is reported as a natural successful game exit.

Reconstruction follows the measured executable frontier: fix the earliest
attributed native failure or missing consumed provider, then rerun the unchanged
entry path against installed assets. Device/window invocation, completed presentation,
world activation and playable recurring updates are separate milestones. Prepare
connected independent source families in parallel where they help these milestones.
Do not introduce success returns, assets, vtables, guards or graphics workarounds.

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
