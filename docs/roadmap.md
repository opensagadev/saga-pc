# Reconstruction roadmap

The goal is a complete PC reconstruction from readable C/C++. The source build
is **not playable**. Verified published baseline BBB is [c1e87df](https://github.com/opensagadev/saga-pc/commit/c1e87dfe7901cb4b383e9b3a4465a51a28e9f2be), with [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37892772484) passing all 13 steps. CCC exact-revision CI will be checked after publication.

The latest frozen source run remained active until its 60-second deadline.
The launcher then terminated it; natural exit was not observed. No fault or
exception was recorded, and no native game or crash window was attributed by
the broad window samples. This does not prove recurring frames, a loaded world,
playability, or that execution passed the earlier declaration failure.

The last attributed failure, in AAA, was exact PCVertexDeclarationFindSlot
receiving count 0x0200090a with descriptor 0x02000909. The source count occupied
cache+32, the key position of a hypothetical fifth record; insertion overwrite
was the leading explanation, with the preceding write unobserved. Native places
its counter eight bytes before its cache. Full original capacity remains unknown;
the earlier four-record view establishes only a minimum for a finite producer
domain. BBB changes no cache storage, counter, search, copy or guard. Retained Mac
catalogs contain no debug array declaration proving this PC allocation.

CCC adds nine functions / 1,685 compared PC bytes, five exact / 203 bytes.
Its source game has not been rerun; the observation above belongs to BBB.
Similarity and mapping measure reconstruction, separately from runnable
milestones. See [matching.json](../matching.json) and
[checkpoint evidence](../config/transfer-checkpoint.json).

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
