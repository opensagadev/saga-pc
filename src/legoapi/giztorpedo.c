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

typedef struct torpedo_machine_query_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    f32 activation_time;
    u8 unknown_20[0xb];
    u8 flags;
} TORPEDO_MACHINE_QUERY_PC;
typedef struct torpedo_machine_query_sys_pc_s {
    TORPEDO_MACHINE_QUERY_PC *machines;
    i32 count;
    f32 scale;
} TORPEDO_MACHINE_QUERY_SYS_PC;
STATIC_ASSERT(sizeof(TORPEDO_MACHINE_QUERY_PC) == 0x2c);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_PC, position) == 0x10);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_PC, flags) == 0x2b);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_SYS_PC, machines) == 0);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_SYS_PC, count) == 4);
STATIC_ASSERT(sizeof(TORPEDO_MACHINE_QUERY_SYS_PC) == 0xc);
void *__cdecl GizTorpMachine_FindNearest(WORLDINFO *world, NUVEC *position, f32 *distance_squared) {
    TORPEDO_MACHINE_QUERY_PC *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    i32 index;
    if (world == NULL || world->torpedo_machine_sys == NULL)
        return NULL;
    for (index = 0; index < ((TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys)->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine =
            &((TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys)->machines[index];
        f32 distance;
        if ((machine->flags & 3) != 3)
            continue;
        distance = NuVecDistSqr(position, &machine->position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = machine;
        }
    }
    if (distance_squared != NULL)
        *distance_squared = nearest_distance;
    return nearest;
}
