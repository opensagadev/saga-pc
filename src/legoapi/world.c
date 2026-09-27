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

void __cdecl edbitsRegisterBaseScene(NUGSCN *scene) {
    edbits_base_scene = scene;
}

void __cdecl edanimRegisterBaseScene(NUGSCN *scene) {
    (void)scene;
}

void __cdecl edbitsRegisterBaseTerrain(void *terrain) {
    edbits_base_terrain = terrain;
}

void __cdecl WorldInfo_Register(WORLDINFO *world) {
    edbitsRegisterBaseScene(world->current_gscn);
    edanimRegisterBaseScene(world->current_gscn);
    edbitsRegisterBaseTerrain(world->terrain);
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

void __cdecl WorldInfo_ReArrangeBuffers(i32 area1, i32 area2) {
    VARIPTR *buffer_end = &WorldInfo[1].buffer_end;
    AREA_DATA_PC *areas;
    u8 single_buffer = 8;
    u32 end;
    u32 old_buffer_end;

    if (area1 != -1) {
        areas = ADataList;
        if (area1 != area2 && (areas[area1].flags & single_buffer) != 0) {
            LWORLD = &WorldInfo[0];
            WORLD = &WorldInfo[0];
            end = WorldInfo[0].buffer_end.addr;
            old_buffer_end = buffer_end->addr;
            if (end >= old_buffer_end) {
                return;
            }
            buffer_end->addr = end;
            WorldInfo[0].buffer_end.addr = old_buffer_end - EDITBUFFERENDSIZE;
            return;
        } else if ((areas[area1].flags & single_buffer) != 0) {
            return;
        }
    }

    end = WorldInfo[0].buffer_end.addr;
    old_buffer_end = buffer_end->addr;
    if (end <= old_buffer_end) {
        return;
    }
    end += EDITBUFFERENDSIZE;
    buffer_end->addr = end;
    WorldInfo[0].buffer_end.addr = old_buffer_end;
}
