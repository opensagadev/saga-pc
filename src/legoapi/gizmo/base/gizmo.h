#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMO_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

typedef struct gizmo_pc_s {
    void *object;
    i16 field_04;
    u8 type_id;
    u8 field_07;
} GIZMO_PC;

STATIC_ASSERT(sizeof(GIZMO_PC) == 8);
STATIC_ASSERT(offsetof(GIZMO_PC, type_id) == 6);

struct gizmoset_pc_s;

typedef struct gizmofns_pc_s {
    i16 options;
    u16 padding;
    i32 (*get_max_gizmos_fn)(void *world);
    void *add_gizmos_fn;
    void (*early_update_fn)(void *world, void *data, f32 delta_time);
    void (*late_update_fn)(void *world, void *data, f32 delta_time);
    void (*draw_fn)(void *world, void *data, f32 delta_time);
    void (*panel_draw_fn)(void *world, void *data, f32 delta_time);
    char *(*get_gizmo_name_fn)(struct gizmo_pc_s *gizmo);
    i32 (*get_output_fn)(struct gizmo_pc_s *gizmo, i32 output, i32 mode);
    char *(*get_output_name_fn)(struct gizmo_pc_s *gizmo, i32 output);
    void *get_num_outputs_fn;
    void (*activate_fn)(struct gizmo_pc_s *gizmo, i32 active);
    i32 (*activate_rev_fn)(struct gizmo_pc_s *gizmo, i32 reverse, i32 visibility);
    void (*set_visibility_fn)(struct gizmo_pc_s *gizmo, i32 visible);
    i32 (*get_visibility_fn)(struct gizmo_pc_s *gizmo);
    void *get_pos_fn;
    void *using_special_fn;
    void *bolt_hit_plat_fn;
    struct gizmo_pc_s *(*get_best_bolt_target_fn)(struct gizmoset_pc_s *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *,
                                                  NUVEC *, f32, f32, i32, i32, i32);
    void *bolt_hit_fn;
    void *(*allocate_progress_data_fn)(union variptr_u *buffer, union variptr_u *end);
    void (*clear_progress_fn)(void *world, void *progress);
    void (*store_progress_fn)(void *world, void *data, void *progress);
    void (*reset_fn)(void *world, void *data, void *progress);
    void *(*reserve_buffer_space_fn)(void *world);
    i32 (*load_fn)(void *world, void *data);
    void (*post_load_fn)(void *world, void *data);
    void *add_level_sfx_fn;
} GIZMOFNS_PC;

STATIC_ASSERT(sizeof(GIZMOFNS_PC) == 0x70);

typedef struct gizmotype_pc_s {
    char name[32];
    char prefix[8];
    GIZMOFNS_PC fns;
    void **progress;
} GIZMOTYPE_PC;

STATIC_ASSERT(sizeof(GIZMOTYPE_PC) == 0x9c);

typedef struct gizmotypes_pc_s {
    i32 count;
    i32 max_levels;
    GIZMOTYPE_PC *types;
} GIZMOTYPES_PC;

typedef struct gizmoset_pc_s {
    GIZMOTYPE_PC *type;
    i32 count;
    i32 max_count;
    GIZMO_PC *gizmos;
    void *data;
} GIZMOSET_PC;

STATIC_ASSERT(sizeof(GIZMOSET_PC) == 0x14);

typedef struct gizmosys_pc_s {
    GIZMOSET_PC *sets;
    char *error_log;
    u16 flags;
    u16 padding;
} GIZMOSYS_PC;

typedef struct addgizmotype_pc_s {
    char *name;
    char *prefix;
    GIZMOFNS_PC fns;
} ADDGIZMOTYPE_PC;

STATIC_ASSERT(sizeof(ADDGIZMOTYPE_PC) == 0x78);
#ifdef __cplusplus
extern "C" ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
#else
extern ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
#endif

STATIC_ASSERT(offsetof(GIZMOSET_PC, data) == 0x10);

typedef struct gizmosys_prefix_pc_s {
    GIZMOSET_PC *sets;
} GIZMOSYS_PREFIX_PC;

STATIC_ASSERT(offsetof(GIZMOSYS_PREFIX_PC, sets) == 0);
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
    struct addgizmotype_pc_s;
    GIZMO_PC *AddGizmo(void *system, i32 type_id, char *name, void *object);
    /* Original gizmo.c private error caller group; existing local CONFIG_SYSTEM view retained. */
    extern const char GizmoSpecialErrorTabPc[3];
    i32 GizmoFileReadName(char *);
    i32 GizmoGetUniqueName(GIZMOSYS_PREFIX_PC *, char *, char *, char *, i32);

    struct addgizmotype_pc_s;

    struct addgizmotype_pc_s;

    struct addgizmotype_pc_s;

    struct addgizmotype_pc_s;

    typedef struct addgizmotype_pc_s TECHNO_REGISTRATION_PC;
    typedef struct addgizmotype_pc_s PORTAL_REGISTRATION_PC;
    typedef struct addgizmotype_pc_s SHADOW_REGISTRATION_PC;
    struct nuvec_s *GizmoGetPos(struct gizmosys_pc_s *gizmo_sys, void *gizmo);
    char *GizmoGetName(struct gizmo_pc_s *gizmo);
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

typedef struct GIZACTIONDEF_PC {
    const char *name;
    GIZACTIONFN_PC action_fn;
} GIZACTIONDEF_PC;

STATIC_ASSERT(sizeof(GIZACTIONDEF_PC) == 8);
STATIC_ASSERT(offsetof(GIZACTIONDEF_PC, action_fn) == 4);
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

    i32 GizmoSys_SetBestBoltTarget(void *, struct WORLDINFO_s *, struct gameobject_s *, NUVEC *, NUVEC *, f32, f32, i32,
                                   i32, i32);
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
