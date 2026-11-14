/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_WORLD_MISSION_H_PART
#define SAGA_LEGOAPI_WORLD_MISSION_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_WORLD_MISSION_H_PART == 1 && !defined(SAGA_LEGOAPI_WORLD_MISSION_H_STOP)

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
        u8 index;             // 0x12
        u8 unknown_13[5];     // 0x13
    };
    typedef struct mission_data_s MISSIONDATA_PC;
#ifdef __cplusplus
}
#endif
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
    extern struct mission_sys_s *MissionSys;
    MISSIONDATA_PC *Mission_Active(struct mission_sys_s *);
    void CheckMissionEnd(struct mission_sys_s *system);
    struct mission_sys_s *Missions_Configure(char *, VARIPTR *, VARIPTR *, struct MISSION_SAVE_READER *);
    void EndMission(struct mission_sys_s *, i32, i32);
    i32 Mission_CurrentState(struct mission_sys_s *);
    i32 Missions_PickupsOff(struct mission_sys_s *);
#ifdef __cplusplus
}
#endif

struct MISSIONSYS_s {                         // size 0x30
    struct MISSIONDATA_s *missions;           // 0x00
    struct MISSIONDATA_s *mission;            // 0x04
    TIMER timer;                              // 0x08
    struct MISSION_SAVE_READER *mission_save; // 0x18
    u8 count;                                 // 0x1c
    u8 field8_0x1d;                           // 0x1d
    u8 flags;                                 // 0x1e
    u8 character_count;                       // 0x1f
    i16 character_ids[8];                     // 0x20
};

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
    struct gameobject_s *Mission_FindTarget(struct mission_sys_s *, u64 *);
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
