# PC data inventory

`src/data.cpp` now defines **115 independently mapped objects, 58,043 mapped
bytes**. The current local `matching.json` verifies all 115 full COFF extents,
initializers, and the two relocations in `WORLD` and `LWORLD`. Isolated VC8
sections keep `waiting_for_character`, `no_more_loads`, and
`characterbuffer_ptr` at their actual four-byte extents instead of absorbing
alignment padding from the next declaration.
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
asserted in `saga.h`. The function's C reconstruction is still being compared.

The permanent/character/super-buffer pointers and sizes are the 13 four-byte
objects used by the verified StartPerm/EndPerm reconstruction. Their initial
values are zero except `CHARACTERBUFFERSIZE = 0x02800000` (PC 40 MiB). The
Android source's different buffer size was not copied.

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
| `0x02920aac` | `part_raycasts_enabled` | Zero-initialized four-byte i32 gating the PC parts raycast path; Android initializes its counterpart to one, so its default was not copied. |
| `0x0297397c`, `0x00755180`, `0x00810dc8` | `NuSound3Reverb`, `NuSound3LoopHoldTimeMultiplier`, `NuSound3LoopHoldTime` | One-byte zero reverb flag, eight-byte f64 value `1000.0`, and four-byte i32 value `133`; the PC sound setters read or write these exact widths. |
| `0x008108e4`, `0x0296206c` | `debris_time_increment`, `DebrisCutSceneMode` | Four-byte f32 `1/60` step and zero i32 mode, written by the verified PC debris setters. |
| `0x02854bf8`, `0x02854c00` | `nu_water_reset_state`, `nu_water_reset_status` | Separate zero-initialized four-byte integers written as 1 and 0 by `NuWaterReset` at `0x005f9530`. |

`NuTimeGetTicksPS` at `0x004ffb20` and `NuTimeBarSlotReset` at `0x005d8370`
are still code work in progress. Their exact data initializers do not establish
matching function bodies.

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
