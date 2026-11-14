/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/gamemessages.h"
#include "nu2api/numath/nuvec.h"
#endif

struct WORLDINFO_s;
struct gameobject_s;

typedef struct add_game_message_s {  // size 0x50
    const char *text;                // 0x00
    NUVEC *position;                 // 0x04
    NUVEC *target_position;          // 0x08
    f32 scale;                       // 0x0c
    f32 target_scale;                // 0x10
    u8 red;                          // 0x14
    u8 green;                        // 0x15
    u8 blue;                         // 0x16
    u8 alpha;                        // 0x17
    u32 flags;                       // 0x18
    f32 duration;                    // 0x1c
    f32 delay;                       // 0x20
    u16 unknown_24;                  // 0x24
    i16 icon;                        // 0x26
    void *special;                   // 0x28
    u32 score;                       // 0x2c
    f32 unknown_30;                  // 0x30
    f32 unknown_34;                  // 0x34
    GAME_MESSAGE_CALLBACK delay_fn;  // 0x38
    GAME_MESSAGE_CALLBACK tick_fn;   // 0x3c
    GAME_MESSAGE_CALLBACK update_fn; // 0x40
    void *unknown_44;                // 0x44
    GAME_MESSAGE_CALLBACK end_fn;    // 0x48
    i8 player_index;                 // 0x4c
    u8 allow_temporary;              // 0x4d
    u8 target_is_coin_total;         // 0x4e
    u8 unknown_4f;                   // 0x4f
} ADD_GAME_MESSAGE_PC;

#ifdef __cplusplus
extern "C" {
#endif
    void Pup_CollectCoin(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_UpdateBlueCoin(struct WORLDINFO_s *, GIZMOPICKUP *);
    void Pup_UpdatePurpleCoin(struct WORLDINFO_s *, GIZMOPICKUP *);
    void Pup_CollectMinikit(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_CollectHeart(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_CollectRedBrick(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_CollectCharKit(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void Pup_UpdatePowerUp(struct WORLDINFO_s *, GIZMOPICKUP *);
    void Pup_CollectPowerUp(struct WORLDINFO_s *, GIZMOPICKUP *, i32, struct gameobject_s *, i32);
    void CollectRedBrick(NUVEC *);
    void SetCoinType(i32, GIZMOPICKUP *);
    extern char dat_00758c00[9];
    extern char dat_00758c0c[8];
    extern char dat_00758c14[10];
    extern char dat_00758c20[8];
    extern char dat_00758c28[10];
    extern char dat_00758c34[6];
    extern char dat_00758c3c[10];
    extern char dat_00758c48[8];
    extern char dat_00758c50[12];
    extern char dat_00758c5c[12];
    extern char dat_00758c68[10];
    extern char dat_00758c74[10];
    extern char dat_00758c80[12];
    extern GIZMO_PICKUP_TYPE GizmoPickupType[10];

    void AddPickups(i32 coins, i32 hearts, i32 torpedoes, i32 powerups, NUVEC *position, NUVEC *direction, f32 unused, i32 player_index,
                    f32 speed, f32 lifetime, struct gameobject_s *owner, i32 random_types, i32 panel);
    void AddCoinsAsParts(i32 coins, NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void AddHeartAsPart(struct gameobject_s *owner, NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void AddTorpedoAsPart(NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void PowerUp_AddPart(NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    extern f32 PANEL_COINSCALE_START;
    extern f32 PartExtraCoinRate;
    extern f32 PartVehicleHeartLifetime;
    extern f32 PartVehicleHeartPodraceHeight;
    extern f32 PartVehicleHeartHeight;
    extern f32 PartVehicleHeartPodraceScale;
    extern f32 PartVehicleHeartGunshipScale;
    extern f32 PartVehicleHeartScale;
    extern f32 coinimpactwait;
    extern const char PowerUpBounceSfx[21];
    extern const char CoinDropGoldSfx[10];
    void AddTorpedoAsPart(NUVEC *, NUVEC *, f32, f32);
    void AddHeartAsPart(struct gameobject_s *, NUVEC *, NUVEC *, f32, f32);
    void PowerUp_AddPart(NUVEC *, NUVEC *, f32, f32);
    void AddCoinsAsParts(i32, NUVEC *, NUVEC *, f32, f32);
#define PickupCoinName PickupCoinSfx // NOLINT(readability-identifier-naming)
#define PickupCoinBName dat_00758c5c // NOLINT(readability-identifier-naming)
    void AddCoinsToPanel(i32 coins, NUVEC *position, i32 player_index, f32 scale, struct gameobject_s *object, i32 mode);
    i32 LoseCoins(struct gameobject_s *object, i32 cause);
    extern char ScorePanelName[9];
    extern char ScoreCounterName[8];
    extern char PickupCoinsLandName[10];
    extern u8 CoinTab[4];
    extern f32 AddCoinDelay[2];
    extern const f64 PlayerSlideSpeed;
    extern const f64 PlayerTightRopePhase;
    void PowerUp_Update(struct gameobject_s *);
    extern PICKUP_TYPE_SYSTEM GizmoPickupSys_Game;
    extern char ScorePanelName[9];
    extern char dat_00758c14[10];
    void GizmoPickups_SetOnOff(void);
#ifdef __cplusplus
}
#endif
