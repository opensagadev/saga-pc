#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/characters/core/characters.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LegoContextNetWait;
    extern f32 LevelHackSendTimer;
    extern APICHARACTERMODELLIST NetFreePlayModelList[49];
    extern u8 net_recievedFreePlayList;
    extern i8 net_FreePlayModelCount;
#ifdef __cplusplus
}
#endif
