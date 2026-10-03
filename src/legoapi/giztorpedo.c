#include "saga.h"

i32 __cdecl PartDraw_Torp(PART_PC *part) {
    i32 draw = PartDraw_Flickerer(part);
    if (draw != 0 && WORLD->level_objects[0x7b].active != 0) {
        NUMTX matrix = part->transform;
        NuMtxPreScaleU(&matrix, (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0));
        NuSpecialDrawAt(&WORLD->level_objects[0x7b].special, &matrix);
    }
    return draw;
}
