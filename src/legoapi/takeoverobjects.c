#include "saga.h"
#include <string.h>

extern i32 num_takeoverobjects;

void __cdecl ClearTakeOverObjectSys(void) {
    memset(takeoverobjects, 0, sizeof(takeoverobjects));
    num_takeoverobjects = 0;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_level_index) == 0xc8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, script_processor) +
                  offsetof(AISCRIPTPROCESS_PC, script) ==
              0x2cc);
STATIC_ASSERT(offsetof(AISCRIPT_PC, name) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);

void __cdecl RegisterTakeOverObject(GAMEOBJECT *object) {
    i32 index;
    AISCRIPT_PC *script;
    u8 level;
    if ((object->character_data->game_character->flags_090 & 0x80) != 0 || (object->flags_1f8 & 0x4000) == 0 ||
        (WORLD->current_level->flags & 0x100000) != 0)
        return;
    level = (u8)WORLD->current_level->area_level_index;
    if (num_takeoverobjects >= 8 || (object->flags_1f8 & 0x400) == 0)
        return;
    for (index = 0; index < num_takeoverobjects; ++index) {
        if (takeoverobjects[index].object == object)
            return;
    }
    takeoverobjects[index].object = object;
    script = object->ai_packet.script_processor.script;
    if (script != NULL && script->name != NULL)
        NuStrNCpy(takeoverobjects[index].script_name, script->name, 16);
    takeoverobjects[index].character_id = object->id;
    takeoverobjects[index].source_creature = object->ai_packet.creature_index;
    takeoverobjects[index].registered_level = level;
    takeoverobjects[index].current_level = level;
    ++num_takeoverobjects;
}

extern void *Door_Last;
extern u64 CharacterMaskAllPc;
i8 __cdecl Area_CharIDInCurrentList(i32 character_id);
void __cdecl StoreStatusTakeOverObjectSys(void) {
    i32 index;
    i32 level;
    u8 character_index;
    TAKEOVEROBJECT_PC *record;
    GAMEOBJECT *object;
    if (netclient != 0)
        return;
    level = WORLD->current_level->area_level_index;
    record = takeoverobjects;
    for (index = 0; index < num_takeoverobjects; ++index, ++record) {
        object = record->object;
        if (object == NULL)
            continue;
        record->last_safe_position = object->last_safe_position;
        record->heading = object->field_0x276;
        if (object->rider != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
            object->rider->character_context == LEGOCONTEXT_BEENTAKENOVER && object->field_280 != 0xff &&
            Door_Last != NULL && ((struct DOOR_s *)Door_Last)->takeover_character_mask != 0) {
            if (((struct DOOR_s *)Door_Last)->takeover_character_mask == CharacterMaskAllPc) {
                record->contact_index = record->object->field_280;
            } else {
                character_index = (u8)Area_CharIDInCurrentList((i16)object->id);
                if (character_index < 63 &&
                    (((struct DOOR_s *)Door_Last)->takeover_character_mask & ((u64)1 << character_index)) != 0)
                    record->contact_index = record->object->field_280;
                else
                    record->contact_index = 0xff;
            }
        } else {
            record->contact_index = 0xff;
        }
        record->hitpoints = record->object->current_hp;
        if (record->hitpoints == 0 && record->object->field_107e != 0)
            record->hitpoints = record->object->field_107e;
        record->current_level = (u8)level;
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, reset_mode) == 0x406);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
void __cdecl ReStoreStatusTakeOverObjectSys(i32 restore_progress) {
    i32 level, index, object_index;
    TAKEOVEROBJECT_PC *record;
    GAMEOBJECT *object, *controller;
    if (netclient != 0)
        return;
    level = (i8)WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        record = &takeoverobjects[index];
        if (restore_progress != 0 || record->current_level != level)
            record->object = NULL;
        if (record->contact_index != 0xff)
            record->current_level = (u8)level;
        if (record->registered_level == level) {
            if (record->object == NULL) {
                if (record->source_creature != 0xff) {
                    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index) {
                        if (Obj[object_index].ai_packet.creature_index == record->source_creature) {
                            record->object = &Obj[object_index];
                            break;
                        }
                    }
                } else if (record->current_level == level) {
                    record->object =
                        AddDynamicCreature(record->character_id, &record->last_safe_position, record->heading,
                                           record->script_name, NULL, NULL, 1, NULL, NULL, 0, 0);
                }
            }
            if (record->object != NULL && record->current_level != level) {
                KillGameObject(record->object, 5, 0);
                record->object = NULL;
            }
        } else if (record->current_level == level && record->object == NULL) {
            record->object = AddDynamicCreature(record->character_id, &record->last_safe_position, record->heading,
                                                record->script_name, NULL, NULL, 1, NULL, NULL, 0, 0);
        }
        object = record->object;
        if (object != NULL) {
            object->current_hp = record->hitpoints;
            if (record->contact_index != 0xff && (controller = (GAMEOBJECT *)Player[record->contact_index]) != NULL) {
                record->object->dead = 0;
                record->object->flags_1fc_word |= 0x1000;
                record->object->flags_1fc_word |= 1;
                record->object->ai_packet.reset_mode = 2;
                SnapCreaturePos(record->object, &controller->position, controller->field_0x276,
                                &controller->ai_packet.path_info, 1);
                InitPlayerAI(controller);
                TakeOverGameObject(controller, record->object, 0, 1);
            } else {
                SnapCreaturePos(record->object, &record->last_safe_position, record->heading, NULL, 1);
            }
        }
    }
}
