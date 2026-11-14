/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/core/hint.h"
#endif

struct STATUSPACKET_s;
struct WORLDINFO_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f195cPc;
    extern HINT_PC Hints_LSW[65];
    void DrawHint_LSW(HINT_PC *, i32);
    void initGameHintSys_LSW(void);
    extern const char PickupCoinSfx_PC[11];
    extern const char CoinDropSilverSfx_PC[10];
#ifdef __cplusplus
}
#endif

typedef struct REGISTER_STATUS_PACKET_PC {
    struct STATUSPACKET_LSW_s *lsw_packet;
    i32 (*init_callback)(struct WORLDINFO_s *, struct STATUSPACKET_s *);
    i32 (*finish_callback)(struct WORLDINFO_s *, struct STATUSPACKET_s *, i32);
    void (*reset_callback)(struct STATUSPACKET_s *);
    void (*draw_background_callback)(struct STATUSPACKET_s *);
    f32 stage_delay;
} REGISTER_STATUS_PACKET_PC;

STATIC_ASSERT(sizeof(REGISTER_STATUS_PACKET_PC) == 0x18);
STATIC_ASSERT(offsetof(REGISTER_STATUS_PACKET_PC, lsw_packet) == 0);
STATIC_ASSERT(offsetof(REGISTER_STATUS_PACKET_PC, init_callback) == 4);
STATIC_ASSERT(offsetof(REGISTER_STATUS_PACKET_PC, finish_callback) == 8);
STATIC_ASSERT(offsetof(REGISTER_STATUS_PACKET_PC, reset_callback) == 0xc);
STATIC_ASSERT(offsetof(REGISTER_STATUS_PACKET_PC, draw_background_callback) == 0x10);
STATIC_ASSERT(offsetof(REGISTER_STATUS_PACKET_PC, stage_delay) == 0x14);
#ifdef __cplusplus
extern "C" {
#endif
    extern const char PCContextName_Jump[5];
    f32 CurrentHintAlpha(void);
#ifdef __cplusplus
}
#endif
