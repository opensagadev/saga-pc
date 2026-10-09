# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The current source
build is **not playable**. Published checkpoint SS
[`2f3c7fdb`](https://github.com/opensagadev/saga-pc/commit/2f3c7fdb38717759ab47bf35e3fef6bde895f833)
has [passing CI](https://github.com/opensagadev/saga-pc/actions/runs/37866549493).
Genuine shader publication is present. The latest source-game observation exits
at a display-list pointer overwritten by the scene copy at index 32. The native
renderer resets the count after processing scenes; its caller connection remains
held while missing draw handlers are reconstructed. A transient window and bounded CPU tests establish neither persistent
window/device presentation nor recurring gameplay. Code similarity and mapping
are separate from runnable milestones; current measurements live in
[matching.json](../matching.json).

The current frame/texture source batch compares 13 new full function extents /
2,089 PC bytes, with nine exact functions. It preserves previous exact matches.
Nineteen draw callbacks still lack compiled source; the first nine plus their
vector helper and a connected shader-provider family are prepared for subsequent
integration. Original scene capacity remains unknown. Matching and the retained
208 shader CPU assertions do not establish loaded assets, device presentation or
gameplay. Full release qualifications are recorded in transfer-checkpoint.json.

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
