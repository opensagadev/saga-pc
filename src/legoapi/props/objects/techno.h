/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART == 1 && !defined(SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_STOP)

struct GAMEPAD_PC;
struct addgizmotype_pc_s;
struct gameobject_s;
struct nuspecial_handle_s;

typedef struct techno_pc_s {
    u8 unknown_00[0x37];
    u8 target_mode;
    u8 unknown_38[0x34];

    union {
        void *controlled_object;
        struct gameobject_s *controlled_player;
        struct nuspecial_handle_s *controlled_special;
    };
} TECHNO_PC;

STATIC_ASSERT(offsetof(TECHNO_PC, target_mode) == 0x37);
STATIC_ASSERT(offsetof(TECHNO_PC, controlled_object) == 0x6c);
STATIC_ASSERT(sizeof(TECHNO_PC) == 0x70);

typedef struct techno_config_pc_s {
    f32 interaction_time;
    i16 idle_object_ids[4];
    NUVEC idle_offsets[3];
    i16 hand_object_ids[2];
    NUVEC hand_offsets[2];
} TECHNO_CONFIG_PC;

STATIC_ASSERT(sizeof(TECHNO_CONFIG_PC) == 0x4c);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, interaction_time) == 0);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, idle_object_ids) == 4);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, idle_offsets) == 0xc);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, hand_object_ids) == 0x30);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, hand_offsets) == 0x34);
#ifdef __cplusplus
extern "C" {
#endif
    extern const char *TechnoRegistrationNamePc;
    extern TECHNO_CONFIG_PC TechnoSysPc;
    extern i32 techno_gizmotype_id;
    struct addgizmotype_pc_s *Technos_RegisterGizmo(i32 type_id);
    extern TECHNO_CONFIG_PC TechnoSysPc;
    extern const char *TechnoRegistrationNamePc;
    extern i32 techno_gizmotype_id;
#ifdef __cplusplus
}
#endif
typedef struct TECHNO_s TECHNO_s;

struct TECHNO_s {
    char name[16];
    NUVEC position;
    NUVEC ground_position;
    f32 ground_offset;
    f32 scale;
    u16 y_rotation;
    u16 ground_x_rotation;
    u16 ground_z_rotation;
    u8 enabled;
    u8 target_mode;
    i32 output;

    union {
        u8 flags;

        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 used_this_frame : 1;
            u8 complete : 1;
            u8 reserved_flags : 4;
        };
    };

    char target_name[16];

    union {
        char target_object_name[31];

        struct {
            u8 target_special_padding[19];
            u8 target_special_storage[12];
        };
    };

    void *controlled_object;
};

STATIC_ASSERT(sizeof(struct TECHNO_s) == 112);
#undef SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART == 2 && !defined(SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    TECHNO_PC *Technos_FindControllingTechno(struct gameobject_s *);

    i32 Techno_FindOperator(void *, struct GAMEPAD_PC **, struct gameobject_s **);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_PROPS_OBJECTS_TECHNO_H_STOP
