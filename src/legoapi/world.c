#include "saga.h"
#include <string.h>

void WorldInfo_InitOnce(void) {
    memset(WorldInfo, 0, sizeof(WorldInfo));
}

WORLDINFO *WorldInfo_CurrentlyActive(void) {
    return WORLD;
}

WORLDINFO *WorldInfo_CurrentlyLoading(void) {
    return LWORLD;
}

i32 __cdecl WorldInfo_OtherLevel(WORLDINFO *world) {
    WORLDINFO *other = &WorldInfo[0];
    if (world == &WorldInfo[0]) {
        other = &WorldInfo[1];
    }
    if (other->loaded != 0) {
        return other->level_idx;
    }
    return -1;
}

void __cdecl WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param) {
    world->room_visibility_flag = 1;
    world->rooms_visible_ptr = world->rooms_visible;
    memset(world->rooms_visible, 0, 0x100);

    if (param == 0) {
        NUGSCN *scene = world->current_gscn;
        if (scene != NULL) {
            i32 num_rooms = scene->num_rooms;
            if (num_rooms > 0) {
                NUGSCNROOM *room = scene->rooms;
                u8 *visible = world->rooms_visible_ptr;
                u8 *room_visibility = &room->visibility_flags;
                i32 remaining = num_rooms;
                do {
                    *visible = (u8)((*room_visibility >> 2) & 1);
                    room_visibility += sizeof(*room);
                    ++visible;
                    --remaining;
                } while (remaining != 0);
            }
        }
    }
}
