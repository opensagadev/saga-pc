#include "saga.h"

/* Retail bolts unit (0x57f700-0x584590): bolts.c and the bolt runtime were one
   translation unit, so private helpers inline into their callers. Definitions
   follow retail address order; C entry points keep C linkage via extern "C". */

extern "C" {
    extern i32 BoltNoSfxPc, BoltNoObjectMomentumPc;
    extern i32(__cdecl *BoltInitSfxFn)(GAMEOBJECT *);
    void __cdecl FindAnglesXY(NUVEC *, u16 *, u16 *);
    f32 __cdecl NewRayCastGetTOFI(void);
    i32 __cdecl NewRayCastGetImpactTerrainType(void);
    i32 __cdecl Bolt_HitGameObjects(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
    PART_PC *__cdecl Bolt_HitParts(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, i32);
    i32 __cdecl Bolt_HitPartMode(BOLT_PC *);
    void __cdecl Bolt_PlayHitSfx(BOLT_PC *);
    i32 __cdecl GizmoSys_BoltHit(void *, WORLDINFO *, BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
}
STATIC_ASSERT(offsetof(BOLTTYPE_PC, init_callback) == 0x44);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1052) == 0x1052);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1054) == 0x1054);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, facing_direction) == 0xf30);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);

extern "C" {
    void __cdecl AddPartDebris(PARTDEBRISSYS *system, i32 debris_id, NUVEC *position);

    STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
    STATIC_ASSERT(offsetof(BOLT_PC, hit_debris) == 0xfc);
    STATIC_ASSERT(offsetof(BOLT_PC, hit_part_debris) == 0xfe);

    STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
    STATIC_ASSERT(offsetof(BOLT_PC, active) == 0x100);
    STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);

    typedef i32(__cdecl *BOLT_HIT_PART_FN_PC)(BOLT_PC *, PART_PC *);
    STATIC_ASSERT(offsetof(BOLTSYS_PC, hit_part) == 0x18);
    STATIC_ASSERT(offsetof(PART_PC, position) == 0x30);
    STATIC_ASSERT(offsetof(PART_PC, velocity) == 0x80);
    STATIC_ASSERT(offsetof(PART_PC, collision_min) == 0x8c);
    STATIC_ASSERT(offsetof(PART_PC, collision_max) == 0x98);
    STATIC_ASSERT(offsetof(PART_PC, field_e4) == 0xe4);
    STATIC_ASSERT(offsetof(PART_PC, flags) == 0x108);
    STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
    STATIC_ASSERT(sizeof(PART_PC) == 0x220);

    typedef struct BOLT_CONFIG_PC {
        char name[16];
        f32 field_10, field_14, field_18, field_1c, field_20, field_24;
        i16 object_ids[3];
        i16 debris_id;
        i16 moving_debris[2];
        i16 moving_debris_counts[2];
        i16 field_38, field_3a;
        u8 field_3c, field_3d;
        u16 field_3e;
        i32 field_40;
        void *callbacks[7];
        u32 flags_60;
        i16 shoot_sfx_id, hit_sfx_id;
        struct {
            NUSPECIALHANDLE object_special, glow_special, reference_object_special;
            NUSPECIALHANDLE reference_glow_special, shadow_special;
        } specials;
    } BOLT_CONFIG_PC;
    STATIC_ASSERT(sizeof(BOLT_CONFIG_PC) == 0xa4);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, object_ids) == 0x28);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, debris_id) == 0x2e);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, moving_debris) == 0x30);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, moving_debris_counts) == 0x34);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, flags_60) == 0x60);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, shoot_sfx_id) == 0x64);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials) == 0x68);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, name) == 0x0);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_10) == 0x10);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_14) == 0x14);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_1c) == 0x1c);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_20) == 0x20);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_38) == 0x38);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_3c) == 0x3c);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, field_40) == 0x40);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, hit_sfx_id) == 0x66);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.object_special) == 0x68);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.glow_special) == 0x74);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.reference_object_special) == 0x80);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.reference_glow_special) == 0x8c);
    STATIC_ASSERT(offsetof(BOLT_CONFIG_PC, specials.shadow_special) == 0x98);
    static BOLT_CONFIG_PC *BT_bolttype = 0;
    static WORLDINFO *BT_worldinfo = 0;
    static NUGSCN *BT_scene = 0;
    static i32 BT_gdeb_moving_count = 0;
    extern NUGSCN *area_scene, *vehicle_scene;
    i32 __cdecl FindPartDebris(PARTDEBRISSYS *, char *);
    void __cdecl BoltTypes_Configure(WORLDINFO *, char *);
    void __cdecl BoltTypes_Init(WORLDINFO *);

    extern const char BoltAxeSwingSoundPc[14] = "wpn_axe_swing";
    extern const char BoltGunShotSoundPc[8] = "GunShot";
    extern const char BoltImpactSoundPc[12] = "imp_impacts";

    /* Native 44-record owner; configuration publication remains in InitGameAfterConfig. */
    BOLTTYPE_PC GlobalBoltType[44] = {{"red",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{-65484, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"green",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{-65484, 196607, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"blue",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{-65484, 262143, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"trainingremote",
                                       4.0f,
                                       0.5f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       8388608,
                                       {0},
                                       {0},
                                       {0}},
                                      {"walker",
                                       8.0f,
                                       1.0f,
                                       0.0f,
                                       0.125f,
                                       3.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       256,
                                       {0},
                                       {0},
                                       {0}},
                                      {"spaceship",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       8.0f,
                                       0.10000000149011612f,
                                       {{6160477, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"xwing",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{7471217, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"falcon",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{7471217, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"falcon_a",
                                       45.0f,
                                       1.5f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{7471217, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       132352,
                                       {0},
                                       {0},
                                       {0}},
                                      {"falcon360",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{7471217, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       17664,
                                       {0},
                                       {0},
                                       {0}},
                                      {"speederbike",
                                       15.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       2.5f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       256,
                                       {0},
                                       {0},
                                       {0}},
                                      {"snowspeeder",
                                       30.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"tiefighter",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{7602291, 5963775, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"tiefighter_d",
                                       60.0f,
                                       1.0f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{7602291, 5963775, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1288,
                                       {0},
                                       {0},
                                       {0}},
                                      {"tiefighter_dog",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{6291551, 5963775, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"photontorpedo",
                                       30.0f,
                                       2.0f,
                                       -2.5f,
                                       0.10000000149011612f,
                                       3.0f,
                                       0.4000000059604645f,
                                       {{7995513, 4718591, -1, 0}},
                                       -1,
                                       0,
                                       10,
                                       255,
                                       0,
                                       0,
                                       Torpedo_InitBolt,
                                       Torpedo_UpdateBolt,
                                       Torpedo_EndBolt,
                                       Torpedo_InitRicochet,
                                       Torpedo_Scale,
                                       {TorpedoFireSfxPc},
                                       {"imp_proton_torp"},
                                       34642700,
                                       {0},
                                       {0},
                                       {0}},
                                      {"photontorpedo_s",
                                       10.0f,
                                       2.0f,
                                       -2.5f,
                                       0.10000000149011612f,
                                       3.0f,
                                       0.4000000059604645f,
                                       {{7995513, 4784127, -1, 0}},
                                       -1,
                                       0,
                                       10,
                                       255,
                                       0,
                                       0,
                                       Torpedo_InitBolt,
                                       Torpedo_UpdateBolt,
                                       Torpedo_EndBolt,
                                       Torpedo_InitRicochet,
                                       Torpedo_Scale,
                                       {TorpedoFireSfxPc},
                                       {"imp_proton_torp"},
                                       34642700,
                                       {0},
                                       {0},
                                       {0}},
                                      {"dd_double",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{6553699, 131071, -1, 0}},
                                       -1,
                                       0,
                                       2,
                                       18,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       67117056,
                                       {0},
                                       {0},
                                       {0}},
                                      {"dd_deflected",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       67117056,
                                       {0},
                                       {0},
                                       {0}},
                                      {"geonosian",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.10000000149011612f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{7733365, 196607, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       UpdateBolt_Geonosian,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"cannon",
                                       8.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       2.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"minikitred",
                                       12.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       2.0f,
                                       0.10000000149011612f,
                                       {{7471217, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"minifalcon360",
                                       12.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       2.0f,
                                       0.10000000149011612f,
                                       {{7471217, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       17664,
                                       {0},
                                       {0},
                                       {0}},
                                      {"minikitgreen",
                                       12.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       2.0f,
                                       0.10000000149011612f,
                                       {{7602291, 196607, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"atat",
                                       30.0f,
                                       1.0f,
                                       0.0f,
                                       0.125f,
                                       5.0f,
                                       0.10000000149011612f,
                                       {{6160477, 5898239, -1, 0}},
                                       -1,
                                       0,
                                       2,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"walker",
                                       30.0f,
                                       1.0f,
                                       0.0f,
                                       0.125f,
                                       3.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       256,
                                       {0},
                                       {0},
                                       {0}},
                                      {"bobafett",
                                       6.0f,
                                       1.340000033378601f,
                                       0.0f,
                                       0.125f,
                                       2.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       2,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"ewokpebble",
                                       4.0f,
                                       2.0f,
                                       -6.0f,
                                       0.05999999865889549f,
                                       1.0f,
                                       0.0f,
                                       {{-65295, 7667711, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {BoltImpactSoundPc},
                                       4198920,
                                       {0},
                                       {0},
                                       {0}},
                                      {"wicketpebble",
                                       4.0f,
                                       2.0f,
                                       -6.0f,
                                       0.05999999865889549f,
                                       1.0f,
                                       0.0f,
                                       {{-65292, 7667711, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {BoltImpactSoundPc},
                                       4198920,
                                       {0},
                                       {0},
                                       {0}},
                                      {"boulder",
                                       6.0f,
                                       1.340000033378601f,
                                       -6.0f,
                                       0.18000000715255737f,
                                       1.0f,
                                       0.0f,
                                       {{-65296, 7667711, -1, 0}},
                                       -1,
                                       0,
                                       3,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {BoltImpactSoundPc},
                                       3150600,
                                       {0},
                                       {0},
                                       {0}},
                                      {"ewoktorpedo",
                                       4.0f,
                                       2.0f,
                                       -6.0f,
                                       0.05999999865889549f,
                                       1.0f,
                                       0.0f,
                                       {{15925490, 7667711, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       EndBolt_EwokTorpedo,
                                       NULL,
                                       NULL,
                                       {TorpedoFireSfxPc},
                                       {"imp_proton_torp"},
                                       4198920,
                                       {0},
                                       {0},
                                       {0}},
                                      {"wickettorpedo",
                                       4.0f,
                                       2.0f,
                                       -6.0f,
                                       0.05999999865889549f,
                                       1.0f,
                                       0.0f,
                                       {{16122101, 7667711, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       EndBolt_EwokTorpedo,
                                       NULL,
                                       NULL,
                                       {TorpedoFireSfxPc},
                                       {"imp_proton_torp"},
                                       4198920,
                                       {0},
                                       {0},
                                       {0}},
                                      {"jedistarfighter",
                                       40.0f,
                                       1.850000023841858f,
                                       0.0f,
                                       0.125f,
                                       0.75f,
                                       0.10000000149011612f,
                                       {{19792173, 5963775, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"jfightergreen",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       7.0f,
                                       0.10000000149011612f,
                                       {{6291551, 5963775, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"naboofighter",
                                       40.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       6.0f,
                                       0.10000000149011612f,
                                       {{6291551, 5963775, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"bonrepgunshipa",
                                       30.0f,
                                       0.75f,
                                       0.0f,
                                       1.0f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{8192124, 9240575, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       134219008,
                                       {0},
                                       {0},
                                       {0}},
                                      {"bonrepgunshipb",
                                       45.0f,
                                       0.5f,
                                       0.0f,
                                       1.0f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{8192124, 9240575, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"bonrepgunshipa",
                                       30.0f,
                                       0.75f,
                                       0.0f,
                                       1.0f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{8192124, 9240575, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"bonrepgunshipb",
                                       45.0f,
                                       0.5f,
                                       0.0f,
                                       1.0f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{8192124, 9240575, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       1280,
                                       {0},
                                       {0},
                                       {0}},
                                      {"spaceship",
                                       16.0f,
                                       0.75f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{6160477, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"Blaster"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"podracesniper",
                                       50.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       8.0f,
                                       0.10000000149011612f,
                                       {{6160477, 9371647, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"PodX_TuskenBlast"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"podracesniper",
                                       27.5f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       4.400000095367432f,
                                       0.10000000149011612f,
                                       {{6160477, 9371647, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {"PodX_TuskenBlast"},
                                       {"Blast_Hit"},
                                       0,
                                       {0},
                                       {0},
                                       {0}},
                                      {"bullet",
                                       4.0f,
                                       2.0f,
                                       0.0f,
                                       0.125f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{-65484, 131071, -1, 0}},
                                       -1,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {BoltGunShotSoundPc},
                                       {"Blast_Hit"},
                                       570425856,
                                       {0},
                                       {0},
                                       {0}},
                                      {"spear",
                                       3.0f,
                                       2.0f,
                                       -3.0f,
                                       0.15000000596046448f,
                                       1.0f,
                                       0.10000000149011612f,
                                       {{-65509, 9568255, -1, 0}},
                                       14,
                                       0,
                                       1,
                                       255,
                                       0,
                                       0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {BoltAxeSwingSoundPc},
                                       {"wpn_bib_stab"},
                                       3258978304,
                                       {0},
                                       {0},
                                       {0}}};
    STATIC_ASSERT(sizeof(GlobalBoltType) == 7216);
    STATIC_ASSERT(42 * sizeof(BOLTTYPE_PC) + offsetof(BOLTTYPE_PC, object_ids) == 0x1b10);

    BOLTSYS_PC BoltSys_LSW = {GlobalBoltType,
                              44,
                              AlertSurroundingCreatures,
                              Bolt_Debris_LSW,
                              GetShootOrigin_LSW,
                              GetShootDirection_LSW,
                              Bolt_HitPart_LSW,
                              Bolt_AlternateFire_LSW};
    BOLTSYS_PC BoltSys_Batman = {GlobalBoltType, 44,  AlertSurroundingCreatures, NULL, NULL, GetShootDirection_Batman,
                                 NULL,           NULL};
}

extern "C" {
    u32 __cdecl GetLevelExBlowupFlags(void);
    i32 __cdecl GizmoSys_BoltHitPlat(void *, WORLDINFO *, BOLT_PC *, u8 *);
    void __cdecl Bolt_AddDeflectedBolt(BOLT_PC *, NUVEC *, NUVEC *, u8 *);
    GIZMOBLOWUP_PC *__cdecl GizmoBlowUp_Hit(GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT_PC *, u32, u8 *);
}
/* Private collision helpers and callers retain their original translation-unit group. */
static i32 Bolt_HitPlatPrivate(BOLT_PC *, u8 *);
static i32 Bolt_HitBlowupPrivate(BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);

extern "C" i32 FindAnglesXFullTempPc;
extern "C" BOLT_PC *__cdecl Bolt_Add(GAMEOBJECT *, NUVEC *, NUMTX *, i32, i32);

extern "C" {
    void __cdecl CalculateInterceptVector(NUVEC *, NUVEC *, NUVEC *, f32, NUVEC *, NUVEC *);
    i32 __cdecl Player_HasDeflectBolts(GAMEOBJECT *);
    i32 __cdecl Player_HasDoubleBoltDamage_FromBolt(BOLT_PC *);
    extern i32 BoltBlockContextPc;
    extern i16 BoltDeactivatedActionPc;
}
typedef struct bolt_context_info_pc_s {
    const char *name;
    i32 action;
    u64 flags;
} BOLT_CONTEXT_INFO_PC;
STATIC_ASSERT(sizeof(BOLT_CONTEXT_INFO_PC) == 0x10);
STATIC_ASSERT(offsetof(BOLT_CONTEXT_INFO_PC, flags) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale_override) == 0xd30);
STATIC_ASSERT(offsetof(GAMEOBJECT, scaled_radius) == 0xa4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, walk_speed) == 0x18);

extern "C" {
    void __cdecl Bolt_Reflect(NUVEC *, NUVEC *, NUVEC *);
    void __cdecl NewTerrHitInfo(u8 *);
    void __cdecl NewRayCastGetImpactNormal(NUVEC *);
}

typedef struct BOLT_UPDATE_TYPE_PC_s {
    u8 unknown_000[0x98];
    NUSPECIALHANDLE shadow_special;
} BOLT_UPDATE_TYPE_PC;
STATIC_ASSERT(offsetof(BOLT_UPDATE_TYPE_PC, shadow_special) == 0x98);
STATIC_ASSERT(sizeof(BOLT_UPDATE_TYPE_PC) == sizeof(BOLTTYPE_PC));

STATIC_ASSERT(offsetof(BOLT_PC, effect_orientation) == 0x48);
STATIC_ASSERT(offsetof(BOLT_PC, scale) == 0xc4);
STATIC_ASSERT(offsetof(BOLT_PC, field_e4) == 0xe4);
STATIC_ASSERT(offsetof(BOLT_PC, reflection_height) == 0xe8);
STATIC_ASSERT(offsetof(BOLT_PC, surface_x_rotation) == 0xf4);
STATIC_ASSERT(offsetof(BOLT_PC, surface_z_rotation) == 0xf6);
STATIC_ASSERT(offsetof(BOLT_PC, field_102) == 0x102);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_24) == 0x24);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, flags_60) == 0x60);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, specials_68) == 0x68);
STATIC_ASSERT(sizeof(NUSPECIALHANDLE) == 0xc);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);

extern "C" i32 __cdecl MatrixReflectionVU0_AXISY(NUMTX *, f32, f32, NUMTX *);

extern "C" {
    static void NuMtxSetRotationYInline(NUMTX *matrix, NUANG angle) {
        f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
        matrix->m00 = cosine;
        matrix->m01 = 0.0f;
        matrix->m02 = -sine;
        matrix->m03 = 0.0f;
        matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
        matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
        matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
        matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
        matrix->m20 = sine;
        matrix->m21 = 0.0f;
        matrix->m22 = cosine;
        matrix->m23 = 0.0f;
        matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
        matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
        matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
        matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    }
}

// Retail private rotation leaves take the matrix in EAX and angle in ECX (VC8 internal ABI).
extern "C" {
    static __declspec(noinline) NUMTX *__cdecl BoltDrawRotateXPrivate(NUMTX *m, i32 angle) {
        f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
        f32 m01 = m->m01;
        f32 m11 = m->m11;
        f32 m21 = m->m21;
        f32 m31 = m->m31;

        m->m01 = m01 * cos_x - m->m02 * sin_x;
        m->m02 = m01 * sin_x + m->m02 * cos_x;
        m->m11 = m11 * cos_x - m->m12 * sin_x;
        m->m12 = m11 * sin_x + m->m12 * cos_x;
        m->m21 = m21 * cos_x - m->m22 * sin_x;
        m->m22 = m21 * sin_x + m->m22 * cos_x;
        m->m31 = m31 * cos_x - m->m32 * sin_x;
        m->m32 = m31 * sin_x + m->m32 * cos_x;
        return m;
    }

    static __declspec(noinline) NUMTX *__cdecl BoltDrawRotateZPrivate(NUMTX *m, i32 angle) {
        f32 cos_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        f32 sin_z = NuTrigTable[(angle >> 1) & 0x7fff];
        f32 m00 = m->m00;
        f32 m10 = m->m10;
        f32 m20 = m->m20;
        f32 m30 = m->m30;

        m->m00 = m00 * cos_z - m->m01 * sin_z;
        m->m01 = m00 * sin_z + m->m01 * cos_z;
        m->m10 = m10 * cos_z - m->m11 * sin_z;
        m->m11 = m10 * sin_z + m->m11 * cos_z;
        m->m20 = m20 * cos_z - m->m21 * sin_z;
        m->m21 = m20 * sin_z + m->m21 * cos_z;
        m->m30 = m30 * cos_z - m->m31 * sin_z;
        m->m31 = m30 * sin_z + m->m31 * cos_z;
        return m;
    }
}

extern "C" void Bolts_Reset() {
    memset(Bolt, 0, sizeof(Bolt));
    i_bolt = 0;
    BOLT_OVERRIDE_PLAYERBOLTSPEED = 0.0f;
    BOLT_OVERRIDE_PLAYERBOLTDURATION = 0.0f;
}

static i32 Bolt_RayCastPrivate(BOLT_PC *bolt, NUVEC *start, NUVEC *movement) {
    NUVEC end;
    NuVecAdd(&end, start, movement);
    GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
    i32 platform = -1, hit;
    if (owner && owner->level_object_index != -1) {
        platform = owner->level_object_index;
        PlatOnOff(platform, 0);
    }
    hit = NewRayCastScaleYMask(start, movement, 0.0f, 1.0f, 0, 0) != 0;
    if (hit) {
        bolt->hit_platform = (i16)TerrainPlatId();
        bolt->field_104 = (i8)NewRayCastGetImpactTerrainType();
        NuVecAdd(&bolt->dogfight_hit_position, start, movement);
        bolt->hit_normal = ShadNorm;
    } else {
        bolt->hit_platform = -1;
        bolt->field_104 = -1;
    }
    if (platform != -1)
        PlatOnOff(platform, 1);
    bolt->ray_end = end;
    IgnoreWallSplines = 0;
    return hit;
}

static void Bolt_GeneratePointsPrivate(BOLT_PC *bolt, NUVEC *points) {
    points[1] = bolt->position;
    if (!(bolt->flags & 0x200)) {
        NUVEC offset = {0, 0, bolt->collision_radius + bolt->collision_radius};
        NuVecMtxRotate(&offset, &offset, &bolt->orientation);
        NuVecSub(&points[0], &bolt->position, &offset);
        NuVecAdd(&points[2], &bolt->position, &offset);
    }
}

static void Bolt_OrientationPrivate(BOLT_PC *bolt) {
    u16 x, y;
    FindAnglesXY(&bolt->direction, &x, &y);
    NuMtxSetRotationX(&bolt->effect_orientation, x + 0x4000);
    NuMtxRotateY(&bolt->effect_orientation, y);
    NuMtxSetRotationX(&bolt->orientation, x);
    NuMtxRotateY(&bolt->orientation, y);
}

extern "C" {
    BOLT_PC *__cdecl Bolt_Alloc(void) {
        BOLT_PC *bolt = NULL;
        i32 attempts = 0;
        while (Bolt[i_bolt].active != 0 && (Bolt[i_bolt].flags & 8) != 0 && attempts < 32) {
            if (++i_bolt == 32)
                i_bolt = 0;
            ++attempts;
        }
        if (attempts <= 32) {
            bolt = &Bolt[i_bolt];
            i_bolt = (i_bolt + 1) % 32;
        }
        return bolt;
    }
}

extern "C" void __cdecl Bolt_Reflect(NUVEC *normal, NUVEC *incoming, NUVEC *out) {
    f32 twice_dot = (normal->x * incoming->x + normal->y * incoming->y + normal->z * incoming->z) * 2.0f;
    out->x = incoming->x - normal->x * twice_dot;
    out->y = incoming->y - normal->y * twice_dot;
    out->z = incoming->z - normal->z * twice_dot;
}

extern "C" void __cdecl Bolt_AddDeflectedBolt(BOLT_PC *bolt, NUVEC *incoming, NUVEC *normal, u8 *processed) {
    BOLTTYPE_PC *type = bolt->type;
    NUVEC reflected, position;
    NUMTX matrix;
    NUANGVEC angles;
    i32 type_id;
    f64 offset;
    if (type->flags_60 & 0x100000)
        return;
    f32 twice_dot = (normal->x * incoming->x + normal->y * incoming->y + normal->z * incoming->z) * 2.0f;
    reflected.x = incoming->x - normal->x * twice_dot;
    reflected.y = incoming->y - normal->y * twice_dot;
    reflected.z = incoming->z - normal->z * twice_dot;
    NuVecNorm(&reflected, &reflected);
    FindAnglesXY(&reflected, NULL, NULL);
    if ((u8)bolt->field_104 > 31 || !(TerSurface[bolt->field_104].flags & 0x800)) {
        FindAnglesXFullTempPc += (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 - 5461.0);
        ConfigBuffer.fields.find_angles_y_rotation =
            (u16)(ConfigBuffer.fields.find_angles_y_rotation +
                  (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 - 5461.0));
    }
    angles.x = FindAnglesXFullTempPc;
    angles.y = ConfigBuffer.fields.find_angles_y_rotation;
    NuMtxSetRotationXYVU0(&matrix, &angles);
    type_id = (i8)type->field_3d;
    if (type_id < 0 || type_id >= BoltSys->count)
        type_id = bolt->type_id;
    offset = (f64)1.01f * bolt->ray_radius;
    position = bolt->position;
    position.x = (f32)(position.x + reflected.x * offset);
    position.y = (f32)(position.y + reflected.y * offset);
    position.z = (f32)(position.z + reflected.z * offset);
    BoltNoSfxPc = 1;
    BOLT_PC *created = Bolt_Add(NULL, &position, &matrix, type_id, bolt->hit_flags);
    if (created) {
        created->flags |= 0x10000000;
        if (processed)
            processed[created->index] = 1;
    }
}

extern "C" {
    void __cdecl Bolt_PlayHitSfx(BOLT_PC *bolt) {
        if (bolt->type->field_66 != -1)
            GameAudio_PlaySfxById(bolt->type->field_66, &bolt->position, 0, 0);
        else
            GameAudio_PlaySfx(0x28, &bolt->position, 0, 0);
    }

    BOLT_PC *__cdecl FindIncomingBolt(GAMEOBJECT *object, i32 check_owner, i32 mark_hit) {
        f32 radius;
        f32 radius_squared;
        f32 nearest;
        BOLT_PC *result;
        BOLT_PC *bolt;
        f32 half_speed;
        f32 limit;
        f32 distance;
        i32 index;
        radius =
            object->bounds_half_height > object->bounds_radius ? object->bounds_half_height : object->bounds_radius;
        radius_squared = radius * radius;
        result = NULL;
        nearest = 100000000.0f;
        bolt = Bolt;
        for (index = 0; index < 32; ++index, ++bolt) {
            if (bolt->active == 0)
                continue;
            if (check_owner != 0 && bolt->owner != NULL && (((GAMEOBJECT *)bolt->owner)->flags_1fc_word & 1) != 0 &&
                (((GAMEOBJECT *)bolt->owner)->flags_1fc_word & 0x1000) != 0 && ((GAMEOBJECT *)bolt->owner)->dead == 0 &&
                ((GAMEOBJECT *)bolt->owner)->field_280 != 0xff)
                continue;
            half_speed = (f32)((f64)bolt->speed * 0.5);
            distance = NuVecDistSqr(&bolt->position, &object->collision_position, NULL);
            limit = half_speed * half_speed;
            if (distance < limit &&
                LineIntersectSphere(&bolt->position, &bolt->direction, &object->collision_position, radius_squared,
                                    NULL) &&
                distance < nearest) {
                nearest = distance;
                result = bolt;
            }
        }
        if (result != NULL && mark_hit != 0) {
            radius = (f32)((f64)object->bounds_radius * 0.75);
            radius_squared = radius * radius;
            if (LineIntersectSphereXZPc(&result->position, &result->direction, &object->collision_position,
                                        radius_squared))
                object->flags_e20 |= 0x2000;
        }
        return result;
    }

    BOLT_PC *__cdecl Bolt_Find(i32 type_id, NUVEC *position, GAMEOBJECT *owner) {
        f32 nearest_distance = 1000000.0f;
        BOLT_PC *nearest = NULL;
        BOLT_PC *bolt = Bolt;
        i32 index;
        for (index = 0; index < 32; ++index, ++bolt) {
            f32 distance;
            if (bolt->active == 0 || (type_id != -1 && bolt->type_id != type_id) ||
                (owner != NULL && bolt->owner != owner))
                continue;
            if (position == NULL)
                return bolt;
            distance = NuVecDistSqr(&bolt->position, position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = bolt;
            }
        }
        return nearest;
    }
}

extern "C" void __cdecl Bolt_End(BOLT_PC *bolt, i32 run_callback) {
    bolt->active = 0;
    if (run_callback != 0 && bolt->type != NULL && bolt->type->end_callback != NULL)
        bolt->type->end_callback(bolt);
}

extern "C" {
    static void BoltTypeStoreConfigPrivatePc(BOLT_CONFIG_PC *type, WORLDINFO *world) {
        i32 index;
        if (type == NULL)
            return;
        for (index = 0; index < 8; ++index) {
            if (NuStrLen(world->bolt_types[index].name) == 0) {
                memcpy(&world->bolt_types[index], type, sizeof(*type));
                return;
            }
        }
    }

    static void BT_name(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 15) {
            NuStrCpy(BT_bolttype->name, parser->word_buffer_ptr);
        }
    }

    static void BT_speed(NUFPAR_PC *parser) {
        BT_bolttype->field_10 = NuFParGetFloat(parser);
    }

    static void BT_duration(NUFPAR_PC *parser) {
        BT_bolttype->field_14 = NuFParGetFloat(parser);
    }

    static void BT_radius(NUFPAR_PC *parser) {
        BT_bolttype->field_1c = NuFParGetFloat(parser);
    }

    static void BT_scale(NUFPAR_PC *parser) {
        BT_bolttype->field_20 = NuFParGetFloat(parser);
    }

    static void BT_rand_angle(NUFPAR_PC *parser) {
        BT_bolttype->field_40 = (i32)((f64)NuFParGetFloat(parser) * (f64)(65536.0f / 360.0f));
    }

    static void BT_sceneconfig(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) == 0) {
            return;
        }
        if (NuStrICmp(parser->word_buffer_ptr, "level") == 0) {
            BT_scene = BT_worldinfo->current_gscn;
        } else if (NuStrICmp(parser->word_buffer_ptr, "area") == 0) {
            BT_scene = area_scene;
        } else if (NuStrICmp(parser->word_buffer_ptr, "vehicle") == 0) {
            BT_scene = vehicle_scene;
        } else {
            BT_scene = things_scene;
        }
    }

    static void BT_obj(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
            if (NuSpecialFind(BT_scene, &BT_bolttype->specials.object_special, parser->word_buffer_ptr, 1) != 0) {
                BT_bolttype->specials.reference_object_special = BT_bolttype->specials.object_special;
            }
        }
    }

    static void BT_ref_obj(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
            NuSpecialFind(BT_scene, &BT_bolttype->specials.reference_object_special, parser->word_buffer_ptr, 1);
        }
    }

    static void BT_glow_obj(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
            if (NuSpecialFind(BT_scene, &BT_bolttype->specials.glow_special, parser->word_buffer_ptr, 1) != 0) {
                BT_bolttype->specials.reference_glow_special = BT_bolttype->specials.glow_special;
            }
        }
    }

    static void BT_ref_glow_obj(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
            NuSpecialFind(BT_scene, &BT_bolttype->specials.reference_glow_special, parser->word_buffer_ptr, 1);
        }
    }

    static void BT_shadow_obj(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0 && BT_scene != NULL) {
            NuSpecialFind(BT_scene, &BT_bolttype->specials.shadow_special, parser->word_buffer_ptr, 1);
        }
    }

    static void BT_debris(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0) {
            BT_bolttype->debris_id = (i16)(FindGameDebris(BT_worldinfo->game_debris_sys, parser->word_buffer_ptr));
        }
    }

    static void BT_debris_moving(NUFPAR_PC *parser) {
        if (BT_gdeb_moving_count < 2 && NuFParGetWord(parser) != 0) {
            BT_bolttype->moving_debris[BT_gdeb_moving_count] =
                (i16)FindGameDebris(BT_worldinfo->game_debris_sys, parser->word_buffer_ptr);
            BT_bolttype->moving_debris_counts[BT_gdeb_moving_count] = (i16)abs((i32)NuFParGetFloat(parser));
            BT_gdeb_moving_count++;
        }
    }

    static void BT_part_hit(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0) {
            BT_bolttype->field_38 = (i16)(FindPartDebris(BT_worldinfo->part_debris_sys, parser->word_buffer_ptr));
        }
    }

    static void BT_sfx_shoot(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0) {
            BT_bolttype->shoot_sfx_id = (i16)(GetSfxId(parser->word_buffer_ptr));
        }
    }

    static void BT_sfx_hit(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0) {
            BT_bolttype->hit_sfx_id = (i16)(GetSfxId(parser->word_buffer_ptr));
        }
    }

    static void BT_damage(NUFPAR_PC *parser) {
        BT_bolttype->field_3c = (u8)(NuFParGetInt(parser));
    }

    static void BT_nodeflect(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 0x100;
    }

    static void BT_converge(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 0x400;
    }

    static void BT_canonlyhitplayers(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 0x80;
    }

    static void BT_no_terrain(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 4;
    }

    static void BT_no_collide(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 0x10000;
    }

    static void BT_trooper_bolt(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 0x80000;
    }

    static void BT_single_debris(NUFPAR_PC *parser) {
        BT_bolttype->flags_60 |= 0x40000000;
    }

    static NUFPCOMJMP BoltType_ConfigKeywords[] = {
        {("name"), BT_name},
        {("speed"), BT_speed},
        {("duration"), BT_duration},
        {("radius"), BT_radius},
        {("scale"), BT_scale},
        {("rand_angle"), BT_rand_angle},
        {("scene"), BT_sceneconfig},
        {("obj"), BT_obj},
        {("ref_obj"), BT_ref_obj},
        {("glow_obj"), BT_glow_obj},
        {("ref_glow_obj"), BT_ref_glow_obj},
        {("shadow_obj"), BT_shadow_obj},
        {("debris"), BT_debris},
        {("debris_moving"), BT_debris_moving},
        {("part_hit"), BT_part_hit},
        {("sfx"), BT_sfx_shoot},
        {("sfx_shoot"), BT_sfx_shoot},
        {("sfx_hit"), BT_sfx_hit},
        {("damage"), BT_damage},
        {("nodeflect"), BT_nodeflect},
        {("converge"), BT_converge},
        {("canonlyhitplayers"), BT_canonlyhitplayers},
        {("no_terrain"), BT_no_terrain},
        {("no_collide"), BT_no_collide},
        {("trooper_bolt"), BT_trooper_bolt},
        {("single_debris"), BT_single_debris},
        {NULL, NULL},
    };

    BOLTTYPE_PC *__cdecl BoltType_FindByID(i32 id, WORLDINFO *world) {
        BOLTSYS_PC *system = BoltSys;
        if (id >= 0 && id < system->count)
            return &system->types[id];

        if (world != NULL && id >= system->count && id < system->count + 8) {
            return &world->bolt_types[id - system->count];
        }

        return NULL;
    }

    i32 __cdecl BoltType_FindIDByName(const char *name, WORLDINFO *world) {
        i32 index;
        if (NuStrLen(name) != 0) {
            for (index = 7; index >= 0; --index)
                if (NuStrICmp(name, world->bolt_types[index].name) == 0)
                    return BoltSys->count + index;
            for (index = BoltSys->count + 7; index >= 0; --index)
                if (NuStrICmp(name, BoltSys->types[index].name) == 0)
                    return index;
        }
        return -1;
    }

    void __cdecl BoltTypes_Init(WORLDINFO *world) {
        BOLT_CONFIG_PC *type = (BOLT_CONFIG_PC *)BoltSys->types;
        i32 index;
        for (index = 0; index < BoltSys->count; ++index, ++type) {
            i32 reflected;
            memset(&type->specials.object_special, 0, sizeof(NUSPECIALHANDLE));
            memset(&type->specials.glow_special, 0, sizeof(NUSPECIALHANDLE));
            memset(&type->specials.reference_object_special, 0, sizeof(NUSPECIALHANDLE));
            memset(&type->specials.reference_glow_special, 0, sizeof(NUSPECIALHANDLE));
            if (type->object_ids[0] != -1) {
                type->specials.object_special = world->level_objects[type->object_ids[0]].special;
                if (type->object_ids[1] != -1)
                    type->specials.glow_special = world->level_objects[type->object_ids[1]].special;
                reflected = LevelObject_GetReflection(type->object_ids[0]);
                if (reflected != -1) {
                    type->specials.reference_object_special = world->level_objects[reflected].special;
                    if (type->object_ids[1] != -1) {
                        reflected = LevelObject_GetReflection(type->object_ids[1]);
                        if (reflected != -1)
                            type->specials.reference_glow_special = world->level_objects[reflected].special;
                    }
                } else {
                    type->specials.reference_object_special = type->specials.object_special;
                    type->specials.reference_glow_special = type->specials.glow_special;
                }
            }
            if (type->object_ids[2] != -1)
                type->specials.shadow_special = world->level_objects[type->object_ids[2]].special;
            else
                memset(&type->specials.shadow_special, 0, sizeof(NUSPECIALHANDLE));
        }
    }

    void __cdecl Bolt_GetShootOrigin_Default(GAMEOBJECT *object, NUVEC *position) {
        *position = object->collision_position;
    }

    u16 __cdecl Bolt_GetShootDirection_Default(GAMEOBJECT *object, NUVEC *direction) {
        if (direction != NULL) {
            direction->x = NuTrigTable[object->movement_facing_angle >> 1];
            direction->y = 0.0f;
            direction->z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
        }
        return object->movement_facing_angle;
    }

    f32 __cdecl Bolt_ObjTargetPosYAdjust(GAMEOBJECT *object) {
        f32 height = object->field_b0;
        i32 random = qrand();
        return (f32)(random * ((height + height) / BoltTargetRandomDenominator) - height);
    }

    i32 __cdecl Bolt_HitPartMode(BOLT_PC *bolt) {
        GAMEOBJECT *owner = (GAMEOBJECT *)bolt->owner;
        if (owner != NULL) {
            if (owner->field_280 == 0)
                return 4;
            if (owner->field_280 == 1)
                return 5;
        }
        return 3;
    }

    PART_PC *__cdecl Bolt_HitParts(BOLT_PC *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius,
                                   i32 reason) {
        PART_PC *part = Part;
        i32 i, point;
        for (i = 0; i < MAXPARTS; ++i, ++part) {
            if (!(part->active & 1))
                continue;
            if (!(part->flags & 0x08000000) && (part->flags & 0xa) != 0xa)
                continue;
            if (minimum->x > part->collision_max.x || part->collision_min.x > maximum->x ||
                minimum->z > part->collision_max.z || part->collision_min.z > maximum->z ||
                minimum->y > part->collision_max.y || part->collision_min.y > maximum->y)
                continue;
            for (point = 2; point >= 0; --point) {
                if ((bolt->flags & 0x200) && point != 1)
                    continue;
                if (SphereSphereOverlapScaleY(&part->position, part->field_e4, part->field_e4, &points[point], radius,
                                              radius))
                    break;
            }
            if (point < 0)
                continue;
            BoltSys->debris(bolt, points, point, &part->velocity, 0);
            Bolt_PlayHitSfx(bolt);
            if (bolt->owner && (((GAMEOBJECT *)bolt->owner)->flags_1fc & 1))
                NewRumble(((GAMEOBJECT *)bolt->owner)->pad->bound_pad, .7f, 0);
            bolt->active = 0;
            if (bolt->type && bolt->type->end_callback)
                bolt->type->end_callback(bolt);
            if (BoltSys->hit_part && BoltSys->hit_part(bolt, part) == 1)
                return part;
            KillPart(part, reason);
            return part;
        }
        return NULL;
    }

    void __cdecl Bolt_Debris_Default(BOLT_PC *bolt, NUVEC *points, i32 point, NUVEC *velocity, i32 flags) {
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        i32 index;

        (void)velocity;
        (void)flags;

        for (index = 0; index < 3; ++index) {
            if ((bolt->flags & 0x40000200) != 0 && point == -1) {
                if (index != 1)
                    continue;
            } else if (point != -1 && point != index) {
                continue;
            }

            AddGameDebris(world->game_debris_sys, bolt->hit_debris, &points[index]);
        }

        if (bolt->hit_part_debris != -1)
            AddPartDebris(world->part_debris_sys, bolt->hit_part_debris, &bolt->position);
    }

    void __cdecl BoltSys_Init(BOLTSYS_PC *system) {
        i32 i;
        for (i = 0; i < system->count; ++i) {
            system->types[i].shoot_sound_id = -1;
            if (system->types[i].shoot_sound != NULL)
                system->types[i].shoot_sound_id = (i16)GetSfxId((char *)system->types[i].shoot_sound);
            system->types[i].hit_sound_id = -1;
            if (system->types[i].hit_sound != NULL)
                system->types[i].hit_sound_id = (i16)GetSfxId((char *)system->types[i].hit_sound);
        }
        BoltSys = system;
        if (BoltSys->debris == NULL)
            BoltSys->debris = Bolt_Debris_Default;
        if (BoltSys->shoot_origin == NULL)
            BoltSys->shoot_origin = Bolt_GetShootOrigin_Default;
        if (BoltSys->shoot_direction == NULL)
            BoltSys->shoot_direction = Bolt_GetShootDirection_Default;
    }
}

static i32 Bolt_HitPlatPrivate(BOLT_PC *bolt, u8 *hit_flags) {
    i32 blowup_hit = 0;
    u32 exclude = GetLevelExBlowupFlags();
    Bolt_PlayHitSfx(bolt);
    if ((bolt->hit_flags & 0x800) != 0 && GizmoSys_BoltHitPlat(WORLD->gizmo_sys, WORLD, bolt, hit_flags))
        return 1;
    GIZMOBLOWUP_PC *blowup = GizmoBlowUp_FindFromPlatID(WORLD, bolt->hit_platform);
    if (blowup != NULL) {
        if ((blowup->draw_flags & 0x8000) != 0) {
            if ((exclude & 1) != 0 && bolt->owner != NULL) {
                i32 field_028_positive =
                    static_cast<GAMEOBJECT *>(bolt->owner)->character_data->game_character->field_028 > 0.0f;
                if ((blowup->draw_flags & 0x80000) != 0 && field_028_positive)
                    return 0;
            }
            if (bolt->owner != NULL && (blowup->draw_flags & 0x80000) != 0 &&
                static_cast<GAMEOBJECT *>(bolt->owner)->rider == NULL)
                return 0;
            if (GizmoBlowupBlowup(blowup, 1, 3, bolt->type->field_3c, NULL, 1) != 0 && bolt->owner != NULL)
                NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.4f, 0);
            return 1;
        }
        blowup_hit = 1;
    }
    if (bolt->field_104 != -1 && ((TerSurface[bolt->field_104].flags & 0x1800) != 0 || blowup_hit)) {
        GameAudio_PlaySfx(41, &bolt->position, 0, 0);
        Bolt_AddDeflectedBolt(bolt, &bolt->direction, &bolt->hit_normal, hit_flags);
        if (bolt->owner != NULL && (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0) {
            NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.4f, 0);
            GameCam_HitJudder();
        }
    }
    return 0;
}

static i32 Bolt_HitBlowupPrivate(BOLT_PC *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius,
                                 u8 *hit_flags) {
    NUVEC *hit_points;
    i32 count;
    if (bolt->flags & 0x200) {
        hit_points = &points[1];
        count = 1;
    } else {
        hit_points = points;
        count = 3;
    }
    if (GizmoBlowUp_Hit(static_cast<GAMEOBJECT *>(bolt->owner), hit_points, count, radius, minimum, maximum, bolt, 1,
                        hit_flags)) {
        BoltSys->debris(bolt, points, -1, NULL, 0);
        if (bolt->owner)
            NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, .6f, 0);
        Bolt_End(bolt, 1);
        Bolt_PlayHitSfx(bolt);
        return 1;
    }
    return 0;
}

extern "C" void __cdecl Bolt_HitGameObjectRC(NETMESSAGE_PC &message) {
    u16 bolt_index, object_index;
    NUVEC points[3];
    i32 hit, client;
    message.Read16(bolt_index);
    message.Read16(object_index);
    message.Read(&points[0], 4, 3);
    message.Read(&points[1], 4, 3);
    message.Read(&points[2], 4, 3);
    message.Read32(hit);
    message.Read32(client);
    BOLT_PC *bolt = &Bolt[bolt_index];
    GAMEOBJECT *object = &Obj[object_index];
    BOLTTYPE_PC *type = bolt->type;
    GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
    if (BoltSys->stop_targeting != NULL && bolt->owner != NULL &&
        (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0)
        BoltSys->stop_targeting(static_cast<GAMEOBJECT *>(bolt->owner), &bolt->position);
    i32 deflect;
    if ((object->flags_1fc & 0x80) != 0 && Player_HasDeflectBolts(object) != 0)
        deflect = 1;
    else
        deflect = 0;
    i32 cheat_deflect;
    if (Cheats_CheckFlags(0x100000) != 0 && (object->flags_1fc & 0x80) != 0)
        cheat_deflect = 1;
    else
        cheat_deflect = 0;
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    if ((bolt->flags & 0x100) == 0 || deflect) {
        u64 context_flags = reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags;
        if ((context_flags & 0x4000000) != 0 ||
            ((context_flags & 0x8000000) != 0 && (object->context_variant_flags & 0x20000) != 0) || deflect ||
            (context_flags & 0x800000) != 0 ||
            ((context_flags & 0x1000000) != 0 && (object->context_variant_flags & 0x10000) != 0) ||
            CannotKill(object)) {
            BoltSys->debris(bolt, points, hit, &object->velocity, 1);
            if (LEGOCONTEXT_HOLD != -1 && object->character_context == LEGOCONTEXT_HOLD)
                NewBlockAction(object);
            GameAudio_PlaySfx(42, &points[hit], 0, 0);
            BoltNoSfxPc = 1;
            if ((BoltBlockContextPc == -1 || object->character_context != BoltBlockContextPc) && !deflect &&
                !(bolt->time >= 0.1f))
                goto finish;
            NUVEC direction;
            if (bolt->owner != NULL && (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 1) != 0)
                CalculateInterceptVector(&points[hit], &static_cast<GAMEOBJECT *>(bolt->owner)->collision_position,
                                         &static_cast<GAMEOBJECT *>(bolt->owner)->velocity, bolt->speed, &direction,
                                         NULL);
            else
                NuVecSub(&direction, &bolt->previous_position, &points[hit]);
            FindAnglesXY(&direction, NULL, NULL);
            if (((bolt->flags & 0x40) == 0 || deflect) && (object->flags_1fc & 0x80) != 0 &&
                ((BoltBlockContextPc != -1 && object->character_context == BoltBlockContextPc) || cheat_deflect) &&
                (object->character_data->model_flags & 8) != 0) {
                if (bolt->owner == NULL || (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 1) == 0 ||
                    ((static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc_word & 0x1001) == 0x1001 &&
                     static_cast<GAMEOBJECT *>(bolt->owner)->dead == 0))
                    goto reflected;
            }
            {
                ConfigBuffer.fields.find_angles_y_rotation +=
                    static_cast<i32>((qrand() * static_cast<f64>(1.0f / 65535.0f)) * 16384.0f - 8192.0f);
                FindAnglesXFullTempPc +=
                    static_cast<i32>((qrand() * static_cast<f64>(1.0f / 65535.0f)) * 16384.0f - 5461.0f);
                if (FindAnglesXFullTempPc < -8192)
                    FindAnglesXFullTempPc = -8192;
                else if (FindAnglesXFullTempPc > 8192)
                    FindAnglesXFullTempPc = 8192;
                temp_zrot = FindAnglesXFullTempPc;
            }
        reflected:
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            NUMTX matrix;
            NUANGVEC angles;
            angles.x = temp_zrot;
            angles.y = ConfigBuffer.fields.find_angles_y_rotation;
            NuMtxSetRotationXYVU0(&matrix, &angles);
            BOLT_PC *reflected_bolt = Bolt_Add(object, &points[hit], &matrix, bolt->type_id, 0);
            if (reflected_bolt != NULL)
                reflected_bolt->flags |= 0x10000000;
            goto finish;
        }
    }
    {
        CHARACTERDATA_PC *data = object->character_data;
        PLAYERCHARACTERCONFIG_PC *character = data->game_character;
        bool shield = object->origin_scale_override == 1.0f ||
                      ((character->flags_094 & 0x4000000) != 0 && VehicleArea != 0 && BonusArea != 0) ||
                      (object->current_hp == 0 && (data->model_flags & 0x2000) != 0) ||
                      ((data->model_flags & 0x20000000) != 0 && object->rider == NULL);
        if (!shield) {
            BoltSys->debris(bolt, points, hit, &object->velocity, 0);
            i32 damage = 0;
            if ((bolt->flags & 0x800000) == 0) {
                damage = BoltType_FindByID(bolt->type_id, WORLD)->field_3c;
                if (Player_HasDoubleBoltDamage_FromBolt(bolt))
                    damage += damage;
            }
            Bolt_PlayHitSfx(bolt);
            owner = static_cast<GAMEOBJECT *>(bolt->owner);
            if (owner != NULL && (owner->flags_1fc & 1) != 0 && (owner->flags_1fc & 0x80) == 0 &&
                (owner->flags_1f8 & 1) != 0 && ((owner->flags_1f8 ^ object->flags_1f8) & 1) == 0)
                goto owner_feedback;
            objhitobj_bolt = reinterpret_cast<BOLT *>(bolt);
            ObjHitObj(owner, object, damage, bolt->hit_flags, 0, 0);
            if ((bolt->flags & 0x800000) != 0 &&
                (reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags & 0x10008000) == 0 &&
                (object->character_data->model_flags & 0x10) != 0 && BoltDeactivatedActionPc != -1 &&
                object->character_model->model_data_b[BoltDeactivatedActionPc] != NULL) {
                DeactivatePlayer(object, DEACTIVATEDTIME, NULL);
                if (bolt->owner != NULL)
                    NewBuzz(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.1f, 0);
                goto owner_feedback;
            }
        owner_feedback:
            if (bolt->owner != NULL && static_cast<GAMEOBJECT *>(bolt->owner)->pad != NULL)
                NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.7f, 0);
        } else {
            if ((bolt->flags & 0x4000000) != 0) {
                BoltSys->debris(bolt, points, hit, &object->velocity, 0);
                ObjHitShield(static_cast<GAMEOBJECT *>(bolt->owner), object, type->field_3c, bolt);
            } else {
                BoltSys->debris(bolt, points, hit, &object->velocity, 1);
                ObjHitShield(static_cast<GAMEOBJECT *>(bolt->owner), object, 0, bolt);
                GameAudio_PlaySfx(41, &bolt->position, 0, 0);
                NewBuzz(object->pad->bound_pad, 0.1f, 0);
                if ((bolt->flags & 0x100000) == 0) {
                    ConfigBuffer.fields.find_angles_y_rotation =
                        NuAtan2D(bolt->position.x - object->collision_position.x,
                                 bolt->position.z - object->collision_position.z);
                    ConfigBuffer.fields.find_angles_y_rotation +=
                        static_cast<i32>((qrand() * static_cast<f64>(1.0f / 65535.0f)) * 16384.0f - 5461.0f);
                    temp_zrot = static_cast<i32>(-1820.0f - (qrand() * static_cast<f64>(1.0f / 65535.0f)) * 6371.0f);
                    NUMTX matrix;
                    NUANGVEC angles;
                    angles.x = temp_zrot;
                    angles.y = ConfigBuffer.fields.find_angles_y_rotation;
                    NuMtxSetRotationXYVU0(&matrix, &angles);
                    BoltNoSfxPc = 1;
                    BOLT_PC *reflected_bolt = Bolt_Add(object, &points[hit], &matrix, bolt->type_id, 0);
                    if (reflected_bolt != NULL)
                        reflected_bolt->flags |= 0x10000000;
                }
            }
        }
    }
finish:
    if (object->dead == 0 && (object->flags_1fc & 2) == 0) {
        f32 amount = object->character_data->game_character->walk_speed;
        if (object->scaled_radius > 1.0f)
            amount /= object->scaled_radius;
        object->velocity.x += bolt->direction.x * amount;
        object->velocity.z += bolt->direction.z * amount;
    }
    if ((object->flags_1fc & 0x80) != 0)
        GameCam_HitJudder();
    bolt->active = 0;
    if (bolt->type != NULL && bolt->type->end_callback != NULL)
        bolt->type->end_callback(bolt);
}

extern "C" void __cdecl Bolts_Draw(WORLDINFO *world) {
    NUVEC scale = {0.0f, 0.0f, 0.0f};
    for (BOLT_PC *bolt = Bolt; bolt != Bolt + 32; ++bolt) {
        if (bolt->active == 0)
            continue;
        BOLTTYPE_PC *type = bolt->type;
        f32 size = type->scale_callback != NULL ? type->scale_callback(bolt) : bolt->scale;
        NUMTX matrix = bolt->effect_orientation;
        if (bolt->time < type->field_24) {
            if ((type->flags_60 & 0x2000000) != 0)
                scale.y = size;
            else
                scale.y = (bolt->time / type->field_24) * size;
            scale.x = scale.z = (bolt->flags & 0x200) != 0 ? scale.y : size;
            NuMtxPreScale(&matrix, &scale);
        } else if (bolt->scale != 1.0f) {
            scale.x = scale.y = scale.z = size;
            NuMtxPreScale(&matrix, &scale);
        }
        NuMtxTranslate(&matrix, &bolt->position);
        NUSPECIALHANDLE *specials = reinterpret_cast<NUSPECIALHANDLE *>(type->specials_68);
        i32 first = 0, second = 0;
        if (NuSpecialExistsFn(&specials[0]))
            first = NuSpecialDrawAt(&specials[0], &matrix);
        if (NuSpecialExistsFn(&specials[1]))
            second = NuSpecialDrawAt(&specials[1], &matrix);
        if (static_cast<f64>(bolt->reflection_height) != 2000000.0) {
            NUMTX reflection;
            if (MatrixReflectionVU0_AXISY(&matrix, bolt->reflection_height, world->current_level->reflection_y,
                                          &reflection)) {
                if (NuSpecialExistsFn(&specials[2]))
                    NuSpecialDrawAt(&specials[2], &reflection);
                if (NuSpecialExistsFn(&specials[3]))
                    NuSpecialDrawAt(&specials[3], &reflection);
            }
        }
        if (static_cast<f64>(bolt->field_e4) != 2000000.0) {
            scale.x = scale.z = scale.y;
            NuMtxSetScale(&matrix, &scale);
            BoltDrawRotateZPrivate(&matrix, bolt->surface_z_rotation);
            NUMTX *shadow_matrix = BoltDrawRotateXPrivate(&matrix, bolt->surface_x_rotation);
            matrix.m30 = bolt->position.x;
            matrix.m31 = static_cast<f32>(static_cast<f64>(bolt->field_e4) + static_cast<f64>(0.005f));
            matrix.m32 = bolt->position.z;
            NuSpecialDrawAt(&specials[4], shadow_matrix);
        }
        if (first != 0 || second != 0)
            bolt->field_102 = 1;
        else if (Paused == 0 && bolt->field_102 != 0 && (bolt->flags & 0x20) == 0)
            bolt->active = 0;
    }
}

extern "C" {
    void __cdecl BoltTypes_Configure(WORLDINFO *world, char *config) {
        NUFPAR_PC *parser;
        BOLT_CONFIG_PC type;
        i32 reading_type = 0;
        memset(world->bolt_types, 0, sizeof(world->bolt_types));
        parser = NuFParCreateMem("bolttypes", config, 0xffff);
        if (parser == NULL) {
            return;
        }

        NuFParPushCom(parser, BoltType_ConfigKeywords);
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) == 0) {
                continue;
            }

            if (reading_type != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, ("bolttype_end")) == 0) {
                    if (type.name[0] != '\0') {
                        BoltTypeStoreConfigPrivatePc(&type, world);
                    }
                    reading_type = 0;
                } else {
                    NuFParInterpretWord(parser);
                }
                continue;
            }

            if (NuStrICmp(parser->word_buffer_ptr, ("bolttype_start")) == 0) {
                BT_worldinfo = world;
                BT_bolttype = &type;
                BT_gdeb_moving_count = 0;
                BT_scene = things_scene;
                memset(&type, 0, sizeof(type));
                memcpy(&type, &GlobalBoltType_Default, sizeof(type));
                type.name[0] = '\0';
                type.moving_debris[0] = -1;
                type.moving_debris[1] = -1;
                reading_type = 1;
            }
        }
        NuFParDestroy(parser);
    }
}

extern "C" i32 __cdecl Bolt_HitGameObject(BOLT_PC *bolt, GAMEOBJECT *object, NUVEC *points, NUVEC *minimum,
                                          NUVEC *maximum, f32 radius, u8 *hit_flags) {
    NUVEC center = object->collision_position;
    i32 hit;
    for (hit = 2; hit >= 0; --hit) {
        NUVEC point;
        if ((bolt->flags & 0x200) != 0 && hit != 1)
            continue;
        point = points[hit];
        if ((bolt->flags & 0x8000000) != 0 && bolt->time >= bolt->type->field_24) {
            center.y = 0.0f;
            point.y = 0.0f;
        }
        if (SphereSphereOverlapScaleY(&center, object->bounds_radius, object->bounds_half_height, &point, radius,
                                      radius)) {
            u16 bolt_index = static_cast<u16>(bolt - Bolt);
            u16 object_index = static_cast<u16>(object - Obj);
            NETMESSAGE_PC message(0x20);
            message.Write16(bolt_index);
            message.Write16(object_index);
            message.Write(&points[0], 4, 3);
            message.Write(&points[1], 4, 3);
            message.Write(&points[2], 4, 3);
            message.Write32(hit);
            message.Write32(netclient);
            Bolt_HitGameObjectRC(message);
            message.Release();
            return 1;
        }
    }
    return 0;
}

extern "C" i32 __cdecl Bolt_HitGameObjects(BOLT_PC *bolt, NUVEC *points, NUVEC *minimum, NUVEC *maximum, f32 radius,
                                           u8 *hit_flags) {
    GAMEOBJECT *object = Obj;
    i32 index;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        GAMEOBJECT *owner;
        u32 flags;
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
            continue;
        owner = static_cast<GAMEOBJECT *>(bolt->owner);
        if (object == owner || (object->flags_e20 & 0x20) != 0)
            continue;
        if (owner != NULL && owner->rider != NULL && object == owner->rider)
            continue;
        flags = bolt->flags;
        if ((flags & 0x80) != 0 && object->field_280 == 0xff &&
            (object->rider == NULL || object->rider->field_280 == 0xff))
            continue;
        if ((reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags & 0x8080) != 0 ||
            (object->flags_e24 & 0x8000) != 0)
            continue;
        if (VehicleArea != 0 && BonusArea == 0 && owner != NULL && owner->field_280 != 0xff &&
            object->field_280 != 0xff)
            continue;
        if ((object->character_data->game_character->flags_090 & 0x8000) != 0)
            continue;
        if (minimum->x > object->collision_max.x || object->collision_min.x > maximum->x ||
            minimum->z > object->collision_max.z || object->collision_min.z > maximum->z)
            continue;
        if ((flags & 0x8000000) == 0 && (minimum->y > object->collision_max.y || object->collision_min.y > maximum->y))
            continue;
        if (Bolt_HitGameObject(bolt, object, points, minimum, maximum, radius, hit_flags))
            return 1;
    }
    return 0;
}

extern "C" void __cdecl Bolt_Init(void *storage, NETMESSAGE_PC &message) {
    BOLT_PC *bolt = static_cast<BOLT_PC *>(storage);
    i16 owner_index;
    NUVEC position;
    NUMTX orientation;
    i32 type_id, hit_flags;
    message.Read16(owner_index);
    message.Read(&position, 4, 3);
    message.Read(&orientation, 4, 16);
    message.Read32(type_id);
    message.Read32(hit_flags);
    GAMEOBJECT *owner = owner_index >= 0 ? &Obj[owner_index] : NULL;
    /* BoltType_FindByID(type_id, WORLD), written in place as retail inlines it. */
    BOLTTYPE_PC *type;
    if (type_id >= 0 && type_id < BoltSys->count)
        type = &BoltSys->types[type_id];
    else if (WORLD != NULL && type_id >= BoltSys->count && type_id < BoltSys->count + 8)
        type = &WORLD->bolt_types[type_id - BoltSys->count];
    else
        type = NULL;
    NUVEC momentum = APIObjectZeroMomentumPC;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 no_sfx = BoltNoSfxPc;
    BoltNoSfxPc = 0;
    i32 no_momentum = BoltNoObjectMomentumPc;
    BoltNoObjectMomentumPc = 0;
    i32 shoot_id = -1;
    BoltShootIdScratch = -1;
    if (type == NULL || !NuSpecialExistsFn(reinterpret_cast<NUSPECIALHANDLE *>(type->specials_68)))
        return;
    if (bolt->active != 0) {
        NUVEC old_points[3];
        Bolt_GeneratePointsPrivate(bolt, old_points);
        BoltSys->debris(bolt, old_points, -1, NULL, 0);
    }
    bolt->type = type;
    bolt->type_id = type_id;
    if (BOLT_OVERRIDE_PLAYERBOLTSPEED != 0.0f && BOLT_OVERRIDE_PLAYERBOLTDURATION != 0.0f) {
        bolt->speed = BOLT_OVERRIDE_PLAYERBOLTSPEED;
        bolt->lifetime = BOLT_OVERRIDE_PLAYERBOLTDURATION;
    } else {
        bolt->speed = type->field_10;
        bolt->lifetime = type->field_14;
    }
    bolt->acceleration_y = type->field_18;
    bolt->radius = type->field_1c;
    bolt->scale = type->field_20;
    bolt->hit_debris = static_cast<i16>(static_cast<u32>(type->field_2c) >> 16);
    bolt->hit_part_debris = type->field_38;
    bolt->index = bolt - Bolt;
    if (hit_flags == 0)
        bolt->hit_flags = ObjHitObj_Flags(owner);
    else
        bolt->hit_flags = hit_flags;
    bolt->hit_flags |= 0x80;
    u32 flags = type->flags_60;
    if (owner != NULL) {
        if ((owner->flags_1fc & 0x80) != 0) {
            flags |= 0x28;
            if (owner->field_280 == 0)
                flags |= 1;
            else if (owner->field_280 == 1)
                flags |= 2;
        }
        bool add_speed = InitBolt_AddMomentumType != NULL && InitBolt_AddMomentumType(bolt, owner, &momentum) != 0;
        if (!add_speed) {
            u32 model_flags = owner->character_data->model_flags;
            add_speed =
                (model_flags & 0x4000000) != 0 || ((owner->flags_1fc & 0x80) != 0 && (model_flags & 0x2000) != 0);
        }
        if (add_speed && !no_momentum) {
            f32 speed = owner->facing_direction.x * owner->velocity.x + owner->facing_direction.z * owner->velocity.z;
            if (speed < 0.0f)
                speed = 0.0f;
            bolt->speed += speed;
        }
        if (owner->rider != NULL)
            flags |= 0x40000;
        if (BoltInitSfxFn != NULL)
            shoot_id = BoltInitSfxFn(owner);
        if (shoot_id == -1)
            shoot_id = owner->character_data->game_character->sfx_shoot;
    }
    if (shoot_id == -1)
        shoot_id = type->field_64;
    BoltShootIdScratch = shoot_id;
    if (!no_sfx && BoltShootIdScratch != -1)
        GameAudio_PlaySfxById(BoltShootIdScratch, &position, 0, 0);
    BoltShootPositionScratch = position;
    bolt->time = 0.0f;
    bolt->active = 1;
    bolt->position = position;
    bolt->previous_position = position;
    bolt->field_102 = 0;
    bolt->hit_platform = -1;
    bolt->owner = owner;
    bolt->flags = flags;
    bolt->field_e4 = 2000000.0f;
    bolt->reflection_height = 2000000.0f;
    bolt->field_105 = -1;
    if (owner != NULL) {
        if (owner->floor_height != 2000000.0f) {
            bolt->field_e4 = owner->floor_height;
            bolt->surface_x_rotation = owner->field_1052;
            bolt->surface_z_rotation = owner->field_1054;
        }
        if (owner->reflection_height != 2000000.0f && owner->reflection_enabled == 2)
            bolt->reflection_height = owner->reflection_height;
    }
    f32 collision_radius = bolt->radius;
    if ((flags & 0x200) == 0)
        collision_radius = (f32)((f64)collision_radius * (f64)(1.0f / 3.0f));
    bolt->radius *= bolt->scale;
    bolt->collision_radius = collision_radius * bolt->scale;
    bolt->velocity.x = 0.0f;
    bolt->velocity.y = 0.0f;
    bolt->velocity.z = bolt->speed;
    NuMtxSetRotationX(&bolt->effect_orientation, 0x4000);
    bolt->orientation = orientation;
    NuVecMtxRotate(&bolt->velocity, &bolt->velocity, &bolt->orientation);
    NuVecAdd(&bolt->velocity, &bolt->velocity, &momentum);
    NuVecNorm(&bolt->direction, &bolt->velocity);
    if ((bolt->flags & 0x1000) != 0) {
        Bolt_OrientationPrivate(bolt);
    } else
        NuMtxMulR(&bolt->effect_orientation, &bolt->effect_orientation, &bolt->orientation);
    NUVEC points[3];
    points[1] = bolt->position;
    if ((bolt->flags & 0x200) == 0) {
        NUVEC offset = {0.0f, 0.0f, bolt->collision_radius + bolt->collision_radius};
        NuVecMtxRotate(&offset, &offset, &bolt->orientation);
        NuVecSub(&points[0], &bolt->position, &offset);
        NuVecAdd(&points[2], &bolt->position, &offset);
    }
    bolt->bounds_min.x = bolt->position.x - bolt->radius;
    bolt->bounds_min.y = bolt->position.y - bolt->radius;
    bolt->bounds_min.z = bolt->position.z - bolt->radius;
    bolt->bounds_max.x = bolt->position.x + bolt->radius;
    bolt->bounds_max.y = bolt->position.y + bolt->radius;
    bolt->bounds_max.z = bolt->position.z + bolt->radius;
    f32 initial_distance = bolt->speed * bolt->lifetime;
    f32 initial_speed = bolt->speed;
    NUVEC movement;
    NuVecScale(&movement, &bolt->direction, (f32)((f64)initial_speed * (f64)0.2f));
    if (bolt->acceleration_y != 0.0f)
        movement.y =
            (f32)((f64)bolt->velocity.y * (f64)0.2f + (f64)bolt->acceleration_y * .5 * (f64)0.04000000283122063f);
    f32 back_distance = 0.01f;
    if (owner != NULL) {
        back_distance = owner->collision_radius;
        if ((owner->character_data->model_flags & 0x2000) == 0 &&
            (owner->character_data->game_character->flags_090 & 0x80) == 0)
            back_distance *= 3.0f;
    }
    NUVEC back, start;
    NuVecScale(&back, &bolt->direction, back_distance);
    NuVecSub(&start, &bolt->position, &back);
    NuVecAdd(&movement, &movement, &back);
    bolt->ray_radius = bolt->collision_radius;
    bolt->ray_time = 0.2f;
    if ((bolt->flags & 4) == 0 && Bolt_RayCastPrivate(bolt, &start, &movement)) {
        f32 offset_time = (back_distance / initial_distance) * bolt->lifetime;
        bolt->lifetime = bolt->ray_time + offset_time;
        f32 hit_time_product = NewRayCastGetTOFI() * bolt->lifetime;
        bolt->lifetime = hit_time_product - offset_time;
        if (bolt->lifetime <= 0.0f) {
            NuVecAdd(&start, &start, &movement);
            AddGameDebris(world->game_debris_sys, bolt->hit_debris, &start);
            AddGameDebris(world->game_debris_sys, bolt->hit_debris, &bolt->position);
            bolt->active = 0;
            if (bolt->type && bolt->type->end_callback)
                bolt->type->end_callback(bolt);
            if ((bolt->flags & 0x10000) == 0 && !Bolt_HitGameObjects(bolt, points, &bolt->bounds_min, &bolt->bounds_max,
                                                                     bolt->collision_radius, NULL)) {
                bool hit = false;
                GAMEOBJECT *current_owner = static_cast<GAMEOBJECT *>(bolt->owner);
                if ((bolt->flags & 0x13) != 0 ||
                    (current_owner != NULL &&
                     ((current_owner->portal_flags & 0x10000000) != 0 || current_owner->field_f00 == 5))) {
                    hit = bolt->hit_platform != -1 && Bolt_HitPlatPrivate(bolt, NULL);
                    if (!hit)
                        hit = GizmoSys_BoltHit(world->gizmo_sys, world, bolt, points, &bolt->bounds_min,
                                               &bolt->bounds_max, bolt->collision_radius, NULL) != 0;
                    if (!hit)
                        hit = Bolt_HitBlowupPrivate(bolt, points, &bolt->bounds_min, &bolt->bounds_max,
                                                    bolt->collision_radius, NULL) != 0;
                    if (!hit)
                        hit = Bolt_HitParts(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius,
                                            Bolt_HitPartMode(bolt)) != NULL;
                }
                if (!hit)
                    Bolt_PlayHitSfx(bolt);
                if (BoltSys->stop_targeting != NULL && bolt->owner != NULL &&
                    (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0)
                    BoltSys->stop_targeting(static_cast<GAMEOBJECT *>(bolt->owner), &bolt->position);
            }
        }
    }
    if (type->init_callback != NULL)
        type->init_callback(bolt);
}

extern "C" BOLT_PC *__cdecl Bolt_Add(GAMEOBJECT *owner, NUVEC *position, NUMTX *matrix, i32 type_id, i32 flags) {
    BOLTTYPE_PC *type = BoltType_FindByID(type_id, WORLD);
    BOLT_PC *bolt = NULL;
    if (type != NULL && NuSpecialExistsFn(reinterpret_cast<NUSPECIALHANDLE *>(type->specials_68))) {
        bolt = Bolt_Alloc();
        if (bolt != NULL) {
            u16 owner_index = owner ? (u16)(owner - Obj) : 0xffff;
            NETMESSAGE_PC message(32);
            message.Write16(owner_index);
            message.Write(position, 4, 3);
            message.Write(matrix, 4, 16);
            message.Write32(type_id);
            message.Write32(flags);
            Bolt_Init(bolt, message);
            message.Release();
        }
    }
    return bolt;
}

extern "C" void __cdecl Bolts_Update(WORLDINFO *world) {
    NUVEC normal = v010;
    u8 processed[32] = {};
    i32 index = 0;
    for (BOLT_PC *bolt = Bolt; bolt != Bolt + 32; ++bolt, ++index) {
        if (!bolt->active || processed[index])
            continue;
        if (bolt->owner != NULL) {
            u32 owner_flags = static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc_word;
            if ((owner_flags & 1) == 0 || (owner_flags & 0x1000) == 0)
                bolt->owner = NULL;
        }
        BOLTTYPE_PC *type = bolt->type;
        if ((bolt->flags & 4) == 0) {
            bolt->ray_time -= FRAMETIME;
            if (bolt->ray_time <= 0.0f) {
                NUVEC movement, end;
                NuVecScale(&movement, &bolt->direction, (f32)((f64)0.2f * bolt->speed));
                bolt->ray_time = 0.2f;
                if (bolt->acceleration_y != 0.0f)
                    movement.y = (f32)((f64)0.2f * bolt->velocity.y +
                                       (f64)bolt->acceleration_y * .5 * BoltRayAccelerationTimeSquared);
                NuVecAdd(&end, &bolt->position, &movement);
                NuVecSub(&movement, &end, &bolt->ray_end);
                if (Bolt_RayCastPrivate(bolt, &bolt->ray_end, &movement)) {
                    u8 info[4];
                    NewRayCastGetImpactNormal(&normal);
                    NewTerrHitInfo(info);
                    bolt->lifetime = NewRayCastGetTOFI() * bolt->ray_time + bolt->time;
                    if (bolt->type->ricochet_callback != NULL)
                        bolt->type->ricochet_callback(bolt, &normal);
                }
                bolt->reflection_height = 2000000.0f;
                f32 height = GameShadow(NULL, &bolt->position, 5.0f, -1);
                if (height != 2000000.0f) {
                    if (NuSpecialExistsFn(&reinterpret_cast<BOLT_UPDATE_TYPE_PC *>(type)->shadow_special)) {
                        bolt->field_e4 = height;
                        FindAnglesZX(&ShadNorm, &bolt->surface_x_rotation, &bolt->surface_z_rotation);
                    }
                    u32 surface = ShadowInfo();
                    if (surface <= 31 && (TerSurface[surface].flags & 2) != 0)
                        bolt->reflection_height = height;
                    else if (static_cast<f32>(NewShadowOnPlatform()) != -1.0f)
                        bolt->reflection_height = FindReflectionNoPlatforms(&bolt->position);
                }
            }
        }
        bolt->time += FRAMETIME;
        if (type->update_callback != NULL)
            type->update_callback(bolt);
        if (bolt->acceleration_y != 0.0f) {
            bolt->velocity.y += FRAMETIME * bolt->acceleration_y;
            bolt->speed = NuVecMag(&bolt->velocity);
            NuVecNorm(&bolt->direction, &bolt->velocity);
            if ((bolt->flags & 0x1000) != 0) {
                Bolt_OrientationPrivate(bolt);
            }
        }
        bolt->position.x += FRAMETIME * bolt->velocity.x;
        bolt->position.y += FRAMETIME * bolt->velocity.y;
        bolt->position.z += FRAMETIME * bolt->velocity.z;
        NUVEC points[3];
        points[1] = bolt->position;
        if ((bolt->flags & 0x200) == 0) {
            NUVEC offset = {0.0f, 0.0f, bolt->collision_radius + bolt->collision_radius};
            NuVecMtxRotate(&offset, &offset, &bolt->orientation);
            NuVecSub(&points[0], &bolt->position, &offset);
            NuVecAdd(&points[2], &bolt->position, &offset);
        }
        bolt->bounds_min.x = bolt->position.x - bolt->radius;
        bolt->bounds_min.y = bolt->position.y - bolt->radius;
        bolt->bounds_min.z = bolt->position.z - bolt->radius;
        bolt->bounds_max.x = bolt->position.x + bolt->radius;
        bolt->bounds_max.y = bolt->position.y + bolt->radius;
        bolt->bounds_max.z = bolt->position.z + bolt->radius;
        if (bolt->time >= bolt->lifetime) {
            i32 surface_hit = 0;
            if (bolt->field_104 != -1) {
                NUVEC reflection;
                Bolt_Reflect(&bolt->hit_normal, &bolt->velocity, &reflection);
                NuVecScale(&reflection, &reflection, 0.2f);
                surface_hit = 1;
                if (bolt->hit_platform != -1 && bolt->field_104 != -1 &&
                    (TerSurface[bolt->field_104].flags & 0x1000) != 0)
                    BoltSys->debris(bolt, points, -1, &reflection, surface_hit);
                else
                    BoltSys->debris(bolt, points, -1, &reflection, 0);
            } else
                BoltSys->debris(bolt, points, -1, NULL, 0);
            Bolt_End(bolt, 1);
            if ((bolt->flags & 0x10000) != 0)
                continue;
            if (Bolt_HitGameObjects(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius, NULL) !=
                0)
                continue;
            if ((bolt->flags & 0x13) != 0 ||
                (bolt->owner != NULL && ((static_cast<GAMEOBJECT *>(bolt->owner)->portal_flags & 0x10000000) != 0 ||
                                         static_cast<u8>(static_cast<GAMEOBJECT *>(bolt->owner)->field_f00) == 5))) {
                if (bolt->hit_platform != -1) {
                    if (Bolt_HitPlatFn == NULL || !Bolt_HitPlatFn(bolt))
                        Bolt_HitPlatPrivate(bolt, processed);
                    goto stop_targeting;
                }
                if (GizmoSys_BoltHit(world->gizmo_sys, world, bolt, points, &bolt->bounds_min, &bolt->bounds_max,
                                     bolt->collision_radius, NULL))
                    goto stop_targeting;
                if (Bolt_HitBlowupPrivate(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius,
                                          NULL))
                    goto stop_targeting;
                if (Bolt_HitParts(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius,
                                  Bolt_HitPartMode(bolt)))
                    goto stop_targeting;
            } else if (surface_hit && bolt->hit_platform != -1 && bolt->field_104 != -1 &&
                       (TerSurface[bolt->field_104].flags & 0x1000) != 0) {
                GameAudio_PlaySfx(41, &bolt->position, 0, 0);
                Bolt_AddDeflectedBolt(bolt, &bolt->direction, &bolt->hit_normal, processed);
            }
            if (surface_hit && bolt->field_104 >= 0 && bolt->field_104 <= 31 &&
                (TerSurface[bolt->field_104].flags & 0x800) != 0 && (bolt->flags & 0x8000) == 0) {
                GameAudio_PlaySfx(41, &bolt->position, 0, 0);
                Bolt_AddDeflectedBolt(bolt, &bolt->velocity, &bolt->hit_normal, processed);
            } else if ((bolt->flags & 0x80000) == 0)
                Bolt_PlayHitSfx(bolt);
        stop_targeting:
            if (BoltSys->stop_targeting != NULL && bolt->owner != NULL &&
                (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0)
                BoltSys->stop_targeting(static_cast<GAMEOBJECT *>(bolt->owner), &bolt->position);
        } else {
            if (Bolt_HitCustomFn != NULL)
                Bolt_HitCustomFn(bolt, points);
            for (i32 i = 0; i < 2; ++i) {
                if (type->moving_debris[i] != -1 && type->moving_debris_counts[i] != 0)
                    AddVariableShotDebrisEffectTimed5(world->game_debris_sys->entries[type->moving_debris[i]].effect,
                                                      &bolt->position, NULL, &bolt->velocity,
                                                      type->moving_debris_counts[i], FRAMETIME, NULL, NULL, 20000, 0);
            }
            if ((bolt->flags & 0x10000) != 0)
                continue;
            GAMEOBJECT *object = Obj;
            for (i32 i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
                if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
                    continue;
                GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
                if (object == owner || (object->flags_e20 & 0x20) != 0)
                    continue;
                if (owner != NULL && owner->rider != NULL && object == owner->rider)
                    continue;
                u32 flags = bolt->flags;
                if ((flags & 0x80) != 0 && object->field_280 == 0xff &&
                    (object->rider == NULL || object->rider->field_280 == 0xff))
                    continue;
                if ((reinterpret_cast<BOLT_CONTEXT_INFO_PC *>(CInfo)[object->character_context].flags & 0x8080) != 0 ||
                    (object->flags_e24 & 0x8000) != 0)
                    continue;
                if (VehicleArea != 0 && BonusArea == 0 && owner != NULL && owner->field_280 != 0xff &&
                    object->field_280 != 0xff)
                    continue;
                if ((object->character_data->game_character->flags_090 & 0x8000) != 0)
                    continue;
                if (bolt->bounds_min.x > object->collision_max.x || object->collision_min.x > bolt->bounds_max.x ||
                    bolt->bounds_min.z > object->collision_max.z || object->collision_min.z > bolt->bounds_max.z)
                    continue;
                if ((flags & 0x8000000) == 0 &&
                    (bolt->bounds_min.y > object->collision_max.y || object->collision_min.y > bolt->bounds_max.y))
                    continue;
                if (Bolt_HitGameObject(bolt, object, points, &bolt->bounds_min, &bolt->bounds_max,
                                       bolt->collision_radius, processed))
                    goto next_bolt;
            }
            if ((bolt->flags & 0x13) != 0 ||
                (bolt->owner != NULL && ((static_cast<GAMEOBJECT *>(bolt->owner)->portal_flags & 0x10000000) != 0 ||
                                         static_cast<u8>(static_cast<GAMEOBJECT *>(bolt->owner)->field_f00) == 5))) {
                if (GizmoSys_BoltHit(world->gizmo_sys, world, bolt, points, &bolt->bounds_min, &bolt->bounds_max,
                                     bolt->collision_radius, processed))
                    continue;
                if (Bolt_HitBlowupPrivate(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius,
                                          processed))
                    continue;
                Bolt_HitParts(bolt, points, &bolt->bounds_min, &bolt->bounds_max, bolt->collision_radius,
                              Bolt_HitPartMode(bolt));
            }
        }
    next_bolt:;
    }
}

extern "C" {
    void __cdecl Bolt_Shoot(GAMEOBJECT *object, i32 type_id, i32 fire_flags) {
        BOLTTYPE_PC *type = BoltType_FindByID(type_id, WORLD);
        u32 flags = type->flags_60;
        f32 speed = type->field_10;
        f32 duration = type->field_14;
        NUVEC aimed_position, adjusted_position, delta, target_velocity, origin, direction;
        NUVEC *target_position;
        NUVEC *intercept_position;
        NUVEC *velocity;
        f32 target_distance;
        i16 sounds[8];
        NUVEC sound_positions[8];
        i32 sound_count = 0;
        i32 bolts[5], bolt_count = 0;
        i32 used_locator = 0;
        i32 index;
        u16 heading;

        if ((object->flags_1fc & 0x80) != 0) {
            flags |= 0x20;
            if (object->field_280 == 0)
                flags |= 1;
            else if (object->field_280 == 1)
                flags |= 2;
        }
        if ((object->flags_ef0 & 8) != 0)
            flags |= 0x40;
        aimed_position = object->attack_target_position;
        BoltSys->shoot_origin(object, &origin);
        heading = BoltSys->shoot_direction(object, &direction);
        if (object->character_context != -1 && object->field_7aa != -1) {
            if ((u16)object->field_7aa == BoltShootClassId0)
                heading = (u16)(heading + 0x4000);
            else if ((u16)object->field_7aa == BoltShootClassId1)
                heading = (u16)(heading - 0x4000);
            else if ((u16)object->field_7aa == BoltShootClassId2)
                heading = (u16)(heading + 0x8000);
        }
        if ((object->flags_e20 & 0x800) != 0) {
            if ((object->portal_flags & 0x8000) == 0 && object->field_ea0_target == NULL && object->field_280 != 0xff &&
                (object->field_c8c != 0.0f || object->field_c90 != 0.0f || object->field_c94 != 0.0f)) {
                target_distance = duration * speed;
                NuVecSub(&adjusted_position, &aimed_position, &origin);
                NuVecNorm(&adjusted_position, &adjusted_position);
                NuVecScale(&adjusted_position, &adjusted_position, target_distance);
                NuVecAdd(&aimed_position, &origin, &adjusted_position);
            } else {
                target_distance = NuVecDist(&origin, &aimed_position, NULL);
            }
        } else {
            target_distance = 0.0f;
        }
        target_position = NULL;
        intercept_position = NULL;
        velocity = NULL;
        if ((object->portal_flags & 0x8000) != 0) {
            target_position = intercept_position = &object->field_e4c;
            if ((object->portal_flags & 0x10000) == 0 && (object->portal_flags & 0x20000) != 0) {
                NuVecSub(&target_velocity, &object->field_e4c, &object->field_e58);
                NuVecScale(&target_velocity, &target_velocity, 1.0f / FRAMETIME);
                velocity = &target_velocity;
            }
        } else if (object->field_ea0_target != NULL) {
            GAMEOBJECT *target = object->field_ea0_target;
            velocity = &target->velocity;
            target_position = intercept_position = &target->collision_position;
            if ((object->character_data->model_flags & 0x2000) == 0 && (object->flags_1f8 & 1) != 0) {
                adjusted_position = target->collision_position;
                adjusted_position.y += Bolt_ObjTargetPosYAdjust(target);
                target_position = &adjusted_position;
            }
        } else if ((object->flags_e20 & 0x800) != 0) {
            target_position = intercept_position = &aimed_position;
        }
        for (index = 0; index < 5; ++index) {
            NUVEC *shot_position;
            NUMTX matrix;
            BOLT_PC *bolt;
            i32 skip_index = -1;
            if (fire_flags == 1)
                skip_index = 1;
            else if (fire_flags == 2)
                skip_index = 0;
            if (index == skip_index)
                continue;
            if (BoltSys->alternate_fire != NULL && BoltSys->alternate_fire(object, index) == 0)
                continue;
            if (index == 4) {
                if (used_locator != 0)
                    break;
                shot_position = &origin;
            } else {
                PLAYERCHARACTERCONFIG_PC *data;
                i32 locator;
                if (object->model_draw_result == 0)
                    continue;
                data = object->character_data->game_character;
                locator = data->weapon_shoot_joints[index];
                if (locator == -1 || object->character_model->points_of_interest[locator] == NULL)
                    locator = data->weapon_joints[index];
                if (locator == -1 || object->character_model->points_of_interest[locator] == NULL)
                    continue;
                used_locator = 1;
                shot_position = NUMTX_GET_ROW_VEC(&object->locator_matrices[locator], 3);
                if (shot_position == NULL)
                    continue;
            }
            if ((flags & 0x600000) != 0) {
                u16 pitch;
                NUANGVEC angles;
                if ((flags & 0x80000000) != 0 && (object->flags_1fc & 0x80) == 0 && object->field_ea0_target != NULL) {
                    f32 distance =
                        NuVecXZDist(&object->field_ea0_target->collision_position, &object->collision_position, NULL);
                    f32 degrees;
                    if (distance > 2.0f)
                        distance = 2.0f;
                    degrees = (flags & 0x200000) != 0 ? -10.0f : -20.0f;
                    degrees = (f32)((f64)degrees * (f64)distance * 0.5);
                    pitch = (u16)(i32)(((f64)degrees * 65536.0) / 360.0);
                } else {
                    pitch = (flags & 0x200000) != 0 ? 0xf8e4 : 0xf1c8;
                }
                if (target_position != NULL)
                    heading = NuAtan2D(target_position->x - shot_position->x, target_position->z - shot_position->z);
                angles.x = pitch;
                angles.y = heading;
                NuMtxSetRotationXYVU0(&matrix, &angles);
            } else if (target_position != NULL) {
                NUANGVEC angles;
                if (velocity != NULL && (object->portal_flags & 0x20000) != 0)
                    CalculateInterceptVector(shot_position, intercept_position, velocity, speed, &delta, NULL);
                else
                    NuVecSub(&delta, target_position, shot_position);
                FindAnglesXY(&delta, NULL, NULL);
                angles.x = temp_zrot;
                angles.y = ConfigBuffer.fields.find_angles_y_rotation;
                NuMtxSetRotationXYVU0(&matrix, &angles);
            } else if (object->movement_mode == 4) {
                NUANGVEC angles;
                FindAnglesXY(&direction, NULL, NULL);
                angles.x = temp_zrot;
                angles.y = ConfigBuffer.fields.find_angles_y_rotation;
                NuMtxSetRotationXYVU0(&matrix, &angles);
            } else {
                NuMtxSetRotationYInline(&matrix, heading);
            }
            BoltNoSfxPc = 1;
            bolt = Bolt_Add(object, shot_position, &matrix, type_id, 0);
            if (bolt != NULL)
                bolts[bolt_count++] = bolt->index;
            if (BoltShootIdScratch != -1 && sound_count < 8) {
                sounds[sound_count] = (i16)BoltShootIdScratch;
                sound_positions[sound_count++] = BoltShootPositionScratch;
            }
            if ((type->flags_60 & 0x20000000) != 0)
                object->field_d54 = BoltShootParameter;
        }
        object->flags_e20 |= 0x40000;
        if (sound_count > 0) {
            i32 sound = sound_count > 1 ? qrand() / (0xffff / sound_count + 1) : 0;
            GameAudio_PlaySfxById(sounds[sound], &sound_positions[sound], 0, 0);
        }
        if ((object->flags_e20 & 0x800) != 0 && bolt_count > 1) {
            for (index = 0; index < bolt_count; ++index) {
                BOLT_PC *bolt = &Bolt[bolts[index]];
                f32 distance = bolt->speed * bolt->lifetime;
                f32 factor = target_distance == 0.0f || distance == 0.0f ? 0.0f : target_distance / distance;
                f32 lifetime = factor * type->field_14;
                if (bolt->lifetime > lifetime)
                    bolt->lifetime = lifetime;
            }
        }
    }
}
