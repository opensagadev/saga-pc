#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct BOLTSYS_s;
struct WORLDINFO_s;
struct bolt_s;

struct PART;
struct gameobject_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 BOLT_OVERRIDE_PLAYERBOLTSPEED;
    extern f32 BOLT_OVERRIDE_PLAYERBOLTDURATION;
    void PCByteCopy(void *destination, const void *source, u32 count);
    extern i32 (*Bolt_HitCustomFn)(void *bolt, void *points);
    extern i32 i_bolt;
    void Bolt_Reflect(NUVEC *, NUVEC *, NUVEC *);
    extern f32 BoltShootParameter;
    extern i32 BoltShootIdScratch;
    extern NUVEC BoltShootPositionScratch;
    extern i32 BoltNoSfx;
    extern i32 BoltNoObjectMomentum;
    extern i32 addbolt_nosfx;
#ifdef __cplusplus
}
#endif

typedef struct bolt_s {         // size 0x138
    struct BOLTTYPE_s *type;    // 0x000
    struct gameobject_s *owner; // 0x004
    NUMTX orientation;          // 0x008
    NUMTX effect_orientation;   // 0x048
    NUVEC position;             // 0x088
    NUVEC previous_position;    // 0x094
    NUVEC velocity;             // 0x0a0
    NUVEC direction;            // 0x0ac
    NUVEC ray_end;              // 0x0b8
    f32 scale;                  // 0x0c4
    f32 time;                   // 0x0c8
    f32 speed;                  // 0x0cc
    f32 lifetime;               // 0x0d0
    f32 radius;                 // 0x0d4
    f32 collision_radius;       // 0x0d8
    f32 ray_time;               // 0x0dc
    f32 ray_radius;             // 0x0e0
    f32 unknown_e4;             // 0x0e4
    f32 reflection_height;      // 0x0e8
    f32 acceleration_y;         // 0x0ec
    u32 flags;                  // 0x0f0
    u16 surface_x_rotation;     // 0x0f4
    u16 surface_z_rotation;     // 0x0f6
    u16 hit_flags;              // 0x0f8

    union { // size 0x2
        i16 hit_platform;
        u16 unknown_fa;
    }; // 0x0fa

    i16 hit_debris;              // 0x0fc
    i16 hit_part_debris;         // 0x0fe
    u8 active;                   // 0x100
    u8 type_id;                  // 0x101
    u8 unknown_102;              // 0x102
    u8 index;                    // 0x103
    i8 unknown_104;              // 0x104
    i8 unknown_105;              // 0x105
    u16 unknown_106;             // 0x106
    NUVEC bounds_min;            // 0x108
    NUVEC bounds_max;            // 0x114
    NUVEC hit_normal;            // 0x120
    NUVEC dogfight_hit_position; // 0x12c
} BOLT;

#ifdef __cplusplus
extern "C" {
#endif
    /* SpecialMove_KillVictim stays static with private ESI ABI in original caller TU; no public declaration. */
    extern struct bolt_s *objhitobj_bolt;
    extern i32 (*Bolt_HitPlatFn)(struct bolt_s *bolt);
    void Bolt_Debris_Default(struct bolt_s *, NUVEC *, i32, NUVEC *, i32);
    void BoltSys_Init(struct BOLTSYS_s *);
    extern BOLT Bolt[32];
    void Bolts_Update(struct WORLDINFO_s *);
    void Bolts_Draw(struct WORLDINFO_s *);
    BOLT *Bolt_Alloc(void);
    void Bolt_PlayHitSfx(BOLT *);
    i32 Bolt_HitPartMode(BOLT *);
    void Bolt_End(BOLT *, i32);
    void Bolt_AddDeflectedBolt(BOLT *, NUVEC *, NUVEC *, u8 *);
    i32 Bolt_HitGameObjects(BOLT *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    BOLT *Bolt_Find(i32, NUVEC *, struct gameobject_s *);
    extern i32 (*InitBolt_AddMomentumType)(struct bolt_s *bolt, struct gameobject_s *object, NUVEC *momentum);
    void Bolt_GetShootOrigin_Default(struct gameobject_s *, NUVEC *);
    u16 Bolt_GetShootDirection_Default(struct gameobject_s *, NUVEC *);
    BOLT *FindIncomingBolt(struct gameobject_s *, i32, i32);
    void Bolt_Shoot(struct gameobject_s *, i32, i32);
    struct PART *Bolt_HitParts(BOLT *, NUVEC *, NUVEC *, NUVEC *, f32, i32);
    i32 Bolt_HitGameObject(BOLT *, struct gameobject_s *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
    extern i32 (*BoltInitSfxFn)(struct gameobject_s *);
    void GuidedMissile_Deflect(struct PART *part);
    void GuidedMissile_Kill(struct PART *part, i32 reason);
    void GuidedMissile_Move(struct PART *part, f32 time);
#ifdef __cplusplus
}
#endif
