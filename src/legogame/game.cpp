#include "saga.h"
#include <string.h>

void __cdecl NewGame(void) {
    OPTIONSSAVE saved_options = Game.options_save;
    i32 i;
    u8 previous_group = 0;
    TempOptions = saved_options;
    memset(&Game, 0, sizeof(Game));
    Game.options_save = saved_options;
    adaptivedifficulty.level = 0;
    adaptivedifficulty.accumulator = 0.5f;
    adaptivedifficulty.time_elapsed = 0.0f;
    Game.save_version = 5;
    if (EDataList != NULL)
        Game.area_save[EDataList[0].area_ids[0]].complete = 1;
    else
        Game.area_save[0].complete = 1;
    if (SENATE_ADATA != NULL)
        Game.area_save[SENATE_ADATA->index].complete = 1;
    if (UTAPAU_ADATA != NULL)
        Game.area_save[UTAPAU_ADATA->index].complete = 1;
    if (HOTH_ADATA != NULL)
        Game.area_save[HOTH_ADATA->index].complete = 1;
    if (BONUSDAGOBAH_ADATA != NULL)
        Game.area_save[BONUSDAGOBAH_ADATA->index].complete = 1;
    if (BONUSKAMINO_ADATA != NULL)
        Game.area_save[BONUSKAMINO_ADATA->index].complete = 1;
    if (BONUSKASHYYYK_ADATA != NULL)
        Game.area_save[BONUSKASHYYYK_ADATA->index].complete = 1;
    Cheats_TurnOff(0);
    Game.purchased_extra_bits_high |= 0x1000;
    Cheat_SetOn(44, 1, 1);
    Hint_ClearHintsAndDoneFlags();
    LswForceUsedHintFlagsPc &= ~7u;
    GamePad_InitButtons();
    Tag_DoneFirst = 0;
    Tag_DoneAny = 0;
    for (i = 0; i < AREACOUNT; ++i)
        Game.area_save[i].challenge_trial_time = (f32)ADataList[i].challenge_trial_time;
    for (i = 0; i < 6; ++i) {
        Game.episode_start[i].superstory_time_limit = 3600.0f;
        Game.episode_start[i].superstory_score_target = 100000;
    }
    Customiser_CopyDefaultPiecesToSave(CharacterCustomiser, Game.customiser_start);
    if (TTab != NULL) {
        if (TTab[204] != NULL)
            NuStrCpy(Game.customiser_start[0].name, TTab[204]);
        if (TTab[205] != NULL)
            NuStrCpy(Game.customiser_start[1].name, TTab[205]);
        FinishWeirdoNames(-1);
    }
    if (id_DEFAULTCHARACTER[0] != -1)
        Game.character_save[id_DEFAULTCHARACTER[0]] |= 3;
    if (id_DEFAULTCHARACTER[1] != -1)
        Game.character_save[id_DEFAULTCHARACTER[1]] |= 3;
    if (PermanentWeirdo1IdPc != -1) {
        Game.character_save[PermanentWeirdo1IdPc] |= 3;
        if (CDataList != NULL)
            CDataList[PermanentWeirdo1IdPc].name_id = 204;
    }
    if (PermanentWeirdo2IdPc != -1) {
        Game.character_save[PermanentWeirdo2IdPc] |= 3;
        if (CDataList != NULL)
            CDataList[PermanentWeirdo2IdPc].name_id = 205;
    }
    Game.customiser_start[0].use_saved_name = 1;
    Game.customiser_start[1].use_saved_name = 1;
    for (i = 0; i < 10; ++i) {
        if (SuitRecordInitPc[i].group != previous_group)
            Game.initial_store_pack_flags |= 1u << i;
        previous_group = SuitRecordInitPc[i].group;
    }
}
