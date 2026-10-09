# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The current source
build is **not playable**. Published baseline UU
[`ea790699`](https://github.com/opensagadev/saga-pc/commit/ea790699cb68bc616345d52f874124aba5093cd6)
has [passing exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37876166258).
Genuine window/input/renderer startup and shader publication are present. The
latest retained source-game run exits while packaging a scene: the exact native
copy overwrites its initialized display-list pointer at index 32. The genuine
renderer drains scenes and resets their count; its frame caller remains omitted
while the material and shader-parameter chain is reconstructed. Original scene
capacity remains unknown.

The current VV batch compares 20 new full PC functions / 3,481 bytes; twelve are
exact. It also restores seven verified calls in the partial renderer, with zero
additional range credit. Nineteen of 20 observed callbacks have compiled source.
The next connected family is the last material handler, the complete shader
parameter dispatcher and real back-buffer/resource providers, followed by native
scene/table/frame flow. All previous exact results are preserved.

The actual-object CPU fixture passes 86 checks. Its selected matrix, shader and
cache/guard domains remain qualified separately from graphics and gameplay.
These checks establish neither a persistent window/device presentation nor
recurring gameplay. Code similarity and mapping are separate from runnable
milestones; current measurements live in [matching.json](../matching.json).
Full release qualifications are recorded in
[transfer-checkpoint.json](../config/transfer-checkpoint.json).

Work proceeds through connected source families and verified PC contracts:

1. Close the genuine renderer, display-list handlers and frame caller path, while
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
