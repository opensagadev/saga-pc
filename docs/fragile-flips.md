# Fragile function flips during the cleanup

Some functions change code generation with the set of identifiers and files their unit sees, without any
change to their own source or types (docs/header-plan.md, cleanup-audit section 6). They are not tuned
during the cleanup; each flip is logged here and they get one tuning pass once the headers are final.

| push | function | unit | before | after |
|---|---|---|---|---|
| port round 3 | BoulderSpawnPc | legogame/episodeIII.c | exact | 83.28 % |
| stack 2 (stubs, port round 4, layout format) | BlockadeRunnerDEjectCreaturePc | legogame/episodeIV.c | exact | 90.97 % |
| stack 2 | NuRndrParticleGroup | nu2api/nu3d_platform_pc.cpp | 95.92 % | 94.77 % |
