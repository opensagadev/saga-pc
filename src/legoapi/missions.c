#include "saga.h"

MISSIONDATA_PC *__cdecl Mission_Active(MISSIONSYS_PC *system) {
    if (system == NULL) {
        system = MissionSys;
        if (system == NULL)
            return NULL;
    }
    if (system->state == 0)
        return NULL;
    return system->active_mission;
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
