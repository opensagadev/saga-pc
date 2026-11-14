/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP)

struct NUGSCN_s;
struct WORLDINFO_s;
struct bolt_s;
struct gameobject_s;
struct nuspecial_handle_s;

typedef struct gizmo_s { // size 0x8
    void *object;        // 0x0
    i16 field_04;        // 0x4
    u8 type_id;          // 0x6
    u8 field_07;         // 0x7
} GIZMO_PC;

struct gizmoset_s;

typedef struct gizmofns_s {                                                     // size 0x70
    i16 options;                                                                // 0x00
    u16 padding;                                                                // 0x02
    i32 (*get_max_gizmos_fn)(void *world);                                      // 0x04
    void *add_gizmos_fn;                                                        // 0x08
    void (*early_update_fn)(void *world, void *data, f32 delta_time);           // 0x0c
    void (*late_update_fn)(void *world, void *data, f32 delta_time);            // 0x10
    void (*draw_fn)(void *world, void *data, f32 delta_time);                   // 0x14
    void (*panel_draw_fn)(void *world, void *data, f32 delta_time);             // 0x18
    char *(*get_gizmo_name_fn)(struct gizmo_s *gizmo);                          // 0x1c
    i32 (*get_output_fn)(struct gizmo_s *gizmo, i32 output, i32 mode);          // 0x20
    char *(*get_output_name_fn)(struct gizmo_s *gizmo, i32 output);             // 0x24
    void *get_num_outputs_fn;                                                   // 0x28
    void (*activate_fn)(struct gizmo_s *gizmo, i32 active);                     // 0x2c
    i32 (*activate_rev_fn)(struct gizmo_s *gizmo, i32 reverse, i32 visibility); // 0x30
    void (*set_visibility_fn)(struct gizmo_s *gizmo, i32 visible);              // 0x34
    i32 (*get_visibility_fn)(struct gizmo_s *gizmo);                            // 0x38
    void *get_pos_fn;                                                           // 0x3c
    void *using_special_fn;                                                     // 0x40
    void *bolt_hit_plat_fn;                                                     // 0x44
    struct gizmo_s *(*get_best_bolt_target_fn)(struct gizmoset_s *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32,
                                               i32);                                   // 0x48
    void *bolt_hit_fn;                                                                 // 0x4c
    void *(*allocate_progress_data_fn)(union variptr_u *buffer, union variptr_u *end); // 0x50
    void (*clear_progress_fn)(void *world, void *progress);                            // 0x54
    void (*store_progress_fn)(void *world, void *data, void *progress);                // 0x58
    void (*reset_fn)(void *world, void *data, void *progress);                         // 0x5c
    void *(*reserve_buffer_space_fn)(void *world);                                     // 0x60
    i32 (*load_fn)(void *world, void *data);                                           // 0x64
    void (*post_load_fn)(void *world, void *data);                                     // 0x68
    void *add_level_sfx_fn;                                                            // 0x6c
} GIZMOFNS;

typedef struct gizmotype_s { // size 0x9c
    char name[32];           // 0x00
    char prefix[8];          // 0x20
    GIZMOFNS fns;            // 0x28
    void **progress;         // 0x98
} GIZMOTYPE;

typedef struct gizmotypes_s { // size 0xc
    i32 count;                // 0x0
    i32 max_levels;           // 0x4
    GIZMOTYPE *types;         // 0x8
} GIZMOTYPES;

typedef struct gizmoset_s { // size 0x14
    GIZMOTYPE *type;        // 0x00
    i32 count;              // 0x04
    i32 max_count;          // 0x08
    GIZMO_PC *gizmos;       // 0x0c
    void *data;             // 0x10
} GIZMOSET_PC;

typedef struct gizmosys_s {
    GIZMOSET_PC *sets;
    char *error_log;
    u16 flags;
    u16 padding;
} GIZMOSYS_PC;

typedef struct addgizmotype_s { // size 0x78
    char *name;                 // 0x00
    char *prefix;               // 0x04
    GIZMOFNS fns;               // 0x08
} ADDGIZMOTYPE_PC;

#ifdef __cplusplus
extern "C" ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
#else
extern ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
#endif

typedef struct gizmosys_prefix_s { // size 0x4
    GIZMOSET_PC *sets;             // 0x0
} GIZMOSYS_PREFIX_PC;

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizmoGetVisibility(void *, void *);
    void *GizmoFindByName(void *gizmo_sys, i32 type_id, char *name);
    extern void *gizmotypes;
    void GizmoSysClearLevelProgress(void *owner, i32 index);
    extern i32 gizmoerrorlogsize;
    i32 GizmoGetOutput(void *system, void *gizmo, i32 output, i32 mode);
    char *GizmoGetOutputName(void *system, void *gizmo, i32 output);
    void GizmoActivateReverse(void *system, void *gizmo, i32 reverse, i32 visibility, i32 mode);
    void GizmoSetVisibility(void *system, void *gizmo, i32 visible, i32 mode);
    void GizmoActivate(void *system, void *gizmo, i32 active, i32 mode);
    void GizmoTypeStoreProgress(GIZMOSYS_PC *system, void *world, i32 progress_index, i32 type_id, char *name);
    struct addgizmotype_s;
    GIZMO_PC *AddGizmo(void *system, i32 type_id, char *name, void *object);
    /* Original gizmo.c private error caller group; existing local CONFIG_SYSTEM view retained. */
    i32 GizmoFileReadName(char *);
    i32 GizmoGetUniqueName(GIZMOSYS_PREFIX_PC *, char *, char *, char *, i32);

    struct addgizmotype_s;

    struct addgizmotype_s;

    struct addgizmotype_s;

    struct addgizmotype_s;

    typedef struct addgizmotype_s TECHNO_REGISTRATION_PC;
    typedef struct addgizmotype_s PORTAL_REGISTRATION_PC;
    typedef struct addgizmotype_s SHADOW_REGISTRATION_PC;
    struct nuvec_s *GizmoGetPos(struct gizmosys_s *gizmo_sys, void *gizmo);
    char *GizmoGetName(struct gizmo_s *gizmo);
    void GizmoSysPanelDraw(void *, void *, f32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP)

typedef struct GIZACTIONDEF { // size 0x8
    const char *name;         // 0x0
    GIZACTIONFN action_fn;    // 0x4
} GIZACTIONDEF;

#ifdef __cplusplus
extern "C" {
#endif
    GIZAIMESSAGE *CheckGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message);
    GIZAIMESSAGE *SetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, f32 value, GIZAIMESSAGE *message);
    char *GizAIMessage_GetName(GIZAIMESSAGE *message);

    void *GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART == 3 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 Gizmo_FindNuSpecial(struct NUGSCN_s *, struct nuspecial_handle_s *, char *, i32, void *, char *, char *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART == 4 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void AddLevelSfxGizmoSys(GIZMOSYS_PREFIX_PC *, struct WORLDINFO_s *, i32 *, i32 *, i32);

    i32 GizmoSys_SetBestBoltTarget(void *, struct WORLDINFO_s *, struct gameobject_s *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32);
    i32 GizmoSys_BoltHitPlat(void *, struct WORLDINFO_s *, struct bolt_s *, u8 *);
    void ResetPaintPuzzle(struct WORLDINFO_s *world);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_STOP
