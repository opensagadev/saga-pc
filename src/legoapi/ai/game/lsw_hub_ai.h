/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#endif

struct gizaimessage_s;

typedef struct HUB_AI_MUSIC_VIEW {                   // size 0x150
    struct AIAREA *areas[10];                        // 0x000
    struct AIPATH *paths[10];                        // 0x028
    AILOCATOR *area_locators[10][4];                 // 0x050
    i16 area;                                        // 0x0f0
    i16 door_state;                                  // 0x0f2
    i16 table_count;                                 // 0x0f4
    i16 counter_count;                               // 0x0f6
    u8 unknown_f8[4];                                // 0x0f8
    void *customer;                                  // 0x0fc
    void *serving_object;                            // 0x100
    struct gizaimessage_s *area_message;             // 0x104
    struct gizaimessage_s *serving_customer_message; // 0x108
    AILOCATOR *counter_a[4];                         // 0x10c
    AILOCATOR *counter_b[4];                         // 0x11c
    AILOCATOR *table_locators[8];                    // 0x12c
    AILOCATOR *serve_player;                         // 0x14c
} HUB_AI_MUSIC_VIEW;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 dat_007f1910;
    extern HUB_AI_MUSIC_VIEW HubAiMusicState;
    i32 Hub_Outside(void);
    i32 Hub_GetRandomCharType(void);
    void LSW_Hub_InitAI(WORLDINFO *);
    void LSW_Hub_ResetAI(WORLDINFO *);
    void LSW_Hub_UpdateAI(WORLDINFO *);
    void *Condition_InHubAreaInit(AISYS_PC *, char *, AISCRIPT *);
    f32 Condition_InHubArea(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
#ifdef __cplusplus
}
#endif
