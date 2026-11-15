#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct nuhspecial_s;

struct WORLDINFO_s;
struct bolt_s;
struct gameobject_s;

typedef struct game_antinode_data_s { // size 0x24
    NUVEC position;                   // 0x00
    f32 radius;                       // 0x0c
    f32 min_y;                        // 0x10
    f32 max_y;                        // 0x14
    f32 extent_x;                     // 0x18
    f32 extent_z;                     // 0x1c
    u16 angle;                        // 0x20
    u8 shape;                         // 0x22
    u8 mode;                          // 0x23
} GAMEANTINODEDATA;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 MAXBLOWUPRANDSPANG;
    extern void (*GameBlowUpBlownUpFn)(void *blowup);
    void GizBlowup_InitTerrain(void);
    void GizBlowup_DeleteTerrain(void);
    void SetLevelExBlowupFlags(u32);
    extern i32 GizmoBlowupNameCount;
    extern char GizmoBlowupNameTable[32][32];
    void GizmoBlowupResetNameTable(void);
    i32 GizmoBlowupGetNameTableId(char *);
    void GizmoBlowupLateUpdate(void *, void *, f32);
    void GizmoBlowupEarlyUpdate(void *, void *, f32);

    i32 gizmoblowup_Load(void *, void *);
    u32 GetLevelExBlowupFlags(void);
    extern u32 EXBLOWUPFLAGS;
#ifdef __cplusplus
}
#endif

typedef struct GIZMOBLOWUPTYPE {        // size 0x100
    nuhspecial_s alternate_specials[4]; // 0x00
    nuhspecial_s animated_special;      // 0x30
    nuhspecial_s decal_special;         // 0x3c
    nuhspecial_s shadow_special;        // 0x48
    nuhspecial_s burst_special;         // 0x54
    u8 unknown_60[0x18];                // 0x60
    f32 animation_time_scale;           // 0x78
    u32 animation_flags;                // 0x7c
    u32 effect_flags;                   // 0x80
    f32 unknown_084;                    // 0x84
    f32 animation_start_frame;          // 0x88
    f32 animation_end_frame;            // 0x8c
    f32 animation_base_frame;           // 0x90
    f32 unknown_094;                    // 0x94
    f32 unknown_098;                    // 0x98
    f32 unknown_09c;                    // 0x9c
    i16 particle_types[9];              // 0xa0
    u16 instance_count;                 // 0xb2
    GAMEANTINODEDATA anti_node_data;    // 0xb4
    char name[32];                      // 0xd8
    u8 unknown_0f8;                     // 0xf8
    u8 unknown_0f9;                     // 0xf9
    u8 unknown_0fa;                     // 0xfa
    u8 unknown_0fb;                     // 0xfb
    u8 animation_runtime_flags;         // 0xfc
    u8 unknown_fd[3];                   // 0xfd
} GIZMOBLOWUPTYPE;

/* GizmoBlowupBlowup hit_type: what hit the blowup, by caller. 10 (GizmoBlowUp_Hit for an explosion with flag
 * 0x10) and 12 have no name yet. */
#define BLOWUP_HIT_PUNCH 1             /* fighting punches, the whip lash: the Punched output */
#define BLOWUP_HIT_BATARANG 2          /* Batarang */
#define BLOWUP_HIT_BOLT 3              /* bolts, directly and through GizmoBlowUp_Hit */
#define BLOWUP_HIT_PROXIMITY 4         /* a character walks into the proximity radius */
#define BLOWUP_HIT_PROXIMITY_VEHICLE 5 /* the same for a vehicle or a taken-over character */
#define BLOWUP_HIT_SIGNAL 6            /* signal.c */
#define BLOWUP_HIT_CARRIED 7           /* carrying, the whip pull */
#define BLOWUP_HIT_TRANSFORM 8         /* transform.c */
#define BLOWUP_HIT_EXPLOSION 9         /* GizmoBlowUp_Hit for an explosion */
#define BLOWUP_HIT_TORPEDO 11          /* torpedo.c */
#define BLOWUP_HIT_SABRE 13            /* GizmoBlowUp_Hit for the lightsabre */

/* GIZMOBLOWUP.status_flags: the outputs (Blownup, Punched, Plugging), the bitfield names below and one bit
 * per hit type that GizmoBlowupBlowup sets. */
#define BLOWUP_STATUS_BLOWN_UP 0x1
#define BLOWUP_STATUS_HIT_PUNCH 0x2
#define BLOWUP_STATUS_HIT_BATARANG 0x4
#define BLOWUP_STATUS_HIT_BOLT 0x8
#define BLOWUP_STATUS_HIT_PROXIMITY_VEHICLE 0x10
#define BLOWUP_STATUS_HIT_PROXIMITY 0x20
#define BLOWUP_STATUS_HIT_SIGNAL 0x40
#define BLOWUP_STATUS_HIT_CARRIED 0x80
#define BLOWUP_STATUS_HIT_TRANSFORM 0x100
#define BLOWUP_STATUS_HIT_EXPLOSION 0x200
#define BLOWUP_STATUS_HIT_TORPEDO 0x800
#define BLOWUP_STATUS_HIT_SABRE 0x2000
#define BLOWUP_STATUS_VISIBLE 0x4000
#define BLOWUP_STATUS_DRAWN 0x8000 /* the last NuSpecialDrawAt of the animated special drew it */
#define BLOWUP_STATUS_ACTIVE 0x800000

/* GIZMOBLOWUP.draw_flags: the per-instance options the level file stores, named from what the game does with
 * each. The other bits (0x2000, 0x4000, 0x10000) are listed in docs/magic-values.md. */
#define BLOWUP_OPTION_BATARANG_TARGET 0x1          /* the batarang's target search takes it */
#define BLOWUP_OPTION_TERRAIN 0x4                  /* GizBlowup_InitTerrain gives it a collision platform */
#define BLOWUP_OPTION_CHARACTER_SMASH 0x8          /* smashed by a character walking into it */
#define BLOWUP_OPTION_TRANSFORMABLE 0x10           /* a transform target (GizmoBlowup_TransformDrawFn, transform.c) */
#define BLOWUP_OPTION_SHADOW_MODE_ONLY 0x20        /* ignored while ShadowMode is 0 */
#define BLOWUP_OPTION_CARRIED_ONLY 0x40            /* only a carried object's hit (BLOWUP_HIT_CARRIED) smashes it */
#define BLOWUP_OPTION_POWERUP 0x100                /* releases a power-up with its studs (before version 30 secondary_flags 1) */
#define BLOWUP_OPTION_VEHICLE_SMASH 0x200          /* smashed by a vehicle walking into it */
#define BLOWUP_OPTION_NO_DECAL 0x400               /* no decal special after it is blown up */
#define BLOWUP_OPTION_CREATE_STUFF 0x800           /* GizmoBlowupCreateStuff when it is blown up */
#define BLOWUP_OPTION_MID_AT_ORIGIN 0x1000         /* the mid point is the transform's origin, not the bounds' centre */
#define BLOWUP_OPTION_SHOOTABLE 0x8000             /* bolts smash it */
#define BLOWUP_OPTION_NO_SHADOW 0x20000            /* no shadow special, no shadow map rendering */
#define BLOWUP_OPTION_SHAKE 0x40000                /* the blow up shakes the camera and rumbles the pads */
#define BLOWUP_OPTION_RIDER_BOLTS_ONLY 0x80000     /* only bolts of a ridden vehicle smash it, and no airborne damage */
#define BLOWUP_OPTION_EX_FUNC 0x200000             /* transform and explosion hits go to BlowupExFunc; bolts do not smash it */
#define BLOWUP_OPTION_FOLLOW_ANIMATION 0x400000    /* the transform follows the animated special's instance matrix */
#define BLOWUP_OPTION_SPECIAL_ANIMATION 0x800000   /* the animated special's instance animation times it, not animation_time */
#define BLOWUP_OPTION_TORPEDO_TARGET 0x1000000     /* a torpedo target; hidden in bonus vehicle areas until it plays */
#define BLOWUP_OPTION_KNOCKBACK 0x10000000         /* its explosion knocks characters back (EXPLOSION_KNOCKBACK) */
#define BLOWUP_OPTION_SHARED_PLATFORM 0x20000000   /* the terrain platform is the special's existing instance (FindPlatInst) */
#define BLOWUP_OPTION_REFLECTION 0x40000000        /* drawn again mirrored at reflection_height */
#define BLOWUP_OPTION_FRAGMENTS_PER_HIT 0x80000000 /* fragments fly at every hit, not only the last */
#define BLOWUP_STATUS_PLUGGING 0x10000000

/* GizmoBlowUp_AddEffects flags: what the effects of a hit or a blow up release (GizmoBlowupBlowup and the
 * carried blowups add the explosion when the blowup has an effect delay, the shake for BLOWUP_OPTION_SHAKE). */
#define BLOWUP_EFFECT_DEBRIS 0x1    /* the type's debris effects (particle types 4-6) */
#define BLOWUP_EFFECT_PARTICLES 0x2 /* the type's particle shots (particle types 0 and 1) */
#define BLOWUP_EFFECT_PICKUPS 0x4   /* the studs and hearts, and the fragments */
#define BLOWUP_EFFECT_EXPLOSION 0x8 /* an explosion after the effect delay */
#define BLOWUP_EFFECT_SHAKE 0x10    /* camera shake and pad rumble */

typedef struct GIZMOBLOWUP { // size 0x114

    union { // size 0x40
        NUMTX transform;

        struct {                 // size 0x40
            u8 unknown_00[0x30]; // 0x000
            NUVEC position;      // 0x030
            u32 unknown_3c;      // 0x03c
        };
    }; // 0x000

    union { // size 0xc
        u8 unknown_40[0xc];
        NUVEC screen_position;
    }; // 0x040

    NUVEC mid_position;  // 0x04c
    NUVEC bounds_min;    // 0x058
    NUVEC bounds_max;    // 0x064
    NUVEC decal_offset;  // 0x070
    NUVEC unknown_07c;   // 0x07c
    NUVEC shadow_offset; // 0x088

    union { // size 0x4
        u32 status_flags;

        struct {                       // size 0x4
            u32 blown_up : 1;          // 0x094:0
            u32 unknown_status_1 : 13; // 0x094:1
            u32 visible : 1;           // 0x094:14
            u32 unknown_status_15 : 8; // 0x094:15
            u32 active : 1;            // 0x094:23
            u32 unknown_status_24 : 4; // 0x094:24
            u32 secondary_output : 1;  // 0x094:28
            u32 unknown_status_29 : 3; // 0x094:29
        };
    }; // 0x094

    u32 draw_flags;        // 0x098
    u32 secondary_flags;   // 0x09c
    u32 score;             // 0x0a0
    GIZMOBLOWUPTYPE *type; // 0x0a4
    f32 target_scale;      // 0x0a8
    f32 proximity_radius;  // 0x0ac
    f32 effect_delay;      // 0x0b0
    f32 activation_delay;  // 0x0b4
    f32 proximity_delay;   // 0x0b8
    f32 animation_time;    // 0x0bc
    f32 decal_scale;       // 0x0c0
    f32 unknown_0c4;       // 0x0c4
    f32 animation_offset;  // 0x0c8
    f32 respawn_timer;     // 0x0cc
    f32 respawn_period;    // 0x0d0
    f32 reflection_height; // 0x0d4
    void *anti_node;       // 0x0d8
    u16 decal_rotation_x;  // 0x0dc
    u16 decal_rotation_y;  // 0x0de
    u16 decal_rotation_z;  // 0x0e0
    u16 shadow_rotation_x; // 0x0e2
    u16 shadow_rotation_y; // 0x0e4
    u16 shadow_rotation_z; // 0x0e6
    u16 model_rotation_x;  // 0x0e8
    u16 model_rotation_y;  // 0x0ea
    u16 model_rotation_z;  // 0x0ec
    u16 pickup_x_rotation; // 0x0ee
    u16 pickup_y_rotation; // 0x0f0
    char name[16];         // 0x0f2

    union { // size 0x2
        i16 platform_id;

        struct {                   // size 0x2
            u8 name_terminator;    // 0x102
            u8 platform_high_byte; // 0x103
        };
    }; // 0x102

    i16 burst_platform_id; // 0x104
    u8 unknown_106[6];     // 0x106
    u8 unknown_10c;        // 0x10c
    u8 score_multiplier;   // 0x10d
    u8 damage;             // 0x10e
    u8 initial_state;      // 0x10f
    u8 remaining_health;   // 0x110
    u8 initial_health;     // 0x111
    u8 unknown_112[2];     // 0x112
} GIZMOBLOWUP;

#ifdef __cplusplus
extern "C" {
#endif
    extern void (*GizmoBlowUp_SfxFn)(GIZMOBLOWUP *, NUVEC *);
    void GizBlowup_Respawn(GIZMOBLOWUP *);
    void UpdateMidPos(GIZMOBLOWUP *);

    i32 GizBlowup_InitSingleTerrain(GIZMOBLOWUP *);
    void GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP *);
    void GizmoSwapBlowups(GIZMOBLOWUP *, GIZMOBLOWUP *);

    void GizmoBlowupUpdateMatrix(GIZMOBLOWUP *);
    void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP *, NUMTX *, i32);
    void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP *, NUMTX *);
    extern i32 (*BlowupExFunc)(GIZMOBLOWUP *, i32);
    extern void (*GizmoBlowup_TransformDrawFn)(GIZMOBLOWUP *target);
#ifdef __cplusplus
}
#endif
typedef i32 (*LEVEL_BLOWUP_CALLBACK)(GIZMOBLOWUP *, i32);
#ifdef __cplusplus
extern "C" {
#endif
    void SetLevelBlowupCallback(LEVEL_BLOWUP_CALLBACK callback);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*GizmoBlowUp_NoTargetFn)(struct WORLDINFO_s *, struct gameobject_s *);
    void GizmoBlowUp_AddEffects(NUVEC *position, GIZMOBLOWUP *blowup, i32 add_offset, i32 flags, struct gameobject_s *object);
    i32 GizmoBlowupCheckProximity(struct WORLDINFO_s *, struct gameobject_s *);
    i32 GizmoBlowupBlowup(GIZMOBLOWUP *, i32, i32, i32, struct gameobject_s *, i32);
    GIZMOBLOWUPTYPE *GizmoBlowupTypeAdd(struct WORLDINFO_s *, struct nuhspecial_s *, i32, i32 *);
    void GizmoBlowupTypeRemove(GIZMOBLOWUPTYPE *, struct WORLDINFO_s *);
    void *gizmoblowup_reservebuffers(struct WORLDINFO_s *);
    void GizmoBlowupsFinalSetup(struct WORLDINFO_s *);
    void GizmoBlowupVisibilityOverrides(struct WORLDINFO_s *);
    GIZMOBLOWUP *FindNearestGizmoBlowUp(struct WORLDINFO_s *, NUVEC *, f32);
    void GizmoSortBlowups(struct WORLDINFO_s *);
    i32 GizmoBlowupGetTypeFromNameTableId(struct WORLDINFO_s *, i32);
    GIZMOBLOWUPTYPE *GizmoBlowup_FindType(char *, struct WORLDINFO_s *);
    void GizmoBlowUpTypeBlowUp(struct WORLDINFO_s *, i32, NUVEC *);
    i32 InitGizmoBlowupTypes(struct WORLDINFO_s *);
    i32 InitGizmoBlowups(struct WORLDINFO_s *);
    i32 InitGizmoBlowupsMtxBuffer(struct WORLDINFO_s *);
    GIZMOBLOWUP *GizmoBlowUp_Hit(struct gameobject_s *, NUVEC *, i32, f32, NUVEC *, NUVEC *, struct bolt_s *, u32, u8 *);
#ifdef __cplusplus
}
#endif
