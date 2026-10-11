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


### Creature, Suit and POD initializer release (2026-10-04)

Thirty-seven shared physical views add613 complete initializer bytes, all with
blank original extents and zero allocation credit. Scope1,540 retains1,503 old
proofs:559 known extents137,619 uniqueB and981 unknown views102,126 sourceB.
All3,192 earlier complete regions remain unchanged; every3,229 normal named
region equals the frozen isolated object in full bytes, size and symbolic
relocations. Rich24-byte Suit metadata preserves30 real name/text pointer
relocations, WORDflags atc, DWORDcapability at10 and signed WORDid at14. The PC
magnet mask is200000; PlayerStart uses the actual16-byte record and pointer0.
The existing type= literal7576f8 is reused without duplicate storage.

Thirteen static selection tables remain in players.c: two5-byte and eleven
3-byte arrays (43 declared bytes,59 complete emitted bytes including alignment).
Their original allocation extents are unknown, so CSV sizes stay blank and types
reflect actual[5]/[3] declarations. Natural plain static AnimBlendModePc1 stays
in apiobject.c without a new section pragma or shared duplicate. Three actual
compiler f64 literals are each8 bytes/24 total known bytes. All17 private entries
and every actual normal emitted copy are independently initializer-exact.

Two known WORDs in resource966 and three known scalars in the graphics fallback
still require honest physical field ownership before aggregation. No exact-data
credit is retired, no aliases/backing are fabricated, and no scratch member-view
prototype has changed production matching rules.


### Effects/graphics and private music initializer release (2026-10-04)

Twenty-four genuine shared views add1,199 complete initializer bytes, all with
blank original owning extents and zero allocation credit. Scope1,564 retains
all1,540 prior proofs:559 known owners/137,619 unique original bytes and1,005
unknown views/103,325 source bytes. All3,229 earlier complete named regions retain
identical extents, bytes and relocations; all3,253 current normal regions equal
the isolated object. New definitions precede the unchanged baseline because a
rejected file-end placement expanded an older empty-key literal's complete COFF
padding from1 to4. No old source definitions are reordered or truncated.

The real null effect contains1,064 bytes (0x428), including typed FloatKey cohorts,
40000f and signed rotation limits. Initializer-only field recovery does not claim
an accepted PC file-stream reader. Scene-name storage is31 bytes including NUL;
its pointer is symbolic. Three device caches/counter views remain zero-initialized
with unknown allocation extents. The texture path reuses existing const55 owner.
Private music tables, keywords/token and NuExp10 state remain in actual original
TUs; all31 requested mappings and all emitted copies are independently checked.
Effects16B and graphics4B compiler literals retain separate real-owner proofs.

Every3,301 prior exact data owner/4,286 actual copies preserves full size, bytes
and symbolic relocations. The protected three fallback scalars12B and two resource
WORDs4B remain exact. Their canonical aggregate/member-view contract is research
only. Embedded fallback DDS8,320B remains an ignored asset research input, with
existing NuUnresolvedTextureIDPC=-1 and canonical55-byte filename reused; no
extracted asset payload or fabricated fallback is committed.


### Startup/lifecycle/native audio initializer release (2026-10-04)

Twenty-two shared physical owners add13,916 source-view bytes: startup13/13,788,
terrain lifecycle4/76 and native audio5/52. Each owning extent stays blank and
receives zero allocation credit. TerrPlatScanDist810b34 contains10f; prior
uncompiled inventory4B proved scalar access rather than owning allocation, so
its size becomes blank with no previous exact/source loss. Actual manager BSS
2972c68 view growsd98→da8 (3,480→3,496) under its native full-clear producer53,
preserving all3,480 prefix bytes plus a zero16-byte tail. It gains no owning extent
or allocation credit. Its existing retail+d14 reverb overlap remains a separate
explicit global-domain hold; no duplicate new owner/alias is created.

All3,252 other old complete shared regions remain identical in size/rawbytes/
symbolic relocations; all3,275 actual normal regions equal the isolated object.
Scope1,587 is1,564 old unchanged proofs +22 new physical definitions +one existing
manager proof added to scope. Known559/137,619 unique bytes stay unchanged;
unknown1,028/120,737 source bytes carry zero allocation credit. Source-private
TorpedoPackets2176 stays torpedo.c with native16-slot/address/stride evidence;
it is not duplicated in data.cpp. Every4,315 actual previous exact initializer
copy preserves full bytes/relocations, and3 additional literal copies match
retail. Current3,330 prior exact-owner rows/4,318 copies are all preserved/exact.
Protected resource WORD4B/fallback scalar12B stay unchanged. Shared queue types
have no constructor or implicit global initialization. Embedded DDS8320 and
optional member migration remain ignored research, without fabricated providers.


### Environment / startup support / action initializer release (2026-10-04)

Eight physical definitions add1,057 source-view bytes: special error tab3 and
type-name literal12 with known full extents; name counter4/name table1,024,
door pointer4/UID counter2, obstacle trigger counter4 and music action pointer4
with unknown owning extents. Genuine compiler1.75f and promoted1.01f64 literals
stay in their real source owners. ActionPairTab's prior unsupported inventory4B
is withdrawn to blank, while its real NULL pointer initializer4B remains. No
pointee count or allocation credit is invented.

All3,275 prior complete shared regions/raw bytes/symbolic relocations and1,587
scope proofs remain unchanged. All3,283 current normal regions equal isolated
VC8 compilation. Scope1,595 contains known561/137,634 unique initializer bytes
and unknown1,034/121,779 source bytes with zero owning-allocation credit. Every
4,318 prior actual exact copy is unchanged; eight additional literal copies are
fully retail-exact. Protected fallback12B and resource WORD4B remain exact.
Optional member tools are present but disabled. The fresh proposed48B resource
parent preserves all4 exact member bytes and retires two physical definitions
honestly; actual source migration remains next-wave work, with parent extent
blank and no48B credit. User journal edits are excluded from staging.


### Resource physical parent and startup working views (2026-10-04)

Active config/data-members.json describes 3 typed fields within the actual
_LevInstAnimResetPc 48-byte COFF parent at 0x87a830. MiscTime 0x87a850 remains unknown
extent; TargetDeg_Mid 0x87a854 and Far 0x87a858 retain complete 2-byte exact histories.
Fresh VC8 and LLVM 18 proofs check exact types/offsetof/sizeof; official full
parent relocation precedes member comparison. No member definition/binding,
byte mask, duplicated backing storage or 48-byte original-allocation claim.

Thirty-seven new shared views 462 B replace three retired fields 8 B; four existing
unknown views grow 103 B, net 557 B source storage and 34 physical definitions.
All 3,276 other complete regions and four old prefixes remain unchanged;
3,317 normal regions equal isolated compilation. Scope 1,631 physical proofs
retains 561 known extents/137,634 unique bytes and 1,070 unknown views/122,340 B.
Known typed members 2/4 B are separate from physical proof counts. Private item
pointer/count storage remains naturally owned by players.c with unknown extent.
Wav RTTI/vtable/literals retain natural C++ ownership; existing TypeDescriptor 24
is preserved, vtable owning extent remains unknown. Ten formerly uncompiled
unsupported 4-byte inventory extents become blank with no prior exact loss.


### Obstacle and Shove working views (2026-10-04)

Seven new shared physical views add a complete 59-byte NUL diagnostic and six
mutable working views totaling 84 bytes. All 3,317 previous full shared regions
preserve raw bytes and named relocations; current normal 3,324 equals isolated
compilation. Scope 1,638 contains 562 known regions/137,693 unique bytes and
1,076 unknown views/122,424 source bytes. All 4,334 previous actual exact physical
initializer copies survive, with nine new exact copies; two known resource
members totaling 4 bytes remain separate from physical definitions/counts.
Natural private owners retain the genuine eight-slot obstacle callback table,
32 bytes, and Shove queue/count view, 388 bytes. Their complete source initializers
match retail working bytes, without claiming original allocation extents.


### Decoder and player state working views (2026-10-04)

Fifteen new shared views add 4,128 source bytes; all owning extents remain unknown.
Actual Huffman tree indices and scalar/pointer/callback readers qualify these
working views without allocation credit. All 3,324 prior complete regions/raw/
named relocations and 1,638 scoped proofs remain unchanged. Current normal 3,339
equals isolated compilation; scope 1,653 retains 562 known regions/137,693 unique
bytes and 1,091 unknown views/126,552 source bytes. All 4,343 old exact physical
initializer copies survive; nine new actual copies produce 4,352, separately from
two unchanged exact resource members totaling 4 bytes. Unsupported uncompiled
_gExplodeInitialised and _GAMEPAD_TOGGLERIGHT 4-byte inventory extents become blank;
actual DWORD state views remain genuine. Private decoder, Ogg and FFT initializers
stay in their original owners. No DAT decode-buffer storage or capacity is guessed.


### Vorbis and gameplay initializer preservation (2026-10-04)

One genuine shared NUVEC unit scale adds twelve unknown-extent source bytes.
All 3,339 old complete shared regions/1,653 scopes survive; current 3,340 regions
equal isolated compilation. Scope 1,654 retains 562 known/137,693 unique bytes
and 1,092 unknown/126,564 working bytes. All 4,352 old actual exact physical copies
survive; 83 new known exact copies yield 4,435, plus two exact members/four bytes.
192 new complete initializer copies match retail: codec179, vector1, gameobjects1,
blowups10 and ZipUp1. 109 unknown-extent copies receive no allocation/exact credit;
4,544 is the verified-copy union, not the exact-copy count. Five complete loader
NUL literals add 57 exact union bytes. Private codec storage stays in original
owners. Unbound unused _seq receives no invented PC identity or source credit.


The native audio/terrain checkpoint adds eight genuine shared views/53 bytes,
all unknown original owning extents: listener IID, sample algorithm GUID, hit
multiplier BYTE, sample-load enabled/limit, stream serial, Ogg heap-cache pointer
and critical-section index. Actual allocation is five 0x114-byte heap records;
no static cache backing is invented. Full 3,348 normal regions equal isolated
output; 1,662 scopes preserve all 1,654 old proofs. Known 562/137,693 unchanged;
unknown 1,100/126,617 are source views only. All 4,435 old exact physical copies and
two exact members/four bytes survive; 8 new known exact copies yield 4,443. All 32
new complete initializer copies are retail-equal;24 unknown-extent copies get
zero exact/extent credit. Verified-copy union 4,467 is not exact coverage.


ZIP/arena/message checkpoint: 26 genuine shared definitions add 632 view bytes,
including three known NUL owners (32 bytes) and 23 unknown views (600 bytes).
The 64-u64 grid at 0x956070 is a verified 512-byte minimum view, without original
allocation credit. Private ZIP/default tables and MessageData's native 512-record,
0x4b4-stride zero pool remain in their original source owners. The latter emits
616,448 complete zero initializer bytes; its unknown original extent supplies no
allocation/coverage credit. All 3,374 normal shared regions equal isolated output;
1,688 scopes retain every old proof. Known scope is 565/137,725; unknown source
views 1,123/127,217. All 4,443 old exact copies and two exact members/four bytes
survive. 38 new known exact copies yield 4,481. All 66 new complete initializer
copies match retail; 28 unknown views receive no exact credit. The 4,509 verified
copy union is not coverage. Exact data union increases by 59 to 630,991 bytes.


Bolt/camera/portal checkpoint adds 81 genuine shared owners/2,740 source-view
bytes: 38 known NUL definitions/436 bytes and 43 unknown views/2,304 bytes.
All 3,374 prior regions and 1,688 scopes survive; 3,455 normal regions equal
isolated output across 1,769 proofs. Known scope is 603/138,161 bytes; unknown
working scope is 1,166/129,521. The same 65,538-byte ConfigBuffer owner exposes
its native Y rotation WORD as a named member at 0x10000, asserted and verified
without a second COFF definition or extent credit. All three old members remain
unchanged; two exact fields retain four bytes. All 4,481 old exact physical copies
survive; 79 new known exact copies
yield 4560. All 122 newly emitted
complete initializer copies match retail; 43 unknown views receive zero owning
extent/allocation credit. Exact data union rises 452 bytes to 631,443.

The inherited sceneParameters[32] working view overlaps canonical zx, texture
count, shadow state and fstack_limit. Native stride 0x218 and its zero initializer
are established; original owning capacity is unknown. A proposed grown union is
held, with no live migration or new backing aliases. Selected portal/render CPU
fixtures exclude scene-stack alias writes. Private compiler and message owners
remain genuine source definitions; initializer equality is not full game proof.


Gameplay consumer checkpoint keeps the shared owner source unchanged: all 3,455
complete regions and 1,769 scoped initializer proofs survive, with four typed
members unchanged (two exact/four bytes). Corrected baseline per-object exact
count is 4,561: TerrainUnusedSpatialBlockPc in data.cpp is exact even though its
aggregate row includes a nonexact owner. The previous row-level 4,560 count
missed that four-byte copy. 4,557 old physical copies remain unchanged and four
duplicate compiler constants naturally leave bolts.c during the accepted C++
parent migration; their full native bits survive in genuine owners. Forty-three
new known complete initializer copies/398 emitted copy bytes yield 4,600 actual
exact copies. Exact data union gains 277 bytes to 631,720; duplicate copy counts
are not coverage. No shared backing, inferred original extent or allocation
credit is added. Next scene width/output views reuse record16 fields of the
existing 0x218-stride scene owner without growing its working allocation.


Scene/configuration checkpoint verifies all3,483 complete shared regions and
1,797 scoped proofs against actual normal output. All3,455 old regions/1,769
scopes and the inherited CRC16 constructor remain unchanged. The28 added views
are547 bytes with unknown original extents and zero allocation credit. Existing
sceneParameters32/17,152B/stride0x218 retains its complete owner; named width and
output views use record16 without a second backing definition. All4,600 old
exact physical copies and all four typed members/two exact/four bytes survive.
All109 new complete initializer comparisons pass:69 known676B and40 unknown859B,
including28 shared547B plus12 original private/unknown literal312B. Current
exact physical copies4,669; unique exact data union632,255 adds535B. Unknown
initializer equality does not establish original allocation or exact coverage.
Next data proposals are finite wider Parts and interaction views; Image/cache
at0x966298 remains held for alias/capacity evidence.


Particle/interaction/Grid/Teleports checkpoint retains all3,483 prior complete
shared regions and1,797 scopes; actual normal3,493/1,807 equal isolated output.
Ten unknown views add58 working bytes without original allocation credit.
Known603/138,161B remains unchanged; unknown1,204/130,126B denotes source views.
All4,669 old exact physical copies and four typed members survive; two members
remain exact/four bytes. All64 new initializer copies equal retail:43 known300B
yield4,712 actual exact copies;21 unknown206B receive zero extent/exact credit.
The genuine12-byte checkantinodefns table remains in aisys.c with blank original
extent; complete source initializer equality does not infer its owning allocation.
Unique exact data union632,384 adds129B. Private Teleports/Grid/Log owners and
inherited CRC16 initializer remain genuine. No viewport/image backing is added.


### Ray, particle and Image storage release (2026-10-04)

Seven genuine shared views add 74 verified working bytes: viewport prefix48
at80e228, Part platform list8 at292097c, collision interval4 at8102dc,
stop-speed squared4 at8102e0, UGNAUGHT WORD2 at7f1a30, hysteresis4 at7f67d0
and visible-baddies count4 at876860. Original owning extents remain blank;
none receives allocation/exact credit. Prefix48 uses the verified native
0,0,640,224 / 0,0,640,224 / 1,1,0,0 initializer and reuses existing
PCSpecialProjectionAdjust16 at80e258. The cache8 union retains existing
sceneParameters[16] fields at8/c, the 0x218 record and17,152-byte owner;
selected cols2..8 imply no original cache-capacity claim.

The original parts TU owns impact normal12 and enabled flag4; the original
nurndr TU owns seven Image static locals. All 3,500 normal full data regions
and 1,814 scoped proofs equal isolated output, preserving all 3,493 old
regions/1,807 scopes and the CRC initializer. All 4,712 previous exact physical
copies and four members survive. All 37 new initializer copies are retail
equal:21 known copies give4,733 exact physical copies;16 unknown copies/118
working bytes remain uncredited. Matched data union stays632,384 bytes.
Known scope603/138,161 bytes and unknown scope1,211/130,200 bytes are distinct
from physical-copy counts and original owning allocation claims. The final
receipt is build/analysis/transfer-execution-20261003/data/
ray-image-world7-production-release.json, tied to the frozen314 inputs and
274 actual normal objects. Future resource globals remain scratch evidence.


### Resource, gameplay and graphics storage release (2026-10-04)

35 real shared views add192 verified working bytes: resource4/16, Trigger4/16,
GameAI20/78 and graphics7/82. Grapple4 has an existing established extent;
34 other shared views/188 bytes keep unknown owning extents blank. Real lever,
spinner/context and particle/editor slots are source backed; private RNG,
script mask/state,21-command table and label/offset views stay with original
owners. The18 command-table strings are source literals, not asset bytes.
All90 new complete physical initializer copies equal retail.42 known copies
raise exact physical copies4,733->4,775;48 unknown copies/7,380 working bytes
receive zero extent/allocation/exact credit. Data matched union632,532 adds148
bytes across proven owning extents, distinct from shared working-view sizes.

Final normal proof: build/analysis/transfer-execution-20261003/data/
resources-trigger-gameai-graphics35-final-normal-release.json. Normal3,535
full regions/1,849 scopes equal isolated output and preserve all prior shared
regions, old exact copies, four complete typed members/parents and CRC initializer.
Two exact WORD members remain4 bytes. Known scope604/138,165 and unknown
scope1,245/130,388 are factual working views, not invented original heap capacities.
No backing aliases, fake data providers or game asset bytes are introduced.


### Lighting/list and GizFlow data release (2026-10-04)

Exactly three new shared i32 views/12 bytes: version5 at80fde8 and timer/random
IDs -1 at8061b8/806144. Original owning extents remain unknown/blank; all12
working bytes get zero allocation or exact credit. Shared3,538 full regions/
1,852 scopes equal isolated output, preserving every old region/initializer,
four typed members/complete parents and inherited CRC10B plus relocations.
Known scope604/138,165 bytes is unchanged; unknown1,248/130,400 is working-view
evidence. Two exact WORD members remain4 bytes.

Final normal receipt: build/analysis/transfer-execution-20261003/data/
lighting-gizflow3-final-normal-release.json. All75 actual new complete copies
are retail-equal: ten known copies give4,785 exact physical copies from4,775;
65 unknown copies/1,045 working bytes give no extent/allocation/exact credit.
Matched union632,544 adds12 proven numeric bytes, separate from shared views.
Genuine67 GizFlow private initializers and five RTL compiler copies retain
original source owners; no duplicate tables/state or new CRC constructor.
All326 protected production inputs and282 actual normal object pins survive.


### Camera/collision two-view release (2026-10-04)

Two genuine shared working views add20 bytes: CameraNearClipPlanePc NUVEC4
at964080 and CollisionVehicleExcludedLevelPc LEVELDATA_PC* at87b104. Both
complete emitted initializers equal retail; original owning extents remain
unknown with zero allocation/exact credit. All3,538 prior shared regions and
1,852 scopes survive; current3,540/1,854 equals isolated proof. All4,785 prior
exact physical copies and four complete named members/parents survive;21 new
known exact copies raise the total4,806. All29 new initializer copies equal
retail, including8 unknown/76 working bytes. Known604/138,165 scoped bytes are
unchanged; unknown1,250/130,420 are working views, not capacities. Private force
constants and specular state stay with original owners; CRC is preserved.
Camera saved-bank/count views remain scratch-only for the next wave.


### Surface and saved-camera eleven-view release (2026-10-04)

11 genuine shared definitions add16,946 working bytes: complete named readonly
NUL literal10 and ten unknown views16,936. Saved count4 and bank16,896 are true
original nucamera_gen public owners; bank is16 records of0x420, one physical
view. Native16 guard/stride proves the working contract; owning extent remains
blank with zero allocation/exact credit. Surface eight mutable working views
use36 bytes; private radius4/PodRaceKey32, camera inverse64 and two RTL12+4
owners remain in their original source contexts. No duplicate state/backing.

All3,540 prior full shared regions and1,854 scopes survive; normal3,551/1,865
matches isolated proof. All4,806 prior physical exact copies and four complete
members/parents survive;24 new known exact copies yield4,830. All44 new complete
initializer copies match retail;20 unknown copies/17,078 working bytes receive
zero original extent/allocation/exact credit. Data union grows10 to632,566.
Known scope605/138,175 and unknown1,260/147,356 working bytes describe verified
views; exact WORD members remain4 bytes and complete CRC is unchanged.


### Seven frame/camera shared views (2026-10-04)

Seven genuine shared definitions add173 working bytes: six24-byte PlayPlane
records, sides-only flag4, Gunship normal12 and four frame/material scalars13.
Owning extents remain blank with zero original allocation/exact credit. Original
private pad arena/ramp/state and renderer animation globals stay in their source
owners; no duplicated platform storage. Shared3551->3558 regions and1865->1872
scope counts are historical aggregate figures: the deleted scope-identity packets
are not recreated or claimed preserved. Fresh complete equality of every prior
physical region instead preserves initializer contracts, four typed members/parents
and CRC. All4830 previous exact physical copies survive;13 new known copies
yield4843. Physical known copies135B and data union67B are different overlap
measures. Unknown views do not receive known-extent or capacity credit.


### Background worker HANDLE (2026-10-04)

The sole writer adds bgproc_thread_handle_pc at921254: native CreateThread return
store and ResumeThread reload establish the4-byte reader/writer view and zero
retail initializer. Owning extent remains unknown/blank with zero allocation or
exact credit. All3,558 previous shared fullregions/CRC,4,843 known exact physical
copies and four typed members remain unchanged. Current3559 shared regions add
only this view; no padding or other ownership migration occurs in this checkpoint.

### Shared data and original owners in the connected runtime wave (2026-10-04)

DAT read_buffer, read_buffer_size, read_buffer_decoded_size and decode_buffer_left
now belong to their true private `nudatfile.c` owner with decode_buffer and
decode_buffer_pos. Both minimum buffers are262144 bytes, which covers the
observed decoded/packed maxima262144/255630 across all seven installed archives
(7468 compressed members/43148 blocks). These bounds establish the tested
installed-data domain, not original allocation sizes or malformed-input safety;
buffer extents remain blank in the map. No asset bytes are retained as evidence.

The keyboard count and display aspect are actual named owner members, preserving
their complete historical known4-byte comparisons without second backing. The
keyboard working view is60 bytes, with key_count+2c/factory+30/mouse axes+34 and
buttons+38. Its producer remains under reconstruction. PCDeviceBackend moves
from shared data into the genuine C++ owner and constructor lifecycle; aspect is
its+630 member. Four existing member comparisons and CRC storage are preserved.
Three render-texture views and three pickup views use proven native initializers
with unknown allocation extents. TempOptions remains OPTIONSSAVE13; its only
change is zero COFF tail padding20→16, not typed bytes or matching credit.


### Native startup and customiser data at checkpoint R (2026-10-05)

The sole shared writer adds81 real views with1,838 typed working bytes and1,847
complete COFF bytes. Their original owning extents remain unknown and receive
zero allocation/exact credit. Two inherited padding intervals change naturally:
known WORDffff keeps its2 typed bytes, and the unknown empty string keeps its
one NUL. All previous5,241 exact physical copies and13 members/28 known bytes
survive. Across all owners,149 of150 new complete initializer copies match
retail; the real CPU SEH scope28 remains nonexact. Sixty-three known new copies
increase the exact data union1,607 bytes to639,477;87 unknown copies get no credit.

The callback4-byte working prefix is qualified for the verified native
zero-registration launch domain only. The argv8-byte prefix covers empty or
recognized-only arguments only, not arbitrary unknown tokens. Native algorithms
are retained without artificial capacity guards. Windowed mode uses the existing
device parameters+0x158, dimensions the existing desktop mode+0x608/+0x60c, and
aspect existing nuapi+0x10; no duplicate scalar backing is introduced.

### Connected renderer and resource data at checkpoint S (2026-10-05)

All nine renderer states stay private in their original PC stream/state owners:
156 minimum working bytes, original complete reader ranges and native initializers.
There is no parallel shared backing or retention control. The sole shared writer
adds149 genuine views: five resource views604 typed bytes and144 named NUL-string
pointees1,479 bytes. debris_name has the native147 entries, reusing the two existing
PUNCH names; Android's400-entry capacity is not imported. All2,300 complete COFF
bytes match retail. Owning extents stay unknown, with zero allocation/exact credit.
The gradient material uses the existing DebMat[5] slot.

All5,304 old exact physical initializer contracts and13 named members/28 known
bytes survive. Across all owners197 new complete initializer copies are retail
equal:38 known copies add366 unique exact data bytes to639,843;159 unknown copies
receive no credit. Shared storage grows5,023 to5,172 physical regions. Only the
inherited unknown DebrisDmaCleanupDelayPc zero tail changes naturally8 to4; its
f32 value is unchanged. CRC, HANDLE and full member parents remain preserved.

Next AI and renderer state proposals require original-owner and complete-reference
qualification before release. Minimum accessed views do not prove original
allocations, general capacities or game boot; the sole writer retains ownership.

### Constructor input-text extent qualification (2026-10-07)

The constructor-data recipe introduced in af1f551 assigned known sizes to 1,158 PcInputText rows by reading each native string through its first NUL (prepare.py lines 23–35 and 62–63). This establishes 14,983 initialized text bytes, but the retained evidence does not establish the original owning extents or private visibility. We therefore qualify only these rows to unknown size, preserving their addresses, char[N] working types, source values and complete compiler-region comparisons. The three names replaced by existing canonical compiler literals before integration and 146 prior canonical pointee reuses are excluded; all unrelated known-size contracts remain intact. This explicitly withdraws 3,319 bytes from the exact-data physical union and 14,983 bytes from the mapped union, with no change to compared code, fuzzy/exact code progress or transferred source credit. Historical proofs and reports remain retained. The enumerated recipe receipt is 43d4f9528799b2ef42eac5a137d114f789198237a6a314bccbd985089587f6a0; accounting against immutable X report f2d38a8fa9b982b5409133c497a7f5d96092b2dbfa34d4f5ca4e2784e032e98f is pinned by receipt 9615b77a95fd0be262e096c199d34b9b24bc8b247d88b1024899388cf47ac88b.

### Canonical Area/FreePlay/accessory storage Z (2026-10-07)

The sole writer added48 shared definitions (914 typed/940 complete COFF bytes),
reusing8 canonical definitions including the existing default-character pair.
PlayerIDs16->18 and Story6->196 are same-owner minimum reader refinements
(9IDs/49 model records), not proven original allocation capacities. Two player
consumers now read named model_id fields at the same native offsets0/4.

All112 newly emitted complete initializer copies match retail:16 known-extent
exact copies and96 unknown-extent copies. Unknown extents receive no exact or
allocation credit. All9,621 old full initializers were checked;5,258 old known
exact compiled copies and13 complete member parents/28known bytes survive.
Preciselyfive old WORD tails change naturally, in addition to the two growths:
ID_DEFAULTCHARACTER_A4->2,LSW_B4->8,TextCrawlRotateXYZ2->4, with typed values
unchanged. Only the previously nonexact known2-byte default WORD now adds2
unique exact data bytes; union637,543->637,545. No source transfer credit arises
from that old tail or from the raw-manifest correction. No angle placement,
padding or further legacy-extent qualification is used; Perm remains held.

### Shader and terrain initializer copies AA (2026-10-07)

All217 newly emitted complete initializer copies/2,797 COFF bytes are native
equal:3 known-extent copies/12bytes and214 unknown-extent copies/2,785bytes.
Unknown owning extents receive no allocation or exact-data credit. All9,733
old full regions and5,275 old known-exact compiled copies survive unchanged.
Known exact physical data/member union remains637,545; the new known copies
reuse existing physical ranges. All13 named member parents/28known bytes,
CRC/HANDLE and the consistently unknown legacy string cohort remain preserved.

No new shared data.cpp storage was added. Ordinary static PickupTerrainPc stays
in its genuine nuterrain.c owner: nativezero4/provedpublication, owning extent
unknown. Parser/compiler literals use complete text and canonical bindings;
source initializer equality does not establish original allocation capacity.

### Terrain and cutscene initializer ownership BB (2026-10-07)

Twelve new full initializer copies total 97 COFF bytes and equal native bytes:
three known-extent copies/20 bytes and nine unknown-extent copies/77 bytes.
Known copies reuse existing physical ranges; unknown extents receive no exact
data or allocation-capacity credit. All 9,950 old full regions and 5,278 old
known-exact compiled copies survive; the physical union remains 637,545 bytes.
All 13 named members/28 bytes and CRC/HANDLE parents are preserved.

First matching correctly rejected a changed _ang candidate extent after three
DWORD1 render controls were appended to central data. The controls now belong
to the existing cutscene source with their proved InstanceRender caller. External
C reconstruction linkage is unchanged; original historic filename/visibility
and owning extents remain unknown. The genuinely shared NULL callback remains
in data.cpp. No angle-table migration, padding, segment, alignment, ordering or
comparison rule was changed. Full _ang extent1026 and native equality are restored.
The terrain private stack pointer has its proved zero initializer; backing
capacity and native unchecked arena domains are not inferred from it.

### Terrain skin and cutscene initializer ownership CC (2026-10-07)

Thirty-one new complete initializer copies / 210 COFF bytes equal native bytes:
10 known-extent copies / 64 bytes and 21 unknown-extent copies / 146 bytes.
Known copies reuse existing physical ranges; unknown extents receive no exact
coverage or allocation-capacity credit. All 5,281 previous known-exact physical
data copies, 13 members / 28 bytes and CRC/HANDLE parents survive. Physical exact
data union stays 637,545 bytes. Of 9,962 previous complete regions, 9,961 are
unchanged. The unknown-extent TextCrawlRotateYPc keeps its u16 type and native
zero two-byte initializer; natural compiled BSS tail changes four to two bytes
after the genuine shared append. No known extent or exact match is retired.

The sole data writer appended 11 genuine shared cutscene definitions / 106 bytes.
The DWORD1 camera-update control belongs with its proved cutscene InstanceUpdate
consumer cohort, retaining external C reconstruction linkage; historic filename
and visibility remain unknown. Five original cutscene statics / 20 bytes stay
private. Three original terrain skin statics / 12 bytes have native zero, NULL
and minus-one initializers, with owning extents and capacity unknown. Full known
_ang extent 1,026 remains exact. No padding, ordering, segment, alignment or
comparison changes were used to repair an extent.

### Shader/font initializer ownership DD (2026-10-07)

All46 new complete initializer copies /724 COFF bytes equal native initializers:
4 known-extent copies70 bytes,42 unknown-extent copies654 bytes. Physical exact
data union increases63 bytes to637,608; overlaps count once. Unknown extents
receive no exact coverage or capacity credit. All9,993 previous complete regions,
5,291 previous known-exact physical copies and13 member parents/28 bytes survive.
Natural RTTI/vtables, font literals and real private initializers retain their
source ownership. Solewriter appended six shared font working views24 bytes:
four i32 controls and two genuine font pointers with native zero initializers.
Their owning extents remain unknown; known _ang1026 remains exact.

Five actual tools/*.def inputs are now included in source fingerprints. The
real D3DXCompileShader@40 export regenerates its import library through existing
Make dependencies. Final431 captured inputs and six actual artifacts (source
EXE plus five generated import libraries) are pinned. Historical426-input
baselines remain historical; no import DLL implementation is fabricated.

### Geometry/music initializer ownership EE (2026-10-07)

Eight new full native-equal copies52 bytes: 8 known
52 bytes and 0 unknown
0 bytes. Physical exact union remains637,608;
unknown extents receive zero coverage/capacity credit. All10,039 old full data
regions and5,295 known-exact physical copies,13 members/28 bytes survive.
No new shared storage. Voice DWORDflags30 shares existing BYTEalias/size34.

Terrain inherited COMMON mismatch remains unchanged; eight new initializers are native-equal. Full known angle1026 remains exact. No allocation or new data capacity is inferred from a compiled view.

### Connected AISYS, tagging, hardware locale and cutscene checkpoint FF (2026-10-07)

Transferred 45 full functions and 13,221 new unique PC bytes: AISYS 4,370,
tag/takeover suppliers and callbacks 1,219, device modes/locale 2,388,
streaming 1,092 and cutscene controller 4,152. Unique compared code is
1,304,919 bytes; objdiff denominator 1,305,490 includes 571 overlapping bytes.
Whole-PC fuzzy coverage is 26.029687144670714%, +0.297057341703816 percentage
points. Exact code is 209,128/3,471,246 bytes (6.024580222778795%), +844 bytes.
These measurements do not establish running gameplay.

All 4,426 prior complete function copies and 10,047 complete data regions were
checked. All 2,105 exact code copies, 5,303 known-exact physical data copies,
13 named members/28 bytes survive. The unchanged-source camera body reproduces
the previously accepted scheduling pair exactly reversed, with the same limited
finite initialized masked-FP domain; only the genuine locale call changes the
other previously nonexact hardware parent. No old data exception or comparison
mask, truncation, extent retirement, flag/order/alignment control was used.

Required build/link/object checks pass for 314 owners/4,471 functions/10,096
data definitions. LLVM18 checks pass: 728 tests, three existing skips. Retail
matching and actual-normal CPU checks pass: AISYS 69, tag/takeover 50, device
modes 162 and cutscene 114 (395 total), with no replacement/supplement providers.
All 432 captured inputs and six generated import-library/EXE artifacts remain
unchanged. Authentic source EXE still exits 5 after 3.642s at NULL Retrieve+16,
full 208-byte body, source 0x50b960. Own-prefix crash dialogs are disabled;
private cleanup leaves no owned game/debugger/crash window. Persistent window,
device/Present/frames/scene/gameplay remain unverified. Controller families
retain explicit real missing audio/fog/panel/scene actions.

The new Mac executable has 30,815 dyld exports and 22,127 combined named code
addresses, including real game functions, shader constructors and parser methods.
It has no embedded DWARF/STABS. Runtime type/name evidence corroborates selected
PC hypotheses, not PC addresses, general layouts or MSVC generated helpers.
Full retained audit: build/analysis/mac2-evidence/README.md. Next connected work
uses these names for shader publication/parser closure and LoadPerm2/character
fixups. Whole-project goal remains active; direct-main pushes are authorized,
with CI verified on each exact pushed revision. Human journal remains excluded.

Forty-nine new complete initializer copies total 372 retail-equal bytes: five
known-extent and 44 unknown-extent copies. Physical exact data union remains
637,608 bytes; unknown extents receive zero coverage/capacity credit. Canonical
TakeOverObject remains the same 352-byte allocation, now typed as eight asserted
44-byte records. Eleven genuine central zero states add 44 working bytes; music
frame sentinel -1 is an ordinary external-C four-byte definition in the existing
controller, qualified by its native producer/consumer lifecycle (original PC
filename/visibility/extent unproved). Native readonly registry names stay real
immutable source literals. All prior angle1026/ripple10/member/CRC/HANDLE proofs
survive. The failed central ten-state and readonly-string variants are retained;
no storage placement or alignment variant was selected to manufacture equality.

### Character loading, default movement and angle checkpoint GG (2026-10-08)

All 38 new complete initializer copies equal retail: two known copies/12 bytes
and 36 unknown-extent copies/348 bytes. Exact physical data union stays 637,608;
unknown extents receive zero allocation/exact credit. The single data writer adds
21 genuine central working definitions/186 typed bytes/204 compiled bytes,
reusing existing IDs and the canonical nopakfile owner. Mac named mutable spider
controls corroborate -2/-0.4/1 initializers; PC consumers independently establish
their identities. Mac exports do not establish PC addresses or allocation sizes.

The complete unchanged external-C mutable u16 ang[513] table moves from data.cpp
to the existing nuang.c math owner. Known native 1,026-byte extent, all 513 values,
symbol linkage and SHA 35e5094c1387d087543073af7c92e184affc1f354be19a64f04788956d371d60
are preserved with exactly one definition. Original PC filename/private visibility
remain unproved. This genuine owner correction adds no coverage; it is not an
extent retirement, alignment/order control or added translation unit.

### Character callbacks, full Jump and save/menu checkpoint HH (2026-10-08)

The125 shared definitions add429 typed working bytes and476 full compiled bytes.
All125 logical initializers are native-equal;124 complete central intervals match.
The new MENUWARNR full2ff00 versus nativeffff remains explicitly nonexact/unknown;
its real native writer5096c3 and reader510375 access one BYTE, and5096e9 writes the
separate following MENUFLASH0G BYTE. Neither logical equality nor minimum width
establishes original allocation extent. Total new complete intervals172/817B include
33 known201B and139 unknown616B; only138 unknown full intervals equal retail.
Known physical exact union637,608 remains unchanged, including ang[513]/1,026 bytes
in the unchanged existing math owner, all named member parents, CRC and HANDLE.

Eight existing unknown WORDs retain their declarations, initial values and native
WORD producers/readers; natural COFF intervals4 become2. They are AreaExtraTargetId7f17ccPc,
AreaExtraSourceId7f17c8Pc, AreaExtraTargetId7f17bcPc, FreePlayId007f199cPc,
TextCrawlRotateZPc, PermanentAllTitleIdPc, PermanentJediTitleIdPc and
PermanentBlasterTitleIdPc. Their full old/new evidence is retained separately;
no known size, old exact match or typed value is retired. True CharFixUp and
Text_InitTable producers, direct accesses and all retained compiled base/addends
are independently audited. No positive-ID assumption or storage variant is used.

Menu save/load flags reuse MenuKeyboardAlwaysPc921d68 and SecondaryPc921d6c.
The six-DWORD saveload_slotused working view is producer-bounded, not an original
array-capacity claim. Jump's24 registered action/context values have true native
initialization sites; the chain timer9505e8 and Slam speed802da0 have real f32
readers/initial values with no writer in the retained audited prefix, explicitly
qualified. Final solewriter source and complete data proof are pinned in
checkpoint-HH/data-integration.json and HH-storage-independent-next.

### Connected motion, graphics lifetime, credits and script loading II (2026-10-08)

The26 central views add76 typed/102 full compiled bytes, all full native-equal.
Every old10,306 complete data interval remains identical, including old exact
copies, ANG, CRC, HANDLE, ripple and13 member contracts/28 bytes. New owning C++
surface/framebuffer9 states retain6325 typed working bytes, unknown original extents;
their ten complete state/CRT records6332 bytes are native-equal. Real generated
constructor58, teardown37 and original constructor iterator50 provide automatic
32-by-five member lifetime. Only the genuine CRT initializer pointer has a known
four-byte extent and adds4 exact-data union bytes. No original pool capacity claim
is inferred from the remaining unknown mapped working shapes.

All112 new initializer copies7386 bytes remain accounted:53 known321 and59 unknown7065.
Only CreditsDurationPc is full nonexact: typed DWORD120 is exact, emitted natural
eight bytes differ in the next native style-pointer region. Five native readers use
DWORD and the compiled references use addend0. Its size stays blank and its full
nonmatch remains visible, with zero allocation/exact-data credit; no layout control.
The scripts8 literal is proved at true LoadPerm1 native75a030 argument and matches in
full, but its original owning extent remains unknown and adds no exact-data credit.
Private credit record/table/counters and seven script load/constants/command objects
remain their actual source owners. All existing CSV rows/known extents survive.

Next combined writer preflight is Push and level callbacks. Geometry pointer/origin
storage still needs genuine producer/capacity contracts. Four save-path arrays remain
held: a191-byte proposal fails a valid native shell-root135 case, so no artificial
shell-root cap or guessed allocation is activated. Solewriter must receive a new
bounded compile/write grant before further actual definitions.


## Connected motion/callback/geometry working views JJ (2026-10-08)

The sole central writer appended 20 native initializer views: pushing 11/34
typed bytes, world/title state 6/84 and geometry state 3/24. They emit 154
complete COFF bytes from 142 typed bytes, all equal to the corresponding native
storage. The original owning extents remain blank; this is no allocation,
capacity or exact-data coverage claim. Earlier definitions and their exact
contracts are preserved. The final unknown `LEGOACT_GLIDE` region naturally
grows from its two-byte -1 word to `ffff0000` when the next four-byte-aligned
view is emitted; both complete regions equal their native bytes, and the word
and original unknown extent are unchanged.

World's six ordinary mutable scalar defaults remain with their callback owner;
its original private picture count, start position and music table remain local.
Geometry provides the actual zero `NUVEC4` origin (16 bytes) and two null pointer
slots (4 bytes each), without fake arrays, arena capacity or producer activation.
The named field assertions stay in the geometry reader owner. Real arena/basis
and origin update publishers are the next connected dependencies.

Across the wave, all 117 new complete initializer copies are native equal:
63 already-known extent copies/386 bytes and 54 unknown extent copies/641 bytes.
Aliases are counted once; the known exact data union remains 637,612 bytes.

## Geometry producer working states KK (2026-10-08)

Thirteen central views add 544 typed/548 complete COFF bytes, all native-equal.
The default 16-byte origin naturally emits 20 bytes including four native zero
tail bytes. Each original owning extent remains unknown; full initializer proof
does not add allocation, capacity or exact-data credit.

The existing canonical `PCDeviceSamplerInvalidation` at `0x0097f9d8` is refined
from `i32[8][2]` to `i32[16][2]`. The genuine reset loops through 16 pairs to
`0x0097fa58`; the device stage readers continue using their original eight pairs.
The full 128 zero bytes equal native storage and retain the original 64-byte
prefix, symbol, segment and linkage. This remains an unknown-extent working view,
with no duplicate cache provider. All other 5,805 prior central-owner regions
stay identical; all 2,150 prior known exact central data copies remain preserved.
Whole-build coverage includes all 13 member contracts/28 bytes and all earlier
known exact data. The 45 new complete initializer copies total 797 bytes, all
native-equal: 21 known/119 bytes and 24 unknown/678 bytes. Their known ranges are
already covered aliases, so the exact-data union remains 637,612 bytes.

### Connected-family central storage snapshot LL (2026-10-08)

The sole writer added 31 genuine central working definitions/138 typed bytes,
with two natural read-only literals making 33 complete regions/161 bytes. These
supply lever/streaming, mission-menu, hat ID and push state. Existing Game storage
contains the canonical mission-save fields; there is no duplicate save backing.
Function-local registration descriptors remain private to their source owners.
The previous data source prefix and existing known-exact initializers survive.

Two unknown-extent full COFF regions changed naturally: PushBlockFacingTolerancePc
WORD910 retains its typed two bytes while a new following DWORD gives a two-byte
zero tail (2 to4 complete bytes); PcGeometryDefaultOriginPc retains all16 typed
zero bytes while the next genuine object removes four prior zero tail bytes
(20 to16). Specific full-byte/relocation/source/native-reader proofs accept these
pairs. Neither original owning extent is inferred and neither gains exact credit.
All 166 new complete initializer copies/1,645 bytes across the wave are retail-equal;
95 known copies/606 bytes and 71 unknown copies/1039 bytes are recorded separately.
Known exact-data range union stays637612 bytes. Full preserved failure and acceptance
pins remain in transfer-checkpoint.json; no generic unknown-tail exception applies.

### Registration and character working states MM (2026-10-08)

Eighteen genuine central views add 60 typed bytes. Three natural read-only
literals add 20 bytes; seven new WORD definitions each carry two native zero
tail bytes, giving 21 complete retail-equal regions/94 bytes. Existing canonical
obstacle and Tube strings are reused by mutable pointer slots with const
pointees. WORD character IDs, hover values and POD timing defaults reproduce
native initializers; valid POD indices are only 0/1. All owning extents remain
unknown and receive zero allocation or exact-data credit. No old data region
changed, including LL's specifically reviewed natural tails.

Across the normal build, 61 new complete initializer copies/950 bytes equal
retail: 13 known copies/48 bytes and 48 unknown copies/902 bytes. The known ranges
were already covered, so the exact-data union remains 637,612 bytes. All 5,575
prior known-exact copies and 13 named members/28 bytes survive. Private 120-byte
registration descriptors stay with their genuine source owners. GCDATA_DEFAULT
and the true startup CharFixUp table remain separate future contracts.

### Techno, Portal, Shadow and character installer source NN (2026-10-08)

The current source adds 47 complete functions/5,816 unique PC bytes: Techno
20/2,467, Portal 15/978, Shadow 10/1,019 and character installer parents 2/1,352.
Five existing owners retain their exact prior prefixes; edgizshadowmachine.c is
one new owner. Every prior complete function output and exact contract survives.
The one changed prior data region is the explicitly reviewed unknown-sized
BATTLEDROID WORD tail: full COFF 2→8 remains nonexact and zero-credit; no tail
is used as the distinct native Commander state.

Required checks pass: 732 tests (three existing skips), LLVM18 format for 360 files and tidy for 333 units,
source build/link/object coverage and full retail matching. CPU 183/183 passes
against the actual normal objects, with genuine sentinel preconditions and no
ID assignments, provider replacements or fixture corrections. Crash dialogs
were disabled and queried; no owned process remained. Whole-PC fuzzy code is
28.574010%, +0.150060 percentage points; exact code is 225,541 bytes (6.497408%),
+1,709. Known exact data remains 637,612 bytes. All 56 new complete initializer
copies/1,089 bytes equal native; unknown owning extents receive zero credit.

GCDATA_DEFAULT supplies its genuine minimum 288-byte record/91 initialized fields
and real model-layer callback. Techno/Portal/Shadow canonical state and private
registration owners retain verified offsets and defaults. FixUpCharacters (178 bytes)
is exact; ExtraCharacterFixUpAfterConfig (1,174 bytes) remains nonexact with its finite
stable initialized record flag-width qualification. Startup CharFixUp 218 and
Commander storage remain held. No source game EXE, assets, GPU or gameplay
milestone is claimed. NN passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37827194083) on 7ed0d813d068fa61b8552cc622559f2edb09f571. Full qualifications,
remaining contracts and evidence pins are in transfer-checkpoint.json.

### Game reset storage OO (2026-10-08)

Three genuine working states add 12 typed bytes: Tag_DoneAny, Tag_DoneFirst
and GAMEPAD_SKIP. Their owning extents remain unknown, with zero allocation or
exact-data credit. The skip initializer is 0x850; its real reset writes 0x840.
Three natural float copies match retail; the new known 4-byte 3600 constant raises
known exact data to 637,616 bytes.

The specific BARMAN full region grows from 2 to 4 bytes, matching both native
intervals while preserving its signed WORD and two true WORD observers. Its
owning extent remains unknown; no padding is reused. NN BATTLEDROID full 8 is
unchanged. All prior exact data, 13 member contracts/28 bytes and ANG survive.
The next callback/ID preflight reuses eight existing callback owners and requests
only three new slots plus eight WORD IDs, 28 typed bytes. Full CharFixUp 218 and
Commander storage remain separate. See transfer-checkpoint.json for complete
source, CPU and publication qualifications.


### Cutscene and callback working storage PP (2026-10-08)

Three genuine NULL callback slots and eight signed WORD IDs add 28 typed bytes.
Their 42 complete initializer bytes match native; owning extents remain unknown
and receive zero credit. Eight prior callback owners and the canonical Batarang
working buffer are reused. All 10,869 prior full data copies and known exact/member
contracts survive. BARMAN's full 4 and BATTLEDROID's full 8 remain strict;
CharFixUp218 and Commander storage remain held.

The 24 new complete initializer copies total 119 bytes: six known (33 bytes),
18 unknown (86 bytes). Existing known name5 and unknown positive 0.85 float
bindings retain their status. Known data stays 637,616 bytes. Collection's genuine
completion DWORD at 9558a0 has a read-only preflight for the next wave; it is not
activated here. See transfer-checkpoint.json for complete output and CPU domains.


### Startup supplier storage QQ (2026-10-08)

The genuine Collection completion DWORD, Hub/Kamino minimum views, Sarlacc packet
pointer and Redirect tables/pointees are now actual source data. Proved fields and
initializers are represented; unknown owning capacities remain unknown. Two native
DWORD unions give existing BYTE/WORD readers their proper low views, without
introducing duplicate backing.

The 84 new complete initializer copies total 3,117 bytes: 83 (3,109 bytes) match
native; TagDown's eight remain nonexact. Its four-byte pointer and three-byte £
literal match separately; the natural tail is retained without a fake neighbor
provider or padding repair. Six known-extent copies (40 bytes) reuse covered
extents; known exact union remains 637,616 bytes. Four precise old unknown physical
storage pairs are qualified; all other old regions stay strict, including BARMAN
and BATTLEDROID. All 13 typed member contracts survive. The next publisher's
configuration, ASCII and completion-state preflights remain scratch proposals.
See transfer-checkpoint.json for full native/output evidence.

### Startup publisher storage RR (2026-10-09)

The actual source adds 102 minimum state views (465 typed bytes), genuine callback
slots, ASCII pointers and original-owner configuration templates. Exact PC field
widths and offsets are asserted. The first three GlobalBolt records begin at
7ef388 and have stride 164; this working prefix does not fabricate the remaining
41 records or establish full owning capacity. Shop hint input is fifteen WORDs
with its native sentinel, distinct from the larger runtime output capacity.

All 210 new complete initializer copies (2,360 bytes) equal native: 44 have known
original extents, and 166 retain unknown extents with zero allocation/exact-data
credit. Existing aliases cover the known storage; exact-data union remains
637,616 bytes. All prior exact regions and 13 named member contracts survive.
The existing unknown-sized Tag pointer changes naturally from eight to four
COFF bytes; its specific complete pair and the independent left-arrow pointer
are proved without a fabricated tail provider. Other old data regions remain
strict. Matching code generation emits the genuine promoted-f32 qword constant.

The separate Mac linked-symbol estimate in symbols.md supplies 32,230 named
code/data definitions and estimated spans. It changes no PC extent, initializer,
capacity or coverage. Full native/source/output qualifications, runtime domains
and evidence pins are in transfer-checkpoint.json.

### Shader source and minimum declaration-cache backing (2026-10-09)

The current shader publisher consumes genuine Android-derived shader source
text, with the reviewed PC token/directive corrections. `uberShader2` is a
157,210-byte source initializer, not copied compiled shader instructions. Its
original PC owning extent remains unknown, and it is not byte exact to the
retained native text. No allocation, mapping-size or exact-data credit is assigned.
Complete shader output and device execution remain separate verification work.

`PCVertexDeclarationCachePC` at `0x297cc98` now declares a minimum two-record
view (16 zero-initialized bytes), with key at 0, resource pointer at 4 and stride 8.
Native material startup creates keys `0x40101` then `0x101`; the second insertion
moves the first record into row one. This establishes the minimum from actual
producer/consumer behavior. Retail's section loader supplies sixteen mutable
zero-filled bytes, and the reviewed complete mapped-data intervals have no
foreign owner in that range. The count remains its genuine separate zero `i32`
at `0x297cc90`; no reset, clamp or fabricated device result was added.

The original array declaration and capacity remain unknown. The CSV size stays
blank, so this working view receives no allocation/extent or exact-data credit.
It bounds only the reviewed default-material startup cohort; later descriptors
and the full lifecycle need further native evidence. Mac symbol spacing and the
different Android pool are not capacity proofs.

### Handler state and consumed matrix minimum UU (2026-10-09)

Four new canonical views are tint NUVEC4 at986ec0 (16 zero bytes), material pointer
at2854c40 (four zero bytes), reflection DWORD at2854d70 (four zero bytes), and
camera ID at80e9f8 (four FF bytes). Their complete compiled initializers equal
native. Unknown owning extents remain blank and supply zero allocation/exact credit.
A distinct data/BSS cohort after TT's resets preserves every old complete data
output and unknown-sized tail. Existing280d3a4 is one canonical four-byte
integer/pointer union: reset_word and lightmap_packet share offset0 without aliases.

PC lighting semantic49 is ps2ShineMtx, count4/field_c4. Its native consumer reads
64 bytes from packet+70. The corrected named NUMTX reader ends atb0, with all
old emitted code/data unchanged. This is an initialized caller-owned minimum
reader, not an original producer or allocation-capacity proof. The runtime guard
rejected the earlier one-vector specular field; no metadata or padding was forced.

### Native handler/device state VV (2026-10-09)

Nine native-initialized scalar/cache views supply 36 consumed bytes:

- `_NuDisplayHandlerOpacityPc` at `0x80e198`: four-byte canonical `f32` view.
- `_NuDisplayHandlerZModePc` at `0x80e690`: four-byte canonical `i32` view.
- `_NuDisplayHandlerShadowPc` at `0x2857168`: four-byte canonical `f32` view.
- `_PCDevicePrimitiveCallCountPc` at `0x2974cf4`: four-byte canonical `u32` view.
- `_PCDeviceIndexedPrimitiveCallCountPc` at `0x2974cf8`: four-byte canonical `u32` view.
- `_PCDeviceVertexDeclarationCachePc` at `0x297bec8`: four-byte canonical `PC_TEXTURE_RESOURCE_PC *` view.
- `_PCDeviceStreamStrideCachePc` at `0x297bed4`: four-byte canonical `u32` view.
- `_PCDeviceStreamNumberCachePc` at `0x297bed8`: four-byte canonical `u32` view.
- `_PCDeviceStreamOffsetCachePc` at `0x297bedc`: four-byte canonical `u32` view.

All nine complete compiled initializers equal native. Original owning extents
remain blank, with zero allocation, mapping-extent or exact-data credit. A distinct
data/BSS cohort after TT/UU resets preserves all previous complete data outputs,
including unknown-sized tails. D3D9 slot refinements name verified existing PC
offsets without changing aggregate size or introducing a substitute interface.
The CPU fixture exercises initialized minimum views, real cache hits and genuine
unbound/NULL/zero guards; it does not establish original full capacities.

The native FaceOnTransform, lightmap-update and changed-shadow uniform calls have
vector-width reads beyond their nominal tint/local/scalar objects. Those complete
bodies are retained and nonexact; these paths were not exercised. No invented
padding, uniform metadata override or zeroed adjacent storage claims equivalence.

### Material/display state checkpoint WW (2026-10-09)

31 typed definitions provide 840 working bytes with complete native-equal initializers.
The handler table is the observed 49-word prefix at 80e6d0 (27 NULLs, 22 pointers
to 20 real callbacks), with its canonical pointer at 80e794. Three sampler limits
remain const; mutable water, material, back-buffer and presentation states retain
separate native initializers. Every original owning extent remains unknown/blank:
there is zero allocation or exact-data credit, and no inferred larger table.

### Post-effect state checkpoint YY (2026-10-09)

Seven scalar/pointer definitions at 9737f8..973810 supply25 typed working bytes.
All full compiler regions (28 bytes including the one-byte flag's natural tail
padding) equal native zero storage. Every original owning extent stays blank;
there is no exact-data or allocation credit and no synthetic filter object.
The vertex-declaration cache still has its earlier two-record working reservation;
the native producer cohort requires at least3/conditionally4, so the current
reservation is inadequate. Full original capacity remains unresolved. No growth,
counter relocation/reset or copy guard is introduced in this checkpoint.

### Vertex-declaration minimum working prefix checkpoint ZZ (2026-10-09)

The existing cache at 0x297cc98 now reserves four 8-byte records, justified by
the verified four-descriptor producer domain and all 24 insertion permutations.
The actual source changes only [2] to [4]. Full 32-byte native-zero initialization
and the old 16-byte prefix are verified. Counter position/initializer, sections,
getter/search/copy bodies and all other central-data outputs remain unchanged.
Original owning extent stays blank: zero allocation, mapping-extent or exact-data
credit. This is a minimum view, not an inferred original capacity or proof of all
startup inputs. The ordinary run passed the earlier copy crash and reached a
separate NULL material-override list; no artificial list publication is added.

### Declaration-count frontier after AAA (2026-10-09)

No central data changes are made in AAA. The restored real material-list producer
clears the previous NULL-list fault. A separate debugger capture attributes the
next fault to the exact declaration search, called with count 0x0200090a and
descriptor 0x02000909. Our counter is cache+32, the key position of a hypothetical
fifth record; native places the counter eight bytes before its cache. Insertion
overwrite is a causal hypothesis, not a captured preceding write. Full owning
capacity remains unknown. Do not treat the earlier finite four-descriptor domain
as an all-startup upper bound, infer 256 entries from Android/Mac spacing, move
globals, reset the counter or add guards to bypass the failure.

### Startup scalar inputs and complete alignment tails checkpoint BBB (2026-10-09)

Ordinary default-context shared u32 NuDebrisIdealDynamicVBSizePc at80e268=262144
and i32 MaxAnimJoints at973890=0 are added. The private API default at8004e0=
0x500000 stays in nuapi.c. All3 full DWORD initializers equal native; original
owning extents stay blank and receive zero data/extent credit. No placement
controls or cache storage changes are introduced.

All6,136 other old central regions are strict raw/named/applied matches to AAA.
Two complete unknown-extent tail pairs are retained explicitly: tPOWERUP2->4 is
allnativezero, while shader157210->157212 gains0000 alignment before the newDWORD,
which differs from native5f5f. The full shader region stays NONEXACT; its actual
source declaration and payload remain157210 unchanged. No crop/extent inference
or extra credit is made. All2,154 central known-extent exact copies survive.

### Startup occlusion and material/input storage CCC (2026-10-11)

The existing 368-byte g_OcclusionManager aggregate keeps its complete native-equal
initializer and C linkage through its genuine C++ type refinement. All 6,140 old
central data regions remain unchanged. Three class material pointers stay in
original occlusion.cpp; two startup material pointers and PlayerLastPort's eight
signed WORDs use ordinary central storage. All six new complete regions total
36 bytes and equal native initialization, including eight -1 WORDs. Their
original owning extents remain unknown/blank: no allocation, mapping or exact-data
credit. No new constructors/dynamic initialization are emitted; the old CRC16
initializer remains intact. No declaration-cache capacity is inferred or changed.

### Connected viewport, lighting, resource and reset checkpoint DDD (2026-10-11)

Six core extents are NuVpInit607bc0/151, NuPs2GetViewport6e4ff0/62,
NuLgtInitializeStatePc609060/206, NuGobjReleaseChildPc609b90/96,
NuGobjRegistryCleanupPc60c6e0/117 and NuGobjRegistryInitPc60cb00/35.
Neutral PC names avoid claiming unproved Android identities. Viewport getter and
child release match all native bytes. Real hardware calls replace three native-site
markers, preserving the complete owned switch tail and old references.

Seven reset extents are LSW_SetIndy42aa00/489, GameAudio_SetActionMusicTimes547a10/21,
Cheats_Reset56e560/13, Batarangs_Reset404990/184, GameCam_Reset54a2f0/232,
Minicam_InitSystem576c40/21 and ResetScene48c3f0/417. Audio/cheats/minicam/scene
are exact. ResetScene ends at48c591; the following unrelated function is excluded
by genuine boundary evidence. PC-specific character branches, unsigned camera
inputs, narrow sentinels and matrix-copy-before-translation order are preserved.
The full CheckResetBits caller remains held on genuine consumed providers.

Canonical vpCurrent remains in nurndr.c. A64-byte working view replaces its old
16-byte prefix and ten separately allocated scalar duplicates. All named widths
and offsets are asserted. Parent owning extent is blank; four coordinates and
four camera scales preserve32 known physical bytes through named4-byte contracts.
Six other migrated scalars remain unknown-size. Fourteen new member contracts
retain all13 old members. Device viewport's owning extent remains unknown.
The getter copies the two named float fields by memcpy4 to preserve raw bits.

Lighting initializes native eight156-byte working records, loader heads/counts
and runtime defaults; these do not establish original allocation boundaries.
Resource partial views have asserted fields and retain real NuFree/COM calls.
NuFree's held arena/manager behavior prevents a lifetime-success claim. Registry
cleanup retains the finite valid-acyclic-list qualification where the compiler
omits an empty traversal. All new owning extents, including filename padding,
remain blank. ActionMusicStickyTimeoutPc is the genuine bounded two-float array
at8020b4/8, native initializer1.0/6.0, adding8 exact data bytes.

Full prior output review accepts only three finite changes: hardware2162→2178,
unchanged-source InitGameAfterConfig4437 constant-order windows, and TempOptions
16→20 with unchanged typed13-byte zero payload and emitted zero padding. The latter
remains nonexact. RippleForceFieldSoundNamePc retains its full10-byte native-exact
initializer in a genuine const subsection. No full extent is trimmed or masked.

The source run, cleanup and next measured startup order are recorded in the
[current execution policy](transfer-execution.md). It establishes no rendered
frame or playable-game milestone; the thread/timer overlap remains a hold.

### Startup timing, lock ownership and inferred declaration backing EEE (2026-10-11)

PcInitializeStartupCriticalSection4ff100/31 and PcInitializePerformanceTiming4ffbb0/101
match complete native bytes. Real hardware sites call them in native order. The
SDK critical-section (24-byte) and guard (four-byte) views are loader zero, original extents unknown. Existing
canonical TickBase920a00/8 and PerformanceFrequency91efa0/8 remain unchanged.
The thread pool's91ef90 arithmetic bias was mistaken for its first object. Actual
first slot `0x91efa8`, with 13 working records, indexedcount-1/index-1, retains native13-created/12-enterable
asymmetry and prior exact enter/leave30-byte bodies. Original pool owning extent
is unknown: the false old 312-byte extent is retired, reducing union data/map credit by 304 bytes
while preserving frequency (eight bytes). No new original allocation credit is introduced.

Cache `0x297cc98` has 256 inferred eight-byte working records, complete zero 2,048-byte native
initializer, with original extent blank. Independent PC water-object start `0x297d4a0`,
Mac inline eight-byte records / 2,048-byte spacing and Android explicit 256-entry pool favor that reconstruction;
257 or smaller-plus-hidden-storage remain possible. It is not a proved original
PC declaration or complete startup-key upper bound. Full old 32-byte prefix and
complete getter (1,684 bytes), search (82) and copy (84) outputs/references are preserved. Counter,
algorithms, source sections, failure behavior and lifecycle remain unchanged.
No counter reset, capacity guard or success provider is added. Actual runtime
outcome and limitations are recorded in the current execution policy.

### Sound initialization and SFX producer FFF (2026-10-11)

Sixteen new canonical sound states have full working initializer evidence, while
original owning extents remain unknown and blank in the map. Complete emitted
COFF regions total 160,421 bytes; this is working storage, not proved original
allocation coverage. Filename slots use 1,601 x 64, name slots 1,601 x 32, reference
counts 1,601 and configuration filename 256. PC strides/successor anchors and
Android declarations converge on these capacities; smaller original objects plus
padding remain possible. Mac exports give no private DWARF/STABS extent proof.

Existing metadata, request/event state and SoundVoiceManager.samples at 0x2973974
are reused without duplicate allocations. The worker-stop flag is a volatile byte;
its two buffers are native 260 bytes. The initial music/sample extensions point
to genuine .vag/.wav literals. All sixteen complete initializers/relocations and
prior exact data were independently reviewed. The canonical one-byte keyboard
scalar at 0x29755c1 remains a complete one-byte exact BSS definition. No data
trimming, constructor substitute or artificial runtime initialization was added.

### Input/math working state GGG (2026-10-11)

17 canonical input/math/file states retain complete native initializer evidence.
The aspect reader reuses PCDeviceBackend.active_aspect+0x630 without duplicate
storage. Keyboard's named working view grows from0x3c to0x44 for real cooperative
and enabled fields; original allocation extent remains blank. New owning extents
remain unknown/blank with zero original allocation credit. The existing four-byte
endian scalar is reused. Actual CRT initializer pointer has its proved four-byte
extent and exact relocation; all previous data outputs, including natural
alignment-tail changes, are reviewed completely without cropping or masking.
