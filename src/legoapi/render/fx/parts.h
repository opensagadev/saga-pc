#ifndef SAGA_LEGOAPI_RENDER_FX_PARTS_H
#define SAGA_LEGOAPI_RENDER_FX_PARTS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/objects/game_obj.h,
 * legoapi/items/objects/objectsall.h, nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    PART_PC *FindPart(NUVEC *, i32, GAMEOBJECT *);
    extern char **PDebNameList;
    void AddVariableShotPARTEffect(i32, NUVEC *, f32, f32, NUMTX *);
    void CollectHitPoint(GAMEOBJECT *, NUVEC *, i32);
    void CollectPowerUp(GAMEOBJECT *, NUVEC *, u16, i32);
    void PowerUp_Particles(WORLDINFO *, NUVEC *);
    void KillParts(GAMEOBJECT *, i32, i32, i32, f32, i32, u16 *);
    void SetKillPartMom(NUVEC *);
    void PartImpact_Brick(PART_PC *);
    void MakePartVector(NUVEC *velocity, NUVEC *direction, f32 scale);
    void Parts_Start(WORLDINFO *world);
    void Parts_Stop(WORLDINFO *world);
    void PartStop_Flickerer(PART_PC *);
    i32 PartDraw_Flickerer(PART_PC *);
    void PartCollide_3D(PART_PC *);
    PART_PC *Part_FindFromHSpecial(NUSPECIALHANDLE *);
    void PartKill_ForceThrow(PART_PC *, i32);
    PART_PC *AddPart(ADDPART_PC *);
    PART_PC *FindIncomingPart(void *owner, NUVEC *position, f32 radius, u32 flags, f32 range);
    extern ADDPART_PC Default_ADDPART;
    PART_PC *HitParts(GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, u32);
    void AddVariableShotDebrisEffectTimed5(i32, NUVEC *, NUVEC *, NUVEC *, i32, f32, NUMTX *, NUMTX *, u16, i8);
    void SetPartRTLSet(RTLSET_PC *rtl_set);
    void LoadPartFile(WORLDINFO *);
    i32 FindPartDebris(PARTDEBRISSYS *, char *);
    void AddPartDebris(PARTDEBRISSYS *, i32, NUVEC *);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_28
typedef struct part_runtime_type_pc_s {
    char name[16];              // 0x000
    char object_names[8][16];   // 0x010
    i16 effect_ids[8];          // 0x090
    i8 effect_pages[8];         // 0x0a0
    i8 scene_indices[8];        // 0x0a8
    i8 variant_count;           // 0x0b0
    u8 variant_mode;            // 0x0b1
    i8 page;                    // 0x0b2
    u8 field_b3;                // 0x0b3
    f32 particle_scale;         // 0x0b4
    f32 effect_scale;           // 0x0b8
    f32 lifetime;               // 0x0bc
    f32 lifetime_random;        // 0x0c0
    f32 speed;                  // 0x0c4
    f32 gravity;                // 0x0c8
    f32 emission_rate;          // 0x0cc
    f32 bounce;                 // 0x0d0
    NUVEC position_random;      // 0x0d4
    NUVEC velocity_random;      // 0x0e0
    f32 emission_period;        // 0x0ec
    f32 emission_period_random; // 0x0f0
    f32 emission_pause;         // 0x0f4
    f32 emission_pause_random;  // 0x0f8
    i32 rotation[3];            // 0x0fc
    i32 rotation_random[3];     // 0x108
    u32 flags;                  // 0x114
    i32 trail_effects[2];       // 0x118
    f32 trail_rates[2];         // 0x120
    i32 attached_effect;        // 0x128
    i32 impact_effect;          // 0x12c
    i32 impact_part;            // 0x130
    char impact_part_name[16];  // 0x134
    i32 kill_effect;            // 0x144
    i32 sounds[4];              // 0x148
    u8 sound_modes[4];          // 0x158
    f32 maximum_distance;       // 0x15c
    f32 field_160;              // 0x160
    f32 field_164;              // 0x164
    f32 field_168;              // 0x168
    f32 last_used_time;         // 0x16c
    f32 scale;                  // 0x170
    u32 field_174;              // 0x174
} PART_RUNTIME_TYPE_PC;         // size 0x178

#endif /* TYPES_OFF_28 */
#endif
