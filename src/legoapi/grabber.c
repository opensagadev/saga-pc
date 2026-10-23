#include "saga.h"

typedef struct grabber_progress_source_pc_s {
    u8 unknown_0000[0x484];
    f32 stored_tfactor;
    u8 unknown_0488[4];
    f32 stored_frame;
    u8 unknown_0490[4];
    f32 stored_parameter;
} GRABBER_PROGRESS_SOURCE_PC;
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_tfactor) == 0x484);
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_frame) == 0x48c);
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_parameter) == 0x494);

typedef struct world_grabber_progress_pc_s {
    u8 unknown_0000[0x46ec];
    GRABBER_PROGRESS_SOURCE_PC *grabber;
} WORLD_GRABBER_PROGRESS_PC;
STATIC_ASSERT(offsetof(WORLD_GRABBER_PROGRESS_PC, grabber) == 0x46ec);

typedef struct level_grabber_progress_pc_s {
    u8 unknown_0000[0x2810];
    f32 stored_frame;
    f32 stored_tfactor;
    f32 stored_parameter;
} LEVEL_GRABBER_PROGRESS_PC;
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_frame) == 0x2810);
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_tfactor) == 0x2814);
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_parameter) == 0x2818);

void __cdecl Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress) {
    if (world != NULL && progress != NULL) {
        GRABBER_PROGRESS_SOURCE_PC *grabber = ((WORLD_GRABBER_PROGRESS_PC *)world)->grabber;
        LEVEL_GRABBER_PROGRESS_PC *saved = (LEVEL_GRABBER_PROGRESS_PC *)progress;
        if (grabber != NULL) {
            saved->stored_frame = grabber->stored_frame;
            saved->stored_tfactor = grabber->stored_tfactor;
            saved->stored_parameter = grabber->stored_parameter;
        } else {
            saved->stored_tfactor = 2000000.0f;
        }
    }
}

typedef struct grabber_pc_s {
    NUMTX matrix;
    NUMTX grab_matrix;
    u8 unknown_080[0x440 - 0x80];
    NUMTX carried_matrix;
    NUVEC initial_position;
    NUVEC grab_position;
    NUVEC velocity;
    NUVEC target_velocity;
    CHARACTER_MODEL_LAYERS_PC *character_model;
    ANIMPACKET_PC animation;
    u8 unknown_4f8[4];
    NUSPECIALHANDLE special, shadow_special;
    GAMEOBJECT *victim;
    GIZOBSTACLE *action_switch;
    GIZOBSTACLE *direction_switches[4];
    GIZMOBLOWUP_PC *carried_blowup;
    f32 scale, speed, radius;
    f32 stuck_timer;
    f32 shadow_height;
    f32 carried_height_offset;
    u16 shadow_z_angle, shadow_x_angle;
    f32 floor_height;
    f32 animation_timer;
    f32 platform_contact_timer;
    u8 state;
    u8 flags;
    u16 yaw, target_yaw;
    i16 character_id, platform_id;
    u8 move_xy, invert_x;
    u8 unknown_564[4];
} GRABBER_PC;
STATIC_ASSERT(sizeof(GRABBER_PC) == 0x568);
STATIC_ASSERT(offsetof(GRABBER_PC, matrix) == 0);
STATIC_ASSERT(offsetof(GRABBER_PC, initial_position) == 0x480);
STATIC_ASSERT(offsetof(GRABBER_PC, character_model) == 0x4b0);
STATIC_ASSERT(offsetof(GRABBER_PC, special) == 0x4fc);
STATIC_ASSERT(offsetof(GRABBER_PC, shadow_special) == 0x508);
STATIC_ASSERT(offsetof(GRABBER_PC, scale) == 0x530);
STATIC_ASSERT(offsetof(GRABBER_PC, speed) == 0x534);
STATIC_ASSERT(offsetof(GRABBER_PC, radius) == 0x538);
STATIC_ASSERT(offsetof(GRABBER_PC, flags) == 0x559);
STATIC_ASSERT(offsetof(GRABBER_PC, character_id) == 0x55e);
STATIC_ASSERT(offsetof(GRABBER_PC, platform_id) == 0x560);
STATIC_ASSERT(offsetof(GRABBER_PC, move_xy) == 0x562);
STATIC_ASSERT(offsetof(GRABBER_PC, invert_x) == 0x563);
STATIC_ASSERT(offsetof(GRABBER_PC, grab_matrix) == 0x40);
STATIC_ASSERT(offsetof(GRABBER_PC, carried_matrix) == 0x440);
STATIC_ASSERT(offsetof(GRABBER_PC, grab_position) == 0x48c);
STATIC_ASSERT(offsetof(GRABBER_PC, velocity) == 0x498);
STATIC_ASSERT(offsetof(GRABBER_PC, target_velocity) == 0x4a4);
STATIC_ASSERT(offsetof(GRABBER_PC, animation) == 0x4b4);
STATIC_ASSERT(offsetof(GRABBER_PC, victim) == 0x514);
STATIC_ASSERT(offsetof(GRABBER_PC, action_switch) == 0x518);
STATIC_ASSERT(offsetof(GRABBER_PC, carried_blowup) == 0x52c);
STATIC_ASSERT(offsetof(GRABBER_PC, stuck_timer) == 0x53c);
STATIC_ASSERT(offsetof(GRABBER_PC, shadow_height) == 0x540);
STATIC_ASSERT(offsetof(GRABBER_PC, carried_height_offset) == 0x544);
STATIC_ASSERT(offsetof(GRABBER_PC, shadow_z_angle) == 0x548);
STATIC_ASSERT(offsetof(GRABBER_PC, floor_height) == 0x54c);
STATIC_ASSERT(offsetof(GRABBER_PC, animation_timer) == 0x550);
STATIC_ASSERT(offsetof(GRABBER_PC, platform_contact_timer) == 0x554);
STATIC_ASSERT(offsetof(GRABBER_PC, state) == 0x558);
STATIC_ASSERT(offsetof(GRABBER_PC, yaw) == 0x55a);
STATIC_ASSERT(offsetof(GRABBER_PC, target_yaw) == 0x55c);
typedef struct world_grabber_config_pc_s {
    u8 unknown_000[0x104];
    void *giz_buffer;
    u8 unknown_108[0x128 - 0x108];
    LEVELDATA_PC *current_level;
    u8 unknown_12c[0x13c - 0x12c];
    NUGSCN *scene;
    u8 unknown_140[0x46ec - 0x140];
    GRABBER_PC *grabber;
} WORLD_GRABBER_PC;
STATIC_ASSERT(offsetof(WORLD_GRABBER_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLD_GRABBER_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLD_GRABBER_PC, scene) == 0x13c);
STATIC_ASSERT(offsetof(WORLD_GRABBER_PC, grabber) == 0x46ec);
extern i16 GrabMachineIdPc, GrabMagnetIdPc, RobotBaseIdPc;
GRABBER_PC *GrabCurrentPc = 0;
static void __cdecl Grab_radius(NUFPAR_PC *parser) {
    GrabCurrentPc->radius = NuFParGetFloat(parser);
}
static void __cdecl Grab_speed(NUFPAR_PC *parser) {
    GrabCurrentPc->speed = NuFParGetFloat(parser);
}
static void __cdecl Grab_rotate(NUFPAR_PC *parser) {
    i32 rotate = NuFParGetInt(parser) & 1;
    GrabCurrentPc->flags = (u8)((GrabCurrentPc->flags & ~8) | (rotate << 3));
}
static void __cdecl Grab_scale(NUFPAR_PC *parser) {
    GrabCurrentPc->scale = NuFParGetFloat(parser);
}
static void __cdecl Grab_move_xy(NUFPAR_PC *parser) {
    if (NuFParGetInt(parser) != 0)
        GrabCurrentPc->move_xy = 1;
}
static void __cdecl Grab_invert_x(NUFPAR_PC *parser) {
    GrabCurrentPc->invert_x = 1;
}
static void __cdecl Grab_type(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0)
        return;
    if (NuStrICmp(parser->word_buffer_ptr, "GRABMAGNET") == 0)
        GrabCurrentPc->character_id = GrabMagnetIdPc;
    else if (NuStrICmp(parser->word_buffer_ptr, "ROBOTBASE") == 0)
        GrabCurrentPc->character_id = RobotBaseIdPc;
    else
        GrabCurrentPc->character_id = GrabMachineIdPc;
}
static void __cdecl Grab_shadow(NUFPAR_PC *parser) {
    GrabCurrentPc->flags |= 0x10;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0)
        GrabCurrentPc->flags &= ~0x10;
}
typedef struct grab_config_data_pc_s {
    f32 level_y_offset;
    NUFPCOMJMP keywords[9];
} GRAB_CONFIG_DATA_PC;
STATIC_ASSERT(offsetof(GRAB_CONFIG_DATA_PC, keywords) == 4);
STATIC_ASSERT(sizeof(GRAB_CONFIG_DATA_PC) == 76);
static GRAB_CONFIG_DATA_PC GrabConfigDataPc = {-0.3f,
                                               {{"radius", Grab_radius},
                                                {"speed", Grab_speed},
                                                {"rotate", Grab_rotate},
                                                {"scale", Grab_scale},
                                                {"move_xy", Grab_move_xy},
                                                {"type", Grab_type},
                                                {"invert_x", Grab_invert_x},
                                                {"shadow", Grab_shadow},
                                                {NULL, NULL}}};
void __cdecl Grabber_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_GRABBER_PC *world = (WORLD_GRABBER_PC *)world_ptr;
    NUSPECIALHANDLE special;
    NUFPAR_PC *parser;
    GRABBER_PC *grabber;
    u8 active = 0;
    if (VehicleArea != 0 || (world->current_level->flags & 0xe0) != 0 || world->grabber != NULL)
        return;
    if (NuSpecialFind(world->scene, &special, "grabber", 1) == 0)
        return;
    grabber = (GRABBER_PC *)(((u32)world->giz_buffer + 15) & ~15u);
    world->grabber = grabber;
    world->giz_buffer = grabber + 1;
    GrabCurrentPc = grabber;
    grabber->special = special;
    parser = NuFParCreateMem("grabbers", config, 0xffff);
    if (parser == NULL)
        return;
    NuFParPushCom(parser, GrabConfigDataPc.keywords);
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "grabber_start") == 0) {
                GrabCurrentPc->move_xy = 0;
                GrabCurrentPc->radius = 0.5f;
                GrabCurrentPc->speed = 1.0f;
                GrabCurrentPc->flags &= ~8;
                GrabCurrentPc->scale = 1.0f;
                GrabCurrentPc->invert_x = 0;
                GrabCurrentPc->flags |= 0x10;
                GrabCurrentPc->character_id = GrabMachineIdPc;
                active = 1;
            } else if (NuStrICmp(parser->word_buffer_ptr, "grabber_end") == 0)
                break;
            else if (active != 0)
                NuFParInterpretWord(parser);
        }
    }
    NuFParDestroy(parser);
    if (GrabCurrentPc->character_id != -1)
        GrabCurrentPc->character_model = APICharacterLoaded(GrabCurrentPc->character_id);
    else
        GrabCurrentPc->character_model = NULL;
    if (GrabCurrentPc->character_model != NULL)
        NuSpecialSetVisibility(&GrabCurrentPc->special, 0);
    else {
        NuSpecialSetVisibility(&GrabCurrentPc->special, 1);
        GrabCurrentPc->character_id = -1;
    }
    GrabCurrentPc->matrix = *NuSpecialGetDrawMtx(&GrabCurrentPc->special);
    if (world->current_level == DEATHSTARRESCUEA_LDATA)
        GrabCurrentPc->matrix.m31 += GrabConfigDataPc.level_y_offset;
    PCByteCopy_candidate(&GrabCurrentPc->initial_position, &GrabCurrentPc->matrix.m30, sizeof(NUVEC));
    NuSpecialFind(world->scene, &GrabCurrentPc->shadow_special, "grabber_shadow", 1);
    world->grabber->platform_id = -1;
}

typedef struct grabber_hg_view_pc_s {
    u8 unknown_000[0x19c];
    i32 shadow_surface_points;
    u8 suppress_shadow_points;
} GRABBER_HG_VIEW_PC;
STATIC_ASSERT(offsetof(GRABBER_HG_VIEW_PC, suppress_shadow_points) == 0x1a0);

typedef struct grabber_antinode_view_pc_s {
    u8 unknown_00[0x50];
    i64 object_mask;
} GRABBER_ANTINODE_VIEW_PC;
STATIC_ASSERT(offsetof(GRABBER_ANTINODE_VIEW_PC, object_mask) == 0x50);

GIZMOPICKUP_PC *__cdecl GizmoPickup_InBox(WORLDINFO *world, i32 type_index, NUVEC *minimum, NUVEC *maximum);
void __cdecl PartKill_Grabber(PART_PC *part, i32 unused1);

/* Retail passes the last NewRumbleAllPlayers argument as an integer. */
#define GRABBER_RUMBLE(strength, duration, frames)                                                                     \
    ((void(__cdecl *)(f32, f32, i32, i32))NewRumbleAllPlayers)(strength, duration, frames, 0)

static void GrabberMtxSetRotationZ(NUMTX *m, NUANG a) {
    f32 cosz;
    f32 sinz;

    cosz = NU_COS_LUT(a);
    sinz = NU_SIN_LUT(a);
    m->m00 = cosz;
    m->m01 = sinz;
    m->m02 = 0.0f;
    m->m03 = 0.0f;
    m->m10 = -sinz;
    m->m11 = cosz;
    m->m12 = 0.0f;
    m->m13 = 0.0f;
    m->m20 = ANI3_EVAL_IDENTITY_MATRIX.m20;
    m->m21 = ANI3_EVAL_IDENTITY_MATRIX.m21;
    m->m22 = ANI3_EVAL_IDENTITY_MATRIX.m22;
    m->m23 = ANI3_EVAL_IDENTITY_MATRIX.m23;
    m->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    m->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    m->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    m->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void GrabberMtxRotateX(NUMTX *m, NUANG a) {
    f32 cosx;
    f32 sinx;
    f32 m01;
    f32 m11;
    f32 m21;
    f32 m31;

    cosx = NU_COS_LUT(a);
    sinx = NU_SIN_LUT(a);
    m01 = m->m01;
    m11 = m->m11;
    m21 = m->m21;
    m31 = m->m31;

    m->m01 = m01 * cosx - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
}

static i32 GrabberFindShadow(NUVEC *position, GRABBER_PC *grabber) {
    NUVEC probe;
    f32 height;
    i32 surface;

    probe.x = position->x;
    probe.y = position->y + 0.25f;
    probe.z = position->z;
    if (WORLD->current_level != CLOUDCITYTRAPA_LDATA)
        NewTerrPlatformsOff();
    height = GameShadow(NULL, &probe, 5.0f, -1);
    if (height != 2000000.0f) {
        surface = ShadowInfo();
        if ((u32)surface <= 31 && (TerSurface[surface].flags & 0x200) != 0) {
            grabber->shadow_height = height;
            FindAnglesZX(&ShadNorm, &grabber->shadow_z_angle, &grabber->shadow_x_angle);
            if ((TerSurface[surface].flags & 2) != 0)
                grabber->floor_height = height;
            else
                grabber->floor_height = 2000000.0f;
            return 1;
        }
    }
    return 0;
}

static void GrabberGrabVictim(GAMEOBJECT *victim) {
    victim->character_context = 0x2f;
    victim->field_7aa = 0x68;
    if (victim->character_model->model_data_b[104] == NULL)
        victim->field_7aa = 5;
    GRABBER_RUMBLE(0.0f, 0.1f, 0);
    victim->context_timer = 0.0f;
    victim->context_duration = 0.75f;
    victim->external_force = victim->start_position;
    victim->field_7b3 = 0;
    Hint_SetComplete(0x28b);
}

NUVEC *__cdecl Grabber_GetGrabPos(void *context, NUMTX *matrix) {
    GRABBER_PC *grabber = (GRABBER_PC *)context;
    if (grabber->character_model != NULL && (grabber->flags & 1) != 0 &&
        grabber->character_model->points_of_interest[0] != NULL) {
        if (matrix != NULL)
            *matrix = grabber->grab_matrix;
        return (NUVEC *)&grabber->grab_matrix.m30;
    }
    if (matrix != NULL)
        *matrix = NuMtxSetIdentityTemplate;
    return &grabber->grab_position;
}

static i32 __cdecl IsGrabbable(GAMEOBJECT *object) {
    return (object->character_data->game_character->flags_090 & 0x8000) == 0;
}

extern i32 CharacterBlobShadowsOnPc;
i32 __cdecl MatrixReflectionVU0_AXISY(NUMTX *matrix, f32 plane, f32 override_plane, NUMTX *result);

void __cdecl Grabber_Draw(WORLDINFO *world) {
    GRABBER_PC *grabber;
    NUVEC *grab_position;
    NUMTX matrix;
    NUMTX reflection;
    NUVEC position;
    RTL_DATA_PC lights;
    NUMTX joints[256];
    f32 distance;

    grabber = (GRABBER_PC *)world->grabber;
    grab_position = NULL;
    if (grabber == NULL)
        return;
    if (grabber->character_model != NULL) {
        grab_position = Grabber_GetGrabPos(grabber, NULL);
        rtlResetEx(&lights, 1);
        rtlApplySetScale(WORLD->rtl_set, &lights, grab_position, NULL, -1, 1.0f);
        rtlSetLights(&lights);
    } else if (NuSpecialGetVisibilityFn(&grabber->special) == 0)
        return;
    matrix = *NuSpecialGetDrawMtx(&grabber->special);
    if (world->current_level == ObstacleReturnLevelPc) {
        distance = NuVecXZDist(&GameCam->position, (NUVEC *)&matrix.m30, NULL);
        if (distance < 2.0f) {
            matrix.m31 +=
                (1.0f - (NU_SIN_LUT((i32)((1.0f - distance * 0.5f) * 32768.0f + 16384.0f)) + 1.0f) * 0.5f) * 1.25f;
        }
    }
    if (grabber->shadow_height != 2000000.0f) {
        if ((grabber->flags & 0x10) != 0 && CharacterBlobShadowsOnPc != 0 &&
            (grabber->character_model == NULL ||
             ((GRABBER_HG_VIEW_PC *)grabber->character_model->hierarchy)->shadow_surface_points == 0)) {
            position.x = grabber->grab_position.x;
            position.y = grabber->shadow_height + 0.005f;
            if (WORLD->current_level == CLOUDCITYTRAPA_LDATA)
                position.y += 0.005f;
            position.z = grabber->grab_position.z;
            if (NuSpecialExistsFn(&grabber->shadow_special) != 0) {
                GrabberMtxSetRotationZ(&reflection, grabber->shadow_x_angle);
                GrabberMtxRotateX(&reflection, grabber->shadow_z_angle);
                NuMtxTranslate(&reflection, &position);
                NuSpecialDrawAt(&grabber->shadow_special, &reflection);
            } else
                NuRndrAddShadow(&position, grabber->radius, 0x80, grabber->shadow_z_angle, 0, grabber->shadow_x_angle);
        }
        if (grabber->floor_height != 2000000.0f)
            MatrixReflectionVU0_AXISY(&matrix, grabber->floor_height, WORLD->current_level->reflection_y, &reflection);
    }
    if (grabber->character_model != NULL) {
        grabber->flags &= ~1;
        ModelAnimLayerListDispatchPc = GCDataList[grabber->character_model->model_id].make_layer_list;
        if ((grabber->flags & 0x10) != 0) {
            ((GRABBER_HG_VIEW_PC *)grabber->character_model->hierarchy)->suppress_shadow_points = 0;
            EnableShadowMapRendering(0);
        } else {
            ((GRABBER_HG_VIEW_PC *)grabber->character_model->hierarchy)->suppress_shadow_points = 1;
            ResetShadowMapRendering();
        }
        if (APIDrawCharacterModel(grabber->character_model,
                                  (APICHARACTERDATA *)&CDataList[grabber->character_model->model_id],
                                  &grabber->animation, &matrix, NULL,
                                  grabber->floor_height != 2000000.0f ? &reflection : NULL, NULL, &grabber->grab_matrix,
                                  NULL, 0xffff, NULL, 0, Paused, FRAMETIME, joints, NULL, WORLD->game_debris_sys) != 0)
            grabber->flags |= 1;
        if ((grabber->flags & 0x10) != 0)
            ResetShadowMapRendering();
        ((GRABBER_HG_VIEW_PC *)grabber->character_model->hierarchy)->suppress_shadow_points = 0;
    } else if (grabber->floor_height != 2000000.0f)
        NuSpecialDrawAt(&grabber->special, &reflection);
    if (grabber->carried_blowup != NULL && grab_position != NULL) {
        matrix = grabber->carried_matrix;
        *(NUVEC *)&matrix.m30 = *grab_position;
        matrix.m31 += grabber->carried_height_offset;
        NuSpecialDrawAt(&grabber->carried_blowup->type->animated_special, &matrix);
        if (grabber->floor_height != 2000000.0f &&
            MatrixReflection(&matrix, 2, grabber->floor_height, WORLD->current_level->reflection_y, &reflection) != 0)
            NuSpecialDrawAt(&grabber->carried_blowup->type->animated_special, &reflection);
    }
}

static void Grabber_SetVictimPos(GRABBER_PC *grabber) {
    GAMEOBJECT *victim;
    NUVEC position;
    NUVEC start;
    f32 fraction;

    victim = grabber->victim;
    if (victim == NULL)
        return;
    position = *Grabber_GetGrabPos(grabber, NULL);
    if (grabber->character_id == GrabMagnetIdPc) {
        fraction = victim->context_timer / victim->context_duration;
        start = victim->external_force;
        if (fraction > 1.0f)
            fraction = 1.0f;
        grabber->victim->position.x = start.x + (position.x - start.x) * fraction;
        grabber->victim->position.y = start.y + (position.y - start.y) * fraction;
        grabber->victim->position.z = start.z + (position.z - start.z) * fraction;
        victim = grabber->victim;
        victim->position.x -= NU_SIN_LUT(victim->field_0x276) * victim->bounds_half_height * fraction;
        victim = grabber->victim;
        victim->position.z -= NU_COS_LUT(victim->field_0x276) * victim->bounds_half_height * fraction;
        if (grabber->victim->field_7b3 == 0 && fraction == 1.0f) {
            PlaySfx("imp_C3PO_magnet", &position);
            grabber->victim->field_7b3 = 1;
        }
    } else if (victim->field_7aa == 5) {
        victim->position.x = position.x - NU_SIN_LUT(victim->field_0x276) * victim->bounds_half_height;
        grabber->victim->position.y = position.y;
        victim = grabber->victim;
        victim->position.z = position.z - NU_COS_LUT(victim->field_0x276) * victim->bounds_half_height;
    } else
        victim->position = position;
    grabber->victim->flags_ef0 &= ~0x2000000;
    grabber->victim->ai_update_position = grabber->victim->position;
}

#define GRABBER_FIND_SWITCH(field, name)                                                                               \
    gizmo = (GIZOBSTACLE **)GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, name);                            \
    if (gizmo != NULL) {                                                                                               \
        GrabCurrentPc->field = *gizmo;                                                                                 \
        if (GrabCurrentPc->field != NULL && GrabCurrentPc->field->anim_set == NULL)                                    \
            GrabCurrentPc->field = NULL;                                                                               \
    } else                                                                                                             \
        GrabCurrentPc->field = NULL

void __cdecl Grabber_Reset(WORLDINFO *world) {
    GRABBER_PC *grabber;
    GIZOBSTACLE **gizmo;

    grabber = (GRABBER_PC *)world->grabber;
    if (grabber == NULL)
        return;
    GRABBER_FIND_SWITCH(action_switch, "pad_drop");
    GRABBER_FIND_SWITCH(direction_switches[0], "pad_n");
    GRABBER_FIND_SWITCH(direction_switches[1], "pad_s");
    GRABBER_FIND_SWITCH(direction_switches[2], "pad_e");
    GRABBER_FIND_SWITCH(direction_switches[3], "pad_w");
    grabber->grab_position.y = grabber->initial_position.y;
    if (world->level_progress != NULL &&
        ((LEVEL_GRABBER_PROGRESS_PC *)world->level_progress)->stored_tfactor != 2000000.0f) {
        grabber->grab_position.x = ((LEVEL_GRABBER_PROGRESS_PC *)world->level_progress)->stored_frame;
        grabber->grab_position.z = ((LEVEL_GRABBER_PROGRESS_PC *)world->level_progress)->stored_parameter;
    } else {
        grabber->grab_position.x = grabber->initial_position.x;
        grabber->grab_position.z = grabber->initial_position.z;
    }
    grabber->floor_height = grabber->shadow_height = 2000000.0f;
    ResetAnimPacket(&grabber->animation, -1);
    grabber->state = 0;
    if (grabber->victim != NULL) {
        Grabber_SetVictimPos(grabber);
        grabber->victim->character_context = -1;
    }
    grabber->victim = NULL;
    grabber->carried_blowup = NULL;
    GrabberFindShadow(&grabber->grab_position, grabber);
    grabber->flags &= 0xfa;
    grabber->target_velocity.x = grabber->target_velocity.y = grabber->target_velocity.z = 0.0f;
    grabber->target_yaw = 0;
    grabber->yaw = 0;
    grabber->platform_contact_timer = 0.0f;
    grabber->velocity = grabber->target_velocity;
    if (world->terrain != NULL)
        grabber->platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&grabber->special));
}

void __cdecl Grabber_Drop(GRABBER_PC *grabber, NUVEC *previous) {
    ADDPART_PC params;
    NUMTX matrix;
    NUVEC velocity;
    NUVEC centre;
    PART_PC *part;

    grabber->flags |= 2;
    if (grabber->victim != NULL) {
        if (grabber->character_id == GrabMagnetIdPc && grabber->victim->field_7b3 != 0)
            PlaySfx("imp_C3PO_magnet_drop", Grabber_GetGrabPos(grabber, NULL));
        Grabber_SetVictimPos(grabber);
        grabber->victim->velocity.x = (grabber->grab_position.x - previous->x) / FRAMETIME * 0.5f;
        if (grabber->character_id == GrabMagnetIdPc)
            grabber->victim->velocity.y = -1.0f;
        grabber->victim->velocity.z = (grabber->grab_position.z - previous->z) / FRAMETIME * 0.5f;
        grabber->victim->character_context = -1;
        grabber->victim = NULL;
    } else if (grabber->carried_blowup != NULL) {
        params = Default_ADDPART;
        matrix = grabber->carried_matrix;
        *(NUVEC *)&matrix.m30 = *Grabber_GetGrabPos(grabber, NULL);
        matrix.m31 += grabber->carried_height_offset;
        params.matrix = &matrix;
        velocity.x = (grabber->grab_position.x - previous->x) / FRAMETIME * 0.5f;
        velocity.y = -0.1f;
        velocity.z = (grabber->grab_position.z - previous->z) / FRAMETIME * 0.5f;
        params.velocity = &velocity;
        NuSpecialGetRadius(&grabber->carried_blowup->type->animated_special, &centre, &params.field_14);
        params.field_18 = params.field_14;
        params.gravity = -6.0f;
        params.special = &grabber->carried_blowup->type->animated_special;
        params.time_step = FRAMETIME;
        params.field_a4 = 3.0f;
        params.flags = 0x311;
        params.kill_fn = PartKill_Grabber;
        part = AddPart(&params);
        if (part != NULL)
            part->field_210 = (u32)grabber->carried_blowup;
        grabber->carried_blowup = NULL;
    }
}

#define GRABBER_SWITCH_ON(obstacle)                                                                                    \
    ((obstacle) != NULL && ((obstacle)->progress_flags & 2) != 0 && (obstacle)->anim_set->state == 2)

void __cdecl Grabber_Update(WORLDINFO *world) {
    GRABBER_PC *grabber;
    GAMEOBJECT *control;
    GAMEOBJECT *object;
    GAMEPAD_PC *pad;
    GIZMOPICKUP_PC *pickup;
    AIANTINODE_PC *node;
    NUVEC *position;
    NUVEC next;
    NUVEC previous;
    NUVEC low;
    NUVEC high;
    NUVEC scale;
    NUMTX matrix;
    f32 *frame;
    f32 value;
    i32 moved;
    i32 minus;
    i32 plus;
    i32 found;
    i32 mask;
    i32 i;
    u16 angle;

    pad = NULL;
    control = NULL;
    grabber = (GRABBER_PC *)world->grabber;
    if (grabber == NULL)
        return;
    if (grabber->victim != NULL && ((grabber->victim->flags_1fc_word & 1) == 0 ||
                                    (grabber->victim->flags_1fc_word & 0x1000) == 0 || grabber->victim->dead != 0))
        grabber->victim = NULL;
    if (grabber->platform_contact_timer > 0.0f)
        grabber->platform_contact_timer -= FRAMETIME;
    next = grabber->grab_position;
    grabber->target_velocity.x = grabber->target_velocity.y = grabber->target_velocity.z = 0.0f;
    previous = grabber->grab_position;
    moved = 0;
    control = FindGameObject(id_GRABCONTROL, 0, 0, 1, 0);
    if (id_GRABR2CONTROL != -1 && control == NULL && StartupGameFlag1 != 0)
        FindGameObject(id_GRABR2CONTROL, 0, 0, 1, 0);
    if (control == NULL) {
        if (StartupGameFlag1 == 0)
            Techno_FindOperator(&grabber->special, &pad, &control);
    } else
        pad = control->pad;
    if (control != NULL && (control->takeover_source != NULL || StartupGameFlag1 == 0) &&
        (control->flags_1fc & 0x80) != 0) {
        if (pad->input_magnitude > 0.0f) {
            if (GrabCurrentPc->move_xy != 0) {
                grabber->target_velocity.x = 0.0f;
                grabber->target_velocity.y = pad->input_direction_x * grabber->speed;
                if (GrabCurrentPc->invert_x != 0)
                    grabber->target_velocity.z = pad->input_direction_z * grabber->speed * -1.0f;
                else
                    grabber->target_velocity.z = pad->input_direction_z * grabber->speed;
            } else {
                angle = GamePad_InputAngle(control, pad);
                if (GrabCurrentPc->invert_x != 0)
                    grabber->target_velocity.x = NU_SIN_LUT(angle) * grabber->speed * -1.0f;
                else
                    grabber->target_velocity.x = NU_SIN_LUT(angle) * grabber->speed;
                grabber->target_velocity.z = NU_COS_LUT(angle) * grabber->speed;
            }
        }
    } else {
        minus = GRABBER_SWITCH_ON(grabber->direction_switches[0]);
        plus = GRABBER_SWITCH_ON(grabber->direction_switches[1]);
        if (minus != 0) {
            if (plus == 0)
                grabber->target_velocity.x = -grabber->speed;
        } else if (plus != 0)
            grabber->target_velocity.x = grabber->speed;
        plus = GRABBER_SWITCH_ON(grabber->direction_switches[2]);
        minus = GRABBER_SWITCH_ON(grabber->direction_switches[3]);
        if (plus != 0) {
            if (minus == 0)
                grabber->target_velocity.z = grabber->speed;
        } else if (minus != 0)
            grabber->target_velocity.z = -grabber->speed;
        if (NuSpecialGetVisibilityFn(&grabber->special) == 0)
            grabber->target_velocity.x = grabber->target_velocity.z = 0.0f;
    }
    grabber->velocity.x = SeekValF(grabber->velocity.x, grabber->target_velocity.x, 5.0f);
    grabber->velocity.y = SeekValF(grabber->velocity.y, grabber->target_velocity.y, 5.0f);
    grabber->velocity.z = SeekValF(grabber->velocity.z, grabber->target_velocity.z, 5.0f);
    if ((grabber->flags & 8) != 0 && (grabber->target_velocity.x != 0.0f || grabber->target_velocity.z != 0.0f))
        grabber->target_yaw = NuAtan2D(grabber->target_velocity.x, grabber->target_velocity.z);
    next.x += FRAMETIME * grabber->velocity.x;
    next.y += grabber->velocity.y * FRAMETIME;
    next.z += FRAMETIME * grabber->velocity.z;
    if (world->current_level == DEATHSTARESCAPEB_LDATA) {
        if (next.z >= 11.84f) {
            next.z = 11.84f;
            grabber->velocity.z = grabber->target_velocity.z = 0.0f;
        } else if (next.z < 10.125f) {
            next.z = 10.125f;
            grabber->velocity.z = grabber->target_velocity.z = 0.0f;
        }
        if (next.y < -4.85f) {
            next.y = -4.85f;
            grabber->velocity.y = grabber->target_velocity.y = 0.0f;
        } else if (next.y > -3.9f) {
            next.y = -3.9f;
            grabber->velocity.y = grabber->target_velocity.y = 0.0f;
        }
    } else if (world->current_level == JABBASPALACEB_LDATA) {
        value = NuVecMag(&grabber->velocity) / grabber->speed * 0.25f + 0.5f;
        if (value > 1.0f)
            value = 1.0f;
        PlaySfxAndSetPitch("env_hover_box_lp", &next, value);
        next.y = NU_SIN_LUT((u16)(NuFmod(GameTimer.time_elapsed, 2.0f) * 0.5f * 65536.0f)) * 0.0125f +
                 grabber->initial_position.y;
        grabber->velocity.y = grabber->target_velocity.y = 0.0f;
        if (grabber->platform_contact_timer > 0.0f)
            next.y -=
                (1.0f - NU_SIN_LUT((i32)(grabber->platform_contact_timer * 4.0f * 65536.0f + 16384.0f))) * 0.5f * 0.02f;
    }
    if (next.x != grabber->grab_position.x || next.y != grabber->grab_position.y ||
        next.z != grabber->grab_position.z) {
        if (world->current_level != JABBASPALACEB_LDATA &&
            (grabber->target_velocity.x != 0.0f || grabber->target_velocity.y != 0.0f ||
             grabber->target_velocity.z != 0.0f)) {
            if (world->current_level == CLOUDCITYTRAPA_LDATA)
                PlaySfx("CarbonFreezeCraneLp", &grabber->grab_position);
            else
                PlaySfx("env_crane_mvt_lp", &grabber->grab_position);
        }
        if (GrabberFindShadow(&next, grabber) != 0) {
            moved = 1;
            grabber->grab_position.x = next.x;
            grabber->grab_position.y = next.y;
            grabber->grab_position.z = next.z;
            if ((grabber->target_velocity.x != 0.0f || grabber->target_velocity.y != 0.0f ||
                 grabber->target_velocity.z != 0.0f) &&
                Cheat_PowerUpActive(-1) == 0)
                ConstantRumble(
                    NULL, (f32)qrand() * PlayerRandomScalePc * 0.4f * (NuVecMag(&grabber->velocity) / grabber->speed),
                    0.15f);
        } else {
            grabber->target_velocity = v000;
            grabber->velocity = grabber->target_velocity;
            if (world->current_level == JABBASPALACEB_LDATA)
                grabber->grab_position.y = next.y;
        }
    }
    if ((grabber->flags & 8) != 0)
        grabber->yaw = SeekRot(grabber->yaw, grabber->target_yaw, 3.0f);
    matrix = grabber->matrix;
    if ((grabber->flags & 8) != 0)
        NuMtxPreRotateY(&matrix, grabber->yaw);
    if (grabber->scale != 1.0f) {
        scale.x = scale.y = scale.z = grabber->scale;
        NuMtxPreScale(&matrix, &scale);
    }
    *(NUVEC *)&matrix.m30 = grabber->grab_position;
    NuSpecialSetDrawMtx(&grabber->special, &matrix);
    NuSpecialUpdate(&grabber->special);
    switch (grabber->state) {
        case 1:
            frame = AnimPlaying(&grabber->animation, 0x66, 1, 0);
            if (frame == NULL)
                break;
            value = grabber->animation_timer - FRAMETIME;
            grabber->animation_timer = value;
            if (value <= 0.0f) {
                if (grabber->victim != NULL || grabber->carried_blowup != NULL) {
                    grabber->stuck_timer = 0.0f;
                    grabber->state = 2;
                } else
                    grabber->state = 0;
                break;
            }
            if ((grabber->flags & 2) != 0)
                break;
            value = AnimListFrame(grabber->character_model, 0x66, 0);
            if (!(value > 0.0f) || !(*frame >= value))
                break;
            grabber->flags |= 2;
            position = Grabber_GetGrabPos(grabber, NULL);
            low.x = position->x - 0.5f;
            high.x = position->x + 0.5f;
            low.y = position->y - 0.5f;
            high.y = position->y + 0.5f;
            low.z = position->z - 0.5f;
            high.z = position->z + 0.5f;
            pickup = GizmoPickup_InBox(world, 4, &low, &high);
            if (pickup != NULL) {
                pickup->flags = (pickup->flags & 0xf9) | 8;
                if ((pickup->flags & 0x40) != 0)
                    Pup_CollectCoin(world, pickup, 2, control, 1);
                else
                    CollectMinikit(&pickup->position, "m_pup1", 1);
            } else {
                grabber->victim =
                    FindNearestGameObject(position, NULL, 0, 0.5f, 0.0f, -1, -1, -1, NULL, 0, IsGrabbable);
                if (grabber->victim != NULL) {
                    GrabberGrabVictim(grabber->victim);
                    goto grabbed;
                }
                grabber->carried_blowup = FindNearestGizmoBlowUp(world, position, 0.25f);
                if (grabber->carried_blowup == NULL)
                    goto lifted;
                grabber->carried_matrix = grabber->carried_blowup->transform;
                GizmoBlowupBlowup(grabber->carried_blowup, 0, -1, -1, NULL, 1);
                grabber->carried_height_offset =
                    grabber->carried_blowup->transform.m31 - grabber->carried_blowup->mid_position.y;
            }
            GRABBER_RUMBLE(0.0f, 0.1f, 0);
            Hint_SetComplete(0x28b);
        grabbed:
            PlaySfx("env_grabber_down", position);
        lifted:
            PlaySfx("env_grabber_up", position);
            break;
        case 3:
            frame = AnimPlaying(&grabber->animation, 0x67, 1, 0);
            if (frame == NULL)
                break;
            value = grabber->animation_timer - FRAMETIME;
            grabber->animation_timer = value;
            if (value <= 0.0f) {
                grabber->state = 0;
                if ((grabber->flags & 2) != 0)
                    break;
            } else {
                if ((grabber->flags & 2) != 0)
                    break;
                value = AnimListFrame(grabber->character_model, 0x67, 0);
                if (!(value > 0.0f) || !(*frame >= value))
                    break;
            }
            Grabber_Drop(grabber, &previous);
            break;
        case 2:
            if (moved == 0 && grabber->victim != NULL && (grabber->victim->flags_1fc & 0x80) != 0) {
                grabber->stuck_timer += FRAMETIME;
                if (!(grabber->stuck_timer >= 3.0f))
                    goto waiting;
                goto release;
            }
            grabber->stuck_timer = 0.0f;
        waiting:
            if (control != NULL) {
                if (grabber->victim == NULL && grabber->carried_blowup == NULL) {
                    grabber->state = 0;
                    break;
                }
                if ((pad->buttons_pressed & (GAMEPAD_ACTION | GAMEPAD_SPECIAL)) == 0)
                    break;
            } else {
                if ((grabber->flags & 4) != 0 && grabber->action_switch != NULL &&
                    (grabber->action_switch->progress_flags & 2) != 0 &&
                    GameAnimSet_IsAnimationReset(grabber->action_switch->anim_set) != 0)
                    grabber->flags &= ~4;
                if (grabber->victim == NULL && grabber->carried_blowup == NULL) {
                    grabber->state = 0;
                    break;
                }
                if ((grabber->flags & 4) != 0 || grabber->action_switch == NULL ||
                    (grabber->action_switch->progress_flags & 2) == 0 ||
                    (grabber->action_switch->anim_set->flags & 1) != 0)
                    break;
            }
        release:
            if (grabber->character_model != NULL && grabber->character_model->model_data_b[103] != NULL) {
                grabber->state = 3;
                grabber->animation_timer = AnimDuration(grabber->character_model->model_id, 0x67, 0.0f, 0.0f, 1);
                grabber->flags &= ~2;
                if (control == NULL)
                    grabber->flags |= 4;
                GRABBER_RUMBLE(0.0f, 0.0f, 2);
            } else {
                Grabber_Drop(grabber, &previous);
                if (control == NULL)
                    grabber->flags |= 4;
            }
            PlaySfx("env_grabber_down", Grabber_GetGrabPos(grabber, NULL));
            break;
        case 0:
            if (pad != NULL) {
                if ((pad->buttons_pressed & (GAMEPAD_ACTION | GAMEPAD_SPECIAL)) == 0)
                    break;
            } else {
                if ((grabber->flags & 4) != 0 && grabber->action_switch != NULL &&
                    (grabber->action_switch->progress_flags & 2) != 0 &&
                    GameAnimSet_IsAnimationReset(grabber->action_switch->anim_set) != 0)
                    grabber->flags &= ~4;
                if ((grabber->flags & 4) != 0)
                    break;
                if (grabber->action_switch == NULL || (grabber->action_switch->progress_flags & 2) == 0 ||
                    (grabber->action_switch->anim_set->flags & 1) != 0)
                    break;
            }
            if (grabber->character_model != NULL && grabber->character_model->model_data_b[102] != NULL) {
                grabber->state = 1;
                grabber->animation_timer = AnimDuration(grabber->character_model->model_id, 0x66, 0.0f, 0.0f, 1);
                grabber->flags &= ~2;
                if (control == NULL)
                    grabber->flags |= 4;
                GRABBER_RUMBLE(0.0f, 0.0f, 2);
            } else if (grabber->character_id == GrabMagnetIdPc) {
                grabber->victim = FindNearestGameObject(Grabber_GetGrabPos(grabber, NULL), NULL, 0x10, 0.5f, 0.0f, -1,
                                                        -1, -1, NULL, 1, IsGrabbable);
                if (grabber->victim != NULL) {
                    GrabberGrabVictim(grabber->victim);
                    grabber->stuck_timer = 0.0f;
                    grabber->state = 2;
                    if (control == NULL)
                        grabber->flags |= 4;
                }
            }
            PlaySfx("env_grabber_pickup", Grabber_GetGrabPos(grabber, NULL));
            break;
    }
    if (grabber->character_model != NULL) {
        grabber->animation.previous_animation = grabber->animation.animation_index;
        switch (grabber->state) {
            case 1:
                grabber->animation.requested_animation = 0x66;
                break;
            case 2:
                grabber->animation.requested_animation = 0xf;
                break;
            case 3:
                grabber->animation.requested_animation = 0x67;
                break;
            default:
                grabber->animation.requested_animation = 1;
                break;
        }
        UpdateAnimPacket(grabber->character_model, &grabber->animation, FRAMETIME * 30.0f, grabber->speed, FRAMETIME,
                         0.0f);
    }
    if (grabber->victim != NULL) {
        grabber->victim->context_timer += FRAMETIME;
        Grabber_SetVictimPos(grabber);
        grabber->victim->velocity.z = 0.0f;
        grabber->victim->velocity.x = 0.0f;
        grabber->victim->velocity.y = -0.1f;
        grabber->victim->field_281 = 0;
    }
    if (grabber->character_id == GrabMachineIdPc) {
        mask = 0;
        found = 0;
        object = Obj;
        for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
            if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0)
                continue;
            if ((object->character_data->game_character->flags_090 & 0x8000) == 0)
                mask |= (i32)((i64)1 << object->hostility_group);
            else
                found = 1;
        }
        if (found != 0) {
            node = AIAntinodeCreateSingleFrame(&grabber->grab_position, 0.25f);
            if (node != NULL) {
                node->min_y = node->position.y - 4.0f;
                ((GRABBER_ANTINODE_VIEW_PC *)node)->object_mask = mask;
            }
        }
    }
}
