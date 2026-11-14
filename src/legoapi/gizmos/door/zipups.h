/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP)

struct WORLDINFO_s;
struct gameobject_s;
struct numtl_s;

typedef struct zipup_s ZIPUP_PC;
#ifdef __cplusplus
extern "C" {
#endif
    struct addgizmotype_pc_s *ZipUps_RegisterGizmo(i32 type_id);
    void InitRopeMtl(char *name, VARIPTR *buffer, VARIPTR *buffer_end);
    extern i32 zipup_gizmotype_id;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void DrawRopeSingle(NUVEC *start, NUVEC *end, f32 amount, struct numtl_s *material, f32 time, f32 grow_time,
                        f32 spacing, f32 scale);
    extern struct numtl_s *ropemtl;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART == 3 && !defined(SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    ZIPUP_PC *ZipUp_FindNearest(struct WORLDINFO_s *, NUVEC *, f32, f32 *, i32 *, struct gameobject_s *);
#ifdef __cplusplus
}
#endif
typedef struct ZIPUP_s ZIPUP_s;

struct ZIPUP_s {                   // size 0x7c
    char name[16];                 // 0x00
    NUVEC lower_position;          // 0x10
    NUVEC hook_origin;             // 0x1c
    NUVEC upper_position;          // 0x28
    u16 hook_x_rotation;           // 0x34
    u16 hook_y_rotation;           // 0x36
    NUVEC hook_position;           // 0x38
    NUVEC rider_start_offset;      // 0x44
    NUVEC rider_target_position;   // 0x50
    struct gameobject_s *occupant; // 0x5c
    u16 pitch_adjustment;          // 0x60
    u16 direction;                 // 0x62
    u16 facing_angle;              // 0x64
    u8 reserved_0x66[2];           // 0x66

    union { // size 0x2

        struct {              // size 0x2
            u8 flags;         // 0x68
            u8 runtime_flags; // 0x69
        };

        struct {                         // size 0x2
            u16 configuration_flags : 6; // 0x68:0
            u16 active : 1;              // 0x68:6
            u16 visible : 1;             // 0x68:7
            u16 runtime_bits : 8;        // 0x68:8
        };
    }; // 0x68

    u8 reserved_0x6a[2];         // 0x6a
    f32 lower_ground_height;     // 0x6c
    f32 upper_ground_height;     // 0x70
    u16 lower_ground_x_rotation; // 0x74
    u16 upper_ground_x_rotation; // 0x76
    u16 lower_ground_z_rotation; // 0x78
    u16 upper_ground_z_rotation; // 0x7a
};

#undef SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMOS_DOOR_ZIPUPS_H_STOP
