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

The progress/music batch establishes these additional PC layouts and globals:

- `LEVEL_PROGRESS` rows have stride `0x2e28`, a `0x2800` scene-progress prefix,
  minimum time factor at `+0x2814`, state at `+0x2820`, and twelve 16-byte disabled
  effect names at `+0x2d68`. Retail clearing and effect-name loops establish
  these extents; unknown fields remain opaque. `WORLDINFO.scene_progress` is at
  `+0x15c`, `secondary_gscn` at `+0x140`, and the debris page index at `+0x2a98`.
- The 64 level-SFX records begin at `WORLDINFO+0x4710`, stride `0x10`, with a
  signed ID at `+0x0c`; the count is at `+0x4b10`. Resetting IDs does not establish
  the other record fields.
- `Music` at `0x00810490` occupies `0x24` bytes, including its context pointer at
  `+0x20`. PC reset stores, independent references to the following initialized
  word at `0x008104b4`, and the next known object corroborate its boundary. Its
  typed initializer matches. `NOMUSIC`, the debris count/table pointers and the
  eight-entry page-use table also match their zero initial states.
- Block readers index 16-byte records at `0x0090ffc8`: tag, signed size and
  64-bit file position. The counter at `0x00800464` starts at one. The source
  reserves 1,024 records from the Android capacity hypothesis, but the complete
  PC allocation extent is **unverified**: its CSV size stays blank and contributes
  no mapping/exact coverage. Do not derive its size from the next zero address.
- Legal-texture suffix strings are independently bound at `0x007547a4`,
  `0x0075cff4`, `0x0075cff8`, `0x0075d004` and `0x0075d010`. The PC language-zero
  branch stores `2.2f` from `0x0075504c` into two mutable scales initialized to
  `1.0f` at `0x008015ac`/`0x008015b0`. Their `TextScale*_candidate` names describe
  observed use; historical names remain uncertain. All complete initializers
  compare independently against retail.

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

The AI parser now has eight separate four-byte zero-filled cursor/list/state
owners at `0x00922234`–`0x00922258` (with gaps retained); these are named shared
fields rather than offsets into an invented owner. Quaternion, matrix, atan and
spline compiler literals have bindings based on their exact PC bit patterns and
actual retail references. The positive 512.0 double is at `0x007640c8`; no negative
512.0 constant exists in the image, so it must not be given an arbitrary binding.
The noninterpolating table index uses a signed 16-bit temporary, sufficient for
its positive-quadrant 0..512 range; this remains a code-generation difference.
The zero-double binding at `0x00751790` is supported by original qword arithmetic
references, even when a rebuilt routine uses `fldz` and leaves its COMDAT unused.

The animation/flow/sound transfer batch adds independently reviewed ANI3 control
bytes and scratch-pointer globals, the three-row flow callback table with real
function relocations, and PC compiler qwords (4095.0, packed-key scale, angle
scale and 15.0). The GameAudio singleton occupies 0x200 bytes at 0x009248f8: its
name array and ID array have 84 entries, not the initially proposed 85; the
initialized pointer at 0x008020bc refers to this source object. CurrentSFXTAB
at 0x00924d18 covers the 100 u16 words used by PC restoration, without copying
Android's trailing name-pointer member. Five NUVEC sound positions at
0x02962b70 span 0x3c independently confirmed by fifteen initializer writes. The
listener fallback refers to the existing global_camera.mtx, not a duplicate
matrix owner. Fade start/end load as 2.0f/15.0f and random seed as 0x20558057.

The DAT read buffer is defined for source linking but retains an unknown CSV
extent and no matching/coverage credit; spacing to another owner alone does not
establish its capacity. New buffer-size globals and PCImplodeWordMask have
reviewed scalar extents. Explicit bss sections keep small verified data owners
separate from compiler alignment gaps in the COFF comparison.

The movement/string/script batch adds three 12-byte string banks, 8-byte string
entries, a relocated blank-string pointer, UTF-16 filter punctuation, script
keywords, spline tuning values, and the 256-byte legacy animation popcount table.
`bad_fluff` owns 26 words (52 bytes), ending at its first zero; the following
alignment word is excluded. Its PC second character is `$`, unlike Android.

PC sound groups have 128 records of eight bytes and 512 sample-ID words. Bolt
storage has 32 records of 0x138 bytes. The initialized BoltSys pointer references
a real 0x20-byte descriptor and its 0xa4-byte default type, retaining function
relocations. Default callback bodies and missing actions are visible in source;
initializer pointers must never be replaced with null merely to make linking pass.

Five vehicle corner tables each contain four NUVECs (48 bytes). Vehicle IDs and
temporary rotation angles are two-byte owners despite four-byte spacing.
Movement literals retain their exact retail precision: the projectile predictor
qword at `0x00762718` is `0.04000000283122063`, not mathematical 0.04.
PC aggregate views remain partial unless complete owner extents are established;
named fields and offset assertions do not by themselves establish full capacity.

The hover/script closure adds reviewed argument-key strings, Hoth level and
hover-state globals, lookup and animation callback slots, the retail identity
matrix and hover separation qword. The terrain table at 0x0080364c contains 17
12-byte records (204 bytes), supported by PC index bounds, access stride and its
initializer. Only the first u32 is named `flags`; the remaining words retain
neutral labels. Source initialization preserves the exact word values rather
than guessing Android meanings.

The animation scale table at 0x0097ae08 is represented by a 256-NUVEC source
prefix: the evaluator accesses through slot 255. This establishes minimum
accessed storage, not a complete original owner extent. Its CSV size stays blank,
so it receives no mapping coverage or exact-data credit. Evaluator drafts are
not linked while their scratch backing capacity remains unresolved. Do not
replace the unknown scratch owner with a one-byte array: returning that address
for real writes would create invalid source-object accesses.

The repeat-SFX queue has 32 records of 16 bytes at 0x00924b18. Producer head
wrapping with `& 0x1f` and the consumer's stride confirm capacity and fields:
i16 ID, u8 state, i8 repeat count, two float delays and a position pointer.
The head at 0x00924afc and the queue start zero-filled in the PC image. The
Speeder Chase flag at 0x009253c4 is another verified zero-initialized i32 owner.

Bridge state is eight 0xb0c-byte records at 0x02983a80 (22624 bytes), plus the
zero-initialized processing gate at 0x029892e0. The sound volume producer and
consumer independently establish 0x4000 floats at 0x02962bd0; the existing
NuSound3VolumeTable is reused. Sound sample/voice records use PC strides 0x40c
and 0x34. The source SoundVoiceManager provides an observed 0xd98-byte prefix,
not a proven complete owner; its CSV size remains blank and contributes no credit.
Its critical section has the SDK's 24-byte layout, the enabled gate starts at 1,
and the volume offset is signed integer -300. The 3D-buffer IID has the exact PC
16-byte initializer.

The AI special callback and current-terrain pointer are separate zero-initialized
four-byte owners. Path angle/radian scales and the four negative polynomial
coefficients retain the PC qword bits. Polynomial coefficients are widened from
rounded floats, not mathematical double decimals. Bolt shot parameter, three
16-bit class-ID defaults, ID scratch and NUVEC position scratch retain verified
scalar extents; runtime initialization is distinct from the retail image's values.
No scratch allocator capacity is inferred from the spacing of neighboring symbols.

Terrain lifecycle views verify the 0xa490 accessed header prefix, 0x38 group
records, 0x6c platform records and 64 track slots of 0x18 bytes. Capacities remain
runtime fields. The platform cell at +0x1b98 is a separate typed member, not an
out-of-bounds access past the 256 ordinary cells. Platform flags are a dword at
+0x4c as the PC load/store instructions establish.

The camera clip state includes a zero matrix scratch, zero X/Y scale scalars,
six aligned four-lane outcode masks, four minus-one lanes, mutable far limits
initialized to one, an enabled gate initialized to one and a zero SSE gate.
The two SoA corner/result buffers and eight outcode words represent only observed
source prefixes; their map sizes remain blank and supply no coverage credit.
Projection reuses the existing identity `pc_vport_mtx`; scissor scratch is a
separate zero 64-byte matrix. Its four scale scalars start at zero and the writable
far bias starts at the PC float 0.1. The sound multiplier starts at 0x4000, and
the `.ogg` pointer is initialized through a real relocation to its five-byte
named string. Path margin is the PC float 0.05; bolt jitter divides by the
verified double 65535.0.

## Movement/object/AI dependency wave

The first execution wave adds typed context records and the source dependencies
for movement, object targeting, AI locator/path processing and gizmo lifecycle.
The isolated released shared-data packets verify 142 definitions over 8,423
bytes and 112 real relocations; the aggregate report additionally checks emitted
compiler constants and the cheats-TU private timer. See the current report for
exact counts rather than adding isolated and aggregate counts together.

`PCContextInfoTable` has 101 contiguous 16-byte PC records; `CInfo` points at
record one so the `-1` context selects the first row. Flags are one 32-bit word,
not Android's wider interpretation. The final NetWait record ends at
`0x007fb778`, where a distinct action-info table begins. Actual names, actions,
flags and parameters are initialized individually, with named string owners
and real relocations. No capacity was inferred merely from Android declarations.

The dynamic antinode pool contains 64 records of stride `0x58` at `0x00922288`;
PC producer loops establish the capacity. The locator callback is a zero-initialized
typed function pointer. The arcade provider has seven 12-byte modes, confirmed
by initializer boundaries and the PC mode-limit store. `ArcadeItem` is a complete
24-byte record, including signed mode at `+0xc`. Text pointers target independently
verified 16-bit text-ID objects. Android's smaller mode counts are not applicable.

Other recovered dependencies include the 16-byte game timer, object-pool globals,
ForcePush thresholds, target distances and angles, vector initializers, and
mutable cheat/gizmo state. `Cheat_PowerUpTime` remains static in `cheats.c`, its
verified original translation unit; shared data ownership does not move genuine
private state into `data.cpp`. All have PC extent and initializer evidence and
are checked by the normal matcher, with no data copied as an opaque retail blob.

## Second structural release: typed owners and private tables

The shared-data release adds **25 complete definitions / 115 bytes**: sixteen
scalar owners (85 bytes), three typed null callback owners (12 bytes), five
player globals (14 bytes), and the camera-offset scalar (4 bytes). Actual PC
setter consumers establish each callback signature. All initializers match
retail; null initial callbacks do not imply their eventual backend is complete.

The edit-file subsystem preserves eight original-TU private state owners,
including its 4096-byte read buffer, for 4,128 exact initializer bytes. The
player subsystem preserves its 42-row, 12-byte SpecialLayer table and associated
strings in the original TU: 43 definitions, 1,154 bytes and 84 real relocations.
Names, offsets, sentinel/bounds and pointers are established from the retail
producer/consumer, not from Android array capacities or neighboring zero data.

The complete 200-byte ADDPART stack record has a retained typed field/initializer
proposal for the next lifecycle release; it is not credited as a new shared-data
definition here. Particle callback backends and dynamic-joint providers remain
separate code dependencies. Animation scratch backing capacity also remains
unproven: observed allocations do not establish a complete static owner extent.


## Third structural release: parser, effects and typed owners

The shared data writer adds 81 complete original definitions, totaling 2,292
bytes, all verified against retail initializers. These include actual movement
and AI scalars, callback pointers, the 200-byte Default_ADDPART, four blade colour
records, the 88-entry LevHSpecial array, eight LevGizObst pointers, and the
148-byte podsprint record. Full producer bounds establish these array extents;
Android declarations alone do not. The former LevHSpecial_50/51 source owners
are removed: both now address the genuine exact aggregate's elements.

The character configuration TU owns its genuine 208-row keyword table (207
entries and a sentinel), private parser/state/backing records and generated
literals. Five private initializers total 22,096 exact bytes; the real keyword
table accounts for 1,664 of those bytes. Its 307 native literals total 3,759
verified bytes. These remain original-TU owners, not opaque copied retail data.
Level-object count/name pointers and four -1 reference scalars likewise remain
in their proper source owner.

WORLD has distinct typed debris systems: game_debris_sys is at +0x134 and the
existing part_debris_sys is at +0x138. Their entries are separate semantic types;
the game table's 20-byte row contains an effect id and inline 16-byte name. The
16-byte runtime level-object row has a NUSPECIALHANDLE and active byte at +0xe.
The object locator cohort contains 16 matrices, proved by PC loading/drawing
producers; it is not a guessed array capacity.

Explosions is the real eight-record pool at 0x00950db8, 0x38 bytes per record,
448 bytes total; i_explosion at 0x00950f78 is a separate scalar. The owner pointer
is at +8, position at +0xc, radius at +0x18, time at +0x1c, duration at +0x20 and
flags at +0x24. The absolute owner-field address 0x00950dc0 is not the array base.
The PC cursor wraps at eight, proving the full extent. LevGizmo is the real
12-pointer array at 0x0087a98c, cleared by an explicit 48-byte producer. The
remaining fields stay opaque until their semantics are established.

Two working source owners deliberately receive **no original extent or exact
credit**. FadeSys at 0x0095079c is the genuine nonvirtual FadeSystem C++ object;
C consumers use the same named scalar layout. PC initialization establishes the
accessed prefix through +0x28, but not an authoritative complete original owner
size, so its CSV size remains blank. ConfigBuffer at 0x0092b0e8 also keeps a blank
original size. Its reconstruction backing is 0x10002 bytes: the reviewed 64 KiB
source limit plus two stripping terminators. The independent temporary rotation
at 0x0093b0e8 only bounds contiguous original storage above; it does not prove
ConfigBuffer's exact extent. Pack-loaded text has no established 64 KiB bound,
and this working backing does not establish arbitrary inputs safe. No invented
original size or guard is used to improve coverage or fabricate a loader result.

## Fourth structural data release

The fourth checkpoint adds 236 newly exact initializer rows without losing an
existing exact data definition. Shared AI strings/state, takeover state,
contexts and movement tuning use reviewed PC initializers. Producer bounds prove
three further complete arrays: 16 one-byte creature-set counters at 0x00876868,
eight LevGameObject pointers at 0x0087a810, and four LevFlag integers at
0x0087aa40. LevGameObject is distinct from Player. GameObjectState_InActive at
0x00753164 is the nine-byte ASCII string including its terminator, not a record
array. The initialized BigJump callback belongs to jumping.c.

The original socket TU owns its 56-entry callback table plus sentinel (456
bytes), parser/runtime state, names and literals. Its 111 compiled data rows
cover 1,849 exact bytes. Original private owners remain local; shared data lives
in data.cpp. Animation-duration state and the collection list/count likewise
remain in their verified original source units.

LevSafePlatID at 0x00803bb0 is a two-entry working access view initialized to -1.
Its complete original capacity remains unproved, so the CSV size is blank and
it receives no extent or exact credit. Four pad-history owners similarly expose
only the two zero-initialized entries proven by PC player-index consumers:
current/previous speed at 0x00926be4/0x00926bd8 and current/previous angle at
0x009253c8/0x00926be0. Their original sizes stay blank; the source access views
do not claim complete array extents. The upcoming one-at-once attacker table has
an independently proved eight-row bound; preserve the PC index-eight bug rather
than enlarging its storage to conceal an out-of-bounds access.

## Fifth release data contracts

The movement and GameAI cohorts add PC constants, strings and control state.
UpdateGameObjects uses independently recovered scalar globals, the signed
9-by-4 adaptive table and its genuine middle-row pointer relocation. The
adaptive state has named level, accumulator and elapsed-time fields; its full
original extent remains unknown. Hub input arrays likewise remain partial
access views. Initialized callback pointers retain their real null state.

The sole data owner compared all 306 current wave definitions covering 3,860
known bytes exactly, including real pointer relocations. Nine working views
remain excluded from original-size credit. The consolidated report and
unknown-view inventory are retained under ignored build/analysis/wave1/data/
wave3/. These are initializer checks, not proof of runtime state.


## Particle scene page IDs

The complete `part_scene_pageid` owner at `0x0291f6a8` contains 32 signed
32-bit page IDs (128 bytes). Original PC initialization and independently
bounded insertion, loading and clearing consumers establish capacity and
ownership through `0x0291f728`. Its signed comparisons and `-1` runtime tags
distinguish it from the separately identified scene-pointer array.

The original PE virtual storage is zero, so data.cpp defines the array with a
zero initializer in its own BSS section. InitParts' later `-1` stores remain
runtime behavior. The named Android ELF object and original PIC/GOT relocation
support identity; PC consumers establish type and extent. Frozen
`nonregistry-data-frontier-next11` and its independent audit retain the proof.


The separate `part_page_used` owner at `0x029208bc` contains eight zero-initialized
32-bit page occupancy gates (32 bytes), ending at `0x029208dc`. Original
initialization plus an independent eight-slot capacity scan and set/reset
consumers prove that complete allocation. Its `i32[8]` declaration supplies
integer gates without claiming signed arithmetic or unchecked caller safety.
The following 20-byte gap remains unidentified. data.cpp retains the original
zero initializer in a separate BSS section.


## CompleteLevel queue and source access views

NewCutInfoCount at `0x00951c9c` is a complete four-byte signed integer owner.
Independent PC counter consumers and distinct neighboring DWORD owners bound
it through `0x00951ca0`; original Android ELF/PIC/GOT records bind the named
object at `0x01290980`. Original PE virtual storage is zero. The reconstructed
zero initializer compares exactly and earns four mapped and exact data bytes.

NewCutInfo at `0x00951c2c` is independently identified with the original named
Android local queue at `0x01290a80`. Its `CUTINFO *[8]` source declaration is an
access view for initialized count 0..8. Complete PC allocation and historical
array type remain unknown: CSV size/ctype stay blank and there is no extent or
exact-match credit. The original signed upper-bound test does not reject
negative counts; the C implementation adds no such guard.

The four-byte LevelCompletionFallbackFlagPC view at `0x00950790` follows the
original zero-filled PE storage and unconditional DWORD-one store. The name is
descriptive reconstruction terminology; historical identity, signedness and
complete ownership remain unknown. Private ten-byte NUL-backed `cutscene=` and
`newlevel=` initializers at `0x00757df0` and `0x007584d4` belong to the reconstructed
callback unit. Their original pool/allocation boundaries remain unknown. These
three views also have blank CSV sizes/ctypes and receive no extent/exact credit.
All four unknown views resolve compiled references without treating source
emission sizes or Android allocations as PC ownership proof.


## Race and animation-speed literal access views

The source-private RaceOpponentDistancePrefix at `0x007578c8` is exactly the
nine-byte NUL-backed `distance` initializer. The original substring consumer
advances by nine; the source does not strengthen it to a `distance=` token.
Compiler float bindings at `0x0075327c` and `0x0075852c` have independently
verified bits `0x3f666666` (0.9f) and `0x3f8ccccd` (1.1f). Actual COFF definitions
resolve through their emitted __real names.

The animation callback's shared `value=` and `multiply_by=` source initializers
at `0x00757c44` and `0x00757ed0` occupy seven and thirteen emitted bytes including
NUL. Original PC operands and bytes prove these source initializers and access
roles. Complete allocation/pooling boundaries and historical data types for all
five literals remain unknown. Every new CSV row keeps size and ctype blank;
37 emitted source data bytes earn zero original extent or exact-match credit.
No first-NUL span, four-byte scalar access or Android allocation size is treated
as complete PC ownership proof.


### Current-speed field and literal access views

Fresh PC CurrentStart/UseCurrentSpeed review verifies `GAMEOBJECT` C4C as a
float storing raw socket current speed. Only the first four bytes of the old
C4C opaque span become `socket_current_speed_pc`; the remaining C50..C6F
bytes stay opaque. C44 and EC8 are alternative scale factors, not a product.
The existing velocity, target velocity, packet flags, world/socket prefixes
and signed socket index/segment fields are reused with asserted offsets.
These bodies do not access SOCKSYS count, so this cohort grants no new count
or index-bounds claim.

`AIUseCurrentSpeedSnapName` at `0x757c68` initializes the 12-byte source view
`snaptospeed\0`; `AIUseCurrentSpeedMultiplierName` at `0x757c74` initializes
the 11-byte source view `multiplier\0`. The substring parser advances 11.
A retained graph owner labels the latter as 12 bytes although the first NUL
occurs within 11; the extra zero does not establish the complete allocation.
Both CSV sizes and ctypes remain blank, and both receive zero original extent
or exact-data credit despite their 23 emitted reconstruction bytes.


### Scale-override reset view

`AIScaleOverrideResetName` at PC `0x757ca0` has the verified initializer view
`reset\0` (six reconstruction bytes). Its analyzed six-byte graph owner and
first NUL corroborate the access but do not independently establish the
complete original allocation. CSV size and ctype stay blank, so it earns no
mapping or exact-data credit. Existing `AIActionTimeName` at `0x757ca8` and
min/max strings are reused without new allocation claims.

The scale callback reuses known collision-scale and processor70/74 float
fields; no aggregate layout is expanded. Distinct original F32 and F64
1,000,000,000 constants remain bound separately. NuRandFloat's fresh original
52-byte body includes F32 storage/reload, and actual callback output also
preserves timed-target and rate rounding before interpolation.


### Surface reflection callback access view

PC `SurfaceInfo_ExtraReflectFn` at `0x951030` corresponds to native Android
OBJECT `0x12a48f0`, with independent original ELF/GOT and PC access evidence.
The source defines a four-byte zero-initialized pointer access view; CSV size
and ctype remain blank because the complete owner allocation and historical
formal type are unproved. This adds no known extent or exact-data credit.

The retained direct writer installs unnamed PC `0x42a690`/167; two readers call
through the slot with one object pointer and ignore the return. Reviewed native
null/literal-target states establish a qualified physical interface, without
universal writer/alias closure or naming the callback from the global's name.
BSS initialization is not permanent runtime nullness. The new surface/socket
helpers otherwise reuse existing layouts and data. Gameobjects adds another
exact copy of the existing eight-byte F64 epsilon at `0x751788`, with no new
original allocation credit.


### Surface query and getter access views

The query adds six neutral PC source views: GetSurfaceInfoDefaultMaskPc8034c4
(byte20), RuntimeMaskPc95102c (zero byte), WaterHeightPc2920da4 and
RoofHeightPc2921884 (zero float words), SurfaceNormalPc292fdd8 and
WaterNormalPc2962020 (three zero words each). Only the default mask has
file-backed bytes; the others are in the hash-pinned PE virtual tail and
start zero under the loader. This does not establish runtime invariants.

The getters use existing unknown-extent ShadPoly292fc10, new neutral
EShadowPolygonPc292fd64 and ShadowRoofPolygonPc292184c pointer views, and
TerrainShadowGroupIndexPc2961ffc as a signed16 source view. The index name
does not identify the native TerI query pointer. Existing CurrentTerrain292fc88
storage is reused despite two retained inventory aliases. Polygon materials
at60/61 and existing terrain group stride38/type2c/scene30 are verified access
cohorts; no full polygon owner or historical aggregate type is inferred.

All ten emitted views total 48 source bytes. CSV sizes/types remain blank,
including the existing ShadPoly binding, so they earn zero allocation or
exact-data credit. Existing TerSurface384, v01012 and angle/area globals are
reused. The additional exact float2e6 copy reuses an already mapped constant.
Object fields224/E42/284 preserve their exact accessed widths and surrounding
layout; unknown286 remains opaque. Independent data/field evidence is frozen
in surface-query-data-fields-independent-next1.json.


### Terrain scratch and skin constant access views

NuScratchCurrentPc827730 has a file-backed pointer initializer02976d00.
NuScratchBackingAccessPc2976d00 is an aligned2,592-byte rebuilt minimum
BSS view for the reviewed two nested requests and pointer trailers. It is
not a recovered full original buffer allocation or capacity for other users.
The current pointer starts at rebuilt backing through an actual DIR32
relocation; no literal originalVA or null initializer is substituted.

Four additional neutral constant views preserve actual raw values:
TerrainSkinSampleCountPc753170 is F64+8, SampleIndexScalePc76e130 F64-8,
BoundsMinPc76e138 F32-123456792 and BoundsMaxPc76e13c F32+123456792.
The two bounds map addresses were corrected from the initial integration
proposal using independent original data reads before release. Existing
SkinFlipTab32, NuTrigTable and F64 NuTimeBarTop/BoltTargetRandomDenominator
storage are reused. NuTimeBarTop is exactly0.05000000074505806, the promoted
float value, rather than the nearest double to decimal0.05.

All six new CSV sizes and ctypes stay blank and earn zero original-data
extent or exact credit. They emit2,620 source bytes including the backing
minimum. Eight existing compiler constant rows gain additional exact
source copies, preserving all previous copies and original extents.
The local polygon view expands to the observed100-byte stride; verified
Y bounds8/c and material60/61 preserve prior offsets. Group access refinements
retain stride38; unused Y bounds and unknown group fields stay opaque.
Frozen final data reconciliation: terrain-wave20-data-independent-next1.json.

## Terrain scan and pool access views (2026-10-01)

The scan/cast/pool implementation adds 24 source definitions and 23 neutral
blank-extent CSV rows. `TerI` already had a blank inventory row and now has
one `void *` storage definition. Existing terrain/polygon/normal owners are
reused. All these partial views retain blank original sizes and ctypes and
receive zero original allocation or exact-data credit.

Seven pool cells are separate from the scaled-query cursor/work pool:
`TerrainSkinInfoCountPc` (`0x02947c1c`), `TerrainSkinInfosPc` (`0x02947c84`),
`TerrainSkinUseEpochPc` (`0x0292fc24`), `TerrainSkinUseRecordsPc` (`0x0296202c`),
`TerrainSkinPoolSlotCountPc` (`0x02924b68`), `TerrainSkinPoolSlotBytesPc`
(`0x0292fc28`) and `TerrainSkinPoolBasePc` (`0x02924b6c`). Pointer storage
uses local opaque tag declarations rather than exporting unreviewed owners.
The accessed skin-info stride remains `0x1c`: original-shape pointer `+0x10`,
group WORD `+0x14`, pool-slot WORD `+0x16`. Use records have stride eight.

`TerrainSurfaceClassPc` is a WORD view at `0x02947db0`; the distinct DWORD
at `0x0292fde8` is not aliased to it. Normal component accesses share their
canonical vector views. New state starts at the reviewed loader-zero state.
The three literals are promoted float epsilon `9.999999747378752e-06` as
F64 at `0x00763d68`, F32 `65536` at `0x00753200`, and F32 `-2000000` at
`0x0076e148`. The generated F64 `0.05000000074505806` copy is separately
bound to the established eight-byte `NuTimeBarTop` cell at `0x007517c0`.
Existing compiler-literal aliases and their known extents are preserved.
No new image-extent credit follows from overlapping access views.

The independently checked data packet is
`terrain-wave21-data-independent-next1.json`. Its selected original reads
are 24 bytes, and its cumulative semantic metadata is 4,010,331 bytes under
a 4 MiB cap. Earlier predicate failures and the CSV whitespace-only repair
are recorded, rather than omitted from the bookkeeping.

## Shadow wrapper access views (2026-10-01)

The existing mutable `LAYER_HOVERIGNORE` row at `0x00802524` has known size
four and type u32. Its source definition now preserves original writable
initializer `0xffffffff`; reusing this row adds no new mapping extent.
`TerrainShadowStatusPc` at `0x0292fde8` and `TerrainScaleWorkBasePc` at
`0x02962b50` are separate four-byte loader-zero access cells, respectively
i32 and polygon pointer. Their complete original owner sizes/types remain
blank, with zero allocation or exact-data credit. The DWORD status is distinct
from the existing WORD surface-class cell at `0x02947db0`.

WORLDINFO exposes the accessed platform-list pointer at `0x2adc`; its AI
pointer remains `0x2ae8` and size `0x51ac`. The private list uses signed count
at +4 and inline entries at +0x18, stride0x14, with a GAMEOBJECT pointer at
entry+0. Only that counted access view is known; the tail declaration supplies
no owner count or universal capacity. GAMEOBJECT exposes the float at +0xd6c
without changing size0x10d8. Existing floor, roof, surface IDs, normals and
character links are reused. Compiler layout assertions and independently
reviewed original/COFF selected cells preserve these distinctions.

## Snap action data views (2026-10-01)

The actions reuse Player at 0x0093d810, the existing eight-pointer party owner;
LevGameObject is a different owner. RaceOpponentDistancePrefix at 0x007578c8
keeps its canonical gameaiactions definition and existing blank map extent;
exporting it lets the new action reuse that storage without a duplicate owner.
Eleven first-NUL parser string views add blank-size, blank-type map rows, with
zero allocation coverage or exact-data credit. Source array lengths do not prove
full original allocations.

Two source F64 aliases reuse established eight-byte scalar cells: yaw factor
182.04444885253906 at 0x007578a0 and positive zero at 0x00751790. All compiled
copies match the original bits; overlapping aliases add no mapped-image extent.
Sock retains four random calls and four external F64-zero multiplications.
`action-snap-wave23-data-independent-next1.json` records the original cells,
canonical owners and generated readonly data; unknown owner extents remain held.
## Structural identity-only data release (2026-10-01)

Three paired original identities retain blank sizes and ctypes:
`_PodRacePc` at 0x00882a8c (Android 0x006a9540),
`_SceneFileExtensionPc` at 0x0075a1fc (Android 0x00565a14) and
`_GizmoSnapOptionNamePc` at 0x00757e88 (Android 0x005694cf).
The first is pointer storage established by paired writer/access operands;
the other two are first-NUL `.gsc` and `SNAP` views established by paired
string-consumer operands. A pointed-object clear does not establish pointer
storage allocation. Android ELF size and pooled string view lengths do not
establish full PC allocation or historical types.

These rows add zero allocation, source or exact-data credit. Canonical graph
IDs distinguish the PodRace data-object from the two data-sites. Original
review and independent mapping-release evidence preserve the unknown owners.

## Data identities from structural function ancestry (2026-10-01)

Ten fixed experimental leads now have independent original operand/semantic
reviews and neutral PC inventory names. All original sizes and ctypes remain
blank; no source definitions, allocation coverage or exact-data credit are added.

| PC address | Inventory name | Native Android address |
| --- | --- | --- |
| 0x007f8cc8 | GizForceSfxLoadVersionPc | 0x00623150 |
| 0x008028b0 | LegoContextNetWaitPc | 0x0066723c |
| 0x0087b9dc | DrawBossHitPointsActorPc | 0x006a1bb0 |
| 0x00950600 | FindSlamOriginUseCurrentPositionFnPc | 0x012a5890 |
| 0x009507cc | ExBlowupFlagsPc | 0x012a2280 |
| 0x009513d8 | DoorExitNamePc | 0x01279e60 |
| 0x0095144c | LastDoorPointerPc | 0x01279db0 |
| 0x00956640 | RippleEffectsWorldInfoPc | 0x012a4980 |
| 0x0095deb4 | RunOutOfPosTabSpacePc | 0x012a5b50 |
| 0x00973904 | DdMaxJointsPc | 0x011aa080 |

The reviews establish distinctive reset/read/write behavior: the version
initializer and byte-loader gate, network context predicate, boss actor/icon
guard, explosion bits, door compare/copy/recompute/store chain, ripple world
pointer consumers, animation joint-count maximum and sticky position-table
overflow. Original native ELF records and actual GOT relocation addends support
the bindings. Loader zeros alone supply no identity evidence. The callback slot
has an independently observed installer and consumer; its callback signature,
target identity, complete alias writers and runtime state remain unapproved.

The exit-name view has no established PC string capacity. Android object sizes,
four-byte accesses, graph undefined types and lack of nearby symbols do not prove
PC allocation boundaries. Tentative user functions remain semantic witnesses;
none receives a function/ABI/source binding merely because it supports a data
identity. Five frozen `structural-selected-data-pair*-original-next1` packets
retain these distinctions under ignored continuation. The fixed ten-lead result
does not measure accuracy across the experimental candidate pool.

An independent ELF routing audit checks the actual PT_LOAD file offsets for
all eight data GOT addends and the two initialized native values. The writable
segment's virtual addresses differ from file offsets by 0x1000; each read uses
its containing segment. All values and relocation records agree with the
reviewed identities. These checks do not establish PC allocation sizes.

## Collision and level-state identities (2026-10-02)

Three independently reviewed identities are added with blank PC sizes and types:
ForceBackPosPc at 0x0087b8ec (native 0x006a2520), MAULA_LDATA at 0x0087b0d8
(0x006a15e0), and DOOKUC_LDATA at 0x0087b13c (0x006a1570). Reset, setter and
directional movement operands establish the force-back position cell. Original
Maul_A/Dooku_C constructor strings, returned-pointer stores and distinct PartKill
dispatch cases establish the level globals. Similar field offsets alone gave a
wrong Maul-to-Dooku proposal, now explicitly rejected in the journal.

These bindings add no source definitions, allocation coverage or exact-data
credit. The companion force-back actor cell and MAULF/Explode1 alternatives remain
held. Native sizes and platform field layouts do not establish PC ownership.
Original and independent packets are frozen as
`structural-fixed-data-families-original-next1.json` and
`structural-fixed-data-families-independent-next1.json` under ignored continuation;
[symbols.md](symbols.md) records the reviewed roles and remaining limits.

## Animation scratch working views (wave 48)

The existing `ANI3_GLOBALBUFFER_PC` prefix now names the signed 16-bit capacity
at +4; its animation-values pointer remains at +8 and the working prefix is
12 bytes. These offsets describe selected accesses, not a complete historical
allocation. The callback owner at 0x9738a4 remains unchanged.

`AnimQuatDepthPc` at 0x9738a8 is a signed 32-bit working counter, and
`AnimQuatSavedPc` at 0x96e508 is a four-byte working save view. Selected loader
zeros and accesses are independently verified. Original allocation sizes and
types stay blank in the inventory, so these definitions supply zero data
coverage and exact credit. The supported working domain is depth zero through
four; the original signed upper guard does not establish negative backing
storage or arbitrary runtime initialization.

## Conditional deformer backing views (wave 49)

Four source views keep blank original sizes and types: weight cursor 0x80e1a8
(initial 61184), pointer cursor 0x80e1ac (initial 2048), pointer backing 0x97c250,
and weight backing 0x9922b8. Cursor values and selected loader zeros are original
verified. They receive zero allocation and exact-data credit. The promoted-float
epsilon at 0x7531a8 reuses its existing literal binding.

The reconstruction weight view has 60669 blocks of 520 bytes (31,547,880 bytes);
its maximum endpoint follows from cursor 0..61184, counts 0..128 and no
cursor-raising writes. Increasing count lowers the selected index enough to
keep the endpoint below the count-zero case. The 2048 pointer slots additionally
require a maintained nonnegative pointer budget, since the original allocator
tests the weight counter. These conditional prefixes fit the PE virtual zero
tail and avoid every captured mapped base/known extent; they do not prove
original allocations, reset writers or arbitrary alias states. Their BSS
backing is checked without reading or materializing a 31MiB original byte array.

The DWA header exposes only its verified 20-byte prefix through the curve-type
pointer at +0x10. An unaccessed node-flags proposal at +0x14 was removed before
validated publication. The later skeletal path needs its own separately
reviewed +0x14 pointer view. Time fields and 128-value weight-block working
strides retain asserted offsets. `dwa-pool-data-independent-next1` records the
conditional bounds, raw scalar initializers and ownership holds.

## Skeletal maximum and layer dispatch working views (wave 50)

DdMaxJointsPc at 0x973904 now has a source definition for its signed 32-bit
running-maximum accesses. Retained original PE virtual-tail evidence establishes
the selected zero initializer. Its existing inventory size and type remain blank.

ModelAnimLayerListDispatchPc at 0x80fb94 is a conditional three-word cdecl
function-pointer view. Exactly four original file-backed bytes establish its
initial target, PCBuildModelLayerList at 0x62e190. Source preserves indirect
dispatch and requires a compatible installed callback producing valid indices
in the 32-entry caller array. This does not prove later writers or all callback
states. Actual DIR32 emission remains a compiler gate.

Both views receive zero original allocation and exact-data credit. Shared
skeletal, pivot/override and model/packet additions replace only verified
working field cohorts; they do not promote historical allocation/type claims.

## Cutscene animation working views (wave 52)

The generic locator, rigid and character animation views assert the PC
animation pointer offsets 0x50, 0x4c and 0x44 respectively. Character rate,
flags, index, unsigned blend and signed start accesses retain offsets 0x58,
0x5c, 0x5d, 0x61 and 0x63. ANI3 uses distinct working prefixes and curve-type
storage; the legacy skeletal view reuses its separately verified flags
pointer at 0x14. These are access contracts, with zero new original allocation
credit. The existing DWA prefix remains unchanged.

Compilation emits an additional copy of the already mapped zero literal at
0x751658. Every copy is compared, and the copy does not create another
original extent. Angular conversion uses the existing verified double at
0x755d18; no new source backing globals are introduced.

## Android transfer execution (2026-10-03)

The shared data transfers include PC extension registrations and their original
suffix literals, audio state, AI parameters, world transition state, laser arrays,
font diagnostics and the fixed 128-record part-type lookup table. Arrays use
producer-established strides and counts; the part table spans 0x2913a78 through
0x291f678 with 0x178-byte records and a 16-byte lookup name. Its untouched initial
state is zero. Unknown record fields remain opaque.

Unknown original allocation extents remain blank in the symbol map. In particular,
`AreaGlobals_PC` is a 0x34-byte access view; its progress field has one mutable
owner rather than a second independently allocated overlapping scalar.
`NuPrimCoordSystemStack` has a minimum two-element access view for the currently
transferred, nonnested laser push/pop paths. This does not establish original
capacity; transferring callers with greater nesting requires extending the view
from evidence. The eight diagnostic status pointers and bounded character-ID
views likewise receive no complete-allocation matching credit.

See [the execution record](transfer-execution.md) for verification and remaining
source dependencies.


### Resource continuation data checkpoint (2026-10-03)

The resource/debris continuation adds 79 independently checked definitions to
src/data.cpp (307 total). Real generator/momentum dispatch tables use named
rebuilt functions and retain original null slots. Grass matrices/clumps, wind/fade
groups, native particle tables and texture load-order storage are runtime pointer
owners; no backing capacities are guessed. Native particle maximum defaults to
64, debris chunk counts to 256/32, and the terrain context is an opaque pointer.

206 proven extents compare exactly across 88,072 original bytes. The other 101
views cover 5,820 source bytes with blank original extents and no allocation or
exact-data coverage credit. Retained initializer evidence and combined map/header
proposals are in the execution analysis data handoff. See the updated execution
record for the whole-wave comparisons and still-missing real provider families.


### Lifecycle continuation data checkpoint (2026-10-03)

The sole shared-data writer now maintains352 checked definitions,45 new in this
wave. Its current tracked cohort has215 proven extents/88,258 exact original
bytes and137 unknown extents/7,117 bounded source bytes without allocation credit.
Existing legacy data remains separate from this cohort. The legacy PART emitter
map was corrected from its interior effect field291f7d4 to allocation view291f7c8;
its former4,320-byte extent credit is explicitly withdrawn until allocation
proof exists. The independently checked zero source view remains4,320 bytes.
The original embedded fallback image is held rather than copied into the repo.
See the execution record for actual source checks, producer layout corrections
and the exact-data credit change.


### Runtime continuation data checkpoint (2026-10-03)

The shared-data cohort now contains374 checked definitions:219 known extents cover
133,487 unique exact original bytes;155 unknown extents cover7,958 bounded source
bytes without allocation credit. Five duplicate owners were removed, including
terrain state and overlapping display fields. New particle page arrays retain
actual producer-proven128/32/45,056-byte extents. The display registry's256-pointer
view and144-byte dynamic scene have blank original extents. Two legacy display
scalar credits are intentionally retired after ownership consolidation.

The separate legacy emitter array's corrected291f7c8/4,320-byte extent is now proven
by InitParts' explicit clear and40-record loop and compares exactly. Its prior
withdrawal was justified by the evidence available at that checkpoint. The separate
canonical CurrentTerrain four-byte owner and unchanged GizObstacle callback remain
exact; the latter uses a dedicated BSS section to prevent compiler padding from
becoming an apparent eight-byte definition. No data comparison is truncated.


### Terrain/movement continuation, 3 October 2026

The frozen data cohort now contains 435 scoped definitions: 248 known extents
covering 133,853 unique bytes and 187 unknown extents providing 15,343 source-view
bytes with zero original allocation credit. This checkpoint adds 61 real
definitions. Every previous proof remains unchanged. Diagnostics reuse the
canonical blank string and message format; the native debug prefix is 12 bytes
including NUL. ScaleTerrainT1 aliases the existing TerrainScaleWorkBasePc owner
at 0x02962b50 rather than defining duplicate storage. The terrain counter's old
inventory-only label is replaced by terraincnt. The selected object-switch view
is 128 zero bytes with original extent blank; valid tested trigger indices are
0 through 127, with -1 preserved as the sentinel. Receipts, canonical-owner
proof and sorted proposals are retained under the ignored transfer-execution
data directory.


### Publication/scanner data checkpoint (2026-10-03)

Added 86 genuine definitions; all 435 baseline proofs are unchanged. Scoped
receipt: 521 definitions, 286 known extents covering 134,250 unique bytes;
235 unknown extents expose 15,837 source bytes without extent/exact credit.
BSS, pointers, initialized values and eight camera corners were independently
checked against PC. Carry 8.0/blowup owners are reused; compiler constant aliases
retain prior bindings. Scanner margin doubles have exact 8-byte initializer
views but blank original extents. Camera/cache/output/registry backing remains
bounded source views without guessed allocation capacity.


### Directional/native/geometry data checkpoint (2026-10-03)

The verified scoped receipt now contains 798 definitions: 521 known extents cover
136,648 unique original bytes; 277 unknown extents expose 20,606 source-view bytes
without original allocation or exact-data credit. This release adds 276 physical
definitions, including the genuine compiler-generated CRC startup entry, and expands
one existing PCDeviceBackend owner. All 521 prior definitions remain unchanged.

ActionInfo retains all 234 actual eight-byte entries, with 231 new complete NUL
names and existing canonical/pool strings reused. PlayerBlockActionInfoPc points to
entry 1 through a real relocation with addend 8. CRC instance/table/program hashes
and actual constructor startup pointer are verified against PC. CRUISERD_LDATA
is genuine null-initialized LEVELDATA_PC pointer storage, with its actual producer
and guarded consumers checked. Shader capabilities extend the same backend source
view to 0x658, preserving its old prefix; its historical allocation extent is blank.

Vertex declaration cache exposes one entry at 0x0297cc98 and count at 0x0297cc90.
Initial insertion and same-key reuse are its established source-view domain; a second
distinct insertion is outside it. No historical array capacity is inferred from
Android. Particle native/software buffers use verified selected domains of four/eight.
TerrWallTab provides four zero bytes for observed indices 0/1 with unknown original
extent. ATST IDs, camera translation and idle string reuse canonical existing owners;
GrapWindLp's old uncompiled inventory name now binds its actual full string owner.
Compiler immutable aliases are preserved; overlapping coverage is counted once.


### Solver/clone/draw/carry data checkpoint (2026-10-03)

Added 35 real physical definitions, preserving all 798 prior scoped definitions.
Receipt: 833 scoped definitions, 522 known extents/136,652 unique original bytes,
311 unknown extents/21,408 source-view bytes without unknown-allocation credit.
SphereData base is 0x02921680, not its radius member at +12: the actual producer
stores 16-byte records while signed count is below 16. TerrPolyObj reuses the
canonical TerrainShadowStatusPc owner directly, without duplicate storage/macro.

The Draw cohort adds exact four-byte "deb\0" plus nine bounded views, including
the actual mutable render scale initialized 1, suspend switch -1 and editor pointer
relocated to its real zero state. Portal plane and material array capacities retain
blank original extents. The impact threshold is actual f32 4.0 and the optional
death callback is actual NULL. TerrWallDeflectYScale is a genuine zero f32 whose
PC setter 0x0066b320 stores its argument and consumer 0x00678057 reads it. The
previous inventory-only four-byte extent is cleared absent allocation proof;
its entire source initializer is checked, with no new exact/extent credit and no
loss of prior compiled exact data. Solver literal comparisons separately prove
actual promoted-f32 .95/.003/.001 constants.


### Query/occlusion/pickup data checkpoint (2026-10-03)

Eleven new physical definitions and two in-place mask width corrections have
complete source initializer proofs. Scoped totals are 846 definitions:
522 known original extents/136,652 unique bytes and 324 unknown extents/21,460
source-view bytes. Every previous 833 scoped proof is preserved. All thirteen
new/corrected four-byte views retain blank original extents, so their source
initializer equality adds no historical allocation or exact-data coverage.

The surface mask union exposes byte .low and DWORD .word in the same canonical
owner. GetSurfaceInfo retains its original byte reads; reviewed TerrainPlayer
and real address-taking setters require four bytes. Initial default is 0x20,
runtime is zero; dynamic setters write DWORD values. No duplicate storage or
retirement of prior exact-data credit occurs.

TerrPlatDis is signed -1, PCPortalsEnabled is 1, and PartVectorSpreadDegrees_PC
is integer 30. AreaPickupGravity/AreaPickupScale are actual zero f32 owners.
The reviewed blowup angular span is integer 60, proven by MOV/FILD consumers,
and its optional sound callback is actual NULL. TerrainPlayer movement scales
2.5/15, connection tolerance .01 and shadow switch 1 remain mutable storage,
not substituted literals. Source-view/relocation proofs use the actual compiled
data.cpp object; unknown original capacities are not inferred from these reads.


### Message storage checkpoint (2026-10-03)

Added actual zero GameMessage[128] at 0x00947be0 (35,328 emitted bytes) and
GameMessageIndex at 0x009505e0 (four bytes). The array view ends exactly before
the index; independent overlap checks and complete source initializer comparisons
pass. Both historical original extents remain blank. Scoped proofs become 848:
522 known extents/136,652 unique bytes and 326 unknown extents/56,792 source-view
bytes, with no additional allocation credit. All 846 previous proofs remain
unchanged. Pickup table initializers remain held on their genuine nonzero callback
providers; no raw addresses or NULL substitutes are introduced.


### UI, scoring and jump data checkpoint (2026-10-03)

Added 43 physical shared definitions plus the real private landing function
pointer in jumping.c. Every complete source initializer matches PC. Four NUL
strings establish 40 original bytes; the other 40 new views have blank original
extents, so initializer equality establishes no allocation credit. Shared scoped
proofs become 891: 526 known extents/136,692 unique bytes and 365 unknown
extents/57,062 source-view bytes. All 848 previous proofs remain unchanged.

STATSPOSY 0x007fc310 also supplies PCArcadePointMessageY; AddGameMsg_Default
0x00802cd8 supplies PCArcadeMessageTemplate; existing BuildUpTotal 0x0087b9a0
supplies ScoreBuildUpDonePc. Header aliases preserve one canonical mutable owner.
The separate BuildUpDone 0x0087b9b4 is unchanged. The private landing pointer
0x00802d9c relocates to genuine BigJump_LandAction_Default 0x00561dd0 in its
original source group, with the complete pointer initializer and callback exact.
Nine optional TerrainPlayer-only data definitions and the full pickup table
remain held; no surrogate pointer or callback storage is created.


### Pickup table and vehicle breakup data checkpoint (2026-10-03)

Seventy new physical shared definitions have exact complete initializer regions
totaling 2,113 bytes. The scoped proof set reaches 961: 549 known original extents
cover 137,497 unique bytes; 412 unknown extents cover 58,370 source-view bytes and
receive no allocation credit. All 891 prior proofs remain unchanged. Of the new
definitions, 47 retain blank original extents; their source views total 1,308 bytes.
Twenty-three known extents total 805 bytes, including the already inventoried
576-byte accessory allocation, so this is not 805 bytes of new mapping coverage.

GizmoPickupType at 0x007f8df8 is the actual ten-record, 56-byte typed table. Its
twelve nonzero callback slots bind nine genuine production functions; no NULL or
integer-address substitutes are used. Table literals have single canonical owners.
PowerUpLoopSfx_PC includes its actual 18-byte NUL-terminated initializer.
GizBuilditGDeb is the twelve-byte all-minus-one view at 0x00804e38; 0x00805638 is
unrelated. PANEL_COINSCALE_START is mutable float 1.0 at 0x007fc2f0, not the unrelated
0x008033a4. DrawCoinTotalTime at 0x0087b960 is genuine zero storage, and the complete
four-byte multiplier format is at 0x00758ca8. All four are checked against retail.
Nine TerrainPlayer-only definitions remain held. Renderer-specific data stays in
reviewed scratch until its genuine source/provider batch is ready.


### Renderer and connected provider data checkpoint (2026-10-03)

Twenty new physical shared definitions have exact complete initializers totaling
248 bytes. Scoped proofs reach 981: 550 known extents/137,513 unique bytes and
431 unknown extents/58,602 source-view bytes. All 961 baseline proofs remain
unchanged. Only env_padLight_on at 0x00759718 establishes a new complete 16-byte
NUL string extent; nineteen new views total 232 bytes and receive no allocation
credit. Wind and renderer scalars have verified zero/one/minus-one initializers;
projection adjustment is {1,1,0,0}, and its two matrix views are initially zero.

Actual shadow append code caps the pointer list at four records; the two working
pointer/result arrays therefore have four entries each. This producer domain does
not infer original allocation extents or introduce corrupt-state guards. Material
pool pointer/count retain genuine zero storage at 0x0096e580/0x009715b0. Snake
segment height and upward speed are the actual .02f/1.0f at 0x00805140/0x00805144.
All source owners are canonical, with full compiled-region and raw retail proofs.


### Combat/context initializer checkpoint (2026-10-03)

Twenty-nine new canonical definitions have exact complete initializers129B.
Scoped proofs reach1,010:553 known extents/137,551 unique bytes and457 unknown
extents/58,693 source-view bytes. All981 previous proofs are unchanged. Three
complete NUL strings at0x755b28/0x755b88/0x755b94 establish38 bytes;26 mutable
views totaling91 bytes retain blank original extents and zero allocationcredit.
Fourteen new compiler-owned aliases are mapped from actual emitted literals,
including the deflection f64 constants10922/-5461; no duplicate storage is added.
Torpedo max5 is now independently established by actual getMaxTorpedos72; local
count/state/timer/positions views retain asserted PC offsets and selected domains.


### Vehicle and dynamic grapple data checkpoint (2026-10-03)

Six new canonical views have exact complete initializers totaling 1,012 bytes:
three mutable gunship hover controls (2.0f/1.25f/.8f), two weapon context values
(-1), and the four-record dynamic grapple view (992 zero-filled PE virtual bytes).
All 1,010 previous proofs remain unchanged. Scoped totals are 1,016 definitions,
553 known extents/137,551 unique original bytes, and 463 unknown extents/59,705
source-view bytes. All six new extents remain blank with zero allocation credit.
The grapple loops establish four working records at f8 stride and fields1d/1e/54;
they do not establish a complete original allocation extent. Actual production
compiled initializer regions are compared through the repository relocator.


### Vehicle public callback/state views (2026-10-03)

The sole writer adds seven canonical initializer views totaling22 bytes:
WeaponInOut_NoAIJediSfx4, DisorientAxisPc1, TractorBeamEnabledPc1,
VehicleLoopTimePc4=1.5f, GizmoBlowUp_NoTargetFn4=NULL and the two fighting
action callback pointers4 each=NULL. All full initializer regions match
retail and all1016 previous proofs remain unchanged. Original allocation
extents remain unknown; no full-allocation coverage credit is assigned.
The scope is1023 definitions:553 known extents over137551 unique bytes,
470 unknown views over59727 emitted bytes. Literal/NUL aliases used by
actual compiled instructions are separately checked and mapped.

Awkward's14-row sphere table and two ID words are verified scratch proposals
for the next coherent source group, not current production storage. FindID
reuses the existing StartupGameFlag0 owner rather than defining its8765a0
flag again, and duplicate WORD views at7f0e98 are reconciled before release.


### Terrain/music/force source views (2026-10-03)

The sole writer adds11 shared definitions/6356 initialized view bytes: three
Awkward sphere/ID views172, six music mutable views24, force records6144 and
canonical LevelTimer16. All initializers match their full emitted regions, all
1023 prior regions remain unchanged and all new owning extents stay unknown.
Scope reaches1034 proofs:553 known extents/137551 unique bytes and481 unknown
views/66083 emitted bytes. Unknown views receive zero allocation coverage credit.

PossibleForceTargetsPc has a384-record working view, not an established PC
capacity. The original unguarded writer is retained under the selected contract
of <=384 cumulative eligible writes. LevelTimer's typed16-byte layout is proven
by real initialization/reset/update/selector accesses; its separate original
allocation boundary is not proven, so the old16 inventory extent is withdrawn.
PCMusicDuckOverride810bf0 is mutable1.0f and replaces the uncompiled constant
candidate; sound sample storage reuses the existing manager member at2973974.

Six traffic-private owners remain in traffic.c with396 emitted bytes and blank
owning extents. Twenty-three initializer regions totaling548 bytes are verified,
including genuine callback/parser tables. They are reported separately from
the shared data scope. The future49-definition vehicle proposal stays scratch
until the connected Cannon/weapon callers are released.


### Required Cannon/weapon mutable views (2026-10-03)

The sole writer releases exactly45 required canonical mutable owners/112 bytes,
each with a real emitted reader and full initializer comparison. All1034 prior
regions remain unchanged. All45 original owning extents remain unknown and
receive zero allocation/exact-allocation credit. Scope1079 comprises553 known
extents/137551 unique bytes and526 unknown views/66195 emitted bytes.
StartupGameFlag0, BATTLEOVERCORUSCANT_ADATA and id_CATAPULT reuse existing owners;
id_ZAMSSPEEDER and the physical WORD7f0e98 each have one canonical owner.

The unused SmartBomb/Shield NUL strings20 bytes, Shield scalar4 and Launch
scalar4 remain scratch, with no production definitions. Actual compiler-owned
R2Move7, paddle sound21, R2Zap6, Jawa_Dizzy11 and910.0f64/8 regions are separately
proved and mapped. They add53 complete literal bytes, rather than invented
shared allocations. Initializer equality does not prove runtime state or owning
capacity for the unknown mutable views.


### Required rendering and root-motion views (2026-10-03)

Twenty-three canonical shared physical owners provide29,215 complete initialized
bytes for actual rendering readers. Every view matches retail, and all1079
previous scoped regions remain unchanged. All23 original owning extents remain
unknown/blank, adding no allocation or coverage credit. Scope1102 comprises553
known extents/137551 unique bytes and549 unknown views/95410 emitted bytes.

PCShadowPackets is a280x104 working view supported by the signed native count
guard and complete record accesses. Selected valid nonnegative counts do not
establish original backing capacity or justify invented invalid-input guards.
Future cutscene owners reuse PCShadowDrawMode8264bc and PCShadowDrawEnabled8264c4.
Every2790 complete old/new emitted data region agrees between aggregate and
frozen isolated production objects. A TempOptions20-to16 padding movement was
scratch-only; actual production preserves its prior20-byte emitted view.


### Callback and Text/input required views (2026-10-03)

The sole writer adds16 callback views49B and164 Text/input views1915B, each
with a real emitted reader and full initializer proof. All1102 previous scoped
definitions remain unchanged. Scope1282 comprises553 known extents/137551 unique
bytes and729 unknown views/97374 source-view bytes. All180 new original owning
extents remain blank, adding no allocation/coverage credit.

Callback state excludes five unused loader globals. Its private352-byte table
remains in the original callback owner, never duplicated in data.cpp. Input
adds19 state views170B,144 named key strings793B and238-pointer table952B.
The existing singleton, literal1 and WorldPathSeparator are reused. The link
array's3x12 rawzeros match retail; runtime reset stores are not initializers.
Key capacity and other backing extents remain unknown. Every2971 complete named
data region agrees between aggregate and frozen isolated production objects.
Selected presets0..9, button/axis11/16, overrides32, links0..2 and keys0..237
define the tested working domains, not inferred historical allocations.


### Beast, level sound and torpedo required views (2026-10-04)

Thirty Beast/Punch/opponent views120B and fifty audio/Torpedo views207B add80
physical definitions327B. Scope1362 retains553 known extents137551 uniqueB;
809 unknown views97701 sourceB receive zero allocation/coverage credit. All1282
prior scoped proofs remain; every3051 complete emitted normal data region,
extent, raw initializer and symbolic relocation equals the frozen isolated object.

The canonical ComboOpponent_Behind and MOSEISLEYC pointer have one physical
owner each. Actual NetWait stores-1. Cheat flags are BYTE views, not four-byte
neighbor copies. Audio adds45 pointer views180B after reusing MOSEISLEYC;
Torpedo adds five views27B, including full XWing_Torpedo14-byte NUL storage.
The existing two-byte Text keyX owner supplies the target marker. Eight earlier
uncompiled pointer inventory rows lose unsupported4-byte owning extents,
retiring32 mapped inventory bytes with no previous exact initializer loss.
Bounded initialized views remain proven independently from allocation extent.


### Vehicle/POD/terrain and cutscene shared views (2026-10-04)

Eight kernel views 32B plus36 Vehicle/POD/terrain views 202B and five loader states 20B
add 49 physical owners 254B. Four actual NUL strings 44B have complete known extents;
45 mutable/pointer/array views 210B have blank original extents and zero allocation
credit. Scope 1,411 comprises 557 known extents 137,595 uniqueB and 854 unknown views
97,911 sourceB. All1,362 previous scoped initializer proofs remain unchanged;
all 3,100 emitted normal regions, extents, raw initializers and symbolic relocations
match the isolated frozen data object. Existing canonical snake ID and text marker
storage are reused.

Pod pitch history uses actual player indices0/1 and sprint lift positions the
actual three-iteration producer. These finite source views do not prove original
allocation capacities. Loader CS_buffend/CS_cutsys/override callback/CutList/CUTCOUNT
are five genuine four-byte BSS owners, separate from CutSceneSys draw settings.
Cutscene single-stream mode is actually2, while the other kernel states retain
their verified zero/NULL initializers. Source-private compiler literals remain
with their real owners.

The next resource reset working view at 87a830 spans 48B and overlaps canonical
MiscTime f32+20 plus target-degree WORD fields+24/+28. A future release must merge
those three physical owners into one asserted typed view, preserve each initializer
and report changed nonexact getter codegen explicitly. No duplicate mutable reset
array may be introduced. That reconciliation is not part of this checkpoint.


### Renderer and UI physical views (2026-10-04)

Nineteen new shared owners add138 complete initializer bytes: two immutable NUL
strings24B have known extents;17 mutable/pointer/array views114B retain blank
original extents and zero allocation credit. Scope1,430 comprises559 known
extents137,619 uniqueB and871 unknown views98,025 sourceB. All1,411 earlier proofs
and all3,100 earlier complete emitted regions remain unchanged; every3,119 normal
emitted region agrees with the frozen isolated object in bytes, size and symbolic
relocations. Original private compiler literals remain with genuine source owners.

DEBPAGE_AREA4, things_scene4 and SURFACEBITS_NODUST2 were uncompiled inventory
extents without allocation evidence and become blank, retiring10 inventory bytes
without old exact-data loss. Existing canonical font/input/spline state is reused.

The future resource reset union preserves all physical initializers, but merging
TargetDeg_Mid and TargetDeg_Far into its typed fields removes two historical exact
WORD records4B from the current COFF-symbol report. Contrary earlier research
notes, those records were compiled and exact. That release remains held. VC8
global anonymous unions cannot emit the required external field symbols; no
assembly aliases, duplicate mutable owners or hidden metric adjustments are used.


### Draw, RTL, UI and locator physical views (2026-10-04)

Seventy-three new shared owners add3,488 complete emitted initializer bytes:
Draw/RTL17/3,245, prompts8/36, renderer4/41, UI14/47, cutscene26/103,
locator backends3/12 and CutStopInfo1/4. All retain blank original extents and
zero allocation credit. Scope1,503 comprises559 known extents137,619 uniqueB
and944 unknown views101,513 sourceB. All1,430 previous proofs and3,119 earlier
complete regions are preserved; every3,192 complete normal named region equals
the frozen isolated object in bytes, size and symbolic relocations. Compiler
literals and callback/filter tables remain with their genuine private owners.

CutStopInfo at951c08 is a real zero-initialized pointer compared by native
cutscene drawing and cleared by CutScenes_End. Its unsupported4B inventory
extent is retired, without an earlier compiled or exact-data loss. The source
provider fixes the actual final-link dependency. No invented capacity or retail
address initializer is used. Next Init/POD37 physical views613B include the real
relocated24-byte Suit records and10 mutable text IDs; those scratch proposals
remain outside current source/data credit until production release.
