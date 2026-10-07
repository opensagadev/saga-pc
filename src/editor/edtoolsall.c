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

i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *list, u32 model_flags, u32 game_flags, i32 require_hats,
                                 i32 reject_hidden) {
    if (list == NULL)
        return 0;
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
    return 0;
}
