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

GAMEOBJECT *CharPlatform_FindObjFromPlatID(CHARPLATFORMSYS_s *platforms, i32 id) {
    i32 i;
    if (platforms == NULL) {
        return NULL;
    }
    for (i = 0; i < platforms->platform_count; ++i) {
        if (platforms->platforms[i].object != NULL && platforms->platforms[i].platform_id == id) {
            return platforms->platforms[i].object;
        }
    }
    return NULL;
}

void CharPlatforms_Configure(WORLDINFO *world, char *config) {
    CHARPLATFORMSYS_s *system;
    NUFPAR *parser;
    world->shadow_platform_list = NULL;
    if (world->current_gscn == NULL) {
        return;
    }
    parser = NuFParCreateMem("CharPlatforms", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    world->shadow_platform_list = (CHARPLATFORMSYS_s *)world->giz_buffer.addr;
    world->shadow_platform_list->scene = world->current_gscn;
    world->shadow_platform_list->platform_count = 0;
    while (NuFParGetLine(parser) != 0) {
        struct CHARPLATFORM_s *platform;
        if (NuFParGetWord(parser) == 0) {
            break;
        }
        if (NuStrICmp(parser->word_buffer_ptr, "char_platform") != 0 || NuFParGetWord(parser) == 0) {
            continue;
        }
        system = world->shadow_platform_list;
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
        ++world->shadow_platform_list->platform_count;
    }
    NuFParDestroy(parser);
    if (world->shadow_platform_list->platform_count > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(CHARPLATFORMSYS_s) +
                                  (world->shadow_platform_list->platform_count - 1) * sizeof(struct CHARPLATFORM_s) + 3U) &
                                 ~3U;
    } else {
        world->shadow_platform_list = NULL;
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
