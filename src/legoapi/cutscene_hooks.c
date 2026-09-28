#include "saga.h"

i32 __cdecl CutScene_StartFn_LSW(CUTINFO *cut) {
    NUMTX matrix;
    NUVEC *position;
    u16 degrees;

    if (PODRACE_ADATA != NULL && PODRACE_ADATA == WORLD->area) {
        if (cut != game_cutscenes_podrace_pod_explode && cut != game_cutscenes_podrace_out_of_time) {
            return 0;
        }
        if (WORLD->current_level == PODRACEA_LDATA) {
            position = &PodRaceADiePos;
            degrees = podrace_die_angle_a;
        } else if (WORLD->current_level == PODRACEB_LDATA) {
            position = &PodRaceBDiePos;
            degrees = 90;
        } else {
            position = &PodRaceCDiePos;
            degrees = 90;
        }
    } else if (BONUS_GUNSHIP_ADATA != NULL && BONUS_GUNSHIP_ADATA == WORLD->area) {
        if (cut != game_cutscenes_bonus_gunship_cavalry_explode) {
            return 0;
        }
        if (WORLD->current_level == BONUS_GUNSHIPA_LDATA) {
            position = &GunshipADiePos;
            degrees = podrace_die_angle_b;
        } else {
            position = &GunshipBDiePos;
            degrees = podrace_die_angle_c;
        }
    } else {
        return 0;
    }

    NuMtxSetRotationY(&matrix, (NUANG)((u32)degrees << 16 / 360));
    NuMtxTranslate(&matrix, position);
    instNuGCutSceneSetMtx((INSTNUGCUTSCENE *)cut->instance, &matrix);
    return 1;
}

void __cdecl CutScene_PreUpdateFn_LSW(CUTINFO *cut) {
    STUBBED_NAMED("CutScene_PreUpdateFn_LSW: avalanche rumble/shake"
                  " (podrace_avalanche and podsprint_avalanche cutscenes only:"
                  " intensity = 111.0f - instance frame clamped via the -110 test,"
                  " 1 - |intensity| / 110, NuVecDist(&GameCam->position,"
                  " &PodAvalanchePeakPos, NULL) scaled at 750.0f,"
                  " NewRumbleAllPlayers(qrand() / 65536 * intensity, 0, 0, 0),"
                  " GameCam_NewShake(GameCam, intensity + intensity, 0.1f, 1.25f))");
}

void __cdecl CutScene_PostUpdateFn_LSW(void) {
}

void __cdecl CutScene_StoppedFn_LSW(CUTINFO *cut) {
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

void __cdecl instNuGCutSceneSetMtx(INSTNUGCUTSCENE *inst, NUMTX *mtx) {
    inst->flags_88 |= 0x80;
    inst->matrix = *mtx;
    NuGCutSceneUpdateMtx(&inst->matrix);
}
