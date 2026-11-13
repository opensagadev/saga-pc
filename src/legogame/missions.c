#include "saga.h"

/* Retail unit 0x496e20-0x497c05: definitions in retail address order. */

typedef struct MISSION_END_DATA_VIEW_PC {
    i16 find_character;
} MISSION_END_DATA_VIEW_PC;
typedef struct MISSION_END_SYSTEM_VIEW_PC {
    u8 unknown_00[8];
    f32 elapsed_time;
} MISSION_END_SYSTEM_VIEW_PC;

void __cdecl MenuUpdateEndMission(MENU *menu) {
    if (menu->menu_time >= 5.0f) {
        CompleteLevel(WORLD);
    } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
        PlaySfx("Victory", NULL);
        BonusWinFlag = 1;
    }
}

void __cdecl MenuDrawEndMission(MENU *menu) {
    char text[32];
    i32 index;
    f32 remaining;
    (void)menu;
    if (MenuStopDraw != 0)
        return;
    if (MissionSys->state == 2) {
        if ((f64)NuFmod(GameTimer.time_elapsed, 0.3f) < (f64)0.2f) {
            index = (i8)MissionSys->active_mission->index;
            remaining = (f32)(i32)MissionSys->missions[index].time - BonusTimer.time_elapsed;
            if (remaining < 0.0f)
                remaining = 0.0f;
            Text_MakeTime(remaining, 0, 1, 1, text);
            Text3D(text, 0.0f, STATSPOSY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
        }
    } else {
        i32 colour_a = 255, colour_b = 63;
        if (menu_flash != 0) {
            colour_a = 191;
            colour_b = 31;
        }
        SmartTextPc(TTab[TextIdPc8806f4], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, colour_a, colour_b, 0, 1.7f, 1);
    }
}

void __cdecl InitMission(MISSIONSYS_PC *system, i32 index) {
    system->active_mission = &system->missions[index];
    system->state = 1;
    ResetTimer(&system->timer, 0.0f);
    Cheats_TurnOff(0);
}

void __cdecl EndMission(MISSIONSYS_PC *system, i32 state, i32 show_message) {
    if (netclient == 0 || show_message == 0) {
        system->state = (u8)state;
        SetBonusWinner(qrand() / 0x8000);
        if (Player[BonusWinner] == NULL || ((((GAMEOBJECT *)Player[BonusWinner])->flags_1fc & 0x80) == 0))
            BonusWinner = BonusWinner == 0;
        BonusWinFlag = 0;
        NewMenu(9, -1, -1);
        ResetGameMessages();
        if (state == 3)
            PlaySfx(ScorePanelName, NULL);
    }
}

void __cdecl CheckMissionEnd(MISSIONSYS_PC *system) {
    GAMEOBJECT *target, *candidate;
    u64 mask;
    i32 index;
    target = FindGameObject(((MISSION_END_DATA_VIEW_PC *)system->active_mission)->find_character, 0, 0, 1, 0);
    if (((MISSION_END_SYSTEM_VIEW_PC *)system)->elapsed_time < 3.0f)
        return;
    if (GetMenuID() == 9)
        return;
    if (target != NULL) {
        mask = U64_PAIR(target->collision_identity_mask);
    } else {
        mask = 0;
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            candidate = &Obj[index];
            if ((candidate->flags_1fc_word & 1) != 0 && (candidate->flags_1fc_word & 0x1000) != 0 &&
                candidate->dead == 0 && candidate->field_280 == 0xff) {
                mask |= U64_PAIR(candidate->collision_identity_mask);
            }
        }
    }
    for (index = 0; index < 8; ++index) {
        candidate = Player[index];
        if (candidate != NULL && (candidate->flags_1fc & 0x80) != 0 &&
            (U64_PAIR(candidate->collision_contact_mask) & mask) != 0) {
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
