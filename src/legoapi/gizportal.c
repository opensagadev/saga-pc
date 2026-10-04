#include "saga.h"
#include <string.h>
typedef struct PORTAL_DOOR_PC {
    NUSPECIALHANDLE special;
    u16 flags;
    u8 portal_id;
    u8 unknown_0f;
} PORTAL_DOOR_PC;
typedef struct PORTAL_DOOR_WORLD_PC {
    u8 unknown_0000[0x5048];
    PORTAL_DOOR_PC *portal_doors;
    i32 portal_door_count;
} PORTAL_DOOR_WORLD_PC;
STATIC_ASSERT(sizeof(PORTAL_DOOR_PC) == 0x10);
STATIC_ASSERT(offsetof(PORTAL_DOOR_PC, flags) == 0xc);
STATIC_ASSERT(offsetof(PORTAL_DOOR_PC, portal_id) == 0xe);
STATIC_ASSERT(offsetof(PORTAL_DOOR_WORLD_PC, portal_doors) == 0x5048);
STATIC_ASSERT(offsetof(PORTAL_DOOR_WORLD_PC, portal_door_count) == 0x504c);
void __cdecl PortalDoors_Configure(WORLDINFO *world, char *config) {
    PORTAL_DOOR_WORLD_PC *view = (PORTAL_DOOR_WORLD_PC *)world;
    PORTAL_DOOR_PC *cursor;
    NUFPAR_PC *parser;
    view->portal_doors = NULL;
    view->portal_door_count = 0;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("portaldoors", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    cursor = (PORTAL_DOOR_PC *)world->giz_buffer.addr;
    view->portal_doors = cursor;
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, "portaldoor") != 0)
            continue;
        memset(cursor, 0, sizeof(*cursor));
        if (NuFParGetWord(parser) == 0 ||
            NuSpecialFind(world->current_gscn, &cursor->special, parser->word_buffer_ptr, 1) == 0)
            continue;
        cursor->portal_id = (u8)NuFParGetInt(parser);
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "trigger_at_end") == 0)
                cursor->flags |= 1;
        }
        ++view->portal_door_count;
        ++cursor;
    }
    NuFParDestroy(parser);
    if (view->portal_door_count > 0)
        world->giz_buffer.addr = ((u32)cursor + 15U) & ~15U;
    else
        view->portal_doors = NULL;
}
