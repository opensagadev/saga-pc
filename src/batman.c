#include "saga.h"

int __cdecl NuMain(int argc, char **argv) {
    WORLDINFO *saved_world;

    NuCommandLine(&argc, &argv);
    WorldInfo_InitOnce();
    InitOnce(argc, argv);

    STUBBED_NAMED("NuMain: DrawSaveSlots callback at 0x00494250");
    MenuInitialise(GameMenuInfo, 0x17, LANGUAGECOUNT, NULL, 1, 0);
    MenuReset();
    edGraInitTerrainSwapProtection();

    NuDatHandles.previous = NuDatGet();
    NuDatHandles.episode[0] = NuDatOpen(EpisodeIDat, &permbuffer_base, NULL);
    NuDatHandles.episode[1] = NuDatOpen(EpisodeIIDat, &permbuffer_base, NULL);
    NuDatHandles.episode[2] = NuDatOpen(EpisodeIIIDat, &permbuffer_base, NULL);
    NuDatHandles.episode[3] = NuDatOpen(EpisodeIVDat, &permbuffer_base, NULL);
    NuDatHandles.episode[4] = NuDatOpen(EpisodeVDat, &permbuffer_base, NULL);
    NuDatHandles.episode[5] = NuDatOpen(EpisodeVIDat, &permbuffer_base, NULL);
    NuDatSet(NuDatHandles.previous);
    NuHGobjReversibleCharacters(1);
    NuHGobjForceShadowsOnCharacters(1);
    GameAISysInit();
    APITransparentInit();
    NuSound3SetLoopHoldTime(0.5f);
    InitShadowLights();
    GamePads_Init();
    bgProcInit();
    if (nu_input_state != NULL) {
        FinishLoop(2);
    } else {
        STUBBED_NAMED("NuMain: FinishLoop waits for PC input-state setup in InitOnce");
    }
    Text_InitLanguageList(Game_LanguageList);
    Text_SetLanguage(-1);
    // LoadPerm calls NuFrameBegin, which needs the still-unreconstructed input owner.
    if (nu_input_state == NULL) {
        STUBBED_NAMED("NuMain: PC input-state setup before LoadPerm");
        return 0;
    }
    StartPerm();
    LoadPerm();
    EndPerm();

    if (GAMEDEMO != 0) {
        saved_world = WORLD;
        WORLD = NULL;
        PLAYERCOUNT = 0;
        STUBBED_NAMED("NuMain: PC trailer Movie_Play and FinishLoop at 0x00492ce5");
        WORLD = saved_world;
    }
    if (openlevels != 0) {
        STUBBED_NAMED("NuMain: Areas_OpenAll at 0x00492d08");
    }
    if (Level == -1) {
        Level = 0;
    }

    // GAME.DAT contains the level and area lists, but their PC parser path is
    // still stubbed, so stop before indexing absent table data.
    if (LDataList == NULL || ADataList == NULL) {
        STUBBED_NAMED("NuMain: GAME.DAT level and area tables");
        return 0;
    }

    Area = LDataList[Level].area_index;
    last_area = -1;
    LastAData = NULL;
    if (Area >= 0 && (ADataList[Area].flags & 4) != 0) {
        NextArea_FreePlay = 1;
        FreePlay = 1;
    } else if (GAMEDEMO > 1 || (GAMEDEMO != 0 && FreePlay != 0)) {
        FreePlay = 1;
        GAMEDEMO = 2;
    }
    makefreeplaymodellist = 0;
    STUBBED_NAMED("NuMain: HUB area comparison and model list");
    STUBBED_NAMED("NuMain: NuFrameBegin/NuFrameEnd terrain-swap preframe");

    for (;;) {
        STUBBED_NAMED("NuMain: restart-level episode and gizmo state");
        GameAISysSetGame();
        STUBBED_NAMED("NuMain: level stream, area initialization and music");
        InitCables(WORLD);
        InitSnakes(WORLD);
        STUBBED_NAMED("NuMain: NuMusic::SetFader(1, 0) at 0x0049312c");
        NuSound3SetReverb(0);
        STUBBED_NAMED("NuMain: PC post-cable world and music setup");

        if (WORLD == NULL || WORLD->loaded == 0) {
            STUBBED_NAMED("NuMain: waiting for the PC world stream");
            break;
        }

        while (NuWindowsProcessMessages() != 0) {
            STUBBED_NAMED("NuMain: PC update helper at 0x00491d50");
            STUBBED_NAMED("NuMain: PC ESI-world render helper at 0x004927f0");
            STUBBED_NAMED("NuMain: frame timing and transition gate");
            break;
        }

        STUBBED_NAMED("NuMain: level transition, status and player-list cleanup");
        if (NewMode == 0) {
            break;
        }
        NewMode = 0;
    }

    STUBBED_NAMED("NuMain: Game_Exit and restart decision at 0x00493c12");
    return 0;
}
