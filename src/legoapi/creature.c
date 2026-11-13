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

static void __cdecl CreatureCrate_Stop(PART_PC *part) {
    GAMEOBJECT *object = part->owner;
    u16 debris;
    i32 type;
    if (object == NULL)
        return;
    debris = (u16)FindGameDebris(WORLD->game_debris_sys, "CRATE_POP");
    if (debris != 0xffff)
        AddGameDebris(WORLD->game_debris_sys, (i16)debris, &part->position);
    type = PARTLookupType("CRATE_PART");
    if (type != -1)
        AddFiniteShotPART(type, &part->position, 1);
    object->flags_1fc_word |= 0x1000;
    object->ai_packet.reset_mode = 2;
    PlaySfx("Explode1", &part->position);
    GameCam_Judder(GameCam, 0.1f, 0, NULL);
    NewRumbleAllPlayers(0.0f, 0.0f, 2, 0);
}

static void __cdecl CreatureCrate_MoveFn(PART_PC *part, f32 elapsed) {
    CREATURE_CRATE_PART_VIEW_PC *crate = (CREATURE_CRATE_PART_VIEW_PC *)part;
    crate->spawn_delay -= elapsed;
    if (crate->spawn_delay <= 0.0f) {
        part->move_fn = NULL;
        part->draw_fn = NULL;
    }
}

static i32 __cdecl CreatureCrate_DrawFn(PART_PC *part) {
    if (((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay > 0.0f)
        return 0;
    return 1;
}

void __cdecl SpawnCreatureFromCrate(GAMEOBJECT *object, f32 height, f32 delay) {
    NUVEC velocity = {0.0f, 0.0f, 0.0f};
    NUMTX matrix;
    ADDPART_PC parameters;
    PART_PC *part;
    if (NuSpecialExistsFn(&WORLD->level_objects[0xe6].special) == 0)
        return;
    NuMtxSetTranslation(&matrix, &object->collision_position);
    matrix.m31 += height;
    parameters = Default_ADDPART;
    parameters.matrix = &matrix;
    parameters.velocity = &velocity;
    parameters.field_14 = 0.1f;
    parameters.field_18 = 0.1f;
    parameters.special = &WORLD->level_objects[0xe6].special;
    parameters.time_step = FRAMETIME;
    parameters.gravity = -2.0f;
    parameters.flags = 0x313;
    parameters.owner = object;
    parameters.draw_fn = CreatureCrate_DrawFn;
    parameters.move_fn = CreatureCrate_MoveFn;
    parameters.stop_fn = CreatureCrate_Stop;
    part = AddPart(&parameters);
    if (part == NULL)
        return;
    part->force_flags = (u16)ObjHitObj_Flags(object);
    if (delay > 0.0f) {
        ((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay = delay;
    } else {
        part->move_fn = NULL;
        part->draw_fn = NULL;
        ((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay = 0.0f;
    }
    object->flags_1fc_word &= ~0x1000u;
    object->ai_packet.reset_mode = 3;
}
