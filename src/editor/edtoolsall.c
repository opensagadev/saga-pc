#include "saga.h"

STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, model_id) == 0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(ARCADE_MODE_PC, flags) == 8);
STATIC_ASSERT(offsetof(ARCADE_ITEM_PC, mode) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

eduimenu_s *submenu0 = NULL;

void __cdecl cbTriggerSubMenu(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    eduimenu_s *submenu = (eduimenu_s *)item->data_ptr;
    if (submenu != NULL) {
        eduiMenuAttach(menu, submenu);
        submenu->x = (i32)((u32)item->x + 10u);
        submenu->y = 50;
        submenu->y = MIN((i32)((u32)PS2_REZ_H * 2u - (u32)submenu->height), 50);
    }
    item->highlighted = 0;
}

void __cdecl cbCancelSubMenuFromItem(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    eduiMenuDetach(menu);
}
