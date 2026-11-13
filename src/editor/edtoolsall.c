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

i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *list, u32 model_flags, u32 game_flags, i32 require_hats,
                                 i32 reject_hidden) {
    if (list != NULL) {
        for (; list->model_id != -1; ++list) {
            i32 id;
            if (Collection_Got(list->model_id) == 0)
                continue;
            id = list->model_id;
            if ((GCDataList[id].flags_090 & game_flags) != game_flags ||
                (CDataList[id].model_flags & model_flags) != model_flags ||
                (reject_hidden != 0 && (GCDataList[id].flags_094 & 0x4000) != 0) ||
                ((model_flags & 8) != 0 && (GCDataList[id].flags_094 & 0x8000) != 0))
                continue;
            if (require_hats == 0 || CanWearHatsInFreePlay(id) != 0)
                return 1;
        }
    }
    return 0;
}

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
