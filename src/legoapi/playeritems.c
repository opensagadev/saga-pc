#include "saga.h"
#include <string.h>

static PLAYER_ITEM_TYPE_RESET_PC *PlayerItemTypePc = 0;
static i32 PlayerItemTypeCountPc = 0;

void PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET_PC *types) {
    PlayerItemTypePc = NULL;
    PlayerItemTypeCountPc = 0;
    if (types == NULL) {
        return;
    }
    PlayerItemTypePc = types;
    while (types->id != -1) {
        ++types;
        ++PlayerItemTypeCountPc;
    }
    if (PlayerItemTypeCountPc == 0) {
        PlayerItemTypePc = NULL;
    }
}

void PlayerItemTypes_Reset(WORLDINFO *world) {
    i32 index;
    if (PlayerItemTypePc != NULL) {
        for (index = 0; index < PlayerItemTypeCountPc; ++index) {
            PlayerItemTypePc[index].bolt_id = BoltType_FindIDByName(PlayerItemTypePc[index].bolt_name, world);
        }
    }
}

void PlayerItem_Set(WEAPON_AMMO_PC *item, VEHICLE_WEAPON_ITEM_PC *type) {
    item->type = type;
    if (type != NULL) {
        item->ammo = type->start_ammo;
        item->field_05 = type->start_field_03;
    } else {
        item->ammo = 0;
        item->field_05 = 0;
    }
}

PLAYER_ITEM_TYPE_RESET_PC *PlayerItemType_Find(i32 id) {
    i32 index;
    if (PlayerItemTypePc == NULL) {
        return NULL;
    }
    for (index = 0; index < PlayerItemTypeCountPc; ++index) {
        if (PlayerItemTypePc[index].id == id) {
            return &PlayerItemTypePc[index];
        }
    }
    return NULL;
}

i32 PlayerItem_GotAmmo(WEAPON_AMMO_PC *item) {
    if (item == NULL) {
        return 1;
    }
    if (item->type == NULL) {
        return 1;
    }
    if (item->type->type != 2) {
        return 1;
    }
    return item->ammo > 0;
}
