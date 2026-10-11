#include "saga.h"

typedef struct level_ai_progress_pc_s {
    u8 unknown_0000[0x2808];
    u32 disabled_ai_object_mask[2];
} LEVEL_AI_PROGRESS_PC;
STATIC_ASSERT(offsetof(LEVEL_AI_PROGRESS_PC, disabled_ai_object_mask) == 0x2808);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, reset_mode) == 0x406);

void __cdecl StoreProgressAICharacter(LEVEL_PROGRESS *progress) {
    LEVEL_AI_PROGRESS_PC *saved = (LEVEL_AI_PROGRESS_PC *)progress;
    GAMEOBJECT *object = Obj;
    i32 index;

    if (saved == NULL) {
        return;
    }
    saved->disabled_ai_object_mask[0] = 0;
    saved->disabled_ai_object_mask[1] = 0;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0 && object->ai_packet.reset_mode == 4) {
            u64 bit = (u64)1 << index;
            saved->disabled_ai_object_mask[0] |= (u32)bit;
            saved->disabled_ai_object_mask[1] |= (u32)(bit >> 32);
        }
    }
}

void AICreatureResumeScript(GAMEOBJECT *object) {
    AISCRIPT_PC *script = object->ai_packet.script_processor.base_script;
    if (script != NULL) {
        AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, NULL, NULL, NULL,
                              0, script, script->base_state);
        object->ai_packet.script_processor.active_ref_count = 0;
    }
}

extern NUVEC alert_pos;
extern f32 alert_time;
void __cdecl AlertSurroundingCreatures(GAMEOBJECT *object, NUVEC *position) {
    if (object != NULL && ((object->flags_1fc_word & 0x1001) != 0x1001 || (object->flags_1f8 & 5) != 0 ||
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
extern NUVEC plr_lastpos;

void __cdecl ResetAICreature(GAMEOBJECT *object, AISYS_PC *system) {
    AIPACKET_PC *packet;
    AICREATURE_PC *creature;
    AI_CREATURE_RESET_VIEW_PC *details;
    AIPATHINFO_PC *path_info;
    AIGROUP_PC *group;
    AIROW_PC *row;
    APIOBJECT_PC *api;
    NUVEC offset;
    i32 angle, column;
    u64 alive;
    if (object == NULL)
        return;
    PreResetCode(object);
    PostResetCode(object);
    if (system == NULL)
        return;
    InitPlayerAI(object);
    ResetPlayerMoves(object);
    Player_ClearContext(object, 1);
    Player_ResetContexts(object->unknown_06c4);
    api = (APIOBJECT_PC *)object;
    packet = &object->ai_packet;
    object->field_107e = object->character_data->game_character->hitpoints;
    object->current_hp = (i8)object->character_data->game_character->hitpoints;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    creature = &system->creatures[packet->creature_index];
    details = (AI_CREATURE_RESET_VIEW_PC *)creature;
    packet->creature_set = details->creature_set;
    if ((creature->flags & 1) != 0)
        packet->flags |= 0x80;
    else
        packet->flags &= ~0x80u;
    packet->flags &= ~0x40000u;
    api->view_distance = creature->view_distance;
    api->hear_distance = creature->hear_distance;
    api->max_view_height = creature->max_view_height;
    api->min_view_height = creature->min_view_height;
    offset.x = (f32)((packet->group_column + 1) / 2) * creature->x_spacing;
    if ((packet->group_column & 1) != 0)
        offset.x = -offset.x;
    offset.y = 0.0f;
    offset.z = -(f32)packet->group_row * creature->z_spacing;
    if (packet->respawn_count != 0 && packet->respawn_locator != NULL) {
        angle = packet->respawn_locator->rotation;
        NuVecRotateY(&offset, &offset, angle);
        NuVecAdd(&object->position, &offset, &packet->respawn_locator->position);
        path_info = &packet->respawn_locator->path_info;
        packet->respawn_locator = NULL;
    } else {
        angle = creature->y_rotation;
        NuVecRotateY(&offset, &offset, angle);
        NuVecAdd(&object->position, &offset, &creature->position);
        path_info = &creature->path_info;
    }
    object->collision_position = object->position;
    object->initial_position = object->position;
    plr_lastpos = object->collision_position;
    object->start_position = object->collision_position;
    object->reset_velocity = v000;
    object->velocity = object->reset_velocity;
    InitSurfaceInfo(object);
    SetObjOnSurface(object, 0);
    object->field_0x276 = (u16)angle;
    object->facing_angle = (u16)angle;
    object->movement_facing_angle = (u16)angle;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, (u16)angle);
    AISysCharacterSetPath(packet, path_info->path);
    AISysCharacterSetPathCnx(packet, &object->position, path_info->connection, path_info->direction);
    object->dead = 0;
    AIScriptProcessorInit(WORLD->ai_sys, packet, &packet->script_processor, creature, details->script_name, NULL, 1,
                          NULL, NULL);
    packet->inside_path_node = -1;
    packet->current_route = 0xff;
    packet->next_route = 0;
    AISysGetCharacterPathPos(WORLD->ai_sys, api, packet, 0xff, (i8)object->field_281);
    ResetAnimPacket(&object->anim_packet, 1);
    SetAnimTimeRandom(object->character_model, &object->anim_packet);
    packet->movement_stopping_distance = 0.0f;
    ((AI_RESET_EXTRA_PC *)object)->reset_timer = 0.0f;
    packet->flags &= ~0x1000000u;
    packet->movement_destination = object->position;
    packet->movement_target = NULL;
    ((AI_RESET_EXTRA_PC *)object)->search_state = 0;
    ((AI_RESET_EXTRA_PC *)object)->avoidance_state = 0;
    PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
    object->flags_1fc_word |= 0x1000;
    ++packet->respawn_count;
    packet->reset_mode = 2;
    group = packet->group;
    if (group != NULL) {
        alive = packet->group_member_index < 64 ? (u64)1 << packet->group_member_index : 0;
        group->member_is_alive |= (i32)alive;
        row = &group->rows[packet->group_row];
        column = packet->group_member_index - details->count_across * packet->group_row;
        alive = (u8)column < 64 ? (u64)1 << (u8)column : 0;
        row->is_alive |= (u8)alive;
        if (packet->group_column == 0) {
            row->position = object->position;
            row->path_info = packet->path_info;
            row->rotation = object->field_0x276;
            row->flags &= ~2u;
            row->next_connection = NULL;
        }
    }
    object->previous_position = object->position;
    object->ai_update_position = object->position;
    if ((object->character_data->model_flags & 0x20000000) != 0 &&
        object->character_model->action_animations[0x41] != NULL) {
        object->context_timer = 1000000000.0f;
        object->character_context = 0x17;
        object->field_7aa = 0x41;
        ResetAnimPacket(&object->anim_packet, 0x41);
    }
}

typedef struct AI_CREATURE_RESET_FLAGS_PC {
    u8 unknown_0000[0x1386];
    u16 reset_flags;
} AI_CREATURE_RESET_FLAGS_PC;
STATIC_ASSERT(offsetof(AI_CREATURE_RESET_FLAGS_PC, reset_flags) == 0x1386);
extern const char GameObjectState_InActive[9];

void __cdecl ResetAICreatures(AISYS_PC *system) {
    GAMEOBJECT *object;
    AICREATURE_PC *creature;
    AI_CREATURE_RESET_VIEW_PC *details;
    AIPACKET_PC *packet;
    AISCRIPTPROCESS_PC *processor;
    i32 index;
    u64 bit;
    LEVEL_AI_PROGRESS_PC *progress;
    if (system == NULL || (((AI_CREATURE_RESET_FLAGS_PC *)system)->reset_flags & 1) != 0)
        return;
    ((AI_CREATURE_RESET_FLAGS_PC *)system)->reset_flags |= 1;
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1f8 & 0x400) == 0)
            continue;
        object->flags_1fc_word &= ~0x1000u;
        progress = (LEVEL_AI_PROGRESS_PC *)WORLD->level_progress;
        bit = (u8)index < 64 ? (u64)1 << (u8)index : 0;
        if (progress != NULL && (((u32)bit & progress->disabled_ai_object_mask[0]) != 0 ||
                                 ((u32)(bit >> 32) & progress->disabled_ai_object_mask[1]) != 0)) {
            object->dead = 1;
            object->ai_packet.reset_mode = 4;
            continue;
        }
        packet = &object->ai_packet;
        creature = &system->creatures[packet->creature_index];
        details = (AI_CREATURE_RESET_VIEW_PC *)creature;
        packet->respawn_count = 0;
        packet->reset_mode = 0;
        processor = &packet->script_processor;
        AIScriptProcessorInit(WORLD->ai_sys, packet, processor, creature, details->script_name,
                              (char *)GameObjectState_InActive, 1, NULL, NULL);
        if (processor->state != NULL && processor->state->name != NULL &&
            NuStrICmp(processor->state->name, GameObjectState_InActive) == 0)
            creature->activation_type = 2;
        if (creature->activation_type != 0)
            continue;
        if (Game.difficulty < creature->activation_difficulty) {
            packet->reset_mode = 4;
            continue;
        }
        if (creature->count > 1 && creature->start_stagger > 0.0f && packet->group_member_index != 0) {
            packet->reset_mode = 1;
            packet->spawn_delay = (f32)packet->group_member_index * creature->start_stagger;
            continue;
        }
        ResetAICreature(object, system);
    }
}

typedef struct creature_crate_part_view_pc_s {
    u8 unknown_000[0x20c];
    f32 spawn_delay;
} CREATURE_CRATE_PART_VIEW_PC;
STATIC_ASSERT(offsetof(CREATURE_CRATE_PART_VIEW_PC, spawn_delay) == 0x20c);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(PART_PC, move_fn) == 0x1b0);
STATIC_ASSERT(offsetof(PART_PC, draw_fn) == 0x1d0);

static void __cdecl CreatureCrate_StopPc(PART_PC *part) {
    GAMEOBJECT *object = part->owner;
    i32 debris, type;
    if (object == NULL)
        return;
    debris = (i16)FindGameDebris(WORLD->game_debris_sys, "CRATE_POP");
    if (debris != -1)
        AddGameDebris(WORLD->game_debris_sys, debris, &part->position);
    type = PARTLookupType("CRATE_PART");
    if (type != -1)
        AddFiniteShotPART(type, &part->position, 1);
    object->flags_1fc_word |= 0x1000;
    object->ai_packet.reset_mode = 2;
    PlaySfx("Explode1", &part->position);
    GameCam_Judder(GameCam, 0.1f, 0, NULL);
    NewRumbleAllPlayers(0.0f, 0.0f, 2, 0.0f);
}

static void __cdecl CreatureCrate_MovePc(PART_PC *part, f32 elapsed) {
    CREATURE_CRATE_PART_VIEW_PC *crate = (CREATURE_CRATE_PART_VIEW_PC *)part;
    crate->spawn_delay -= elapsed;
    if (crate->spawn_delay <= 0.0f) {
        part->move_fn = NULL;
        part->draw_fn = NULL;
    }
}

static i32 __cdecl CreatureCrate_DrawPc(PART_PC *part) {
    return !(((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay > 0.0f);
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
    parameters.time_step = FRAMETIME;
    parameters.special = &WORLD->level_objects[0xe6].special;
    parameters.gravity = -2.0f;
    parameters.flags = 0x313;
    parameters.owner = object;
    parameters.draw_fn = CreatureCrate_DrawPc;
    parameters.move_fn = CreatureCrate_MovePc;
    parameters.stop_fn = CreatureCrate_StopPc;
    part = AddPart(&parameters);
    if (part == NULL)
        return;
    part->force_flags = (u16)ObjHitObj_Flags(object);
    if (!(delay > 0.0f)) {
        part->move_fn = NULL;
        part->draw_fn = NULL;
        delay = 0.0f;
    }
    ((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay = delay;
    object->flags_1fc_word &= ~0x1000u;
    object->ai_packet.reset_mode = 3;
}
