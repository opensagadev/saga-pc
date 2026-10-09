# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified published baseline ZZ is [822c1c5](https://github.com/opensagadev/saga-pc/commit/822c1c5b13a24633b71bccf9209120d2f5c8b15f), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37889168457) passing all 13 steps. YY also passed all 13 steps. AAA exact-revision CI will be checked after publication.

The genuine material-override initializer NuMtlInitOverride at 0x5e4eb0 now
runs in the native hardware-startup order, with arguments (512, buffer, end).
Its complete 35 bytes match native. The existing real list allocator retains
its native arena-size check and failure behavior. No artificial list or backing
provider is added. The earlier NULL-list fault no longer appears.

The latest ordinary source run exits 5 after 54.710 seconds in the byte-exact
82-byte PCVertexDeclarationFindSlot. A separate headless capture proves that
NuGetVertexDeclaration loads count 33,556,746 (0x0200090a) from the canonical
counter, with descriptor 0x02000909. In our layout that counter occupies cache+32,
the key position of a hypothetical fifth record. An insertion overwriting the
counter is the leading explanation; the preceding write itself was not captured.
The native counter precedes its cache by eight bytes. Full original cache
capacity remains unproved. The four-record prefix is a verified minimum for the
earlier finite producer domain, not sufficient storage for every startup input.
Do not grow the array, move the counter, reset counts or add search/copy guards
without recovering the actual storage contract.

AAA adds 35 compared PC bytes, all exact. Similarity and mapping measure source
reconstruction, separately from runnable milestones. See [matching.json](../matching.json)
and [checkpoint evidence](../config/transfer-checkpoint.json).

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
