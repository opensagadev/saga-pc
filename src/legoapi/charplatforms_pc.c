#include "saga.h"
typedef struct terrain_player_platform_record_pc_s {
    u8 unknown_0[0xe];
    i16 platform_id;
    GAMEOBJECT *object;
} TERRAIN_PLAYER_PLATFORM_RECORD_PC;
typedef struct terrain_player_platform_sys_pc_s {
    u8 unknown_0[4];
    i32 count;
    TERRAIN_PLAYER_PLATFORM_RECORD_PC entries[1];
} TERRAIN_PLAYER_PLATFORM_SYS_PC;
STATIC_ASSERT(sizeof(TERRAIN_PLAYER_PLATFORM_RECORD_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, platform_id) == 0xe);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, object) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, count) == 4);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, entries) == 8);
GAMEOBJECT *__cdecl CharPlatform_FindObjFromPlatID(void *system, i32 id) {
    TERRAIN_PLAYER_PLATFORM_SYS_PC *platforms = (TERRAIN_PLAYER_PLATFORM_SYS_PC *)system;
    i32 i;
    if (platforms != NULL) {
        for (i = 0; i < platforms->count; ++i) {
            if (platforms->entries[i].platform_id == id && platforms->entries[i].object != NULL)
                return platforms->entries[i].object;
        }
    }
    return NULL;
}

typedef struct EQUIVALENT_GROUP_PC {
    i16 object_count;
    i16 byte_size;
    NUSPECIALHANDLE objects[1];
} EQUIVALENT_GROUP_PC;
typedef struct EQUIVALENT_WORLD_VIEW_PC {
    u8 unknown_0[0x46b0];
    EQUIVALENT_GROUP_PC *equivalent_groups;
    i32 equivalent_group_count;
} EQUIVALENT_WORLD_VIEW_PC;
typedef struct CHAR_PLATFORM_PC {
    NUSPECIALHANDLE special;
    i16 object_id;
    i16 platform_id;
    GAMEOBJECT *object;
} CHAR_PLATFORM_PC;
typedef struct CHAR_PLATFORM_SYSTEM_PC {
    NUGSCN *scene;
    i32 platform_count;
    CHAR_PLATFORM_PC platforms[1];
} CHAR_PLATFORM_SYSTEM_PC;
typedef struct CHAR_PLATFORM_WORLD_PC {
    u8 unknown_0[0x2adc];
    CHAR_PLATFORM_SYSTEM_PC *char_platform_sys;
} CHAR_PLATFORM_WORLD_PC;
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, object_count) == 0);
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, byte_size) == 2);
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, objects) == 4);
STATIC_ASSERT(offsetof(EQUIVALENT_WORLD_VIEW_PC, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(EQUIVALENT_WORLD_VIEW_PC, equivalent_group_count) == 0x46b4);
STATIC_ASSERT(sizeof(CHAR_PLATFORM_PC) == 0x14);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, special) == 0);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, object_id) == 0xc);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, platform_id) == 0xe);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, object) == 0x10);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_SYSTEM_PC, scene) == 0);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_SYSTEM_PC, platform_count) == 4);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_SYSTEM_PC, platforms) == 8);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_WORLD_PC, char_platform_sys) == 0x2adc);

void __cdecl CharPlatforms_Configure(WORLDINFO *world, char *config) {
    CHAR_PLATFORM_WORLD_PC *view = (CHAR_PLATFORM_WORLD_PC *)world;
    CHAR_PLATFORM_SYSTEM_PC *system;
    NUFPAR_PC *parser;
    view->char_platform_sys = NULL;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("CharPlatforms", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    system = (CHAR_PLATFORM_SYSTEM_PC *)world->giz_buffer.addr;
    view->char_platform_sys = system;
    system->scene = world->current_gscn;
    system->platform_count = 0;
    while (NuFParGetLine(parser) != 0) {
        CHAR_PLATFORM_PC *platform;
        if (NuFParGetWord(parser) == 0)
            break;
        if (NuStrICmp(parser->word_buffer_ptr, "char_platform") != 0 || NuFParGetWord(parser) == 0)
            continue;
        system = view->char_platform_sys;
        platform = &system->platforms[system->platform_count];
        platform->object_id = CharIDFromName(parser->word_buffer_ptr);
        if (platform->object_id == -1 || NuFParGetWord(parser) == 0)
            continue;
        if (NuSpecialFind(world->current_gscn, &platform->special, parser->word_buffer_ptr, 1) == 0)
            continue;
        platform->platform_id = -1;
        platform->object = NULL;
        ++view->char_platform_sys->platform_count;
    }
    NuFParDestroy(parser);
    system = view->char_platform_sys;
    if (system->platform_count > 0)
        world->giz_buffer.addr = ((u32)&system->platforms[system->platform_count] + 3U) & ~3U;
    else
        view->char_platform_sys = NULL;
}
