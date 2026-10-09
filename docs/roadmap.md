# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified published baseline XX is [d1695b6](https://github.com/opensagadev/saga-pc/commit/d1695b6f9004d105f4fbf500dc9bdc069a93aa4b), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37885753616) passing all 13 steps. YY exact-revision CI is still pending. ZZ exact-revision CI will be checked after publication.

The latest ordinary source run passed the earlier vertex-cache copy failure and
exited 5 after 7.879 seconds at a new NULL-list read. A separate headless capture
proves NuMtlDestroy passes NULL g_overrideList to NuLstGetNext. That complete
40-byte list function is byte exact; its NULSTHDR.head read at +8 faults. The
canonical list pointer has no producer in current source. Hardware startup still
omits native NuMtlInitOverride at 0x5e4eb0 with arguments (512, buffer, end).
That genuine list initializer is the next connected transfer. The separate
shader-cache initializer at 0x60ded0 does not produce this list.

The vertex-declaration cache now supplies a reviewed four-record minimum view
for four verified native descriptors. All 24 insertion orders fit this prefix;
its complete 32-byte zero initializer matches native and preserves the old
16 bytes. The counter, getter, search and copy bodies remain unchanged. This is
a minimum accessed prefix, with original owning extent blank and zero data or
allocation credit. Full native capacity and all-startup sufficiency remain
unproved. Passing the earlier crash is an observation, not a full capacity proof.

ZZ adds six methods / 219 compared PC bytes, four exact / 47 bytes. All previous
exact results survive, including the C-to-C++ post-filter owner migration.
Similarity and mapping are separate from runnable milestones; measurements are
in [matching.json](../matching.json), with complete qualifications in
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
