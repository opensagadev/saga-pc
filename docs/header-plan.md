# Header plan: from saga.h to module headers

`src/saga.h` (33,569 lines) is the one header nearly every unit includes. The
owner's decisions (2026-11-13): saga.h goes; declarations live in real module
headers following the Android decompilation's tree (`../saga-android/src`);
units stop forward-declaring and declaring `extern`s themselves; enums and
flags live in the header of the struct that uses them; `decomp.h` (as in the
Android tree) takes decomp-only scaffolding; `externals.h` stays only for what
has no retail header.

Decisions on the first draft of this plan (2026-11-14):

1. Android's catch-alls `globals.h` and `legoapi/legoapi_types.h` are
   acceptable step-1 targets. Their rows say `catch-all (needs a module
   owner)` in `detail`, so a later pass can find them and give them owners.
2. The libogg/libvorbis API comes from the original library headers, not
   from copies in saga.h or an externals file (section 1.4).
3. No shim of 4,500 exact-order fragments. The default split groups each
   header's items and reorders them where that is provably inert. The shim is
   a list of whole-header includes, and the few headers that still need parts
   are listed with the reason (section 2). Token neutrality no longer holds.
   The proof is: the same declarations per unit, identical objects per unit,
   and the same exact set by address (section 3).
4. Shared declarations follow one rule: the compiland that defines it, else
   the one whose units use it most, with ties going to the lowest address
   (evidence `rule`). Asserts follow their type. Declarations no unit uses are
   listed as deletion candidates with a proposed owner. The ones from the
   Android-transferred sections go to `transitional/unused.h`, which the
   shim includes and step 3 deletes. Targets that shadow SDK/CRT header
   names are renamed.

## Status (2026-11-14): step 1 tried, not applied

The split was applied on origin/main 352eb5eb (branch `wip-split-narrow`,
not merged). `--build-map` and `--apply`, then `make format`, wrote 421
module headers; `src/saga.h` became 612 lines: guard, comment,
`#define SAGA_SHIM` and 602 includes. Units were unchanged: every one still
included `saga.h`.

Result against 352eb5eb (full build, `match.py --update`):

- Exact set by address: 13,224 before, 13,223 after.
  `BlockadeRunnerDEjectCreaturePc` (`legogame/episodeIV.c`) drops from
  exact to 95.18%. `BoulderSpawnPc` and `SubItemMenu` stay exact.
- Reproduced data: 970,153 bytes before and after.
- 366 of 373 objects are identical apart from the COFF timestamp. Of the
  other seven, three differ only in names: `pc_device.obj` and
  `shader_dictionary.obj` in the number of one compiler label
  (`$submit$N`, `$allocate_first$N`), `edlevelall.obj` in the order of its
  symbol table. Four change code:

| unit | function | objdiff before | after |
|---|---|---:|---:|
| `legogame/episodeIV.c` | `BlockadeRunnerDEjectCreaturePc` | exact | 95.1754 |
| `legogame/shop.c` | `DrawSubItems` | 82.9484 | 82.9811 |
| `gamelib/net/NetStats.cpp` | `NetStats::Draw` | 99.9353 | 99.9727 |
| `legogame/game.cpp` | `InitGameAfterConfig` | 82.2397 | 82.2397 (other bytes) |

On the earlier base e4aa6520 (421 vs 420 headers, fewer declarations) the
same split kept all exact functions and moved `NuRndrParticleGroup` and
`NuDeferredFilterGen::render` instead of `BlockadeRunnerDEjectCreaturePc`
and `NetStats::Draw`: which fragile function moves depends on the id count
of the day (section 6, "Files as ids").

The decision was to recover a lost function by narrowing its unit (step 2)
in the same change. That is not possible yet (next section), so the split
waits.

Changes that came with it (tool and build changes, usable without the
split):

- `--apply` carries `// NOLINTBEGIN(...)` / `// NOLINTEND(...)` regions into
  every header that receives part of one (a run of asserts alone gets
  none). Before, `fixed_width.h` got the BEGIN and `decomp.h` an unmatched
  END, which clang-tidy rejects.
- The Makefile rebuilds objects when any `src/**/*.h` changes, not only
  saga.h.
- On the branch: `port.py` reads PC structs from every project header, and `globals.py`
  checks every project header before it declares a name. It still writes
  new declarations into its section at the end of saga.h (the shim), which
  is where an unmapped declaration stays. `typexfer.py` and
  `typeimport.py` edit struct text in saga.h in place; they stop with a
  message until step 3 teaches them the module headers.
- On the branch: oddscan budgets `data-file` (the shim), `dup-type` (names saga.h already
  declared twice now sit in two files) and `nolint` (the regions repeated
  per header) go up, with reasons in `config/budget-raises.txt`.
- After the split, `--build-map`, `--apply`, `--stats`, `--roundtrip`,
  `--check-counts`, `--check-neutral` and `--exact-order` have no unsplit
  saga.h to read; `--place` (the target header of a name) keeps working.

### Step 2 is blocked on self-contained headers

The plan for the fragile units was to narrow them in the same change. It is
not possible yet: the generated headers cannot be included on their own.

- Each header was compiled alone (`#include "H"` in an empty C file,
  clang with the lint flags): 19 of 421 compile. `decomp.h` does not:
  `fixed_width.h`, which it includes first, uses `STATIC_ASSERT` before
  `decomp.h` defines it, and does not include `decomp.h`.
- A header's own includes are listed at its top. In the shim they are
  no-ops; outside it a header with parts includes everything all its parts
  need before part 1. Many of those headers include it back, so the first
  part they need is not there yet. Example: `nuvec.h` includes `numtx.h`
  for its second part, `numtx.h` includes `nuvec.h` for `NUVEC`.
- The include graph has one cycle of 120 headers, `decomp.h` among them.
  `legogame/episodeIV.c` uses 45 headers by name (`config/header-map.csv`);
  their closure is 166 headers, 68 of them in parts. `legogame/shop.c`
  uses 47, closure 176. Replacing `#include "saga.h"` by those lists (own
  header, engine, game) stops VC8 at 100 errors in both units, the first in
  `fixed_width.h`.

So no include list a programmer would write compiles for any of the
fragile units, and order within the unit cannot fix it: a direct include
gives all parts of a header at once. What has to come first is step 3's
ownership work, at least for the headers in the closure of a narrowed
unit: move the joint types (`WORLDINFO_s`, `AIPACKET_PC`, `gameobject_s`,
the view types of `decomp.h`) to headers both sides include, so the
headers have one part each and no cycle; give `decomp.h`'s prelude and
`STATIC_ASSERT` a header that everything includes first. A header that
compiles alone is the test (`clang -fsyntax-only` per header, C and C++).

The transition has three steps, each with its own proof. VC8's code for a few
fragile functions depends on how many symbol ids a unit consumes before them
(docs/cleanup-audit.md, section 7).

1. Split saga.h into module headers behind an include-all shim. Every unit
   sees the same declarations, grouped by header.
2. Narrow each unit's includes to the headers it needs. Proof: the exact set.
3. Delete the shim and the transitional headers, and merge the remaining
   header parts. `decomp.h` and `externals.h` keep only their own content.

The tool is `tools/headersplit.py` (test `tools/test_headersplit.py`). The
mapping is `config/header-map.csv`. `tools/check.py` accepts the map's
target headers as project headers. Snapshot: origin/main a289e8ae.

```sh
python3 tools/headersplit.py --roundtrip                 # the segmentation gives saga.h back byte for byte
python3 tools/headersplit.py --build-map                 # regenerate the map (keeps rows with evidence "review")
python3 tools/headersplit.py --stats                     # items per kind, evidence, target
python3 tools/headersplit.py --interleaving              # headers that still need parts, and why
python3 tools/headersplit.py --dry-run                   # headers and shim it would write
python3 tools/headersplit.py --apply [--root DIR]        # write the split into DIR/src, formatted
python3 tools/headersplit.py --check-counts UNIT... [--debug]   # same declarations before/after (section 3)
python3 tools/headersplit.py --exact-order ...           # saga.h order instead of grouped (fallback)
python3 tools/headersplit.py --check-neutral UNIT... [--debug]  # token identity of the --exact-order split
python3 tools/headersplit.py --place NAME...             # target header of names declared in units (declscan C/D)
```

## 1. The map

### 1.1 Items

The segmenter cuts saga.h into 15,980 top-level items and 378 context
directives: 32 top-level `#if` frames with 7 `#else`/`#elif` branches, and
153 `extern "C"` blocks. One item can be any of these:

- a struct body,
- a prototype list split by an `#ifdef`,
- a `#pragma pack` region,
- a conditional that changes its own condition (`#ifndef X` / `#define X`
  ...).

Concatenating the items and directives gives saga.h back byte for byte.

Every item has a target: 15,980 of 15,980 items, all 32,455 item lines.

| kind | items |
|---|---:|
| STATIC_ASSERT | 5,155 |
| extern variable | 4,924 |
| function prototype | 4,455 |
| struct | 752 |
| forward declaration | 350 |
| enum | 110 |
| typedef | 87 |
| class | 65 |
| macro | 48 |
| union, include, `#pragma pack` region, other | 34 |

### 1.2 Evidence (`config/header-map.csv`)

Each row describes one item:

- `key`: kind and first name, with `#n` for repeats. The tool matches rows
  by key; line numbers are informational.
- `kind`, `names`, `line_start`, `line_end`, `lines`.
- `target`: a path under `src/`.
- `evidence` and `detail`.

The evidence classes, strongest first:

| evidence | items | rule |
|---|---:|---|
| `decomp` | 166 | platform prelude (SDK includes, `WIN32_LEAN_AND_MEAN`, `externals.h`), scaffolding macros, `*_VIEW` overlay types, data-model checks, layout checks of SDK and library types |
| `library` | 79 | the saga.h copy of a libogg/libvorbis declaration (section 1.4) |
| `externals` | 36 | Bink exports (`tools/binkw32.def`, `externals/bink.h`), layout checks of `src/externals.h` types (`externals/dsound.h`) |
| `android` | 4,627 | an Android header (not a catch-all) declares the same name; ties go to the shorter path |
| `symbol` | 1,288 | function: `config/symbols.csv` address in a `config/layout-target.csv` compiland |
| `definition` | 163 | global defined in exactly one unit other than data.cpp |
| `android-stem` | 159 | Android declares the PC name without its `_PC`/`Pc` suffix, or with other case |
| `users` | 2,457 | units of exactly one compiland use the name (data.cpp excluded) |
| `type-users` | 60 | a type that only the items of one header use |
| `rule` | 728 | shared declaration: owner by the rule below (section 1.3) |
| `data-order` | 411 | a global whose nearest placed neighbours in retail data, on both sides, go to the same header |
| `data-nearest` | 188 | a global only data.cpp mentions: the header of its nearest placed neighbour by retail address |
| `same-entity` | 12 | the PC and original name of one global (config/sibling-names.csv), see 1.6 |
| `android-weak` | 286 | declared only in an Android catch-all (`globals.h`, `decomp.h`, `batman.h`, stubs) |
| `unused` | 297 | no unit uses it: a deletion candidate (section 1.5) |
| `follow-type` | 5,019 | STATIC_ASSERT: the header of the type it checks |
| `follow-definition`, `follow-next` | 4 | forward declaration with its definition; `#undef` with the next item |

A compiland-based target uses the Android path of the module when it exists,
tried in this order (`header_for_compiland`):

1. the Android header with the compiland's (or unity member's) stem,
2. else the Android unit's directory,
3. else the PC directory.

The map has 419 target headers. The largest:

| header | items |
|---|---:|
| `nu3d_includes.h` | 729 |
| `legoapi/legoapi_types.h` | 682 |
| `gameliball.h` | 530 |
| `apiobject.h` | 491 |
| `move.h` | 381 |
| `nu2api/PCInput.h` | 368 |
| `levels.h` | 366 |
| `decomp.h` | 330 |
| `transitional/unused.h` | 329 |
| `globals.h` | 276 |

Catch-alls (decision 1): 958 rows target `legoapi/legoapi_types.h` (682) or
`globals.h` (276). By evidence:

| evidence | rows |
|---|---:|
| `follow-type` (asserts) | 483 |
| `android-weak` | 263 |
| `android` | 155 |
| `android-stem` | 46 |
| `type-users`, `same-entity`, `rule` | 11 |

Every one of these rows has `detail` starting with
`catch-all (needs a module owner)`.

Renamed targets (decision 4): the module names `windows.h`, `signal.h` and
`traffic.h` collide with SDK/CRT headers. MSVC searches the directories of
every open includer for a quoted include, so these names are unsafe. The map
renames them:

| old name | new name | rows |
|---|---|---:|
| `nu2api/nuwin32/windows.h` | `nu2api/nuwin32/nu2api_windows.h` | 30 |
| `legoapi/props/objects/signal.h` | `legoapi/props/objects/legoapi_signal.h` | 18 |
| `legoapi/items/objects/traffic.h` | `legoapi/items/objects/legoapi_traffic.h` | 1 |

The check is against every header name in the pinned SDK and CRT include
directories. The `detail` of each renamed row says `renamed from ...`.

### 1.3 Shared declarations: the rule

A declaration used by units of two or more compilands (or by none) gets its
owner by the first rule that applies:

1. A variable or function defined in a unit other than data.cpp goes to the
   defining compiland. If several define it, the lowest address wins.
2. Otherwise it goes to the compiland whose units mention its names most
   often: occurrences counted per file, data.cpp excluded. Ties go to the
   compiland with the lowest address.
3. A type that only saga.h uses goes where most of its saga.h users go. A
   use of a typedef or macro counts as a use of what it stands for. A struct
   whose units reach it only through a forward typedef
   (`typedef struct debris_key_pc_s DEBRISKEY_PC;`) goes where the typedef
   went.

This decides 728 rows:

| kind | rows |
|---|---:|
| variable | 411 |
| struct | 179 |
| forward | 60 |
| function | 29 |
| typedef | 22 |
| class | 14 |
| union, `#pragma pack` region, macro, block | 13 |

1,760 STATIC_ASSERTs follow these types (`follow-type`). This resolves most of the
1,918 asserts that the first draft left on unknown types; the rest follow
types placed by other rules.

Two more rules cover the rest:

- `data-nearest`: a global only data.cpp mentions (188) goes to the header of
  its nearest placed neighbour in retail data.
- `type-users`: a type only one header's items use goes there.

The 50 largest `rule` rows:

| name | kind | lines | target | why |
|---|---|---:|---|---|
| `PLAYERCHARACTERCONFIG_PC` | struct | 157 | `legoapi/characters/motion/move.h` | used most in legogame/move.c (25) |
| `PC_D3D9_METHODS_PC` | struct | 131 | `nu2api/nufmv/pc/Bink/dsplay_bink.h` | used in saga.h only, most by nu2api/nufmv/pc/Bink/dsplay_bink.h (1) |
| `PC_DEVICE_BACKEND_PC` | struct | 125 | `nu2api/nuwin32/d3dCore.h` | used most in nu2api/nuwin32/d3dCore.cpp (44) |
| `gamesave_s` | struct | 90 | `legogame/game.h` | used most in legogame/game.c (2) |
| `debris_info_pc_s` | struct | 90 | `gameapi/edtools/edgra.h` | used most in gameapi/gameliball.cpp[edgra.c] (65) |
| `PART_PC` | struct | 90 | `legoapi/items/objects/objectsall.h` | used most in gameapi/objectsall.cpp (56) |
| `EdClassInterface` | class | 89 | `editor/edlevelall.h` | used most in editor/edlevelall.cpp (16) |
| `GIZMOBLOWUP_PC` | struct | 88 | `legoapi/gizmo/object/gizmoblowups.h` | used most in legoapi/gizmoblowups.c (104) |
| `PC_WINDOW_OPTIONS_PC` | struct | 73 | `nu2api/pc_options.h` | used most in nu2api/pc_options.cpp (33) |
| `debris_key_pc_s` | struct | 71 | `gameapi/edptl.h` | with its typedef DEBRISKEY_PC (rule) |
| `FRAME_PAD_PC` | struct | 59 | `gameapi/edtools/edui.h` | used most in gameapi/edtoolsall.cpp[edui.cpp] (52) |
| `ShaderCompilerDictionaryPc` | struct | 58 | `gamelib/util/CRC16.h` | used most in nu2api/CRC16.cpp (25) |
| `PC_SPECIAL_SHADOW_LIGHT` | struct | 57 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (16) |
| `parttype_lookup_pc_s` | struct | 53 | `legoapi/items/objects/objectsall.h` | used most in gameapi/objectsall.cpp (9) |
| `nurndrlibscene_s` | struct | 52 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (3) |
| `FRAME_SCANNED_PAD_PC` | struct | 52 | `nu2api/huffdecode.h` | used most in nu2api/nucore_includes.cpp[huffdecode.c] (2) |
| `ADDPART_PC` | struct | 48 | `legoapi/items/objects/game_obj.h` | used most in legogame/game_obj.c (8) |
| `GIZMOPICKUP_PC` | struct | 46 | `legoapi/gizmos/fx/gizmopickups.h` | used most in legoapi/gizmopickups.c (36) |
| `GIZMO_LEVER_PC` | struct | 43 | `legoapi/gizmos/object/lever.h` | used most in legogame/lever.c (51) |
| `nudisplay_list_pc_s` | struct | 42 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (45) |
| `player_batarang_slot_pc_s` | struct | 41 | `legogame/batman_gameai.h` | used most in legogame/batman_gameai.c (9) |
| `PC_INPUT_CONFIG_PC` | pack | 40 | `nu2api/PCInput.h` | used most in nu2api/PCInput.cpp (20) |
| `ShaderCompilerParserPc` | class | 40 | `nu2api/shader_parser.h` | used most in nu2api/shader_parser.cpp (25) |
| `FLOWBOX_PC` | struct | 39 | `legoapi/gizmo/base/gizflow.h` | used most in legoapi/gizflow.c (36) |
| `HINT_PC` | struct | 36 | `legoapi/menus/core/hint.h` | used most in legoapi/hint.c (20) |
| `GRAPPLE_MOTION_PC` | struct | 36 | `legoapi/gizmos/transport/grapples.h` | used most in legoapi/grapples.c (68) |
| `PC_FRAMEBUFFER_PC` | block | 35 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (30) |
| `EdStream` | class | 35 | `editor/edlevelall.h` | used most in editor/edlevelall.cpp (32) |
| `CHARACTERANIM_CONFIG_PC` | struct | 34 | `legoapi/items/base/apiobject.h` | used most in gameapi/apiobject.c (19) |
| `area_data_pc_s` | struct | 32 | `legoapi/lib/hub.h` | used most in legogame/hub.cpp (14) |
| `WavReader` | pack | 32 | `nu2api/wavreader.h` | used most in nu2api/wavreader.cpp (15) |
| `TERRAIN_SET_PC` | struct | 31 | `legoapi/render/core/gameliball.h` | used most in gameapi/gameliball.cpp (113) |
| `PC_SHADER_PARAMETER_LOCAL_PC` | struct | 31 | `nu2api/nu3d/android/nushaderobject.h` | used most in nu2api/nushaderobject.cpp (14) |
| `EdInputContext` | struct | 31 | `editor/edlevelall.h` | used most in editor/edlevelall.cpp (56) |
| `AREA_SAVE_PICKUP_PC` | struct | 30 | `legoapi/items/collect/shard.h` | used most in legoapi/shard.c (2) |
| `GAMEPAD_PC` | struct | 30 | `legoapi/characters/motion/move.h` | used most in legogame/move.c (18) |
| `add_game_message_pc_s` | struct | 30 | `legoapi/gizmo/object/gizmopickup.h` | used most in legogame/gizmopickup.c (7) |
| `NuKeyboardOwner_PC` | struct | 29 | `nu2api/PCInput.h` | used most in nu2api/PCInput.cpp (11) |
| `GIZMOBLOWUPTYPE_PC` | struct | 29 | `legoapi/gizmo/object/gizmoblowups.h` | used most in legoapi/gizmoblowups.c (45) |
| `EdRef` | class | 29 | `editor/edlevelall.h` | used most in editor/edlevelall.cpp (96) |
| `RTL_DATA_PC` | struct | 27 | `legoapi/render/core/rtl.h` | used most in gameapi/rtl.c (15) |
| `EdControl` | class | 27 | `editor/edlevelall.h` | used most in editor/edlevelall.cpp (45) |
| `nuvpcurrent_s` | struct | 26 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (6) |
| `GRAPHICS_RENDER_TEXTURE_PC` | struct | 26 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (70) |
| `ANI3_HEADER_PC` | struct | 25 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (41) |
| `mission_sys_pc_s` | struct | 25 | `legoapi/world/missions.h` | used most in legoapi/missions.c (11) |
| `ARCADE_ITEM_PC_s` | struct | 24 | `legoapi/world/worldinfo.h` | used most in legoapi/worldinfo.cpp (1) |
| `NUHGOBJ_EVAL_PC` | struct | 24 | `nu2api/nu3d/nu3d_includes.h` | used most in nu2api/nu3d_includes.cpp (26) |
| `BURN_PARAMETERS_PC` | struct | 24 | `legoapi/render/fx/game_deb.h` | used most in legogame/game_deb.c (46) |
| `EdEnumControl` | class | 24 | `editor/edlevelall.h` | used most in editor/edlevelall.cpp (29) |

### 1.4 libogg and libvorbis

What `lib/ogg-vorbis` materialises (`config/ogg-vorbis.json`, pinned archive
of commit 6370ba4a, `src/vendored/`):

- `vorbis/vorbis/codec.h` and `vorbis/vorbis/vorbisfile.h` are the libvorbis
  headers. `nu2api/oggreader.cpp` and `nu2api/nustream.cpp` already include
  `"vorbis/vorbisfile.h"`.
- `vorbis/ogg/ogg.h`, `vorbis/ogg/os_types.h` and `vorbis/smallft.h` are
  stubs that include `saga.h`. saga.h carries the content they stand for:
  - the libogg API: token-identical to libogg 1.1.3 `include/ogg/ogg.h`;
  - the `ogg_int*_t` typedefs: identical to the `_WIN32` MSVC branch of
    `os_types.h`;
  - the `drft_*` declarations: libvorbis 1.2.0 `lib/smallft.h`, apart from
    `extern` and parameter names.

Verification of the library layouts:

- clang `-fdump-record-layouts` (i686 MSVC target, the lint flags) of
  `oggpack_buffer`, `ogg_page`, `ogg_stream_state`, `ogg_packet`,
  `ogg_sync_state` and `drft_lookup`.
- One run with saga.h, one with the upstream `ogg/ogg.h` + `os_types.h` +
  `smallft.h`.
- Result: field offsets and sizes are identical. The saga.h STATIC_ASSERTs on
  these types stay in `decomp.h` as layout checks of a library type.

So the units should get the API from the library headers. That needs the pin
to carry the upstream `ogg/ogg.h`, `os_types.h` and `smallft.h` instead of
the stubs. The vendored units' includes then resolve as today, through
`-Ilib/ogg-vorbis/vorbis`.

Until then, the split moves the 79 copied declarations into holding headers
(evidence `library`):

- `transitional/libogg/ogg.h`
- `transitional/libogg/os_types.h`
- `transitional/libvorbis/smallft.h`

They cannot take the library names under `src/`. MSVC's quoted-include
search (`/Isrc` comes before `/Ilib/ogg-vorbis/vorbis`) would find
`src/ogg/ogg.h` or `src/smallft.h` before the stub from a vendored unit, and
that unit would lose the rest of saga.h.

When the pin changes:

1. delete the holding headers;
2. the shim (or, after step 2, the units that use the API: `oggreader.cpp`,
   `nustream.cpp`, the vendored units) includes `<ogg/ogg.h>` /
   `"vorbis/vorbisfile.h"`;
3. prove it with `--check-counts` and objects.

### 1.5 Declarations no unit uses: deletion candidates

297 declarations have no use in any unit or vendored library unit:

| kind | count |
|---|---:|
| variable | 104 |
| enum | 65 |
| struct | 43 |
| forward | 43 |
| function | 37 |
| macro | 5 |

The first draft counted 285 because it did not include enums and macros.
These are excluded:

- declarations that a kept declaration needs (a type named anywhere, or a
  variable or function used in a body or macro);
- asserts;
- the pairs of `config/sibling-names.csv` (1.6).

Deleting any of them changes the declaration count of every unit that sees
it, so deletion waits until step 2 has narrowed the units (section 5).

- 287 come from the Android-transferred sections (from line 25,487,
  `tools/globals.py` / `tools/typeimport.py`). They go to
  `transitional/unused.h`, which the shim includes and step 3 deletes. The
  `detail` gives the owner each would get (`would go to X (evidence)`): the
  Android header when Android declares the name, else the module of the
  nearest placed declaration around it in saga.h (`saga.h neighbour`).
  Their 42 asserts follow them.
- 10 are PC declarations (before line 25,487). They stay with their proposed
  owner, also evidence `unused`.

Transferred, by proposed owner:

- `legoapi/legoapi_types.h` (android): `AIPATHCNXCONTROLSYS_s`, `CLIMBOBJECTSYS_s`, `CLIMBOBJECT_s`, `CUSTOMPIECE`, `CUSTOMPIECECATEGORY`, `CUTSYS`, `HINT_s`, `MINEENTRY_s`, `MINESYS_s`, `PARTLIGHTSOURCE_s`, `PLAYERITEM_s`, `PLAYERPACKET_s`, `PLAYERSUBOBJ2_s`, `PLAYERSUBOBJ_s`, `RUMBLEPACKET`, `SCENEPROGRESS_s`, `SCENESPECIALPROGRESS_s`, `GIZMOPICKUP_STATE_ALTERNATE_TYPE`, `GIZPANEL_DRAW_FLAG_ALT_MODEL`, `GIZPANEL_DRAW_FLAG_LEGACY`, `GIZPANEL_DRAW_FLAG_NO_SHADOW_RESET`, `GIZPANEL_FLAG_BADDIE`, `GIZPANEL_FLAG_HIDE_BASE`, `GIZPANEL_FLAG_HIDE_TARGET`, `NUINSTANIM_FLAG_PLAYING`, `NUINSTANIM_FLAG_REPEATING`, `GIZBUILDIT_AVAILABILITY_HAS_BLOWUP_TYPE`, `GIZBUILDIT_AVAILABILITY_LINKED`, `GAMEANIMSET_VISIBILITY_NONE`, `GIZSPINNER_STATE_SHADOW_PLATFORM`, `GIZMOPICKUP_STATE_COLLECTED`, `GIZFORCE_CONFIG_RESET_STATE_ON_ACTIVATE`, `GIZFORCE_PROGRESS_ENABLED`, `GIZFORCE_RUNTIME_HAS_PLATFORM`, `GIZFORCE_STATE_DESTROYED_OR_THROWN`, `GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS`, `GIZBUILDIT_STATE_REISSUE_REWARD_ON_ACTIVATE`, `GIZSPINNER_FLAG_HIDE_ARM`, `GIZSPINNER_FLAG_HIDE_BASE`, `GIZBUILDIT_AVAILABILITY_INTERACTING`, `GIZFORCE_CONFIG_ALONG_SOCKET`, `GIZFORCE_CONFIG_CAST_SHADOW`, `GIZFORCE_CONFIG_DRAW_REFLECTION`, `GIZFORCE_PROGRESS_VISIBLE`, `GIZFORCE_RUNTIME_ALONG_SOCKET_HIDDEN`, `GIZBUILDIT_AVAILABILITY_ACTIVE`, `GIZBUILDIT_AVAILABILITY_VISIBLE`, `GIZBUILDIT_RUNTIME_REWARD_RELEASED`, `GIZBUILDIT_STATE_TURN_OFF_WHEN_COMPLETE`, `GIZMOPICKUP_STATE_ENABLED`, `GIZMOPICKUP_STATE_VISIBLE`
- `legoapi/characters/core/character.h` (android): `PLAYERCHARACTERCONFIG_s`, `id_DARTHVADER`, `id_MILLENNIUMFALCON`, `id_PROBEDROID`, `id_JABBA`, `id_TRAININGREMOTE`, `id_WEIRDO1`, `id_WEIRDO2`, `id_WHIP`, `id_WOMPRAT`, `id_ATST`, `id_JEDISTARFIGHTERREDEP3`, `id_MINIIMPERIALSHUTTLE`, `id_MINIMILLENNIUMFALCON`, `id_MINITIEINTERCEPTOR`, `id_CLONEARC`, `id_IMPERIALSHUTTLE`, `id_NABOOSTARFIGHTER`, `id_SNOWSPEEDER`, `id_SPEEDERBIKESNOW`, `g_loadingCharacterInHub`
- `globals.h` (android-weak): `AREA_GLOBAL_VALUES`, `LEGOMENU_PAUSECUT`, `CLOUDCITYTRAPOUTRO_LDATA`, `GizObstacle_CheckExcludeFlagsFn`, `grab_screen_image`, `Reflections_On`, `PODRACEC_LDATA`, `MENUEXITB`, `MENUEXITG`, `MENUEXITR`, `DAGOBAHA_LDATA`, `DAGOBAHD_LDATA`, `DAGOBAHE_LDATA`, `SpaceRumbleTimer`, `TEMPLESTATUS_LDATA`, `HOTHBATTLED_LDATA`, `RESETBIT_DOOR_TRANSITION`
- `legoapi/misc/legoapi_status.h` (saga.h neighbour): `MENULOSTG2`, `MENULOSTR1`, `MENULOSTR2`, `tALLLEVELPROGRESSWILLBELOST`, `tALLSUPERSTORYPROGRESSWILLBELOST`, `tARCADE_NOWIN`, `tEXITTOMAP`, `tEXITWITHOUTSAVING`, `tGAMEPROGRESSWILLBELOST`
- `legoapi/items/base/apiobject.h` (android): `CHARACTER_SHADOW_s`, `APIOBJECT_FLAG_PLAYER_ACTIVE`, `APIOBJECT_STATE_FLAG_IGNORE_DOORS`, `GAMEOBJECT_EF8_FLAG_KEEP_WEAPON_OUT`, `CHARACTER_CONTEXT_DOOMED`, `CHARACTER_CONTEXT_NONE`, `CHARACTER_CONTEXT_SIGNAL`
- `legoapi/ai/game/creature.h` (saga.h neighbour): `tNO`, `tQUITGAME`, `tRESTARTDEMO2`, `tRESTARTLEVEL`, `tSAVEANDEXIT`, `tYES`
- `nu2api/nu3d/nuspecial_internal.h` (android): `NuSpecialBoundsDisplayLayout`, `NuSpecialLegacyObjectBoundsLayout`, `NuSpecialLegacySceneLayout`
- `legoapi/gizmos/transport/gizportal.h` (saga.h neighbour): `SPINNER_RUNTIME_AT_OUTPUT`, `SPINNER_RUNTIME_AUTO_RETURN`, `SPINNER_RUNTIME_INPUT_DISABLED`, `SPINNER_STATE_ROTATING`, `SPINNER_STATE_STOP_ANIMATION`
- `legoapi/menus/screens/gamestructure.h` (saga.h neighbour): `MENULOSTA1`, `MENULOSTA2`, `MENULOSTB1`, `MENULOSTB2`, `MENULOSTG1`
- `legoapi/world/level.h` (android): `LEVELSCRIPTPROGRESS_s`, `LEVEL_PROGRESS_s`, `LEVEL_OUTRO`
- `legoapi/menus/screens/gamestatus_lsw.h` (saga.h neighbour): `tCHALLENGECOMPLETE`, `tCONTINUESTORY`, `tFINALCHAPTERLOCKED`, `tFINISHSTORY`
- `legoapi/menus/screens/shop.h` (saga.h neighbour): `tMAP`, `tNEXTCHAPTERLOCKED`, `tRESTART`, `Text_MenuColourLerp`
- `legoapi/menus/screens/store.h` (android): `storepack_s`, `Store_IsPackUnlocked`, `StorePack`
- `legoapi/props/doors/door.h` (android): `Door_ExitCameraSplineName`, `DOOR_FLAG_ONE_WAY`, `DOOR_FLAG_CAMERA_LOOK_AT_PLAYERS`, `DOOR_FLAG_DO_NOT_USE`
- `nu2api/nu3d/nudlist.h` (android): `nuclipbounds_s`, `nuclipobject_s`
- `gameapi/ai/aisys/aisys.h` (android): `AIAREA_s`, `FindAlternativeSpecialObjectFn`
- `gameapi/edpart.h` (saga.h neighbour): `edpartFinishMenu`, `edpartRemoveInstance`, `nullobjectname`
- `gameapi/gui/apimenu.h` (android): `ICONSIZE`, `MENUTEXTSCALE`, `menu_pulse`
- `legoapi/gizmo/object/giztorpedo.h` (android): `GIZTORPMACHINE_FLAG_ACTIVE`, `GIZTORPMACHINE_FLAG_VISIBLE`, `GIZTORPMACHINE_FLAG_ALTERNATE_MODEL`
- `legoapi/gizmos/fx/gizmopickups.h` (android): `PickUpFlickerFrames`, `PickUpFlickerTest`, `PickupFlickerFrame`
- `legoapi/menus/screens/shop.h` (android): `SHOPINPUT`, `subitemselected`
- `decomp.h` (android-weak): `AREAFLAG_BONUS_AREA`, `AREAFLAG_VEHICLE_AREA`
- `gameapi/ai/aisys/aisys.h` (saga.h neighbour): `aieditor_cbCancelSaveMenu`, `locatorEditor_cbRenameLocator`
- `gameapi/edtools/edanim_internal.h` (android): `edbitsGetSoundName`, `edanimParamDestroy`
- `gameapi/edtools/edui.h` (android): `eduiMenuEnsureSelection`, `eduiMenuProcessSelectedItem`
- `legoapi/ai/core/aisysall.h` (android): `aieditor_ClearMainMenu`, `aieditor_cbCancelMainMenu`
- `legoapi/ai/core/aisysall.h` (saga.h neighbour): `locatorEditor_cbCancelRenameMenu`, `routeEditor_cbCancelRouteUsers`
- `legoapi/characters/core/character.h` (saga.h neighbour): `KITPOS2Y`, `LEGOOBJ_CHARKIT`
- `legoapi/gizmos/door/plugs.h` (android): `PLUG_s`
- `legoapi/gizmos/door/spinner.h` (saga.h neighbour): `DynamicGrapple`, `Grapples_DrawList`
- `legoapi/gizmos/traps/gizturrets.h` (android): `GIZTURRET_RUNTIME_FLAG_BLOWUP_NAME_ID`, `GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING`
- `legoapi/items/collect/spacelevel.h` (android): `spacelevel_cross_s`
- `legoapi/menus/core/text.h` (android): `tCANCEL`, `smarttextex_drawmessagebox`
- `legoapi/misc/utilities.h` (android): `CameraZoom`, `SolidMtl3D`
- `legoapi/props/system/socksys.h` (android): `SOCKSYS`
- `legoapi/render/core/rtldata.h` (android): `rtldata_s`
- `legoapi/world/levels/episodeI.h` (saga.h neighbour): `gunship_weapons`, `ParseAIPathCnxFlag`
- `legoapi/world/levels/hub_types.h` (android): `HUB_FREEPLAY_MODEL_MINIKIT`, `HUB_FREEPLAY_MODEL_VEHICLE`
- `legoapi/world/levels/levels.h` (android): `trooper_boltid`, `max_nsnipers`
- `legoapi/world/levels/levels.h` (saga.h neighbour): `PodSprint_InitAISpline`, `Hub_MarkCharacterUnavailable`
- `legoapi/world/levels/podrace.h` (android): `_vuv_s`
- `legoapi/world/world.h` (android): `LEVEL_PROGRESS_DATA_s`
- `nu2api/huffdecode.h` (saga.h neighbour): `MakePlayCorner`, `SetPlayPlane`
- `nu2api/nu3d/nuqfnt.h` (android): `NUQFNT_CSMODE_ABSOLUTE`, `NUQFNT_CSMODE_NORMALISED`
- `nu2api/nu3d/nurndr.h` (android): `nucolour3_s`
- `nu2api/nucore/NuDynamicLight.h` (saga.h neighbour): `TintStack`, `NuSpecialLegacyLayout`
- `nu2api/nucore/nugcutscene.h` (android): `instNUGCUTRIGID_s`
- `nu2api/nucore/nupad.h` (saga.h neighbour): `GIZTORP_LEVEL_OBJECT_ALTERNATE`, `GIZTORP_LEVEL_OBJECT_PRIMARY`
- `nu2api/nuplatform/nuplatform.h` (android): `NuPlatform`
- `gameapi/edbits.h` (saga.h neighbour): `FS_EncodedDateKey`
- `gameapi/edtools/edcam.h` (android): `edcamSetSpeed`
- `gameapi/edtools/edpp_internal.h` (android): `edpp_page_scene`
- `gameapi/edtools/edui.h` (saga.h neighbour): `SPINNER_RUNTIME_ANIMATION_HIDDEN`
- `legoapi/actions/character/simpletons.h` (saga.h neighbour): `LEGOOBJ_MINIKIT`
- `legoapi/ai/core/gameai.h` (saga.h neighbour): `tWINNER`
- `legoapi/characters/core/characters.h` (saga.h neighbour): `CreatePodRaceMine`
- `legoapi/characters/core/players.h` (saga.h neighbour): `pod_pacemaker`
- `legoapi/characters/motion/move.h` (android): `LEGO_SET_SLOWDOWNFN`
- `legoapi/characters/motion/move.h` (saga.h neighbour): `ForceBackRadius`
- `legoapi/core/input/qrand.h` (android): `QRAND_FLOAT`
- `legoapi/core/input/timer.h` (android): `LastTouchTime`
- `legoapi/gizmo/base/gizmo.h` (saga.h neighbour): `GizTurret_ReadAnimSetData`
- `legoapi/gizmo/object/giztorpedo.h` (saga.h neighbour): `TintStack`
- `legoapi/gizmos/object/gizbuildits.h` (android): `GizBuildIt_TurnOff`
- `legoapi/gizmos/object/gizpanel.h` (android): `GIZPANEL_PLAYERPOSLIFT`
- `legoapi/gizmos/traps/attracto.h` (saga.h neighbour): `tCHEAT_SELFDESTRUCT`
- `legoapi/items/objects/cable.h` (android): `DestroyCable`
- `legoapi/items/objects/objectsall.h` (android): `edpart_cam_pos`
- `legoapi/lib/hub.h` (saga.h neighbour): `hub_makefreeplaylist_addotherid`
- `legoapi/misc/androidbatman.h` (saga.h neighbour): `LEGOMENU_PAUSESYNC`
- `legoapi/props/objects/techno.h` (android): `TECHNO_FLAG_COMPLETE`
- `legoapi/render/core/gameliball.h` (android): `DebrisStatusNormal`
- `legoapi/render/core/gameliball.h` (saga.h neighbour): `routeEditor_cbSetRouteUsers`
- `legoapi/render/core/screen.h` (android): `GetAspectRatio`
- `legoapi/render/core/terrain_internal.h` (android): `TERRAINMASK_NONDROID`
- `legoapi/world/levels/episodeIII.h` (saga.h neighbour): `HothBattle_AssignSpawnLocator`
- `legoapi/world/levels/episodeV.h` (saga.h neighbour): `gameobj`
- `legoapi/world/levels/hub.h` (android): `hub_new_level`
- `legoapi/world/mission.h` (saga.h neighbour): `NuLegacyInstanceLayout`
- `nu2api/nu3d/nu3d_includes.h` (android): `NuHtmlFlush`
- `nu2api/nu3d/nudlist.h` (saga.h neighbour): `nudlist_debug_level`
- `nu2api/nu3d/nupostresources.h` (android): `NuFramebufferGetFrontBuffer`
- `nu2api/nu3d/nuprim_internal.h` (android): `NuRndrPrimUV`
- `nu2api/nu3d/nuspecial_internal.h` (saga.h neighbour): `NuSpecialLegacyLayout`
- `nu2api/nu3d/nutex.h` (android): `NuTexAddReference`
- `nu2api/nu3d/nutex.h` (saga.h neighbour): `NuLegacyInstanceLayout`
- `nu2api/nucore/implode.h` (android): `ImplodeGetI`
- `nu2api/nuerror_gen.h` (saga.h neighbour): `nudebug_handler`
- `nu2api/nufile/nufile.h` (saga.h neighbour): `eduiFntPrintEx`
- `nu2api/nusound/nusound.h` (android): `NUSOUND_STREAM_3`

PC declarations, with their owner:

- `PAD_INPUT_PC` (struct, line 5329): `legoapi/misc/legoapi_status.h`
- `PlayerCarryRotationScalePc` (macro, line 10251): `nu2api/nuerror_gen.h`
- `EdppUnresolvedInstancesPc` (variable, line 12050): `gameapi/edptl.h`
- `PC_STEREO_SAMPLE_METADATA` (forward, line 12272): `nu2api/numusic_gen.h`
- `PickupCoinName` (macro, line 12509): `legoapi/gizmo/object/gizmopickup.h`
- `PickupCoinBName` (macro, line 12510): `legoapi/gizmo/object/gizmopickup.h`
- `ScoreBuildUpDonePc` (macro, line 12591): `legoapi/lib/hub.h`
- `NuUnresolvedTextureDDS` (variable, line 17750): `nu2api/nustencilshadows_PC.h`
- `ExBlowupFlagsPc` (variable, line 18719): `legoapi/characters/motion/animation_ids.h`
- `PcPrepareTiming` (function, line 21204): `nu2api/PCInput.h`

### 1.6 The 16 globals blocked in the rename batch

Commit 2f65e26a left 16 verified rows of `config/sibling-names.csv` open. For
each, the original name (`addbolt_nosfx`, `Door_CutCamPos0`, ...) is already
declared, unused, in the transferred Android block (lines 29,186-30,668), and
deleting that declaration changed code generation (presumably the id count).

The map puts both declarations of each pair in one header (rule
`same-entity`: the side with better evidence decides). The 1.5 pass keeps
these pairs out of the unused list.

- 13 go to a module header: `door.h` 4, `move.h` 2, `characters.h` 2,
  `nu3d_includes.h` 2, `bolts.h`, `pushblocks.h`, `gameliball.h`.
- 3 go to Android's `globals.h`: `Game_CompletionSave`, `COMPLETIONPOINTS`,
  `Game_EpisodeSave`. These are catch-all rows.

The fix differs by kind:

- 11 are globals defined in data.cpp. Rename the definition and the PC
  declaration to the original name and keep both declarations: two `extern`
  declarations of one name, so the count stays the same. Where the types
  differ, the Android declaration takes the PC type:
  - `setlastdoor_last` is `struct DOOR_s *`, but `LastDoorPointerPc` is
    `void *`;
  - `Game_CompletionSave` is `u16 *`, but the PC one is a reader struct
    pointer.

  The duplicate goes in step 2 or 3, when its header no longer reaches the
  fragile units, or under the exact-set check.
- 5 are file statics of their unit: `ForceBackObjPc` and `ForceBackTypePc` in
  move.c, `IconScenePc` in characters.cpp, `RendererAlphaMaskMaterialPc` and
  `RendererZClearMaterialPc` in nu3d_includes.cpp. A static cannot follow the
  `extern` of the same name, so the Android declaration must go with the
  rename. After the split it is in the owning module's header, so the count
  change reaches only units that include that header. Do it in step 2, after
  the unit is narrowed, under the exact-set check.

## 2. The grouped split

### 2.1 Which reorderings are inert

Two top-level items commute, meaning they can swap places without changing
the meaning of either, unless one of these holds (`dependency_graph`):

- **Same name.** Both declare the same name: redeclarations keep their order
  (linkage, composite types, overloads, the C and C++ variants of one
  declaration).
- **Use.** One declares a name the other mentions. A mention of a macro or
  typedef also mentions what it stands for, transitively. A use of
  `DEBRIS_PARTICLE_CHUNK_PC` needs `struct debris_particle_chunk_pc_s`
  complete when it accesses members.
- **Macros.** One is a macro definition or `#undef` and the other contains
  its name.
- **Conditions.** One tests, in its `#if` context, a macro that the other
  defines.
- **Includes.** One is an `#include`:
  - a project header (`debug/diagnostics.h`, `externals.h`) counts as
    declaring and mentioning what it declares and mentions;
  - the system includes of the prelude (`<windows.h>` ...) are barriers;
  - `<vector>` declares the macros and the identifiers outside braces of its
    preprocessed text (one clang -E), and keeps its place against every
    saga.h macro that any identifier in its files could meet.

Exceptions:

- A macro's own body does not order it against the names it uses: they bind
  where the macro is used.
- A parameter name in a prototype binds nothing. It meets only a macro of
  that name.
- Redeclaring a tag that is already declared and visible (`struct s;` after
  `struct s;` or `struct s {...}`) changes nothing for the mentions around
  it.
- Completing a declared tag changes nothing for a prototype that only names
  it.

So:

- prototypes and `extern`s move freely after the types they name;
- type definitions keep their dependency order;
- macros keep their order against everything that spells their names.

Any order that keeps every non-commuting pair in saga.h order gives every
unit the same declarations, each with the same meaning. Only two things
change: their order, and the `__COUNTER__` numbers that name the
STATIC_ASSERT typedefs.

### 2.2 Scheduling

1. **Forced parts.** A header must have parts in any order when an item of
   another header depends on one of its items and is needed by another of
   its items (`forced_splits`).
2. **Breaking cycles.** While the headers that stay whole still form a cycle,
   the header with the most dependency edges inside that cycle also gets
   parts (`choose_splits`).
3. **Emitting.** Whole headers are emitted as soon as all their dependencies
   are out, in saga.h order. The items of a split header wait until no whole
   header is ready, then continue the header's current part while they can.

The result with the current map (`--interleaving`):

| plan | includes in the shim | headers in parts | parts |
|---|---:|---:|---:|
| exact order (`--exact-order`) | 5,024 | 356 of 419 | 4,961 |
| grouped, default | 593 | 74 of 419 | 248 |

The grouped shim is 603 lines: guard, comment, `#define SAGA_SHIM`, and 593
includes. 345 headers are one plain include.

Parts per header for the 74:

| parts | headers |
|---:|---:|
| 2 | 28 |
| 3 | 29 |
| 4 | 7 |
| 5 | 4 |
| 7 | 2 |
| 8 | 1 |
| 9 | 1 |
| 10 | 1 |
| 16 | 1 |

The largest are:

| header | parts |
|---|---:|
| `legoapi_types.h` (catch-all) | 16 |
| `gameliball.h` | 10 |
| `aisys.h` | 9 |
| `nu3d_includes.h` | 8 |
| `decomp.h` | 7 |
| `apiobject.h` | 7 |

Why the 74 still need parts:

- 66 are forced: their own declarations form a chain through another
  header. `--interleaving` names one witness per header. By what the chain
  connects:
  - 31 are type to type, a true type cycle across headers. For example,
    `struct AIPATHCNX_PC` (aisys.h) → `struct AIPACKET_PC` (apiobject.h) →
    `struct AILOCATORSET_PC` (aisys.h). `struct WORLDINFO_s` (world.h)
    is the witness for 9 headers.
  - 29 are type to declaration: a header's prototype or `extern` needs a
    type of another header, which needs a type of the first. For example,
    `struct PLAYERCHARACTERCONFIG_PC` (move.h) → `characterdata_pc_s`
    (character.h) → `gameobject_s` (apiobject.h) → `LetGoOfBalloon` (move.h).
  - 6 start at something that is not a type:
    - 5 at a prototype that names a struct before any declaration of it
      (`PC_SHADER_OBJECT_FULL_VIEW` in four shader headers,
      `GIZSPINNER_s`). In C that tag has prototype scope, so the prototype
      keeps its place before the definition.
    - 1 at `decomp.h`'s prelude include.
- 8 break the remaining cycles: `world.h`, `hub.h`, `game_deb.h` (3 parts
  each), `edgra.h`, `legoapi_status.h`, `nuquat.h`, `nufile_pc.h`,
  `legoapi_signal.h` (2 each).
- `decomp.h` has parts because it holds both the prelude and the view types
  and checks that use the module types.

All of these are ownership questions. Moving the type at the joint
(`WORLDINFO_s`, `AIPACKET_PC`, `gameobject_s`, the view types) to the header
that both sides include removes the parts. The catch-all reviews do most of
that. Each part is a block of the header's file:

```c
#if SAGA_NU2API_NUFILE_PC_H_PART == 2 && !defined(SAGA_NU2API_NUFILE_PC_H_STOP)
...items of part 2...
#undef SAGA_NU2API_NUFILE_PC_H_PART
#define SAGA_NU2API_NUFILE_PC_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFILE_PC_H_STOP
#endif
#endif
```

The shim includes the header once per part. Included directly, the header
gives all its parts at once.

Includes inside a header:

- A header includes the headers it uses plainly when all of them come
  earlier in the shim, which makes the include a no-op there.
- Otherwise it includes them inside `#ifndef SAGA_SHIM`, with the note
  "included via saga.h only for now".

Each item keeps its preprocessor context. A run inside a top-level `#if` is
wrapped in the original directive, or a derived `#if !(a) && (b)` in a later
branch. A run inside `extern "C"` gets its own `#ifdef __cplusplus extern "C"
{` block.

`--apply` formats what it writes with the house style (`.clang-format`
never sorts includes). `tools/check.py` accepts every target header of the
map (`split_headers`). On a scratch copy of the tree (map of this commit),
`--apply` followed by `check.py` and `check.py --format` passed as written.

## 3. The proof of step 1

Token neutrality holds only for `--exact-order`. For the grouped split the
build worker proves three things, unit by unit, Matching and Debug:

1. **Same declarations** (`--check-counts UNIT... --debug`). The unit is
   preprocessed in the base tree and in the split tree (clang -E, lint flags,
   one process at a time), then cut into top-level declarations: up to a `;`
   outside brackets, or the `}` of a function body or namespace. Each
   declaration carries its `extern "C"` linkage and the `#pragma pack` in
   effect, and `saga_static_assert_N` loses its counter number. Both the
   number of declarations and the multiset must be equal, and
   `clang -fsyntax-only` must give the same errors on both sides (none
   today). C units are read with `-std=gnu89` rather than the lint flags'
   `-std=c89`, because VC8 reads `//` comments in C too.
2. **Same objects.** `make -j2 build` on the split tree, then
   `check.py --objects`. Code and data sections of every object must be
   identical to the base build.
3. **Same exact set.** `match.py --update` must report the same exact set by
   address, with the FRAGILE functions (`tools/unitscore.py`) unchanged.

Proof 2 cannot hold as stated: every new header file costs each unit one
symbol id (section 6, "Files as ids"). The fragile functions change even
with `--exact-order`. If 2 or 3 fails for a unit beyond the fragile
functions and 1 holds, look at the order:

1. Rebuild with `--exact-order`, which `--check-neutral` proves
   token-identical, to confirm it.
2. Bisect by moving the unit's offending header back to its saga.h
   position.
3. Record the case in docs/cleanup-audit.md.

Per docs/cleanup-audit.md the fragile functions depend on the number of ids
a unit consumes, not on which declaration consumes them. Proof 1 holds the
number fixed.

## 4. Trial

All runs used a scratch copy (`build/headersplit/`, deleted afterwards) and
the map of this commit. `--check-counts` on 14 units, Matching and Debug,
all the same declarations with 0 clang errors on both sides:

- the six fragile units: `legogame/episodeIII.c`, `legogame/shop.c`,
  `legogame/game.cpp`, `legogame/episodeIV.c`, `legoapi/parts.c`,
  `nu2api/nu3d_platform_pc.cpp`;
- `nu2api/shader_parser.cpp`, `gameapi/aisysall.c`, `data.cpp` (45,614
  declarations), `batman.cpp`;
- `lib/ogg-vorbis/vorbis_smallft.c`;
- the Ogg users `nu2api/oggreader.cpp` and `nu2api/nustream.cpp`;
- `debug/log.c`, a control without saga.h (3,868 declarations, none moved).

About 15,000 of 29,000 declarations per unit move. The other runs:

- `--check-counts` on every 11th unit of `git ls-files` (32 units, Matching):
  all 32 see the same declarations, with 0 clang errors.
- `--check-neutral` on the `--exact-order` split (`episodeIII.c`, `data.cpp`,
  `vorbis_smallft.c`, `shader_parser.cpp`, Matching and Debug):
  all 8 token streams are identical (650,000 to 1,050,000 tokens). The C units
  show 19 clang errors on both sides, because these runs use the lint flags'
  `-std=c89`.

Controls:

- Dropping one prototype from a generated header is reported as
  `only before x1`.
- Moving one header to the end of the shim gives 1,521 clang errors.

The first grouped trial found four real reorder errors, now handled by the
rules in 2.1:

- a typedef named after `void` by the segmenter (`typedef void (__cdecl
  bgprocdofn)(...)`);
- uses through a forward typedef that need the struct complete
  (`DEBRIS_PARTICLE_CHUNK_PC`, `CUTSYS_PC`);
- `-std=c89` turning comments into tokens;
- for the dependency analysis, parameter names that matched CRT names.

Exact-order history: the first neutrality trial found `#ifndef
SAGA_HARDWARE_LIFECYCLE_PROPOSAL` / `#define ...` inside saga.h, so a
conditional that changes its own condition is one item.

## 5. Procedure

### Step 1: split behind the shim (tried, see Status)

1. `python3 tools/headersplit.py --apply` (tool output only, no hand edits).
2. `python3 tools/check.py`, `python3 tools/check.py --format`.
3. Proof 1: `--check-counts` on every unit, `--debug` included (about 4 clang
   runs of 2-4 s per unit and mode, one at a time: about 1.5 hours for the
   344 units). Proofs 2 and 3: build, `check.py --objects`, `match.py
   --update`. Debug compile.
4. Commit as one batch, so `git revert` restores saga.h.

Rerun `--build-map` after any saga.h change before applying. Rows are matched
by key, and rows with evidence `review` are kept.

### Step 2: narrow units

Prerequisite (see "Step 2 is blocked on self-contained headers" above): every
header a narrowed unit reaches compiles on its own, in one part, without an
include cycle.

For each unit, replace `#include "saga.h"` with the module headers it uses.
At the median, a unit sees about 16 headers directly and 86 with their
dependencies; data.cpp sees 283. Outside the shim every header gives all its
parts at once and includes its dependencies, so the unit sees fewer
declarations, in another order.

Proof per batch:

- build;
- the exact set by address is unchanged (`match.py --update`);
- the FRAGILE functions are compared separately;
- a loss is bisected to the declarations that cause it and recorded in
  docs/cleanup-audit.md.

Do deletions (1.5, 1.6) here, header by header, after the units that see the
header are narrowed.

Order: the declaration count matters most where fragile functions live, so
those go last.

1. Units that need few headers (41 need fewer than 50 with dependencies),
   `src/debug`, and the Ogg users and vendored units together with the
   library pin (1.4).
2. The other units by number of headers needed.
3. The unity compilands (`nu3d_includes.cpp`, `nucore_includes.cpp`,
   `gameliball.cpp`, ...), then `batman.cpp`, then `data.cpp` (data order
   only).
4. Last, the units of the fragile functions, each alone, retuned as in the
   assert pilot when its count changes:
   - `legogame/episodeIII.c` (`BoulderSpawnPc`)
   - `legogame/shop.c` (`SubItemMenu`)
   - `legogame/game.cpp` (`InitGameAfterConfig`)
   - `legogame/episodeIV.c` (`BlockadeRunnerDEjectCreaturePc`)
   - `legoapi/parts.c` (`DebrisSetup2`, `NuRndrParticleGroup`)
   - `nu2api/nu3d_platform_pc.cpp`

Batch size: 15-25 units per build, fewer for the unity files, one unit per
batch for the six fragile units.

### Step 3: delete the shim

When no unit includes saga.h:

1. Delete saga.h, `SAGA_SHIM` and the `transitional/` headers. Delete the
   unused declarations, or move each one the build shows to be needed to its
   `would go to` owner.
2. Merge each header's parts into one body. The `#if ..._PART` sections and
   the `#ifndef SAGA_SHIM` wrappers go; the order inside a header may then
   change, proven by objects.
3. Give the catch-all rows (`globals.h`, `legoapi_types.h`, detail
   `catch-all`) module owners.
4. Move `externals/*.h` into `externals.h`.
5. `decomp.h` keeps the prelude, `STATIC_ASSERT`, `STUBBED` and the views
   until batch 10.
6. Update the tools that write into saga.h (`tools/globals.py`,
   `tools/typeimport.py`: they take the target from `--place`) and the saga.h
   special cases in `oddscan.py`, `declscan.py` and `layoutcheck.py`.

### Declarations in units (declscan classes C and D)

`--place NAME...` applies the same rules as the map to a name declared in a
unit, in this order:

1. Android header,
2. owning compiland by symbol or definition,
3. single-compiland users,
4. the shared-declaration rule,
5. Android catch-all.

Class D declarations (2,630) move to the printed header during step 2,
together with the narrowing of their unit, so one exact-set check covers
both.

Class C declarations (376) are forward declarations of functions defined
later in the same unit. Most disappear when the unit reaches retail function
order (docs/declarations.md). A class C name that another compiland also uses
gets its header from `--place`. The rest stay unit-local: `static`
prototypes are not header material.

## 6. Risks

- **Order and ids.** The grouped split changes the declaration order in
  every unit. Proof 1 holds the declarations and their count fixed. Whether
  VC8's id assignment depends on order beyond the count is what proofs 2 and
  3 settle; `--exact-order` is the fallback (token-identical, 4,961 parts).
- **Files as ids (measured, 2026-11-14).** Step 1 is not byte-neutral, and
  not because of macros. VC8 spends one symbol id on each distinct source
  file a unit opens, the same as one typedef, in C and C++. The split adds
  419 files to every unit. Measured by compiling single objects against an
  edited saga.h (no other change):
  - 1,000 unused `#define`s: every object identical. Guard macros,
    `SAGA_SHIM` and the `_PART` macros cost nothing.
  - `#line`: identical. Line numbers cost nothing.
  - Moving the whole saga.h body into one included file: `game.obj`
    changes (`InitGameAfterConfig`). The file is the cause.
  - N empty files included at the top of saga.h gave the same object
    signatures as N `typedef int t_i;`, for N = 0..70. Checked on
    `legogame/shop.c` (C) and `nu2api/nu3d_platform_pc.cpp` (C++). The
    period is 64: N = 64, 128 and 448 at the top, or 64 at the end, leave
    all six test objects identical.
  - Including an already-opened file again costs nothing. This holds for a
    plain file, a guarded file and a `#pragma once` file. Nesting depth
    costs nothing either: 4 nested files equal 4 flat files.
  - Position matters. 64 files spread through saga.h change three of the six
    objects, because every declaration after a file shifts by one id.
  - Confirmation: the original saga.h plus one empty-file `#include` at each
    of the 419 points where the `--exact-order` split first opens a header
    reproduces the split's objects. The match is exact for
    `episodeIII.obj` (`BoulderSpawnPc`), `game.obj`, `nu3d_includes.obj`
    (`NuDeferredFilterGen::render`), `edlevelall.obj` and
    `nu3d_platform_pc.obj`. For `shop.obj` it is one id off (`SubItemMenu`
    there, `DrawSubItems` in the split).

  `#pragma once` instead of guards changes nothing. A byte-neutral step 1
  would need every unit's file count to come out right at each fragile
  declaration, modulo 64. Only padding files or contrived header grouping
  can do that, which is steering. So the split will change the fragile
  functions, as step 2 already expects. They have to be retuned honestly,
  for example by the order of the saga.h assert conversion, under the
  exact-set check.
- **Declaration count.** Every deletion of a declaration (the 297
  candidates, the 16 duplicates, the saga.h asserts that the layout comments
  replace) changes the id count of every unit that sees it. Do those after
  the units are narrowed, header by header.
- **Commutation rules.** They are syntactic and conservative where they
  could not be exact: system headers are read lexically, and any identifier
  counts as a mention. Proof 1 checks the result per unit. A rule that is
  wrong shows up there as a missing type or a changed declaration.
- **Ownership quality.** `rule`, `data-order`, `data-nearest` and
  `saga.h neighbour` are weaker than `android`, `symbol` or `users`. A wrong
  owner costs only a later move. The catch-all rows are the first to review,
  because they also cause most of the remaining parts.
- **Tools that read saga.h.** `oddscan.py` (saga.h statistics), `declscan.py`
  (only the wording of a reason), `layoutcheck.py` (retries a header with
  `-include saga.h`), `globals.py` and `typeimport.py` (write into saga.h).
  Adjust them in step 3.
