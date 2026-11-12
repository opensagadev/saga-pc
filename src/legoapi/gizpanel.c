#include "saga.h"

/* Retail unit 0x46d7f0-0x46f890 (legoapi/gizmos/object/gizpanel.cpp): definitions in retail address order. */

typedef struct bulk_ai_panel_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL_PC;
typedef struct {
    u8 opaque_00[0x40];
    char name[16];
    u8 opaque_50[0x90 - 0x50];
} ADD_PANEL_PC;
typedef struct registry_panel_record_pc_s {
    u8 storage[0x90];
} REGISTRY_PANEL_RECORD_PC;
typedef struct registry_panel_system_pc_s {
    i32 count;
    REGISTRY_PANEL_RECORD_PC *panels;
} REGISTRY_PANEL_SYSTEM_PC;
typedef struct {
    u32 triggered[1], enabled[1], visible[1];
} PANEL_PROGRESS_PC;
typedef struct {
    u8 unused00[0x62];
    u8 flags;
    u8 unused63[0x2d];
} PANEL_PROGRESS_RECORD_PC;
typedef struct {
    i32 count;
    PANEL_PROGRESS_RECORD_PC *records;
} PANEL_PROGRESS_SYSTEM_PC;
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
typedef struct panel_load_system_pc_s {
    i32 count;
    PANEL_MOVEMENT_PC *panels;
} PANEL_SYSTEM_PC;
/* Private append-only fragment for canonical 144B PANEL_MOVEMENT_PC. */
extern const f64 GameObjectNearFloorNoFloorHeight;
extern const f64 PanelVariantTwoTargetY, PanelVariantTwoTargetZ;
extern const f64 PanelVariantThreeTargetY, PanelVariantThreeTargetZ;
extern const f32 PanelTrackEnterDistanceSquared, PanelTrackLeaveDistanceSquared;
extern const char PanelDeactivateSfxPc[10], PanelActivateSfxPc[10];
/* Private append to gizpanel_pc.c; reuse canonical PANEL_MOVEMENT_PC144/PANEL_SYSTEM_PC8. */
extern f32 GizPanelPictureYPc, GizPanelPictureZPc;
extern const f64 GameObjectNearFloorNoFloorHeight;
extern u8 PanelStormActivePc, PanelDroidActivePc, PanelBountyHunterActivePc;
void __cdecl SetProtocolDroidInterfaceAction(GAMEOBJECT *object);
/* Native PC hint updater records retain their original packed field offsets. */
typedef struct HINT_PANEL_PC {
    u8 reserved00[0x61];
    u8 model_variant, flags;
    u8 reserved63[0x29];
    f32 target_scale;
} HINT_PANEL_PC;
struct panel_movement_pc_s;
struct panel_movement_pc_s *__cdecl GizPanel_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *, i32);

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
    if (system == NULL)
        return;
    for (index = 0; index < ((PANEL_SYSTEM_PC *)world->giz_panel_sys)->count; ++index) {
        ((PANEL_SYSTEM_PC *)world->giz_panel_sys)->panels[index].platform_id = -1;
        GizPanel_CreateTerrain(&((PANEL_SYSTEM_PC *)world->giz_panel_sys)->panels[index]);
    }
}

static NUVEC *PanelGetTrackingPositionPc(PANEL_MOVEMENT_PC *panel, NUVEC *position) {
    *position = panel->position;
    switch (panel->model_variant) {
        case 3:
            position->y = (f32)((f64)position->y + PanelVariantThreeTargetY);
            position->z = (f32)((f64)position->z - PanelVariantThreeTargetZ);
            break;
        case 2:
            position->y = (f32)((f64)position->y + PanelVariantTwoTargetY);
            position->z = (f32)((f64)position->z - PanelVariantTwoTargetZ);
            break;
    }
    return position;
}

i32 __cdecl GizPanel_Load(void *world_pointer, void *unused) {
    typedef union PanelLoadFlagsPc {
        u8 value;
        struct {
            u8 unknown_0 : 5;
            u8 flag_5 : 1;
            u8 flag_6 : 1;
            u8 flag_7 : 1;
        } bits;
    } PanelLoadFlagsPc;
    typedef union PanelLoadDrawFlagsPc {
        u8 value;
        struct {
            u8 flag_0 : 1;
            u8 flag_1 : 1;
            u8 flag_2 : 1;
            u8 unknown_3 : 5;
        } bits;
    } PanelLoadDrawFlagsPc;
    typedef struct PanelLoadStatePc {
        u8 unknown_00[0x62];
        PanelLoadFlagsPc flags;
        PanelLoadDrawFlagsPc draw_flags;
    } PanelLoadStatePc;
    STATIC_ASSERT(sizeof(PanelLoadFlagsPc) == 1);
    STATIC_ASSERT(sizeof(PanelLoadDrawFlagsPc) == 1);
    STATIC_ASSERT(offsetof(PanelLoadStatePc, flags) == 0x62);
    STATIC_ASSERT(offsetof(PanelLoadStatePc, draw_flags) == 0x63);
    STATIC_ASSERT(sizeof(PanelLoadStatePc) == 0x64);
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    PANEL_SYSTEM_PC *system;
    i32 version, index, length;
    u8 value;
    u16 rotation;
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
            ((PanelLoadStatePc *)&system->panels[index])->flags.bits.flag_5 = value & 1;
        }
        if (version >= 4) {
            EdFileReadNuVec(&system->panels[index].target_offset);
            system->panels[index].target_scale = EdFileReadFloat();
        } else {
            system->panels[index].target_offset.y = 0.0f;
            system->panels[index].target_offset.x = 0.0f;
            system->panels[index].target_offset.z = -0.4f;
            system->panels[index].target_scale = 1.0f;
        }
        if (version >= 5) {
            value = (u8)EdFileReadChar();
            ((PanelLoadStatePc *)&system->panels[index])->flags.bits.flag_6 = value & 1;
        }
        if (version >= 6) {
            value = (u8)EdFileReadChar();
            ((PanelLoadStatePc *)&system->panels[index])->flags.bits.flag_7 = value & 1;
            value = (u8)EdFileReadChar();
            ((PanelLoadStatePc *)&system->panels[index])->draw_flags.bits.flag_0 = value & 1;
        }
        if (version >= 7) {
            value = (u8)EdFileReadChar();
            ((PanelLoadStatePc *)&system->panels[index])->draw_flags.bits.flag_1 = value & 1;
        }
        if (version >= 8) {
            value = (u8)EdFileReadChar();
            ((PanelLoadStatePc *)&system->panels[index])->draw_flags.bits.flag_2 = value != 0;
        } else {
            ((PanelLoadStatePc *)&system->panels[index])->draw_flags.bits.flag_2 = 0;
        }
    }
    return 1;
}

void *__cdecl GizPanels_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    REGISTRY_PANEL_SYSTEM_PC *system;
    world->giz_panel_sys = NULL;
    if (world->current_level->max_levers > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->giz_panel_sys = (GIZPANELSYS_s *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += sizeof(REGISTRY_PANEL_SYSTEM_PC);
        system = (REGISTRY_PANEL_SYSTEM_PC *)world->giz_panel_sys;
        system->count = 0;
        system->panels = NULL;
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        system = (REGISTRY_PANEL_SYSTEM_PC *)world->giz_panel_sys;
        system->panels = (REGISTRY_PANEL_RECORD_PC *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_gizpanels * sizeof(*system->panels);
        memset(system->panels, 0, world->current_level->max_gizpanels * sizeof(*system->panels));
    }
    return world->giz_panel_sys;
}

void __cdecl GizPanel_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    typedef union PanelVisibleFlagsPc {
        u8 value;
        struct {
            u8 unknown_0 : 2;
            u8 visible : 1;
            u8 unknown_3 : 5;
        } bits;
    } PanelVisibleFlagsPc;
    typedef struct PanelVisibleStatePc {
        u8 unknown_00[0x62];
        PanelVisibleFlagsPc flags;
    } PanelVisibleStatePc;
    STATIC_ASSERT(sizeof(PanelVisibleFlagsPc) == 1);
    STATIC_ASSERT(offsetof(PanelVisibleStatePc, flags) == 0x62);
    PANEL_MOVEMENT_PC *panel;
    PanelVisibleStatePc *state;
    i32 was_visible;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    panel = (PANEL_MOVEMENT_PC *)gizmo->object;
    state = (PanelVisibleStatePc *)panel;
    was_visible = state->flags.bits.visible;
    state->flags.bits.visible = visible != 0;
    if (state->flags.bits.visible && !was_visible)
        GizPanel_CreateTerrain(panel);
    else if (!state->flags.bits.visible && was_visible)
        DeletePlatinst((i16)panel->platform_id);
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

i32 __cdecl GizPanel_CanUsePanel(GAMEOBJECT *object, BULK_AI_PANEL_PC *panel) {
    if (panel == NULL || object == NULL)
        return 0;
    if (SuperWeirdo(object) != 0)
        return 1;
    switch (panel->model_variant) {
        case 0:
            if ((object->character_data->model_flags & 0x40) != 0 ||
                (object->character_data->model_flags & 0x1000010) == 0x1000010)
                return 1;
            break;
        case 1:
            if ((object->character_data->model_flags & 0x20) != 0 ||
                (object->character_data->model_flags & 0x1000010) == 0x1000010)
                return 1;
            break;
        case 3:
            if (object->helmet == 5 || (object->character_data->game_character->flags_090 & 0x40000) != 0)
                return 1;
            if (GCDataList[(i16)object->id].weapon_action == 1 || GCDataList[(i16)object->id].weapon_action == 8)
                return 1;
            if ((i16)object->id == FreePlayId007f199cPc || (i16)object->id == id_THEEMPEROR ||
                (i16)object->id == FreePlayId007f1a4cPc || (i16)object->id == FreePlayId007f1a48Pc ||
                (i16)object->id == FreePlayId007f1a44Pc)
                return 1;
            break;
        case 2:
            if ((object->character_data->model_flags & 0x1000000) != 0 || object->helmet == 6)
                return 1;
            break;
    }
    return 0;
}

PANEL_MOVEMENT_PC *__cdecl GizPanel_FindByName(WORLDINFO *world, char *name) {
    PANEL_MOVEMENT_PC *panel;
    i32 index;
    if (world == NULL || world->giz_panel_sys == NULL)
        return NULL;
    panel = ((PANEL_SYSTEM_PC *)world->giz_panel_sys)->panels;
    for (index = 0; index < ((PANEL_SYSTEM_PC *)world->giz_panel_sys)->count; index++, panel++) {
        if (NuStrICmp(panel->name, name) == 0)
            return panel;
    }
    return NULL;
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
            nearest_player = NULL;
            maximum_distance =
                (panel->flags & 0x10) != 0 ? PanelTrackLeaveDistanceSquared : PanelTrackEnterDistanceSquared;
            for (player_index = 0; player_index < 8; ++player_index) {
                GAMEOBJECT *player = Player[player_index];
                if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0) {
                    NUVEC position, direction;
                    f32 distance;
                    PanelGetTrackingPositionPc(panel, &position);
                    distance = NuVecDistSqr(&position, &player->collision_position, &direction);
                    if (distance < nearest_distance && distance < maximum_distance) {
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
            target_y -= panel->y_rotation;
            if (previous_y <= 0x4000 && target_y > 0x4000 && target_y < 0xc000)
                target_y = 0x4000;
            else if (previous_y >= 0xc000 && target_y > 0x4000 && target_y < 0xc000)
                target_y = 0xc000;
            if (previous_x <= 0x4000 && target_x > 0x4000 && target_x < 0xc000)
                target_x = (u16)(panel->y_rotation + 0x4000);
            else if (previous_x >= 0xc000 && target_x > 0x4000 && target_x < 0xc000)
                target_x = 0xc000;
            panel->field_7c = SeekRot(panel->field_7c, target_x, 5.0f);
            panel->field_7e = SeekRot(panel->field_7e, target_y, 5.0f);
            if ((panel->flags & 0x10) == 0) {
                panel->activation_time = 0.0f;
                panel->flags |= 0x10;
                PlaySfx((char *)PanelActivateSfxPc, &panel->position);
            } else {
                f32 time = panel->activation_time + FRAMETIME;
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
                f32 time = panel->activation_time - delta_time;
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

void __cdecl GizPanel_Draw(void *world_pointer, void *unused, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    PANEL_SYSTEM_PC *system;
    u16 target_spin;
    f32 target_alpha;
    NUMTX base_matrix;
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
        i32 base, animated, transition, state_on, state_off, arm, target;
        f32 arm_y, arm_z;
        if ((panel->flags & 4) == 0 && editor_active == 0)
            continue;
        if ((panel->draw_flags & 2) == 0)
            EnableShadowMapRendering(0);
        else
            ResetShadowMapRendering();
        switch (panel->model_variant) {
            case 0:
                base = (panel->draw_flags & 1) | 0x106;
                animated = ((u32)panel->flags >> 7) | 0x108;
                transition = 0x10a;
                state_on = 0x10b;
                state_off = 0x10c;
                arm = -1;
                target = 0x55;
                arm_y = 0.0f;
                arm_z = 0.0f;
                break;
            case 1:
                base = (panel->draw_flags & 1) != 0 ? 0x107 : 0x100;
                animated = (panel->flags & 0x80) != 0 ? 0x102 : 0x101;
                transition = 0x103;
                state_on = 0x104;
                state_off = 0x105;
                arm = -1;
                target = 0x55;
                arm_y = 0.0f;
                arm_z = 0.0f;
                break;
            case 2:
                base = 0x10d;
                animated = 0x10e;
                transition = 0x10f;
                state_on = 0x110;
                state_off = 0x111;
                arm = 0x112;
                target = 0x113;
                arm_y = 0.245f;
                arm_z = -0.19f;
                break;
            case 3:
                base = 0x114;
                animated = 0x115;
                transition = 0x116;
                state_on = 0x117;
                state_off = 0x118;
                arm = 0x119;
                target = 0x11a;
                arm_y = 0.34f;
                arm_z = -0.17f;
                break;
            default:
                base = -1;
                animated = -1;
                transition = -1;
                state_on = -1;
                state_off = -1;
                arm = -1;
                target = -1;
                arm_y = 0.0f;
                arm_z = 0.0f;
                break;
        }
        NuMtxSetRotationY(&base_matrix, panel->y_rotation);
        NuMtxTranslate(&base_matrix, &panel->position);
        if ((panel->flags & 0x20) == 0 && base != -1 && world->level_objects[base].active != 0)
            NuSpecialDrawAt(&world->level_objects[base].special, &base_matrix);
        if ((panel->flags & 0x20) == 0) {
            if ((panel->flags & 2) != 0) {
                if (state_on != -1 && world->level_objects[state_on].active != 0)
                    NuSpecialDrawAt(&world->level_objects[state_on].special, &base_matrix);
            } else if (state_off != -1 && world->level_objects[state_off].active != 0) {
                NuSpecialDrawAt(&world->level_objects[state_off].special, &base_matrix);
            }
        }
        if ((panel->flags & 0x20) == 0 && (panel->activation_time > 0.0f || editor_active != 0)) {
            NUVEC picture_offset, transition_offset;
            NUMTX transition_base, animated_matrix;
            f32 frame;
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
            if (panel->activation_time < 1.2f && editor_active == 0) {
                f32 progress = panel->activation_time / 1.2f;
                f32 alpha;
                frame = panel->activation_time / 1.2f * 23.0 + 1.0;
                EvalAnim(&world->level_objects[transition].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &transition_base);
                alpha = 1.0f - progress;
                NuSpecialDrawAtAlpha(&world->level_objects[transition].special, &animated_matrix, alpha);
                frame = (f64)panel->activation_time / (f64)0.8f * 24.0 + 1.0;
                EvalAnim(&world->level_objects[animated].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &base_matrix);
                NuSpecialDrawAtAlpha(&world->level_objects[animated].special, &animated_matrix, 1.0f);
            } else {
                frame = (f64)panel->activation_time / (f64)0.8f * 24.0 + 1.0;
                EvalAnim(&world->level_objects[animated].special, frame, &animated_matrix, 0);
                animated_matrix.m32 = 0.0f;
                animated_matrix.m31 = 0.0f;
                animated_matrix.m30 = 0.0f;
                NuMtxMulVU0(&animated_matrix, &animated_matrix, &base_matrix);
                NuSpecialDrawAtAlpha(&world->level_objects[animated].special, &animated_matrix, 1.0f);
            }
        }
        if ((panel->flags & 0x40) == 0 && (panel->flags & 8) != 0 &&
            (f64)panel->target_offset.y != GameObjectNearFloorNoFloorHeight && target != -1 &&
            world->level_objects[target].active != 0) {
            NUVEC target_position;
            GizPanel_GetAbsTargetPos(panel, &target_position, 0);
            NuMtxSetRotationY(&base_matrix, target_spin);
            if (panel->target_roll != 0)
                NuMtxRotateZ(&base_matrix, panel->target_roll);
            if (panel->target_pitch != 0)
                NuMtxRotateX(&base_matrix, panel->target_pitch);
            NuMtxTranslate(&base_matrix, &target_position);
            NuMtxPreScaleU(&base_matrix, panel->target_scale);
            NuSpecialDrawAtAlpha(&world->level_objects[target].special, &base_matrix,
                                 (panel->flags & 1) != 0 ? 0.0f : target_alpha);
        }
        if ((panel->flags & 0x20) == 0 && arm != -1 && world->level_objects[arm].active != 0) {
            NUVEC arm_offset;
            arm_offset.x = 0.0f;
            arm_offset.y = arm_y;
            arm_offset.z = arm_z;
            NuVecRotateX(&arm_offset, &arm_offset, panel->arm_x_rotation);
            NuVecRotateY(&arm_offset, &arm_offset, panel->y_rotation);
            NuMtxSetRotationX(&base_matrix, panel->arm_x_rotation);
            NuMtxRotateY(&base_matrix, panel->y_rotation);
            NuMtxPreRotateX(&base_matrix, panel->field_7c);
            NuMtxRotateY(&base_matrix, panel->field_7e);
            NuMtxTranslate(&base_matrix, &panel->position);
            NuMtxTranslate(&base_matrix, &arm_offset);
            NuSpecialDrawAt(&world->level_objects[arm].special, &base_matrix);
        }
    }
    ResetShadowMapRendering();
}

i32 __cdecl GizPanel_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world != NULL)
        return world->current_level->max_gizpanels;
    return 0;
}

void __cdecl GizPanel_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    if (w == NULL || w->giz_panel_sys == NULL)
        return;
    for (i = 0; i < w->giz_panel_sys->count; ++i)
        if (NuStrLen(((ADD_PANEL_PC *)w->giz_panel_sys->panels)[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_PANEL_PC *)w->giz_panel_sys->panels)[i]);
}

char *__cdecl GizPanel_GetGizmoName(void *input) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    if (gizmo != NULL && gizmo->object != NULL)
        return ((PANEL_MOVEMENT_PC *)gizmo->object)->name;
    return NULL;
}

i32 __cdecl GizPanel_GetOutput(void *input, i32 output, i32 mode) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    PANEL_MOVEMENT_PC *panel = (PANEL_MOVEMENT_PC *)gizmo->object;
    (void)output;
    (void)mode;
    if ((panel->flags & 8) != 0 && (panel->flags & 4) != 0)
        return (panel->flags >> 1) & 1;
    return 0;
}

char *__cdecl GizPanel_GetOutputName(void *unused, i32 output_index) {
    return output_index == 0 ? (char *)"Finished" : NULL;
}

i32 __cdecl GizPanel_GetNumOutputs(void *unused) {
    (void)unused;
    return 1;
}

void *__cdecl GizPanels_AllocateProgressData(void *buffer, void *end) {
    return GizmoBufferAlloc((VARIPTR *)buffer, (VARIPTR *)end, 0xc);
}

void __cdecl GizPanels_ClearProgress(void *owner, void *progress_ptr) {
    PANEL_PROGRESS_PC *progress = (PANEL_PROGRESS_PC *)progress_ptr;
    if (progress == NULL)
        return;
    progress->triggered[0] = 0;
    progress->enabled[0] = 0xffffffffu;
    progress->visible[0] = 0xffffffffu;
}

void __cdecl GizPanels_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    PANEL_PROGRESS_PC *progress = (PANEL_PROGRESS_PC *)progress_ptr;
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    PANEL_PROGRESS_SYSTEM_PC *system;
    PANEL_PROGRESS_RECORD_PC *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL)
        return;
    progress->triggered[0] = 0;
    progress->enabled[0] = 0xffffffffu;
    progress->visible[0] = 0xffffffffu;

    if (world == NULL || world->giz_panel_sys == NULL ||
        ((PANEL_PROGRESS_SYSTEM_PC *)world->giz_panel_sys)->records == NULL)
        return;
    for (index = 0; index < ((PANEL_PROGRESS_SYSTEM_PC *)world->giz_panel_sys)->count && index < 32; ++index) {
        system = (PANEL_PROGRESS_SYSTEM_PC *)world->giz_panel_sys;
        record = &system->records[index];
        word = index / 32;
        bit = 1u << (index % 32);
        if (record->flags & 2)
            progress->triggered[word] |= bit;
        if (!(record->flags & 4))
            progress->visible[word] &= ~bit;
        if (!(record->flags & 8))
            progress->enabled[word] &= ~bit;
    }
}

void __cdecl GizPanel_PlaySfx(char *name, NUVEC *position, i32 player_bits) {
    i16 sfx_id;
    if (name == NULL || position == NULL)
        return;
    sfx_id = (i16)GetSfxId(name);
    if (sfx_id == -1)
        return;
    if (player_bits != 0) {
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
    } else {
        PlaySfxById(sfx_id, position);
    }
}

void __cdecl GizPanel_MoveCode(WORLDINFO *world, GAMEOBJECT *object, i32 use) {
    PANEL_MOVEMENT_PC *panel;
    f32 distance;
    f32 radius;
    f32 radius_squared;
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
        {
            f32 duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
            object->context_timer = duration;
            if (duration <= 0.0f)
                object->context_timer = 2.0f;
            object->field_db0 = 0.0f;
        }
    } else if (use != 0 && (object->flags_1fc & 0x80) != 0 && (object->character_data->model_flags & 0x20) != 0 &&
               object->field_db0 <= 0.0f) {
        GizPanel_PlaySfx("TC14_VLN", &object->collision_position, 1 << object->field_280);
        object->field_db0 = 0.5f;
    }
}

i32 __cdecl GizPanel_UpdateHint(HINT_PC *hint) {
    u8 storm = 0, bountyhunter = 0, droid = 0;
    i32 index;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
        return 0;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object != NULL) {
            f32 distance;
            HINT_PANEL_PC *panel =
                (HINT_PANEL_PC *)GizPanel_FindNearest(WORLD, &object->position, object, &distance, 0);
            if (panel != NULL && (panel->flags & 8) != 0 && (panel->flags & 3) == 0) {
                volatile f32 radius =
                    (f32)(((f64)((GAMEOBJECT *)Player[index])->bounds_radius + 1.0) * panel->target_scale);
                volatile f32 radius_squared = radius * radius;
                if (radius_squared > distance) {
                    switch (panel->model_variant) {
                        case 0:
                        case 1:
                            droid = 1;
                            break;
                        case 2:
                            bountyhunter = 1;
                            break;
                        case 3:
                            storm = 1;
                            break;
                    }
                }
            }
        }
    }
    if (storm != 0 && ((u16)hint->id == 0x260 || (u16)hint->id == 0x269)) {
        if (AvailableToPlayer(0, 1, 5, 0) != 0)
            return (u16)hint->id == 0x260;
        return (u16)hint->id == 0x269;
    }
    if (bountyhunter != 0 && ((u16)hint->id == 0x261 || (u16)hint->id == 0x26a)) {
        if (AvailableToPlayer(0x1000000, -1, 6, 0) == 0)
            return (u16)hint->id == 0x26a;
        return (u16)hint->id == 0x261;
    }
    if (droid != 0 && (u16)hint->id == 0x25f) {
        if ((LswForceUsedHintFlagsPc & 2) == 2)
            return 1;
        return 0;
    }
    return 0;
}

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

void __cdecl GizPanels_Reset(void *world_pointer, void *unused, void *progress_pointer) {
    typedef union PanelProgressFlagsPc {
        u8 value;
        struct {
            u8 unknown_0 : 2;
            u8 progress_2 : 1;
            u8 progress_3 : 1;
            u8 unknown_4 : 4;
        } bits;
    } PanelProgressFlagsPc;
    typedef struct PanelProgressStatePc {
        u8 unknown_00[0x62];
        PanelProgressFlagsPc flags;
    } PanelProgressStatePc;
    STATIC_ASSERT(sizeof(PanelProgressFlagsPc) == 1);
    STATIC_ASSERT(offsetof(PanelProgressStatePc, flags) == 0x62);
    STATIC_ASSERT(sizeof(PanelProgressStatePc) == 0x63);
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
            PanelProgressStatePc *state = (PanelProgressStatePc *)panel;
            i32 word = index / 32;
            u32 bit = 1u << (index % 32);
            if (progress[word] & bit)
                panel->flags |= 2;
            state->flags.bits.progress_2 = (progress[word + 2] & bit) != 0;
            state->flags.bits.progress_3 = (progress[word + 1] & bit) != 0;
        }
    }
}

void __cdecl GizPanel_Activate(void *input, i32 active) {
    typedef union PanelActiveFlagsPc {
        u8 value;
        struct {
            u8 unknown_0 : 3;
            u8 active : 1;
            u8 unknown_4 : 4;
        } bits;
    } PanelActiveFlagsPc;
    typedef struct PanelActiveStatePc {
        u8 unknown_00[0x62];
        PanelActiveFlagsPc flags;
    } PanelActiveStatePc;
    STATIC_ASSERT(sizeof(PanelActiveFlagsPc) == 1);
    STATIC_ASSERT(offsetof(PanelActiveStatePc, flags) == 0x62);
    STATIC_ASSERT(sizeof(PanelActiveStatePc) == 0x63);
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    PANEL_MOVEMENT_PC *panel;
    if (gizmo != NULL && gizmo->object != NULL) {
        panel = (PANEL_MOVEMENT_PC *)gizmo->object;
        ((PanelActiveStatePc *)panel)->flags.bits.active = active != 0;
        if (active != 0)
            GizPanel_Reset(panel);
    }
}
static ADDGIZMOTYPE_PC GizmoRegistryGizPanelAddTypePc = {0};
const char GizmoRegistryGizPanelNamePc[] = "Panel";

ADDGIZMOTYPE_PC *__cdecl GizPanel_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizPanelAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.get_pos_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    gizpanel_gizmotype_id = type_id;
    addtype->name = (char *)GizmoRegistryGizPanelNamePc;
    addtype->prefix = "";
    addtype->fns.options = 0xc;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizPanel_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizPanel_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizPanel_Update;
    addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizPanel_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizPanel_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizPanel_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizPanel_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizPanel_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizPanel_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizPanel_SetVisibility;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizPanels_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizPanels_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizPanels_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizPanels_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizPanels_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizPanel_Load;
    return addtype;
}
