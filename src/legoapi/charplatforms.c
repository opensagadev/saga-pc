#include "saga.h"

typedef struct terrain_player_platform_record_pc_s { // size 0x14
    u8 unknown_0[0xe];                               // 0x00
    i16 platform_id;                                 // 0x0e
    GAMEOBJECT *object;                              // 0x10
} TERRAIN_PLAYER_PLATFORM_RECORD_PC;

typedef struct terrain_player_platform_sys_pc_s { // size 0x1c
    u8 unknown_0[4];                              // 0x00
    i32 count;                                    // 0x04
    TERRAIN_PLAYER_PLATFORM_RECORD_PC entries[1]; // 0x08
} TERRAIN_PLAYER_PLATFORM_SYS_PC;

GAMEOBJECT *CharPlatform_FindObjFromPlatID(void *system, i32 id) {
    TERRAIN_PLAYER_PLATFORM_SYS_PC *platforms = system;
    i32 i;
    if (platforms == NULL) {
        return NULL;
    }
    for (i = 0; i < platforms->count; ++i) {
        if (platforms->entries[i].object != NULL && platforms->entries[i].platform_id == id) {
            return platforms->entries[i].object;
        }
    }
    return NULL;
}

typedef struct EQUIVALENT_GROUP_PC { // size 0x10
    i16 object_count;                // 0x0
    i16 byte_size;                   // 0x2
    NUSPECIALHANDLE objects[1];      // 0x4
} EQUIVALENT_GROUP_PC;

typedef struct EQUIVALENT_WORLD_VIEW_PC {   // size 0x46b8
    u8 unknown_0[0x46b0];                   // 0x0000
    EQUIVALENT_GROUP_PC *equivalent_groups; // 0x46b0
    i32 equivalent_group_count;             // 0x46b4
} EQUIVALENT_WORLD_VIEW_PC;

typedef struct CHAR_PLATFORM_PC { // size 0x14
    NUSPECIALHANDLE special;      // 0x00
    i16 object_id;                // 0x0c
    i16 platform_id;              // 0x0e
    GAMEOBJECT *object;           // 0x10
} CHAR_PLATFORM_PC;

typedef struct CHAR_PLATFORM_SYSTEM_PC { // size 0x1c
    NUGSCN *scene;                       // 0x00
    i32 platform_count;                  // 0x04
    CHAR_PLATFORM_PC platforms[1];       // 0x08
} CHAR_PLATFORM_SYSTEM_PC;

typedef struct CHAR_PLATFORM_WORLD_PC {         // size 0x2ae0
    u8 unknown_0[0x2adc];                       // 0x0000
    CHAR_PLATFORM_SYSTEM_PC *char_platform_sys; // 0x2adc
} CHAR_PLATFORM_WORLD_PC;

void CharPlatforms_Configure(WORLDINFO *world, char *config) {
    CHAR_PLATFORM_WORLD_PC *view = (CHAR_PLATFORM_WORLD_PC *)world;
    CHAR_PLATFORM_SYSTEM_PC *system;
    NUFPAR_PC *parser;
    view->char_platform_sys = NULL;
    if (world->current_gscn == NULL) {
        return;
    }
    parser = NuFParCreateMem("CharPlatforms", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    view->char_platform_sys = (CHAR_PLATFORM_SYSTEM_PC *)world->giz_buffer.addr;
    view->char_platform_sys->scene = world->current_gscn;
    view->char_platform_sys->platform_count = 0;
    while (NuFParGetLine(parser) != 0) {
        CHAR_PLATFORM_PC *platform;
        if (NuFParGetWord(parser) == 0) {
            break;
        }
        if (NuStrICmp(parser->word_buffer_ptr, "char_platform") != 0 || NuFParGetWord(parser) == 0) {
            continue;
        }
        system = view->char_platform_sys;
        platform = &system->platforms[system->platform_count];
        platform->object_id = CharIDFromName(parser->word_buffer_ptr);
        if (platform->object_id == -1 || NuFParGetWord(parser) == 0) {
            continue;
        }
        if (NuSpecialFind(world->current_gscn, &platform->special, parser->word_buffer_ptr, 1) == 0) {
            continue;
        }
        platform->platform_id = -1;
        platform->object = NULL;
        ++view->char_platform_sys->platform_count;
    }
    NuFParDestroy(parser);
    if (view->char_platform_sys->platform_count > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(CHAR_PLATFORM_SYSTEM_PC) +
                                  (view->char_platform_sys->platform_count - 1) * sizeof(CHAR_PLATFORM_PC) + 3U) &
                                 ~3U;
    } else {
        view->char_platform_sys = NULL;
    }
}

void CharPlatforms_Reset(CHARPLATFORMSYS_s *system) {
    struct gameobject_s *object_cursor;
    i32 i;
    if (system == NULL) {
        return;
    }

    object_cursor = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object_cursor) {
        if ((object_cursor->flags_1fc_half & 1) != 0) {
            object_cursor->level_object_index = -1;
        }
    }

    for (i = 0; i < system->platform_count; ++i) {
        NuSpecialSetVisibility(&system->platforms[i].special, 0);
        system->platforms[i].platform_id = FindPlatInst(NuSpecialGetInstanceix(&system->platforms[i].special));
        system->platforms[i].object = NULL;
        if (system->platforms[i].platform_id != -1) {
            GAMEOBJECT *object = FindGameObject(system->platforms[i].object_id, 0, 1, 0, 1);
            if (object != NULL) {
                object->level_object_index = system->platforms[i].platform_id;
                system->platforms[i].object = object;
            }
        }
    }
}

void CharPlatforms_Update(CHARPLATFORMSYS_s *system) {
    i32 i;
    if (system == NULL) {
        return;
    }
    for (i = 0; i < system->platform_count; ++i) {
        i32 visible;
        struct gameobject_s *object;
        i32 j;
        if (system->platforms[i].object_id == -1) {
            continue;
        }
        visible = 0;
        object = Obj;
        for (j = 0; j < HIGHGAMEOBJECT; ++j, ++object) {
            if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
                object->level_object_index == system->platforms[i].platform_id) {
                nuhspecial_s *special = &system->platforms[i].special;
                NUMTX *matrix = NuSpecialGetDrawMtx(special);
                if (matrix != NULL) {
                    f32 rise;
                    *matrix = object->transform;
                    rise = GameTimer.time_elapsed * 2.0f;
                    if (rise < 1.0f) {
                        matrix->m31 -= 1.0f - rise;
                    }
                    NuSpecialUpdate(special);
                }
                visible = 1;
                break;
            }
        }
        NuSpecialSetVisibility(&system->platforms[i].special, visible);
    }
}
