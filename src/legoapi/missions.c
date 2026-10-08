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

struct mission_data_pc_s {
    i16 find_character;
    i16 level;
    i16 name_id;
    i16 text_id;
    i32 bounty;
    i32 alternate_bounty;
    u16 time;
    u8 index;
    u8 unknown_13[5];
};
typedef struct MISSION_SAVE_READER_PC {
    f32 best_times[20];
    u8 completed[20];
} MISSION_SAVE_READER_PC;
typedef struct MISSION_SYSTEM_READER_PC {
    MISSIONDATA_PC *missions;
    MISSIONDATA_PC *active_mission;
    TIMER timer;
    MISSION_SAVE_READER_PC *save;
    u8 count;
    u8 state;
    u8 flags;
    u8 character_count;
    i16 character_ids[8];
} MISSION_SYSTEM_READER_PC;
STATIC_ASSERT(sizeof(MISSIONDATA_PC) == 0x18);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, find_character) == 0);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, level) == 2);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, name_id) == 4);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, text_id) == 6);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, bounty) == 8);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, alternate_bounty) == 0xc);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, time) == 0x10);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, index) == 0x12);
STATIC_ASSERT(sizeof(MISSION_SAVE_READER_PC) == 0x64);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, best_times) == 0);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, completed) == 0x50);
STATIC_ASSERT(sizeof(MISSION_SYSTEM_READER_PC) == 0x30);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, missions) == 0);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, active_mission) == 4);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, timer) == 8);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, save) == 0x18);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, count) == 0x1c);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, state) == 0x1d);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, flags) == 0x1e);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, character_count) == 0x1f);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, character_ids) == 0x20);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);

i32 __cdecl Mission_CurrentState(MISSIONSYS_PC *system) {
    MISSION_SYSTEM_READER_PC *reader;
    if (system == NULL)
        system = MissionSys;
    if (system == NULL)
        return 0;
    reader = (MISSION_SYSTEM_READER_PC *)system;
    if (reader->state != 0 && reader->active_mission != NULL)
        return reader->state;
    return 0;
}

i32 __cdecl Missions_PartyAvailable(MISSIONSYS_PC *system) {
    MISSION_SYSTEM_READER_PC *reader;
    i32 index;
    if (system == NULL)
        system = MissionSys;
    if (system == NULL)
        return 1;
    reader = (MISSION_SYSTEM_READER_PC *)system;
    for (index = 0; index < reader->character_count; ++index) {
        if (Collection_Got(reader->character_ids[index]) == 0)
            return 0;
    }
    return 1;
}

i32 __cdecl Missions_NumCompleted(MISSIONSYS_PC *system, MISSION_SAVE_READER_PC *save, i32 complete) {
    MISSION_SYSTEM_READER_PC *reader;
    i32 index, total = 0;
    if (system == NULL)
        system = MissionSys;
    if (system == NULL)
        return 0;
    reader = (MISSION_SYSTEM_READER_PC *)system;
    for (index = 0; index < reader->count; ++index) {
        if (complete != 0) {
            save->completed[index] = 1;
            if (save->best_times[index] == 0.0f)
                save->best_times[index] = (f32)reader->missions[index].time - 1.0f;
        }
        if (save->completed[index] != 0)
            ++total;
    }
    return total;
}

GAMEOBJECT *__cdecl Mission_FindTarget(MISSIONSYS_PC *system, u64 *identity) {
    MISSION_SYSTEM_READER_PC *reader;
    GAMEOBJECT *target;
    if (system == NULL)
        system = MissionSys;
    if (system == NULL) {
        if (identity != NULL)
            *identity = 0;
        return NULL;
    }
    reader = (MISSION_SYSTEM_READER_PC *)system;
    target = FindGameObject(reader->active_mission->find_character, 0, 0, 1, 0);
    if (identity != NULL) {
        *identity =
            target == NULL ? 0 : ((u64)target->collision_identity_mask[1] << 32) | target->collision_identity_mask[0];
    }
    return target;
}

i32 __cdecl Missions_PickupsOff(MISSIONSYS_PC *system) {
    MISSION_SYSTEM_READER_PC *reader;
    if (system == NULL)
        system = MissionSys;
    if (system == NULL)
        return 0;
    reader = (MISSION_SYSTEM_READER_PC *)system;
    if (reader->state == 0 || reader->active_mission == NULL)
        return 0;
    return (reader->flags ^ 1) & 1;
}
