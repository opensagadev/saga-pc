#include "gameapi/edtools/edfile.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/config/cheats.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/trigger/gizspecial.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

i32 GetAnimDirection(NUINSTANIM *animation) {
    f32 tfactor;
    if (animation == NULL) {
        return -1;
    }
    tfactor = animation->tfactor;
    if (tfactor < 0.0f) {
        return 1;
    }
    if (tfactor > 0.0f) {
        return 0;
    }
    return -1;
}

extern "C" void DrawCross(NUVEC *position, f32 size, NUMTL *material, i32 colour) {
    if (material != NULL) {
        colour = ((u8)(i32)(material->diffuse_b * 255.0) << 8 | (u8)(i32)(material->diffuse_g * 255.0)) << 8 |
                 (u8)(i32)(material->diffuse_r * 255.0);
    }
    NuRndrLine3dDbg(position->x - size, position->y, position->z, position->x + size, position->y, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y - size, position->z, position->x, position->y + size, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y, position->z - size, position->x, position->y, position->z + size, colour);
}

extern "C" void edbobsDrawBox(NUVEC *minimum, NUVEC *maximum, i32 colour) {
    NuRndrLine3dDbg(minimum->x, minimum->y, minimum->z, maximum->x, minimum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, minimum->z, maximum->x, minimum->y, maximum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, maximum->z, minimum->x, minimum->y, maximum->z, colour);
    NuRndrLine3dDbg(minimum->x, minimum->y, maximum->z, minimum->x, minimum->y, minimum->z, colour);
    NuRndrLine3dDbg(minimum->x, maximum->y, minimum->z, maximum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, maximum->y, minimum->z, maximum->x, maximum->y, maximum->z, colour);
    NuRndrLine3dDbg(maximum->x, maximum->y, maximum->z, minimum->x, maximum->y, maximum->z, colour);
    NuRndrLine3dDbg(minimum->x, maximum->y, maximum->z, minimum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(minimum->x, minimum->y, minimum->z, minimum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, minimum->z, maximum->x, maximum->y, minimum->z, colour);
    NuRndrLine3dDbg(maximum->x, minimum->y, maximum->z, maximum->x, maximum->y, maximum->z, colour);
    NuRndrLine3dDbg(minimum->x, minimum->y, maximum->z, minimum->x, maximum->y, maximum->z, colour);
}

extern "C" void edbobs_DrawCoordinateInfo(NUVEC *position, i32 x, i32 y) {
    const i32 row = 1000 + y * 8;
    NuQFntPrintEx(system_qfont, (x + 10) * 16, row, 0x10, "%.3f", position->x);
    NuQFntPrintEx(system_qfont, (x + 80) * 16, row, 0x10, "%.3f", position->y);
    NuQFntPrintEx(system_qfont, (x + 150) * 16, row, 0x10, "%.3f", position->z);
}

i32 GizmoFileReadGameAnimSet(GAMEANIMSET *set, void *world_ptr, void (*read_data)(GAMEANIMOBJ *, u8), u8 version, char *prefix,
                             char *suffix) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 success = 1;
    i32 i;
    u8 file_version = EdFileReadChar();
    u8 count = EdFileReadChar();
    for (i = 0; i < count; ++i) {
        char name[64];
        i32 length = EdFileReadChar();
        f32 start_frame;
        f32 end_frame;
        u32 flags;
        GAMEANIMOBJ *object;
        GAMEANIMOBJ missing;
        if (length) {
            EdFileRead(name, length);
        }
        start_frame = EdFileReadFloat();
        end_frame = EdFileReadFloat();
        flags = file_version > 1 ? EdFileReadInt() : 0;
        if (!length) {
            continue;
        }
        if (!set->object_pool || !set->object_pool->free_objects) {
            success = 0;
        }
        object = GameAnimSet_AddObjectByName(set, world->current_gscn, name, start_frame, end_frame, 0, world->gizmo_sys, prefix, suffix);
        if (!object) {
            memset(&missing, 0, sizeof(missing));
            object = &missing;
        }
        object->serialized_flags = flags;
        if (read_data) {
            read_data(object, version);
        }
        if (file_version < 3) {
            object->serialized_flags &= ~2u;
        }
    }
    return success;
}
