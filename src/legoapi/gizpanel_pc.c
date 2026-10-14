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
    u16 field_86;
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
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, field_86) == 0x86);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, activation_time) == 0x88);

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
