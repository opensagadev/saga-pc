#ifndef SAGA_LEGOAPI_AI_GAME_GAMEANTINODE_H
#define SAGA_LEGOAPI_AI_GAME_GAMEANTINODE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
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
    extern u64 GameAntinodeGridPc[64];
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_3
typedef struct GAME_ANTI_NODE_DATA_PC {
    NUVEC position;       // 0x00
    f32 radius;           // 0x0c
    f32 min_y;            // 0x10
    f32 max_y;            // 0x14
    f32 extent_x;         // 0x18
    f32 extent_z;         // 0x1c
    u16 angle;            // 0x20
    u8 shape;             // 0x22
    u8 mode;              // 0x23
} GAME_ANTI_NODE_DATA_PC; // size 0x24

#endif /* TYPES_OFF_3 */
#endif
