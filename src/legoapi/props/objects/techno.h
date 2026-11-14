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

typedef struct techno_s { // size 0x70
    u8 unknown_00[0x37];  // 0x00
    u8 target_mode;       // 0x37
    u8 unknown_38[0x34];  // 0x38

    union { // size 0x4
        void *controlled_object;
        struct gameobject_s *controlled_player;
        struct nuspecial_handle_s *controlled_special;
    }; // 0x6c
} TECHNO_PC;

typedef struct techno_config_s { // size 0x4c
    f32 interaction_time;        // 0x00
    i16 idle_object_ids[4];      // 0x04
    NUVEC idle_offsets[3];       // 0x0c
    i16 hand_object_ids[2];      // 0x30
    NUVEC hand_offsets[2];       // 0x34
} TECHNO_CONFIG_PC;

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

struct TECHNO_s {          // size 0x70
    char name[16];         // 0x00
    NUVEC position;        // 0x10
    NUVEC ground_position; // 0x1c
    f32 ground_offset;     // 0x28
    f32 scale;             // 0x2c
    u16 y_rotation;        // 0x30
    u16 ground_x_rotation; // 0x32
    u16 ground_z_rotation; // 0x34
    u8 enabled;            // 0x36
    u8 target_mode;        // 0x37
    i32 output;            // 0x38

    union { // size 0x1
        u8 flags;

        struct {                    // size 0x1
            u8 active : 1;          // 0x3c:0
            u8 visible : 1;         // 0x3c:1
            u8 used_this_frame : 1; // 0x3c:2
            u8 complete : 1;        // 0x3c:3
            u8 reserved_flags : 4;  // 0x3c:4
        };
    }; // 0x3c

    char target_name[16]; // 0x3d

    union { // size 0x1f
        char target_object_name[31];

        struct {                           // size 0x1f
            u8 target_special_padding[19]; // 0x4d
            u8 target_special_storage[12]; // 0x60
        };
    }; // 0x4d

    void *controlled_object; // 0x6c
};

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
