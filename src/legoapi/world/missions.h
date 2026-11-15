#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/core/input/timer.h"

struct menu_s;
struct mission_data_s;

typedef struct MISSION_SAVE_READER { // size 0x64
    f32 best_times[20];              // 0x00
    u8 completed[20];                // 0x50
} MISSION_SAVE_READER;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct mission_sys_s {             // size 0x30
        struct mission_data_s *missions;       // 0x00
        struct mission_data_s *active_mission; // 0x04
        TIMER timer;                           // 0x08
        MISSION_SAVE_READER *save;             // 0x18
        u8 count;                              // 0x1c
        u8 state;                              // 0x1d

        union { // size 0x1
            u8 flags;

            struct {                  // size 0x1
                u8 pickups : 1;       // 0x1e:0
                u8 unknown_flags : 7; // 0x1e:1
            };
        }; // 0x1e

        u8 character_count;   // 0x1f
        i16 character_ids[8]; // 0x20
    } MISSIONSYS;

#ifdef __cplusplus
}
#endif

struct MISSION_SAVE_READER;
#ifdef __cplusplus
extern "C" {
#endif
    i32 Missions_PartyAvailable(MISSIONSYS *);
    i32 Missions_NumCompleted(MISSIONSYS *, struct MISSION_SAVE_READER *, i32);
    extern f32 MissionIconGap;
    extern f32 MissionIconStep;
    extern i16 MissionMenuInstructionText;
    extern f32 MissionIconBaseSize;
    extern f32 MissionIconSelectedSize;
    extern f32 MissionIconY;
    extern f32 MissionNameY;
    extern f32 MissionNameScale;
    extern f32 MissionBestTimeX;
    extern f32 MissionBestTimeY;
    extern f32 MissionBestTimeScale;
    void MenuDrawEndMission(struct menu_s *);
    void MenuUpdateEndMission(struct menu_s *);
#ifdef __cplusplus
}
#endif
