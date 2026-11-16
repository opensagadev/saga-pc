#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PlayerInitCapabilitiesVariant1(GAMEOBJECT *);
    extern f32 jump_stuck_time;
    i32 StarWars_PrepareR2D2Glide(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareHatch(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareHoverTube(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareBigJump(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareTakeOverJump(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareObstacle(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareJump(AIPACKET *, APIOBJECT *, i32);
    void StarWars_GameAISysInit(void);
#ifdef __cplusplus
}
#endif
