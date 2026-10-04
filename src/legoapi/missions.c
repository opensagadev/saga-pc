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

typedef struct MISSION_END_DATA_VIEW_PC {
    i16 find_character;
} MISSION_END_DATA_VIEW_PC;
typedef struct MISSION_END_SYSTEM_VIEW_PC {
    u8 unknown_00[8];
    f32 elapsed_time;
} MISSION_END_SYSTEM_VIEW_PC;
STATIC_ASSERT(offsetof(MISSION_END_DATA_VIEW_PC, find_character) == 0);
STATIC_ASSERT(offsetof(MISSION_END_SYSTEM_VIEW_PC, elapsed_time) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_identity_mask) == 0x1e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
void __cdecl CheckMissionEnd(MISSIONSYS_PC *system) {
    GAMEOBJECT *target, *candidate;
    u32 low, high;
    i32 index;
    target = FindGameObject(((MISSION_END_DATA_VIEW_PC *)system->active_mission)->find_character, 0, 0, 1, 0);
    if (((MISSION_END_SYSTEM_VIEW_PC *)system)->elapsed_time < 3.0f)
        return;
    if (GetMenuID() == 9)
        return;
    if (target != NULL) {
        low = target->collision_identity_mask[0];
        high = target->collision_identity_mask[1];
    } else {
        low = high = 0;
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            candidate = &Obj[index];
            if ((candidate->flags_1fc_word & 0x1001) == 0x1001 && candidate->dead == 0 &&
                candidate->field_280 == 0xff) {
                low |= candidate->collision_identity_mask[0];
                high |= candidate->collision_identity_mask[1];
            }
        }
    }
    for (index = 0; index < 8; ++index) {
        candidate = Player[index];
        if (candidate != NULL && (candidate->flags_1fc & 0x80) != 0 &&
            ((candidate->collision_contact_mask[0] & low) | (candidate->collision_contact_mask[1] & high)) != 0) {
            if (netclient != 0)
                return;
            system->state = 2;
            SetBonusWinner(qrand() / 0x8000);
            if (Player[BonusWinner] == NULL || ((((GAMEOBJECT *)Player[BonusWinner])->flags_1fc & 0x80) == 0))
                BonusWinner = BonusWinner == 0;
            BonusWinFlag = 0;
            NewMenu(9, -1, -1);
            ResetGameMessages();
            return;
        }
    }
}
