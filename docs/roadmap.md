# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified published baseline XX is [d1695b6](https://github.com/opensagadev/saga-pc/commit/d1695b6f9004d105f4fbf500dc9bdc069a93aa4b), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37885753616) passing all 13 steps. YY exact-revision CI is pending publication.

The latest ordinary source-game run exits 5 inside the existing exact 84-byte
PCByteCopy. A separate headless debugger run on the frozen XX source image
identifies the caller as NuGetVertexDeclaration: it requested 6,863,024 bytes.
The getter's full 1,660 compiled bytes equal that linked source image; its native
comparison extent remains the complete 1,684 bytes. The reconstructed cache has
two records, with its count at the third record's key address. Verified native
material producers require at least three distinct records, conditionally four.
The observed count matches a genuine glass descriptor, supporting the specific
count-overwrite mechanism; the earlier write itself was not observed.

Native cache stride and the accessed records are verified, but its original full
capacity remains unproved. Retained PC/Mac/Android evidence does not establish
that extent. Cache storage is unchanged in YY; no guessed capacity, count reset,
relocation, copy guard or replacement producer is added. The earlier NULL
DebMat[5] failure did not recur after genuine debris setup was restored.

YY adds 12 functions / 988 compared PC bytes, six exact / 174 bytes. Post-effect
setters and the real sampler provider have source; filter construction and
resource lifetimes remain held. XX restores genuine debris startup and has
passing exact-revision CI. Existing parents remain partial; all prior exact
contracts survive. Similarity and mapping are separate from runnable milestones;
measurements are in [matching.json](../matching.json), with full qualifications in
[transfer-checkpoint.json](../config/transfer-checkpoint.json).

Work proceeds through connected source families and verified PC contracts:

1. Close genuine cache storage, resource loading and the remaining renderer providers, while
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
