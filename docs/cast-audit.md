# Cast audit: which casts are needed, and which hide a wrong type

A decompilation collects casts: each one made a function compile or match at
the time it was written, and few were removed when the types got better. This
page counts every explicit cast in `src/`, puts each one into a class by the
reason it is there, names the declarations and type pairs that cause the most
casts, and gives the order of the cleanup.

```sh
python3 tools/castscan.py --stats                       # counts per class and per cause
python3 tools/castscan.py --report                      # top 50 root causes; build/castscan/{casts.csv,groups.json,roots.json}
python3 tools/castscan.py --list redundant-identity,redundant-implicit src/legogame/bolt.c
python3 tools/castscan.py --check                       # class counts against config/castscan-budget.json (make check-core)
python3 tools/castscan.py --write-budget                # lower the budget after a cleanup
```

`tools/castscan.py` parses every unit with libclang and the lint flags
(`tools/declscan.py` helpers: 32-bit MSVC target, `-fms-extensions`, the unit's
language) and a second time with `-DSAGA_DEBUG` when the unit uses a diagnostic
macro, so casts that only the Debug build kind compiles are counted too (89). It
records every C-style cast, functional cast, `static_cast`, `reinterpret_cast`
and `const_cast` written in `src/`, with file, line, column, function, source
type, target type, the context the value goes to (assignment, argument,
comparison, arithmetic, ...) and the declaration the operand reads (variable,
field, parameter, function result). A cast in the body of a `src/` macro counts
once, at the `#define`; casts that `NULL` and SDK macros expand to are not ours
and are skipped. `lib/` is not scanned. A full scan takes about five minutes with
two parses at a time; `build/castscan/cache.json` keeps the facts per unit (the key
is the unit text and all `src/` headers), so a rerun after editing a unit parses
only that unit.

## 1. Classes

Each cast gets the first class whose rule applies, in this order. Snapshot after
the pilot below (`origin/main` of 2026-11-13 plus this branch): **27,805 casts**.

| Class | Casts | Cause | Rule |
|---|---:|---|---|
| redundant-identity | 809 | redundant | source and target are the same type after typedefs |
| redundant-implicit | 1,587 | redundant | the implicit conversion of an assignment, initialization, return, prototyped argument or integer promotion gives the same value without a diagnostic |
| discard | 1,284 | idiomatic | `(void)` of an unused value or parameter |
| null-pointer | 142 | idiomatic | `(T *)0` |
| address-constant | 52 | int-pointer | other integer constant cast to a pointer (fixed address, GUID table entry) |
| raw-offset | 377 | missing field | `(T *)((u8 *)p + 0x..)`, `(T *)((u32)p + n)`: a field or a struct is missing |
| embedded-base | 965 | idiomatic | the record is the first field of the other one (C inheritance: `edui_slider_s` starts with `eduiitem_s base`); an upcast can become `&p->base`, a downcast stays |
| view-type | 523 | duplicate type | a `*_VIEW` overlay type on either side |
| duplicate-type | 1,644 | duplicate type | a record defined in a unit, or under one name in several files, on either side |
| declaration-type | 1,710 | wrong declaration | the same declaration is cast to the same type at half or more of its uses (or the only use of a local): its declared type is wrong |
| layout-compatible | 295 | duplicate type | records with identical fields (offsets, sizes, kinds), or one a prefix of the other |
| function-pointer | 796 | wrong prototype | function or function-pointer cast: a prototype or a table element type does not fit |
| field-address | 339 | missing field | address of a field cast to another type (`(NUVEC *)&mtx->m30`): a row or sub-struct the field list lacks |
| byte-pointer | 1,051 | laundering | pointer cast from or to `void *`, `char *`, `u8 *` that is not one of the above |
| record-pun | 588 | wrong declaration | pointer to one record cast to an unrelated record |
| scalar-pun | 273 | steers code | `f32 *` <-> `u32 *` and similar bit access |
| int-pointer | 807 | int-pointer | pointer to integer or integer variable to pointer (`(u32)p + n`, handles) |
| const | 1,967 | const | qualifier-only cast, mostly `(char *)` of a `const char[]` name passed to a parameter that lacks `const` |
| void-pointer-cpp | 1,251 | necessary | `void *` to an object pointer in C++ (allocations, callbacks) |
| unprototyped-argument | 22 | necessary | argument of a call without prototype |
| vararg | 29 | necessary | argument in the variadic part of a call |
| enum, bool | 9 | idiomatic | integer <-> enum or bool |
| float-int | 3,066 | steers code | floating <-> integer |
| float-width | 3,980 | steers code | `double` <-> `float` |
| truncation | 2,088 | steers code | integer to a narrower integer |
| signedness | 1,951 | steers code | integer to another signedness |
| widening | 179 | steers code | value-preserving widening that changes the arithmetic type |
| class-hierarchy | 17 | idiomatic | C++ base/derived conversion |
| other | 4 | other | record construction, references |

By cause: steers code 11,537 (41%), duplicate type 2,462, idiomatic 2,417,
redundant 2,396, wrong declaration 2,298, const 1,967, necessary 1,302,
laundering 1,051, int-pointer 859, wrong prototype 796, missing field 716.
By syntax: C-style 25,478, `static_cast` 809, `reinterpret_cast` 780,
`const_cast` 736, functional 2. 380 casts sit in `src/` macro bodies.

Notes on the rules:

- *redundant* is decided from the types alone and is conservative: no pointer
  cast from an integer literal, no cast in an lvalue, `sizeof`, an unprototyped
  or overloaded call, an enum target in C++, and an arithmetic context only when
  the promoted source type is already `int`. The pilot removed 153 of them in
  four objects without a single changed byte.
- *declaration-type* groups the casts of one declaration by target type and
  mode (value, address, element, call result). A declaration outside `src/`
  (CRT, SDK) is never a candidate, and a const cast is not one either:
  `(char *)ConstName` wants a `const` parameter, not a non-const name.
- The numeric classes are counted, not judged: the pilot shows that some of
  them are needed for the exact build and some are not (section 3).
- `--report` groups the casts by root cause across classes: `Player[i]` cast to
  `GAMEOBJECT *` is `byte-pointer` in C and `void-pointer-cpp` in C++, but one
  declaration change removes both. Tag keywords are dropped so the C and C++
  spellings of a type (`struct nuvec_s *`, `nuvec_s *`) meet.

## 2. Top 50 root causes

4,473 root causes (one declaration or one record pair each) explain 11,417
casts; the 135 with ten or more casts explain 4,540, the 335 with five or more
5,832. Line numbers are those of the snapshot.

| # | Casts | Classes | Root cause | Declared at | Files |
|---:|---:|---|---|---|---:|
| 1 | 338 | byte-pointer, void-pointer-cpp | `variable Player (element): void *[8] -> gameobject_s *` | src/saga.h:16153 | 46 |
| 2 | 310 | record-pun | `APIOBJECT_PC <-> gameobject_s` (src/saga.h:1559, 6278) |  | 19 |
| 3 | 265 | view-type | `PC_SCENE_MTL_VIEW <-> numtl_s` (src/saga.h:17333, 2476) |  | 13 |
| 4 | 209 | layout-compatible | `NUVEC4 <-> nuvec_s` (src/saga.h:101, 84) |  | 13 |
| 5 | 195 | signedness | `field gameobject_s.id: u16 -> short` | src/saga.h:7001 | 41 |
| 6 | 157 | field-address | `field numtx_s.m30 (address): f32 -> nuvec_s *` | src/saga.h:134 | 40 |
| 7 | 109 | void-pointer-cpp, byte-pointer | `variable CurrentTerrain: void * -> TERRAIN_SET_PC *` | src/saga.h:16538 | 5 |
| 8 | 109 | declaration-type | `variable edui_font: void * -> vufnt_s *` | src/saga.h:28831 | 1 |
| 9 | 102 | byte-pointer, void-pointer-cpp | `field APIOBJECT_PC.owner: void * -> gameobject_s *` | src/saga.h:1560 | 6 |
| 10 | 92 | duplicate-type | `part_runtime_type_pc_s <-> parttype_lookup_pc_s` (src/gameapi/edpart.c:9) |  | 1 |
| 11 | 84 | signedness | `function EdFileReadChar (result): char -> unsigned char` | src/saga.h:15432 | 14 |
| 12 | 81 | duplicate-type | `cut_instance_load_pc_s <-> instnugcutscene_s` (src/legoapi/cutscene_lifecycle.c:749) |  | 1 |
| 13 | 79 | signedness | `field gameobject_s.field_280: u8 -> signed char` | src/saga.h:6391 | 23 |
| 14 | 79 | view-type | `PC_MATERIAL_CREATE_VIEW <-> numtl_s` (src/nu2api/nu3d_includes.cpp:4286) |  | 1 |
| 15 | 74 | declaration-type | `field bolt_s.owner: void * -> gameobject_s *` | src/saga.h:3851 | 10 |
| 16 | 65 | declaration-type | `variable gizmotypes: void * -> gizmotypes_pc_s *` | src/saga.h:13803 | 4 |
| 17 | 54 | declaration-type | `function NuStringTableGetByName (result): u16 * -> char *` | src/saga.h:13956 | 1 |
| 18 | 51 | const | `variable AIActionFalseName: const char[6] -> char *` | src/saga.h:14214 | 2 |
| 19 | 44 | duplicate-type | `LEVER_WORLD_PC <-> WORLDINFO_s` (src/legogame/lever.c:35) |  | 1 |
| 20 | 43 | record-pun | `TERRAIN_POLYGON_INFO_PC <-> TERRAIN_SHAPE_BATCH` (src/saga.h:642, 927) |  | 4 |
| 21 | 39 | duplicate-type | `permanent_material_reader_pc_s <-> numtl_s` (src/legoapi/screen.cpp:25) |  | 1 |
| 22 | 37 | declaration-type | `variable nu_input_state: NuKeyboardState_PC * -> PC_INPUT_RUNTIME_PC *` | src/saga.h:13692 | 2 |
| 23 | 36 | const | `variable AIGameCharacterName: const char[11] -> char *` | src/saga.h:14517 | 1 |
| 24 | 34 | void-pointer-cpp, byte-pointer | `field nuspecial_handle_s.scene: void * -> NUGSCN_s *` | src/saga.h:1072 | 12 |
| 25 | 34 | declaration-type | `variable id_ATAT: i16 -> unsigned short` | src/saga.h:14282 | 8 |
| 26 | 33 | duplicate-type | `GRAPHICS_DEVICE_PC <-> PC_D3D9_DEVICE_PC` (src/nu2api/pc_device.cpp:531) |  | 1 |
| 27 | 32 | duplicate-type | `debris_material_pc_s <-> numtl_s` (src/legoapi/parts.c:878) |  | 1 |
| 28 | 32 | duplicate-type | `PC_DI_RANGE_PC <-> PC_DI_PROPERTY_PC` (src/nu2api/pc_input.cpp:208) |  | 1 |
| 29 | 31 | byte-pointer | `field variptr_u.void_ptr: void * -> locator_prim_vertex_pc_s *` | src/saga.h:1944 | 1 |
| 30 | 29 | signedness | `function EdFileReadShort (result): i16 -> unsigned short` | src/saga.h:15430 | 9 |
| 31 | 29 | byte-pointer | `variable LevFlag: i32[4] -> unsigned char *` | src/saga.h:14378 | 5 |
| 32 | 28 | declaration-type | `field nudisplay_list_pc_s.items: void * -> nudisplaylistitem_s *` | src/saga.h:3315 | 2 |
| 33 | 27 | duplicate-type | `HATMACHINE_WORLD_PC <-> WORLDINFO_s` (src/legogame/hatmachine.cpp:37) |  | 1 |
| 34 | 27 | record-pun | `NUVEC4 <-> VuVec` (src/saga.h:101, 27156) |  | 1 |
| 35 | 27 | byte-pointer | `variable WORLD: WORLDINFO * -> void *` | src/saga.h:16151 | 3 |
| 36 | 26 | duplicate-type | `carry_basis_pc_s <-> gameobject_s` (src/legoapi/carrying.c:55) |  | 1 |
| 37 | 26 | duplicate-type | `nuqfnt_atlas_material_pc_s <-> numtl_s` (src/nu2api/nu3d_platform_pc.cpp:1151) |  | 1 |
| 38 | 26 | declaration-type | `variable EdLevelFnt: i32 -> void *` | src/legoapi/rtlhelper.cpp:18 | 2 |
| 39 | 26 | declaration-type | `variable id_SPEEDERBIKE: i16 -> unsigned short` | src/saga.h:12951 | 8 |
| 40 | 25 | duplicate-type | `techno_callback_pc_s <-> techno_pc_s` (src/legogame/techno.c:115) |  | 1 |
| 41 | 24 | duplicate-type | `CUT_LOAD_RECORD_PC <-> cutinfo_s` (src/legoapi/cutscenes.cpp:94) |  | 1 |
| 42 | 24 | const | `variable AIScriptActionNameArgPrefix: const char[6] -> char *` | src/saga.h:16791 | 2 |
| 43 | 24 | declaration-type | `variable GAMERAND (address): i32 -> nunrand_s *` | src/saga.h:15534 | 7 |
| 44 | 24 | declaration-type | `variable NuDebrisVertexData: void * -> nulgtvertex_pc_s *` | src/saga.h:13344 | 1 |
| 45 | 23 | declaration-type | `field SoundQueue.volume_sink: SoundQueueVolumeSink * -> IDirectSoundBuffer *` | src/saga.h:19404 | 1 |
| 46 | 23 | signedness | `field gameobject_s.jump_animation_state: u8 -> signed char` | src/saga.h:6404 | 5 |
| 47 | 23 | duplicate-type | `charconfig_variant_reader_pc_s <-> PLAYERCHARACTERCONFIG_PC` (src/legogame/config.c:31) |  | 1 |
| 48 | 22 | field-address | `field numtx_s.m20 (address): f32 -> nuvec_s *` | src/saga.h:130 | 11 |
| 49 | 21 | void-pointer-cpp | `field variptr_u.void_ptr: void * -> float *` | src/saga.h:1944 | 3 |
| 50 | 20 | duplicate-type | `cut_instance_lifecycle_pc_s <-> instnugcutscene_s` (src/legoapi/cutscene_lifecycle.c:137) |  | 1 |

What the list says:

- **Untyped globals and fields in `src/saga.h`** are the largest single
  cause: `Player[8]` (338), `CurrentTerrain` (109), `edui_font` (109),
  `APIOBJECT_PC.owner` (102), `bolt_s.owner` (74), `gizmotypes` (65),
  `nuspecial_handle_s.scene` (34) are `void *` that every user casts to the
  same type.
- **Two names for one struct**: `APIOBJECT_PC` is a second decomposition of
  `GAMEOBJECT` (310); `PC_SCENE_MTL_VIEW`, `PC_MATERIAL_CREATE_VIEW` and four
  unit-local material readers overlay `NUMTL` (265 + 79 + 39 + 32 + 26);
  `NUVEC4` starts with the fields of `NUVEC` (209, plus 27 with `VuVec`); unit
  copies of `WORLDINFO`, `instnugcutscene_s`, `cutinfo_s` and
  `PLAYERCHARACTERCONFIG_PC` carry a dozen to 100 casts each.
- **Missing rows**: `numtx_s` has no `NUVEC` view of its rows, so
  `(NUVEC *)&m->m30` appears 157 times (and `m20` 22).
- **Signedness of a declaration** (`gameobject_s.id`, `field_280`,
  `EdFileReadChar`, `id_ATAT`): the declaration may well be wrong, but every
  other use of it compiles with the new signedness, so these are matching
  experiments, not mechanical fixes.

## 3. Pilot

On this branch, units chosen away from `src/saga.h` and the concurrent rename
batch. Every touched object was compiled before and after with
`make -j1 build/obj/<unit>.obj` and compared byte for byte (COFF header time
stamp ignored); the Debug kind of each touched unit compiles.

| Change | Casts removed | Objects |
|---|---:|---|
| Redundant casts of `legogame/bolt.c` (56), `legogame/gizforce.c` (55), `gameapi/apiobject.c` (39 of 48) | 150 | identical |
| `TerI` declared `TerrainQuery_s *` in `data.cpp` and the four unit externs; five alias macros (`TERRAIN_QUERY_PC`, `TERI`, ...) gone; one `static_cast` added for the C++ scratch allocation | 125 − 1 | identical (data, terrain_scan, terrain_collision, terrain_query, nuterrain) |
| `dir_list` declared `struct FILE_DIRECTORY_ENTRY_PC *` (`data.cpp`, `edfile.c`); its directory bit named `FILE_DIRECTORY_FLAG_DIRECTORY_PC` | 16 | identical |
| `podrace_netpacket` typed; the two local Pod race packet structs, which differed only in the word at 0x0c (`u32 unknown_0c` / `f32 lap_alpha`), merged into `PODRACE_NETPACKET_PC` | 10 | identical |
| `vadera_netpacket` typed `VADERA_NETPACKET_PC *` | 6 | identical |

Total 309 casts (28,114 to 27,805), four declarations and one duplicate type
fixed. Nine redundant casts in the animation-flag tests of
`APIDrawCharacterModel` were put back: without them the textual
`tools/oddscan.py` sees the existing `0x20`/`0x220` flag tests, and its
`magic-flag`/`magic-mask` budgets would have to be raised (a reviewed
exception). They can go together with names for those bits, or with an
approved raise. The same thing happens to any batch that uncovers a magic
number; naming the constant is the clean way. After a full build the exact function set is the same as
`origin/main`'s by address (11,821 before and after, none lost or gained).

**Casts found necessary.** To calibrate the steering classes, every
`signedness` and `truncation` cast of the three redundant-pilot units that sits
in an assignment, initialization, argument or return (where the implicit
conversion gives the same bits) was removed alone and the unit rebuilt: 34 of
43 left the object identical, 9 changed it and stay:

| Site | Cast | Why it is needed |
|---|---|---|
| `legogame/bolt.c:49` | `id = (i16)object->id` (u16 field into an i16 local) | with the cast the field is loaded with `movsx` (`movswl 0x1064(%ecx)`), without it with `movzx`; the original sign-extends |
| `legogame/gizforce.c:516`, `1627`, `1628`, `1629` | `(u16)` of an i16 field passed to `NuVecRotateX/Y` and `AddPickups` | the cast makes the load `movzx`; without it the field is loaded with `movsx` (checked for 1627) |
| `legogame/gizforce.c:1684`, `1708` | `(u16)qrand()` passed to `NuVecRotateY` | the cast emits the explicit `movzwl %ax, %edx` before the push; without it the int result is pushed as is and the register allocation changes (checked for 1684) |
| `gameapi/apiobject.c:1939`, `1943` | `interrupted_reversed = (i8)packet->reversed_3c` (u8 -> i8) | not disassembled; the object changes, presumably the load extension as in bolt.c:49 |

The 34 neutral ones were put back as well: they state an intended narrowing,
and removing them trades a visible cast for an implicit one (C4244 at /W4,
`-Wconversion` in lint). They are not a cleanup target; section 4 leaves the
steering classes alone.

## 4. Batch plan

Ordered by casts removed per change and by risk to matching. "Local" means the
declaration or type lives in one unit or a small header, so a batch rebuilds a
few objects and can be proven byte for byte; a `src/saga.h` change rebuilds
everything and needs the full exact-set comparison (the fragile functions in
`docs/cleanup-audit.md` react to declaration counts, not to a changed type, but
the check is needed anyway).

| # | Batch | Casts | Changes | Risk | How |
|---:|---|---:|---|---|---|
| 1 | Redundant casts, one directory per commit | ~2,300 of 2,396 (C-style, outside macros; 183 files) | 5-8 commits | none: objects byte-identical | delete with the classes of `--list redundant-identity,redundant-implicit`, drop the parentheses left behind, compare each object |
| 2 | Unit-local declarations (`declaration-type` with a declaration in a `.c`/`.cpp`; `EdLevelFnt`, `edpp.c` item parameters, level packets, `hub.cpp` bonus gizmo) | ~560 | ~150 | low: one or two objects each | as the pilot: retype, delete the casts, compare objects |
| 3 | Unit copies of `src/saga.h` structs (`duplicate-type`, `view-type` with a local side: `edpart.c`, `cutscene_lifecycle.c`, `lever.c`, `hatmachine.cpp`, `screen.cpp`, `parts.c`, `techno.c`, `config.c`, `cutscenes.cpp`, `pc_device.cpp`, `pc_input.cpp`) | ~1,800 | ~100 types | low to medium: field names and types must be merged into the real struct; one unit each | move missing fields into the `src/saga.h` struct only when another unit needs them, otherwise rename the fields of the local copy's users |
| 4 | `void *` globals and fields of `src/saga.h` (`Player`, `CurrentTerrain`, `edui_font`, `APIOBJECT_PC.owner`, `bolt_s.owner`, `gizmotypes`, `nuspecial_handle_s.scene`, `nudisplay_list_pc_s.items`, `NuDebrisVertexData`, ...) | ~1,150 (declaration-type) + ~700 (laundering roots) | ~40 declarations, 3-4 `src/saga.h` commits | medium: full build and exact set; C++ units keep a cast where they assign an allocation | coordinate with the `src/saga.h` struct-comment conversion; one commit per group of declarations |
| 5 | Record pairs in `src/saga.h`: `NUVEC4` embeds `NUVEC`, `numtx_s` gets row vectors, `PC_SCENE_MTL_VIEW` into `NUMTL`, `APIOBJECT_PC` into `GAMEOBJECT`, `TERRAIN_POLYGON_INFO_PC`/`TERRAIN_SHAPE_BATCH` | ~1,000 | 5-6 type changes | medium to high: layouts must be proven equal (`STATIC_ASSERT`s), every user recompiles | one pair per commit, full build each |
| 6 | `const`: string-name tables (`AIActionFalseName`, `AIGameCharacterName`, ...) passed to `char *` parameters | ~1,000 of 1,967 | the prototypes of the receiving helpers | low for code (const does not change code), medium for churn | make the parameters `const char *`; the casts go with them |
| 7 | Function-pointer casts (`function-pointer`) | ~400 of 796 | table element types and prototypes | medium | type the tables; callbacks that really take different arguments keep their cast |
| 8 | Signedness of declarations (`gameobject_s.id`, `field_280`, `EdFileReadChar/Short`, `id_*` globals) | ~450 | ~10 declarations | high: the pilot shows that the load extension follows the declared type | only as matching experiments, each with a full exact-set comparison |
| — | Steering classes (`float-int`, `float-width`, `truncation`, `signedness`, `widening`, `scalar-pun`), necessary (`void-pointer-cpp`, vararg, unprototyped) and idiomatic (`discard`, `null-pointer`, `embedded-base`, enum, bool) | ~15,000 | — | — | not a target; new casts of these classes still raise the budget, which shows them in review |

Batches 1-7 remove an estimated 8,000 to 9,000 casts (about 30% of 27,805);
batches 1-3 alone, which need no `src/saga.h` change, about 4,600. After each
batch `python3 tools/castscan.py --write-budget` lowers the budget so that
`make check-core` keeps the gain.
