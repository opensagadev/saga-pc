#ifndef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOPICKUP_H
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOPICKUP_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/gizmos/fx/gizmopickups.h, legoapi/items/base/apiobject.h,
 * legoapi/legoapi_types.h, legoapi/menus/core/gamemessages.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/gamemessages.h"
#endif

typedef struct add_game_message_pc_s {
    const char *text;
    NUVEC *position;
    NUVEC *target_position;
    f32 scale;
    f32 target_scale;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
    u32 flags;
    f32 duration;
    f32 delay;
    u16 unknown_24;
    i16 icon;
    void *special;
    u32 score;
    f32 unknown_30;
    f32 unknown_34;
    GAME_MESSAGE_CALLBACK_PC delay_fn;
    GAME_MESSAGE_CALLBACK_PC tick_fn;
    GAME_MESSAGE_CALLBACK_PC update_fn;
    void *unknown_44;
    GAME_MESSAGE_CALLBACK_PC end_fn;
    i8 player_index;
    u8 allow_temporary;
    u8 target_is_coin_total;
    u8 unknown_4f;
} ADD_GAME_MESSAGE_PC;

STATIC_ASSERT(sizeof(ADD_GAME_MESSAGE_PC) == 0x50);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, text) == 0x0);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, position) == 0x4);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, target_position) == 0x8);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, scale) == 0xc);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, target_scale) == 0x10);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, red) == 0x14);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, green) == 0x15);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, blue) == 0x16);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, alpha) == 0x17);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, flags) == 0x18);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, duration) == 0x1c);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, delay) == 0x20);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, icon) == 0x26);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, special) == 0x28);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, score) == 0x2c);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, unknown_30) == 0x30);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, unknown_34) == 0x34);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, delay_fn) == 0x38);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, tick_fn) == 0x3c);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, update_fn) == 0x40);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, unknown_44) == 0x44);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, end_fn) == 0x48);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, player_index) == 0x4c);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, allow_temporary) == 0x4d);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, target_is_coin_total) == 0x4e);
STATIC_ASSERT(offsetof(ADD_GAME_MESSAGE_PC, unknown_4f) == 0x4f);
#ifdef __cplusplus
extern "C" {
#endif
    void Pup_CollectCoin(WORLDINFO *, GIZMOPICKUP_PC *, i32, GAMEOBJECT *, i32);
    void Pup_UpdateBlueCoin(WORLDINFO *, GIZMOPICKUP_PC *);
    void Pup_UpdatePurpleCoin(WORLDINFO *, GIZMOPICKUP_PC *);
    void Pup_CollectMinikit(WORLDINFO *, GIZMOPICKUP_PC *, i32, GAMEOBJECT *, i32);
    void Pup_CollectHeart(WORLDINFO *, GIZMOPICKUP_PC *, i32, GAMEOBJECT *, i32);
    void Pup_CollectRedBrick(WORLDINFO *, GIZMOPICKUP_PC *, i32, GAMEOBJECT *, i32);
    void Pup_CollectCharKit(WORLDINFO *, GIZMOPICKUP_PC *, i32, GAMEOBJECT *, i32);
    void Pup_UpdatePowerUp(WORLDINFO *, GIZMOPICKUP_PC *);
    void Pup_CollectPowerUp(WORLDINFO *, GIZMOPICKUP_PC *, i32, GAMEOBJECT *, i32);
    void CollectRedBrick(NUVEC *);
    void SetCoinType(i32, GIZMOPICKUP_PC *);
    extern char PickupTypeLiteral758c00[9];
    extern char PickupTypeLiteral758c0c[8];
    extern char PickupTypeLiteral758c14[10];
    extern char PickupTypeLiteral758c20[8];
    extern char PickupTypeLiteral758c28[10];
    extern char PickupTypeLiteral758c34[6];
    extern char PickupTypeLiteral758c3c[10];
    extern char PickupTypeLiteral758c48[8];
    extern char PickupTypeLiteral758c50[12];
    extern char PickupTypeLiteral758c5c[12];
    extern char PickupTypeLiteral758c68[10];
    extern char PickupTypeLiteral758c74[10];
    extern char PickupTypeLiteral758c80[12];
    extern GIZMO_PICKUP_TYPE_PC GizmoPickupType[10];

    void AddPickups(i32 coins, i32 hearts, i32 torpedoes, i32 powerups, NUVEC *position, NUVEC *direction, f32 unused,
                    i32 player_index, f32 speed, f32 lifetime, GAMEOBJECT *owner, i32 random_types, i32 panel);
    void AddCoinsAsParts(i32 coins, NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void AddHeartAsPart(GAMEOBJECT *owner, NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void AddTorpedoAsPart(NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    void PowerUp_AddPart(NUVEC *position, NUVEC *direction, f32 speed, f32 lifetime);
    extern f32 PANEL_COINSCALE_START;
    extern f32 PartExtraCoinRate_PC;
    extern f32 PartVehicleHeartLifetime_PC;
    extern f32 PartVehicleHeartPodraceHeight_PC;
    extern f32 PartVehicleHeartHeight_PC;
    extern f32 PartVehicleHeartPodraceScale_PC;
    extern f32 PartVehicleHeartGunshipScale_PC;
    extern f32 PartVehicleHeartScale_PC;
    extern f32 coinimpactwait;
    extern const char PowerUpBounceSfx_PC[21];
    extern const char CoinDropGoldSfx_PC[10];
    void AddTorpedoAsPart(NUVEC *, NUVEC *, f32, f32);
    void AddHeartAsPart(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32);
    void PowerUp_AddPart(NUVEC *, NUVEC *, f32, f32);
    void AddCoinsAsParts(i32, NUVEC *, NUVEC *, f32, f32);
    extern const char PowerUpLoopSfx_PC[18];
#define PickupCoinName PickupCoinSfx_PC         // NOLINT(readability-identifier-naming)
#define PickupCoinBName PickupTypeLiteral758c5c // NOLINT(readability-identifier-naming)
    void AddCoinsToPanel(i32 coins, NUVEC *position, i32 player_index, f32 scale, GAMEOBJECT *object, i32 mode);
    i32 LoseCoins(GAMEOBJECT *object, i32 cause);
    extern char ScorePanelName[9];
    extern char ScoreCounterName[8];
    extern char PickupCoinsLandName[10];
    extern u8 CoinTab[4];
    extern f32 AddCoinDelay[2];
    extern const f64 PlayerSlideSpeedPc;
    extern const f64 PlayerTightRopePhasePc;
    void PowerUp_Update(GAMEOBJECT *);
    extern PICKUP_TYPE_SYSTEM_PC GizmoPickupSys_Game;
    extern char ScorePanelName[9];
    extern char PickupTypeLiteral758c14[10];
    void GizmoPickups_SetOnOff(void);
#ifdef __cplusplus
}
#endif

#endif
