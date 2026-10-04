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
