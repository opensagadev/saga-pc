#include "saga.h"
#include <stdio.h>
void __cdecl LoadPartFile(WORLDINFO *world) {
    char path[256];
    world->page_part = -1;
    edpartSetParticlePage(world->page_pp);
    if ((world->current_level->flags & 0xe0) == 0) {
        sprintf(path, "%s.par", world->config_file);
        if (NuFileExists(path) > 0)
            world->page_part = edpartLoadPage(path, 1, world->current_gscn);
        world->part_debris_sys = InitPartDebris(&world->giz_buffer, &world->buffer_end, 64, part_debris_count,
                                                PartDebrisNameListPc, world->page_part);
    }
}
