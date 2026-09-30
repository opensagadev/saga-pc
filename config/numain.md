# NuMain investigation queue

The original PC `NuMain` is at `0x00492b40`, with a complete 4,332-byte extent.
[Identification notes](identification.md) explain its entry chain and implemented
dependencies. Treat each row as a queue item: `symbols.csv` holds accepted
identities, while local exact comparisons are recorded separately. Unresolved
names remain candidates until the callee, complete boundary and type/ABI are
reviewed.

Android `src/batman.cpp` and the named original Android ELF supplied correlations;
the PC call order, instructions and data references determine whether they apply.
Never port Android-specific paths. In particular, PC uses separate frame update
and render helpers, six episode DAT loads and a Windows message pump. The render
helper receives its world pointer in ESI, so its inferred decompiler signature
cannot be used as a public interface without further work.

The source entry now follows the verified PC startup order through permanent-data
loading. `InitOnce` at `0x004011e0` has a structural stub with its full 1,019-byte
comparison extent. Its PC-sized permanent allocation, argument iteration and
current-directory setup are present; input, camera and time-bar dependencies
remain open. The retail argument helper uses a private ECX/EAX calling convention,
so the source-compatible C wrapper is structural rather than byte-matching.
`InitOnce` now opens the PC's seven startup DAT names, beginning with
`GAME.DAT`, through the exact archive-registration helper. The surrounding
hardware dispatcher and the `LoadPermData` parser path remain incomplete.
`LoadPermData` now calls the PC `Level_SetDefaults` initializer and starts the
episode, area and level text parsers using the retail path strings. The
PC-specific `FixUpAreas` selector now matches exactly using three verified
tables; `Episodes_ConfigureList` has a typed 0x1c-byte record parser. The early
permanent-resource setup, `FixUpLevels`, and later allocation dependencies still
need reconstruction.
The PC default wind speed is 3.0f; the Android value is different.
`NuMain` opens the six episode archives, then stops before indexing level/area
tables. DAT member dispatch now opens, reads, seeks and closes stored archive
members through PC-style handles. The four-record `NuFParOpen` allocator and BOM
setup, `NuFParCreate`, parser position save/restore, and the PC
`Levels_ConfigureList` record parser are implemented structurally. The PC
`Areas_ConfigureList` parser now has typed area and cheat records, including its
level, character, and cheat lookup dependencies. Both generic level keyword
tables and their parser wrappers match, along with 52 of the 54 generic callbacks.
All 44 game-specific before-load callbacks now have mapped PC extents and
source definitions; their 45-record dispatch table and keyword strings have
exact initializers. These callbacks match, but their containing parser and
full `LoadPermData` path remain incomplete.
The two music callbacks and both full record loaders remain structural, so
level/area loading is not yet ready to run through `NuMain`. Its startup
includes a temporary guard before `NuFrameBegin`
because the PC keyboard input owner is not yet initialized by our partial
`InitOnce`. Its later restart and frame-loop phases contain explicit
`STUBBED_NAMED` markers, so this is a runnable scaffold, not a complete or
matching 4,332-byte reconstruction. Keep unresolved references fatal when linking.

The restart scaffold now calls `InitCables` and `InitSnakes` at their verified
PC sites. Both functions match their complete original extents. The PC hub
model-list routine, AI message pool, font state leaves, file-position
dispatch, DAT-member open helpers, animation progress family and music methods are
also reconstructed, though some adjacent
functions remain structural. `Levels_ConfigureList` parses PC level records
with typed fields and verified defaults, but is not a full-byte match.
`Areas_ConfigureList` now parses PC area records and is also nonmatching. The
source EXE does not load these tables through startup before the remaining
parser and allocation paths are verified. The next coherent target is the early
PC permanent-resource setup and the guarded startup handoff to populated tables.

The frame chain now includes `NuFrameBegin`, its portal, water-reset, mouse and
occlusion callees, and a structural `NuFrameEnd` implementation with explicit
markers for renderer and timing dependencies. `NuKeyboardRead` at `0x006d9340`
still has unresolved PC input-state dependencies. The 987-byte PC `NuFrameEnd`
starts with the six-byte `NuHasError` getter at `0x004feb50` and calls the
11-byte `NuClearError` setter at `0x004feb60`; both touch the four-byte state
at `0x00921248`. The PC error-dialog branch uses `ShowingError` at
`0x0091ef3c`. The call at `0x005f3d60` receives a time-bar set index; its
2,130-byte PC body is `NuTimeBarSetRender(i32)`, including GPU timing and
rectangle rendering. The former `NuRndrSwapScreen` attribution was incorrect.
Time-bar slot/control helpers now have separate PC mappings and source bodies;
the render functions remain unimplemented.

`NuDatOpenEx` now reads PC archive metadata through the native Win32 and
host-file layers into InitOnce's permanent arena. It retains the PC's eight
open-file slots, eight-byte tree nodes, and version -3 hash-index compaction;
the hash-string self-copy is preserved. Synthetic Windows archive checks cover
buffered and unbuffered reads, encoded metadata offsets, size reporting, lookup
ordering, and cleanup. A separate local read-only smoke parsed all seven installed
GOG DAT archives in both buffering modes and compared one complete stored payload
per archive against independent Win32 reads. Compressed members have not been
decompressed or validated. The parser is structural and nonmatching. It adds
short-read, bounds and null-arena rejection absent from retail so the incomplete
source entry fails safely. Source startup still exits before indexing absent
level and area tables. The `GAME.DAT`/NuFPar parser closure and remaining frame loop are the next
dependency work, not a completed 4,332-byte `NuMain` match.

The structural math/spline and animation-set dependencies now have broader source
coverage. Animated evaluation, display-special updates, full flow dispatch and
regular AI definition lookup remain marked or unimplemented. The next coherent
port batches follow those closures; NuMain's startup/loop status is unchanged.

## Direct-call candidates

The connected progress closure now includes `ClearLevelProgress`,
`StoreSceneProgress`, effect-off tracking, gizmo progress callbacks, and debris
lookup/status dispatch. These use asserted PC fields, including the embedded
`WORLDINFO` scene-progress record and the animation index. The animation-page
reset/start/stop/clear family, music reset and level-SFX reset are also implemented.
These are structural ports with fresh retail comparisons; they do not complete
world streaming or make the guarded NuMain frame loop playable.

The file layer now supports native/memory writes, copy operations, and nested
block reads. `NuFileEndBlkRead`, `NuFileGetBlkSize` and `NuFileAlignRead` match;
the parent block-reader comparison remains nonexact. `NuCameraSet` only implements
its identity/reset/multiply substeps and marks the missing PC camera builder and
Direct3D submissions. `NuCameraSetEx` has a reviewed 1,105-byte extent at
`0x005ed540`, but its PC backend dependencies remain open.

The next structural target is `StoreLevelProgressFn` and its area-progress
callers, followed by the permanent scene/material loaders. Preserve the startup
table guard until their real dependency closure is ready.

Ghidra body counts below describe reachable instructions and may omit internal
padding or shared/disjoint ranges. They are **not** authoritative CSV sizes.
The first call site and multiplicity are from the complete PC NuMain extent.

| PC address | Candidate | First call | Calls | Ghidra body bytes | Ranges |
| --- | --- | --- | ---: | ---: | ---: |
| `0x004011e0` | `InitOnce` | `0x00492b65` | 1 | 1019 | 1 |
| `0x00401870` | `UNKNOWN` | `0x00492cf4` | 1 | 1 | 1 |
| `0x00408800` | `InitCables` | `0x00493105` | 1 | 127 | 1 |
| `0x0040aa00` | `ViewCamSetActive` | `0x004936dc` | 1 | 79 | 1 |
| `0x00414d40` | `MakePlayerList` | `0x004939de` | 3 | 348 | 1 |
| `0x0041ade0` | `FixUpLayers` | `0x00493084` | 1 | 215 | 2 |
| `0x0042aa00` | `LSW_SetIndy` | `0x00492df1` | 1 | 489 | 1 |
| `0x0042b960` | `CheckResetBits` | `0x00493284` | 1 | 2966 | 1 |
| `0x0042d180` | `Areas_OpenAll` | `0x00492d08` | 1 | 148 | 1 |
| `0x0042d5f0` | `FindGameCutScenes` | `0x004930f5` | 1 | 335 | 1 |
| `0x0042f3a0` | `EndChallenge` | `0x004934ab` | 1 | 193 | 1 |
| `0x0043c800` | `GameAISysSetGame` | `0x00492e46` | 1 | 43 | 1 |
| `0x0043c830` | `GameAISysInit` | `0x00492c38` | 1 | 353 | 1 |
| `0x0046a840` | `GizmoSysSetGame` | `0x00492e4b` | 1 | 40 | 1 |
| `0x0047aa30` | `Hub_MakeModelList` | `0x00492dac` | 2 | 151 | 1 |
| `0x00483c70` | `InitStatusScreen` | `0x0049391e` | 1 | 5012 | 2 |
| `0x00486c00` | `UpdateAchievements` | `0x00493940` | 1 | 1 | 1 |
| `0x00488630` | `ReleaseAllTakeOvers` | `0x00493788` | 1 | 36 | 1 |
| `0x0048a4b0` | `ClearLevData` | `0x004930df` | 1 | 482 | 1 |
| `0x0048b430` | `ClearUpAreaData` | `0x00493beb` | 1 | 260 | 1 |
| `0x0048b540` | `LoadAreaCharacters` | `0x0049307f` | 1 | 3370 | 1 |
| `0x0048f740` | `ClearAreaProgress` | `0x004937cd` | 1 | 252 | 1 |
| `0x0048f990` | `StoreLevelProgress` | `0x00493773` | 1 | 27 | 1 |
| `0x0048f9b0` | `NewArea` | `0x00493159` | 1 | 631 | 1 |
| `0x00491d50` | `UpdateFrame_candidate` | `0x004934bf` | 1 | 2715 | 1 |
| `0x004927f0` | `RenderFrame_ESI_world_candidate` | `0x0049356b` | 1 | 846 | 2 |
| `0x004badd0` | `InitPanel` | `0x00493431` | 1 | 106 | 1 |
| `0x004bbbf0` | `PanelRender` | `0x004935d8` | 1 | 286 | 1 |
| `0x004ca5b0` | `LoadPerm` | `0x00492c84` | 1 | 1986 | 1 |
| `0x004ef5c0` | `UNKNOWN` | `0x00492ebb` | 2 | 98 | 1 |
| `0x004f0d40` | `NuDatSet` | `0x00492c27` | 1 | 16 | 1 |
| `0x004f0d50` | `NuDatGet` | `0x00492b95` | 1 | 6 | 1 |
| `0x004f3e20` | `NuFrameBegin` | `0x00492dc7` | 2 | 32 | 1 |
| `0x004f4060` | `NuCommandLine` | `0x00492b51` | 1 | 24 | 1 |
| `0x004f5100` | `NuVideoGetSwapMode` | `0x004933eb` | 1 | 6 | 1 |
| `0x004f5110` | `NuVideoRollingFrameRateReset` | `0x00493367` | 2 | 1 | 1 |
| `0x004fc2b0` | `NuDatOpen` | `0x00492baa` | 6 | 26 | 1 |
| `0x004fd050` | `NuFrameEnd` | `0x00492dd1` | 2 | 987 | 1 |
| `0x004feef0` | `bgProcInit` | `0x00492c5d` | 1 | 162 | 1 |
| `0x00509620` | `MenuInitialise` | `0x00492b86` | 1 | 49 | 1 |
| `0x00509760` | `MenuReset` | `0x00492b8b` | 2 | 51 | 1 |
| `0x005476e0` | `StartPerm` | `0x00492c7f` | 1 | 23 | 1 |
| `0x00547700` | `EndPerm` | `0x00492c89` | 1 | 124 | 1 |
| `0x00547780` | `ResetCharacterBuffer` | `0x00492fe7` | 1 | 87 | 1 |
| `0x00547a30` | `legoSetMusicVolume` | `0x0049319b` | 3 | 21 | 1 |
| `0x00547ab0` | `GameSetMusicVolume` | `0x00493823` | 1 | 110 | 1 |
| `0x00548ae0` | `GamePlayMusic` | `0x00493171` | 2 | 324 | 1 |
| `0x00548e90` | `MusicClearAll` | `0x004938b8` | 1 | 82 | 1 |
| `0x00548f00` | `UpdateBackgroundMusic` | `0x00493533` | 1 | 1 | 1 |
| `0x00548fb0` | `ResetTimer` | `0x004931c0` | 1 | 46 | 1 |
| `0x00549430` | `ResetFrameCounters` | `0x004931d4` | 1 | 30 | 1 |
| `0x0054a5e0` | `GamePads_Init` | `0x00492c58` | 1 | 117 | 1 |
| `0x0054b540` | `Text_InitLanguageList` | `0x00492c6e` | 1 | 49 | 1 |
| `0x0054b580` | `Text_SetLanguage` | `0x00492c77` | 1 | 27 | 1 |
| `0x0055c5e0` | `ClearPause` | `0x00493230` | 2 | 13 | 1 |
| `0x0055c6d0` | `Game_Exit` | `0x00493c12` | 1 | 175 | 1 |
| `0x0055e1b0` | `RememberPlayerIDs` | `0x00493bd4` | 1 | 249 | 1 |
| `0x0055f430` | `WorldInfo_InitOnce` | `0x00492b56` | 1 | 21 | 1 |
| `0x0055f470` | `WorldInfo_ReArrangeBuffers` | `0x0049309f` | 1 | 132 | 1 |
| `0x005605b0` | `WorldInfo_ClearAllIfScreenFaded` | `0x00493c06` | 1 | 115 | 1 |
| `0x00560630` | `WorldInfo_StreamLevel` | `0x004930d1` | 1 | 154 | 1 |
| `0x00560aa0` | `WorldInfo_Activate` | `0x004930f0` | 1 | 18 | 1 |
| `0x00565470` | `WidescreenCode` | `0x0049352b` | 1 | 339 | 1 |
| `0x00565700` | `DrawCutBorders` | `0x00493587` | 1 | 279 | 1 |
| `0x00565ba0` | `GrabStillScreen` | `0x004935a4` | 1 | 51 | 1 |
| `0x00565d90` | `HandleStillRender` | `0x004935a9` | 1 | 617 | 1 |
| `0x005661f0` | `NoRender` | `0x00493572` | 1 | 159 | 1 |
| `0x00566290` | `FinishLoop` | `0x00492c64` | 4 | 271 | 1 |
| `0x00571b20` | `Parts_Start` | `0x0049321b` | 1 | 30 | 1 |
| `0x00571b40` | `Parts_Stop` | `0x00493736` | 1 | 30 | 1 |
| `0x00574e20` | `GetMenuID` | `0x00493419` | 2 | 48 | 1 |
| `0x0057cd80` | `CutScene_PlayingOrRequested` | `0x0049348c` | 1 | 78 | 1 |
| `0x0057cec0` | `CutScenes_Stop` | `0x00493753` | 1 | 49 | 1 |
| `0x0057d230` | `CutScenes_BGLoadManager` | `0x004933d4` | 1 | 53 | 1 |
| `0x0057eec0` | `CutScenes_End` | `0x004935e0` | 1 | 135 | 1 |
| `0x0058a8d0` | `Mission_Clear` | `0x00492fde` | 1 | 44 | 1 |
| `0x00595a90` | `FreeGameObjectLights` | `0x00493be6` | 1 | 84 | 2 |
| `0x00596530` | `InitShadowLights` | `0x00492c53` | 1 | 632 | 1 |
| `0x00597250` | `InitSnakes` | `0x00493111` | 1 | 99 | 1 |
| `0x0059fa80` | `StoreStatusTakeOverObjectSys` | `0x00493783` | 1 | 342 | 1 |
| `0x005a81b0` | `Particles_Start` | `0x004931f0` | 1 | 80 | 1 |
| `0x005a8200` | `Particles_Stop` | `0x00493741` | 1 | 113 | 1 |
| `0x005aab50` | `ResetGizAIMessageSys` | `0x00492ee1` | 1 | 99 | 1 |
| `0x005aabc0` | `ClearGizAIMessageSys` | `0x00492efa` | 1 | 53 | 1 |
| `0x005aac00` | `CheckGizAIMessage` | `0x00492f0e` | 3 | 324 | 1 |
| `0x005b11b0` | `FadeSystem::SetStage` | `0x00493271` | 1 | 44 | 1 |
| `0x005b1220` | `FadeSystem::SetFade` | `0x00493266` | 1 | 47 | 1 |
| `0x005b2510` | `Movie_Play` | `0x00492ce5` | 2 | 384 | 1 |
| `0x005c9130` | `NuPause` | `0x00493236` | 2 | 11 | 1 |
| `0x005d8370` | `NuTimeBarSlotReset` | `0x004933f3` | 1 | 131 | 1 |
| `0x005d8400` | `_NuTimeBarSlotBegin` | `0x00493400` | 1 | 54 | 1 |
| `0x005dabd0` | `NuHGobjReversibleCharacters` | `0x00492c2d` | 1 | 16 | 1 |
| `0x005dabe0` | `NuHGobjForceShadowsOnCharacters` | `0x00492c33` | 1 | 16 | 1 |
| `0x005fd450` | `NuRndrSwapStreamBuffers` | `0x0049330d` | 1 | 207 | 2 |
| `0x0062f970` | `APITransparentInit` | `0x00492c3d` | 1 | 325 | 1 |
| `0x00652ad0` | `SoundKillAll` | `0x004938bd` | 1 | 20 | 1 |
| `0x00653000` | `RestoreGameCut` | `0x004932ed` | 1 | 67 | 1 |
| `0x006579f0` | `DebrisSetTimeIncrement` | `0x00493411` | 1 | 11 | 1 |
| `0x00659160` | `DebrisGlassInit` | `0x004930fa` | 1 | 194 | 1 |
| `0x0065f4d0` | `edGraEnableTerrainSwap` | `0x00492dcc` | 2 | 1 | 1 |
| `0x0065f4e0` | `edGraDisableTerrainSwap` | `0x00492dd8` | 2 | 1 | 1 |
| `0x0065f4f0` | `edGraInitTerrainSwapProtection` | `0x00492b90` | 1 | 1 | 1 |
| `0x00689c80` | `edgraStartPage` | `0x0049320d` | 1 | 474 | 2 |
| `0x00689e70` | `edgraStopPage` | `0x00493728` | 1 | 81 | 2 |
| `0x00693fd0` | `NuMusic::StopAll` | `0x004938b3` | 1 | 90 | 1 |
| `0x006941a0` | `NuMusic::ResumeTrack` | `0x004932f9` | 1 | 147 | 1 |
| `0x00694d50` | `NuMusic::SetFader` | `0x0049312c` | 2 | 84 | 1 |
| `0x00697a30` | `NuSound3SetReverb` | `0x00493138` | 1 | 22 | 1 |
| `0x006987a0` | `NuSound3FlushLoops` | `0x00493763` | 1 | 1 | 1 |
| `0x00698910` | `NuSound3StopRumble` | `0x00493709` | 2 | 1 | 1 |
| `0x00698a00` | `NuSound3SetLoopHoldTime` | `0x00492c4e` | 1 | 21 | 1 |
| `0x00698e70` | `UNKNOWN` | `0x0049322b` | 4 | 86 | 1 |
| `0x006d3420` | `UNKNOWN` | `0x004933e6` | 1 | 1 | 1 |
| `0x006dd390` | `ProcessWindowsMessages_candidate` | `0x004933d9` | 1 | 97 | 1 |
| `0x006dea60` | `TerrainTrackFlush` | `0x0049370e` | 1 | 1 | 1 |

## Data and member candidates

These entries record access sites and plausible roles. Some are fields inside
larger objects, not separate globals. Observed access widths do not prove a
complete object extent; Android type sizes do not establish PC layouts. Accepted
independent scalar entries and aggregate ranges appear in `symbols.csv`; this
queue also retains the candidates still needing type or ownership review.

| PC address | Candidate role | Observed access/type hypothesis |
| --- | --- | --- |
| `0x00802c54` | `WORLD` | pointer |
| `0x0093b358` | `GAMEDEMO` | i32 |
| `0x0093d838` | `PLAYERCOUNT` | i32 |
| `0x0087b504` | `openlevels` | i32 |
| `0x00951bc0` | `Level` | i32 |
| `0x00951b98` | `LDataList` | pointer |
| `0x00803784` | `Area` | i32 |
| `0x0095123c` | `last_area` | i32 |
| `0x00951374` | `ADataList` | pointer |
| `0x00951378` | `LastAData` | pointer |
| `0x00951384` | `HUB_ADATA` | pointer |
| `0x0093b2bc` | `NextArea_FreePlay` | i32 |
| `0x0093b2c4` | `FreePlay` | i32 |
| `0x0087a9e8` | `makefreeplaymodellist` | i32 |
| `0x00951b9c` | `LastLData` | pointer |
| `0x0088ab50` | `highallocaddr` | unknown_integer_or_pointer |
| `0x0093b2c8` | `SuperStory` | i32 |
| `0x0095df6c` | `gizaimessagesys` | pointer |
| `0x00951388` | `reset_area` | i32 |
| `0x009553f4` | `MissionSys` | pointer |
| `0x00856c28` | `ChallengeMode` | i32 |
| `0x0093d810` | `Player` | pointer_array_8 |
| `0x007fac20` | `PlayTrailer` | i32 |
| `0x009248ac` | `characterbuffer_base` | pointer |
| `0x009248b0` | `characterbuffer_ptr` | pointer |
| `0x009248b4` | `characterbuffer_end` | pointer |
| `0x0087b2fc` | `loadareacharacters_loadedlevel` | i32 |
| `0x00802c68` | `level_already_loaded` | i32 |
| `0x0093d868` | `next_level` | i32 |
| `0x0093d860` | `abort_load` | i32 |
| `0x0088a56c` | `dagobah_training` | i32 |
| `0x00876cec` | `party_cant_be_under_cover` | i32 |
| `0x00955b54` | `ResetOldFStop` | i32 |
| `0x0087b73c` | `new_level_from_menu` | i32 |
| `0x008668ec` | `Game.options_save` | aggregate_member_anchor |
| `0x008668f0` | `Game.options_save.field4_0x4` | aggregate_member_anchor |
| `0x008668f2` | `SuperOptions.music_enabled` | byte_anchor |
| `0x00951ba8` | `TITLES_LDATA` | pointer |
| `0x008765e4` | `reset_restart` | i32 |
| `0x0093d874` | `no_more_loads` | i32 |
| `0x00925028` | `BonusTimer` | aggregate_anchor |
| `0x007f3b14` | `BonusWinner` | i32 |
| `0x0085ea94` | `BonusCoinTotal` | i32 |
| `0x008765ac` | `NewMode` | i32 |
| `0x00802c6c` | `reset_load` | i32 |
| `0x02962054` | `NOSOUND` | i32 |
| `0x009507c0` | `FadeSys.pending_type` | aggregate_member_anchor |
| `0x0095069c` | `MainRenderTargetTime` | float |
| `0x00950698` | `MainRenderTime` | float |
| `0x00803df4` | `newlevel_resumecutaudio` | i32 |
| `0x00925038` | `FRAMETIME` | float |
| `0x0092503c` | `DEFAULTFRAMETIME` | float |
| `0x0087b548` | `frameout_count` | i64_includes0087b54c |
| `0x0087b544` | `frameout` | i32 |
| `0x00802c5c` | `other_level` | i32 |
| `0x0093d86c` | `waiting_for_level` | i32 |
| `0x009513a8` | `gone_through_door_to_new_level` | i32 |
| `0x0093d870` | `waiting_for_new_level` | i32 |
| `0x00951ba0` | `NewLData` | pointer |
| `0x009507a0` | `FadeSys.fade` | aggregate_member_anchor |
| `0x0087b4f0` | `pFadeInfo` | pointer |
| `0x007f23a4` | `waiting_for_character` | i32 |
| `0x00876597` | `TempOptions.field11_0xb` | aggregate_member_anchor |
| `0x008668f7` | `Game.options_save.field11_0xb` | aggregate_member_anchor |
| `0x00950764` | `screendump` | i32 |
| `0x00951264` | `AreaGlobals.values.field_0x20` | aggregate_member_anchor |
| `0x00951bb4` | `HUB_LDATA` | pointer |
| `0x00855e00` | `MiniCutCam` | i32 |
| `0x0093d864` | `WaitingForLevelTime` | float |
| `0x00855eec` | `WaitingForCharacterTime` | float |
| `0x00950768` | `pause_rndr_on` | i32 |
| `0x0087b538` | `PANELOFF` | i32 |
| `0x00802e30` | `clear_screen_onstill` | i32 |
| `0x0087b55c` | `HubMainRenderTimeHack` | i32 |
| `0x0297ad94` | `nuvideo_global_vbcnt` | volatile_i32 |
| `0x0087b4fc` | `peak_poly_count` | i32 |
| `0x0097bb54` | `nurndr_tritot_this_frame` | i32 |
| `0x0087b500` | `poly_count` | i32 |
| `0x00925074` | `MAXFRAMETIME` | float |
| `0x00951c00` | `CUTSTOPGAME` | i32 |
| `0x0093b348` | `Paused` | i32 |
| `0x0093b2c0` | `save_paused` | i32 |
| `0x00802e14` | `FinishLoop_On` | i32 |
| `0x00951b70` | `netclient` | i32 |
| `0x00951bc4` | `LevelChange` | i32 |
| `0x00951bbc` | `CREDITS_LDATA` | pointer |
| `0x0093b0f8` | `Game_OptionsSave` | pointer |
| `0x0087b2f4` | `LevelChangesInArea` | i32 |
| `0x0093b344` | `Arcade` | i32 |
| `0x00805a00` | `hub_from_arcade` | i32 |
| `0x00951bb8` | `STATUS_LDATA` | pointer |
| `0x0087a648` | `StatusPacket` | aggregate_anchor |
| `0x0087a6fc` | `StatusPacket.status_flags` | aggregate_member_anchor |
| `0x0093d83c` | `UsePlayerList` | i32 |
| `0x00951440` | `Door_UseCutCam` | i32 |
| `0x0087a1f8` | `status_plr_active` | byte_array_8 |
| `0x0093d530` | `PlayerProgress` | aggregate_array_anchor_stride0x10 |
| `0x0093d5b0` | `PlayerList` | i16_array_8 |
| `0x0087a6e4` | `StatusPacket.player0_model` | aggregate_member_anchor |
| `0x0087a6e6` | `StatusPacket.player1_model` | aggregate_member_anchor |
| `0x0087a6ec` | `StatusPacket.player0_active` | aggregate_member_anchor |
| `0x0087a6ed` | `StatusPacket.player1_active` | aggregate_member_anchor |
| `0x0092487c` | `exit_request_unknown` | i32 |
| `0x02975a94` | `windows_message` | MSG |
| `0x0091ef34` | `NuCommandLine_argc` | i32 |
| `0x0091ef38` | `NuCommandLine_argv` | pointer |
| `0x0091eea8` | `curr_dat` | pointer |
| `0x0093d878` | `world_array` | aggregate_anchor_stride0x51ac_count2 |

The source/binary graph queue now has broad reviewed ANI3, flow, resource and AI
callback inventory anchors. Implement the six-function ANI3 matrix path next
(`NuAnimData2CalcMatrix` at `0x005f3390` through `0x005ce350` and interpolation
helpers), then its caller `EvalAnim2` and special/display-list update closure.
Their named PC layout proposals and portable drafts are under ignored
`build/analysis/bulk-anim-matrix/` and `bulk-gameanim/`; recheck them against the
retail instructions when integrating. This target expands animation behavior
without displacing the broader structural identification queue.

The connected transfer checkpoint now supplies the ANI3 matrix/extraction chain
and all unblended players, typed EvalAnim/EvalAnim2 callers, special/display-list
updates, and the flow callback/process/reset closure. The four ANI3 blend
players/dispatcher remain the immediate connected animation target. Sound
selection and wrappers are present, but grouped samples and physical backend
playback remain explicit missing actions. NuMain and game startup remain partial.
See identification.md for the next broader string-table and shared object-layout
batches; do not return to register/instruction tuning ahead of those transfers.
