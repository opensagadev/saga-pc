# PC data inventory

`src/data.cpp` defines independently mapped shared objects. The current
[`matching.json`](../matching.json) records which complete COFF extents and
initializers match and which still differ or lack an established original
extent. Exact initializers include relocated pointers and the 65,536-byte runtime-filled
sound-volume table's zero initial state. Isolated VC8 sections keep small scalar and string
definitions at their actual extents instead of absorbing alignment padding.
The accepted entries are in [symbols.csv](symbols.csv), and normal local matching
checks their emitted bytes. The detailed scratch comparison and candidate scans
are under ignored `build/analysis/data/`. This includes previously mapped symbols
that acquired definitions as well as new mappings.

This is a verified first tranche, **not an exhaustive data decompilation**.
The source contains typed definitions and understood lookup values; no section
was converted into an array of extracted bytes. Reference exports and candidate
inventories stay under ignored `build/analysis/data/`.

## Primitive tables

| PC VA | Reconstruction symbol | Bytes | PC boundary/type evidence |
| --- | --- | ---: | --- |
| `0x00764258` | `_KeyStructSizes` | 16 | Byte lookups at `0x005cac1b`, `0x005cac34`, and `0x005cac4d`; each type is checked against 16. |
| `0x0080048c` | `_bad_equiv` | 32 | Two 16-bit code points per entry; `0x004f3480` reads the second word, and `0x004f34a6` scales the index by four until a zero first word. Seven substitutions plus the sentinel. |
| `0x00804224` | `_vehicle_timebase_dist` | 16 | Four explicit x87 single-precision reads at offsets 0, 4, 8, 12 in `0x00589762`–`0x005897ce`. |
| `0x00804234` | `_vehicle_timebase_nframes` | 16 | Four word-stride values converted to bytes for consecutive destination fields at `0x0058976e`, `0x00589792`, `0x005897b6`, `0x005897da`. |
| `0x00805c94` | `_TrooperStepFrames` | 16 | Four i32 values; PC index shifted by two and masked with 3 at `0x005aa933`–`0x005aa939`. |
| `0x00805ca4` | `_trooperspeedtab` | 16 | Four f32 values; PC index shifted by four and masked with 3 at `0x005a9766`–`0x005a976c`. |
| `0x00806c00` | `_cubeEdgeIndices` | 96 | Twelve pairs of i32 vertex indices; paired reads in the cube intersection loop. |
| `0x00806c60` | `_cubeEdgeTopologies` | 96 | Twelve pairs of incident-face indices. Shared PC loop advances by 8 and stops at `0x60` (`0x005c8511`–`0x005c8517`), establishing both 96-byte extents without using adjacent addresses. |
| `0x008066c8` | `_ang` | 1,026 | Two-byte indexed samples of the ratio over [0, 1], scaled by 512 in `0x005be030`; both endpoints require 513 entries. All values independently regenerate as `floor(atan(i / 512) * 32768 / pi + 0.5)`. |
| `0x0080ed78` | `_crctab` | 1,024 | Three indexed u32 reads at `0x006106fc`, `0x0061070f`, `0x00610722`, with indices reduced to a byte. All 256 entries independently regenerate with eight reflected CRC-16/IBM steps using polynomial `0xa001`, stored as u32. |
| `0x0080f5a0` | `_NuKeyChar` | 520 | `0x0061e6b0` traverses 8-byte records until the u32 key is zero; byte fields are at +4 and +5. There are 64 nonzero keys plus the complete sentinel record. The PC layout is asserted explicitly in `saga.h`. |
| `0x00810b38` | `_SkinFlipTab` | 32 | Two vertex orders of four i32 indices; PC forms `vertex + mirrored * 4` and reads a four-byte index at `0x0066f29a`. All eight values agree with the independently named Android table. |

Android source references for initializer spelling and names:

- `src/nu2api/nucore/nuanim3.cpp`: `KeyStructSizes`.
- `src/nu2api/nucore/nustring_c.cpp`: `bad_equiv`.
- `src/legoapi/characters/core/charconfig.cpp`: the vehicle timebase arrays.
- `src/legoapi/world/levels/episode.cpp`: trooper frame selection; the original
  Android ELF also names the four-entry `trooperspeedtab`.
- `src/legoapi/misc/utilities.cpp`: `cubeEdgeIndices` and `crctab`; the original
  Android ELF names the corresponding `cubeEdgeTopologies` object.
- `src/nu2api/numath/nutrig.cpp`: `ang`.
- `src/nu2api/nucore/nucore_plain.cpp`: `NuKeyChar` initializers. Only the simple
  record established by PC loads was carried over, not that file's platform code.
- `src/legoapi/characters/motion/charplatforms.cpp`: `SkinFlipTab`.

Every listed initialized extent occurs in the PC binary with the reconstructed
values. The CRC and angle tables were additionally regenerated mathematically
and compared to every retail entry, rather than treating a blob match as the
implementation.

## Centralized state

Four seed states move from the newly reconstructed function units to this data
unit: `GAMERAND`, `NuWindQS`, `partseed`, and `debrisseed`. Their existing identity
and initializer evidence remains in the seed initialization notes. The existing
`qseed`, `fseed`, and `global_rand` definitions stay with their original owners.
`GAMERAND` keeps its C++ linkage; new shared C declarations retain the mapped
underscore names. Centralization changes source visibility for formerly private
state, not the observed original address, initializer, or reference bytes.

`argc`, `argv`, and `curr_dat` are four-byte zero-initialized state backed by the
reconstructed command-line/data setter/getter routines. `win_message` is the
28-byte Win32 `MSG` used by the verified Windows message pump. `WorldInfo` has
two PC records at stride `0x51ac`, not Android's stride. Its `0xa358`-byte BSS
extent and `WORLD`/`LWORLD` pointer initializers are verified independently by
the world initialization/switch/getter work. `WorldInfo_OtherLevel` at
`0x0055f500` selects the other record at `0x0093d878` or `0x00942a24`, tests
the four-byte `loaded` field at `+0x110`, and returns the `level_idx` field at
`+0x11c` or -1. Only recovered PC fields belong in the shared type; initializer
equality does not establish unknown members.

PC `WorldInfo_UpdateRoomVisibility` at `0x0055f560` also establishes
`current_gscn` at `+0x13c`, a visibility flag at `+0x2990`, a 256-byte room
visibility array at `+0x2991`, and its pointer at `+0x2a94`. Scene room count
and room pointer are at `+0x74` and `+0x78`; room records advance by `0x18`
bytes and expose a visibility-flags byte at `+0x10`. These named offsets are
asserted in `saga.h`. The function's C reconstruction matches the complete PC
extent.

The permanent/character/super-buffer pointers and sizes are the 13 four-byte
objects used by the verified StartPerm/EndPerm reconstruction. Their initial
values are zero except `CHARACTERBUFFERSIZE = 0x02800000` (PC 40 MiB). The
Android source's different buffer size was not copied.

The PC startup dispatcher passes a seven-pointer table at `0x007ef090` to
`StartupOpenDats`. Its first string is uppercase `GAME.DAT`, followed by six
`Episode_*.DAT` names, distinct from NuMain's lowercase-suffix strings.
The table and all seven strings match their complete initialized extents.
`StartupDatList` at `0x02974d14` has at least seven pointer slots; its full
original capacity is not yet established, so its CSV size remains blank.
`StartupDatCount` and `NuSoundDat` are separate zero-initialized scalars.

The PC parser allocates at most four `NUFPAR_PC` records of stride `0x584`
at `0x00917870` and four `0x1000`-byte read buffers at `0x008cab70`.
Their root pointers at `0x0091eedc` and `0x0091eee0` start null. Those
types and extents follow the PC allocator's loop and indexed accesses; the
level and area table loaders still return null until their record parsing is
reconstructed.

## Startup and subsystem state

The new definitions below have exact full-extent initializer comparisons in
the current local `matching.json`. PC instruction references establish their
types and boundaries; Android names are used only where PC behavior supports
them.

| PC VA | Object | Extent and PC evidence |
| --- | --- | --- |
| `0x0297dbe0` | `nuapi` | 0x90-byte zero-initialized aggregate. PC `0x004f3db9` clears all 0x90 bytes; access widths establish i32 fields at `+4` language, `+8` video mode, `+0xc` swap mode, `+0x10` aspect, `+0x1c` PAL, `+0x24` frame-begin count, `+0x7c` reversible characters, `+0x84` forced shadows, and f32 brightness at `+0x8c`. The PC swap-mode offset differs from Android. |
| `0x007514d0`, `0x00751528` | `InitOnceWindowTitle`, `InitOnceHalf` | Complete 36-byte title string including its PC single-byte trademark codes, and four-byte f32 `0.5f`; both are read by PC InitOnce. |
| `0x0080bbe0` | `g_OcclusionManager` | Full 0x170-byte initialized manager ending before distinct data at `0x0080bd50`. `initialized` starts zero, `enabled` starts one, and f32 fields at `+0x158`/`+0x15c` start `0.3f`/`-1.0f`. PC BeginFrame accesses the two set pointers at `+0x150`/`+0x154` and clears counters at `+0x160`/`+0x164`. |
| `0x007ef01c`, `0x00924890`, `0x009253cc`, `0x00926bd0` | `SUPERBUFFERSIZE`, `original_permbuffer_base`, `Game_NuPad`, `Game_NuPad_Store` | PC allocation size is `0x09600000`; InitOnce copies the returned pointer and passes both pad slots at `0x00926bd0` and `+4` into hardware setup. The pointer globals and two-slot array start zero. |
| `0x0092500c`, `0x00925010`, `0x00925014`, `0x00925040`, `0x00925184` | Four `app_tb*set` IDs and `pNuCam` | InitOnce stores four returned i32 set IDs and a camera pointer in separate four-byte globals; all start zero. |
| `0x00963fb0`, `0x009663a8`, `0x0096d9e0`, `0x0096e598` | `fstack`, `fstack_limit`, `fstack_top`, `fstackmem` | Three four-byte cursor pointers and a 0x3000-byte zero-filled portal stack. PC `NuPortalInit` at `0x005d9030` sets its start, aligned top, and limit. |
| `0x009716c0`, `0x0097398c`, `0x00920a00` | `NuTimeBar_SetList`, `NuTimeBar_Initialised`, `NuTime_TickBase` | 16 zero pointers (64 bytes), a zero i32, and a zero i64. PC time-bar initialization clears exactly 0x40 bytes at `0x009716c0`; slot code indexes four-byte entries. |
| `0x009738a4`, `0x0097388c` | End-frame callback pointer, `nu2api_paused` | Four-byte zero pointer assigned at `0x005c9ff0`; four-byte zero f32 set by `NuPause` at `0x005c9130`. The pause value remains owned by its function unit. |
| `0x0093b34c` | `NetPaused` | Zero-initialized four-byte i32 written with `Paused` by the exact PC `ClearPause` routine at `0x0055c5e0`. |
| `0x02920aac` | `PartRTL` | Zero-initialized `RTLSET_PC *`. `SetPartRTLSet` receives `world->rtl_set` at PC offset `0x2ab4`; the later parts renderer tests this pointer before allocating a lighting template. |
| `0x0297397c`, `0x00755180`, `0x00810dc8` | `NuSound3Reverb`, `NuScale1000`, `NuSound3LoopHoldTime` | One-byte zero reverb flag, shared eight-byte f64 value `1000.0`, and four-byte i32 value `133`; PC sound and time conversion routines reuse the same constant. |
| `0x02962bd0`, `0x00810dbc`, `0x02972c20` | `NuSound3VolumeTable`, `NuSound3VolumeOffset`, `NuSound3StereoStreams` | PC stream-volume wrapper indexes a 16,384-float zero-initialized table filled at runtime, adds the initialized `-300` offset, then calls a queue method through one of two zero-initialized queue pointers. |
| `0x007fc274`, `0x007fc278`, `0x007fc3c4`, `0x007fc3c8` | Panel projection and wide-mode sizes | Four PC f32 initializers, `1222.0`, `1145.0`, `740.0`, and `387.5`, feed `InitPanel`; the two computed multipliers begin zero. |
| `0x00925058`, `0x009250e8`, `0x0091efa0` | `MainFrameCounters`, `do_multiframe_update`, `NuTime_PerformanceFrequency` | The PC frame scheduler writes five consecutive counter bytes and reads a zero i32 update flag. Time conversion reads a zero-initialized eight-byte cached frequency set by a later platform timer initializer. |
| `0x008108e4`, `0x0296206c` | `debris_time_increment`, `DebrisCutSceneMode` | Four-byte f32 `1/60` step and zero i32 mode, written by the verified PC debris setters. |
| `0x02854bf8`, `0x02854c00` | `nu_water_reset_state`, `nu_water_reset_status` | Separate zero-initialized four-byte integers written as 1 and 0 by `NuWaterReset` at `0x005f9530`. |

`NuTimeGetTicksPS` at `0x004ffb20` and `NuTimeBarSlotReset` at `0x005d8370`
are still code work in progress. Their exact data initializers do not establish
matching function bodies.

The PC text subsystem has two seven-record language tables, each ending with
`{-1, 0}`; Android's current default table lacks that final record. Its path and
display-name strings, initialized pointer globals, string-table state, and
eight two-byte default text IDs are mapped separately. Their complete original
extents and initializers match. The PC language-path switch selects Japanese for
0, French for 2, Spanish for 3, German for 4, Italian for 5, Danish for 8,
and English otherwise; this ordering is not inferred from Android's enum.

## Additional file, font, and gameplay state

The next PC startup batch adds zero-initialized DAT and memory-file position
records: 20 records of `0x30` bytes at `0x0297dc80`, and 20 of `0x14` bytes at
`0x02983760`. File initialization clears precisely those extents; position and
seek instructions establish the typed offsets in `saga.h`. The source retains
PC `GAME.DAT` level/area table loading as an open parser dependency.

Font mode saves use 16 four-byte entries starting at `0x0096d890`, with a
separate stack index at `0x009738ec`. PC push and restore displacements establish
that array base; the preceding word at `0x0096d88c` is not absorbed into it.
The typed font platform-data pointer at `VUFNT+0x48` leads to x/y/z at
`+4/+8/+0xc` and a 64-byte matrix at `+0x10`; their users now have layout
assertions instead of numeric byte access.

The AI message pool uses eight-byte list links and `0x38`-byte message records,
including a 16-bit flags field at `+0x36`. The hub model list has six four-byte
records, and the snake bodies occupy four `0x110`-byte slots. Their initializers
and individual source definitions are compared independently with the PC image.

## NuMain scalar state

49 additional standalone i32 globals are supported by the established NuMain
call/control-flow correspondence, individually sized four-byte Android ELF
objects and source declarations, and original PC four-byte writes. None of the
`Game.options_save`, `StatusPacket`, `FadeSys`, or other aggregate-member anchors
was promoted to an independent global. Full original instruction references and
source declaration locations are recorded locally in `numain-scalars.json`;
[the NuMain queue](numain.md) retains the broader access/role candidates.

`waiting_for_character` also has a 16-bit narrowing read at `0x00419e93`, and
`ChallengeMode` a byte narrowing read at `0x00483e14`. Their stores remain
four-byte scalar operations; neither narrow read reduces the object's extent.
`abort_load` and `waiting_for_level` retain volatile qualifiers: repeated fresh
memory reads in PC world-loading paths agree with the Android declarations.
The exact qualifier is a source-level hypothesis, not something initializer
matching alone can establish.

The following initializers are the original PC values, even when the current
Android reconstructed source spells a different default (for example,
`last_area` is zero and `other_level` is -1 here).

| PC VA | Recovered name | Initial i32 value |
| --- | --- | ---: |
| `0x007f23a4` | `waiting_for_character` | -1 |
| `0x007f3b14` | `BonusWinner` | -1 |
| `0x007fac20` | `PlayTrailer` | -1 |
| `0x00802c5c` | `other_level` | -1 |
| `0x00802c68` | `level_already_loaded` | -1 |
| `0x00802c6c` | `reset_load` | 1 |
| `0x00802e14` | `FinishLoop_On` | 1 |
| `0x00802e30` | `clear_screen_onstill` | 1 |
| `0x00803784` | `Area` | -1 |
| `0x00803df4` | `newlevel_resumecutaudio` | 1 |
| `0x00805a00` | `hub_from_arcade` | -1 |
| `0x00855e00` | `MiniCutCam` | 0 |
| `0x00856c28` | `ChallengeMode` | 0 |
| `0x0085ea94` | `BonusCoinTotal` | 0 |
| `0x008765ac` | `NewMode` | 0 |
| `0x008765e4` | `reset_restart` | 0 |
| `0x00876cec` | `party_cant_be_under_cover` | 0 |
| `0x0087a9e8` | `makefreeplaymodellist` | 0 |
| `0x0087b2f4` | `LevelChangesInArea` | 0 |
| `0x0087b2fc` | `loadareacharacters_loadedlevel` | 0 |
| `0x0087b4fc` | `peak_poly_count` | 0 |
| `0x0087b500` | `poly_count` | 0 |
| `0x0087b544` | `frameout` | 0 |
| `0x0087b55c` | `HubMainRenderTimeHack` | 0 |
| `0x0087b73c` | `new_level_from_menu` | 0 |
| `0x0088a56c` | `dagobah_training` | 0 |
| `0x0093b2bc` | `NextArea_FreePlay` | 0 |
| `0x0093b2c4` | `FreePlay` | 0 |
| `0x0093b2c8` | `SuperStory` | 0 |
| `0x0093b344` | `Arcade` | 0 |
| `0x0093b348` | `Paused` | 0 |
| `0x0093b358` | `GAMEDEMO` | 0 |
| `0x0093d838` | `PLAYERCOUNT` | 0 |
| `0x0093d83c` | `UsePlayerList` | 0 |
| `0x0093d860` | `abort_load` | 0 |
| `0x0093d868` | `next_level` | 0 |
| `0x0093d86c` | `waiting_for_level` | 0 |
| `0x0093d870` | `waiting_for_new_level` | 0 |
| `0x0093d874` | `no_more_loads` | 0 |
| `0x00950764` | `screendump` | 0 |
| `0x00950768` | `pause_rndr_on` | 0 |
| `0x0095123c` | `last_area` | 0 |
| `0x00951388` | `reset_area` | 0 |
| `0x009513a8` | `gone_through_door_to_new_level` | 0 |
| `0x00951440` | `Door_UseCutCam` | 0 |
| `0x00951bc0` | `Level` | 0 |
| `0x00951bc4` | `LevelChange` | 0 |
| `0x00951c00` | `CUTSTOPGAME` | 0 |
| `0x00955b54` | `ResetOldFStop` | 0 |

## Explicit remaining inventory

A read-only scan examined 8,913 sized Android data symbols. It produced 133
uniquely occurring PC byte-correspondence records representing 132 distinct
PC ranges; 80 distinct ranges have direct references in the existing Ghidra
export. Equality supplies a candidate, not a proof of PC type, object boundary,
or original name. Complex structures, codec lookup sets, buffers, strings with
unproven capacity, and arrays without reviewed PC indexing remain unimplemented.
The inventory is `android_equal_ranges.json`; no candidates were copied into the
map solely because their bytes happened to match.

The scan excluded 6,625 Android BSS symbols from byte-search identification
because repeated zeroes cannot establish their PC addresses or extents. It also
excluded 1,465 very small/large candidates and 114 low-information initialized
patterns. The NuMain scalar work therefore uses references and declarations
rather than a zero-pattern match. `deferred-numain-anchors.json` preserves 50
remaining startup anchors and the reason each remains unresolved.

Coverage below counts the union of all current accepted map extents plus this
tranche, including previously inventoried IAT slots and RTTI. It is **inventory
coverage**, not a claim that every field in each record has a recovered type.
File alignment beyond a section's virtual size is excluded.

| Original region | Virtual bytes | Covered by accepted extents | Still unmapped |
| --- | ---: | ---: | ---: |
| .rdata file-backed | 646,766 | 820 | 645,946 |
| .data file-backed | 417,792 | 6,518 | 411,274 |
| .data zero-filled | 34,954,408 | 42,060 | 34,912,348 |

`coverage.json` records all 174 remaining virtual spans. The Ghidra reference
export contains 17,006 distinct target addresses in these data regions; 16,650
are not inside accepted symbol extents. They are listed with reference examples
in `unreviewed-references.json`. Those target counts are analysis candidates,
not a count of symbols: references can point into records, strings, arrays,
vtable metadata or buffers. Recovering the remaining extents requires further
PC-specific review, especially the roughly 35 MiB of zero-filled storage.

## Typed subsystem state added after the initial inventory

| PC VA | Object | Extent and load-time value |
| --- | --- | --- |
| `0x00925038` | `FRAMETIME` | Four-byte zero-filled `f32`; NuMain sets it during startup. |
| `0x029899c0` | `NuTrigTable` | 131,072 zero-filled bytes, 32,768 `f32` entries filled by `NuTrigInit`. |
| `0x00973708` | `global_camera` | One zero-filled 120-byte camera, matrix at offset zero. |
| `0x0080b6fc` | `cam_axes` | Three `f32` values, each 1.0. |
| `0x0080b708`–`0x0080b9c7` | Ten camera matrices | Ten distinct 64-byte identity `NUMTX` objects; individual addresses are in `symbols.csv`. |
| `0x00806b9c`, `0x00806cc0` | Special alpha and tint | `f32` 1.0 and three-float `{1, 1, 1}` tint. |
| `0x008015b4`, `0x008015f0` | Font coordinate scales | Both `f32` 1.0; the PC widescreen setter writes only these two. |
| `0x0297dc00` | `NuVideoFrameRate` | Zero-filled `f32`, selected by PC swap mode. |

The zero/one compiler COMDATs at `0x00751658`/`0x00751524` are mapped from
independent original references and exact bits. The local compiler can emit them
without a relocation when it uses x87 `fldz`/`fld1`.

## Startup and frame state

The current `src/data.cpp` also defines the PC startup menu, episode archive
names, font constants, gamepad records, background-worker state, and debris
constants. Their individual addresses, extents and types are in `symbols.csv`.
The 0x7e4c-byte PC `GAMESAVE` aggregate now owns `options_save` and player
progress in one typed definition. The six options fields copied during
`InitOnce` use layout-checked members of that aggregate. The host-file data
includes 32 native handles, 33 `FILEINFO` records, four 0x408-byte file buffers,
and the 0x234-byte PC device template; `NuFileDevice` has three separately
asserted directory fields. The keyboard owner extent is still unknown and has
no data-match credit; its typed partial layout must not imply a complete size.
The DAT parser reads the signed four-byte `datsys_offset` at `0x0091eebc` and
uses the permanent arena pointer. `SUPERBUFFERSIZE` and
`original_permbuffer_base` have C linker names in this reconstruction so the C
`InitOnce` unit can use typed declarations; their preferred-image addresses,
four-byte extents and values remain mapped. The retail image is stripped, so
these CSV names are reconstruction choices rather than original source names.
`NUAPI` remains one 144-byte object at `0x0297dbe0`; PC references establish
two eight-byte `NuTime` fields at offsets `0x2c` and `0x34`, with layout asserts
in `saga.h`. `ShowingError` at `0x0091ef3c` and `bHaveErr` at `0x00921248` are
separate zero-initialized four-byte globals: the latter is read by `NuHasError`
and cleared by `NuClearError`.

The current primitive state map includes PC `f32` scale/bias constants and
four-byte stream cursor/count owners. Retail initializers establish the exact
`NuPrimPS2XScale`, `NuPrimPS2YScale`, `NuPrimTwo`, and `NuPrimMinusTwo` values;
the corresponding definitions match. The coordinate-system stack at
`0x02981c70` has a reviewed base address but no defensible complete capacity,
so its CSV size and data definition remain absent. The four keyboard profile
names used by the PC command initializer are mapped as exact read-only strings.
The `NuMusic` album and track strides are PC-confirmed as 0x24 and 0x3c, but
the complete `music_man` allocation extent remains unknown. Its CSV row gives
an address without claiming data-byte coverage.

The PC terrain table at `0x008034c8` has 32 twelve-byte records. Its complete
384-byte initializer independently agrees with the Android table; the PC code
confirms the movement scale, flags and final word offsets. The 64 animation
parameter records at `0x0292fe38` have a PC stride of `0x2d4`, with the page
byte at `+0x2d0`. Related zero-filled page arrays, the two 100-entry sound-bit
arrays, and their extents now have separate map and source definitions. The
level-config keyword strings at `0x00761ca4`–`0x00762281` and the
2,000,000.0f sentinel at `0x00751d84` were read directly from the retail image;
they are defined separately so the loader can bind their original references.
The PC before-load and after-load generic keyword tables at `0x00803bb8`
and `0x00803be0` span 40 and 424 bytes (five and 53 eight-byte entries).
Both source initializers match every byte and all 112 pointer relocations.
Their entries anchor 54 PC callbacks. The 47 previously implemented callbacks
and five additional scalar/ambient callbacks match complete code extents;
the two music callbacks are still structural.
The game-specific before-load table at `0x007fa608` has 45 eight-byte records,
including its sentinel. Its 44 keyword strings, 44 callbacks and all table
relocations are now mapped and exact. The retail startup paths for episodes,
areas and levels are defined from their original NUL-terminated bytes.
The PC area fixup selector uses three separate tables: 65, 2 and 5 entries,
including null sentinels. Their pointer initializers and destination globals
are mapped from retail; unlike Android, the PC branches among the LSW,
lost-temple and Gotham tables. Time-bar source and colour constants are defined
from retail initializers. Individual area-name strings still require review of
their compiler-owned alignment bytes before claiming exact matches.
All 56 generic level keyword strings and 29 area/remaining-level keyword
strings are separately defined and their complete NUL-terminated initializers
match the PC image. The PC area record is `0x9c` bytes with a 16-bit flags field
at `+0x7a`; the cheat system uses an initialized pointer at `0x008032c4`
to zero-filled state at `0x00950da4`. Only the first 12 bytes of that state
have a reviewed layout, so its map extent remains blank. The two trial-time
defaults expose 16-bit values 300 and 600; their full owner extents are also
left blank.
