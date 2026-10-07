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
