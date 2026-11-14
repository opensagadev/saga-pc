/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/lib/hub.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART == 1 && !defined(SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP)

struct REGISTER_STATUS_PACKET_PC;
struct STATUSPACKET_s;
struct STATUS_STAGE_s;
struct WORLDINFO_s;

typedef struct PAD_INPUT_PC {
    u8 unknown_00[0x92];
    u8 analog_left_x;
    u8 analog_left_y;
} PAD_INPUT_PC;

typedef struct RUMBLE_PACKET_RESOURCE_PC {
    union {
        u8 unknown_00[4];
        f32 intensity;
    };

    f32 amount;

    union {
        u8 unknown_08[4];
        f32 decay_duration;
    };

    f32 duration;
    u8 active;
} RUMBLE_PACKET_RESOURCE_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f17d0Pc;
/* Windows SDK commctrl.h aliases DrawStatusText to DrawStatusTextA. */
#ifdef DrawStatusText
#undef DrawStatusText
#endif
    void DrawStatusText(char *, u16, f32, f32, f32, u32, i32);
    extern const char RenderSplineBountyStartPc[13];
    extern f32 DrawStatusScaleXPc;
    extern f32 DrawStatusScaleYPc;

    /* Additive shared contracts only; existing RTL/flight/game/player types reused. */
    STATIC_ASSERT(offsetof(RUMBLE_PACKET_RESOURCE_PC, amount) == 4);
    STATIC_ASSERT(offsetof(RUMBLE_PACKET_RESOURCE_PC, duration) == 0xc);
    STATIC_ASSERT(offsetof(RUMBLE_PACKET_RESOURCE_PC, active) == 0x10);
    extern NUVEC PanelLightingDirectionsPc[3];
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(RUMBLE_PACKET_RESOURCE_PC, intensity) == 0);
STATIC_ASSERT(offsetof(RUMBLE_PACKET_RESOURCE_PC, decay_duration) == 8);
STATIC_ASSERT(sizeof(RUMBLE_PACKET_RESOURCE_PC) == 0x14);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 StatusNewMinikitCountPc;
    extern i32 currentminikit;
    extern i16 StatusNewCharacterIdsPc[8];
    extern u32 StatusCollectedSuitBitsPc;
    extern f32 StatusMiniKitDirectionOffsetPc;
    extern f32 slideseek;
    void AddStatusMiniKitParts(void);
    extern f32 jibberlen;
    extern f32 slidetime;
    extern f32 pop_timer;
    extern i32 PauseSaveAndExitPc;
    f32 StatusScoreRatioPc(f32, f32);
    extern i32 StatusDemoOptionPc;
    void StatusRumbleUpdatePc(void);
    void FinishStatusPacket(i32);
    extern i32 DrawGoldBrick_Stage;
    extern i32 DrawGoldBrick_Phase;
    f32 StatusIconsOnOff(f32);
    void NewStatusRumbleBuzz(i32, f32, f32, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART == 2 && !defined(SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP)

typedef struct minikit_piece_pc_s {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x18];
    u8 direction;
    u8 unknown_65_b[0x1];
    u8 enabled;
    u8 direction_index;
} MINIKIT_PIECE_PC;

STATIC_ASSERT(sizeof(MINIKIT_PIECE_PC) == 0x68);
STATIC_ASSERT(offsetof(MINIKIT_PIECE_PC, special) == 0);
STATIC_ASSERT(offsetof(MINIKIT_PIECE_PC, matrix) == 0xc);
STATIC_ASSERT(offsetof(MINIKIT_PIECE_PC, direction) == 0x64);
STATIC_ASSERT(offsetof(MINIKIT_PIECE_PC, direction_index) == 0x67);

typedef struct status_minikit_draw_cache_pc_s {
    NUMTX matrix;
    MINIKIT_PIECE_PC *piece;
    i32 active;
} STATUS_MINIKIT_DRAW_CACHE_PC;

STATIC_ASSERT(sizeof(STATUS_MINIKIT_DRAW_CACHE_PC) == 0x48);
STATIC_ASSERT(offsetof(STATUS_MINIKIT_DRAW_CACHE_PC, matrix) == 0);
STATIC_ASSERT(offsetof(STATUS_MINIKIT_DRAW_CACHE_PC, piece) == 0x40);
STATIC_ASSERT(offsetof(STATUS_MINIKIT_DRAW_CACHE_PC, active) == 0x44);
#ifdef __cplusplus
extern "C" {
#endif
    extern COLLECTION_BUILD_PC MasterCollectionPc;
    extern STATUS_MINIKIT_DRAW_CACHE_PC StatusMiniKitDrawCachePc[10];
    extern PART_LIGHTING_DATA_PC StatusMiniKitPartLightingPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART == 3 && !defined(SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern AREA_DATA_PC *GOTHAMSTREETS_ADATA;
    void StatusStage_Reset(struct STATUS_STAGE_s *stage);
    void NextStatusStage(struct STATUSPACKET_s *);
    void RegisterStatusScreen(struct STATUS_STAGE_s *, i32 *, const struct REGISTER_STATUS_PACKET_PC *);
    void DrawStatusMiniKit(f32, f32, f32, f32, f32, i32, struct STATUSPACKET_s *, f32);
    i32 UpdateNewMiniKits(struct STATUSPACKET_s *, struct STATUS_STAGE_s *);
    void StatusPacketReset(struct STATUSPACKET_s *);
    i32 newCharactersCollected(struct STATUSPACKET_s *);
    void InitStatusScreen(struct WORLDINFO_s *);
    extern AREA_DATA_PC *NEGOTIATIONS_ADATA;
    void SetDrawGoldBrick(struct STATUSPACKET_s *, i32);
    void DrawBonusTime(struct STATUSPACKET_s *, f32, i32);
    extern struct STATUS_STAGE_s *StatusStages;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_MISC_LEGOAPI_STATUS_H_STOP
