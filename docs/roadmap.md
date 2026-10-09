# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified published baseline VV is [d5faf138](https://github.com/opensagadev/saga-pc/commit/d5faf1388f5e5570df2bbfceefa14761109da4aa), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37879148887) passing all 13 steps. WW exact-revision CI is pending publication.

The latest ordinary source executable exits 5 in LoadPerm1 while reading
DebMat[5]->texture_id through a NULL material pointer. The preceding genuine
DebrisSetup call at native 48a6f8 is still omitted. Its native wrapper at 683530
and the existing DebrisSetup2 material/resource producers are the next connected
matching targets. No replacement material or direct DebMat assignment is added.

The WW batch compares 27 new complete functions / 8,078 PC bytes, fourteen exact,
and restores four genuine frame/render calls. All 20 observed draw callbacks and
the material/shader-parameter chain have source. The parents remain partial;
all previous exact contracts survive. One ordinary source-game run confirms the
NULL material fault above, without establishing a successful frame or gameplay.
Code similarity and mapping are separate from runnable milestones; current
measurements live in [matching.json](../matching.json), with full qualifications
in [transfer-checkpoint.json](../config/transfer-checkpoint.json).

Work proceeds through connected source families and verified PC contracts:

1. Close genuine debris/material startup and the remaining renderer providers, while
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
