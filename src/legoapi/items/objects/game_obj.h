#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx/edsplines.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct PART;
struct WORLDINFO_s;
struct gameobject_s;
struct numtx_s;
struct nuhspecial_s;

typedef struct PART_LIGHTING_DATA PART_LIGHTING_DATA;
typedef void (*PART_MOVE)(struct PART *, f32);
typedef i32 (*PART_DRAW)(struct PART *);

/* The first 0xcc bytes of an RTL_DATA: what a part copies from its lighting_data. */
struct PART_LIGHTING_DATA {     // size 0xcc
    u8 pad_00[0x78];            // 0x00
    NUVEC intensity_vectors[3]; // 0x78
    NUVEC direction[3];         // 0x9c
    NUVEC ambient;              // 0xc0
};

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 objhitobj_noimpactsfx;
    extern i32 objhitobj_noattackerrumble;
    extern i32 objhitobj_throwkillpartsup;
    extern u8 objhit_damage_joints[3];
    extern i32 CombatHitReactionInterval;
    extern f32 CombatReactionVerticalScale;
    extern f32 CombatReactionDuration;
    extern f32 CombatNpcKillDifficultyAmount;
    extern f32 CombatPlayerDeathDifficultyAmount;
    extern i16 id_OBIWANKENOBIEP3;
    extern i16 id_ANAKINJEDI;
    extern const char ShieldHitSfx[9];
    extern const char ShieldFlickerSfx[13];
    extern i32 KillPartsPreviousVariant;
    extern f32 KillPartsTieTrailScale;
    extern NUVEC KillPartsTieSpinTranslation;
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

    extern f32 InteractionArrowScale;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern RTL_DATA KillPartsZeroLighting;
#ifdef __cplusplus
}
#endif

typedef void (*PART_KILL)(struct PART *, i32);

/* PART.active: AddPart takes a slot without it and sets it, KillPart clears it; the part loops skip slots
 * without it. */
#define PART_IN_USE 0x1

/* PART.active, the other runtime bits, from AddPart, CastPart, the part update and draw and the detonators;
 * 0x10, 0x80, 0x400, 0x800 and 0x1000 are not named yet. */
#define PART_STATE_STOPPED 0x2        /* it has stopped moving (debris may stop with it) */
#define PART_STATE_IMPACT_PENDING 0x4 /* CastPart found an impact within this step */
#define PART_STATE_LIT 0x8            /* lighting data from the parameters */
#define PART_STATE_SCALING_UP 0x20    /* growing after AddPart (PARTSCALEUPTIME) */
#define PART_STATE_SCALING_DOWN 0x40  /* shrinking */
#define PART_STATE_BELOW_CUTOFF 0x100 /* fell below its cutoff height: dies off screen */
#define PART_STATE_DRAWN 0x200        /* drawn this frame */
#define PART_STATE_LIGHT_SCALE 0x2000 /* the parameters' lighting scale */
#define PART_STATE_BEEPED 0x4000      /* a thermal detonator's last-second beep has played */
#define PART_STATE_IN_WATER 0x8000    /* a thermal detonator under the water surface */
#define PART_STATE_REFLECTED 0x20000  /* a thermal detonator drawn reflected */

/* PART.flags, ADDPART.flags and the part type's flags: the part editor's "Instance Flags" and "Debris Settings"
 * toggles (parts.c), labels in the comments. */
#define PART_FLAG_DIE_WHEN_STOPPED 0x1             /* "Die when stopped" */
#define PART_FLAG_COLLIDE_WITH_CHARACTERS 0x2      /* "Collide with Characters" */
#define PART_FLAG_ACTIVE_CHARACTERS_ONLY 0x4       /* "Interact only with Active Chars" */
#define PART_FLAG_DAMAGE_CHARACTERS 0x8            /* "Damage Characters" */
#define PART_FLAG_KEEP_OFF_SCREEN 0x10             /* "Don't die when off-screen" */
#define PART_FLAG_COLLECTIBLE 0x40                 /* "Is a Collectible" */
#define PART_FLAG_ROTATE_RANDOMLY 0x80             /* "Rotate Randomly" */
#define PART_FLAG_NO_BOUNCE 0x100                  /* "No Bounce" */
#define PART_FLAG_SLOT_CANNOT_BE_STOLEN 0x200      /* "Slot Cannot be Stolen" */
#define PART_FLAG_IGNORE_TERRAIN 0x400             /* "Ignore Terrain" */
#define PART_FLAG_FACE_MOVEMENT 0x800              /* "Face Direction of Movement" */
#define PART_FLAG_DISABLE_DRAW 0x1000              /* "Disable Draw" */
#define PART_FLAG_REAL_TIME_LIGHTING 0x2000        /* "Real Time Lighting" */
#define PART_FLAG_IGNORE_CREATURE 0x4000           /* "Ignore Creature" */
#define PART_FLAG_THROWN 0x8000                    /* "Thrown" */
#define PART_FLAG_CAN_DAMAGE_OWNER 0x10000         /* "Can Damage Owner" */
#define PART_FLAG_ORDERED_ROTATE_RANDOMLY 0x100000 /* "Ordered Rotate Randomly" */
#define PART_FLAG_DEBRIS_STOPS_WITH_PART 0x200000  /* "Debris stops when Part stops" */
#define PART_FLAG_TRIGGER_INSIDE_CUTOFF 0x400000   /* "Trigger effect inside cutoff" */

typedef struct PART { // size 0x220

    union { // size 0x40
        NUMTX transform;

        struct {                       // size 0x40
            u8 transform_prefix[0x30]; // 0x000
            NUVEC position;            // 0x030
            f32 transform_m33;         // 0x03c
        };
    }; // 0x000

    NUMTX previous_transform;       // 0x040
    NUVEC velocity;                 // 0x080
    NUVEC collision_min;            // 0x08c
    NUVEC collision_max;            // 0x098
    NUVEC impact_position;          // 0x0a4
    NUVEC impact_normal;            // 0x0b0
    NUVEC guided_target;            // 0x0bc
    NUVEC previous_position;        // 0x0c8
    struct gameobject_s *owner;     // 0x0d4
    struct gameobject_s *recipient; // 0x0d8
    f32 radius;                     // 0x0dc
    f32 glow_radius;                // 0x0e0
    f32 collision_radius;           // 0x0e4
    f32 gravity;                    // 0x0e8
    f32 bounce;                     // 0x0ec
    f32 cutoff_height;              // 0x0f0
    f32 time_of_impact;             // 0x0f4
    f32 time_step;                  // 0x0f8
    f32 scale_time;                 // 0x0fc
    f32 time_remaining;             // 0x100
    f32 initial_time;               // 0x104
    u32 flags;                      // 0x108

    union { // size 0x18
        u32 pickup_data[6];
        i32 rotation_parameters[6];
    }; // 0x10c

    /* rotation speeds (x, y, z) and the random spin of the part */
    i32 spin_rates[5];            // 0x124
    u16 rotation_x;               // 0x138
    u16 rotation_y;               // 0x13a
    u16 rotation_z;               // 0x13c
    i16 part_id;                  // 0x13e
    u16 unknown_140;              // 0x140
    u8 rotation_axes[2];          // 0x142
    u32 active;                   // 0x144
    nuhspecial_s special;         // 0x148
    nuhspecial_s *source_special; // 0x154
    NUVEC lighting[7];            // 0x158
    PART_ACTION callback_1ac;     // 0x1ac
    PART_MOVE move_fn;            // 0x1b0
    PART_ACTION deflect_fn;       // 0x1b4
    PART_ACTION impact_fn;        // 0x1b8
    PART_ACTION collision_fn;     // 0x1bc
    PART_KILL kill_fn;            // 0x1c0
    PART_ACTION update_fn;        // 0x1c4
    PART_ACTION stop_fn;          // 0x1c8
    PART_ACTION destroy_fn;       // 0x1cc
    PART_DRAW draw_fn;            // 0x1d0
    i32 inline_debris_handle;     // 0x1d4
    i32 *debris_handle;           // 0x1d8
    i32 *lighting_handle;         // 0x1dc
    i32 effect_id_1e0;            // 0x1e0
    i32 effect_id_1e4;            // 0x1e4
    f32 trail_rate_a;             // 0x1e8
    f32 trail_rate_b;             // 0x1ec
    i32 effect_id_1f0;            // 0x1f0
    i32 effect_id_1f4;            // 0x1f4
    i32 effect_id_1f8;            // 0x1f8
    f32 effect_scale;             // 0x1fc
    u32 unknown_200;              // 0x200
    u16 damage;                   // 0x204
    i8 player_mask;               // 0x206
    u8 hostility_group;           // 0x207
    u8 first_frame;               // 0x208
    u8 surface_type;              // 0x209
    u8 flicker_flags;             // 0x20a
    i8 effect_slot;               // 0x20b

    union { // size 0x4
        f32 crate_spawn_delay;
        f32 speeder_index; /* the speeder chase bike parts: the KillPartsBikes slot, -1 for none */
    }; // 0x20c

    union { // size 0x4
        u32 user_word;
        struct GIZMOBLOWUP *carried_blowup; /* the blowup the grabber drops (Grabber_Drop) */
    }; // 0x210

    f32 particle_scale; // 0x214

    union { // size 0x4
        u32 force_flags;
        struct GIZMOBLOWUP *supercarry_blowup; /* the carried blowup a thrown part stands for (SuperCarry_Throw) */
    }; // 0x218

    union { // size 0x4
        u32 trail_state;
        f32 reflection_height;
    }; // 0x21c
} PART;

typedef struct ADDPART {            // size 0xc8
    struct numtx_s *matrix;         // 0x00
    NUVEC *position;                // 0x04
    NUVEC *velocity;                // 0x08
    struct gameobject_s *owner;     // 0x0c
    struct gameobject_s *recipient; // 0x10
    f32 start_radius;               // 0x14
    f32 end_radius;                 // 0x18
    f32 gravity;                    // 0x1c
    f32 bounce;                     // 0x20
    struct nuhspecial_s *special;   // 0x24
    i32 part_id;                    // 0x28
    u32 flags;                      // 0x2c
    PART_ACTION callback_30;        // 0x30
    PART_MOVE move_fn;              // 0x34
    PART_ACTION deflect_fn;         // 0x38
    PART_ACTION impact_fn;          // 0x3c
    PART_ACTION collision_fn;       // 0x40
    PART_KILL kill_fn;              // 0x44
    PART_ACTION update_fn;          // 0x48
    PART_ACTION stop_fn;            // 0x4c
    PART_ACTION destroy_fn;         // 0x50
    PART_DRAW draw_fn;              // 0x54
    i32 effect_id_58;               // 0x58
    i32 *debris_handle;             // 0x5c
    i32 effect_id_60;               // 0x60
    i32 effect_id_64;               // 0x64
    f32 trail_rate_a;               // 0x68
    f32 trail_rate_b;               // 0x6c
    i32 effect_id_70;               // 0x70
    i32 effect_id_74;               // 0x74
    i32 effect_id_78;               // 0x78
    f32 effect_scale;               // 0x7c
    i32 *lighting_handle;           // 0x80
    i32 lighting_id;                // 0x84
    f32 lifetime;                   // 0x88
    f32 time_step;                  // 0x8c
    u32 hostility_group;            // 0x90
    RTL_DATA *lighting_data;        // 0x94
    f32 lighting_scale_x;           // 0x98
    f32 lighting_scale_y;           // 0x9c
    f32 lighting_scale_z;           // 0xa0
    f32 fade_time;                  // 0xa4
    u32 rotation_parameters[6];     // 0xa8
    f32 model_scale;                // 0xc0
    u8 flicker_flags;               // 0xc4
} ADDPART;

typedef struct KILLPARTS_BIKE { // size 0x34
    SPLINEPOS spline_position;  // 0x00
    u8 active;                  // 0x20
    u8 unknown_21[3];           // 0x21
    struct gameobject_s *rider; // 0x24
    NUVEC rider_position;       // 0x28
} KILLPARTS_BIKE;

/* KillGameObject and KillPlayer reasons; the reason folds into 2 before it is stored in the object's `dead` */
#define KILL_REASON_KILLED 2     /* an ordinary death: debris, the "BeenKilled" script */
#define KILL_REASON_FELL 3       /* fell below the terrain or was crushed (the creature movement's terrain checks) */
#define KILL_REASON_REMOVED 4    /* removed without debris or script; its rider goes too and the creature may respawn */
#define KILL_REASON_TAKEN_OVER 5 /* removed like 4, but stays out of play: no longer a character, its AI reset disabled */

#ifdef __cplusplus
extern "C" {
#endif

    void InstantKillParts(struct gameobject_s *, i32, f32);
    void ObjHitShield(struct gameobject_s *, struct gameobject_s *, i32, void *);
    void PlayerTakeHit(struct gameobject_s *, struct gameobject_s *);
    i32 PlayerTakeHitReaction(struct gameobject_s *, struct gameobject_s *, f32, u32);

    void LoseHelmet(struct gameobject_s *, i32, i32);
    void SpawnCreatureFromCrate(struct gameobject_s *, f32, f32);
    void KillParts_TIEFIGHTER(ADDPART *, i32, i32, struct gameobject_s *, i32, u16, u16, NUVEC *);
    void KillParts_ATAT(ADDPART *, i32, i32, struct gameobject_s *);
    i32 KillGameObject(struct gameobject_s *, i32, i32);
    extern KILLPARTS_BIKE KillPartsBikes[4];
    extern struct gameobject_s *player_tag_to;
    void ClearLastSafeTakeOver(struct gameobject_s *);
    void GameObjectToCameraCode(struct gameobject_s *);
    void CollideGameObjects(struct WORLDINFO_s *);
    char *GameObj_GetName(i32, struct gameobject_s *, char *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void TiePart_Kill(struct PART *, i32);
    void TiePart_KillExplode(struct PART *, i32);
    void TiePart_Impact(struct PART *);
    void TiePart_Move(struct PART *, f32);
    void TieSpinZPart_Move(struct PART *, f32);
    void AtatPart_Stop(struct PART *);
    void AtatPart_Update(struct PART *);
    void SpeederPart_Kill(struct PART *, i32);
    void SpeederPart_Update(struct PART *);
    i32 SpeederPart_Draw(struct PART *);
    extern struct gameobject_s *player_tag_from;
    void GameObjectRotation(struct gameobject_s *, i32);
    void UpdateGameObjects(struct WORLDINFO_s *world);
#ifdef __cplusplus
}
#endif
