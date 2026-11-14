#ifndef SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/collect/thermaldetonators.h,
 * legoapi/items/objects/objectsall.h, legoapi/legoapi_types.h, legoapi/render/core/rtl.h,
 * legoapi/render/fx/edsplines.h, legoapi/world/levels/episodeV.h, legoapi/world/world.h, nu2api/nu3d/nu3d_includes.h,
 * nu2api/numath/numtx.h, nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART == 1 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP)
typedef struct PART_LIGHTING_DATA_PC PART_LIGHTING_DATA_PC;
typedef void (*PART_MOVE_PC)(PART_PC *, f32);
typedef i32 (*PART_DRAW_PC)(PART_PC *);

struct PART_LIGHTING_DATA_PC {
    u8 unknown_0[0x78];
    NUVEC values[7];
};

STATIC_ASSERT(offsetof(PART_LIGHTING_DATA_PC, values) == 0x78);
STATIC_ASSERT(sizeof(PART_LIGHTING_DATA_PC) == 0xcc);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 objhitobj_noimpactsfx;
    extern i32 objhitobj_noattackerrumble;
    extern i32 objhitobj_throwkillpartsup;
    extern u8 objhit_damage_joints[3];
    extern i32 CombatHitReactionIntervalPc;
    extern f32 CombatReactionVerticalScalePc;
    extern f32 CombatReactionDurationPc;
    extern f32 CombatNpcKillDifficultyAmountPc;
    extern f32 CombatPlayerDeathDifficultyAmountPc;
    extern i16 id_OBIWANKENOBIEP3;
    extern i16 id_ANAKINJEDI;
    extern const char ShieldHitSfxPc[9];
    extern const char ShieldFlickerSfxPc[13];
    extern i32 KillPartsPreviousVariantPc;
    extern char KillPartsTieSpinsTextPc[10];
    extern f32 KillPartsTieTrailScalePc;
    extern NUVEC KillPartsTieSpinTranslationPc;
    extern char KillPartsAtatExplodeTextPc[9];
    extern const NUVEC GAMEOBJECT_ROTATION_BUGGY_OFFSETS[4];
    extern const NUVEC GAMEOBJECT_ROTATION_RADIOCAR_OFFSETS[4];
    extern const NUVEC GAMEOBJECT_ROTATION_POLICECAR_OFFSETS[4];
    extern const NUVEC GAMEOBJECT_ROTATION_MOWER_OFFSETS[4];
    extern const NUVEC GAMEOBJECT_ROTATION_DEFAULT_OFFSETS[4];
    extern i16 id_PKDROID;
    void ManageGameObjects(void);
    extern f32 CurrentSpeed;

    extern u16 GameObjectRotationBuggyId;
    extern u16 GameObjectRotationRadioCarId;
    extern u16 GameObjectRotationPoliceCarId;
    extern u16 GameObjectRotationMowerId;
    extern f32 AIFireIntervalMul;
    extern f32 GhostLightMul;
    extern f32 GhostLightTargetMul;
    extern f32 tieonsfxwait;
    extern f32 tieoffsfxwait;
    extern f32 CurrentSpeedOverride;
    extern f32 BaseCurrentSpeed;
    extern i8 adtabentries[9][4];

    extern f32 InteractionArrowScalePc;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART == 2 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern RTL_DATA_PC KillPartsZeroLightingPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART == 3 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP)

typedef struct ADDPART_PC {
    NUMTX *matrix;
    NUVEC *position;
    NUVEC *velocity;
    GAMEOBJECT *owner;
    GAMEOBJECT *recipient;
    f32 field_14;
    f32 field_18;
    f32 gravity;
    f32 field_20;
    NUSPECIALHANDLE *special;
    i32 part_id;
    u32 flags;
    PART_ACTION_PC callback_30;
    PART_MOVE_PC move_fn;
    PART_ACTION_PC deflect_fn;
    PART_ACTION_PC impact_fn;
    PART_ACTION_PC collision_fn;
    PART_KILL_PC kill_fn;
    PART_ACTION_PC update_fn;
    PART_ACTION_PC stop_fn;
    PART_ACTION_PC destroy_fn;
    PART_DRAW_PC draw_fn;
    i32 effect_id_58;
    i32 *debris_handle;
    i32 effect_id_60;
    i32 effect_id_64;
    f32 field_68;
    f32 field_6c;
    i32 effect_id_70;
    i32 effect_id_74;
    i32 effect_id_78;
    f32 effect_scale;
    i32 *lighting_handle;
    i32 lighting_id;
    f32 field_88;
    f32 time_step;
    u32 hostility_group;
    PART_LIGHTING_DATA_PC *lighting_data;
    f32 lighting_scale_x;
    f32 lighting_scale_y;
    f32 lighting_scale_z;
    f32 field_a4;
    u32 field_a8[6];
    f32 field_c0;
    u8 field_c4;
} ADDPART_PC;

STATIC_ASSERT(sizeof(ADDPART_PC) == 0xc8);
STATIC_ASSERT(offsetof(ADDPART_PC, matrix) == 0);
STATIC_ASSERT(offsetof(ADDPART_PC, velocity) == 8);
STATIC_ASSERT(offsetof(ADDPART_PC, owner) == 0xc);
STATIC_ASSERT(offsetof(ADDPART_PC, recipient) == 0x10);
STATIC_ASSERT(offsetof(ADDPART_PC, field_14) == 0x14);
STATIC_ASSERT(offsetof(ADDPART_PC, gravity) == 0x1c);
STATIC_ASSERT(offsetof(ADDPART_PC, special) == 0x24);
STATIC_ASSERT(offsetof(ADDPART_PC, part_id) == 0x28);
STATIC_ASSERT(offsetof(ADDPART_PC, flags) == 0x2c);
STATIC_ASSERT(offsetof(ADDPART_PC, move_fn) == 0x34);
STATIC_ASSERT(offsetof(ADDPART_PC, deflect_fn) == 0x38);
STATIC_ASSERT(offsetof(ADDPART_PC, impact_fn) == 0x3c);
STATIC_ASSERT(offsetof(ADDPART_PC, collision_fn) == 0x40);
STATIC_ASSERT(offsetof(ADDPART_PC, kill_fn) == 0x44);
STATIC_ASSERT(offsetof(ADDPART_PC, update_fn) == 0x48);
STATIC_ASSERT(offsetof(ADDPART_PC, stop_fn) == 0x4c);
STATIC_ASSERT(offsetof(ADDPART_PC, destroy_fn) == 0x50);
STATIC_ASSERT(offsetof(ADDPART_PC, draw_fn) == 0x54);
STATIC_ASSERT(offsetof(ADDPART_PC, effect_id_58) == 0x58);
STATIC_ASSERT(offsetof(ADDPART_PC, debris_handle) == 0x5c);
STATIC_ASSERT(offsetof(ADDPART_PC, effect_id_60) == 0x60);
STATIC_ASSERT(offsetof(ADDPART_PC, field_68) == 0x68);
STATIC_ASSERT(offsetof(ADDPART_PC, effect_id_70) == 0x70);
STATIC_ASSERT(offsetof(ADDPART_PC, effect_scale) == 0x7c);
STATIC_ASSERT(offsetof(ADDPART_PC, lighting_handle) == 0x80);
STATIC_ASSERT(offsetof(ADDPART_PC, lighting_id) == 0x84);
STATIC_ASSERT(offsetof(ADDPART_PC, field_88) == 0x88);
STATIC_ASSERT(offsetof(ADDPART_PC, time_step) == 0x8c);
STATIC_ASSERT(offsetof(ADDPART_PC, hostility_group) == 0x90);
STATIC_ASSERT(offsetof(ADDPART_PC, lighting_data) == 0x94);
STATIC_ASSERT(offsetof(ADDPART_PC, lighting_scale_x) == 0x98);
STATIC_ASSERT(offsetof(ADDPART_PC, field_a4) == 0xa4);
STATIC_ASSERT(offsetof(ADDPART_PC, field_a8) == 0xa8);
STATIC_ASSERT(offsetof(ADDPART_PC, field_c0) == 0xc0);
STATIC_ASSERT(offsetof(ADDPART_PC, field_c4) == 0xc4);

typedef struct KILLPARTS_BIKE_PC {
    SPLINEPOS spline_position;
    u8 active;
    u8 unknown_21[3];
    GAMEOBJECT *rider;
    NUVEC rider_position;
} KILLPARTS_BIKE_PC;

STATIC_ASSERT(sizeof(KILLPARTS_BIKE_PC) == 0x34);
STATIC_ASSERT(offsetof(KILLPARTS_BIKE_PC, active) == 0x20);
STATIC_ASSERT(offsetof(KILLPARTS_BIKE_PC, rider) == 0x24);
STATIC_ASSERT(offsetof(KILLPARTS_BIKE_PC, rider_position) == 0x28);
#ifdef __cplusplus
extern "C" {
#endif

    void InstantKillParts(GAMEOBJECT *, i32, f32);
    void ObjHitShield(GAMEOBJECT *, GAMEOBJECT *, i32, void *);
    void PlayerTakeHit(GAMEOBJECT *, GAMEOBJECT *);
    i32 PlayerTakeHitReactionPc(GAMEOBJECT *, GAMEOBJECT *, f32, u32);

    void LoseHelmet(GAMEOBJECT *, i32, i32);
    void SpawnCreatureFromCrate(GAMEOBJECT *, f32, f32);
    void KillParts_TIEFIGHTER(ADDPART_PC *, i32, i32, GAMEOBJECT *, i32, u16, u16, NUVEC *);
    void KillParts_ATAT(ADDPART_PC *, i32, i32, GAMEOBJECT *);
    i32 KillGameObject(GAMEOBJECT *, i32, i32);
    extern KILLPARTS_BIKE_PC KillPartsBikesPc[4];
    extern EXPLOSION_PC Explosions[8];
    extern GAMEOBJECT *player_tag_to;
    void ClearLastSafeTakeOver(GAMEOBJECT *);
    void GameObjectToCameraCode(GAMEOBJECT *);
    void CollideGameObjects(WORLDINFO *);
    char *GameObj_GetNamePc(i32, GAMEOBJECT *, char *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART == 4 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void TiePart_Kill(PART_PC *, i32);
    void TiePart_KillExplode(PART_PC *, i32);
    void TiePart_Impact(PART_PC *);
    void TiePart_Move(PART_PC *, f32);
    void TieSpinZPart_Move(PART_PC *, f32);
    void AtatPart_Stop(PART_PC *);
    void AtatPart_Update(PART_PC *);
    void SpeederPart_Kill(PART_PC *, i32);
    void SpeederPart_Update(PART_PC *);
    i32 SpeederPart_Draw(PART_PC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ITEMS_OBJECTS_GAME_OBJ_H_STOP
