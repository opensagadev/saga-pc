#include "saga.h"

#include <string.h>

extern u8 snakebodies[0x440];
static NUSPECIALHANDLE snake_hspecials[3];

static i32 __cdecl NuSpecialFindDisplay(NUGSCN *scene, NUSPECIALHANDLE *handle, char *name, i32 flags) {
    NUDISPLAY_LIST_PC *display_list = scene->display_list;
    NUDISPLAY_SPECIAL_PC *special = display_list->specials;
    i32 i;

    (void)flags;
    for (i = 0; i < display_list->special_count; ++i, ++special) {
        if (NuStrICmp(name, special->name) == 0) {
            handle->scene = scene;
            handle->special = NULL;
            handle->display_special = (NUMTX *)special;
            return 1;
        }
    }

    handle->scene = NULL;
    handle->special = NULL;
    handle->display_special = NULL;
    return 0;
}

i32 __cdecl NuSpecialFind(NUGSCN *scene, NUSPECIALHANDLE *handle, char *name, i32 flags) {
    NUSPECIAL_LEGACY_PC *special;
    i32 i;

    (void)flags;
    if (scene != NULL && name != NULL) {
        if (scene->display_list != NULL) {
            return NuSpecialFindDisplay(scene, handle, name, 1);
        }

        special = scene->specials;
        for (i = 0; i < scene->special_count; ++i, ++special) {
            if (NuStrICmp(name, special->name) == 0) {
                handle->scene = scene;
                handle->special = (NUMTX *)special;
                handle->display_special = NULL;
                return 1;
            }
        }
    }

    handle->scene = NULL;
    handle->special = NULL;
    handle->display_special = NULL;
    return 0;
}

void __cdecl InitSnakes(WORLDINFO *world) {
    memset(snakebodies, 0, sizeof(snakebodies));
    NuSpecialFind(world->current_gscn, &snake_hspecials[0], "Snake_bit_1", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[1], "Snake_bit_2", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[2], "Snake_bit_3", 1);
}
