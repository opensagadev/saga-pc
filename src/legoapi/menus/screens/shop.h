#ifndef SAGA_LEGOAPI_MENUS_SCREENS_SHOP_H
#define SAGA_LEGOAPI_MENUS_SCREENS_SHOP_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/config/cheat.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f17d4Pc;
    i32 Shop_UpdateHint(HINT_PC *);
    extern i32 shop_from_cutsceneplayer;
#ifdef __cplusplus
    extern CHEAT_PC ExtrasCheatsPc[46];
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern COLLECTION_BUILD_PC ShopCollectionPc;
    extern i32 POINTS_PER_HINT;
    extern i32 SHOPHINTCOUNT;
    extern i32 CutScenePlayCountPc;
    extern void (*drawptr)(void);
    extern i32 picked;
    extern CHEAT_PC ExtrasCheatsPc[46];
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
    extern i32 ShopBrickRotationDegreesPc;
    extern f32 ShopCharacterPeriodPc;
    extern f32 ShopCodePulseScalePc;
    extern f32 ShopCodeArrowBingPc;
    extern f32 ShopCodeArrowAmplitudePc;
    extern i16 TextIdPc88070c;
    extern i16 TextIdPc880800;
    extern i16 TextIdPc8808c4;
    extern i16 TextIdPc880904;
    extern i16 TextIdPc880908;
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
    extern const char GameAudioName0075c348Pc[10];
    extern const char GameAudioName0075c354Pc[12];
    extern const char GameAudioName0075c360Pc[9];
    extern const char GameAudioName0075c36cPc[11];
    extern const char GameAudioName0075c378Pc[9];
    extern const char GameAudioName0075c3a0Pc[11];
    extern const char GameAudioName0075c3b8Pc[9];
    extern const char GameAudioName0075c3c4Pc[8];
    extern const char GameAudioName0075c3ccPc[7];
    extern i32 SHOPACTIVE;
    extern f32 ShopLockedScalePc;
    extern i16 TextIdPc8808c0;
    void DrawShopPanel(void);
    extern void (*ShopDrawPanelFnPc)(void);
    void InitShop(WORLDINFO *);
    void DrawShop3D(WORLDINFO *);
#ifdef __cplusplus
}
#endif

#endif
