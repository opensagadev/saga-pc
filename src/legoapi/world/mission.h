#ifndef SAGA_LEGOAPI_WORLD_MISSION_H_PART
#define SAGA_LEGOAPI_WORLD_MISSION_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_WORLD_MISSION_H_PART == 1 && !defined(SAGA_LEGOAPI_WORLD_MISSION_H_STOP)

struct MISSION_SAVE_READER_PC;
struct gameobject_s;
struct mission_sys_pc_s;

#ifdef __cplusplus
extern "C" {
#endif
    void EndChallenge(i32, i32);

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
    typedef struct mission_data_pc_s MISSIONDATA_PC;
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(MISSIONDATA_PC) == 0x18);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, time) == 0x10);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, level) == 2);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, index) == 0x12);
typedef struct MISSIONSYS_s MISSIONSYS_s;
#ifdef __cplusplus
extern "C" {
#endif
    void InitChallenge(i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_MISSION_H_PART
#define SAGA_LEGOAPI_WORLD_MISSION_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_MISSION_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_MISSION_H_PART == 2 && !defined(SAGA_LEGOAPI_WORLD_MISSION_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct mission_sys_pc_s *MissionSys;
    MISSIONDATA_PC *Mission_Active(struct mission_sys_pc_s *);
    void CheckMissionEnd(struct mission_sys_pc_s *system);
    struct mission_sys_pc_s *Missions_Configure(char *, VARIPTR *, VARIPTR *, struct MISSION_SAVE_READER_PC *);
    void EndMission(struct mission_sys_pc_s *, i32, i32);
    i32 Mission_CurrentState(struct mission_sys_pc_s *);
    i32 Missions_PickupsOff(struct mission_sys_pc_s *);
#ifdef __cplusplus
}
#endif

struct MISSIONSYS_s {
    struct MISSIONDATA_s *missions;
    struct MISSIONDATA_s *mission;
    TIMER timer;
    struct MISSION_SAVE_READER_PC *mission_save;
    u8 count;
    u8 field8_0x1d;
    u8 flags;
    u8 character_count;
    i16 character_ids[8];
};

STATIC_ASSERT(sizeof(struct MISSIONSYS_s) == 48);
#undef SAGA_LEGOAPI_WORLD_MISSION_H_PART
#define SAGA_LEGOAPI_WORLD_MISSION_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_MISSION_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_MISSION_H_PART == 3 && !defined(SAGA_LEGOAPI_WORLD_MISSION_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    struct gameobject_s *Mission_FindTarget(struct mission_sys_pc_s *, u64 *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_MISSION_H_PART
#define SAGA_LEGOAPI_WORLD_MISSION_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_MISSION_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_WORLD_MISSION_H_STOP
