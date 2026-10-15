#include "saga.h"
#include <string.h>

void *__cdecl SetLevelHack(i32 size) {
    memset(OldLevelHackData, 0, 0x80);
    memset(LevelHackData, 0, 0x80);
    LevelHackSendTimer = 0.0f;
    LevelHackDataSize = size;
    return OldLevelHackData;
}

void __cdecl ResetLevel(WORLDINFO *world, char *cutscene, i32 unused) {
    CutDeadVehiclePlayer = NULL;
    if (world != NULL && NewCutScene(NULL, world->cutscene_sys, cutscene, 0) != NULL) {
        ResetBits |= 7;
        if (player != NULL && player->dead != 0)
            CutDeadVehiclePlayer = player;
        else if (player2 != NULL && player2->dead != 0)
            CutDeadVehiclePlayer = player2;
        reset_reimport = 1;
    } else {
        NewGameMode();
        ResetBits |= 0x17;
        reset_restart = 1;
        LevelCompletionFallbackFlagPC = 1;
        GizmoTypeStoreProgress(WORLD->gizmo_sys, WORLD, WORLD->current_level->area_level_index, gizmopickup_typeid,
                               NULL);
    }
}

void __cdecl ClearLevData(void) {
    LevFlag[0] = 0;
    LevFlag[1] = 0;
    LevFlag[2] = 0;
    LevFlag[3] = 0;
    LevSfxFlag[0] = 0;
    LevSfxFlag[1] = 0;
    LevSfxFlag[2] = 0;
    LevSfxFlag[3] = 0;
    memset(LevHSpecial, 0, sizeof(LevHSpecial));
    memset(dynamic_antinodes, 0, 0x1600);
    memset(&LevInstAnimResetPc, 0, sizeof(LevInstAnimResetPc));
    LevArea[0] = NULL;
    LevArea[1] = NULL;
    LevArea[2] = NULL;
    LevArea[3] = NULL;
    memset(LevPathNodes, 0, sizeof(LevPathNodes));
    memset(LevPathCnx, 0, sizeof(LevPathCnx));
    memset(LevGameObject, 0, sizeof(LevGameObject));
    memset(LevGamePart, 0, sizeof(LevGamePart));
    memset(LevAIMessage, 0, sizeof(LevAIMessage));
    LevelLocator = NULL;
    LevBlowUp[0] = NULL;
    LevBlowUp[1] = NULL;
    memset(LevGizObst, 0, sizeof(LevGizObst));
    memset(LevGizmo, 0, sizeof(LevGizmo));
    LevSfxId[0] = 0;
    LevSfxId[1] = 0;
    LevSfxId[2] = 0;
    LevSfxId[3] = 0;
    memset(LevelCodeSpline, 0, sizeof(LevelCodeSpline));
    LevPlatID[0] = -1;
    LevPlatID[1] = -1;
    LevPathCnxDir = 0;
    LevDeaths = 0;
    LevLock[3] = 0;
    LevLock[2] = 0;
    LevLock[1] = 0;
    LevLock[0] = 0;
    LevSafePlatID[1] = -1;
    LevSafePlatID[0] = -1;
}
