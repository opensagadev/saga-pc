#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/core/hint.h"

struct STATUSPACKET_s;
struct WORLDINFO_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 dat_007f195c;
    extern HINT Hints_LSW[65];
    void DrawHint_LSW(HINT *, i32);
    void initGameHintSys_LSW(void);
    extern const char PickupCoinSfx[11];
    extern const char CoinDropSilverSfx[10];
#ifdef __cplusplus
}
#endif

typedef struct REGISTER_STATUS_PACKET {                                         // size 0x18
    struct STATUSPACKET_LSW_s *lsw_packet;                                      // 0x00
    i32 (*init_callback)(struct WORLDINFO_s *, struct STATUSPACKET_s *);        // 0x04
    i32 (*finish_callback)(struct WORLDINFO_s *, struct STATUSPACKET_s *, i32); // 0x08
    void (*reset_callback)(struct STATUSPACKET_s *);                            // 0x0c
    void (*draw_background_callback)(struct STATUSPACKET_s *);                  // 0x10
    f32 stage_delay;                                                            // 0x14
} REGISTER_STATUS_PACKET;

#ifdef __cplusplus
extern "C" {
#endif
    f32 CurrentHintAlpha(void);
    void Alt_registerStatusScreenExtra(void);
#ifdef __cplusplus
}
#endif
