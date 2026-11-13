#include "saga.h"
#include <string.h>

/* Retail unit 0x5a8590-0x5a86c4: definitions in retail address order. */

static PLAYER_ITEM_TYPE_RESET_PC *PlayerItemTypePc = 0;
static i32 PlayerItemTypeCountPc = 0;

void __cdecl PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET_PC *types) {
    PlayerItemTypePc = NULL;
    PlayerItemTypeCountPc = 0;
    if (types == NULL)
        return;
    PlayerItemTypePc = types;
    while (types->id != -1) {
        ++types;
        ++PlayerItemTypeCountPc;
    }
    if (PlayerItemTypeCountPc == 0)
        PlayerItemTypePc = NULL;
}

void __cdecl PlayerItemTypes_Reset(WORLDINFO *world) {
    i32 index;
    if (PlayerItemTypePc != NULL) {
        for (index = 0; index < PlayerItemTypeCountPc; ++index)
            PlayerItemTypePc[index].bolt_id = BoltType_FindIDByName(PlayerItemTypePc[index].bolt_name, world);
    }
}

PLAYER_ITEM_TYPE_RESET_PC *__cdecl PlayerItemType_Find(i32 id) {
    i32 index;
    if (PlayerItemTypePc == NULL)
        return NULL;
    for (index = 0; index < PlayerItemTypeCountPc; ++index)
        if (PlayerItemTypePc[index].id == id)
            return &PlayerItemTypePc[index];
    return NULL;
}

i32 __cdecl PlayerItem_GotAmmo(WEAPON_AMMO_PC *item) {
    if (item == NULL)
        return 1;
    if (item->type == NULL)
        return 1;
    if (item->type->type != 2)
        return 1;
    return item->ammo > 0;
}
