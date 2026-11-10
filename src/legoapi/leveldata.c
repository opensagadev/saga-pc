#include "saga.h"
#include <string.h>

/* PC registers no editor helpers. */
void __cdecl RegisterHelpers(void) {
}

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
