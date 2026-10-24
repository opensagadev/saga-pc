#include "saga.h"

extern i32 DEBPAGE_CHARACTER, DEBPAGE_GENERAL, DEBPAGE_AREA;
void __cdecl DebFreeAllCreatedEffects(void);
void __cdecl Particles_Stop(WORLDINFO *world) {
    if (world->page_anim != -1)
        edanimStopPage(world->page_anim);
    if (world->page_pp != -1)
        edppStopPage((i8)(u8)world->page_pp);
    if (DEBPAGE_CHARACTER != -1)
        edppStopPage((i8)DEBPAGE_CHARACTER);
    if (DEBPAGE_GENERAL != -1)
        edppStopPage((i8)DEBPAGE_GENERAL);
    if (DEBPAGE_AREA != -1)
        edppStopPage((i8)DEBPAGE_AREA);
    DebFreeAllCreatedEffects();
}

void __cdecl edppStartPage(i32 page);
void __cdecl Particles_Start(WORLDINFO *world) {
    if (DEBPAGE_AREA != -1)
        edppStartPage(DEBPAGE_AREA);
    if (world->page_pp != -1)
        edppStartPage((u8)world->page_pp);
    DebrisSetRenderGroup(1);
    if (world->page_anim >= 0)
        edanimStartPage(world->page_anim);
}

#include <stdio.h>
APIDEBRISSYS_PC *__cdecl InitGameDebris(VARIPTR *, VARIPTR, i32, i32, char **, i8);
void __cdecl Particles_Load(WORLDINFO *world, char **names, i32 count, i32 named_count) {
    char path[256];
    VARIPTR end;
    world->page_pp = -1;
    sprintf(path, "%s.ptl", world->config_file);
    if (NuFileExists(path))
        world->page_pp = edppLoadPage(path, 1, world->current_gscn);
    end.void_ptr = &world->buffer_end;
    world->game_debris_sys = InitGameDebris(&world->giz_buffer, end, count, named_count, names, (i8)world->page_pp);
}

void Particles_LoadAreaPage(char *path) {
    DEBPAGE_AREA = -1;
    if (NuFileExists(path) != 0) {
        DEBPAGE_AREA = edppLoadPage(path, 1, 0);
    }
}

void Particles_DumpAreaPage() {
    if (DEBPAGE_AREA != -1) {
        edppStopPage((i8)(DEBPAGE_AREA));
        edppClearPage((i8)(DEBPAGE_AREA));
        DEBPAGE_AREA = -1;
    }
}
