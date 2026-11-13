# Cleanup audit: what a real codebase would not have

The goal is source that looks like the original and compiles to identical
code. This page counts what does not look like that yet, says which parts can
be removed without changing code generation and which cannot, and gives the
order of the work. The counts come from `tools/oddscan.py` (stdlib only, test
in `tools/test_oddscan.py`); `config/oddscan-budget.json` holds the counts of
the snapshot below, so CI can forbid new instances while old ones are removed.

```sh
python3 tools/oddscan.py                         # table: count, files, vendored, top files per class
python3 tools/oddscan.py --list --class noinline  # file:line list of one class
python3 tools/oddscan.py --magic [--hints DIR]   # enum and flag candidates, ranked by use
python3 tools/oddscan.py --json > oddscan.json   # everything (about 20 s)
python3 tools/oddscan.py --check                 # fails if a class grew; says which ones shrank
python3 tools/oddscan.py --write-budget          # after a cleanup batch: lower the budget to the counts
```

The scan is textual. Comments and string literals are separated first, and
braces are tracked to know whether a line is in a function body, a struct or
enum body, an `extern "C"` block or at file scope. Heuristic classes
(`ptr-int-cast`, `hex-name`, the magic-number classes) have some false
positives; the budget only compares counts, so that is harmless. Vendored
sources (`lib/ogg-vorbis`) are counted in a separate column and never budgeted.
Snapshot: `origin/main` of 2026-11-13.

## 1. Inventory

Verdict: **block** = a real codebase would not have it and it can go;
**sensitive** = can probably go, but it steers code generation, so every
removal needs proof; **stays** = currently the only known way to match (stays,
marked and counted); **ok** = harmless or accepted.

### a. Pragmas

| class | count | files | top files | verdict |
|---|---:|---:|---|---|
| `pragma-seg` (`const_seg` 4,180, `bss_seg` 3,057, `data_seg` 2,324, `section` 170) | 9,731 | 71 | data.cpp 7,873, ai_registry_pc.c 961, rtl.c 123, players.c 86 | sensitive |
| `pragma-optimizer` (`inline_depth` 4, `auto_inline` 4, `intrinsic` 3) | 11 | 5 | nurndr.c 4, support.h 2, nuterrain.c 2, terrain_collision.cpp 2 | sensitive |
| `pragma-pack` | 16 | 4 | saga.h 10, nupad_text.cpp, nustream.cpp, oggreader.cpp | ok (real layouts) |
| `pragma-macro` (`push_macro`/`pop_macro` around Windows A/W names) | 30 | 4 | edlevelall.cpp 14, NetworkObjectManager.cpp 6, saga.h 6 | block |
| `pragma-warning`, `pragma-comment`, other | 0 | | | |

Vendored: no pragmas. The section pragmas name a `$` subsection per object
(`#pragma const_seg(".rdata$Name")`) so that the linker sorts the data of the
central `data.cpp` into retail order. The original had no such thing: each
compiland emitted its data in definition order. The pragmas can only go after
the globals live in their owning compilands (layout migration, then batch 9).

### b. Compiler-steering keywords

| class | count | files | top files | verdict |
|---|---:|---:|---|---|
| `noinline` | 367 | 66 | chris.cpp 61, move.c 39, aisysall.c 36, pc_device.cpp 24, cutscene_lifecycle.c 20 | sensitive |
| `forceinline` | 36 | 23 | aisysall.c 8 | sensitive |
| `inline-keyword` (`__inline`) | 29 | 16 | edpp.c 9 | ok (VC8 C has no `inline`) |
| `asm` | 3 | 2 | cpu_features.cpp (cpuid, `orps`), nu3d_platform_pc.cpp | ok (retail CPU checks) |
| `volatile-local` | 4 | 3 | screen.cpp 2 (`volatile f32` pulse), panel.c (`time_spill`), shader_dictionary.cpp | stays (marked) |
| `volatile-other` | 37 | 9 | saga.h 11, data.cpp 10, debug units | ok (thread-shared data, Debug) |
| `register` | 0 | | | |
| `fastcall` / `unused-edx` | 16 / 6 | 6 / 2 | keyboard_init.c 8 | block: `__fastcall` + dummy EDX emulates `__thiscall` members in C |
| `thiscall` | 214 | 12 | saga.h 112, pc_device.cpp 39, pc_options.cpp 33 | block: explicit `__thiscall` on C++ members is the default spelling; removing it is neutral |
| `stdcall-definition` | 5 | 4 | WinMain, window procedure, thread procedure, enum callback | ok (Win32 callbacks) |
| `goto` | 397 | 76 | move.c 37, levels.cpp 28, game_obj.c 25, apiobject.c 22 (vendored: 120) | sensitive (some are original) |
| `if0` | 0 | | (vendored: 25) | |

No `__declspec(naked)` and no `#if 0` in `src/`. Explicit `__cdecl` is
spelled 13,736 times (4,640 in saga.h); it is the default convention, so
dropping it is codegen-neutral cosmetics and low priority.

### c. Markers and unnatural code

| class | count | files | top files | verdict |
|---|---:|---:|---|---|
| `match-exception` | 89 | 50 | aisysall.c 9, shader_parser.cpp 9, parts.c 6 | stays (each reviewed again in batch 11) |
| `nolint` (50 identifier-naming, 12 runtime-int) | 64 | 15 | saga.h 32, rtl.c 14 (vendored: 115) | ok |
| `format-off` | 0 | | (data.cpp's region removed, batch 2) | |
| `do-while0` outside macros | 2 | 2 | gameobjects.c, move.c | sensitive |
| `dummy` (dummy/sink names, `x = x;`) | 26 | 7 | dsplay_bink.cpp 7, edanim_load.c 6, move.c 6 | sensitive |
| `void-cast` (`(void)name;`) | 1,254 | 121 | aisysall.c 301, gameaiscript.c 247 | block (unused-parameter noise; neutral) |
| `ptr-int-cast` | 55 | 15 | nu3d_includes.cpp 16, gcutscn.c 7 | sensitive |
| `raw-offset` (`(u8 *)p + 0x50`) | 11 | 7 | aisysall.c 4, nu3d_platform_pc.cpp 2 | block (CONTRIBUTING forbids byte-offset casts) |
| `view-type` / `view-use` (`*_VIEW_PC`, `*_VIEW`) | 198 / 1,465 | 54 / 80 | saga.h 48/246, nu3d_includes.cpp 13/206, menu.c, players.c | sensitive |
| `assert-offsetof-header` | 4,714 | 2 | saga.h 4,698, pc_fmv_native.h 16 | see section 6 |
| `assert-offsetof-unit` | 6,609 | 203 | nu3d_includes.cpp 384, players.c 368, aisysall.c 328, nu3d_platform_pc.cpp 286 | block (overlay scaffolding) |
| `assert-sizeof` | 1,804 | 180 | saga.h 751, nu3d_includes.cpp 74, edlevelall.cpp 55 | see section 6 |
| `placeholder-field` (`field_0x14`, `unknown_102`, `pad_`) | 9,785 | 232 | saga.h 1,689, move.c 737, rtl.c 531 | block (naming; neutral) |
| `hex-name` (`TextIdPc8808e4`, `FUN_`, `data_0x`) | 12,063 | 110 | data.cpp 8,105, text_tables.cpp 1,482, ai_registry_pc.c 717, saga.h 335 | block (naming; neutral) |
| `candidate-name` (`_candidate`) | 439 | 63 | nu3d_includes.cpp 45, saga.h 41, nu3d_platform_pc.cpp 31 | block (naming; neutral) |
| `pc-suffix` (accepted, count only) | 52,581 | 323 | saga.h 8,981 | ok |
| `comment-draft` (`SOURCE DRAFT ONLY`) | 2 | 2 | aisysall.c, rtl.c | block |
| `comment-todo` | 1 | 1 | pc_input.cpp | ok |
| `comment-retail` | 459 | 202 | nu3d_includes.cpp 23, edlevelall.cpp 20 | block where it narrates matching; ok as rationale |
| `comment-address` | 813 | 148 | levels.cpp 588 (a table), nu3d_platform_pc.cpp 9 | block (addresses belong in config/symbols.csv) |

### d. Macros and declarations

| class | count | files | top files | verdict |
|---|---:|---:|---|---|
| `define-in-unit` | 547 | 103 | terrain_query.cpp 48, nu3d_includes.cpp 47, nuterrain.c 41 | mostly ok (file-local helpers with `#undef`); the shared ones go to headers |
| `undef-in-unit` | 225 | 33 | terrain_collision.cpp 38, terrain_query.cpp 38 | ok |
| `define-keyword` | 0 | | | |
| `define-prototype` (`*_BARE` unprototyped calls) | 3 | 3 | aisysall.c, charconfig.c, gizmo.c | stays (unprototyped call ABI), marked |
| `define-steering` | 1 | 1 | diagnostics.h (Debug) | ok |
| `dup-macro` / conflicting | 168 / 16 | 45 / 8 | `NU_SIN_LUT`, `NU_COS_LUT` (C4005 in rtl.c, move.c, parts_breakup.cpp), `SHADOW_BOUNDS_OVERLAP` | block |
| `dup-type` / conflicting | 1,190 / 60 | 143 / 25 | players.c 104, move.c 94, cutscene_lifecycle.c 51, gcutscn_locators.c 51 | block (local copies of shared types) |
| `local-extern` | 21 | 4 | parts.c 18 | block |

Declaration classes A-E of `tools/declscan.py` (docs/declarations.md) cover the
file-scope prototypes and `extern`s in units (2,709 class D); they are not
repeated here.

### e. Build system

| special case | where | reason (documented) |
|---|---|---|
| `/GS-` for every vendored libvorbis object except vorbisfile | Makefile:41 | retail libvorbis has no security cookies |
| second compiler (VC8 RTM) for libvorbis and `vorbis_smallft` | Makefile:44-46, config/toolchain.json `secondary` | retail links a prebuilt RTM libvorbis |
| `/DSAGA_VENDORED_LIBRARY` for vendored objects | Makefile:48 | they never included the engine's SSE header (`<xmmintrin.h>`) |
| `#define SAGA_VENDORED_LIBRARY` in a game unit | nufmv/pc/Bink/dsplay_bink.cpp | RAD sample code, also built without the SSE header |
| `/DSAGA_BUILD_ID=...` for `src/debug/log.c` | tools/cc.py | Debug identity string |
| Debug kind: `/DSAGA_DEBUG /Z7`, console subsystem | Makefile | diagnostics build, never compared |
| 7 Debug-only units excluded from Matching | Makefile `SOURCES` filter | diagnostics |
| `/FORCE:UNRESOLVED` in `make link` | Makefile | incomplete decompilation |

No forced includes (`/FI`), no per-unit optimisation flags in the game. All
game units: `/O2 /Gy /GS /MT`, default warning level.

### f. Structure

| measure | value |
|---|---:|
| `src/saga.h` lines | 31,064 |
| saga.h file-scope declarations (prototypes, externs, typedefs; asserts excluded) | 10,081 |
| saga.h named structs/unions/enums | 855 |
| saga.h `STATIC_ASSERT` lines | 5,454 |
| headers in `src/` | 6 (saga.h, externals.h, pc_fmv_native.h, 3 Debug headers) |
| units | 349 (343 include only saga.h; 15 also include another project header, all Debug or FMV) |
| `data.cpp` | 35,505 lines, about 5,760 file-scope definitions (724 of the 1,043 cross-unit variables, docs/declarations.md) |
| diagnostic stub units (`*_diagnostic_pc.c`, `*_diagnostic.c`) | 12 |
| invented `*_pc.*` file names | 37 |
| bitfield members in our structs | 2,442 |

The target has 243 compilands (docs/source-layout.md) and an original header
per module; see section 7.

### g. Magic numbers and typing

Literal uses inside function bodies (the scan ignores `0`, `1`, `-1` outside
`case`, all float literals, and literals in loop headers for bounds):

| class | count | files | vendored | top files |
|---|---:|---:|---:|---|
| `magic-state` (`==`, `!=`, `case`, assignment) | 8,622 | 234 | 157 | move.c 1,150, nu3d_includes.cpp 311, game_obj.c 210, edlevelall.cpp 200 |
| `magic-flag` (single bit in `&`, `|`, `^`, `|=`, `&= ~`) | 7,482 | 201 | 43 | move.c 827, aisysall.c 509, gameaiscript.c 373, game_obj.c 253 |
| `magic-mask` (several bits) | 1,178 | 167 | 43 | nu3d_includes.cpp 102, move.c 53 |
| `magic-bound` (`<`, `>`, `<=`, `>=`) | 1,627 | 177 | 73 | aisysall.c 84, nucore_includes.c 83 |
| `enum-anonymous` | 122 | 8 | | saga.h 108 (mostly one-constant `enum { X = 64 };`) |
| `enum-value-name` (`ZIPUP_FLAG_CONFIG_2`) | 6 | 1 | | zipup.c |

Fields are resolved to their struct through the declared type of the access
chain (`object->flags_e20` with `GAMEOBJECT *object` gives
`gameobject_s.flags_e20`), else through a unique member name. 293 fields are
compared with or assigned three or more distinct values (enum candidates) and
292 fields are tested with two or more distinct single bits (flag
candidates). Uses that already go through a name are rare: of the top 20 flag
fields, 15 have no named use at all.

The 20 most-used resolved fields (the pilot batch, section 8):

| # | field | width | kind | literal uses | distinct values | names in use / hint |
|---:|---|---|---|---:|---:|---|
| 1 | `gameobject_s.character_context` | i8 | enum | 783 | 97 | `LEGOCONTEXT_*` are `extern i32` variables; `CHARACTER_CONTEXT_*` names 4 values |
| 2 | `gameobject_s.flags_1fc_word` | u32 | flags | 516 | 14 | `APIOBJECT_FLAG_*` names 3 |
| 3 | `characterdata_pc_s.model_flags` | u32 | flags | 258 | 26 | - |
| 4 | `gameobject_s.flags_e20` | u32 | flags | 248 | 32 | - |
| 5 | `gameobject_s.flags_1fc` | u8 | flags | 239 | 4 | - |
| 6 | `STATUS_STAGE_s.field_0x14` | i32 | enum | 207 | 10 | - |
| 7 | `gameobject_s.field_7aa` | i16 | enum | 206 | 104 | 78 named uses (animation ids) |
| 8 | `gameobject_s.context_variant_flags` | u32 | flags | 206 | 18 | - |
| 9 | `gameobject_s.flags_1f8` | u32 | flags | 175 | 15 | - |
| 10 | `gameobject_s.portal_flags` | u32 | flags | 154 | 31 | - |
| 11 | `GIZMOBLOWUP_PC.status_flags` | u32 | flags | 153 | 26 | - |
| 12 | `FRAME_PAD_PC.digital_buttons_pressed` | u32 | flags | 151 | 14 | `PAD_BUTTON_*` |
| 13 | `PLAYERCHARACTERCONFIG_PC.flags_090` | u32 | flags | 147 | 30 | - |
| 14 | `gameobject_s.flags_ef0` | u32 | flags | 143 | 32 | - |
| 15 | `AIPACKET_PC.flags` | u32 | flags | 140 | 24 | - |
| 16 | `gameobject_s.flags_ef4` | u32 | flags | 126 | 32 | - |
| 17 | `debris_info_pc_s.generator_type` | i8 | enum | 120 | 7 | - |
| 18 | `animpacket_pc_s.requested_animation` | i16 | enum | 110 | 36 | animation ids |
| 19 | `FRAME_PAD_PC.digital_buttons` | u32 | flags | 110 | 14 | `PAD_BUTTON_*` names 6 |
| 20 | `area_data_pc_s.flags` | u16 | flags | 89 | 14 | Android `enum AREA_FLAGS` (value overlap 0.93) |

Ambiguous member names (`*.flags`, `*.type`, `*.state`: 43 fields, 1,269
uses) and plain variables (`flags`, `mode`, `state`: 3,362 uses) follow once
their owners are typed.

## 2. Worst findings

1. **Data placed by section pragmas**: 9,731 `const_seg`/`data_seg`/`bss_seg`/`section`
   pragmas, 7,873 of them in `data.cpp`, which defines about 5,760 globals
   centrally. The original defined data in its owning compilands.
2. **One 31,064-line header**: saga.h holds 10,081 declarations, 855 types and
   5,454 asserts; 343 of 349 units include nothing else. There are six headers
   in total.
3. **Layout asserts as code**: 13,127 `STATIC_ASSERT` lines, 6,609 of them in
   units as overlay scaffolding. They are typedefs, so their count perturbs VC8
   code generation (commit 7bf73838 added seven trailing offset asserts to keep
   `episodeIII.c`, `episodeIV.c`, `game.cpp` and `nupostdeferred.cpp` at their
   scores).
4. **Local copies of shared types**: 1,190 struct/typedef definitions repeated
   across 143 files, 60 of them with different bodies; 168 repeated macros, 16
   conflicting (`NU_SIN_LUT`/`NU_COS_LUT` cause C4005 in three units).
5. **Address and placeholder names**: 12,063 address-named identifiers (8,105 in
   data.cpp), 9,785 `field_0x14`/`unknown_102` fields, 439 `_candidate` names.
6. **Magic numbers**: 17,282 state, flag and mask literals; 585 fields are enum
   or flag candidates, and the top 20 fields alone carry 4,281 literal uses.
7. **Inlining steered by hand**: 367 `__declspec(noinline)`, 36
   `__forceinline`, 11 `inline_depth`/`auto_inline` pragmas.
8. **Overlay views**: 198 `*_VIEW_PC`/`*_VIEW` types with 1,465 uses instead
   of the real struct members.
9. **Invented files**: 12 diagnostic stub units, 37 `*_pc` files, two blocks
   marked `SOURCE DRAFT ONLY`.
10. **Weak enforcement** (fixed by batches 0 and 2, section 3): clang-tidy ran
    two checks; the MSVC build runs at the default warning level and printed 207
    distinct warnings that nothing tracked.

## 3. Formatting and lint enforcement

Status (batches 0 and 2 done): every rule below runs in CI and fails on growth.
CONTRIBUTING.md ("House rules enforced by CI") says how to run each check and
how to lower a budget.

| rule | where | budget / state |
|---|---|---|
| oddity classes (this page) | `make check-core`: `tools/oddscan.py --check` (about 20 s) | `config/oddscan-budget.json`, 65 classes |
| unit declarations, classes A-E (docs/declarations.md) | `make check-core`: `tools/declscan.py --check` | `config/declscan-budget.json`: A 126, B 5, C 376, D 2,630, E 5 |
| budgets only go down | `make check-core`: `tools/budget.py --guard` against the merge base with `origin/main` | raises need a line in `config/budget-raises.txt` |
| MSVC warnings, distinct per code | `make check-warnings` after the Matching and the Debug build (CI) | `config/warning-budget.json` |
| clang-format | `make lint` | `make format` applies it |
| clang-tidy, Matching and Debug view of every game unit | `make lint` | warnings are errors |

**Budgets.** `--write-budget` (oddscan, declscan) and `make lower-warning-budget`
only lower a budget; `--check` names the budgets that can be lowered. The guard
compares each budget file with the merge base of HEAD and `origin/main` (HEAD's
first parent on main itself); CI fetches main's commits without trees for it.
A budget file that is new since the base is not compared. declscan's check
skips the Debug parse (it only decides whether a declaration is used) and walks
only the unit's own cursors: a cold scan of all units takes about 85 s on four
cores (225 s before), a warm one under a second; CI caches the facts.

**MSVC warnings.** `tools/cc.py` writes the warnings of every compile next to the
object (`<object>.warnings`); `tools/msvcwarn.py` counts each distinct warning
once (a header warning is not multiplied by its includers) and writes
`build/warnings.json` (Debug: `build/Debug/warnings.json`, uploaded by CI). On
main after this batch: Matching and Debug 205 each (C4133 156, C4700 21, C4090
12, C4996 11, C4530 4, D9025 1); C4005 went from 6 (Debug 8) to 0. `/W3` does
not change object code (whole-object identity on zipup.c, rtl.c,
nu3d_includes.cpp, game.cpp, edlevelall.cpp); on six sample units (those and
camera.c) it adds C4244 121 and C4996 120 (CRT deprecations), C4700 19, C4099
4, C4090 1, C4309 1 distinct warnings, so a full `/W3` build would print about
2,000 to 3,000 distinct warnings. Proposal: add `/W3` with `/wd4244 /wd4996`
first (no object change; budget the rest), then work C4244 down by unit; make
C4005, C4133 and C4700 errors when they reach zero.

**clang-format.** `.clang-format` (LLVM base, indent 4, 120 columns) now also
sets `SeparateDefinitionBlocks: Always`, `MaxEmptyLinesToKeep: 1`,
`KeepEmptyLinesAtTheStartOfBlocks: false`, `InsertBraces: true`,
`RemoveSemicolon: true`, `InsertNewlineAtEOF: true`, `LineEnding: LF`.
`SortIncludes` stays `Never` and `IncludeBlocks` `Preserve`: include order
steers VC8 code generation (moving `<string.h>`/`<stdio.h>` in game.cpp or
`<new>` in nushadermanager.cpp changes code). `make format` reformats `src/`
(`tools/check.py --format-fix`); the pinned Ogg Vorbis sources keep their style
(`config/clang-format-vendored`) because their checksums are pinned. The single
`clang-format off` region is gone (the identity-matrix macro in data.cpp holds
only the values now; objects identical). The tree-wide reformat is one
mechanical commit, listed in `.git-blame-ignore-revs` (`git config
blame.ignoreRevsFile .git-blame-ignore-revs`); its objects are byte-identical to
the commit before it. After a rebase it is regenerated rather than merged:
drop the commit, take `.clang-format` from it and run `make format`. The only
`__LINE__` user left is the `LOG` macro in debug/diagnostics.h, which no unit
calls; `STATIC_ASSERT` names its typedefs with `__COUNTER__`. Before and after:

```c
/* before (legogame/zipup.c) */
} HINT_ZIPUP_PC;
typedef struct zipup_transport_view_pc_s {
    ...
static i32 ZipUp_ActivateRev(GIZMO_PC *gizmo, i32 active, i32 query) {
    ZIPUP_PC *zipup;

    if (gizmo == NULL)
        return 0;
    zipup = (ZIPUP_PC *)gizmo->object;
    if (query & 1)
        return active != zipup->direction;

/* after */
} HINT_ZIPUP_PC;

typedef struct zipup_transport_view_pc_s {
    ...
static i32 ZipUp_ActivateRev(GIZMO_PC *gizmo, i32 active, i32 query) {
    ZIPUP_PC *zipup;

    if (gizmo == NULL) {
        return 0;
    }
    zipup = (ZIPUP_PC *)gizmo->object;
    if (query & 1) {
        return active != zipup->direction;
    }
```

**What clang-format cannot do** is counted by oddscan group h, budgeted like the
rest (numbers after the reformat): mid-file `#include` 5, function bodies of 40+
lines without a blank line 1,196, functions over 400 lines 21, files over 5,000
lines 12, data-like files (over 200 lines, under 2% blank) 6 (23 before the
reformat added blank lines between definitions), several statements on a line
0, commented-out code 0. Mid-file includes were moved to the top in 40 units
and in saga.h (`<math.h>`), objects identical. Five stay: game.cpp (`<string.h>`,
`<stdio.h>`: moving them swaps two loads in InitGameAfterConfig) and
nushadermanager.cpp (`<new>`: its inline placement new changes the code of
four shader-manager methods), and in saga.h `<vector>` (inside the
`SAGA_SHADER_PARSER_TYPES` C++ block) and `debug/diagnostics.h` (needs the
types above it); the saga.h split (section 7) resolves those two.

**STATIC_ASSERT names.** The typedef of each assertion was named after
`__LINE__`. VC8's code for a few fragile functions depends on the identifiers it
has seen: inserting two blank lines near the top of saga.h changed
InitGameAfterConfig (game.cpp), one or three did not. `STATIC_ASSERT` now uses
`__COUNTER__`, so moving lines (formatting) keeps every name; with it, shifting
saga.h lines leaves game.obj identical. The switch itself kept every object
identical on the 2026-11-13 base; on the later base it changes only
InitGameAfterConfig, which is not an exact match before or after (objdiff
82.24% both).

**clang-tidy** (`.clang-tidy`). Step 1 of the stricter configuration is in:
`readability-identifier-naming`, `google-runtime-int`, 143 bugprone,
readability, misc, cert and performance checks that have no finding in the tree
(five of them have findings only in upstream Ogg Vorbis code and are exempted
for it by `tools/check.py`: `bugprone-incorrect-roundings`,
`misc-header-include-cycle`, `performance-type-promotion-in-math-fn`,
`readability-avoid-unconditional-preprocessor-if`,
`readability-redundant-control-flow`), and `clang-diagnostic-*` minus the
classes that still have findings. Fixed for it: the conflicting
`NU_SIN_LUT`/`NU_COS_LUT` copies (macro-redefined, C4005), nine units that
called `sprintf` undeclared (implicit-function-declaration), the Debug-only
`STUBBED` redefinition. Not enabled, with the whole-tree count of distinct
findings (Matching view):

| class | findings | note |
|---|---:|---|
| `clang-diagnostic-gnu-folding-constant` | 11,427 | STATIC_ASSERT array bounds with `offsetof` |
| `clang-diagnostic-vla-cxx-extension` | 7,235 | same, in C++ units |
| `clang-diagnostic-c++11-compat-deprecated-writable-strings` | about 900 | string literal to `char *` (C++98 allows it) |
| `clang-diagnostic-deprecated-declarations` | 551 | CRT `_CRT_SECURE` deprecations (= C4996 at /W3) |
| `clang-diagnostic-incompatible-pointer-types` (+ discards-qualifiers) | 154 + 13 | = C4133, C4090 |
| `clang-diagnostic-microsoft-cast` | 111 | function pointer to data pointer casts |
| `clang-diagnostic-format-security` | 34 | non-literal format strings |
| `clang-diagnostic-tautological-undefined-compare` | 30 | `this == NULL` style checks |
| `clang-diagnostic-warning` | 16 | tentative array definitions in gameaiscript.c |
| `clang-diagnostic-deprecated-non-prototype` | 6 | the deliberate unprototyped calls (keep) |
| 19 more classes (dangling-else, pointer-sign, shift-negative-value, format arguments, unsequenced, ...) | 1-5 each, 44 in all | |
| `readability-duplicate-include` | 22 in src | clang-tidy 18 flags an include of a header that an earlier header already included; no file includes a header twice |

Step 2 and 3 of the table below remain. Game units are now also analysed with
`-DSAGA_DEBUG`, so code inside `#ifdef SAGA_DEBUG` is linted;
`tools/check.py --tidy --matching-only` skips that pass. `make lint` takes
11 minutes on a four-core machine, against 3.5 minutes before: the new checks
add about 1 minute (4.6 minutes with `--matching-only`) and the Debug pass
about 6 minutes.

The original measurements that chose step 1 (clang-tidy 18 on 15 units, all of
`bugprone-*`, `readability-*`, `misc-*`, `cert-*`, `performance-*`,
`clang-analyzer-core.*`/`deadcode.*` and `clang-diagnostic-*`, units only):

| step | checks | hits in the sample | note |
|---|---|---:|---|
| 1, done | every bugprone/readability/misc/cert/performance check with 0 hits in the whole tree, the clean compiler warning classes, macro-redefined, implicit-function-declaration | 0 | |
| 2 | `clang-diagnostic-incompatible-pointer-types` 42 (= C4133), `readability-redundant-declaration` 10 (= declscan class A), `readability-named-parameter` 11, `bugprone-multi-level-implicit-pointer-conversion` 11, `bugprone-switch-missing-default-case` 16 | 90 | casts and declarations; verify object identity |
| 3, review | `clang-analyzer-core.NullDereference` 12, `bugprone-integer-division` 7, `bugprone-branch-clone` 12, `bugprone-signed-char-misuse` 24, `misc-unused-parameters` 77 | 132 | some are retail bugs; keep those with a reason |
| not suitable | `readability-magic-numbers` 3,443 (also fires on array bounds; oddscan's budget does this job), `readability-uppercase-literal-suffix` 1,961, `readability-braces-around-statements` 1,280 (clang-format's InsertBraces does it now), `hicpp-signed-bitwise` 1,063, `bugprone-narrowing-conversions` 280 | | style or codegen-sensitive at this size |

## 4. What is harmless and what blocks

Harmless or accepted: `Pc`/`_PC` suffixes, `__inline` (VC8 C), Win32
`__stdcall` callbacks, `#pragma pack` on real packed layouts, the vendored
special cases, file-local helper macros with `#undef`, the CPUID `__asm`, the
Debug units and their `SAGA_DEBUG` blocks, `NOLINT` with a reason.

Blocks "real codebase", codegen-neutral to remove: address/placeholder/candidate
names, address and matching-narration comments, `SOURCE DRAFT ONLY`, `(void)x;`
noise, explicit `__thiscall` on members, explicit `__cdecl`, duplicate and
conflicting macros, `push_macro` scaffolding, literals instead of named constants.

Blocks, matching-sensitive: section pragmas and central data, saga.h and the
local type copies (declaration count), unit-level STATIC_ASSERT overlays,
`*_VIEW` overlays, `noinline`/`forceinline`/optimizer pragmas, raw-offset and
pointer-through-integer casts, `__fastcall` + dummy EDX emulation.

Stays for now (marked, counted, budgeted): the 89 `MATCH-EXCEPTION`s, the 4
`volatile` locals, the 3 unprototyped `*_BARE` calls, `goto`s that reproduce
retail block order, and any `noinline` whose removal loses an exact match after
the function is in its retail compiland. Each keeps a comment that says why.

## 5. House rules for enums, flags and bitfields

What VC8 does:

- A named constant (`#define` or enum member) instead of a literal is
  codegen-neutral: the constant expression is identical. Exceptions to watch:
  an enum constant has type `int`, so a mask `>= 0x80000000` becomes negative
  and can change unsigned comparisons, shifts and divisions. Write such masks
  as `#define NAME 0x80000000u` (or keep them in a `u32` constant).
- An enum type is `int`-sized and signed in C and in VC8 C++ (C++98 has no
  fixed underlying type; VC8's `enum E : unsigned char` is a Microsoft
  extension and not used here). So only `i32` fields, locals and parameters
  can become enum-typed without a codegen change. `u8`/`i8`/`u16`/`i16`
  fields keep their width (`u8 state; /* enum GAME_STATE */`), and a `u32`
  field turned into an enum changes unsigned comparisons; verify those.
- A bitfield is not codegen-neutral in general. A one-bit bitfield test or set
  compiles like a mask on the storage unit, but VC8 narrows read-modify-writes
  to the byte that holds the bit (`or byte ptr [m+1], 4` where a `u32` mask
  gives `or dword ptr [m], 0x400`), a multi-bit store is
  `xor r, [m]; and r, mask; xor [m], r` (docs/vc8-idioms.md I26), and a
  signed bitfield read sign-extends with `shl`/`sar`. These are the
  signatures of retail bitfields: byte-sized RMW on a field we hold as `u32`,
  the `xor`/`and`/`xor` insert, and `shl`+`sar` extraction. `idiomscan.py`
  already tags I26 (130 in the tally of that page).

House rule:

1. Values of one field that name a state, mode, type or kind: an `enum` in the
   owning header, named `<OWNER>_<STATE>` after the Android decompilation where
   it has the enum (`AREA_FLAGS`, `AIPACKET_MOVEMENT_MODE`, ...). The field is
   typed as the enum only when it is `i32`; otherwise the comment names it.
2. Independent bits of one word: named masks next to the field
   (`#define GAMEOBJECT_FLAG_E20_xxx 0x1000` or an enum of masks), never
   numbers. Multi-bit masks get their own name (`..._MASK`).
3. A bitfield only where retail shows a bitfield signature (above) or the
   field already is one (2,442 members today); never as a matching guess.
4. No enum member is named after its value (`FOO_0x10`, `STATE_3`); use
   `UNKNOWN_0x10` only inside an enum that is otherwise named, and count it.
5. Use what names exist: enums and `#define`s in our tree and in the Android
   decompilation (`tools/oddscan.py --magic --hints <android src>`), string
   tables in the executable's data (keyword tables of the config parsers, AI
   script action and condition names, animation names in `data.cpp`), and the
   symbol names of sibling builds as hints. A name without evidence is
   descriptive (`GIZMOBLOWUP_STATUS_ACTIVE`), never invented to look original.

## 6. Layout comments instead of STATIC_ASSERT

Today: 13,127 assert lines. `offsetof` 11,323 (saga.h 4,698 = 41%; units
6,609 in 203 files, top nu3d_includes.cpp 384, players.c 368, aisysall.c 328,
nu3d_platform_pc.cpp 286); `sizeof` 1,804 (saga.h 751 = 42%; nu3d_includes.cpp
74, edlevelall.cpp 55, nu3d_platform_pc.cpp 50). saga.h holds 5,454 (42%).

Grammar (one required comment per field and per struct, checked by a tool):

```c
typedef struct example_s {
    NUVEC pos;              // 0x000
    u32 flags;              // 0x1fc
    union {                 // 0x200
        f32 timer;          // 0x200
        i32 counter;        // 0x200
    };
    u32 active : 1;         // 0x204:0
    u32 mode : 3;           // 0x204:1
    struct {                // 0x208
        i16 x;              // 0x208 (absolute, not relative)
    } grid;
} EXAMPLE;                  // size 0x48c
```

- Offsets are absolute from the start of the outermost named struct, lower-case
  hex, at least three digits wide inside a struct larger than 0xfff (four for
  larger), so they align and sort.
- Bitfields: `0xOFF:BIT` (storage unit offset and first bit).
- Anonymous unions and structs: the opening line carries the offset; members
  repeat it.
- C++ classes with a vtable: a comment line `// 0x000 vtable` before the first
  member; base classes `// 0x000 base BASE_NAME`.
- Closing line: `// size 0xNN` after the type name (or after `};`).
- Types whose layout is unknown past some point keep an explicit tail field
  (`u8 unknown_1f0[0x10]; // 0x1f0`), so the size is still stated.

Checker (`tools/layoutcheck.py`, planned): parse every struct definition in
`src/**/*.h` (and units while local types remain), and fail if a field or a
struct lacks the comment (mandatory, so new structs cannot skip it). Verify
the numbers two ways:

- **Fast, local and in CI**: clang with the MS ABI. `clang-tidy-18` (already
  installed by CI) accepts `-Xclang -fdump-record-layouts-complete` (tested:
  it prints offsets, bitfield positions, vtable pointers and sizes of every
  complete record for `--target=i686-pc-windows-msvc`), so the checker can run
  it once per header on a generated unit that includes the header, and
  compare. clang-18 itself is not needed.
- **Authoritative**: generate a throwaway unit of `STATIC_ASSERT`s from the
  comments and compile it with the pinned VC8 through `tools/cc.py` in `make
  check-core` (CI has Wine and the compiler before that step). This catches
  the few places where Clang's MS layout and VC8 disagree.

Risk: STATIC_ASSERT lines are typedef declarations, and the number of
declarations a unit sees perturbs VC8 code generation of a few fragile
functions (commit 7bf73838 retuned trailing asserts for exactly that). Pilot
the removal on one header that few units include, under exact-set comparison
plus code-section identity of every object. A function that only matches
because of the assert count is a finding: list it in this document with the
unit and the count, do not keep asserts silently to preserve it. If the effect
is real, the fix is a declaration of the right kind in the right place (the
original headers had their own declaration counts), found by the header
migration, not padding asserts.

## 7. Header structure

Owner decisions (the target):

1. `src/saga.h` goes away entirely. Every type, enum, prototype and global
   declaration moves to a real module header.
2. `src/externals.h` stays only for what has no retail header. Its 55
   declarations are DirectX (D3D9, D3DX, DirectSound) forward declarations and
   COM vtables; they would move to the real SDK headers (`<d3d9.h>`,
   `<d3dx9.h>`, `<dsound.h>`, `<dinput.h>`, `<xinput.h>`) if the toolchain
   also extracted the August 2007 DirectX SDK `Include` directory (today only
   `Lib` is extracted). The two `*_VIEW` DirectSound views in it are scaffolding.
3. A `src/decomp.h`, like the Android decompilation's `src/decomp.h`, holds
   the decomp-only scaffolding: layout checks (until section 6 replaces them),
   `*_VIEW_PC` overlays still needed, `STUBBED`, candidate prototypes, the
   hooks of `src/debug/diagnostics.h` and `support.h` used from game code
   (`LOG_INFO_IF` 25, `LOG_INFO` 22, `LOG_WARN` 4, `LOG_WARN_ONCE[_IF]` 3 in 9
   game units; 4 `SAGA_DEBUG` blocks in game units), and things without a known
   home. Game headers stay clean; a unit that needs scaffolding includes
   `decomp.h` explicitly.

**Template: the Android decompilation.** It has 477 headers next to its units:
`nu2api/nucore/*.h` (62: `common.h`, `fixed_width.h`, `numem.h`, `nufile`,
`nupad.h`, `nustring.h`, ...), `nu2api/nu3d/*.h` (39: `nuprim.h`, `nurndr.h`,
`nucamera.h`, `nudlist.h`, `nuspecial.h`, ...), `nu2api/numath/*.h` (14:
`nuvec.h`, `numtx.h`, `nuquat.h`, `nutrig.h`, ...), `nu2api/nufile/*.h`, and
about 295 under `legoapi`, `gameapi`, `gamelib`, `legogame`, `editor`. Its
`decomp.h` (244 lines) holds `DECOMP_ASSERT`, Ghidra-style `undefined*`
typedefs, host-build switches, logging, and a few types without a home. It
also has a central `globals.h` (1,196 lines), which we do not copy: globals go
to their module headers. Its `legoapi` grouping into subdirectories
(`characters/core`, `world/levels`, ...) is its own organisation; ours follows
the compilands in `config/layout-target.csv` (flat `legoapi/`, `legogame/`,
`nu2api/nuwin32/`, ...). Header *names* follow Android where it has them.

Evidence for module names: retail `__FILE__` paths name the source tree
(`nu2api.saga\nucore\`, `nu2api.saga\numath\`, `nu2api.saga\nu3d\` with
`pc\` subdirectories, `gameapi.saga\gui\gamemenu\`, `gameapi.saga\edtools\`,
`gameapi.saga\gamelib\`, `gameapi.saga\rtl\`), but no header paths; the Batman
Mac symbols name original headers such as `legoapi/legoapi.h`,
`legoapi/customiser.h`, `nu2api/nu3d/nuprim.h` and
`gameapi/Support/ThingManager.h` (docs/declarations.md); the compilands come
from `config/layout-target.csv` (243 rows).

**Where saga.h goes.** Matching saga.h's 10,894 named items (types, typedefs,
enumerators, prototypes, externs, macros) by name against the Android headers:

| bucket | items | note |
|---|---:|---|
| module header that Android has (by name) | 5,475 | 3,145 prototypes, 1,614 globals, 244 structs, 284 typedefs, 121 enumerators. Largest Android targets: `globals.h` 471 (to be spread over modules), `legoapi/legoapi_types.h` 241, `render/core/gameliball.h` 214, `batman.h` 163, `menus/screens/gamemenuall.h` 146, `characters/core/character.h` 139, `nu2api/nufile/nufile.h` 100, `numath/numtx.h` 90 |
| new module header, PC-only names (`*Pc`, `*_PC`, `PC*`) | 2,391 | Windows/D3D platform (`nu2api/nuwin32/`, `nu3d/pc/`, `nusound3/pc/`), PC menus and options, and PC names for engine items not yet matched to Android names |
| module header, no Android match and no PC marker | 2,624 | owner from the defining compiland (`config/units.csv`) |
| `decomp.h` scaffolding (`*_VIEW*`, `_candidate`, diagnostics, `STATIC_ASSERT` machinery) | 93 | plus the 5,454 assert lines |
| externals / SDK | 21 | D3DX method tables, window procedure |
| dead (no unit defines or uses it) | 290 | 130 structs, 89 prototypes, 52 externs: delete |

Struct layouts that differ between Android and PC (only 25 types have a size
assert in both under the same name; 4 differ): `MENU` 224 vs 9,848 bytes (probably two different types under one name),
`EDAIAREA_s` 72 vs 68, `SoundTable` 200 vs 204, `WORLDINFO` 20,908 vs 20,912.
PC types also carry Windows handles and D3D9 objects where Android has GL ones;
those headers (`NuRenderDevice.h`, `ShaderManagerOpenGL.h`, `nuandroid/`) have
PC counterparts (`nuwin32/`, `nu3d/pc/`) instead.

**Target layout** (first cut; names after Android, paths after the compilands):

- `src/nu2api/nucore/common.h` (base types `i8`..`f64`, `NUWCHAR`, `MIN`/`MAX`),
  and one header per nucore module (`numem.h`, `nufile.h`, `nupad.h`,
  `nustring.h`, `nulist.h`, `nuthread.h`, `nutime.h`, ...), PC parts in
  `nucore/pc/`.
- `src/nu2api/numath/` (`nuvec.h`, `nuvec4.h`, `numtx.h`, `nuquat.h`,
  `nutrig.h`, `nurand.h`, `nufloat.h`) and the SSE vector header that pulls in
  `<xmmintrin.h>` (every engine unit must keep including it, see the
  `SAGA_VENDORED_LIBRARY` note in section 1e).
- `src/nu2api/nu3d/` (`nuprim.h`, `nurndr.h`, `nucamera.h`, `nudlist.h`,
  `nuspecial.h`, `nutex.h`, `numtl.h`, `nugscn.h`, ...) and `nu3d/pc/` for
  the D3D9 side; `src/nu2api/nuwin32/` (`pcapi.h`, `d3dCore.h`, ...);
  `nusound3/`, `nufmv/` (with today's `pc_fmv_native.h`).
- `src/gameapi/` per module (`gamemenu/apimenu.h`, `edtools/edui.h`,
  `rtl/rtl.h`, `aisys/aisys.h`, `Support/ThingManager.h`, ...).
- `src/legoapi/legoapi.h` plus per-module headers (`customiser.h`,
  `characters.h`, `players.h`, `levels.h`, `worldinfo.h`, `move.h`, ...);
  `src/legogame/` for the TCS game files.
- `src/externals.h` (SDK gaps only), `src/decomp.h` (scaffolding),
  `src/debug/*.h` unchanged.

Each enum lands in its owning header together with the struct whose field
uses it and the prototypes that take it (`AREA_FLAGS` with `AREA_DATA` in
`legoapi/areas.h`; `GAMEOBJECT` flag masks with `gameobject_s`).

**Migration order and the declaration-count risk.** The number and kind of
declarations a unit sees perturbs VC8 code generation of a few fragile
functions (`BoulderSpawnPc`, `SubItemMenu`, `InitGameAfterConfig`,
`BlockadeRunnerDEjectCreaturePc`, `NuRndrParticleGroup`, `DebrisSetup2`;
`FRAGILE` in `tools/unitscore.py`). So:

1. Split saga.h into the module headers in the same order as today, and make
   saga.h a shim that only includes all of them. Every unit still sees the
   same declarations in the same order: verify code-section identity of every
   object (should be byte-identical; the `__LINE__`-named assert typedefs
   change name only).
2. Move each type out of the units' local copies into its header (dup-type,
   dup-macro) and each class D declaration (docs/declarations.md) into the
   owner's header, one module at a time, under exact-set comparison.
3. Narrow units one at a time: replace `#include "saga.h"` by the headers the
   unit needs. Exact-set comparison per batch, FRAGILE functions compared
   separately; a loss is bisected to the declaration that causes it and
   recorded here (as with the two units in docs/declarations.md).
4. Delete the shim when no unit includes it.

Enums and named masks (section 5) go in during step 2, in the header of the
owning struct, so a field is converted once.

## 8. Plan

Gate for every batch: `python3 tools/check.py`, `--format`, `make lint`, `make
check-core`, Debug compile (`make -k BUILD_KIND=Debug build`, zero errors:
declarations used only in Debug macros are invisible to the Matching build),
then the batch's own proof. **Neutral** batches: code sections of every object
byte-identical to the base build (data sections too where data moves are not
intended). **Sensitive** batches: the exact set by address after `match.py
--update` identical (gains welcome), FRAGILE functions rerun, and
`check-objects`. After each batch, `oddscan.py --write-budget`.

| # | batch | size | codegen | verify |
|---:|---|---|---|---|
| 0, done | `oddscan.py --check`, `declscan.py --check` and the budget guard in `make check-core`; MSVC warning budget after each CI build | tool only | none | unit tests |
| 1 | Comments: 2 `SOURCE DRAFT ONLY`, 813 address comments (to symbols.csv notes or removed), the matching-narration part of 459 `retail` comments | ~1,300 lines, ~200 files | neutral | object identity |
| 2, done except `push_macro` | Lint step 1 (section 3), the conflicting `NU_SIN_LUT`/`NU_COS_LUT` copies (C4005), mid-file includes, stricter clang-format and the reformat; open: 30 `push_macro` lines (undefine the A/W macros once in the header) | ~60 sites + reformat | neutral | object identity |
| 3 | Names: 439 `_candidate`, explicit `__thiscall` on members (214), `(void)x;` noise (1,254) | ~1,900 lines | neutral (`_candidate` renames change symbol names: update config/symbols.csv) | object identity + check-objects |
| 4 | Pilot: section 7 step 1 (saga.h split into headers + include-all shim) and `decomp.h` | saga.h's 31k lines | neutral by construction | object identity, all units |
| 5 | Pilot: enums/flags for the 20 fields of section 1g, named constants only (no field retyping except `i32`) | ~4,300 uses | neutral | object identity |
| 6 | Pilot: layout comments for one header, its STATIC_ASSERTs removed, `layoutcheck.py` | one header | sensitive (declaration count) | exact set + list any assert-count dependency |
| 7 | Local types and macros to headers (1,190 dup-types, 60 conflicting, 168 macros, 21 local externs, 2,709 class D declarations), module by module, with enums for their fields | ~150 files | sensitive (declaration noise) | exact set; Debug compile |
| 8 | Placeholder and address names (9,785 fields, 12,063 symbols) from data tables, Android and sibling-build names | ~22,000 sites | neutral (symbol renames update config) | object identity + check-objects |
| 9 | Data to owning compilands, drop section pragmas (9,731) as each compiland's data reproduces retail order; empty data.cpp | after the layout migration | sensitive (data order) | data-section comparison + exact set |
| 10 | Views and overlays: 198 `*_VIEW` types, 6,609 unit offset asserts, 11 raw-offset and 55 pointer-integer casts become real members | ~2,000 sites | sensitive | exact set |
| 11 | Steering: 367 `noinline`, 36 `forceinline`, 11 optimizer pragmas, 16 `__fastcall`/6 dummy-EDX (become C++ members), 4 `volatile` locals, the 89 MATCH-EXCEPTIONs: retry each once its function sits in its retail compiland | ~500 sites | sensitive | exact set per function; what still needs it stays, marked |
| 12 | Narrow units to their headers, delete the saga.h shim; fold the 12 diagnostic stub units and the `*_pc` files into their compilands | 349 units | sensitive | exact set per batch |

Batches 1-3 and 5 can start now and run beside the layout migration (they do
not move code). 4 must precede 7 and 12; 9, 11 and 12 follow the layout
migration of docs/source-layout.md. What cannot be removed without losing a
match stays, with a `MATCH-EXCEPTION` comment, and is counted by the budget.
