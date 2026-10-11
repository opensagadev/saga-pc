# Reconstruction roadmap

Checkpoint III transfers 68 additional functions from connected Android source
families, covering **17,037 original PC bytes**. These include AI actions and
providers, player combat, matrix/vector/half conversion, rectangle rendering and
pause resumption. Thirty-eight new functions match **4,138 bytes** exactly.
Seven real movement calls and the after-animation laser path are restored.

Startup now uses three real PC text tables and the complete reconstructed
Text_LoadStrings tail. All **9,760 emitted table bytes** and their pointer
relocations match native memory. The tables reuse 46 canonical ID definitions
and provide 669 new IDs. Unknown owning extents receive no allocation credit.
The loader's old boundary ended inside an instruction; its complete PC extent
is corrected from 397 to 403 bytes, accounting for six additional compared bytes.

Whole-PC fuzzy coverage is **30.428346%**
(+0.374951 percentage points over HHH).
Exact code is **241,489/3,471,246 bytes**;
compared PC code is **1,516,024 bytes**.
No previous exact function or data match was lost. Dedicated text-table ownership
preserves canonical data definitions. Two older nonexact central regions change
only in emitted zero padding; all their typed values and full initializers remain
native-equal. Unchanged-source compiler scheduling differences were reviewed.

Local validation passes: 797 tests (three existing skips), LLVM 18 formatting
and static analysis, source EXE build/link, 350-object checks and full retail
matching. HHH revision `a774cd3` passed
[CI](https://github.com/opensagadev/saga-pc/actions/runs/38147589059).
III's exact-revision CI is verified after its direct push to main.

A bounded ordinary run with installed assets in an isolated Wine prefix held a
visible game window for about 30 seconds; the captured image remains entirely
black. No game fault was logged, but Mesa reported EGL/driver warnings. The run
ended at its deadline and left no prefix-owned processes. This does not prove
completed rendering, a loaded world or playability. The source is **not playable**.

Continue bulk source transfer through named Android callers and existing PC
anchors. The retained direct-call frontier contains 56,502 bytes of candidate
spans; those are research only. The next connected target is the AI connection
controller family with its genuine fake-animation producer, followed by the
remaining player combat dependencies. For runtime, attribute the earliest
startup wait and close actual permanent/world-loading providers. Avoid register
and instruction-order tuning unless comparison reveals a behavior/type error.

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
