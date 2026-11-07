#include "saga.h"

void __cdecl SetBonusWinner(i32 player_index) {
    BonusWinner = player_index;
    GameCam_Blend(GameCam, 1.5f, 0.0f, 1);
    LookAtBoth = 1;
}

void __cdecl EndChallenge(i32 state, i32 show_message) {
    if (netclient == 0 || show_message == 0) {
        ChallengeMode = state;
        SetBonusWinner(qrand() / 0x8000);
        if (Player[BonusWinner] == NULL || !(((GAMEOBJECT *)Player[BonusWinner])->flags_1fc & 0x80))
            BonusWinner = BonusWinner == 0;
        BonusWinFlag = 0;
        NewMenu(10, -1, -1);
        ResetGameMessages();
        if (state == 3)
            PlaySfx(ScorePanelName, NULL);
    }
}
