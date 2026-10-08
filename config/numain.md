# NuMain investigation queue

Current whole-project transfer status is recorded in
[transfer-execution.md](transfer-execution.md). Portable rendering, sound, resource
I/O and gameplay additions do not close NuMain's full lifecycle. The existing
entry-path guard remains necessary while device/input initialization, fixups,
resource creation and world lifecycle providers are incomplete. A linked source
executable is not yet a playable reconstruction. The input guard also stops
before `LoadPerm` calls `NuFrameBegin`; keyboard polling requires the missing
input owner.


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
reproduced by the same-TU static C helper; InitOnce itself remains partial.
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
per archive against independent Win32 reads. The compressed-member reader now
uses the genuine implode decoder, with native-sized working buffers and checks
against independent LH5 output for selected installed payloads, including
multiple-block and boundary-spanning reads. This does not establish scene or
world loading through the source entry. The parser is structural and nonmatching. It adds
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

## Movement, object, animation and script expansion

The next structural batch follows connected portable families beyond NuMain.
`MovePlayerSpline` (`0x00415c60`, 1322 bytes),
`VehicleTurnOrLoopOffset` (`0x00498320`, 342), and
`APIObjectVelocities` (`0x0062b040`, 169) now use asserted PC object/pad fields.
Spline positioning retains its existing owner in `edsplines.c`; seek helpers and
player movement belong in `move.c`. Terrain following and the context-0x3a area
check remain explicit missing actions.

`GameObjectRotation` uses the complete 2650-byte span at `0x00431740`, including
its internal alignment gaps. Its rotation, reflection and suspension state uses
PC offsets, shared four-corner tables, and the reconstructed `FindAnglesZX`.
Terrain sampling and vehicle-hover queries remain missing actions.
`GameObjectOrigin` (`0x0055d2b0`, 993) reconstructs the ordinary predicted origin
and bounds. Its model-joint selection and context-dependent terrain adjustment
remain incomplete: neither an Android joint-array capacity nor a context-table
extent is sufficient PC evidence. Portal lookup uses `collision_position` at
`GAMEOBJECT+0x80`, rather than Android's position at `+0x5c`.

`Bolts_Update` (`0x00583030`, 2738) reconstructs the 32-record update outline,
owner flags, timing, acceleration, integration, bounds and callback dispatch.
Raycasts, collision responses and bolt termination still contain markers. Its
default debris callback is partial; missing game-debris actions remain explicit.
Shoot/Add and the networking abstraction are subsequent dependencies.

ANI3 blended players, fast quaternion interpolation, and the legacy animation
reader/curve-set allocator are reconstructed against PC control flow. The legacy
bit-count builder is `0x005c9140`; it is not an animation-data allocator.
String-table CSV/TXT loading, lookups, filtering and PC extension handling are
reconstructed. The extension helper preserves the retail `PC` suffix check;
the invalid-size assertion action remains missing.

AI script finders, deep list copying, inherited-state construction, and the outer
script-list loader have source implementations. State/parameter/derive/include
callbacks are expanded. Per-file parsing and recursive include parsing retain
markers: their PC driver uses private register arguments, so no invented cdecl
callee is supplied. The indexed constant records have an unresolved allocation
extent; `xConst` is deliberately not implemented with Android's guessed capacity.

Next broad closures: the PC DirectSound voice manager and playback leaves;
projectile Shoot/Add with verified networking and target fields; the per-file
AI parser and its register-ABI integration. Keep instruction tuning secondary.

The next structural batch closes the script-to-sound parameter path and the
vehicle hover calculation. `GameAIScriptAddLevelSfx` traverses the real script,
state and action lists. `Action_PlaySfx` uses the PC parameter keys, the actual
`AIParamToFloat` conversion and nullable callback-based `GetNamedGameObject`
wrapper. The script processor stores four float parameters at +0x14; the timer
remains at +0x24. The `param` substring conversion passes the match pointer to
NuAToI, without skipping the prefix, as retail does. Do not silently repair this
PC behavior using Android intent. Retail also forms the collision-position
address after character lookup without a null guard; startup callback registration
is still incomplete, so this is not evidence of a playable source build.

The three ordinary-ABI path helpers at 0x0051a7c0, 0x0051b7f0 and 0x0051b890 now
share the asserted packet layout. Existing packet flags remain at +0x1e8 while
route masks, selection and path state occupy +0x130..+0x17f; capability flags
are at +0x1f4. Keep the genuine compiler-generated 64-bit shift and its 31-byte
`__allshl` binding rather than declaring that register-ABI helper as cdecl.

`GetVehicleHoverHeight` includes both player-separation gates, level/character
fallbacks, Speeder Chase and the Hoth terrain predicate. The terrain index is
signed at object+0x283. Retail 0x0055ee8e..0x0055eeb2 checks it against 17 before
accessing the same 12-byte table used by the hover function. This closes the
terrain-table identity and removes the previous Hoth marker. The hover function
itself retains retail's lack of a bounds guard in that branch.

The two animation evaluators at 0x005e96d0 (1720 bytes) and 0x005e9d90 (1325
bytes) remain inventory entries, with uncommitted implementation drafts under
ignored build/analysis/. Direct PC loops establish joint stride 0x60, bind matrix
at +0x00 and parent byte at +0x50. A guessed 0x50-byte record was corrected before
landing evaluator code. Their scratch allocation dependency is not ready: the
cursor at 0x00827730 initially points to 0x02976d00, but complete backing capacity
is unresolved. Observed 0x948 and nested 0xd0 requests establish only a minimum
0xa20 live prefix, not the maximum needed by dynamic callers. Do not implement
this allocator over a one-byte anchor or infer its capacity from adjacent owners.
`NuMtxGetScale` is independently implemented with the PC output-pointer ABI.

The DirectSound voice methods, AIPathNodeUpdatePos (683 bytes), WithinConnection
(1532 bytes) and bridge lifecycle now have source implementations. Positional
playback, renderer actions and projectile networking remain connected targets.
Retained bridge research verifies Init/Remove/Create/Update/Draw spans and eight
0xb0c-byte records; Create's 16 stack slots still need parameter-order review.
Reuse these bundles before assigning implementation. Keep instruction tuning
secondary to transferring these complete dependency families.

Repeat-SFX producer and consumer are reconstructed together: one-shot calls
bypass the queue; delayed/repeated requests use the 32-entry ring. `UpdateRepeatSfx`
at 0x005488a0 (135 bytes) advances states 1/3 using FRAMETIME, dispatches state 2
through PC PlaySfxById, decrements the signed remaining count and either resets
or schedules the next interval. Android's additional GameAudio wrapper at this
point is absent from the PC call and is not copied. Physical DirectSound playback
still remains a separate incomplete dependency; queue reconstruction does not
establish working sound or a playable game.

The bridge, PC voice, animation-fixup and projectile batch expands complete
comparison spans rather than tuning individual instruction sequences. Bridge
Init/Remove/Create/Update/Draw use eight asserted 0xb0c-byte records. Their real
terrain getter/setter, platform-hit, platform creation/deletion and track-cleanup
dependencies are implemented, with real calls through the special-instance lookup. The physical
platform arrays contain 24 entries; retail warns about larger creation counts but
continues the loop, so no Android capacity or added guard is substituted.
Bridge drawing still marks its missing renderer and rope-segment actions, including
the unresolved draw-result branch; it never invents a successful draw.

The six ordinary DirectSound voice methods use reviewed PC vtable slots, a
64-slot manager prefix, 0x34-byte voice slots and 0x40c-byte sample records.
Sample ID and voice-bank selection are distinct; frequency divides the sample
rate by 1000. Two one-byte RET hooks are genuine PC routines. The standalone C
volume leaves live in the existing sound unit. Sound loading/initialization and
the playback backend remain separate dependencies; this is not working audio.

ANI3 and legacy animation pointer fixups, including the separate pointer-block
relocator, are implemented. The evaluators still await a proven scratch capacity.
`AIPathNodeUpdatePos` uses the PC special callback, endpoint-oriented displacement,
radius traversal checks and animation activity test. `WithinConnection` is
implemented with its eleven PC stack slots: previous
connection is argument six, route is argument seven, and arguments five/eight
are unused. Its complete extent is 1532 bytes. The numeric helper at
0x00512900 is arcsine-like, not acos; its neutral reconstruction name is
`PCPathArcSine`, with the full signed EAX result kept until callers narrow it.

`Bolt_Shoot` now carries PC type/callback selection, five-slot firing gates,
portal target views, fallback origin, flash state and the final shot flag. Locator
lookup, aim helpers, `Bolt_Add`/network dispatch, selected sound and lifetime updates
remain explicit actions to reconstruct. The source does not fabricate a bolt ID.
NuMain and the runnable game remain partial.

The PC unpositioned playback function and its wrapper are now implemented
(118/62 bytes). They preserve the sample validity gate, signed volume scaling,
voice allocation, table indexing and zero pitch parameter. The 141-byte sample
name method lowercases the PC path, removes a final `_l`, and appends `.ogg`.
Positional playback still requires the private-register listener/distance helper;
loading and DirectSound manager initialization remain incomplete.

`Bolt_Alloc` scans the PC 32-slot pool and retains its original behavior when no
free slot is found. `Bolt_ObjTargetPosYAdjust` uses the verified object field at
+0xb0, C `qrand` and the PC double denominator 65535. The random family is now
compiled as C; its mapped addresses and extents are unchanged.

Next connected target: Bolt_Add, its NetMessage constructor/writers and the
2551-byte Bolt_Init. Independent retail owner-index division at 0x00582ef6 and
an object-array walk at 0x0059fc9e establish the 0x10d8 object stride. The current
GAMEOBJECT definition still represents its shorter observed prefix; recover the
remaining fields/tail before introducing array indexing. Renderer 0x005f0f00 is a
separate larger backend/layout batch. Reuse ignored research under build/analysis/.

The camera/culling bundle at 0x00603f10 (497), 0x00604110 (456) and
0x006042e0 (1608) is implemented. PC scalar and SSE branches retain their
separate comparison behavior, including unordered SSE comparisons. SSE input
uses two 48-byte blocks containing four X, four Y and four Z values; it is not
an array of four-component vectors. Screen clipping follows the retail RET hook
and infinite loop. Complete comparison extents retain the original stack-cookie
instructions even though current project flags disable compiler cookies.

Viewport/scissor projection helpers at 0x005ed300/0x005ed420 (282/274 bytes)
rebuild their scratch matrices when the mode argument is zero and otherwise
reuse cached state. Optional near/far values select the ordinary camera fields
only when zero; unordered/nonzero values select the optional fields. The original
reuses its mode argument slot to hold the rounded cotangent. Projection m32 is
(-2 * far * near) / (far - near), verified through the x87 stack operations.
NuCameraSetEx and its renderer/frustum/backend closure remain unreconstructed;
these helpers do not complete camera setup or NuMain.


## Current structural execution state

The full GameObject stride is now asserted as 0x10d8 with verified named field
cohorts and an opaque unknown tail; older prefix-only notes above describe prior
checkpoints. Compiler cookies are now enabled using the real pinned CRT, and
every existing exact result survives both structural releases. SnapCreaturePos
calls the real character-path query through WORLD->ai_sys at +0x2ae8; its surface
initialization and placement actions remain explicit omissions.

Gravity, formation/path geometry, player layers, gizmo/edit-file loading and
spline intersection closures add complete structural source families. NuMain,
startup backends and gameplay remain partial. The current continuous work queue
is vehicle/directional movement, AI destination/action drivers, object lifecycle
and particle backends, plus character-configuration parsing. Use the execution
plan and current report for metrics; historical body counts are not authoritative
complete comparison extents.


## Third structural release and remaining actions

The current sources add real AI movement dispatch, retreat/wander/intersection
logic, script processing and character processing. AIScriptProcess still marks
the PC-only network notification action; a registered respawn callback is a real
call, but its eventual side-effect implementation remains separate work. Vehicle
movement uses PC inputs, constants and typed fields, including the animation
flags byte at +0xa and WORLD's external spline pointer. Missing rumble and
collision effect/damage actions remain explicit.

GameObjectOrigin now reconstructs the real model-joint branch. GetBlade,
LightSabre, object animation dispatch, AfterAnimation and Kill preserve reviewed
PC control and state; substantial rendering, damage, scripting and callback
construction actions remain marked. Kill uses the actual portal-flag fallback
in the common respawn-count comparison rather than Android's simplified removal
rule. No null or successful placeholder return conceals a missing query.

The particle constructor, KillPart and hit scan now use the full asserted PART
and ADDPART records, real callbacks, pool selection, transforms and rotation
initialization. Dynamic-light and debris backend actions remain incomplete.
Flicker, basketball impact and Boba deflect callbacks are real; Boba movement
retains the genuine frame particle-count RNG but marks its missing trail emission.
Gizmo proximity preserves count/state and the PC persistent destroy flag while
marking its nested blowup action.

Character configuration now supplies the PC keyword callbacks, private state,
parser and loader logic. RedirectTextFile's third argument is filename capacity,
and zero stripped text still follows the PC parser path. Diagnostic omissions
are explicit. NuFilePakLoadKey uses real alignment, file reading, checksum and
key decoding providers, including the existing PC retry routine. Its dependency
NuFileLoadBuffer retains the separately documented packed-file path gap.

Next concrete closures are takeover/tag/rider processing, socket configuration
and geometry, ordinary AI actions, and last-safe/Buck logic. NuMain and a playable
game remain partial; current percentages come from matching.json, not these
historical function lists.

## Fourth structural dependency release

Takeover/tag/rider calls now use recovered animation-duration, jump and Buck
providers. Last-safe position tracking, platform tests and player restoration
use the PC fields and genuine callback state. StartBigJump has the PC six-argument
interface; context-reset actions remain marked. ManageGameObjects, removal and
deactivation reconstruct their PC state transitions, with missing network,
creature-reset, group and torpedo actions explicitly retained as gaps.

AfterAnimation now constructs the basketball and rocket ADDPART descriptors,
including real callbacks, targeting and allocation results. Detonate returns
the real eight-record explosion pool allocation; debris and judder omissions
remain visible. The nested blowup provider replaces the existing caller's missing
whole action, while five unreconstructed effect/action kinds remain marked in
its new body. None of these closures establishes completed startup or gameplay.

Socket parsing, configuration, generation, position queries and rotation now
use genuine matrix/quaternion/trigonometric dependencies. PC behavior differs
from Android in the rail cursor return, allocation-exhaustion path, projection
calculation and several wrappers. The next movement dispatcher and combat action
cohorts are prepared separately; camera terrain queries still require their real
scan/cast providers before implementation can be released.

## Fifth structural release and stopping point

MovePlayer, POD, rolling, gunship, circle, twist and force-along movement now
use verified PC layouts and genuine socket/input providers. GameAI simpleton,
combat and movement-control actions use PC constants and real queries.
UpdateGameObjects reconstructs its complete comparison span with explicit
missing action markers; remembering player IDs, resetting context, rendering,
networking and other unreconstructed actions remain gaps. No successful
placeholder return conceals a missing query.

The GameAI name lookup, creature-origin cache, object dimensions, double-score
zone, Yoda distance, techno controller and active-mission providers are real.
The creature inline name has a minimal address view; its original writable
capacity is still unknown. Adaptive difficulty uses typed mixed fields and
the original signed nine-row table, with its pointer initialized to row four.
Unknown owner extents receive no mapping or exact-size credit.

Work is paused at the user's request after this verified commit. Next broad
source target is the reviewed 8,385-byte enemy/control callback family, followed
by player context/reset actions needed by UpdateGameObjects. Terrain camera
work remains scratch-only pending semantic and state-producer verification.
NuMain and gameplay remain incomplete.


## Resumed queue and level-transition source

Continuous work has resumed. Action_CompleteLevel now uses genuine cutscene
queue/lookup and level transition/fallback providers, adding 566 original code
bytes to comparison. The typed field cohort preserves WORLDINFO's layout;
queue and cutscene declarations remain access prefixes with explicit normal
runtime conditions. No unknown allocation receives extent or exact credit.
This closes that AI callback's reviewed source dependencies, not the larger
NuMain loading, rendering and gameplay gaps. RaceOpponent's reviewed movement
closure is the next source preparation target.


RaceOpponent and animation-speed callbacks now use genuine C with existing
movement, socket and parser providers, adding 976 compared original code bytes.
Their partial typed field cohorts preserve the object layout; full owner types
and literal allocations remain held. This extends the reviewed AI source path
without closing NuMain's unrelated gameplay/loading/rendering gaps. The next
connected dependency review is UseCurrentSpeed and its CurrentStart provider.


The current-speed closure is now genuine PC source: CurrentStart499ca0/288
and accepted UseCurrentSpeed44c2a0/336 add 624 compared original bytes. Fresh
identity, field, source and actual compiled-contract reviews preserve the
zero-only speed gate, direct PC dogfight-level fallback, C44/EC8 selection,
signed socket arguments and all stack/x87 return states. Neither routine is
exact; literal complete extents remain held. Next prepare the accepted
687-byte SetScaleOverride callback using existing parser/random providers
and collision-scale/process fields. Its x87 NaN predicates, timed rounding
and interpolation require their own source and compiled review. The separate
SetCurrentSpeed indirect lookup at slot923924 remains unresolved.


SetScaleOverride44c750/687 is now genuine source with independently checked
parser/random closure and actual compiled float-storage/predicate contracts.
Its full comparison is nonexact at 54.141594%; 687 additional original bytes
are compared without new mapping extent. The next connected family is
SnapToPosition446d90/663 and SnapToSockPosition447230/1413. Their surface
and socket helpers require original identity, private-register ABI and field
review before release. InitSurfaceInfo's optional callback951030 and its
unimplemented query/shadow/reflection providers remain concrete holds;
SetObjOnSurface and SetSockPostion are separate bounded closure candidates.
The independent SetCurrentSpeed slot923924 debt remains unresolved.


The independent surface/socket release now implements SetObjOnSurface572660/233
and SetSockPostion53f640/241, adding 474 compared and mapped original bytes.
Both actual compiler contracts are reviewed in their existing compilation
units; previous private helper report rows and all 801 exact functions remain
unchanged. The callback slot951030 has a qualified null/literal-target access
view, with unknown owner extent/type and zero coverage credit. InitSurfaceInfo
and the accepted SnapToPosition/SnapToSockPosition callbacks still need their
separate surface-query, shadow and reflection closure; mapping the slot alone
does not complete that provider family. SetCurrentSpeed's independent indirect
lookup at923924 remains held.


GetSurfaceInfo572440/536 and four terrain getters109 now have genuine source,
as does the accepted three-vector transform5c0620/196. The getters add109 exact
bytes while preserving every earlier exact function. The query keeps the
qualified callback slot interface and precise surface/water/roof field views;
unknown global owner sizes/types remain held. InitSurfaceInfo's genuine draft
still needs actual shadow/reflection implementations. Their NewShadowEx path
reaches NewScanRot/NewCast8155 and the terrain skin group, whose record and
local initialization source gates remain explicit. No playable entry-path
closure follows from these comparisons; slot923924 remains a separate debt.


The terrain skin and scratch batch now compares2,511 more original bytes.
TerrainSkin984 and SkinPlatform1475 share the existing terrain unit and
passed independent actual compiler ABI/rounding/defined-local reviews.
NuScratchRelease15 is exact, with all805 earlier exact functions preserved;
NuScratchAlloc3237 is genuine nonexact source. The scratch initializer and
minimum backing view are verified without full original allocation credit.
The skin compiler reaches x87 peak8 from empty entry, so future callers
require their own live-prefix check. Valid-record selector/output-capacity
qualifications remain explicit.

The next concrete provider is TerrainSkinAllocate66f790/345, whose genuine
pool-reuse/eviction draft needs independent source/data review and compiler
verification. Its actual globals differ from the previously inspected query
cursor pool. NewScanRot/NewCast8155 then become the next larger source
family; cell/SET associations, W values, pointer-list append limits and
96-entry platform-list writer evidence are frozen. NewCast retains a
qualified regular-input draft; original+2million sentinel-first ties may
read undefined stack normal values, so no invented default is ported.
Shadow/reflection wrappers and InitSurfaceInfo still need this closure.

The scan/cast/pool batch now compares another 8,500 original bytes in the
existing terrain unit. This supersedes the earlier pending-source queue for
TerrainSkinAllocate, NewScanRot and NewCast. Their actual compiled ABI,
rounding, stack/register and provider-depth gates pass with the documented
valid-engine qualifications; all 806 earlier exact functions remain intact.
Whole-PC fuzzy coverage moves from 6.3490629% to 6.3935661%, while exact
code remains 45,870 / 3,471,246 bytes. Compared original bytes become
368,040 (367,985 unique after the unchanged 55-byte alias overlap).

Next is the genuine NewShadowEx wrapper, then GameShadow/reflection and
InitSurfaceInfo/snap callers. NewShadowEx uses a local position copy and
returns its resulting Y after discarding NewCast's ST0 return and releasing
scratch. Its null-terrain path resets the observed outputs without allocation
or clearing the platform-off word. The two switch providers and the world
platform-list fields require separate acceptance/compilation before the
larger wrappers can be released. No playable entry-path closure is claimed.

CI for the preceding `d4e209f1` checkpoint timed out twice during Wine package
downloads, before lint/tests/build ran. Those cancelled runs are not passes.
The build job limit is increased from 20 to 60 minutes for the next push;
its exact commit must still receive terminal CI verification.

Shadow wrappers, platform toggles and InitSurfaceInfo now have genuine source,
with 1,260 newly compared original bytes and four exact wrapper/switch bodies.
SnapCreaturePos's two actual provider calls are restored without claiming
new credit for its already-compared 408-byte body. New Init and switch
identities are separately reviewed; original types and full data-owner extents
remain held. The next connected source batch is the 2,076-byte snap action
family. Its socket/parser/actor/party-array evidence must close before release;
no marker or inventory-only mapping counts as implementation progress.


### Current startup frontier after checkpoint S (2026-10-05)

Checkpoint R connected the native hardware/window/input/CPU/shutdown owners;
its debugger observed nonzero input, HWND and D3D factory, without a device
pointer read. S restores the real NuRndrInitEx stream call and all real
NuDisplayListInit calls, including the private state initializer and BeginFrame
tail. Init's complete127 bytes now match exactly. StreamAlloc uses the verified
three physical arguments and complete140-byte extent; its full candidate stays
nonexact. Nine renderer states stay in their original private PC owners.

Actual S Wine execution passes the earlier stream-backed descriptor write and
fails later in NuDisplayListAddRenderScene: displaylist_2d_first is NULL at the
item copy. The genuine renderer configuration167 remains an explicit omission,
so its real initializer/provider chain is the next connected source work.
The run exits5 without watchdog, debugger dialog or owned leftover process.
S does not provide new device/input memory observations, Present, repeated
frames or a loaded scene. No forced cursor, guard, entry or fake value provider
is used to obtain a runtime milestone.

NuReadGraphicsData, NuGScnRead and scene clones now retain their real18 provider
contracts, with the native void texture-ID fixup explicitly missing. LoadPerm1
and its existing LoadPermData caller are connected but partial:17 native actions
remain marked, and missing earlier material setup can leave gradient state
invalid. Gradient storage uses canonical DebMat[5]; native end arguments carry
the address of the real permbuffer_end word, without second backing.
Collection constructors and the original charconfig variant callback are
transferred; selected88 actual-normal checks establish only their tested arena
and parser domains. They do not establish full resource or world loading.

Next suppliers are the real AI ingress/paths/bounds five-function6,656-byte
family and renderer configuration/descriptors/arena/quad/VBIB/material six-function
1,144-byte family. Existing partial script607 and renderer InitEx32 receive no
new transfer credit. WORLD/permanent/area parents still need genuine character
model and area configuration providers. Texture fallback requires provenance;
retail DDS assets cannot be copied into source to fabricate a provider. Device
reset/recreate, CPU and shutdown omissions remain explicit.

### Current startup and resource frontier (2026-10-07)

Checkpoint W connects six real WORLD/minikit/resource calls and the shader
dictionary's macro, register and output-buffer children. Its actual executable
still exits5 at a NULL ShaderManager receiver before world loading. Hardware,
window and input startup remain connected; whole-code matching percentages do
not establish playable completion. Unattended tests suppress the crash dialog
in their own prefix and preserve the genuine fault/exit.

The next source batch adds real condition/AST emission, ripple lifecycle and
partial texture-ID repair. Ingestion now calls the genuine void fixer, and
minikit cleanup calls real scene removal. The fallback texture value, Restore
actions and populated-material shader path remain incomplete. The shader manager
publisher7412e0/constructor7460d0 still needs genuine parser/backend values;
the separate5f22a0 initializer cannot publish a substitute manager. Uniform
lookup/program expansion and lighting are reviewed candidates awaiting their
separate storage/release gates. Area/FreePlay/permanent source remains held on
initializer preservation and real producer domains, without padding controls.

### Connected source transfer Y (2026-10-07)

Hub/Episode/BuildIt, lighting, backdrop/resource, text crawl, shader uniform/program
expansion and finite parser/token source are now connected for normal-object
verification. This does not complete the startup shader-manager producer. The
real Retrieve receiver is still expected to be null until genuine parser/backend
and HLSL publication are reconstructed. Native missing void actions in Hub,
scene restoration/fallback and startup remain explicit. No replacement manager,
borrowed executable code, fallback asset or fabricated ID was installed.
Unattended smoke preserves the fault/exit, suppresses the Wine debugger dialog
in its own prefix and cleans only its own processes. Device Present, recurring
frames, loaded scenes and playable completion remain unverified.

### Current connected entry/resource frontier after Z (2026-10-07)

Area/FreePlay, accessory and expression families are now real compared source.
Area's already-ready resource suppliers are connected; unready particle/music/
draw/input actions remain explicit markers. Source-only entry still reaches a
NULL shader-manager Retrieve receiver before world/scene loading. ActualEXE
exit 5/3.319s at source5058e0,+16/full208 is retained, with ShowCrashDialog0 and
no owned game/fixture/debugger/dialog leftovers. No window/Present/repeated
frames/scene/gameplay milestone is inferred from matching or CPU tests.

Next close genuine shader declaration/node/semantic and compiler Configure
providers, followed by source/include/cache/Generate publication. Full include
vector outlines remain a real COFF/native helper contract; no dummy vtable,
borrowed HLSL or fabricated success. Terrain handle/ray families progress in
parallel. DDS/value fallback, Perm angle extent loss and Hub FP pair stay held.

### Current shader and terrain frontier after AA (2026-10-07)

Shader declaration/semantic and base configuration/filter providers, terrain
pickup/platform mutation and three ray variants are now real compared source.
Actual313-normal-provider selected terrain checks pass46/0, with zero replacements
or supplements and all426inputs unchanged. The unmodified source EXE still
exits5 after3.406s at NULL Retrieve receiver, source5058e0,+16/full208.
ShowCrashDialog0 and cleanup leave no owned debugger/crash window or game child.
Persistent window/input/device/Present/loaded scene/gameplay remain unverified.

Continue terrain Subset/Reset3311, HLSL methods974 and genuine cutscene
scheduling/streaming. Parser/include9/1977 and compiler controls7/728 remain
source research on real STL/record helper closure. Complete335/581-byte EH
groups have independently reviewed identities; their four internal catch points
receive no separate guessed extents/credit. Tool support remains a reviewed
proposal, and actual embedded shader source/backend publication stays unready.
No dummy vtable, borrowed HLSL, fake provider or successful fallback is introduced.

### Source/runtime frontier after BB (2026-10-07)

Real terrain Reset/Subset/Full/dispatch, HLSL methods and cutscene scheduling
providers are compiled and compared. Ninety selected terrain checks pass using
all 313 actual normal providers, zero replacement/supplement objects and frozen
426 inputs. The fixture supplies four explicit native caller prestates; it does
not establish real loaded scenes or safe arena capacity. Native wall cursor and
rotating NULL-matrix hazards remain in source.

The authentic source EXE still exits5 at NULL Retrieve receiver, +16/full208.
ShowCrashDialog DWORD 0 and launch-private cleanup leave no owned game/debugger
process or crash dialog. Persistent window, input/device setup, Present, repeated
frames, loaded world and gameplay remain unverified. Matching retains priority.
Next are accepted terrain consumers/draw14/8,171, cutscene11 additional5,984 and
backend7/1,310 with the genuine D3DXCompileShader import export. Manager publication
and natural class lifetime/vtable/RTTI closure remain separate work. Authored
embedded HLSL preserves tokens/directives but full text is nonexact and inactive.
Parser EH full335/581 groups and portable support are reviewed research; unbound
natural generated helpers still prevent producer activation. No fake tables,
providers or alternate startup are introduced.

### Source/runtime frontier after CC (2026-10-07)

Terrain floor/radius/separation, ray backoff and shadow consumers, connected draw
and skin/arena producers, and cutscene scheduling are compiled and compared.
Selected CPU fixtures pass 160 checks / zero failures: 109 terrain consumer and
51 skin/arena checks, using all 313 actual normal providers and 426 frozen inputs
with zero replacement or supplement providers. This tests their explicit valid
input domains; it does not prove native capacity, loaded scenes or cutscene
runtime. Native unchecked hazards are retained.

Three previously skipped authentic hardware calls are restored: input/graphics
capability configuration, render-buffer descriptors and warning/debug state
initialization. The actual source EXE still exits 5 after 3.314 seconds at NULL
Retrieve receiver +16/full208 (source fault 0x507a60, native method 0x741750).
Own-prefix ShowCrashDialog DWORD 0 and launch-private cleanup leave no owned
game/debugger process or visible crash dialog. Window, input/device, Present,
repeated frames, scene loading and gameplay remain unverified.

Next transfer closed compiler lifetime/setters 412 and genuine HLSL backend
1,310 with the actual D3DXCompileShader export, plus the connected font resource
load/lifetime family. These close real dependencies but do not independently
fix NULL manager publication. Parser generated helpers, publisher lifetime and
embedded-source provenance remain separate gates. No invented providers or
forced startup state is introduced.

### Connected font loading and authentic runtime DD (2026-10-07)

LoadPermData now calls genuine LoadPermLoadFonts93 before its language spin.
Four native game-font paths use the real permanent-buffer cursors and Text_LoadFont.
The existing partial parent1532 receives zero new byte credit. Resource QFN
read/create/duplicate/lifetime/write and game/button font loading suppliers are
compiled; system embedded font and shader-dependent successful loading remain
unverified. Native uninitialized draw/arena lanes, unchecked cursor assumptions
and nonrecovering diagnostic/failure policies remain visible.

Selected actual-normal CPU fixture passes64 meaningful checks/zero failures:
QFN/universal serialization, actual file/cursor failure policies and material/
texture lifetime. All313 real source providers plus only fixture addition,
zero replacements/supplements,431 inputs and six artifacts remain unchanged.
Compiler/backend source review does not establish positive D3DX/GPU output.
Authentic source EXE still exits5 after3.489 seconds at NULL Retrieve+16/full208,
source0x5087e0/native0x741750. Own-prefix ShowCrashDialog DWORD 0 and private
cleanup leave no owned game/debugger process or crash window. Window/device/
Present/repeated frames/loaded scenes/gameplay remain unverified. Shader manager
publication, parser helper/lifetime closure and embedded-source provenance
remain the genuine startup frontier.

### Authentic runtime and connected music frontier EE (2026-10-07)

Selected actual-normal geometry145/0 plus music42/0 use all313 providers,
431 frozen inputs/six artifacts and zero replacement/supplement engine definitions.
Geometry valid finite domains exclude native wall count>=3 nontermination and
NULL-node/nonempty removal. Music status/error/prepare_only/subtrack1/-1f/NaN
checks use genuine suppliers but establish no audio/asset success. Authentic
source EXE still exits5/4.230s at NULL Retrieve+16/full208, source0x508830.
ShowCrashDialog DWORD 0 and private cleanup leave no owned crash dialog/debugger
or game. Window/device/Present/frames/scene/gameplay remain unverified.

Music value suppliers unlock the true cutscene staging statuses; controller activation remains separately gated. No alternate startup entry, fake device/COM provider, forced publisher or runtime guard was introduced.

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

### Character loading, default movement and angle checkpoint GG (2026-10-08)

LoadPerm2 now brings real character configuration, model-loading, collection,
resident, customiser/special-move and status-screen music calls into the source.
The actual ExtraCharacterFixUpAfterConfig callback installer remains explicit;
default animation/movement suppliers are the next connected source wave.
The canonical nopakfile reference resolves the true shared state without a fake
model-loading provider or duplicate storage. This does not bypass the current
NULL shader-manager publication failure. Authentic source startup/owned cleanup
results are retained in checkpoint-GG/runtime-acceptance.json; persistent window,
device/Present/frame/scene/gameplay success remains unverified.

### Character callbacks, full Jump and save/menu checkpoint HH (2026-10-08)

The complete JumpCode6473 and related real animation, lunge and combat providers
are now source-linked, with the genuine guarded seven-argument call restored in
Move_CHARACTER at native4b4e69. Default movement still omits40 true void actions;
five special motion parents and default animation retain their documented void
omissions. Full Jump has32 actual providers and no fabricated return values.
Callback source identification does not complete LoadPerm2's separate installer.
Save/menu state and drawing families add51 real functions; two coordinator parents
retain five missing file operations. Runtime checks cover CPU state only.

Startup is still blocked by NULL shader-manager publication. The current actual
Retrieve208 equals physical EXE source bytes at513270; fault513280 is+16, whereas
native741750/208 remains nonexact. A transient unclassified window is insufficient
to prove device/Present/frame/scene/gameplay success. Own-prefix crash suppression
and private cleanup work unattended. Dedicated next hardware work follows actual
lost-device/resource/framebuffer providers; unused generated shader-helper identity
holds are preserved without alternate setup, fake publication or guard removal.

### Connected motion, graphics lifetime, credits and script loading II (2026-10-08)

The source EXE now contains genuine automatic surface/framebuffer C++ lifetime and
the permanent loader's real four-argument script-load call. All319 source owners
link through real dependencies. These facts do not establish graphics setup or a
working game: the authentic entry still reaches NULL shader Retrieve+16 and exits5
after3.801405 seconds. Current whole208-byte normal code equals its physical source
EXE, while differing from retail. No persistent native window/device/Present/frame/
scene/gameplay success is inferred from the one unclassified transient window.

Actual-normal646 CPU checks pass, including real automatic CRT registration and
pool allocation/reuse, while all437 inputs/six artifacts remain stable. They use no
replacement engine provider or fake COM/asset callback. Own-prefix crash dialogs
are disabled and final owned-window/process inventories are empty. Keep closing
genuine shader publisher/parser and device/reset/provider gaps; never substitute
a pointer/guard bypass. Source script callbacks still have real action/condition
definition omissions; the15 connected caller actions add zero new unique coverage.

### Connected source snapshot LL (2026-10-08)

Move_CHARACTER now invokes genuine HatMachine_MoveCode and Lever_MoveCode at
native action sites 4b4e8b/4b4ebd. The new lifecycle/Draw, push, mission-menu and
streaming families add 25,502 compared PC bytes; the parent reconnects receive no
additional transferred credit. All required local checks pass for 331 owners;
actual-normal bounded CPU fixtures pass 264 assertions with no replacement providers.
They do not establish real worker asset loading, positive Hat drawing, device
presentation or recurring gameplay. Native registration parents still require
all 28 ordered callback values plus NULL before activation.

No new source-game EXE launch was made. II remains the last observed full-game
frontier: exit 5 at NULL shader Retrieve+16 before level loading. Genuine manager/
compiler publication and its real constructors/returned values remain required.
Automated fixture launch set and queried ShowCrashDialog DWORD 0 and left no owned
processes. Specific old nonexact compiler outputs retain finite reviewed qualifications;
LL passed [exact-revision CI](https://github.com/opensagadev/saga-pc/actions/runs/37811078781). This does not alter the startup frontier.

### Connected registration and character source MM (2026-10-08)

The genuine obstacle, Tube, Plug, MiniCut and special registration families,
character-list loader and ten character callbacks/suppliers now compile and link.
Their complete PC extents add 15,476 unique compared code bytes. CPU tests cover
241 bounded registration/progress/parser/POD cases through real normal objects;
they do not activate installed characters or camera playback. Missing character
VOID actions remain explicit. The full 28-way LSW registration parent and real
startup CharFixUp table remain held until every consumed callback/storage value
is supplied; no NULL table substitute is introduced.

No source-game launch was repeated. The observed startup frontier remains II's
NULL shader Retrieve+16 before a level. Real shader manager/compiler publication,
resource ownership, device presentation and recurring frames remain required.
The test's own Wine prefix suppressed crash dialogs and its process group was
empty afterward. Full measurements and release qualifications remain in
transfer-checkpoint.json; matching percentage is not runnable completion.

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
milestone is claimed. Exact pushed-revision CI is pending. Full qualifications,
remaining contracts and evidence pins are in transfer-checkpoint.json.
