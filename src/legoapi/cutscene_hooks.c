#include "saga.h"

i32 __cdecl CutScene_StartFn_LSW(void *cut) {
    STUBBED_NAMED("CutScene_StartFn_LSW: podrace/bonus-gunship death matrix setup"
                  " (DiePos statics 0x7f5d08/0x7f5d14/0x7f5d24/0x7f5d34/0x7f5d40,"
                  " angle words 0x876618/0x87661c/0x876620 and the 90-degree consts,"
                  " NuMtxSetRotationY + NuMtxTranslate + instNuGCutSceneSetMtx)");
    return 0;
}

void __cdecl CutScene_PreUpdateFn_LSW(void *cut) {
    STUBBED_NAMED("CutScene_PreUpdateFn_LSW: avalanche rumble/shake"
                  " (podrace_avalanche and podsprint_avalanche cutscenes only,"
                  " intensity from instance frame vs 111.0f, NuVecDist to"
                  " PodAvalanchePeakPos scaled at 750.0f, NewRumbleAllPlayers +"
                  " GameCam_NewShake)");
}

void __cdecl CutScene_PostUpdateFn_LSW(void) {
}

void __cdecl CutScene_StoppedFn_LSW(void *cut) {
    if (cut == game_cutscenes_cutscene) {
        if (NuSpecialExistsFn(&LevHSpecial_50)) {
            AddPartDebris(WORLD->part_debris_sys, 0x10, NuSpecialGetDrawPos(&LevHSpecial_50));
            NuSpecialSetVisibility(&LevHSpecial_50, 0);
        }
        if (NuSpecialExistsFn(&LevHSpecial_51)) {
            AddPartDebris(WORLD->part_debris_sys, 0x10, NuSpecialGetDrawPos(&LevHSpecial_51));
            NuSpecialSetVisibility(&LevHSpecial_51, 0);
        }
    }
}

i32 __cdecl CutScene_ReplaceCharacterModelFn_LSW(void *cut, void *character) {
    STUBBED_NAMED("CutScene_ReplaceCharacterModelFn_LSW: podrace/podsprint/gunship"
                  " green-vehicle character replacement by id");
    return -1;
}

i32 __cdecl InitBolt_AddMomentumType_LSW(void *bolt, void *object, void *momentum) {
    STUBBED_NAMED("InitBolt_AddMomentumType_LSW: LSW1 momentum scaling and"
                  " speederbike/deathstar return gates");
    return 0;
}

i32 __cdecl Bolt_HitPlatFn_LSW(void *bolt) {
    STUBBED_NAMED("Bolt_HitPlatFn_LSW: if WORLD->current_level == KAMINOE_LDATA then"
                  " KaminoE_CheckPlatHit(bolt)");
    return 0;
}

i32 __cdecl Bolt_HitCustomFn_LSW(void *bolt, void *points) {
    STUBBED_NAMED("Bolt_HitCustomFn_LSW: if WORLD->current_level == DOGFIGHTA_LDATA then"
                  " return ChrisExtraBoltCollision(bolt, points)");
    return 0;
}

void __cdecl GameBlowUpBlownUpFn_LSW(void *blowup) {
    STUBBED_NAMED("GameBlowUpBlownUpFn_LSW: womprat/spider dynamic-creature spawns"
                  " from blowup special names in MoseIsley/HUB/Tatooine/LostTemple");
}

void __cdecl GizObstacle_SetDefaultSFXFn_LSW(void *world, GIZOBSTACLE *obstacle) {
    if (obstacle->mode == 2 && obstacle->trigger_radius < 0.25f) {
        obstacle->start_sfx_id = (i16)GetSfxId(GetSfxSwPPadName);
    }
}