#include "saga.h"

#include <string.h>

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
        if (NuSpecialExistsFn(&LevHSpecial[50])) {
            AddPartDebris(WORLD->part_debris_sys, 0x10, NuSpecialGetDrawPos(&LevHSpecial[50]));
            NuSpecialSetVisibility(&LevHSpecial[50], 0);
        }
        if (NuSpecialExistsFn(&LevHSpecial[51])) {
            AddPartDebris(WORLD->part_debris_sys, 0x10, NuSpecialGetDrawPos(&LevHSpecial[51]));
            NuSpecialSetVisibility(&LevHSpecial[51], 0);
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

i32 __cdecl Bolt_HitPlatFn_LSW(BOLT *bolt) {
    if (WORLD->current_level == KAMINOE_LDATA) {
        KaminoE_CheckPlatHit(bolt);
    }
    return 0;
}

extern i32 __cdecl ChrisExtraBoltCollision(BOLT *bolt, NUVEC *points);

i32 __cdecl Bolt_HitCustomFn_LSW(void *bolt, void *points) {
    if (WORLD->current_level == DOGFIGHTA_LDATA)
        return ChrisExtraBoltCollision((BOLT *)bolt, (NUVEC *)points);
    return 0;
}

const char BlowupSpawnWompratScriptPc[16] = "spawned_womprat";
const char BlowupBinNamePc[4] = "bin";
const char BlowupBinLidNamePc[8] = "bin_lid";
const char BlowupSpawnSpiderScriptPc[14] = "blowup_spider";
const char BlowupWebNamePc[4] = "web";
const char BlowupBushNamePc[5] = "bush";
const char BlowupWompratGeneratorNamePc[9] = "womp_gen";
extern i16 CharacterFixupIdPc007f18c4;
extern AREA_DATA_PC *MOSEISLEY_ADATA, *TATOOINE_ADATA;
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, position) == 0x30);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, type) == 0xa4);
STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animated_special) == 0x30);

void __cdecl GameBlowUpBlownUpFn_LSW(void *blowup_pointer) {
    GIZMOBLOWUP_PC *blowup = (GIZMOBLOWUP_PC *)blowup_pointer;
    char *name;
    if (blowup == NULL || blowup->type == NULL || WORLD->area == NULL || netclient != 0)
        return;
    if (WORLD->area == MOSEISLEY_ADATA || WORLD->area == HUB_ADATA) {
        name = NuSpecialGetName(&blowup->type->animated_special);
        if (name == NULL || NuStrIStr(name, (char *)BlowupBinLidNamePc) != NULL ||
            NuStrIStr(name, (char *)BlowupBinNamePc) == NULL || qrand() / 656 >= 75)
            return;
    } else if (WORLD->area == TATOOINE_ADATA) {
        name = NuSpecialGetName(&blowup->type->animated_special);
        if (name == NULL || NuStrIStr(name, (char *)BlowupWompratGeneratorNamePc) == NULL)
            return;
    } else if (WORLD->area == LOSTTEMPLE_ADATA) {
        if (aicreature_sets_alive[15] >= 5)
            return;
        name = NuSpecialGetName(&blowup->type->animated_special);
        if (name == NULL ||
            (NuStrIStr(name, (char *)BlowupBushNamePc) == NULL && NuStrIStr(name, (char *)BlowupWebNamePc) == NULL))
            return;
        AddDynamicCreature(ZipUpLineCharacterIdPc, &blowup->position, 0, (char *)BlowupSpawnSpiderScriptPc, NULL, NULL,
                           1, NULL, NULL, 0, 16);
        return;
    } else {
        return;
    }
    AddDynamicCreature(CharacterFixupIdPc007f18c4, &blowup->position, 0, (char *)BlowupSpawnWompratScriptPc, NULL, NULL,
                       1, NULL, NULL, 0, 16);
}

void __cdecl GizObstacle_SetDefaultSFXFn_LSW(void *world, GIZOBSTACLE *obstacle) {
    if (obstacle->mode == 2 && obstacle->trigger_radius < 0.25f) {
        obstacle->start_sfx_id = (i16)GetSfxId(GetSfxSwPPadName);
    }
}

void SpeedBlur_Apply(WORLDINFO *world) {
    struct level_data_pc_s *level;
    if (editor_active != 0 || CUTSTOPGAME != 0 || MiniCutCam != 0) {
        return;
    }
    level = world->current_level;
    if (level == PODRACEA_LDATA || level == PODRACEB_LDATA || level == LevelFixupSlotPc0087b0b4) {
        if (PodRace_InStartCountdown(WORLD) != 0) {
            return;
        }
    } else {
        f32 countdown;
        if (level != PODSPRINTA_LDATA) {
            return;
        }
        countdown = PodSprint_InStartCountdown(WORLD);
        if (!(countdown <= 0.0f)) {
            return;
        }
    }
    NuSpeedBlurSetMotionFactors(0.01f, -0.024f, 0.02f);
    NuLightSpeedBlur(Paused, 0.5f);
}

void __cdecl instNuGCutSceneSetMtx(INSTNUGCUTSCENE *inst, NUMTX *mtx) {
    inst->flags_88 |= 0x80;
    inst->matrix = *mtx;
    NuGCutSceneUpdateMtx(&inst->matrix);
}

i32 __cdecl KaminoE_CheckPlatHit(BOLT *bolt) {
    KAMINOEPLATE *hit;

    if (bolt->unknown_fa == kamino_plate_id) {
        hit = kamino_plate_hit;
        if (kamino_plate_1 != NULL && kamino_plate_1_active != 0) {
            if ((kamino_plate_1->flags_13a & 0x30) == 0 && (kamino_plate_1->flags_13a & 0x2) != 0) {
                if (hit == NULL) {
                    hit = kamino_plate_1;
                    kamino_plate_hit = hit;
                }
            }
        }
        if (kamino_plate_2 != NULL && kamino_plate_2_active != 0) {
            if ((kamino_plate_2->flags_13a & 0x30) == 0 && (kamino_plate_2->flags_13a & 0x2) != 0) {
                if (hit == NULL) {
                    hit = kamino_plate_2;
                    kamino_plate_hit = hit;
                }
            }
        }
        if (kamino_plate_3 != NULL && kamino_plate_3_active != 0) {
            if ((kamino_plate_3->flags_13a & 0x30) == 0 && (kamino_plate_3->flags_13a & 0x2) != 0) {
                if (hit == NULL) {
                    hit = kamino_plate_3;
                    kamino_plate_hit = hit;
                }
            }
        }
        if (kamino_plate_4 != NULL && kamino_plate_4_active != 0) {
            if ((kamino_plate_4->flags_13a & 0x30) == 0 && (kamino_plate_4->flags_13a & 0x2) != 0) {
                if (hit == NULL) {
                    kamino_plate_hit = kamino_plate_4;
                }
            }
        }
        return 1;
    }
    return 0;
}

#define CUT_HAND_FLIP(m)                                                                                               \
    do {                                                                                                               \
        (m)->m02 = -(m)->m02;                                                                                          \
        (m)->m12 = -(m)->m12;                                                                                          \
        (m)->m20 = -(m)->m20;                                                                                          \
        (m)->m21 = -(m)->m21;                                                                                          \
        (m)->m23 = -(m)->m23;                                                                                          \
        (m)->m32 = -(m)->m32;                                                                                          \
    } while (0)

i32 __cdecl PCCutAni3CurveInteger(ANI3_HEADER_PC *animation, f32 *values, i32 index) {
    if (animation->curve_types[index] == 10) {
        i32 raw;
        memcpy(&raw, &values[index], sizeof(raw));
        return raw;
    }
    return (i32)(values[index] < 0.0f ? values[index] - NuQFntHalfScale : values[index] + NuQFntHalfScale);
}

f32 *__cdecl PCCutLocatorAni3(CUT_LOCATOR_ANIMATION_PC *locator, NUMTX *matrix, f32 frame) {
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(locator->animation, 0, frame, NULL);
    NUVEC translation;
    if (locator->animation->node_flags[0] & 1) {
        NUANGVEC angle;
        angle.x = (i32)(values[3] * AIPathAngularScale);
        angle.y = (i32)(values[4] * AIPathAngularScale);
        angle.z = (i32)(values[5] * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else
        NuMtxSetIdentity(matrix);
    translation.x = values[0];
    translation.y = values[1];
    translation.z = values[2];
    NuMtxTranslate(matrix, &translation);
    CUT_HAND_FLIP(matrix);
    NuMtxTranslate(matrix, (NUVEC *)&locator->base_matrix.m30);
    return values;
}

void __cdecl PCCutRigidAni3(CUT_RIGID_ANIMATION_PC *rigid, f32 frame, NUMTX *matrix) {
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(rigid->animation, 0, frame, NULL);
    u8 flags = rigid->animation->node_flags[0];
    NUVEC translation, scale;
    if (flags & 1) {
        NUANGVEC angle;
        angle.x = (i32)(values[3] * AIPathAngularScale);
        angle.y = (i32)(values[4] * AIPathAngularScale);
        angle.z = (i32)(values[5] * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else
        NuMtxSetIdentity(matrix);
    if (flags & 8) {
        scale.x = values[6];
        scale.y = values[7];
        scale.z = values[8];
        NuMtxPreScale(matrix, &scale);
    }
    translation.x = values[0];
    translation.y = values[1];
    translation.z = values[2];
    NuMtxTranslate(matrix, &translation);
    CUT_HAND_FLIP(matrix);
    NuMtxTranslate(matrix, (NUVEC *)&rigid->base_matrix.m30);
}

void __cdecl PCCutCharacterAni3(CUT_CHARACTER_ANIMATION_PC *character, f32 frame, NUMTX *matrix, i32 *visible,
                                u32 *animation_index, f32 *animation_rate, f32 *blend_time, f32 *start_frame,
                                i32 *layer_mask) {
    ANI3_HEADER_PC *animation = character->animation;
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, 0, frame, NULL);
    u8 flags = animation->node_flags[0];
    NUVEC scale;
    if (animation->curve_count > 6)
        *visible = (i32)(values[6] < 0.0f ? values[6] - NuQFntHalfScale : values[6] + NuQFntHalfScale);
    else
        *visible = character->flags & 1;
    if (animation_index != NULL) {
        if (animation->curve_count > 7)
            *animation_index = (i32)(values[7] < 0.0f ? values[7] - NuQFntHalfScale : values[7] + NuQFntHalfScale);
        else
            *animation_index = character->animation_index;
        if (start_frame != NULL) {
            if (*animation_index == 0 || *animation_index == 255)
                *start_frame = 0.0f;
            else if (animation->curve_count > 10)
                *start_frame = values[10];
            else
                *start_frame = character->start_frame;
        }
    } else if (start_frame != NULL)
        *start_frame = 0.0f;
    if (*visible == 0)
        return;
    if (layer_mask != NULL)
        *layer_mask = animation->curve_count > 11 ? PCCutAni3CurveInteger(animation, values, 11) : -1;
    if (flags & 1) {
        NUANGVEC angle;
        angle.x = (i32)(values[3] * AIPathAngularScale);
        angle.y = (i32)(values[4] * AIPathAngularScale);
        angle.z = (i32)(values[5] * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else
        NuMtxSetIdentity(matrix);
    NuMtxTranslate(matrix, (NUVEC *)values);
    CUT_HAND_FLIP(matrix);
    scale = *NuMtxGetScale(&scale, &character->base_matrix);
    NuMtxPreScale(matrix, &scale);
    if (animation_rate != NULL)
        *animation_rate = animation->curve_count > 9 ? values[9] : character->animation_rate;
    if (blend_time != NULL)
        *blend_time = animation->curve_count > 8 ? values[8] : character->blend_time;
}
#undef CUT_HAND_FLIP

#define CUT2_VALUE(c, t, k, n) ((k)[n] == 0 ? (c)[n].constant : NuAnimCurve2CalcValEx(&(c)[n], (t), (i32)(k)[n]))
#define CUT2_FLIP(m)                                                                                                   \
    do {                                                                                                               \
        (m)->m02 = -(m)->m02;                                                                                          \
        (m)->m12 = -(m)->m12;                                                                                          \
        (m)->m20 = -(m)->m20;                                                                                          \
        (m)->m21 = -(m)->m21;                                                                                          \
        (m)->m23 = -(m)->m23;                                                                                          \
        (m)->m32 = -(m)->m32;                                                                                          \
    } while (0)

f32 *__cdecl NuGCutLocatorCalcMtx(CUT_LOCATOR_GENERIC_PC *locator, f32 frame, NUMTX *matrix, NUANIM_TIME2_PC *time) {
    CUT_ANIMATION_PC *animation = locator->animation;
    DWA_CURVE_CELL_PC *curves;
    i8 *types;
    NUVEC translation;
    if (animation == NULL) {
        *matrix = locator->base_matrix;
        return NULL;
    }
    if ((i32)animation->prefix.header.magic >= (i32)0x414e4934 &&
        (i32)animation->prefix.header.magic <= (i32)0x414e4935)
        return PCCutLocatorAni3((CUT_LOCATOR_ANIMATION_PC *)locator, matrix, frame);
    curves = animation->prefix.curves;
    types = animation->prefix.curve_types;
    if (animation->node_flags[0] & 1) {
        volatile f32 rx = CUT2_VALUE(curves, time, types, 3);
        volatile f32 ry = CUT2_VALUE(curves, time, types, 4);
        volatile f32 rz = CUT2_VALUE(curves, time, types, 5);
        NUANGVEC angle;
        angle.x = (i32)(rx * AIPathAngularScale);
        angle.y = (i32)(ry * AIPathAngularScale);
        angle.z = (i32)(rz * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else
        NuMtxSetIdentity(matrix);
    translation.x = CUT2_VALUE(curves, time, types, 0);
    translation.y = CUT2_VALUE(curves, time, types, 1);
    translation.z = CUT2_VALUE(curves, time, types, 2);
    NuMtxTranslate(matrix, &translation);
    CUT2_FLIP(matrix);
    NuMtxTranslate(matrix, (NUVEC *)&locator->base_matrix.m30);
    return NULL;
}

void __cdecl NuGCutRigidCalcMtx(CUT_RIGID_GENERIC_PC *rigid, f32 frame, NUMTX *matrix) {
    CUT_ANIMATION_PC *animation = rigid->animation;
    NUANIM_TIME2_PC time;
    DWA_CURVE_CELL_PC *curves;
    i8 *types;
    u8 flags;
    NUVEC translation, scale;
    if (animation == NULL) {
        *matrix = rigid->base_matrix;
        return;
    }
    if ((i32)animation->prefix.header.magic >= (i32)0x414e4934 &&
        (i32)animation->prefix.header.magic <= (i32)0x414e4935) {
        PCCutRigidAni3((CUT_RIGID_ANIMATION_PC *)rigid, frame, matrix);
        return;
    }
    types = animation->prefix.curve_types;
    curves = animation->prefix.curves;
    flags = animation->node_flags[0];
    NuAnimData2CalcTime(&animation->prefix, frame, &time);
    if (flags & 1) {
        volatile f32 rx = CUT2_VALUE(curves, &time, types, 3);
        volatile f32 ry = CUT2_VALUE(curves, &time, types, 4);
        volatile f32 rz = CUT2_VALUE(curves, &time, types, 5);
        NUANGVEC angle;
        angle.x = (i32)(rx * AIPathAngularScale);
        angle.y = (i32)(ry * AIPathAngularScale);
        angle.z = (i32)(rz * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else
        NuMtxSetIdentity(matrix);
    if (flags & 8) {
        scale.x = CUT2_VALUE(curves, &time, types, 6);
        scale.y = CUT2_VALUE(curves, &time, types, 7);
        scale.z = CUT2_VALUE(curves, &time, types, 8);
        NuMtxPreScale(matrix, &scale);
    }
    translation.x = CUT2_VALUE(curves, &time, types, 0);
    translation.y = CUT2_VALUE(curves, &time, types, 1);
    translation.z = CUT2_VALUE(curves, &time, types, 2);
    NuMtxTranslate(matrix, &translation);
    CUT2_FLIP(matrix);
    NuMtxTranslate(matrix, (NUVEC *)&rigid->base_matrix.m30);
}

void __cdecl NuGCutCharAnimProcess(CUT_CHARACTER_GENERIC_PC *character, f32 frame, NUMTX *matrix, i32 *visible,
                                   u32 *animation_index, f32 *animation_rate, f32 *blend_time, f32 *start_frame,
                                   i32 *layer_mask) {
    CUT_ANIMATION_PC *animation = character->animation;
    NUANIM_TIME2_PC time;
    DWA_CURVE_CELL_PC *curves;
    i8 *types;
    i8 flags;
    NUVEC translation, scale;
    if (animation == NULL) {
        *visible = character->flags & 1;
        if (animation_index != NULL)
            *animation_index = character->animation_index;
        *matrix = character->base_matrix;
        if (animation_rate != NULL)
            *animation_rate = character->animation_rate;
        if (blend_time != NULL)
            *blend_time = character->blend_time;
        return;
    }
    if ((i32)animation->prefix.header.magic >= (i32)0x414e4934 &&
        (i32)animation->prefix.header.magic <= (i32)0x414e4935) {
        PCCutCharacterAni3((CUT_CHARACTER_ANIMATION_PC *)character, frame, matrix, visible, animation_index,
                           animation_rate, blend_time, start_frame, layer_mask);
        return;
    }
    NuAnimData2CalcTime(&animation->prefix, frame, &time);
    curves = animation->prefix.curves;
    types = animation->prefix.curve_types;
    flags = animation->node_flags[0];
    if (character->animation->prefix.curve_count > 6) {
        volatile f32 value = CUT2_VALUE(curves, &time, types, 6);
        *visible = (i32)value;
    } else
        *visible = character->flags & 1;
    if (animation_index != NULL) {
        if (character->animation->prefix.curve_count > 7) {
            volatile f32 value = CUT2_VALUE(curves, &time, types, 7);
            if (value < 0.0f)
                *animation_index = 255;
            else
                *animation_index = (i32)value;
        } else
            *animation_index = character->animation_index;
        if (start_frame != NULL) {
            if (*animation_index == 0 || *animation_index == 255)
                *start_frame = 0.0f;
            else if (character->animation->prefix.curve_count > 10) {
                volatile f32 value = CUT2_VALUE(curves, &time, types, 10);
                *start_frame = value;
            } else
                *start_frame = character->start_frame;
        }
    } else if (start_frame != NULL)
        *start_frame = 0.0f;
    if (*visible == 0)
        return;
    if (layer_mask != NULL) {
        if (character->animation->prefix.curve_count > 11) {
            volatile f32 value = CUT2_VALUE(curves, &time, types, 11);
            *layer_mask = (i32)value;
        } else
            *layer_mask = -1;
    }
    if (flags & 1) {
        volatile f32 rx = CUT2_VALUE(curves, &time, types, 3);
        volatile f32 ry = CUT2_VALUE(curves, &time, types, 4);
        volatile f32 rz = CUT2_VALUE(curves, &time, types, 5);
        NUANGVEC angle;
        angle.x = (i32)(rx * AIPathAngularScale);
        angle.y = (i32)(ry * AIPathAngularScale);
        angle.z = (i32)(rz * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else
        NuMtxSetIdentity(matrix);
    translation.x = CUT2_VALUE(curves, &time, types, 0);
    translation.y = CUT2_VALUE(curves, &time, types, 1);
    translation.z = CUT2_VALUE(curves, &time, types, 2);
    NuMtxTranslate(matrix, &translation);
    CUT2_FLIP(matrix);
    scale = *NuMtxGetScale(&scale, &character->base_matrix);
    NuMtxPreScale(matrix, &scale);
    if (animation_rate != NULL) {
        if (character->animation->prefix.curve_count > 9) {
            volatile f32 value = CUT2_VALUE(curves, &time, types, 9);
            *animation_rate = value;
        } else
            *animation_rate = character->animation_rate;
    }
    if (blend_time != NULL) {
        if (character->animation->prefix.curve_count > 8) {
            volatile f32 value = CUT2_VALUE(curves, &time, types, 8);
            *blend_time = value;
        } else
            *blend_time = character->blend_time;
    }
}
#undef CUT2_VALUE
#undef CUT2_FLIP

typedef struct CUT_VISIBLE_LOCATOR_PC {
    NUMTX matrix;
    NUVEC pivot;
    f32 scale;
    NUANIMDATA2_PC *animation;
    u32 unknown_54;
    u8 flags;
} CUT_VISIBLE_LOCATOR_PC;
STATIC_ASSERT(offsetof(CUT_VISIBLE_LOCATOR_PC, scale) == 0x4c);
STATIC_ASSERT(offsetof(CUT_VISIBLE_LOCATOR_PC, animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_VISIBLE_LOCATOR_PC, flags) == 0x58);
static char CutLocatorSfxFilterPc[] = {3, -1};
static char CutLocatorFilterPc[] = {6, 7, -1};
static char CutLocatorScaleFilterPc[] = {6, 7, 8, -1};

i32 __cdecl NuGCutLocatorIsVisble_3(CUT_LOCATOR_GENERIC_PC *locator, f32 frame, f32 *scale, f32 *rate) {
    ANI3_HEADER_PC *animation = (ANI3_HEADER_PC *)locator->animation;
    char *filter;
    f32 *values;
    i32 result;
    f32 visible;
    if (animation->curve_count == 4)
        filter = CutLocatorSfxFilterPc;
    else if (animation->curve_count == 8)
        filter = CutLocatorFilterPc;
    else
        filter = CutLocatorScaleFilterPc;
    values = NuAnimCurveExtractAllNodeCurves_3(animation, 0, frame, filter);
    if (animation->curve_count == 4) {
        visible = values[3];
        return (i32)(visible < 0.0f ? (f64)visible - 0.5 : (f64)visible + 0.5);
    }
    visible = values[7];
    result = (i32)(visible < 0.0f ? (f64)visible - 0.5 : (f64)visible + 0.5);
    if (result != 0 && scale != NULL)
        *scale = values[6];
    if (rate != NULL)
        *rate = animation->curve_count <= 8 ? 1.0f : values[8];
    return result;
}

i32 __cdecl NuGCutLocatorIsVisble(CUT_LOCATOR_GENERIC_PC *locator, f32 frame, NUANIM_TIME2_PC *time, f32 *scale,
                                  f32 *rate) {
    NUANIMDATA2_PC *animation = (NUANIMDATA2_PC *)locator->animation;
    DWA_CURVE_CELL_PC *curves;
    i8 *types;
    i32 visible;
    volatile f32 evaluated;
    if (animation == NULL) {
        CUT_VISIBLE_LOCATOR_PC *constant = (CUT_VISIBLE_LOCATOR_PC *)locator;
        visible = constant->flags & 8;
        if (visible != 0 && scale != NULL)
            *scale = constant->scale;
        return visible;
    }
    if ((i32)animation->header.magic >= (i32)0x414e4934 && (i32)animation->header.magic <= (i32)0x414e4935)
        return NuGCutLocatorIsVisble_3(locator, frame, scale, rate);
    if (rate != NULL)
        *rate = 1.0f;
    curves = animation->curves;
    types = animation->curve_types;
    if (animation->curve_count == 4) {
        evaluated = types[3] == 0 ? curves[3].constant : NuAnimCurve2CalcValEx(&curves[3], time, types[3]);
        return (i32)evaluated;
    }
    evaluated = types[7] == 0 ? curves[7].constant : NuAnimCurve2CalcValEx(&curves[7], time, types[7]);
    visible = (i32)evaluated;
    if (visible != 0 && scale != NULL) {
        evaluated = types[6] == 0 ? curves[6].constant : NuAnimCurve2CalcValEx(&curves[6], time, types[6]);
        *scale = evaluated;
    }
    return visible;
}
