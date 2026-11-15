#include "saga.h"
#include <stdio.h>

void LoadPartFile(WORLDINFO *world) {
    char path[256];
    world->page_part = -1;
    edpartSetParticlePage(world->page_pp);
    if ((world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0) {
        sprintf(path, "%s.par", world->config_file);
        if (NuFileExists(path) > 0) {
            world->page_part = edpartLoadPage(path, 1, world->current_gscn);
        }
        world->part_debris_sys =
            InitPartDebris(&world->giz_buffer, &world->buffer_end, 64, part_debris_count, PDebNameList, world->page_part);
    }
}
