#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    void *GameAntinode_RegisterAntiNodeUsingData(void *, NUVEC *, u16, void *, f32, i32);
    void *GameAntinode_RegisterAntiNode(void *, NUVEC *, f32, f32, f32, u16, i32, f32);
    void GameAntinode_FindGridPosition(WORLDINFO *, NUVEC *, f32, f32, u8 *, u8 *, u8 *, u8 *);
    void GameAntinode_Update(void *);
    void GameAntinode_UnregisterAntiNode(void *, void *);
    void *GameAntinode_UpdateAntiNodeUsingData(void *, void *, NUVEC *, u16, void *, f32, i32);
    extern u64 GameAntinodeGrid[64];
#ifdef __cplusplus
}
#endif
