#include "saga.h"

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
