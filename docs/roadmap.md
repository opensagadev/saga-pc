# Reconstruction roadmap

The source build is **not playable**.

Checkpoint HHH restores real save-folder registration and six-slot enumeration,
legal texture/material loading and the legal quad's fallback display-list path.
It also transfers the AI locator-action family and eleven matrix routines from
Android source with PC-specific contracts. The 22 new functions cover **4,486 PC
bytes**; fourteen match **1,208 bytes** exactly. The pointer-before-allocation
correction in `NuDisplayListLinkMtl` has an original-free regression that also
rejects the former buggy ordering.

Whole-PC fuzzy coverage is **30.053396%**
(+0.121316 percentage points).
Exact code is **237,351/3,471,246 bytes**;
compared PC code is **1,498,981 bytes**. No previous exact
function was lost. Three exact four-byte globals migrated to verified named
members of the canonical fallback list, preserving their physical coverage.
Full old central-data initializers are unchanged, including unknown-size regions.
Unknown owning extents still earn no allocation credit.

Local validation passes: 797 tests (three existing skips), LLVM 18 formatting
and static analysis, source EXE build/link, 347-object coverage checks and full
retail matching. The starting revision `56ec513` has passing
[CI](https://github.com/opensagadev/saga-pc/actions/runs/38144893214);
HHH's exact-revision CI is checked after publication.

A bounded ordinary run in an isolated Wine prefix held the game window for
30.30 seconds. The 5- and 15-second window captures are entirely black. The
30-second screenshot command failed; no frame or gameplay is inferred from it.
No game fault was logged, but Mesa reported EGL/driver initialization warnings.
Termination was by the deadline, not natural exit. Crash-dialog suppression was
set and queried; no prefix-owned processes remained after cleanup. This run does
not establish responsiveness, completed rendering, a loaded world or playability.

The user resumed work and prioritized **bulk Android source transfer**. The next
wave uses named Android callers and existing PC anchors to expose unmapped
callee families, then transfers whole source families with shared field/data
contracts. Current drafts cover AI actions, combat/force code, projection/vector
helpers and startup text-table backing. Candidate graph spans are research only;
only complete reviewed, compiled and compared bodies count as progress. Avoid
instruction scheduling work unless it reveals a behavior/type error.

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
