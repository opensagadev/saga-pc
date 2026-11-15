#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/core/config/cheat.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 dat_007f17d4;
    i32 Shop_UpdateHint(HINT *);
    extern i32 shop_from_cutsceneplayer;
#ifdef __cplusplus
    extern CHEAT ExtrasCheats[46];
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern COLLECTION_BUILD ShopCollection;
    extern i32 POINTS_PER_HINT;
    extern i32 SHOPHINTCOUNT;
    extern i32 CutScenePlayCount;
    extern void (*drawptr)(void);
    extern i32 picked;

/* picked: the shop shelf being browsed, by the item list each value selects (shop.c). */
#define SHOP_SHELF_HINTS 0       /* HintItems */
#define SHOP_SHELF_CHARACTERS 1  /* CharItems */
#define SHOP_SHELF_EXTRAS 2      /* ExtraItems */
#define SHOP_SHELF_GOLD_BRICKS 4 /* BrickItems */
#define SHOP_SHELF_CUTSCENES 5   /* CutItems */
    extern CHEAT ExtrasCheats[46];
    extern struct shopitem_s *BrickItems;
    extern i32 BrickShelfIds[7];
    extern struct shopitem_s *CharItems;
    extern i32 CharShelfIds[7];
    extern shopitem_s CutItems[128];
    extern i32 CutShelfIds[7];
    void DrawTopShelf(i32 picked_item);
    void DrawShopPrompts(void);
    void DrawItemMenu2D(void);
    void DrawCodeMenu3D(void);
    void DrawSubItems(void);
    extern i32 ShopBrickRotationDegrees;
    extern f32 ShopCharacterPeriod;
    extern f32 ShopCodePulseScale;
    extern f32 ShopCodeArrowBing;
    extern f32 ShopCodeArrowAmplitude;
    extern i16 dat_0088070c;
    extern i16 dat_00880800;
    extern i16 dat_008808c4;
    extern i16 dat_00880904;
    extern i16 dat_00880908;
    extern struct shopitem_s *ExtraItems;
    extern i32 ExtraShelfIds[7];
    extern struct shopitem_s *HintItems;
    extern i32 HintShelfIds[7];
    extern i32 SHOPCHARCOUNT;
    i32 SelectShopItem(struct shopitem_s *items, i32 index);
    i32 BuyShopItem(struct shopitem_s *items, i32 index, i32 charge);
    i32 MoveSubItemsLeft(i32 *ids, NUVEC *positions, i32 count);
    i32 MoveSubItemsRight(i32 *ids, NUVEC *positions, i32 count);
    extern f32 ShopNameAlpha;
    extern NUVEC SubShelfPos[7];
    extern shopitem_s TopShelf[6];
    extern nuhspecial_s iconback;
    extern f32 TopShelfScale[6];
    extern f32 toppush[6];
    extern f32 topscale[6];
    extern i32 SHOPACTIVE;
    extern f32 ShopLockedScale;
    extern i16 dat_008808c0;
    void DrawShopPanel(void);
    extern void (*ShopDrawPanelFn)(void);
    void InitShop(WORLDINFO *);
    void DrawShop3D(WORLDINFO *);
#ifdef __cplusplus
}
#endif
