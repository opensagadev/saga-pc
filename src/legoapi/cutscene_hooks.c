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

i32 __cdecl CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *cut, CUTSCENECHARACTER *character) {
    i16 id;
    GAMEOBJECT *vehicle;

    if (cut == NULL || character == NULL) {
        return -1;
    }
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
        if (cut != game_cutscenes_podrace_pod_explode && cut != game_cutscenes_podrace_out_of_time &&
            cut != game_cutscenes_podrace_sebulba) {
            return -1;
        }
        id = id_ANAKINSPOD;
        if (id == -1) {
            return -1;
        }
        if (NuStrICmp(character->name, apicharsys->char_data[id].name) == 0) {
            return -1;
        }
        vehicle = CutDeadVehiclePlayer;
        if (vehicle == NULL) {
            vehicle = player;
            if (vehicle == NULL) {
                return -1;
            }
        }
        if (vehicle->id == id_ANAKINSPODGREEN) {
            return id_ANAKINSPODGREEN;
        }
        return -1;
    }
    if (WORLD->current_level == PODSPRINTA_LDATA) {
        if (cut != game_cutscenes_podsprint_out_of_time && cut != game_cutscenes_podsprint_sebulba) {
            return -1;
        }
        id = id_ANAKINSNEWPOD;
        if (id == -1) {
            return -1;
        }
        if (NuStrICmp(character->name, apicharsys->char_data[id].name) == 0) {
            return -1;
        }
        vehicle = CutDeadVehiclePlayer;
        if (vehicle != NULL) {
            if (vehicle->id == id_ANAKINSNEWPODGREEN) {
                return id_ANAKINSNEWPODGREEN;
            }
            return id_ANAKINSNEWPOD;
        }
        if (player->id == id_ANAKINSNEWPODGREEN) {
            return id_ANAKINSNEWPODGREEN;
        }
        return id_ANAKINSNEWPOD;
    }
    if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
        if (cut != game_cutscenes_bonus_gunship_cavalry_explode) {
            return -1;
        }
        vehicle = CutDeadVehiclePlayer;
        if (vehicle == NULL) {
            return -1;
        }
        id = id_REPUBLICGUNSHIP_GREEN;
        if (vehicle->id == id) {
            return id;
        }
        return -1;
    }
    if (GUNSHIP_ADATA != NULL && WORLD->area == GUNSHIP_ADATA) {
        if (cut != game_cutscenes_bonus_gunship_cavalry_explode) {
            return -1;
        }
        vehicle = CutDeadVehiclePlayer;
        if (vehicle != NULL && vehicle->id == id_NEW_REPUBLIC_GUNSHIP_GREEN) {
            return id_NEW_REPUBLIC_GUNSHIP_GREEN;
        }
        return id_NEW_REPUBLIC_GUNSHIP;
    }
    if (DOGFIGHT_ADATA != NULL && WORLD->area == DOGFIGHT_ADATA) {
        if (cut != game_cutscenes_dogfight_die) {
            return -1;
        }
        vehicle = CutDeadVehiclePlayer;
        if (vehicle == NULL) {
            return -1;
        }
        id = id_JEDISTARFIGHTERYELLOWEP3;
        if (vehicle->id == id) {
            return id;
        }
    }
    return -1;
}

i32 __cdecl InitBolt_AddMomentumType_LSW(BOLT *bolt, GAMEOBJECT *object, NUVEC *momentum) {
    f32 scale;

    if (object == NULL) {
        return 0;
    }
    if (LSW1 != 0 && momentum != NULL) {
        if (WORLD->current_level == BONUS_GUNSHIPB_LDATA) {
            scale = 0.25f;
        } else {
            scale = 0.5f;
        }
        NuVecScale(momentum, &object->target_velocity, scale);
        bolt->speed += NuVecMag(momentum);
        return 0;
    }
    if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->id == id_SPEEDERBIKE) {
        return 1;
    }
    if (WORLD->current_level == DEATHSTARBATTLED_LDATA) {
        return 1;
    }
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
