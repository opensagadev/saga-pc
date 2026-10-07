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

typedef struct collection_buyable_reader_pc_s {
    i16 id;
    u8 unknown_02[6];
    u8 can_buy;
    u8 unknown_09[0x13];
} COLLECTION_BUYABLE_READER_PC;
STATIC_ASSERT(sizeof(COLLECTION_BUYABLE_READER_PC) == sizeof(COLLECTID_PC));
STATIC_ASSERT(offsetof(COLLECTION_BUYABLE_READER_PC, can_buy) == 8);
STATIC_ASSERT(offsetof(APICHARSYS, char_data) == 0x20);
STATIC_ASSERT(sizeof(APICHARACTERDATA) == 0x4c);
STATIC_ASSERT(offsetof(APICHARACTERDATA, model_flags) == 4);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, model_flags) == 4);

void __cdecl Collection_CreateMaster(char *name, i16 *title_id, COLLECTION_BUILD_PC *collection, i32 columns,
                                     f32 scale) {
    collection->list = CollectList;
    collection->columns = (u16)columns;
    collection->count = (u16)CollectCount;
    collection->title_id = title_id;
    collection->name = name;
    collection->scale = scale;
}

void __cdecl Collection_CreateCustom(char *name, i16 *title_id, COLLECTION_BUILD_PC *collection,
                                     u32 required_model_flags, u32 excluded_model_flags, u32 required_game_flags,
                                     i32 require_buyable, i32 columns, VARIPTR *buffer, VARIPTR *buffer_end,
                                     i32 use_all_characters, f32 scale) {
    i32 index;
    (void)buffer_end;
    collection->columns = (u16)columns;
    collection->count = 0;
    collection->title_id = title_id;
    collection->name = name;
    collection->scale = scale;
    collection->list = (COLLECTID_PC *)((buffer->addr + 3) & ~3u);
    if (use_all_characters == 0) {
        for (index = 0; index < CollectCount; ++index) {
            COLLECTID_PC *source = &CollectList[index];
            i32 id = source->id;
            if (excluded_model_flags != 0 && (apicharsys->char_data[id].model_flags & excluded_model_flags) != 0)
                continue;
            if (require_buyable != 0 && ((COLLECTION_BUYABLE_READER_PC *)source)->can_buy == 0)
                continue;
            if (required_game_flags != 0 && (GCDataList[id].flags_090 & required_game_flags) != required_game_flags)
                continue;
            if (required_model_flags != 0 && (CDataList[id].model_flags & required_model_flags) != required_model_flags)
                continue;
            collection->list[collection->count++] = *source;
        }
    } else {
        for (index = 0; index < CHARCOUNT; ++index) {
            COLLECTID_PC *entry;
            if (excluded_model_flags != 0 && (apicharsys->char_data[index].model_flags & excluded_model_flags) != 0)
                continue;
            if (required_game_flags != 0 && (GCDataList[index].flags_090 & required_game_flags) != required_game_flags)
                continue;
            if (required_model_flags != 0 &&
                (CDataList[index].model_flags & required_model_flags) != required_model_flags)
                continue;
            entry = &collection->list[collection->count++];
            memset(entry, 0, sizeof(*entry));
            entry->id = (i16)index;
        }
    }
    buffer->addr = (u32)(collection->list + collection->count);
}

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
    extern APICHARACTERMODELLIST FreePlayModelList[49];
    extern AREA_EXTRA_MODEL_PC ExtraModelList[6];
    extern COLLECTION_BUILD_PC MiniKitCollection;
    extern i32 bonusmodearcade;
    extern i32 FreePlayModelCount, FreePlayResidentCount, FreePlayBonusCount;
    i32 __cdecl CanWearHatsInFreePlay(i32);
    i32 __cdecl RandomIDFromFlags(u32, u32, i32, APICHARACTERMODELLIST *, i32);
    i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *, u32, u32, i32, i32);
    i32 __cdecl Collection_GetIDList(COLLECTION_BUILD_PC *, u32, u32, i16 *, i32 *, i32 *, i32);
    void __cdecl MakeFreePlayModelList(i32, i32, i32, i32, i32);
    void __cdecl Move_DEFAULT(GAMEOBJECT *);
    extern i16 FreePlayId007f199cPc;
    extern i16 id_THEEMPEROR;
    extern i16 FreePlayId007f1a4cPc;
    extern i16 FreePlayId007f1a48Pc;
    extern i16 FreePlayId007f1a44Pc;
    extern i16 VehicleBoltId7f17d0Pc;
    extern i16 VehicleBoltId7f1aacPc;
    extern i16 FreePlayId007f1ab0Pc;
    extern i16 FreePlayId007f1ab4Pc;
    extern i16 VehicleSpecialShooterIdPc;
    extern i16 FreePlayId007f1abcPc;
    extern i16 id_XWING;
    extern i16 id_YWING;
    extern i16 VehicleBoltId7f1a24Pc;
    extern i16 VehicleBoltId7f19a4Pc;
    extern i16 id_TIEFIGHTER;
    extern i16 VehicleBoltId7f1a28Pc;
    extern i16 VehicleBoltId7f196cPc;
    extern i16 id_TIEBOMBER;
    extern i16 VehicleBoltId7f1a2cPc;
    extern i16 VehicleBoltId7f195cPc;
    extern i16 id_ANAKINSNEWPOD;
    extern i16 VehicleBoltId7f1a9cPc;
    extern i16 VehicleBoltId7f1b34Pc;
    extern i16 id_NEW_REPUBLIC_GUNSHIP;
    extern i16 id_JEDISTARFIGHTERYELLOWEP3;
    extern i16 id_ZAMSSPEEDER;
    extern i16 VehicleBoltId7f19e8Pc;
    extern i16 id_SEBULBASPOD;
    extern i16 FreePlayId007f1b44Pc;
    extern i16 FreePlayId007f1b40Pc;
    extern i16 FreePlayId007f1a98Pc;
#ifdef __cplusplus
}
#endif

i32 __cdecl Collection_GetIDList(COLLECTION_BUILD_PC *collection, u32 mask, u32 required, i16 *out, i32 *first,
                                 i32 *second, i32 unused) {
    i32 index;
    i32 count = 0;
    if (first != NULL)
        *first = -1;
    if (second != NULL)
        *second = -1;
    for (index = 0; index < collection->count; ++index) {
        i32 id = collection->list[index].id;
        if ((CDataList[id].model_flags & mask) != required || Collection_Got(id) == 0)
            continue;
        if (out != NULL)
            out[count] = (i16)id;
        ++count;
        if (first != NULL) {
            if (*first == -1)
                *first = id;
            else if (second != NULL && *second == -1)
                *second = id;
        }
    }
    if (out != NULL)
        out[count] = -1;
    return count;
}
