#include "saga.h"

void __cdecl InitPanel(i32 mode) {
    f32 width;
    f32 height;

    if (mode != 0) {
        width = PanelWideWidth;
        height = PanelWideHeight;
    } else {
        width = 640.0f;
        height = 448.0f;
    }
    PANEL3DMULX = 640.0 / (PanelProjectionX * (640.0 / width));
    PANEL3DMULY = 224.0 / (PanelProjectionY * (224.0 / height));
}

void __cdecl RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z) {
    switch (order) {
        case 0:
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 1:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 2:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            break;
        case 3:
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            break;
    }
}

i32 __cdecl InDoubleScoreZone(GAMEOBJECT *object) {
    i32 i;
    if ((WORLD->current_level->flags & 0x800) != 0)
        return 1;
    for (i = 19; i <= 23; ++i) {
        if (WORLD->camera_splines[i] != NULL &&
            OutSideSplineArea(&object->collision_position, WORLD->camera_splines[i], NULL, NULL, 0) == 0)
            return 1;
    }
    return 0;
}

i32 __cdecl CoinsGoToMainTotal(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || SuperStory != 0 ||
        (world->area != NULL && (world->area->flags & 0x100)) ||
        (world->current_level != NULL && (world->current_level->flags & 0x400)))
        return 1;
    return 0;
}
