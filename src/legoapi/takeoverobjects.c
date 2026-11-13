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
    i32 level;
    if ((object->character_data->game_character->flags_090 & 0x80) != 0 || (object->flags_1f8 & 0x4000) == 0 ||
        (WORLD->current_level->flags & 0x100000) != 0)
        return;
    level = WORLD->current_level->area_level_index;
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
    GAMEOBJECT *object;
    if (netclient != 0)
        return;
    level = WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        object = takeoverobjects[index].object;
        if (object == NULL)
            continue;
        takeoverobjects[index].last_safe_position = object->last_safe_position;
        takeoverobjects[index].heading = object->field_0x276;
        if (object->rider != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
            object->rider->character_context == LEGOCONTEXT_BEENTAKENOVER && object->field_280 != 0xff &&
            Door_Last != NULL && ((struct DOOR_s *)Door_Last)->takeover_character_mask != 0 &&
            (((struct DOOR_s *)Door_Last)->takeover_character_mask == CharacterMaskAllPc ||
             ((character_index = (u8)Area_CharIDInCurrentList((i16)object->id)) < 63 &&
              (((struct DOOR_s *)Door_Last)->takeover_character_mask & ((u64)1 << character_index)) != 0)))
            takeoverobjects[index].contact_index = takeoverobjects[index].object->field_280;
        else
            takeoverobjects[index].contact_index = 0xff;
        takeoverobjects[index].hitpoints = takeoverobjects[index].object->current_hp;
        if (takeoverobjects[index].hitpoints == 0 && takeoverobjects[index].object->field_107e != 0)
            takeoverobjects[index].hitpoints = takeoverobjects[index].object->field_107e;
        takeoverobjects[index].current_level = (u8)level;
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
    GAMEOBJECT *object, *controller, *candidate;
    if (netclient != 0)
        return;
    level = WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        if (restore_progress != 0 || takeoverobjects[index].current_level != level)
            takeoverobjects[index].object = NULL;
        if (takeoverobjects[index].contact_index != 0xff)
            takeoverobjects[index].current_level = (u8)level;
        if (takeoverobjects[index].registered_level == level) {
            if (takeoverobjects[index].object == NULL) {
                if (takeoverobjects[index].source_creature == 0xff) {
                    if (takeoverobjects[index].current_level == level) {
                        takeoverobjects[index].object = AddDynamicCreature(
                            takeoverobjects[index].character_id, &takeoverobjects[index].last_safe_position,
                            takeoverobjects[index].heading, takeoverobjects[index].script_name, NULL, NULL, 1, NULL,
                            NULL, 0, 0);
                    }
                } else {
                    candidate = Obj;
                    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++candidate) {
                        if (candidate->ai_packet.creature_index == takeoverobjects[index].source_creature) {
                            takeoverobjects[index].object = candidate;
                            break;
                        }
                    }
                }
            }
            if (takeoverobjects[index].object != NULL && takeoverobjects[index].current_level != level) {
                KillGameObject(takeoverobjects[index].object, 5, 0);
                takeoverobjects[index].object = NULL;
            }
        } else if (takeoverobjects[index].current_level == level && takeoverobjects[index].object == NULL) {
            takeoverobjects[index].object = AddDynamicCreature(
                takeoverobjects[index].character_id, &takeoverobjects[index].last_safe_position,
                takeoverobjects[index].heading, takeoverobjects[index].script_name, NULL, NULL, 1, NULL, NULL, 0, 0);
        }
        object = takeoverobjects[index].object;
        if (object != NULL) {
            object->current_hp = takeoverobjects[index].hitpoints;
            if (!(takeoverobjects[index].contact_index != 0xff &&
                  (controller = (GAMEOBJECT *)Player[takeoverobjects[index].contact_index]) != NULL)) {
                SnapCreaturePos(takeoverobjects[index].object, &takeoverobjects[index].last_safe_position,
                                takeoverobjects[index].heading, NULL, 1);
            } else {
                takeoverobjects[index].object->dead = 0;
                takeoverobjects[index].object->flags_1fc_word |= 0x1000;
                takeoverobjects[index].object->flags_1fc_word |= 1;
                takeoverobjects[index].object->ai_packet.reset_mode = 2;
                SnapCreaturePos(takeoverobjects[index].object, &controller->position, controller->field_0x276,
                                &controller->ai_packet.path_info, 1);
                InitPlayerAI(controller);
                TakeOverGameObject(controller, takeoverobjects[index].object, 0, 1);
            }
        }
    }
}
