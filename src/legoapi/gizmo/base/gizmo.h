#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmos/traps/gizforce.h"
#include "nu2api/numath/nuvec.h"

struct NUGSCN_s;
struct WORLDINFO_s;
struct bolt_s;
struct gameobject_s;
struct nuhspecial_s;

typedef struct gizmo_s { // size 0x8
    void *object;        // 0x0
    i16 unknown_04;      // 0x4
    u8 type_id;          // 0x6
    u8 unknown_07;       // 0x7
} GIZMO;

struct gizmoset_s;

/* A gizmo type's bolt hooks: a bolt hitting the type's platforms, and a bolt hitting one of its gizmos. */
typedef i32 (*GIZMO_BOLT_HIT_PLAT_FN)(void *world, void *data, struct bolt_s *bolt, u8 *hit_flags);
typedef i32 (*GIZMO_BOLT_HIT_FN)(void *world, void *data, struct gameobject_s *owner, NUVEC *points, i32 mode, f32 radius, NUVEC *minimum,
                                 NUVEC *maximum, struct bolt_s *bolt, i32 hit_type, u8 *hit_flags);

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
    GIZMO_BOLT_HIT_PLAT_FN bolt_hit_plat_fn;                                    // 0x44
    struct gizmo_s *(*get_best_bolt_target_fn)(struct gizmoset_s *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32,
                                               i32);                                   // 0x48
    GIZMO_BOLT_HIT_FN bolt_hit_fn;                                                     // 0x4c
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
    GIZMO *gizmos;          // 0x0c
    void *data;             // 0x10
} GIZMOSET;

typedef struct gizmosys_s {
    GIZMOSET *sets;
    char *error_log;
    u16 flags;
    u16 padding;
} GIZMOSYS;

typedef struct addgizmotype_s { // size 0x78
    char *name;                 // 0x00
    char *prefix;               // 0x04
    GIZMOFNS fns;               // 0x08
} ADDGIZMOTYPE;

#ifdef __cplusplus
extern "C" ADDGIZMOTYPE Default_ADDGIZMOTYPE;
#else
extern ADDGIZMOTYPE Default_ADDGIZMOTYPE;
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizmoGetVisibility(void *, void *);
    void *GizmoFindByName(void *gizmo_sys, i32 type_id, char *name);
    GIZMOSYS *CreateGizmoSys(void *world, VARIPTR *buffer, VARIPTR *end);
    extern GIZMOTYPES *gizmotypes;
    void GizmoSysClearLevelProgress(void *owner, i32 index);
    extern i32 gizmoerrorlogsize;
    i32 GizmoGetOutput(void *system, void *gizmo, i32 output, i32 mode);
    char *GizmoGetOutputName(void *system, void *gizmo, i32 output);
    void GizmoActivateReverse(void *system, void *gizmo, i32 reverse, i32 visibility, i32 mode);
    void GizmoSetVisibility(void *system, void *gizmo, i32 visible, i32 mode);
    void GizmoActivate(void *system, void *gizmo, i32 active, i32 mode);
    void GizmoTypeStoreProgress(GIZMOSYS *system, void *world, i32 progress_index, i32 type_id, char *name);
    struct addgizmotype_s;
    GIZMO *AddGizmo(void *system, i32 type_id, char *name, void *object);
    /* Original gizmo.c private error caller group; existing local CONFIG_SYSTEM view retained. */
    i32 GizmoFileReadName(char *);
    i32 GizmoGetUniqueName(GIZMOSYS *, char *, char *, char *, i32);

    struct addgizmotype_s;

    struct addgizmotype_s;

    struct addgizmotype_s;

    struct addgizmotype_s;

    typedef struct addgizmotype_s TECHNO_REGISTRATION;
    typedef struct addgizmotype_s PORTAL_REGISTRATION;
    typedef struct addgizmotype_s SHADOW_REGISTRATION;
    struct nuvec_s *GizmoGetPos(struct gizmosys_s *gizmo_sys, void *gizmo);
    char *GizmoGetName(struct gizmo_s *gizmo);
    void GizmoSysPanelDraw(void *, void *, f32);
#ifdef __cplusplus
}
#endif

struct FLOWBOX;
struct GIZFLOW;

typedef void (*GIZACTIONFN)(struct GIZFLOW *, struct FLOWBOX *, char **, i32);

typedef struct GIZACTIONDEF { // size 0x8
    const char *name;         // 0x0
    GIZACTIONFN action_fn;    // 0x4
} GIZACTIONDEF;

#ifdef __cplusplus
extern "C" {
#endif

    void *GizmoBufferAlloc(VARIPTR *, VARIPTR *, u32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Gizmo_FindNuSpecial(struct NUGSCN_s *, struct nuhspecial_s *, char *, i32, void *, char *, char *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void AddLevelSfxGizmoSys(GIZMOSYS *, struct WORLDINFO_s *, i32 *, i32 *, i32);

    i32 GizmoSys_SetBestBoltTarget(void *, struct WORLDINFO_s *, struct gameobject_s *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32);
    i32 GizmoSys_BoltHitPlat(void *, struct WORLDINFO_s *, struct bolt_s *, u8 *);
    void ResetPaintPuzzle(struct WORLDINFO_s *world);
    i32 GizmoGetTypeIDByName(void *, char *);
    void GizmoDiagnostic(char *format, ...);
    void GizmoSysReset(void *system, void *world, i32 progress_index);
    void GizmoSysStoreProgress(void *system, void *world, i32 index);
    void RegisterGizmoTypes(VARIPTR *, VARIPTR *, REGISTER_GIZMO_CONFIG_TYPE_FN *, i32);
    void GizmoSysEarlyUpdate(void *system, void *world, f32 delta_time);
    void GizmoSysLateUpdate(void *system, void *world, f32 delta_time);
#ifdef __cplusplus
}
#endif
