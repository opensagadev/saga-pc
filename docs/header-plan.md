# Header plan: from saga.h to module headers

`src/saga.h` (33,569 lines) is the one header nearly every unit includes. The
owner's decisions (2026-11-13): saga.h goes; declarations live in real module
headers following the Android decompilation's tree (`../saga-android/src`);
units stop forward-declaring and declaring `extern`s themselves; enums and
flags live in the header of the struct that uses them; `decomp.h` (as in the
Android tree) takes decomp-only scaffolding; `externals.h` stays only for what
has no retail header.

Because VC8's code for a few fragile functions depends on how many symbol ids
a unit consumes before them (docs/cleanup-audit.md, section 7), the transition
has three steps, each with its own proof:

1. split saga.h into module headers behind an include-all shim: every unit
   sees exactly the same declarations in the same order (proof: identical
   preprocessed token streams, then identical objects);
2. narrow each unit's includes to the headers it needs (proof: exact set);
3. delete the shim; merge header parts; decomp.h and externals.h keep only
   their own content.

The tool is `tools/headersplit.py` (test `tools/test_headersplit.py`), the
reviewed mapping is `config/header-map.csv`. Snapshot: origin/main ae97eb11.

```sh
python3 tools/headersplit.py --roundtrip                 # the segmentation gives saga.h back byte for byte
python3 tools/headersplit.py --build-map                 # regenerate the map (keeps rows with evidence "review")
python3 tools/headersplit.py --stats                     # items per kind, evidence, target; unknowns by size
python3 tools/headersplit.py --interleaving              # headers that need parts, and what separates them
python3 tools/headersplit.py --dry-run                   # headers and shim it would write
python3 tools/headersplit.py --apply [--root DIR]        # write the split into DIR/src (default: this tree)
python3 tools/headersplit.py --check-neutral UNIT... [--debug]
python3 tools/headersplit.py --place NAME...             # target header of names declared in units (declscan C/D)
```

## 1. Measured picture

### Items

The segmenter cuts saga.h into 15,980 top-level items and 378 context
directives (32 top-level `#if` frames with 7 `#else`/`#elif` branches, 153
`extern "C"` blocks). A struct body, a prototype list split by an `#ifdef`, a
`#pragma pack` region and a conditional that changes its own condition
(`#ifndef X` / `#define X` ...) are one item each. Concatenating the items and
directives gives saga.h back byte for byte.

| kind | items | placed |
|---|---:|---:|
| STATIC_ASSERT | 5,155 | 3,237 |
| extern variable | 4,924 | 4,280 |
| function prototype | 4,455 | 4,406 |
| struct | 752 | 569 |
| forward declaration | 350 | 286 |
| enum | 110 | 102 |
| typedef | 87 | 64 |
| class | 65 | 51 |
| macro | 48 | 41 |
| union, include, `#pragma pack` region, other | 34 | 23 |
| total | 15,980 | 13,059 (81.7 %; 25,433 of 32,455 item lines, 78.4 %) |

### Evidence (`config/header-map.csv`)

One row per item: `key` (kind:first name, `#n` for repeats; the tool matches
rows by key, line numbers are informational), `kind`, `names`, `line_start`,
`line_end`, `lines`, `target` (path under `src/`, empty = stays in saga.h),
`evidence`, `detail`. Rules, strongest first:

| evidence | items | rule |
|---|---:|---|
| `decomp` | 116 | platform prelude (SDK includes, `WIN32_LEAN_AND_MEAN`, `externals.h`), scaffolding macros (`STATIC_ASSERT`, `SAGA_JOIN`, `STUBBED`, `U64_PAIR`), `*_VIEW` overlay types, asserts on SDK types |
| `externals` | 115 | libogg/libvorbis API (`externals/ogg.h`), Bink exports (`tools/binkw32.def`, `externals/bink.h`), layout checks of `src/externals.h` types (`externals/dsound.h`) |
| `android` | 4,818 | an Android header (not a catch-all) declares the same name; ties go to the shorter path |
| `symbol` | 1,288 | function: `config/symbols.csv` address in a `config/layout-target.csv` compiland |
| `definition` | 163 | global defined in exactly one unit other than data.cpp |
| `android-stem` | 159 | Android declares the PC name without its `_PC`/`Pc` suffix, or with other case (`gameobject_s` = `GameObject_s`) |
| `users` | 2,457 | units of exactly one compiland use the name (data.cpp excluded) |
| `type-users` | 60 | a type that only the items of one header use |
| `data-order` | 411 | a global whose nearest placed neighbours in retail data (both sides) go to the same header |
| `same-entity` | 12 | the PC and original name of one global (config/sibling-names.csv), see 1.5 |
| `android-weak` | 306 | declared only in an Android catch-all (`globals.h`, `decomp.h`, `batman.h`, stubs) |
| `follow-type` | 3,151 | STATIC_ASSERT: the header of the type it checks |
| `follow-definition`, `follow-next` | 4 | forward declaration with its definition; `#undef` with the next item |
| `unknown` | 2,920 | none of the above (target empty) |

A compiland-based target uses the Android path of the module when it exists:
the Android header with the compiland's (or unity member's) stem, else the
Android unit's directory, else the PC directory (`header_for_compiland`). The
map has 406 target headers; 331 of them exist in the Android tree and take
11,170 items. The 75 new ones are PC-only modules (`nu2api/PCInput.h`,
`nu2api/nuwin32/d3dCore.h`, the `*_gen.h` unity members) and `externals/*.h`.
Largest targets: `legoapi/legoapi_types.h` 733 items, `gameliball.h` 480,
`nu3d_includes.h` 439, `apiobject.h` 417, `nu2api/PCInput.h` 300,
`globals.h` 289, `decomp.h` 282.

Android coverage: 5,474 of the 10,813 named items (51 %) have a name that an
Android header declares (functions 3,090 of 4,455, variables 1,800 of 4,924,
structs 227 of 752, enums 102 of 110); 4,819 of them in a specific header.
Three Android headers do not parse with the textual segmenter
(`gizmos_gizactions_types.h`, `numtx_inline.h`, `nusound_voice_android_types.h`:
an anonymous namespace and macro-generated functions); their names give no
evidence.

**Unknowns** (2,920 items, 7,022 lines; `--stats` lists them by size):

- 1,918 STATIC_ASSERTs on a type that is itself unknown: they follow it.
- 653 declarations used by units of two or more compilands (363 by exactly
  two): 381 variables, 183 structs, 64 forward declarations, 27 functions.
  The largest: `PLAYERCHARACTERCONFIG_PC` (157 lines, 36 compilands),
  `PC_DEVICE_BACKEND_PC` (125), `GAMESAVE` (90), `DEBRIS_INFO_PC` (90),
  `PART_PC` (90), `EdClassInterface` (89), `GIZMOBLOWUP_PC` (88). The detail
  column names the compilands that use each one most; a reviewer picks the
  owner and marks the row `review`.
- 285 declarations no unit uses (263 variables, mostly in the blocks that
  `tools/globals.py` transferred from Android): candidates for deletion, which
  changes the id count (section 4).
- 64 types used only inside saga.h by items of several headers
  (`PC_D3D9_METHODS_PC`, `PC_WINDOW_OPTIONS_PC`, `debris_key_pc_s`).

`globals.h` (289 items) and `legoapi/legoapi_types.h` (733) are Android's own
catch-alls. They are valid targets for step 1 (the names are declared there in
Android) but should not survive step 3 as catch-alls: their rows are the first
to review.

### Interleaving

saga.h grew by work batch, not by module, so most modules are scattered:

| measure | count |
|---|---:|
| runs (maximal sequences of items with one target) | 5,123 |
| runs that go to a header | 4,524; 599 stay in saga.h |
| headers with several runs | 347 of 406 |
| runs if STATIC_ASSERTs and forward declarations were ignored | 4,659 |
| runs of type definitions only | 428 |
| runs of prototypes and `extern`s only | 3,637 |

So the scattering comes from prototypes and `extern`s, throughout the file
(4,025 runs start before line 25,487, where the blocks transferred from
Android begin; 1,098 after), not from the asserts. Single items that split a run of another header: 249 variables,
200 functions, 30 forward declarations, 28 asserts, 20 structs. The most
fragmented headers: `globals.h` 173 runs, `gameliball.h` 159,
`nu3d_includes.h` 135, `legoapi_types.h` 130, `move.h` 88, `decomp.h` 56.

Header dependencies (the closest earlier declaration of every type, typedef,
macro or enumerator an item uses): on average 4.3 other headers each; one
strongly connected group of 54 headers (pointer uses counted too, so this is
an upper bound); 371 headers need a header that is not entirely earlier in
saga.h order, 191 use something that is still unknown.

### decomp.h and externals.h

`decomp.h` (282 items): the platform prelude (`<windows.h>` and friends,
`WIN32_LEAN_AND_MEAN`, `NOMINMAX`, `_WIN32_WINNT`, `#include "externals.h"`),
the scaffolding macros, 34 `*_VIEW` overlay types with their 162 asserts, 62
data-model checks (`sizeof(void *) == 4`, `sizeof(MSG) == 28`), and
`#include "debug/diagnostics.h"`. The views leave decomp.h when cleanup batch
10 turns them into real members.

`externals/*.h` (166 items): the libogg/libvorbis API that saga.h carries
(`externals/ogg.h`, 128; `lib/ogg-vorbis/vorbis/ogg/ogg.h` includes saga.h for
it), the 12 Bink exports (`externals/bink.h`) and 24 layout checks of the
DirectSound stand-ins in `src/externals.h` (`externals/dsound.h`). In step 3
they merge into `externals.h` (Bink, DirectSound: no SDK header in the build)
or are replaced by the original library header (libogg: see the open
questions).

### The 16 globals blocked in the rename batch

Commit 2f65e26a left 16 verified rows of `config/sibling-names.csv` open: the
original name (`addbolt_nosfx`, `Door_CutCamPos0`, ...) is already declared,
unused, in the transferred Android block (lines 29,186-30,668), and deleting
that declaration changed code generation (presumably the id count). The map
puts the declarations of each pair in one header (rule `same-entity`, the
better-evidenced side decides): 13 go to a module header (`door.h` 4,
`move.h` 2, `characters.h` 2, `nu3d_includes.h` 2, `bolts.h`, `pushblocks.h`,
`gameliball.h`), 3 to Android's `globals.h` (`Game_CompletionSave`,
`COMPLETIONPOINTS`, `Game_EpisodeSave`; to review), none stays unknown.

- 11 are globals defined in data.cpp. Rename the definition and the PC
  declaration to the original name and keep both declarations: two `extern`
  declarations of one name, so the count stays. Where the types differ
  (`setlastdoor_last` is `struct DOOR_s *`, `LastDoorPointerPc` `void *`;
  `Game_CompletionSave` `u16 *`, the PC one a reader struct pointer) the
  Android declaration takes the PC type. The duplicate goes in step 2 or 3,
  when its header no longer reaches the fragile units or under the exact-set
  check.
- 5 are file statics of their unit (`ForceBackObjPc`, `ForceBackTypePc` in
  move.c, `IconScenePc` in characters.cpp, `RendererAlphaMaskMaterialPc` and
  `RendererZClearMaterialPc` in nu3d_includes.cpp). A static cannot follow the
  `extern` of the same name, so the Android declaration must go with the
  rename. After the split it is in the owning module's header, so the count
  change reaches only units that include that header; do it in step 2, after
  the unit is narrowed, under the exact-set check.

## 2. Procedure

### Step 1: split behind the shim

Mechanics (`--apply`): each header gets its items with their contexts. A run
inside a top-level `#if` is wrapped in the original directive (a derived
`#if !(a) && (b)` when it starts in a later branch; consecutive branches reuse
the original `#else`/`#elif`), a run inside `extern "C"` gets its own
`#ifdef __cplusplus extern "C" {` block. A header with one run has a normal
guard (`SAGA_<PATH>_H`). A header with several runs gets numbered parts:
saga.h includes it once per part, in saga.h order; included directly, it gives
all parts at once:

```c
#if SAGA_NU2API_NUMATH_NUVEC_H_PART == 2 && !defined(SAGA_NU2API_NUMATH_NUVEC_H_STOP)
...items of part 2...
#undef SAGA_NU2API_NUMATH_NUVEC_H_PART
#define SAGA_NU2API_NUMATH_NUVEC_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUMATH_NUVEC_H_STOP
#endif
#endif
```

Includes: a header includes the headers it uses plainly when all of them come
earlier in saga.h (a no-op through the shim), otherwise inside
`#ifndef SAGA_SHIM` with the note "included via saga.h only for now: ..."
(order wins over self-containment). The shim is the guard, `#define
SAGA_SHIM`, and the includes and unplaced items in saga.h order (about 14,000
lines with the current map: 4,524 includes and 2,921 items still in place).

Procedure:

1. Review the map: give owners to the unknowns that units use (653 rows plus
   their asserts follow), mark reviewed rows `review`, decide `globals.h` and
   `legoapi_types.h` rows. Rerun `--build-map` after any saga.h change
   (rows are matched by key; reviewed rows are kept).
2. `python3 tools/headersplit.py --apply`; extend `check_headers` in
   `tools/check.py` to accept the headers named in the map; `make format`.
3. Proof A (preprocessor): `--check-neutral` on every unit, Matching and
   `--debug` (about 2 s per unit and mode with one clang process, about 25
   minutes for the tree). Run it again after `make format`.
4. Proof B (objects): `make -j2 build` and `check.py --objects`; code and data
   sections of every object identical to the base build, then `match.py
   --update` with the exact set by address unchanged. Debug compile.
5. Commit as one batch (tool output only, no hand edits), so `git revert`
   restores saga.h.

Optional before 3: an order probe. Prototypes and `extern`s cause 3,637 of the
runs. Per docs/cleanup-audit.md the fragile functions depend on the number of
ids a unit consumes, not on which declaration consumes them. A variant shim
that keeps types, macros and asserts in order (1,051 runs) and puts each
header's prototypes and `extern`s in one block per header would need about a
fifth of the parts. It is not token-neutral, so its only proof is step 4 for
every unit; if that holds, prefer it.

### Step 2: narrow units

For each unit, replace `#include "saga.h"` by the module headers it uses (a
unit sees about 16 headers directly, 86 with their dependencies, at the
median; data.cpp 283). Outside the shim every header gives all its parts at
once and includes its dependencies, so the unit sees fewer declarations, in
another order. Proof per batch: build, exact set by address unchanged
(`match.py --update`), the FRAGILE functions (tools/unitscore.py) compared
separately; a loss is bisected to the declarations that cause it and recorded
in docs/cleanup-audit.md. Units that still use an unknown item cannot narrow
(318 of 332 today): the review of step 1 is the gate.

Order (the declaration count matters most where fragile functions live, so
those go last):

1. units that need few headers (41 need fewer than 50 with dependencies;
   `unref.cpp`, `posteffect_shader_tokens_pc.cpp` and the diagnostic stubs
   one or none), the vendored `lib/ogg-vorbis` units (they need
   `externals/ogg.h`), `src/debug`;
2. the other units by number of headers needed (216 need 50 to 99 with
   dependencies);
3. the unity compilands (`nu3d_includes.cpp`, `nucore_includes.cpp`,
   `gameliball.cpp`, ...), `batman.cpp`, then `data.cpp` (data order only);
4. last, the units of the fragile functions: `legogame/episodeIII.c`
   (`BoulderSpawnPc`), `legogame/shop.c` (`SubItemMenu`), `legogame/game.cpp`
   (`InitGameAfterConfig`), `legogame/episodeIV.c`
   (`BlockadeRunnerDEjectCreaturePc`), `legoapi/parts.c` (`DebrisSetup2`,
   `NuRndrParticleGroup`), `nu2api/nu3d_platform_pc.cpp`. Each one alone,
   retuned as in the assert pilot when its count changes.

Batch size: 15-25 units per build (about 15 batches for the 332 units), fewer
for the unity files; one unit per batch for the six fragile units.

### Step 3: delete the shim

When no unit includes saga.h: delete it and `SAGA_SHIM`; merge each header's
parts into one body (`#if ..._PART` sections and the `#ifndef SAGA_SHIM`
wrappers go; the order inside a header may then change, proven by objects);
move the transitional catch-all rows (`globals.h`, `legoapi_types.h`) to their
modules; `externals/*.h` into `externals.h` (or the original library header);
`decomp.h` keeps the prelude, `STATIC_ASSERT`, `STUBBED` and the views until
batch 10. Update the tools that write into saga.h (`tools/globals.py`,
`tools/typeimport.py`: they take the target from `--place`) and the saga.h
special cases in `oddscan.py`, `declscan.py` and `layoutcheck.py`.

### Declarations in units (declscan classes C and D)

`--place NAME...` applies the same rules as the map to a name declared in a
unit: Android header, owning compiland by symbol or definition, single-compiland
users, Android catch-all last. Class D declarations (2,630) move to the
printed header during step 2, together with the narrowing of their unit, so
the one exact-set check covers both. Class C (376, forward declarations of functions defined later in
the same unit) mostly disappear when the unit reaches retail function order
(docs/declarations.md); a C name that another compiland also uses gets its
header from `--place`, the rest stay unit-local (`static` prototypes are not
header material).

## 3. Neutrality trial

`--check-neutral` on 12 units in a scratch copy (`build/headersplit/`,
deleted afterwards), Matching and Debug, with the map of this commit:
`legogame/episodeIII.c`, `legogame/shop.c`, `legogame/game.cpp`,
`legogame/episodeIV.c`, `legoapi/parts.c`, `nu2api/nu3d_platform_pc.cpp`
(all fragile units), `nu2api/shader_parser.cpp` (`SAGA_SHADER_PARSER_TYPES`),
`gameapi/aisysall.c` (`AISYSALL_SCRIPT_DIAGNOSTIC_PROTOTYPES`), `data.cpp`,
`batman.cpp`, `lib/ogg-vorbis/vorbis_smallft.c` (`SAGA_VENDORED_LIBRARY`) and,
as a control without saga.h, `debug/log.c`: all 24 token streams identical
(590,000 to 1,050,000 tokens each). A second sample, every 11th unit of
`git ls-files` (31 units, Matching): all identical. A deliberate swap of two asserts in one generated
header is reported at the first differing token. The C units report 19 clang
errors on both sides: the lint flags preprocess C as `-std=c89`, where the
`//` comment after `_WIN32_WINNT` in saga.h becomes part of the macro; that is
a lint-configuration issue, the same before and after.

The first trial found one real problem, now handled: `#ifndef
SAGA_HARDWARE_LIFECYCLE_PROPOSAL` / `#define SAGA_HARDWARE_LIFECYCLE_PROPOSAL`
inside saga.h made a split frame evaluate to false in its second part, so a
conditional that changes its own condition is one item.

## 4. Risks

- **Ids and macros.** Include guards and part macros are preprocessor names;
  per the assert pilot the fragile functions depend on symbol ids, which
  macros should not consume. Step 1's object proof settles it; the fallback
  is `#pragma once` for single-part headers.
- **Declaration count.** Every deletion of a declaration (dead `extern`s, the
  16 duplicates, the saga.h asserts the layout comments replace) changes the
  id count of every unit that sees it. Do those after the units are narrowed, header
  by header, so only the units that include the header are affected.
- **Header names that shadow SDK headers.** `nu2api/nuwin32/windows.h`,
  `legoapi/props/objects/signal.h` and `legoapi/items/objects/traffic.h` have
  the names of SDK/CRT headers. MSVC searches the directories of all open
  includers for a quoted include, so a quoted `"windows.h"` anywhere in an
  include chain from those directories would find ours. No unit spells it that
  way today; rename them in the map if one would.
- **Shared types without an owner.** 653 rows need a decision; a wrong owner
  only costs a later move, but the step 2 narrowing waits for them.
- **Tools that read saga.h.** `oddscan.py` (saga.h statistics), `declscan.py`
  (only the wording of a reason), `layoutcheck.py` (retries a header with
  `-include saga.h`), `globals.py` and `typeimport.py` (write into saga.h):
  adjust in step 3.
- **check_headers.** `tools/check.py` allows only a fixed list of project
  headers; the apply commit must extend it.
