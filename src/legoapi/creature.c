#include "saga.h"

typedef struct level_ai_progress_pc_s {
    u8 unknown_0000[0x2808];
    union {
        u32 disabled_ai_object_mask[2];
        u64 disabled_ai_objects;
    };
} LEVEL_AI_PROGRESS_PC;
STATIC_ASSERT(offsetof(LEVEL_AI_PROGRESS_PC, disabled_ai_object_mask) == 0x2808);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, reset_mode) == 0x406);

void __cdecl AlertSurroundingCreatures(GAMEOBJECT *object, NUVEC *position) {
    if (object != NULL &&
        ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || (object->flags_1f8 & 5) != 0 ||
         (object->character_data->game_character->flags_090 & 0x8000) != 0))
        return;
    alert_obj = object;
    alert_pos = *position;
    alert_timer = alert_time;
}

typedef struct AI_CREATURE_RESET_VIEW_PC {
    u8 unknown_00[0x10];
    char script_name[0x10];
    u8 unknown_20[0x2c];
    u8 creature_set;
    u8 unknown_4d[4];
    u8 count_across;
} AI_CREATURE_RESET_VIEW_PC;
STATIC_ASSERT(offsetof(AI_CREATURE_RESET_VIEW_PC, script_name) == 0x10);
STATIC_ASSERT(offsetof(AI_CREATURE_RESET_VIEW_PC, creature_set) == 0x4c);
STATIC_ASSERT(offsetof(AI_CREATURE_RESET_VIEW_PC, count_across) == 0x51);
typedef struct AI_RESET_EXTRA_PC {
    u8 unknown_000[0x1086];
    u8 search_state;
    u8 avoidance_state;
    u8 unknown_1088[8];
    f32 reset_timer;
} AI_RESET_EXTRA_PC;
STATIC_ASSERT(offsetof(AI_RESET_EXTRA_PC, search_state) == 0x1086);
STATIC_ASSERT(offsetof(AI_RESET_EXTRA_PC, avoidance_state) == 0x1087);
STATIC_ASSERT(offsetof(AI_RESET_EXTRA_PC, reset_timer) == 0x1090);
STATIC_ASSERT(offsetof(AIPACKET_PC, movement_destination) == 0x104);
STATIC_ASSERT(offsetof(AIPACKET_PC, movement_stopping_distance) == 0x110);
STATIC_ASSERT(offsetof(AILOCATOR_PC, rotation) == 0x1c);
STATIC_ASSERT(offsetof(AILOCATOR_PC, path_info) == 0x20);
STATIC_ASSERT(offsetof(AIROW_PC, is_alive) == 0x2d);
STATIC_ASSERT(offsetof(AIROW_PC, flags) == 0x30);

typedef struct AI_CREATURE_RESET_FLAGS_PC {
    u8 unknown_0000[0x1386];
    u16 reset_flags;
} AI_CREATURE_RESET_FLAGS_PC;
STATIC_ASSERT(offsetof(AI_CREATURE_RESET_FLAGS_PC, reset_flags) == 0x1386);

typedef struct creature_crate_part_view_pc_s {
    u8 unknown_000[0x20c];
    f32 spawn_delay;
} CREATURE_CRATE_PART_VIEW_PC;
STATIC_ASSERT(offsetof(CREATURE_CRATE_PART_VIEW_PC, spawn_delay) == 0x20c);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(PART_PC, move_fn) == 0x1b0);
STATIC_ASSERT(offsetof(PART_PC, draw_fn) == 0x1d0);
