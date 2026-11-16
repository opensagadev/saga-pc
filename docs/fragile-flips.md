# Fragile function flips during the cleanup

Some functions change code generation with the set of identifiers and files their unit sees, without any
change to their own source or types (docs/header-plan.md, cleanup-audit section 6). They are not tuned
during the cleanup; each flip is logged here and they get one tuning pass once the headers are final.

| push | function | unit | before | after |
|---|---|---|---|---|
| port round 3 | BoulderSpawn | legogame/episodeIII.c | exact | 83.28 % |
| stack 2 (stubs, port round 4, layout format) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.97 % |
| stack 2 | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.92 % | 94.77 % |
| literals at every use | BoulderSpawn | legogame/episodeIII.c | 83.28 % | exact |
| literals at every use | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | 90.97 % | exact |
| layout asserts to comments | BoulderSpawn | legogame/episodeIII.c | exact | different |
| layout asserts to comments | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | different |
| duplicate types into headers | BoulderSpawn | legogame/episodeIII.c | different | exact |
| duplicate types into headers | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | different |
| conflicting unit types unified | BoulderSpawn | legogame/episodeIII.c | exact | different |
| conflicting unit types unified | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | different | exact |
| 27 more repeated types into headers | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | different |
| view types out of decomp.h | BoulderSpawn | legogame/episodeIII.c | exact | different |
| repeated unit types into headers (bf2074d8) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| layout asserts deleted | BoltTypes_Configure | legoapi/bolts.cpp | exact | 99.88 % (newly fragile: two asserts fewer in the unit) |
| layout asserts deleted | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | 90.96 % | exact |
| area and level flag names | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | different |
| character context names | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | different | exact |
| fold NUSPECIALHANDLE (641acdcb) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| fold NUSPECIALHANDLE (641acdcb) | BoltTypes_Configure | legoapi/bolts.cpp | 99.88 % | exact |
| legoapi data into its units | BoltTypes_Configure | legoapi/bolts.cpp | exact | 99.88 % |
| records out of legoapi_types.h | DebrisSetup2 | legoapi/parts.c | exact | 99.95 % |
| editor declarations into headers | BoltTypes_Configure | legoapi/bolts.cpp | 99.88 % | exact |
| editor declarations into headers | DebrisSetup2 | legoapi/parts.c | 99.95 % | exact |
| raw offsets to fields, pad and display list names (e0d401e3, ed99ef3e) | BoltTypes_Configure | legoapi/bolts.cpp | exact | 99.88 % |
| rendering types into d3dCore.h (a7157fd0) | DebrisSetup2 | legoapi/parts.c | exact | 99.95 % |
| gameapi declarations into headers | BoulderSpawn | legogame/episodeIII.c | different | exact |
| nu2api declarations into headers | BoulderSpawn | legogame/episodeIII.c | exact | 83.28 % |
| nu2api declarations into headers | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| nu2api declarations into headers (2674d959) | DebrisSetup2 | legoapi/parts.c | exact | 99.95 % |
| flag names through carriers (93a2b335) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| editor pseudo ids and door flags (f87fb539) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | 90.96 % | exact |
| frame stats struct (PcFrameStatsReset) | DebrisSetup2 | legoapi/parts.c | 99.95 % | exact |
| frame stats struct (PcFrameStatsReset) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| episodeIII.c narrowed (87894ba9) | BoulderSpawn | legogame/episodeIII.c | 83.28 % | exact |
| game.cpp narrowed | InitGameAfterConfig | legogame/game.cpp | 82.2397 % | 82.2372 % |
| nu3d_platform.cpp narrowed | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.06 % | 95.37 % |
| character config bitfields in PLAYERCHARACTERCONFIG | BoulderSpawn | legogame/episodeIII.c | exact | different |
| character config bitfields in PLAYERCHARACTERCONFIG | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | 90.96 % | exact |
| one EXPLOSION record (the 0x34 view deleted) | BoulderSpawn | legogame/episodeIII.c | 83.28 % | exact |
| one EXPLOSION record (the 0x34 view deleted) | BoltTypes_Configure | legoapi/bolts.cpp | 99.88 % | exact |
| one EXPLOSION record (the 0x34 view deleted) | SubItemMenu | legogame/shop.c | 97.72 % | 98.31 % |
| one EXPLOSION record (the 0x34 view deleted) | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.37 % | 94.56 % |
| one MISSIONSYS record (0x30) | BoulderSpawn | legogame/episodeIII.c | exact | 83.28 % |
| one MISSIONSYS record (0x30) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| one MISSIONSYS record (0x30) | BoltTypes_Configure | legoapi/bolts.cpp | exact | 99.88 % |
| one MISSIONSYS record (0x30) | SubItemMenu | legogame/shop.c | 98.31 % | 97.72 % |
| one MISSIONSYS record (0x30) | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 94.56 % | 95.37 % |
| one MISSIONSYS record (0x30) | InitGameAfterConfig | legogame/game.cpp | 82.2372 % | 82.2397 % |
| data-move declarations into headers | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | 90.96 % | exact |
| PANEL_MOVEMENT/PANEL_SYSTEM in gizpanel.h | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 95.18 % |
| PANEL_MOVEMENT/PANEL_SYSTEM in gizpanel.h | DebrisSetup2 | legoapi/parts.c | exact | 99.95 % |
| PANEL_MOVEMENT/PANEL_SYSTEM in gizpanel.h | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.06 % | 95.37 % |
| PANEL_MOVEMENT/PANEL_SYSTEM in gizpanel.h | NuDeferredFilterGen::render | nu2api/nu3d_includes.cpp | 93.90 % | 93.89 % |
| PANEL_MOVEMENT/PANEL_SYSTEM in gizpanel.h | InitGameAfterConfig | legogame/game.cpp | 82.2372 % | 82.2397 % |
| BURN_SET in rtl.h | NuDeferredFilterGen::render | nu2api/nu3d_includes.cpp | 93.89 % | 93.90 % |
| BURN_SET in rtl.h | do_maya_mouse_camera | editor/edlevelall.cpp | 98.15 % | 98.11 % (newly fragile) |
| PC_HG_SHADOW types in nustencilshadows_PC.h | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.37 % | 95.92 % |
| EQUIVALENT_GROUP in world.h | BoltTypes_Configure | legoapi/bolts.cpp | 99.88 % | exact |
| EQUIVALENT_GROUP in world.h | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.92 % | 95.37 % |
| EQUIVALENT_GROUP in world.h | InitGameAfterConfig | legogame/game.cpp | 82.2397 % | 82.2372 % |
| pc marker renames (renames/17-*.csv, no code change) | BoltTypes_Configure | legoapi/bolts.cpp | exact | 99.88 % |
| pc marker renames (renames/17-*.csv, no code change) | DebrisSetup2 | legoapi/parts.c | 99.95 % | exact |
| one NUDISPLAYLIST record (0x44) | BoulderSpawn | legogame/episodeIII.c | 83.28 % | exact |
| one NUDISPLAYLIST record (0x44) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| one NUDISPLAYLIST record, after rebase onto e1a372f4 | BoltTypes_Configure | legoapi/bolts.cpp | exact | 99.88 % |
| one NUDISPLAYLIST record, after rebase onto e1a372f4 | SubItemMenu | legogame/shop.c | 97.72 % | 98.31 % |
| force records: one GIZFORCE, GIZFORCEGROUP, GIZFORCESYS | BoulderSpawn | legogame/episodeIII.c | exact | 83.28 % |
| force records: one GIZFORCE, GIZFORCEGROUP, GIZFORCESYS | DebrisSetup2 | legoapi/parts.c | exact | 99.95 % |
| force records: one GIZFORCE, GIZFORCEGROUP, GIZFORCESYS | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | 90.96 % | exact |
| episode records: one PODSPRINT, the unused copies go | InitGameAfterConfig | legogame/game.cpp | 82.2397 % | 82.2372 % |
| legoapi declarations into module headers (both passes, on 103b32b2) | DebrisSetup2 | legoapi/parts.c | 99.95 % | exact |
| dead type copies, AI hook typedefs, CUT_PLAYER_STATE in CUTINFO (on d5817b62) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.96 % |
| dead type copies, AI hook typedefs, CUT_PLAYER_STATE in CUTINFO (on d5817b62) | DrawSubItems | legogame/shop.c | 82.98 % | 82.95 % (newly fragile: no change to its unit's source) |
| dead type copies, AI hook typedefs, CUT_PLAYER_STATE in CUTINFO (on d5817b62) | do_maya_mouse_camera | editor/edlevelall.cpp | 98.11 % | 98.15 % |
| dead type copies, AI hook typedefs, CUT_PLAYER_STATE in CUTINFO (on d5817b62) | PC_SPECIAL_SHADOW_LIGHT::RenderSceneShadow | nu2api/nu3d_includes.cpp | 69.90 % | 71.14 % (newly fragile: no change to the function) |
| nu2api declarations into module headers | BoulderSpawn | legogame/episodeIII.c | 83.28 % | exact |
| view types onto their records (gizmo bolt hooks, MINIANIMPACKET, menu pads, customiser; stream A on f878f821) | BoulderSpawn | legogame/episodeIII.c | exact | 83.28 % |
| view types onto their records (gizmo bolt hooks, MINIANIMPACKET, menu pads, customiser; stream A on f878f821) | NuRndrParticleGroup | nu2api/nu3d_platform.cpp | 95.92 % | 95.37 % |
| writable char arrays inlined as literals (stream C) | BoulderSpawn | legogame/episodeIII.c | exact | 83.28 % |
| writable char arrays inlined as literals (stream C) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 90.97 % |
| nufpar wide word buffer, typed WORLDINFO.guidelines (stream B) | BoulderSpawn | legogame/episodeIII.c | exact | different |
| status rumble pair and Factory B net packet (stream C) | BoulderSpawn | legogame/episodeIII.c | different | exact |
| typed VARIPTR members, prim vertex copies merged into NULGTVERTEX (stream B) | BlockadeRunnerDEjectCreature | legogame/episodeIV.c | exact | 95.18 % |
