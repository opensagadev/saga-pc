#include "saga.h"
typedef struct carry_plug_pc_s {
    char name[16];
    NUVEC position;
    u16 rotation_x, rotation_y, rotation_z;
    u8 enabled, flags;
    u8 unknown_24[0x10];
} CARRY_PLUG_PC;
typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs;
    i32 count;
} CARRY_PLUG_SYS_PC;
STATIC_ASSERT(sizeof(CARRY_PLUG_PC) == 0x34);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, position) == 0x10);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_x) == 0x1c);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_y) == 0x1e);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_z) == 0x20);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, flags) == 0x23);
STATIC_ASSERT(sizeof(CARRY_PLUG_SYS_PC) == 8);
STATIC_ASSERT(offsetof(CARRY_PLUG_SYS_PC, count) == 4);

CARRY_PLUG_PC *__cdecl Plug_FindNearest(CARRY_PLUG_SYS_PC *system, NUVEC *position, f32 *distance_squared,
                                        i32 only_unplugged) {
    CARRY_PLUG_PC *nearest = NULL, *plug;
    f32 best = 1000000000.0f, distance;
    i32 i;
    if (system != NULL) {
        plug = system->plugs;
        for (i = 0; i < system->count; ++i, ++plug) {
            if (only_unplugged != 0 && (plug->flags & 4))
                continue;
            distance = NuVecDistSqr(position, &plug->position, NULL);
            if (distance < best) {
                best = distance;
                nearest = plug;
            }
        }
    }
    if (distance_squared != NULL)
        *distance_squared = best;
    return nearest;
}
void __cdecl Plug_MakeDrawMtx(CARRY_PLUG_PC *plug, NUMTX *matrix) {
    NuMtxSetRotationY(matrix, plug->rotation_y);
    NuMtxRotateZ(matrix, plug->rotation_z);
    NuMtxRotateX(matrix, plug->rotation_x);
    NuMtxTranslate(matrix, &plug->position);
}
