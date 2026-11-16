#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

struct CABLE_s;

typedef struct CABLE_RECORD CABLE_RECORD;
#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *CableTargetGameObject(GAMEOBJECT *, NUVEC *, f32);
    CABLE_RECORD *CreateCable(GAMEOBJECT *, GAMEOBJECT *, i32);
    extern const f32 AIPositionUnset;
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
    extern LEVELDATA *dat_0087b208;
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
    void DrawCables(void);
    void UpdateCables(void);
#ifdef __cplusplus
}
#endif
