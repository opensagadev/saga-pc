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
struct player_coin_packet_pc_s;

typedef struct game_message_pc_s GAME_MESSAGE_PC;

typedef struct GIZMOPICKUP_PC {
    char name[8];
    NUVEC position;

    union {
        u8 unknown_14[2];

        struct {
            u8 filter_byte;
            u8 filter_reserved;
        };
    };

    u8 model_variant;

    union {
        u8 flags;

        struct {
            u8 state_active : 1;
            u8 state_enabled : 1;
            u8 state_visible : 1;
            u8 state_collected : 1;
            u8 state_drawn : 1;
            u8 state_draw_visible : 1;
            u8 state_alternate_type : 1;
            u8 state_activated : 1;
        };
    };

    u8 activation_group;
    u8 unknown_19[3];
    f32 floor;
    u16 shadow_x;
    u16 shadow_z;

    union {
        i8 room;
        u8 collected;
    };

    u8 type_index;
    u16 rotation;
    f32 timer;
} GIZMOPICKUP_PC;

STATIC_ASSERT(sizeof(GIZMOPICKUP_PC) == 0x2c);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, position) == 8);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, model_variant) == 0x16);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, flags) == 0x17);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, floor) == 0x1c);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, shadow_x) == 0x20);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, shadow_z) == 0x22);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, room) == 0x24);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, type_index) == 0x25);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, rotation) == 0x26);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, timer) == 0x28);
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
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, filter_byte) == 0x14);
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

typedef struct PICKUP_TYPE_SYSTEM_PC {
    struct GIZMO_PICKUP_TYPE_PC *types;
    u8 *random_coin_types;
    u16 type_count;
    i8 minikit_type;
    i8 gizmo_type_id;
    i8 alternate_type;
    i8 unknown_0d;
    i16 unknown_0e;
} PICKUP_TYPE_SYSTEM_PC;

STATIC_ASSERT(sizeof(PICKUP_TYPE_SYSTEM_PC) == 0x10);
STATIC_ASSERT(offsetof(PICKUP_TYPE_SYSTEM_PC, types) == 0);
STATIC_ASSERT(offsetof(PICKUP_TYPE_SYSTEM_PC, random_coin_types) == 4);
STATIC_ASSERT(offsetof(PICKUP_TYPE_SYSTEM_PC, type_count) == 8);
STATIC_ASSERT(offsetof(PICKUP_TYPE_SYSTEM_PC, minikit_type) == 0xa);
STATIC_ASSERT(offsetof(PICKUP_TYPE_SYSTEM_PC, gizmo_type_id) == 0xb);
STATIC_ASSERT(offsetof(PICKUP_TYPE_SYSTEM_PC, alternate_type) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 (*GizmoPickups_Collide2DFn)(struct gameobject_s *);
    void UpdateCoinPacket(struct player_coin_packet_pc_s *packet, i32 active, i32 player_index);
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

struct game_message_pc_s {
    const char *text;
    char text_buffer[0x78];
    NUVEC position_a;
    NUVEC target_position;
    NUVEC position;
    NUVEC start_position;
    f32 scale_start;
    f32 target_scale;
    f32 scale_initial;
    f32 scale_current;
    f32 elapsed;
    f32 duration;
    f32 parameter_c4;
    f32 parameter_c8;
    f32 scale_multiplier;
    f32 delay;
    f32 parameter_d4;
    u32 flags;
    u32 score;
    u16 parameter_e0;
    u16 parameter_e2;
    u16 parameter_e4;
    u16 icon;
    NUSPECIALHANDLE special;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
    u8 active;
    u8 has_start_position;
    u8 expired;
    u8 parameter_fb;
    u8 parameter_fc;
    i8 player_index;
    u8 target_is_coin_total;
    u8 allow_temporary;
    GAME_MESSAGE_CALLBACK_PC delay_fn;
    GAME_MESSAGE_CALLBACK_PC tick_fn;
    GAME_MESSAGE_CALLBACK_PC update_fn;

    union {
        void *parameter_10c;
        void (*draw_fn)(struct game_message_pc_s *, NUVEC *, f32);
    };

    GAME_MESSAGE_CALLBACK_PC end_fn;
};

STATIC_ASSERT(sizeof(GAME_MESSAGE_PC) == 0x114);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, text) == 0x0);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, text_buffer) == 0x4);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, position_a) == 0x7c);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, target_position) == 0x88);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, position) == 0x94);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, start_position) == 0xa0);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, scale_start) == 0xac);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, target_scale) == 0xb0);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, scale_initial) == 0xb4);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, scale_current) == 0xb8);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, elapsed) == 0xbc);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, duration) == 0xc0);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_c4) == 0xc4);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_c8) == 0xc8);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, scale_multiplier) == 0xcc);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, delay) == 0xd0);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_d4) == 0xd4);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, flags) == 0xd8);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, score) == 0xdc);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_e0) == 0xe0);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_e2) == 0xe2);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_e4) == 0xe4);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, icon) == 0xe6);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, special) == 0xe8);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, red) == 0xf4);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, green) == 0xf5);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, blue) == 0xf6);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, alpha) == 0xf7);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, active) == 0xf8);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, has_start_position) == 0xf9);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, expired) == 0xfa);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_fb) == 0xfb);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_fc) == 0xfc);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, player_index) == 0xfd);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, target_is_coin_total) == 0xfe);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, allow_temporary) == 0xff);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, delay_fn) == 0x100);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, tick_fn) == 0x104);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, update_fn) == 0x108);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, parameter_10c) == 0x10c);
STATIC_ASSERT(offsetof(GAME_MESSAGE_PC, end_fn) == 0x110);
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
