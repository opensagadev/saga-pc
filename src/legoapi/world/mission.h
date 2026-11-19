#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/core/input/timer.h"

struct MISSION_SAVE_READER;
struct gameobject_s;
struct mission_sys_s;

#ifdef __cplusplus
extern "C" {
#endif
    void EndChallenge(i32, i32);

    struct mission_data_s {   // size 0x18
        i16 find_character;   // 0x00
        i16 level;            // 0x02
        i16 name_id;          // 0x04
        i16 text_id;          // 0x06
        i32 bounty;           // 0x08
        i32 alternate_bounty; // 0x0c
        u16 time;             // 0x10
        i8 index;             // 0x12
        u8 reserved_13[5];    // 0x13
    };
    typedef struct mission_data_s MISSIONDATA;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void InitChallenge(i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct mission_sys_s *MissionSys;
    MISSIONDATA *Mission_Active(struct mission_sys_s *);
    void CheckMissionEnd(struct mission_sys_s *system);
    struct mission_sys_s *Missions_Configure(char *, VARIPTR *, VARIPTR *, struct MISSION_SAVE_READER *);
    void EndMission(struct mission_sys_s *, i32, i32);
    i32 Mission_CurrentState(struct mission_sys_s *);
    i32 Missions_PickupsOff(struct mission_sys_s *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    struct gameobject_s *Mission_FindTarget(struct mission_sys_s *, u64 *);
#ifdef __cplusplus
}
#endif
