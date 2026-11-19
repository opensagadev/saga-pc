#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/charplatforms.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"

typedef struct terrain_player_platform_record_s { // size 0x14
    u8 reserved_00[0xe];                            // 0x00
    i16 platform_id;                              // 0x0e
    GAMEOBJECT *object;                           // 0x10
} TERRAIN_PLAYER_PLATFORM_RECORD;

typedef struct terrain_player_platform_sys_s { // size 0x1c
    u8 reserved_00[4];                           // 0x00
    i32 count;                                 // 0x04
    TERRAIN_PLAYER_PLATFORM_RECORD entries[1]; // 0x08
} TERRAIN_PLAYER_PLATFORM_SYS;

GAMEOBJECT *CharPlatform_FindObjFromPlatID(void *system, i32 id) {
    TERRAIN_PLAYER_PLATFORM_SYS *platforms = system;
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

typedef struct CHAR_PLATFORM { // size 0x14
    nuhspecial_s special;      // 0x00
    i16 object_id;             // 0x0c
    i16 platform_id;           // 0x0e
    GAMEOBJECT *object;        // 0x10
} CHAR_PLATFORM;

typedef struct CHAR_PLATFORM_SYSTEM { // size 0x1c
    NUGSCN *scene;                    // 0x00
    i32 platform_count;               // 0x04
    CHAR_PLATFORM platforms[1];       // 0x08
} CHAR_PLATFORM_SYSTEM;

typedef struct CHAR_PLATFORM_WORLD {         // size 0x2ae0
    u8 reserved_00[0x2adc];                    // 0x0000
    CHAR_PLATFORM_SYSTEM *char_platform_sys; // 0x2adc
} CHAR_PLATFORM_WORLD;

void CharPlatforms_Configure(WORLDINFO *world, char *config) {
    CHAR_PLATFORM_WORLD *view = (CHAR_PLATFORM_WORLD *)world;
    CHAR_PLATFORM_SYSTEM *system;
    NUFPAR *parser;
    view->char_platform_sys = NULL;
    if (world->current_gscn == NULL) {
        return;
    }
    parser = NuFParCreateMem("CharPlatforms", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    view->char_platform_sys = (CHAR_PLATFORM_SYSTEM *)world->giz_buffer.addr;
    view->char_platform_sys->scene = world->current_gscn;
    view->char_platform_sys->platform_count = 0;
    while (NuFParGetLine(parser) != 0) {
        CHAR_PLATFORM *platform;
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
        world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(CHAR_PLATFORM_SYSTEM) +
                                  (view->char_platform_sys->platform_count - 1) * sizeof(CHAR_PLATFORM) + 3U) &
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
        if ((object_cursor->flags_1fc_half & APIOBJECT_FLAG_IN_USE) != 0) {
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
            if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 &&
                object->dead == 0 && object->level_object_index == system->platforms[i].platform_id) {
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
