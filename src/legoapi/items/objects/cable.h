#ifndef SAGA_LEGOAPI_ITEMS_OBJECTS_CABLE_H
#define SAGA_LEGOAPI_ITEMS_OBJECTS_CABLE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

typedef struct CABLE_RECORD_PC CABLE_RECORD_PC;
#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *CableTargetGameObject(GAMEOBJECT *, NUVEC *, f32);
    CABLE_RECORD_PC *CreateCable(GAMEOBJECT *, GAMEOBJECT *, i32);
    extern const f32 AIPositionUnsetPc;
    void InitCables(WORLDINFO *world);
    extern u8 cables[0xf60];
    extern f32 default_targetmomf;
    extern f32 targetmomf;
    extern f32 default_ownermomf;
    extern f32 ownermomf;
    extern f32 default_max_cable_length;
    extern f32 max_cable_length;
    extern f32 default_tow_length;
    extern f32 tow_length;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b208;
    void DestroyCable(struct CABLE_s *cable);
    extern i32 atat_locators[4];
    extern f32 cable_damping;
    extern f32 cable_gravity;
    extern f32 cable_ground_damping;
    extern f32 cable_speed;
    extern f32 cable_slack;
    extern i32 nsegments_drawn;
    extern f32 nsegments_per_unit;
    extern f32 slack_factor;
    extern i32 solid_cable;
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_23
struct CABLE_RECORD_PC {
    GAMEOBJECT *source;      // 0x000
    GAMEOBJECT *target;      // 0x004
    NUVEC points[16];        // 0x008
    NUVEC velocities[16];    // 0x0c8
    f32 segment_lengths[15]; // 0x188
    f32 max_length;          // 0x1c4
    f32 total_length;        // 0x1c8
    u8 unused_01cc[4];       // 0x1cc
    f32 slack;               // 0x1d0
    f32 pull_time;           // 0x1d4
    u8 wrap_indices[15];     // 0x1d8
    u8 point_count;          // 0x1e7
    u8 wrap_count;           // 0x1e8
    u8 unused_01e9;          // 0x1e9
    u16 flags;               // 0x1ea
}; // size 0x1ec

#endif /* TYPES_OFF_23 */
#endif
