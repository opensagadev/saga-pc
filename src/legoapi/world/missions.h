/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

struct mission_data_pc_s;

typedef struct MISSION_SAVE_READER_PC {
    f32 best_times[20];
    u8 completed[20];
} MISSION_SAVE_READER_PC;

STATIC_ASSERT(sizeof(MISSION_SAVE_READER_PC) == 0x64);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, best_times) == 0);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, completed) == 0x50);
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct mission_sys_pc_s {
        union {
            u8 unknown_00[4];
            struct mission_data_pc_s *missions;
        };

        struct mission_data_pc_s *active_mission;

        union {
            u8 unknown_08[0x15];

            struct {
                u8 unknown_count_prefix[0x14];
                u8 count;
                u8 state;
                u8 reserved_1e[2];
            };

            struct {
                TIMER timer;
                u8 reserved_18[8];
            };
        };
    } MISSIONSYS_PC;

    STATIC_ASSERT(offsetof(MISSIONSYS_PC, active_mission) == 4);
    STATIC_ASSERT(offsetof(MISSIONSYS_PC, state) == 0x1d);
#ifdef __cplusplus
}
#endif

struct MISSION_SAVE_READER_PC;
#ifdef __cplusplus
extern "C" {
#endif
    i32 Missions_PartyAvailable(MISSIONSYS_PC *);
    i32 Missions_NumCompleted(MISSIONSYS_PC *, struct MISSION_SAVE_READER_PC *, i32);
    extern f32 MissionIconGapPc;
    extern f32 MissionIconStepPc;
    extern i16 MissionMenuInstructionTextPc;
    extern f32 MissionIconBaseSizePc;
    extern f32 MissionIconSelectedSizePc;
    extern f32 MissionIconYPc;
    extern f32 MissionNameYPc;
    extern f32 MissionNameScalePc;
    extern f32 MissionBestTimeXPc;
    extern f32 MissionBestTimeYPc;
    extern f32 MissionBestTimeScalePc;
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(MISSIONSYS_PC) == 0x20);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, unknown_00) == 0);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, unknown_count_prefix) == 8);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, missions) == 0);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, timer) == 8);

STATIC_ASSERT(offsetof(MISSIONSYS_PC, active_mission) == 4);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, unknown_08) == 8);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, count) == 0x1c);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, state) == 0x1d);
