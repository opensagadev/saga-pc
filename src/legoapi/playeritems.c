#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include <string.h>

static PLAYER_ITEM_TYPE_RESET *PlayerItemType = 0;
static i32 PlayerItemTypeCount = 0;

void PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET *types) {
    PlayerItemType = NULL;
    PlayerItemTypeCount = 0;
    if (types == NULL) {
        return;
    }
    PlayerItemType = types;
    while (types->id != -1) {
        ++types;
        ++PlayerItemTypeCount;
    }
    if (PlayerItemTypeCount == 0) {
        PlayerItemType = NULL;
    }
}

void PlayerItemTypes_Reset(WORLDINFO *world) {
    i32 index;
    if (PlayerItemType != NULL) {
        for (index = 0; index < PlayerItemTypeCount; ++index) {
            PlayerItemType[index].bolt_id = BoltType_FindIDByName(PlayerItemType[index].bolt_name, world);
        }
    }
}

void PlayerItem_Set(WEAPON_AMMO *item, VEHICLE_WEAPON_ITEM *type) {
    item->type = type;
    if (type != NULL) {
        item->ammo = type->start_ammo;
        item->unknown_05 = type->start_field_03;
    } else {
        item->ammo = 0;
        item->unknown_05 = 0;
    }
}

PLAYER_ITEM_TYPE_RESET *PlayerItemType_Find(i32 id) {
    i32 index;
    if (PlayerItemType == NULL) {
        return NULL;
    }
    for (index = 0; index < PlayerItemTypeCount; ++index) {
        if (PlayerItemType[index].id == id) {
            return &PlayerItemType[index];
        }
    }
    return NULL;
}

i32 PlayerItem_GotAmmo(WEAPON_AMMO *item) {
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
