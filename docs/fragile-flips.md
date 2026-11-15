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
