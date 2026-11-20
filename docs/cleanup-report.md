# Cleanup report

This report covers the readability cleanup that ran from 2026-11-16 to 2026-11-20. Its goal was source that reads like hand-written code and still compiles byte-identical to retail.

Two numbers frame it:

- **Start:** main 5f61bf2c4, the last commit before 2026-11-16.
- **End:** main d5f2612a3: run 3 merged at 429b623e0, then the tuning pass.

## How it ran

1. **Proven passes, 2026-11-16 (up to 5717c76e).** Every change was proven object-identical before it landed. These passes covered:
   - data.cpp sizes and section pragmas;
   - self-contained headers;
   - duplicate types;
   - forward declarations;
   - magic values;
   - layout asserts turned into checked offset comments;
   - removing `clang-format off`;
   - string inlining.
2. **Quick runs 1–3** (branches cleanup/quick-run, -2, -3; one worker per module). Each run fixed the open list fast, recorded the match without blocking on it, then refixed every dropped function and merged into main:
   - **Run 1:** merged at 922b11901, refixed through b625d3233.
   - **Run 2:** merged at 974f6e0bc.
   - **Run 3:** merged at 429b623e0.
   - **Where a refix needed the old form,** that single site carries `// MATCH-EXCEPTION: <reason>`. There are three: SetPodMergeAnims, ConfigureJoystickRanges and the gameapi ClassObject accessor.
   - **Budgets:** every budget sits at its real count. Any class still above its start value has a reason line in config/budget-raises.txt.

## Matching

| | Start (5f61bf2c4) | End (d5f2612a3) |
|---|---:|---:|
| Functions exact (matching.json) | 13,530 | 13,531 |
| Functions exact (proof) | 13,516 at 5717c76e | 13,517 |
| Data symbols exact (matching.json) | 17,177 | 17,123 |

The data count went down on 2026-11-16 in the data.cpp honest-sizes pass, before any quick run. Padding is not scored, by the owner's decision. Every quick run kept data unchanged at 17,063 in proof terms.

The fragile functions flip with unrelated changes, such as register allocation or the set of type names. docs/fragile-flips.md lists each one with its state. After the tuning pass:

- BoulderSpawn and DebrisSetup2 are exact.
- BlockadeRunnerDEjectCreature is exact again (d2c1505a6, a local copy of the parameter).
- BoltTypes_Configure sits at 91 %.
- SubItemMenu and do_maya_mouse_camera were never exact.
- Not tried yet: NuRndrParticleGroup, InitGameAfterConfig, DrawSubItems, NuDeferredFilterGen::render, RenderSceneShadow.

The tuning pass also found that BoltTypes_Configure compiled differently between clean rebuilds of unchanged source (99.88 %, then 96.99 %, then 91.07 %). The worktree path was ruled out; the shared wine prefix or wineserver state is suspect. So some rows of docs/fragile-flips.md may record this effect rather than a source change. Making the build environment deterministic is open work.

## Counts per row, start → end

The counts come from the scans' budget files, which sit at their real counts.

The "Kind" column says how each row should be finished:
- **mechanical:** can be finished by scripts and readable edits;
- **needs evidence:** needs a reason found in the code or the game data;
- **kept by design:** what is left stays on purpose.

| Row | Start | End | What is left, and why | Kind |
|---|---:|---:|---|---|
| `(void)name;` statements | 1,249 | 3 | Three reads of an `extern volatile` vblank counter | kept by design |
| Unit `#define` / `#undef` | 384 / 162 | 20 / 14 | Config defines before includes, MATCH-EXCEPTION macros, Win32 name-clash push/pop pairs, vorbis static names | kept by design |
| Declarations of other units' functions (declscan D) | 1,149 | 29 | Diagnostics in units without headers, declarations whose type differs from the definition | needs evidence |
| Re-declarations (declscan A) | 22 | 21 | Removable with `declscan --fix` | mechanical |
| Local externs | 51 | 1 | One left | mechanical |
| Duplicate types | 365 | 16 | FadeBase (C/C++ split), SDK forward declarations, cross-module API/game layering | needs evidence |
| View types / view uses | 177 / 693 | 9 / 41 | NU_POST_*_VIEW (seen by C code), PC_SHADER_SLOT/RECORD_VIEW, a few overlays whose size differs from the record. Also, `field_of_view` is a false positive | kept by design |
| Placeholder fields (unk_/pad_/field_0xNN) | 8,420 | 1,054 | Every field that code reads or writes has a name. The rest are gaps nothing touches | needs evidence |
| Address names, fun_/dat_ (src + symbols.csv) | 16,127 | 4,913 | About 300 empty or constant stubs, globals nobody reads, and about 624 text-table ids that only the game's text file can name | needs evidence |
| Old address-style names (hex-name) | 536 | 121 | Mostly placeholder-like fields and false positives (Read16) | needs evidence |
| Casts, all classes | 23,201 | 17,472 | Mostly steering, idiomatic and float/int classes | kept by design |
| Casts: declaration-type | 1,548 | 725 | 269 `(u16)id_X` compares (ids are i16 on purpose), type-erased editor interfaces fixed by retail mangled names, signedness that steers codegen | kept by design |
| Casts: duplicate-type | 1,241 | 53 | The APIOBJECT↔GAMEOBJECT base pun. Fixing it means embedding APIOBJECT as GAMEOBJECT's first member | needs evidence |
| Casts: view-type | 531 | 56 | The kept views above | kept by design |
| Casts: record-pun | 807 | 348 | Terrain shape-batch stream, AISCRIPTPROCESS puns, the DEBRIS_MATERIAL↔NUMTL remainder | needs evidence |
| Casts: void* in C++ | 983 | 554 | Allocator results, VARIPTR cursors, the display-list stream, typeless editor interfaces | kept by design |
| Casts: byte-pointer | 651 | 272 | Variable-length steps (heap, terrain stream, file relocation) | kept by design |
| Casts: raw-offset | 368 | 350 | Relocations, variable-length streams, written-out spline macros, and 12 legogame sites that lose a function if changed | kept by design |
| Casts: layout-compatible | 286 | 55 | Vector/matrix wrapper puns (VuVec/VuMtx vs NUVEC/NUMTX) | needs evidence |
| Calls without a prototype | 22 | 0 | | |
| Magic values: states / flags | 6,184 / 3,925 | 4,310 / 2,483 | docs/magic-values.md lists the reasons: counts, colours, character codes, hardware ids, bits with unsettled setters | needs evidence |
| Magic values: masks / bounds | 1,050 / 1,578 | 1,048 / 1,587 | The same tests now counted at real fields after the view merges | needs evidence |
| `clang-format off` | 4 | 0 | | |
| Section pragmas | 0 | 0 | | |
| Layout asserts in units | 3 | 0 | Converted to offset comments, which tools/layoutcheck.py checks | |
| goto | 397 | 397 | They carry the retail block order; restructuring them is not allowed | kept by design |
| MATCH-EXCEPTION sites | 80 | 81 | Each one carries its reason | kept by design |
| pc-suffix names | 74 | 56 | docs/pc-names.md lists the kept names, including the engine's own NUAPI_SETUP_PC_* tags | kept by design |

## Checks that now enforce the rules

- tools/proof.py: the match, data.cpp, SSE alignment (tools/sse_align.py), and an opt-in Debug smoke run.
- tools/oddscan.py, castscan.py, declscan.py, literalscan.py, floatlit.py: budgets that may only go down. config/budget-raises.txt holds the reviewed exceptions.
- tools/layoutcheck.py: struct offset comments.
- tools/standalone.py: all 419 module headers compile on their own.
- Keep lists: config/castscan-keep.csv and config/literalscan-keep.csv.
- clang-format and clang-tidy, pinned to version 18.

## How to resume

- Use the Kind column. Mechanical rows can go through another quick run. Rows that need evidence need a reason from the code, the game data or build/reference before a name or type changes.
- Run any new run the same way:
  1. Work on a branch with the quick gate.
  2. Record the match without blocking on it.
  3. Refix every drop, then merge.
  4. Settle every budget at its real count before main.
- Remote branches fully contained in main can be deleted: cleanup/quick-run, cleanup/quick-run-2, cleanup/quick-run-3, cleanup/parked-saga-pc-b, -b2, -c2, -c2w.
