#include "saga.h"

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

typedef struct panel_load_system_pc_s {
    i32 count;
    PANEL_MOVEMENT_PC *panels;
} PANEL_SYSTEM_PC;
STATIC_ASSERT(sizeof(PANEL_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(PANEL_SYSTEM_PC, panels) == 4);

STATIC_ASSERT(sizeof(PANEL_MOVEMENT_PC) == 0x90);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, floor_position) == 0x64);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, arm_x_rotation) == 0x80);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_pitch) == 0x82);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_roll) == 0x84);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, activation_time) == 0x88);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);

STATIC_ASSERT(sizeof(PANEL_MOVEMENT_PC) == 0x90);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);

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
