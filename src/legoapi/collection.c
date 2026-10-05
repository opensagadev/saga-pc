#include "saga.h"

static i32 CollectCount;
static COLLECTID_PC *CollectList;
COLLECTID_PC *TempCollectID;

i32 __cdecl InCollectList_Index(i32 id, COLLECTID_PC *list, i32 count) {
    i32 index;
    TempCollectID = NULL;
    if (list == NULL) {
        list = CollectList;
        count = CollectCount;
        if (list == NULL)
            return id;
    }
    for (index = 0; index < count; ++index, ++list) {
        if (list->id == id) {
            TempCollectID = list;
            return index;
        }
    }
    return -1;
}

typedef struct coin_reset_packet_pc_s {
    u8 unknown_00[4];
    f32 scale;
    u8 unknown_08[2];
    u8 active;
    u8 unknown_0b;
    f32 double_score_timer;
} COIN_RESET_PACKET_PC;
STATIC_ASSERT(offsetof(COIN_RESET_PACKET_PC, scale) == 4);
STATIC_ASSERT(offsetof(COIN_RESET_PACKET_PC, active) == 0xa);
STATIC_ASSERT(offsetof(COIN_RESET_PACKET_PC, double_score_timer) == 0xc);
void __cdecl ResetCoinPacket(void *packet) {
    COIN_RESET_PACKET_PC *view = (COIN_RESET_PACKET_PC *)packet;
    if (packet != NULL) {
        view->scale = 1.0f;
        view->double_score_timer = 0.0f;
        view->active = 1;
    }
}

i32 __cdecl Collection_Got(i32 id) {
    i32 area;
    if (InCollectList_Index(id, NULL, 0) == -1)
        return 0;
    area = AreaFromMiniKitID(id);
    if (area != -1) {
        if (Game_AreaSave == NULL)
            return 0;
        return Game_AreaSave[area].minikit_complete != 0 ? 2 : 0;
    }
    if (Game_CharacterSave != NULL && (Game_CharacterSave[id] & 1) == 0)
        return 0;
    return 1;
}

void __cdecl UpdateCoinPacket(PLAYER_COIN_PACKET_PC *packet, i32 active, i32 player_index) {
    u32 decrement;
    if (packet == NULL)
        return;
    if (active == 0) {
        packet->active = 1;
        packet->scale = 1.0f;
        packet->multiplier_time = 0.0f;
        if (BonusArea != 0) {
            if (packet->coins < 10) {
                packet->coins = 0;
                return;
            }
            decrement = (u32)((f64)FRAMETIME * 20000.0);
            decrement = (decrement / 10) * 10;
            if (decrement < 10)
                decrement = 10;
            if (decrement > packet->coins)
                packet->coins = 0;
            else
                packet->coins -= decrement;
        }
        return;
    }
    packet->scale = SeekLinearF(packet->scale, 1.0f, (f32)((f64)FRAMETIME * 3.0));
    if (((u32)DoubleScore & (1u << ((u32)player_index & 31))) == 0) {
        packet->active = 1;
        packet->multiplier_time = 0.0f;
    } else if (packet->multiplier_time > 0.0f) {
        packet->multiplier_time -= FRAMETIME;
        if (packet->multiplier_time <= 0.0f)
            packet->active = 1;
    }
}

typedef struct collection_variant_reader_pc_s {
    u8 unknown_00[0x116];
    i8 variant_type;
    u8 unknown_117[9];
} COLLECTION_VARIANT_READER_PC;
STATIC_ASSERT(sizeof(COLLECTION_VARIANT_READER_PC) == 0x120);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(COLLECTION_VARIANT_READER_PC, variant_type) == 0x116);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, model_flags) == 4);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(sizeof(COLLECTID_PC) == 0x1c);
STATIC_ASSERT(offsetof(COLLECTID_PC, id) == 0);
i32 __cdecl Collection_GotAnyOfType(i32 type, u32 flags) {
    i32 index;
    if (CollectList == NULL)
        return 0;
    for (index = 0; index < CollectCount; ++index) {
        i32 id = CollectList[index].id;
        if (type != -1 && ((COLLECTION_VARIANT_READER_PC *)GCDataList)[id].variant_type != type)
            continue;
        if (flags != 0 && (CDataList[id].model_flags & flags) != flags)
            continue;
        if (Collection_Got(id) != 0)
            return 1;
    }
    return 0;
}
