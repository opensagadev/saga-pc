#include "saga.h"

i32 __cdecl GizPanel_GetNumOutputs(void *unused) {
    (void)unused;
    return 1;
}

extern const f32 PlayerPanelVariantZeroX, PlayerPanelVariantZeroZ, PlayerPanelVariantOneX, PlayerPanelVariantOneZ;

typedef struct panel_movement_pc_s {
    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 y_rotation;
    u8 unknown_5e[3];
    u8 model_variant, flags, draw_flags;
    NUVEC floor_position;
    NUVEC target_offset;
    u16 field_7c, field_7e;
    u16 arm_x_rotation, target_pitch, target_roll;
    i16 platform_id;
    f32 activation_time;
    f32 target_scale;
} PANEL_MOVEMENT_PC;
STATIC_ASSERT(sizeof(PANEL_MOVEMENT_PC) == 0x90);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, matrix) == 0x0);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, name) == 0x40);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, position) == 0x50);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, y_rotation) == 0x5c);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, draw_flags) == 0x63);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, floor_position) == 0x64);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_offset) == 0x70);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_scale) == 0x8c);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, field_7c) == 0x7c);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, field_7e) == 0x7e);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, arm_x_rotation) == 0x80);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_pitch) == 0x82);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_roll) == 0x84);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, platform_id) == 0x86);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, activation_time) == 0x88);

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

char *__cdecl GizPanel_GetGizmoName(void *input) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    if (gizmo != NULL && gizmo->object != NULL)
        return ((PANEL_MOVEMENT_PC *)gizmo->object)->name;
    return NULL;
}

void __cdecl GizPanel_GetAbsTargetPos(void *context, NUVEC *target, i32 player_position) {
    PANEL_MOVEMENT_PC *panel = (PANEL_MOVEMENT_PC *)context;
    NUVEC offset;
    if (panel == NULL || target == NULL)
        return;
    if (player_position != 0 && panel->model_variant == 0) {
        offset.x = PlayerPanelVariantZeroX;
        offset.y = panel->target_offset.y;
        offset.z = PlayerPanelVariantZeroZ;
    } else if (player_position != 0 && panel->model_variant == 1) {
        offset.x = PlayerPanelVariantOneX;
        offset.y = panel->target_offset.y;
        offset.z = PlayerPanelVariantOneZ;
    } else {
        offset = panel->target_offset;
    }
    NuVecRotateY(&offset, &offset, panel->y_rotation);
    offset.x += panel->position.x;
    offset.z += panel->position.z;
    *target = offset;
}

typedef struct panel_load_system_pc_s {
    i32 count;
    PANEL_MOVEMENT_PC *panels;
} PANEL_SYSTEM_PC;
STATIC_ASSERT(sizeof(PANEL_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(PANEL_SYSTEM_PC, panels) == 4);

i32 __cdecl GizPanel_Load(void *world_pointer, void *unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    PANEL_SYSTEM_PC *system;
    i32 version, index, length;
    u8 value;
    u16 rotation;
    f32 scale;
    (void)unused;
    if (world == NULL || world->giz_panel_sys == NULL)
        return 0;
    system = (PANEL_SYSTEM_PC *)world->giz_panel_sys;
    if (system->count != 0)
        return 0;
    version = EdFileReadInt();
    system->count = EdFileReadInt();
    for (index = 0; index < system->count; ++index) {
        length = EdFileReadInt();
        EdFileRead(system->panels[index].name, length);
        EdFileReadNuVec(&system->panels[index].position);
        rotation = EdFileReadUnsignedShort();
        system->panels[index].y_rotation = rotation;
        value = (u8)EdFileReadChar();
        system->panels[index].model_variant = value;
        if (version >= 3) {
            value = (u8)EdFileReadChar();
            system->panels[index].flags = (u8)((system->panels[index].flags & ~0x20u) | ((value & 1u) << 5));
        }
        if (version >= 4) {
            EdFileReadNuVec(&system->panels[index].target_offset);
            scale = EdFileReadFloat();
            system->panels[index].target_scale = scale;
        } else {
            system->panels[index].target_offset.y = 0.0f;
            system->panels[index].target_offset.x = 0.0f;
            system->panels[index].target_offset.z = -0.4f;
            system->panels[index].target_scale = 1.0f;
        }
        if (version >= 5) {
            value = (u8)EdFileReadChar();
            system->panels[index].flags = (u8)((system->panels[index].flags & ~0x40u) | ((value & 1u) << 6));
        }
        if (version >= 6) {
            value = (u8)EdFileReadChar();
            system->panels[index].flags = (u8)((system->panels[index].flags & ~0x80u) | ((value & 1u) << 7));
            value = (u8)EdFileReadChar();
            system->panels[index].draw_flags = (u8)((system->panels[index].draw_flags & ~1u) | (value & 1u));
        }
        if (version >= 7) {
            value = (u8)EdFileReadChar();
            system->panels[index].draw_flags = (u8)((system->panels[index].draw_flags & ~2u) | ((value & 1u) << 1));
        }
        if (version >= 8) {
            value = (u8)EdFileReadChar();
            system->panels[index].draw_flags = (u8)((system->panels[index].draw_flags & ~4u) | (value != 0 ? 4 : 0));
        } else {
            system->panels[index].draw_flags &= ~4u;
        }
    }
    return 1;
}

/* Private append-only fragment for canonical 144B PANEL_MOVEMENT_PC. */
extern const f64 GameObjectNearFloorNoFloorHeight;

STATIC_ASSERT(sizeof(PANEL_MOVEMENT_PC) == 0x90);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, floor_position) == 0x64);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, arm_x_rotation) == 0x80);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_pitch) == 0x82);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_roll) == 0x84);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, activation_time) == 0x88);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);

void __cdecl GizPanel_Reset(void *context) {
    PANEL_MOVEMENT_PC *panel = (PANEL_MOVEMENT_PC *)context;
    NUVEC offset, probe_position;
    volatile f32 rounded_height;
    f32 height;
    panel->floor_position.y = 0.0f;
    panel->floor_position.x = 0.0f;
    panel->floor_position.z = 0.4f;
    NuVecRotateY(&panel->floor_position, &panel->floor_position, panel->y_rotation + 0x8000);
    NuVecAdd(&panel->floor_position, &panel->floor_position, &panel->position);
    offset = panel->target_offset;
    NuVecRotateY(&offset, &offset, panel->y_rotation);
    probe_position.x = panel->position.x + offset.x;
    probe_position.z = panel->position.z + offset.z;
    probe_position.y = panel->position.y;
    panel->floor_position.y = GameShadow(NULL, &panel->floor_position, 0.2f, -1);
    rounded_height = GameShadow(NULL, &probe_position, 0.2f, -1);
    height = rounded_height;
    panel->target_offset.y = height;
    if ((f64)height != GameObjectNearFloorNoFloorHeight) {
        panel->target_offset.y = height + 0.001f;
        FindAnglesZX(&ShadNorm, &panel->target_pitch, &panel->target_roll);
    }
    panel->flags = (u8)((panel->flags & 0xfcu) | 0x0cu);
    panel->activation_time = 0.0f;
    panel->arm_x_rotation = 0;
    NuMtxSetRotationY(&panel->matrix, panel->y_rotation);
    NuMtxTranslate(&panel->matrix, &panel->position);
}

void __cdecl GizPanel_Activate(void *input, i32 active) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    PANEL_MOVEMENT_PC *panel;
    if (gizmo != NULL && gizmo->object != NULL) {
        panel = (PANEL_MOVEMENT_PC *)gizmo->object;
        panel->flags ^= (u8)(((active != 0) << 3) ^ panel->flags) & 8;
        if (active != 0)
            GizPanel_Reset(panel);
    }
}

void __cdecl GizPanels_Reset(void *world_pointer, void *unused, void *progress_pointer) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    PANEL_SYSTEM_PC *system;
    PANEL_MOVEMENT_PC *panel;
    u32 *progress = (u32 *)progress_pointer;
    i32 index;
    (void)unused;
    if (world == NULL)
        return;
    system = (PANEL_SYSTEM_PC *)world->giz_panel_sys;
    if (system == NULL || system->panels == NULL || system->count <= 0)
        return;
    for (index = 0; index < system->count; ++index) {
        panel = &system->panels[index];
        GizPanel_Reset(panel);
        if (progress != NULL && index < 32) {
            i32 word = index / 32;
            u32 bit = 1u << (index % 32);
            if (progress[word] & bit)
                panel->flags |= 2;
            panel->flags = (u8)((panel->flags & ~4u) | ((progress[word + 2] & bit) != 0 ? 4 : 0));
            panel->flags = (u8)((panel->flags & ~8u) | ((progress[word + 1] & bit) != 0 ? 8 : 0));
        }
    }
}

extern const f64 PanelVariantTwoTargetY, PanelVariantTwoTargetZ;
extern const f64 PanelVariantThreeTargetY, PanelVariantThreeTargetZ;
extern const f32 PanelTrackEnterDistanceSquared, PanelTrackLeaveDistanceSquared;
extern const char PanelDeactivateSfxPc[10], PanelActivateSfxPc[10];

static NUVEC *PanelGetTrackingPositionPc(PANEL_MOVEMENT_PC *panel, NUVEC *position) {
    *position = panel->position;
    if (panel->model_variant == 3) {
        position->y = (f32)((f64)position->y + PanelVariantThreeTargetY);
        position->z = (f32)((f64)position->z - PanelVariantThreeTargetZ);
    } else if (panel->model_variant == 2) {
        position->y = (f32)((f64)position->y + PanelVariantTwoTargetY);
        position->z = (f32)((f64)position->z - PanelVariantTwoTargetZ);
    }
    return position;
}

void __cdecl GizPanel_Update(void *world_pointer, void *unused, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    PANEL_SYSTEM_PC *system = (PANEL_SYSTEM_PC *)world->giz_panel_sys;
    GAMEOBJECT *nearest_player = NULL;
    NUVEC nearest_direction;
    i32 index;
    (void)unused;
    if (system == NULL)
        return;
    for (index = 0; index < system->count; ++index) {
        PANEL_MOVEMENT_PC *panel = &system->panels[index];
        if ((panel->flags & 8) != 0) {
            f32 nearest_distance = 1000000000.0f;
            f32 maximum_distance;
            i32 player_index;
            maximum_distance =
                (panel->flags & 0x10) != 0 ? PanelTrackLeaveDistanceSquared : PanelTrackEnterDistanceSquared;
            nearest_player = NULL;
            for (player_index = 0; player_index < 8; ++player_index) {
                GAMEOBJECT *player = Player[player_index];
                if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0) {
                    NUVEC position, direction;
                    volatile f32 distance;
                    PanelGetTrackingPositionPc(panel, &position);
                    distance = NuVecDistSqr(&position, &player->collision_position, &direction);
                    if (nearest_distance > distance && maximum_distance > distance) {
                        nearest_distance = distance;
                        nearest_direction = direction;
                        nearest_player = Player[player_index];
                    }
                }
            }
        }
        if (nearest_player != NULL) {
            u16 previous_x = panel->field_7c;
            u16 previous_y = panel->field_7e;
            u16 target_x, target_y;
            FindAnglesXY(&nearest_direction, &target_x, &target_y);
            target_y = (u16)(target_y - panel->y_rotation);
            if (previous_y <= 0x4000 && (u16)(target_y - 0x4001) <= 0x7ffe)
                target_y = 0x4000;
            else if (previous_y >= 0xc000 && (u16)(target_y - 0x4001) <= 0x7ffe)
                target_y = 0xc000;
            if (previous_x <= 0x4000 && (u16)(target_x - 0x4001) <= 0x7ffe)
                target_x = (u16)(panel->y_rotation + 0x4000);
            else if (previous_x >= 0xc000 && (u16)(target_x - 0x4001) <= 0x7ffe)
                target_x = 0xc000;
            panel->field_7c = SeekRot(panel->field_7c, target_x, 5.0f);
            panel->field_7e = SeekRot(panel->field_7e, target_y, 5.0f);
            if ((panel->flags & 0x10) == 0) {
                panel->activation_time = 0.0f;
                panel->flags |= 0x10;
                PlaySfx((char *)PanelActivateSfxPc, &panel->position);
            } else {
                volatile f32 time = panel->activation_time + FRAMETIME;
                panel->activation_time = time;
                if ((f64)time > (f64)0.8f)
                    panel->activation_time = 0.8f;
            }
        } else {
            panel->field_7c = SeekRot(panel->field_7c, 0xe000, 10.0f);
            panel->field_7e = SeekRot(panel->field_7e, 0, 10.0f);
            if ((panel->flags & 0x10) != 0)
                PlaySfx((char *)PanelDeactivateSfxPc, &panel->position);
            panel->flags &= (u8)~0x10;
            if (panel->activation_time > 0.0f) {
                volatile f32 time = panel->activation_time - delta_time;
                panel->activation_time = time;
                if (time < 0.0f)
                    panel->activation_time = 0.0f;
            }
        }
    }
}

/* Real mutable original globals: 007f90c4 Z and 007f90c8 Y, each4B. */
f32 GizPanelPictureZPc = -0.123f;
f32 GizPanelPictureYPc = 0.0775f;

/* Private append to gizpanel_pc.c; reuse canonical PANEL_MOVEMENT_PC144/PANEL_SYSTEM_PC8. */
extern f32 GizPanelPictureYPc, GizPanelPictureZPc;
extern const f64 GameObjectNearFloorNoFloorHeight;
STATIC_ASSERT(sizeof(PANEL_MOVEMENT_PC) == 0x90);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);

typedef struct PanelDrawModelsPc {
    i32 base, animated, transition, state_on, state_off, arm, target;
    f32 arm_y, arm_z;
} PanelDrawModelsPc;

void __cdecl GizPanel_Draw(void *world_pointer, void *unused, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    PANEL_SYSTEM_PC *system;
    u16 target_spin;
    volatile f32 target_alpha;
    i32 index;
    (void)unused;
    (void)unused_time;
    if (world == NULL)
        return;
    system = (PANEL_SYSTEM_PC *)world->giz_panel_sys;
    if (system == NULL || system->count == 0)
        return;
    target_spin = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    target_alpha =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] *
            (f64)0.2f +
        (f64)0.8f;
    for (index = 0; index < system->count; ++index) {
        PANEL_MOVEMENT_PC *panel = &system->panels[index];
        PanelDrawModelsPc models;
        NUMTX base_matrix;
        i32 state_model;
        if ((panel->flags & 4) == 0 && editor_active == 0)
            continue;
        if ((panel->draw_flags & 2) == 0)
            EnableShadowMapRendering(0);
        else
            ResetShadowMapRendering();
        models.base = models.animated = models.transition = models.state_on = models.state_off = models.arm =
            models.target = -1;
        models.arm_y = 0.0f;
        models.arm_z = 0.0f;
        switch (panel->model_variant) {
            case 0:
                models.base = (panel->draw_flags & 1) | 0x106;
                models.animated = (panel->flags >> 7) | 0x108;
                models.transition = 0x10a;
                models.state_on = 0x10b;
                models.state_off = 0x10c;
                models.target = 0x55;
                break;
            case 1:
                models.base = (panel->draw_flags & 1) != 0 ? 0x107 : 0x100;
                models.animated = (panel->flags & 0x80) != 0 ? 0x102 : 0x101;
                models.transition = 0x103;
                models.state_on = 0x104;
                models.state_off = 0x105;
                models.target = 0x55;
                break;
            case 2:
                models.base = 0x10d;
                models.animated = 0x10e;
                models.transition = 0x10f;
                models.state_on = 0x110;
                models.state_off = 0x111;
                models.arm = 0x112;
                models.target = 0x113;
                models.arm_y = 0.245f;
                models.arm_z = -0.19f;
                break;
            case 3:
                models.base = 0x114;
                models.animated = 0x115;
                models.transition = 0x116;
                models.state_on = 0x117;
                models.state_off = 0x118;
                models.arm = 0x119;
                models.target = 0x11a;
                models.arm_y = 0.34f;
                models.arm_z = -0.17f;
                break;
        }
        NuMtxSetRotationY(&base_matrix, panel->y_rotation);
        NuMtxTranslate(&base_matrix, &panel->position);
        if ((panel->flags & 0x20) == 0 && models.base != -1 && world->level_objects[models.base].active != 0)
            NuSpecialDrawAt(&world->level_objects[models.base].special, &base_matrix);
        state_model = (panel->flags & 2) != 0 ? models.state_on : models.state_off;
        if ((panel->flags & 0x20) == 0 && state_model != -1 && world->level_objects[state_model].active != 0)
            NuSpecialDrawAt(&world->level_objects[state_model].special, &base_matrix);
        if ((panel->flags & 0x20) == 0 && (panel->activation_time > 0.0f || editor_active != 0)) {
            NUVEC picture_offset, transition_offset;
            NUMTX transition_base, animated_matrix;
            volatile f32 frame;
            picture_offset.x = 0.0f;
            picture_offset.y = GizPanelPictureYPc;
            picture_offset.z = GizPanelPictureZPc;
            transition_offset.x = 0.0f;
            transition_offset.y = GizPanelPictureYPc;
            transition_offset.z = (f64)GizPanelPictureZPc - (f64)0.0005f;
            NuVecRotateY(&picture_offset, &picture_offset, panel->y_rotation);
            NuVecRotateY(&transition_offset, &transition_offset, panel->y_rotation);
            transition_base = base_matrix;
            NuMtxTranslate(&base_matrix, &picture_offset);
            NuMtxTranslate(&transition_base, &transition_offset);
            if (1.2f > panel->activation_time && editor_active == 0) {
                f64 progress = (f64)panel->activation_time / (f64)1.2f;
                volatile f32 rounded_progress = progress;
                volatile f32 alpha;
                frame = progress * 23.0 + 1.0;
                EvalAnim(&world->level_objects[models.transition].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &transition_base);
                alpha = 1.0 - (f64)rounded_progress;
                NuSpecialDrawAtAlpha(&world->level_objects[models.transition].special, &animated_matrix, alpha);
            }
            frame = (f64)panel->activation_time / (f64)0.8f * 24.0 + 1.0;
            EvalAnim(&world->level_objects[models.animated].special, frame, &animated_matrix, 0);
            animated_matrix.m32 = 0.0f;
            animated_matrix.m31 = 0.0f;
            animated_matrix.m30 = 0.0f;
            NuMtxMulVU0(&animated_matrix, &animated_matrix, &base_matrix);
            NuSpecialDrawAtAlpha(&world->level_objects[models.animated].special, &animated_matrix, 1.0f);
        }
        if ((panel->flags & 0x48) == 8 && (f64)panel->target_offset.y != GameObjectNearFloorNoFloorHeight &&
            models.target != -1 && world->level_objects[models.target].active != 0) {
            NUVEC target;
            f32 alpha;
            GizPanel_GetAbsTargetPos(panel, &target, 0);
            NuMtxSetRotationY(&base_matrix, target_spin);
            if (panel->target_roll != 0)
                NuMtxRotateZ(&base_matrix, panel->target_roll);
            if (panel->target_pitch != 0)
                NuMtxRotateX(&base_matrix, panel->target_pitch);
            NuMtxTranslate(&base_matrix, &target);
            NuMtxPreScaleU(&base_matrix, panel->target_scale);
            alpha = (panel->flags & 1) != 0 ? 0.0f : target_alpha;
            NuSpecialDrawAtAlpha(&world->level_objects[models.target].special, &base_matrix, alpha);
        }
        if ((panel->flags & 0x20) == 0 && models.arm != -1 && world->level_objects[models.arm].active != 0) {
            NUVEC arm_offset;
            arm_offset.x = 0.0f;
            arm_offset.y = models.arm_y;
            arm_offset.z = models.arm_z;
            NuVecRotateX(&arm_offset, &arm_offset, panel->arm_x_rotation);
            NuVecRotateY(&arm_offset, &arm_offset, panel->y_rotation);
            NuMtxSetRotationX(&base_matrix, panel->arm_x_rotation);
            NuMtxRotateY(&base_matrix, panel->y_rotation);
            NuMtxPreRotateX(&base_matrix, panel->field_7c);
            NuMtxRotateY(&base_matrix, panel->field_7e);
            NuMtxTranslate(&base_matrix, &panel->position);
            NuMtxTranslate(&base_matrix, &arm_offset);
            NuSpecialDrawAt(&world->level_objects[models.arm].special, &base_matrix);
        }
    }
    ResetShadowMapRendering();
}

/* Retail terrain helper is private to these callers (panel arrives in ESI). */
static void GizPanel_CreateTerrain(PANEL_MOVEMENT_PC *panel) {
    if (panel == NULL)
        return;
    if (panel->platform_id != -1)
        DeletePlatinst((i16)panel->platform_id);
    switch (panel->model_variant) {
        case 0:
            panel->platform_id = NewPlatPickupInst(panel, 4);
            break;
        case 1:
            panel->platform_id = NewPlatPickupInst(panel, 7);
            break;
        case 2:
            panel->platform_id = NewPlatPickupInst(panel, 5);
            break;
        case 3:
            panel->platform_id = NewPlatPickupInst(panel, 6);
            break;
        default:
            break;
    }
    PlatInstRotate((i16)panel->platform_id, 1);
}

void GizPanel_InitTerrain(WORLDINFO *world) {
    i32 index;
    PANEL_SYSTEM_PC *system = (PANEL_SYSTEM_PC *)world->giz_panel_sys;
    if (system == NULL || system->count <= 0)
        return;
    for (index = 0; index < ((PANEL_SYSTEM_PC *)world->giz_panel_sys)->count; ++index) {
        ((PANEL_SYSTEM_PC *)world->giz_panel_sys)->panels[index].platform_id = -1;
        GizPanel_CreateTerrain(&((PANEL_SYSTEM_PC *)world->giz_panel_sys)->panels[index]);
    }
}

void __cdecl GizPanel_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    PANEL_MOVEMENT_PC *panel;
    i32 was_visible;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    panel = (PANEL_MOVEMENT_PC *)gizmo->object;
    was_visible = (panel->flags >> 2) & 1;
    panel->flags = (u8)((panel->flags & ~4u) | ((visible != 0) << 2));
    if ((panel->flags & 4) != 0) {
        if (was_visible == 0)
            GizPanel_CreateTerrain(panel);
    } else if (was_visible != 0) {
        DeletePlatinst((i16)panel->platform_id);
    }
}

i32 __cdecl GizPanel_GetOutput(void *input, i32 output, i32 mode) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    u32 flags = ((PANEL_MOVEMENT_PC *)gizmo->object)->flags;
    (void)output;
    (void)mode;
    if ((flags & 8) != 0 && (flags & 4) != 0)
        return (flags >> 1) & 1;
    return 0;
}

extern u8 PanelStormActivePc, PanelDroidActivePc, PanelBountyHunterActivePc;
i32 __cdecl GizPanel_CanUsePanel(GAMEOBJECT *, void *);

STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, panel_request_data) == 0x7a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, protocol_interface_action) == 0xe39);

void __cdecl SetProtocolDroidInterfaceAction(GAMEOBJECT *object) {
    if (object->protocol_interface_action == 3)
        object->field_7aa = 0x45;
    else if (object->protocol_interface_action == 2)
        object->field_7aa = 0x46;
    else if (object->protocol_interface_action == 1)
        object->field_7aa = 0x47;
}

PANEL_MOVEMENT_PC *__cdecl GizPanel_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object,
                                                f32 *distance_squared, i32 check_eligibility) {
    PANEL_SYSTEM_PC *system;
    PANEL_MOVEMENT_PC *nearest = NULL;
    f32 nearest_distance = 1.0e9f;
    i32 index;
    if (world == NULL || world->giz_panel_sys == NULL)
        return NULL;
    system = (PANEL_SYSTEM_PC *)world->giz_panel_sys;
    for (index = 0; index < system->count; ++index, system = (PANEL_SYSTEM_PC *)world->giz_panel_sys) {
        PANEL_MOVEMENT_PC *panel = &system->panels[index];
        f32 distance;
        if (object != NULL) {
            NUVEC target;
            if ((panel->flags & 0x0f) != 0x0c || panel->floor_position.y == 2000000.0f)
                continue;
            if (check_eligibility != 0 && GizPanel_CanUsePanel(object, panel) == 0)
                continue;
            GizPanel_GetAbsTargetPos(panel, &target, 0);
            distance = NuVecDistSqr(position, &target, NULL);
        } else {
            distance = NuVecDistSqr(position, &panel->position, NULL);
        }
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = panel;
        }
    }
    if (distance_squared != NULL)
        *distance_squared = nearest_distance;
    return nearest;
}

void __cdecl GizPanel_PlaySfx(char *name, NUVEC *position, i32 player_bits) {
    i16 sfx_id;
    if (name == NULL || position == NULL)
        return;
    sfx_id = (i16)GetSfxId(name);
    if (sfx_id == -1)
        return;
    if (player_bits == 0) {
        PlaySfxById(sfx_id, position);
    } else {
        if ((player_bits & 1) != 0) {
            nusound_special_positions[1] = *position;
            PlaySfxById(sfx_id, &nusound_special_positions[1]);
            nusound_special_positions[1] = nusound_special_positions[0];
        }
        if ((player_bits & 2) != 0) {
            nusound_special_positions[2] = *position;
            PlaySfxById(sfx_id, &nusound_special_positions[2]);
            nusound_special_positions[2] = nusound_special_positions[0];
        }
    }
}

void __cdecl GizPanel_MoveCode(WORLDINFO *world, GAMEOBJECT *object, i32 use) {
    PANEL_MOVEMENT_PC *panel;
    f32 distance;
    f32 radius;
    volatile f32 radius_squared;
    (void)world;
    if (object->field_db0 > 0.0f)
        object->field_db0 -= FRAMETIME;
    PanelStormActivePc = 0;
    PanelDroidActivePc = 0;
    PanelBountyHunterActivePc = 0;
    if (object->character_context == 0x0b && object->grapple != NULL) {
        if (object->character_model->model_data_b[object->field_7aa] == NULL ||
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
            object->context_hover_target_y += FRAMETIME;
            if (object->context_hover_target_y > 1.0f)
                object->context_hover_target_y = 1.0f;
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                object->character_context = -1;
                ((PANEL_MOVEMENT_PC *)object->grapple)->flags &= (u8)~1;
                ((PANEL_MOVEMENT_PC *)object->grapple)->flags |= 2;
                object->grapple = NULL;
            }
            if ((object->flags_e20 & 0x1000) == 0)
                object->flags_e20 |= 0x1000;
        } else {
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                if (object->grapple != NULL) {
                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                    if (((PANEL_MOVEMENT_PC *)object->grapple)->model_variant == 2)
                        PlaySfx("Hunter_Granted", &object->collision_position);
                    if (((PANEL_MOVEMENT_PC *)object->grapple)->model_variant == 3)
                        PlaySfx("Trooper_Granted", &object->collision_position);
                } else {
                    NewRumble(object->pad->bound_pad, 0.5f, 0);
                }
                if ((object->flags_1fc & 0x80) != 0) {
                    u8 variant = ((PANEL_MOVEMENT_PC *)object->grapple)->model_variant;
                    if (variant == 0 || variant == 1) {
                        if (StartupGameFlag1 != 0 && Mission_Active(NULL) != NULL) {
                            Hint_SetComplete(0x2b9);
                        } else {
                            Hint_SetComplete(0x25f);
                            LswForceUsedHintFlagsPc |= 4;
                        }
                    } else if (variant == 3) {
                        Hint_SetComplete(0x260);
                    } else if (variant == 2) {
                        Hint_SetComplete(0x261);
                    }
                }
                object->character_context = -1;
                ((PANEL_MOVEMENT_PC *)object->grapple)->flags &= (u8)~1;
                ((PANEL_MOVEMENT_PC *)object->grapple)->flags |= 2;
                object->grapple = NULL;
            } else if ((object->character_data->model_flags & 0x20) != 0) {
                SetProtocolDroidInterfaceAction(object);
            }
        }
        return;
    }
    if (object->character_context != -1 && object->character_context != 1 && object->character_context != 2 &&
        !objInNetWaitContext(object, 0x0b))
        return;
    panel = GizPanel_FindNearest(WORLD, &object->position, object, &distance, 1);
    if (panel != NULL) {
        radius = (f32)((0.25 + object->bounds_radius) * panel->target_scale);
        radius_squared = radius * radius;
        if (!(distance < radius_squared) ||
            (use == 0 && (object->panel_use_request != 1 || object->panel_request_data == NULL)))
            return;
        object->movement_facing_angle = panel->y_rotation;
        object->context_hover_target_y = 0.0f;
        object->delayed_turn_timer = 0.0f;
        object->flags_e20 &= ~0x1000u;
        object->character_context = 0x0b;
        object->grapple = panel;
        object->field_7b3 = 0;
        FastWeaponIn(object, 0);
        object->flags_e24 |= 0x200;
        panel = (PANEL_MOVEMENT_PC *)object->grapple;
        if (panel->model_variant == 2) {
            object->field_7aa = object->character_model->model_data_b[0x46] != NULL ? 0x46 : 0x45;
            Hint_SetComplete(0x261);
            Hint_SetComplete(0x26a);
        } else if (panel->model_variant == 3) {
            object->field_7aa = object->character_model->model_data_b[0x45] != NULL
                                    ? 0x45
                                    : (object->character_model->model_data_b[0x46] != NULL ? 0x46 : 0x45);
            MakeBaddiesForgetAboutParty(1);
            Hint_SetComplete(0x260);
            Hint_SetComplete(0x269);
        } else {
            object->field_7aa = 0x18;
            if (panel->model_variant == 1) {
                if (!InStory() || (((PANEL_MOVEMENT_PC *)object->grapple)->draw_flags & 4) == 0)
                    GizPanel_PlaySfx("TC14_VLA", &object->collision_position, 1 << object->field_280);
                if ((object->character_data->model_flags & 0x20) != 0)
                    SetProtocolDroidInterfaceAction(object);
            } else {
                if (!InStory() || (((PANEL_MOVEMENT_PC *)object->grapple)->draw_flags & 4) == 0)
                    GizPanel_PlaySfx("R2D2_VLA", &object->collision_position, 1 << object->field_280);
            }
            Hint_SetComplete(0x25f);
            LswForceUsedHintFlagsPc |= 4;
        }
        object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
        object->field_db0 = 0.0f;
        if (object->context_timer <= 0.0f)
            object->context_timer = 2.0f;
    } else if (use != 0 && (object->flags_1fc & 0x80) != 0 && (object->character_data->model_flags & 0x20) != 0 &&
               object->field_db0 <= 0.0f) {
        GizPanel_PlaySfx("TC14_VLN", &object->collision_position, 1 << object->field_280);
        object->field_db0 = 0.5f;
    }
}
