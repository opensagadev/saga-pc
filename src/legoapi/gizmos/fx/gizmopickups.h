/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/core/gamemessages.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP)

struct GIZMO_PICKUP_TYPE_PC;
struct WORLDINFO_s;
struct gameobject_s;
struct player_coin_packet_s;

typedef struct game_message_pc_s GAME_MESSAGE_PC;

typedef struct GIZMOPICKUP_PC { // size 0x2c
    char name[8];               // 0x00
    NUVEC position;             // 0x08

    union { // size 0x2
        u8 unknown_14[2];

        struct {                // size 0x2
            u8 filter_byte;     // 0x14
            u8 filter_reserved; // 0x15
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
    u8 unknown_19[3];    // 0x19
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
} GIZMOPICKUP_PC;

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
#undef SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP)
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

typedef struct PICKUP_TYPE_SYSTEM_PC {  // size 0x10
    struct GIZMO_PICKUP_TYPE_PC *types; // 0x0
    u8 *random_coin_types;              // 0x4
    u16 type_count;                     // 0x8
    i8 minikit_type;                    // 0xa
    i8 gizmo_type_id;                   // 0xb
    i8 alternate_type;                  // 0xc
    i8 unknown_0d;                      // 0xd
    i16 unknown_0e;                     // 0xe
} PICKUP_TYPE_SYSTEM_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 (*GizmoPickups_Collide2DFn)(struct gameobject_s *);
    void UpdateCoinPacket(struct player_coin_packet_s *packet, i32 active, i32 player_index);
    void GizmoPickups_InitSys(PICKUP_TYPE_SYSTEM_PC *system);
    GIZMOPICKUP_PC *GizmoPickups_Collide(struct WORLDINFO_s *world, struct gameobject_s *object, i32 unused);
    i32 GizmoPickup_NumberOfType(struct WORLDINFO_s *, i32, u8);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART == 3 && !defined(SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP)

struct game_message_pc_s {              // size 0x114
    const char *text;                   // 0x000
    char text_buffer[0x78];             // 0x004
    NUVEC position_a;                   // 0x07c
    NUVEC target_position;              // 0x088
    NUVEC position;                     // 0x094
    NUVEC start_position;               // 0x0a0
    f32 scale_start;                    // 0x0ac
    f32 target_scale;                   // 0x0b0
    f32 scale_initial;                  // 0x0b4
    f32 scale_current;                  // 0x0b8
    f32 elapsed;                        // 0x0bc
    f32 duration;                       // 0x0c0
    f32 parameter_c4;                   // 0x0c4
    f32 parameter_c8;                   // 0x0c8
    f32 scale_multiplier;               // 0x0cc
    f32 delay;                          // 0x0d0
    f32 parameter_d4;                   // 0x0d4
    u32 flags;                          // 0x0d8
    u32 score;                          // 0x0dc
    u16 parameter_e0;                   // 0x0e0
    u16 parameter_e2;                   // 0x0e2
    u16 parameter_e4;                   // 0x0e4
    u16 icon;                           // 0x0e6
    NUSPECIALHANDLE special;            // 0x0e8
    u8 red;                             // 0x0f4
    u8 green;                           // 0x0f5
    u8 blue;                            // 0x0f6
    u8 alpha;                           // 0x0f7
    u8 active;                          // 0x0f8
    u8 has_start_position;              // 0x0f9
    u8 expired;                         // 0x0fa
    u8 parameter_fb;                    // 0x0fb
    u8 parameter_fc;                    // 0x0fc
    i8 player_index;                    // 0x0fd
    u8 target_is_coin_total;            // 0x0fe
    u8 allow_temporary;                 // 0x0ff
    GAME_MESSAGE_CALLBACK_PC delay_fn;  // 0x100
    GAME_MESSAGE_CALLBACK_PC tick_fn;   // 0x104
    GAME_MESSAGE_CALLBACK_PC update_fn; // 0x108

    union { // size 0x4
        void *parameter_10c;
        void (*draw_fn)(struct game_message_pc_s *, NUVEC *, f32);
    }; // 0x10c

    GAME_MESSAGE_CALLBACK_PC end_fn; // 0x110
};

#ifdef __cplusplus
extern "C" {
#endif
    void GameMsg_Draw_MiniKitDetector(struct game_message_pc_s *arg0, struct nuvec_s *arg1, f32 arg2);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMOS_FX_GIZMOPICKUPS_H_STOP
