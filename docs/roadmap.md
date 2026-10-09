# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified published baseline WW is [93b3548](https://github.com/opensagadev/saga-pc/commit/93b3548f5fee4ab8e4fcf58918aa915575e8875a), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37883640294) passing all 13 steps. XX exact-revision CI is pending publication.

The latest ordinary source executable exits 5 in the backward-copy read of
PCByteCopy. Its complete 84-byte source body equals retail. Fault source5160a0,
native4ef6a0, reads BYTE [ESI-1] at invalid address02def4bf. This run does not
establish the caller or the copy arguments; archive/resource producers are the
next investigation. The earlier NULL DebMat[5] failure did not recur after
restoring genuine DebrisSetup. No guard, replacement material or invented
successful result is added.

The XX batch restores genuine debris startup: one new complete function / 43 PC
bytes, all exact, and the native call to existing resource/material providers.
The earlier WW batch added 27 functions / 8,078 PC bytes and restored four genuine
frame/render calls; it now has passing exact-revision CI. Existing parents remain
partial and all prior exact contracts survive. Code similarity and mapping are
separate from runnable milestones; current measurements live in
[matching.json](../matching.json), with full qualifications in
[transfer-checkpoint.json](../config/transfer-checkpoint.json).

Work proceeds through connected source families and verified PC contracts:

1. Close genuine archive/resource loading and the remaining renderer providers, while
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
