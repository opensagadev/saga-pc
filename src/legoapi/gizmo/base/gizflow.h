#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/gizmo/base/gizactions.h, legoapi/gizmo/base/gizmo.h,
 * nu2api/nucore/common.h, nu2api/nucore/fixed_width.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_STOP)

typedef struct GIZFLOWPROGRESS_PC {
    i32 valid;
    u32 active[16];
    u32 triggered[16];
    u32 completed[16];
    u32 latched[16];
    u32 output_state[16];
} GIZFLOWPROGRESS_PC;

typedef struct FLOWBOXCONDITION_PC {
    u8 operation;
    u8 threshold;
} FLOWBOXCONDITION_PC;

STATIC_ASSERT(offsetof(FLOWBOXCONDITION_PC, operation) == 0);
STATIC_ASSERT(offsetof(FLOWBOXCONDITION_PC, threshold) == 1);

typedef struct FLOWBOX_PC FLOWBOX_PC;
typedef struct FLOWBOXGIZMOREF_PC FLOWBOXGIZMOREF_PC;
typedef struct FLOWBOXGIZMODATA_PC FLOWBOXGIZMODATA_PC;

struct FLOWBOX_PC {
    u8 parent_count;
    u8 loop_parent_count;
    u8 child_count;
    u8 type;

    union {
        i32 runtime_id;

        struct {
            u8 ai_trigger_group;
            u8 unknown_05_trigger_group[3];
        };
    };

    u8 last_process_frame;
    u8 loop_checksum;

    union {
        u16 state_flags;

        struct {
            u8 state_flags_low;
            u8 state_flags_high;
        };

        struct {
            u16 enabled : 1;
            u16 reserved_state_flags : 15;
        };
    };

    void *data;
    FLOWBOX_PC **parents;
    FLOWBOX_PC **children;
    u8 *output_indices;
    char *name;
};

struct FLOWBOXGIZMOREF_PC {
    void *gizmo;
    char *name;
};

struct FLOWBOXGIZMODATA_PC {
    i32 gizmo_count;
    i32 capacity;
    FLOWBOXGIZMOREF_PC **gizmos;
};

STATIC_ASSERT(sizeof(GIZFLOWPROGRESS_PC) == 0x144);
STATIC_ASSERT(offsetof(GIZFLOWPROGRESS_PC, valid) == 0x0);
STATIC_ASSERT(offsetof(GIZFLOWPROGRESS_PC, active) == 0x4);
STATIC_ASSERT(offsetof(GIZFLOWPROGRESS_PC, triggered) == 0x44);
STATIC_ASSERT(offsetof(GIZFLOWPROGRESS_PC, completed) == 0x84);
STATIC_ASSERT(offsetof(GIZFLOWPROGRESS_PC, latched) == 0xc4);
STATIC_ASSERT(offsetof(GIZFLOWPROGRESS_PC, output_state) == 0x104);

STATIC_ASSERT(sizeof(FLOWBOX_PC) == 0x20);
STATIC_ASSERT(offsetof(FLOWBOX_PC, parent_count) == 0x0);
STATIC_ASSERT(offsetof(FLOWBOX_PC, loop_parent_count) == 0x1);
STATIC_ASSERT(offsetof(FLOWBOX_PC, child_count) == 0x2);
STATIC_ASSERT(offsetof(FLOWBOX_PC, type) == 0x3);
STATIC_ASSERT(offsetof(FLOWBOX_PC, runtime_id) == 0x4);
STATIC_ASSERT(offsetof(FLOWBOX_PC, last_process_frame) == 0x8);
STATIC_ASSERT(offsetof(FLOWBOX_PC, loop_checksum) == 0x9);
STATIC_ASSERT(offsetof(FLOWBOX_PC, state_flags) == 0xa);
STATIC_ASSERT(offsetof(FLOWBOX_PC, state_flags_low) == 0xa);
STATIC_ASSERT(offsetof(FLOWBOX_PC, state_flags_high) == 0xb);
STATIC_ASSERT(offsetof(FLOWBOX_PC, data) == 0xc);
STATIC_ASSERT(offsetof(FLOWBOX_PC, parents) == 0x10);
STATIC_ASSERT(offsetof(FLOWBOX_PC, children) == 0x14);
STATIC_ASSERT(offsetof(FLOWBOX_PC, output_indices) == 0x18);
STATIC_ASSERT(offsetof(FLOWBOX_PC, name) == 0x1c);

STATIC_ASSERT(sizeof(FLOWBOXGIZMOREF_PC) == 0x8);
STATIC_ASSERT(offsetof(FLOWBOXGIZMOREF_PC, gizmo) == 0x0);
STATIC_ASSERT(offsetof(FLOWBOXGIZMOREF_PC, name) == 0x4);

STATIC_ASSERT(sizeof(FLOWBOXGIZMODATA_PC) == 0xc);
STATIC_ASSERT(offsetof(FLOWBOXGIZMODATA_PC, gizmo_count) == 0x0);
STATIC_ASSERT(offsetof(FLOWBOXGIZMODATA_PC, capacity) == 0x4);
STATIC_ASSERT(offsetof(FLOWBOXGIZMODATA_PC, gizmos) == 0x8);
#ifdef __cplusplus
extern "C" {
#endif
    extern u8 gizflow_loop_checksum;
    void GizFlowLoadDiagnosticPc(char *);
    void GizFlowMissingNamePc(char *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_STOP)

typedef struct FLOWBOXACTION_PC {
    struct FLOWBOXACTION_PC *next;
    char **parameters;
    i32 parameter_count;
    GIZACTIONDEF_PC *definition;
} FLOWBOXACTION_PC;

typedef struct FLOWBOX_CALLBACKS_PC {
    void (*reset)(GIZFLOW_PC *, FLOWBOX_PC *);
    i32 (*process)(GIZFLOW_PC *, FLOWBOX_PC *, u8);
    i32 (*check_output)(GIZFLOW_PC *, FLOWBOX_PC *, u8);
} FLOWBOX_CALLBACKS_PC;

STATIC_ASSERT(sizeof(FLOWBOXACTION_PC) == 0x10);
STATIC_ASSERT(offsetof(FLOWBOXACTION_PC, next) == 0);
STATIC_ASSERT(offsetof(FLOWBOXACTION_PC, parameters) == 4);
STATIC_ASSERT(offsetof(FLOWBOXACTION_PC, parameter_count) == 8);
STATIC_ASSERT(offsetof(FLOWBOXACTION_PC, definition) == 0xc);
STATIC_ASSERT(sizeof(FLOWBOX_CALLBACKS_PC) == 0xc);
STATIC_ASSERT(offsetof(FLOWBOX_CALLBACKS_PC, reset) == 0);
STATIC_ASSERT(offsetof(FLOWBOX_CALLBACKS_PC, process) == 4);
STATIC_ASSERT(offsetof(FLOWBOX_CALLBACKS_PC, check_output) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    void GizFlowStoreProgress(GIZFLOW_PC *system, GIZFLOWPROGRESS_PC *progress);
    extern const FLOWBOX_CALLBACKS_PC flowbox_callbacks[3];
    i32 CheckOutputGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
    i32 ProcessConditionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
    i32 CheckOutputConditionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
    i32 ProcessActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
    void PerformActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box);
    void ResetGizFlow(GIZFLOW_PC *system, GIZFLOWPROGRESS_PC *progress);
    FLOWBOX_PC *FlowBoxFindByName(GIZFLOW_PC *system, char *name);
    void ResetGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box);
    i32 CheckOutputActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
    void ProcessFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
    void ResetGizFlowPointers(GIZFLOW_PC *system);
    void ProcessGizFlow(GIZFLOW_PC *system, f32 delta_time);
    i32 ProcessGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_STOP
