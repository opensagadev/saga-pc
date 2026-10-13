#include "saga.h"

extern "C" void __cdecl DumpAreaData(i32 mode, i32 last_area);
extern "C" void __cdecl BackDrop_Dump(void);

i32 __cdecl Game_Exit(i32 last_area) {
    if (PcWindowQuit != 0) {
        if (WorldInfo[0].loaded != 0)
            WorldInfo_Dump(&WorldInfo[0]);
        if (WorldInfo[1].loaded != 0)
            WorldInfo_Dump(&WorldInfo[1]);
        DumpAreaData(1, last_area);
        if (save_icon_scene != NULL)
            NuGScnRemove(save_icon_scene);
        if (big_icon_scene != NULL)
            NuGScnRemove(big_icon_scene);
        if (area_scene != NULL)
            NuGScnRemove(area_scene);
        if (things_scene != NULL)
            NuGScnRemove(things_scene);
        if (vehicle_scene != NULL)
            NuGScnRemove(vehicle_scene);
        BackDrop_Dump();
        return last_area;
    }
    return 0;
}

void ClearPause(void) {
    Paused = 0;
    NetPaused = 0;
}

extern "C" void NewGameMode() {
    NewMode = 1;
    reset_load = 1;
}

extern "C" void __cdecl NuGCutSceneUpdateMtx(NUMTX *matrix) {
    STUBBED_NAMED("NuGCutSceneUpdateMtx: matrix-dependent recalculation"
                  " (retail passes the operand in EDI)");
}

extern "C" i32 __cdecl InStory(void) {
    if (FreePlay != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL || Arcade != 0) {
        return 0;
    }
    return 1;
}

extern "C" {
    extern CUSTOMISER_COMPLETION_READER_PC *CustomiserCompletionSavePc;
    i32 __cdecl Game_100PercentComplete(void) {
        if (CustomiserCompletionSavePc == NULL)
            return 0;
        return (CustomiserCompletionSavePc->flags & 1) != 0;
    }
}

extern "C" void AddToGoldBricks() {
    STATUSCOLLECT_s *save = reinterpret_cast<STATUSCOLLECT_s *>(CustomiserCompletionSavePc);
    const i32 points = GOLDBRICKPOINTS;
    if (save != NULL && save->gold_bricks < points) {
        ++save->gold_bricks;
        if (save->gold_bricks == points && (save->flags & SAVE_REWARD_ALL_GOLD_BRICKS) == 0) {
            if (Game_AllGoldBricksFn != NULL)
                Game_AllGoldBricksFn();
            reinterpret_cast<STATUSCOLLECT_s *>(CustomiserCompletionSavePc)->flags |= SAVE_REWARD_ALL_GOLD_BRICKS;
        }
    }
}
