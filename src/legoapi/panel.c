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

extern i32 NuMainSkipCutBordersPc;
extern i32 AutoSaveIconPendingPc;

STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) + 6 * sizeof(void *) == 0x80);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) == 0xb8);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);

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

void Panel_Clear() {
    statstime = 0.0f;
    DrawMiniKitTime = 0.0f;
    MiniKitScale = 1.0f;
    DrawBuildUpTime = 0.0f;
    builduptime = 0.0f;
    BuildUpScale = 1.0f;
    DrawRedBrickTime = 0.0f;
    RedBrickScale = 1.0f;
    DrawCoinTotalTime = 0.0f;
    cointotaltime = 0.0f;
    CoinTotalScale = 1.0f;
    redbrickslidetime = 0.0f;
    goldbricktime = 0.0f;
    Arcade_ResetPanel();
}

void DrawBossHitPoints(GAMEOBJECT *object) {
    drawbosshitpoints = object;
}

i32 __cdecl CoinsGoToMainTotal(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || SuperStory != 0 ||
        (world->area != NULL && (world->area->flags & 0x100)) ||
        (world->current_level != NULL && (world->current_level->flags & 0x400)))
        return 1;
    return 0;
}

f32 __cdecl PowerUp_GetPanelY(i32 player_index) {
    f32 y = POWERUPYPOS1;
    if (player_index == 0 || player_index == 1) {
        i32 angle = (i32)((f64)PowerUp_PanelPosMul[player_index] * 32768.0 + 16384.0);
        f64 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
        y = (f32)(position * ((f64)POWERUPYPOS2 - (f64)POWERUPYPOS1) + (f64)POWERUPYPOS1);
    }
    return y;
}
