#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct GIZMO_PICKUP_TYPE;
struct PART;
struct WORLDINFO_s;
struct addgizmotype_s;
struct gameobject_s;
struct player_coin_packet_s;

typedef struct game_message_s GAME_MESSAGE;

/* SPECIAL_MINIKIT.flags (gizmopickups.c): the specialminikit keywords that set them. */
#define SPECIAL_MINIKIT_DRAW_ALWAYS 0x1          /* until a draw_at_* keyword */
#define SPECIAL_MINIKIT_DRAW_AT_START 0x2        /* draw_at_start */
#define SPECIAL_MINIKIT_DRAW_WHILE_ANIMATING 0x4 /* draw_while_animating */
#define SPECIAL_MINIKIT_DRAW_AT_END 0x8          /* draw_at_end */
#define SPECIAL_MINIKIT_ORIENTATE 0x10           /* orientate */
#define SPECIAL_MINIKIT_AT_GIZMO 0x20            /* at_gizmo (at_object clears it) */

/* GIZMOPICKUP.flags: GizmoPickup_TurnOnPickup sets ENABLED and VISIBLE, the draw loop skips a pickup without
 * VISIBLE or with COLLECTED, collecting sets COLLECTED, and a minikit already saved draws the alternate type. */
#define GIZMOPICKUP_STATE_ACTIVE 0x1 /* the slot holds a pickup: both loops skip one without it */
#define GIZMOPICKUP_STATE_ENABLED 0x2
#define GIZMOPICKUP_STATE_VISIBLE 0x4
#define GIZMOPICKUP_STATE_COLLECTED 0x8
#define GIZMOPICKUP_STATE_DRAWN 0x10        /* drawn this frame (cleared at the start of the draw loop) */
#define GIZMOPICKUP_STATE_DRAW_VISIBLE 0x20 /* set by the update loop; the draw loop skips a pickup without it */
#define GIZMOPICKUP_STATE_ALTERNATE_TYPE 0x40

typedef struct GIZMOPICKUP { // size 0x2c
    char name[8];            // 0x00
    NUVEC position;          // 0x08

    union { // size 0x2
        u8 type_flags[2];

        struct {             // size 0x2
            u8 type_code;    // 0x14
            u8 config_flags; // 0x15
        };
    }; // 0x14

    u8 model_variant; // 0x16

    union { // size 0x1
        u8 flags;

        struct {                         // size 0x1
            u8 state_active : 1;         // 0x17:0
            u8 state_enabled : 1;        // 0x17:1
            u8 state_visible : 1;        // 0x17:2
            u8 state_collected : 1;      // 0x17:3
            u8 state_drawn : 1;          // 0x17:4
            u8 state_draw_visible : 1;   // 0x17:5
            u8 state_alternate_type : 1; // 0x17:6
            u8 state_activated : 1;      // 0x17:7
        };
    }; // 0x17

    u8 activation_group; // 0x18
    u8 reserved_19[3];    // 0x19
    f32 floor;           // 0x1c
    u16 shadow_x;        // 0x20
    u16 shadow_z;        // 0x22

    union { // size 0x1
        i8 room;
        u8 collected;
    }; // 0x24

    u8 type_index; // 0x25
    u16 rotation;  // 0x26
    f32 timer;     // 0x28
} GIZMOPICKUP;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 AreaPickupScale;
    extern f32 COINMSGTIME;
    extern f32 AreaPickupScale;
    void ResetCoinPacket(void *);
    extern f32 COINMAGNETSCALE;

    void SetAreaPickupGravity(i32 area, i32 level);
    extern i32 gizmopickup_typeid;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void GizmoPickup_CollectCoin(struct WORLDINFO_s *, NUVEC *, i32, i32, struct gameobject_s *, i32);
    void SpecialMiniKits_Configure(struct WORLDINFO_s *, char *);
    void SpecialMiniKits_Reset(struct WORLDINFO_s *);
    void SpecialMiniKits_Draw(struct WORLDINFO_s *);

#ifdef __cplusplus
}
#endif

typedef struct PICKUP_TYPE_SYSTEM {  // size 0x10
    struct GIZMO_PICKUP_TYPE *types; // 0x0
    u8 *random_coin_types;           // 0x4
    u16 type_count;                  // 0x8
    i8 minikit_type;                 // 0xa
    i8 gizmo_type_id;                // 0xb
    i8 alternate_type;               // 0xc
    i8 reserved_0d;                   // 0xd
    i16 reserved_0e;                  // 0xe
} PICKUP_TYPE_SYSTEM;

#ifdef __cplusplus
extern "C" {
#endif
    extern PICKUP_TYPE_SYSTEM GizmoPickupSys_Game;

    extern f32 (*GizmoPickups_Collide2DFn)(struct gameobject_s *);
    void UpdateCoinPacket(struct player_coin_packet_s *packet, i32 active, i32 player_index);
    void GizmoPickups_InitSys(PICKUP_TYPE_SYSTEM *system);
    GIZMOPICKUP *GizmoPickups_Collide(struct WORLDINFO_s *world, struct gameobject_s *object, i32 unused);
    i32 GizmoPickup_NumberOfType(struct WORLDINFO_s *, i32, u8);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void GameMsg_Draw_MiniKitDetector(struct game_message_s *arg0, struct nuvec_s *arg1, f32 arg2);
    extern char *GizmoPickup_OutputName;
    void GizmoPickups_PostLoad(void *, void *);
    struct addgizmotype_s *GizmoPickups_RegisterGizmo(i32);
    GIZMOPICKUP *GizmoPickup_FindByName(struct WORLDINFO_s *, char *);
    GIZMOPICKUP *GizmoPickup_InBox(struct WORLDINFO_s *world, i32 type_index, NUVEC *minimum, NUVEC *maximum);
    i32 GizmoPickup_BeenTurnedOn(GIZMOPICKUP *);
    u32 GizmoPickups_TotalScore(struct WORLDINFO_s *world);
    void GizmoPickup_TurnOnPickup(GIZMOPICKUP *);
    void PartUpdate_Heart(struct PART *part);
#ifdef __cplusplus
}
#endif
