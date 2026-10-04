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
