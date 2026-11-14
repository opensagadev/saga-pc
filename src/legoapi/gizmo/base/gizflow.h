/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART
#define SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMO_BASE_GIZFLOW_H_STOP)

struct GIZACTIONDEF;
struct GIZFLOW;

typedef struct GIZFLOWPROGRESS { // size 0x144
    i32 valid;                   // 0x000
    u32 active[16];              // 0x004
    u32 triggered[16];           // 0x044
    u32 completed[16];           // 0x084
    u32 latched[16];             // 0x0c4
    u32 output_state[16];        // 0x104
} GIZFLOWPROGRESS;

typedef struct FLOWBOXCONDITION { // size 0x2
    u8 operation;                 // 0x0
    u8 threshold;                 // 0x1
} FLOWBOXCONDITION;

typedef struct FLOWBOX FLOWBOX;
typedef struct FLOWBOXGIZMOREF FLOWBOXGIZMOREF;
typedef struct FLOWBOXGIZMODATA FLOWBOXGIZMODATA;

struct FLOWBOX {          // size 0x20
    u8 parent_count;      // 0x00
    u8 loop_parent_count; // 0x01
    u8 child_count;       // 0x02
    u8 type;              // 0x03

    union { // size 0x4
        i32 runtime_id;

        struct {                            // size 0x4
            u8 ai_trigger_group;            // 0x04
            u8 unknown_05_trigger_group[3]; // 0x05
        };
    }; // 0x04

    u8 last_process_frame; // 0x08
    u8 loop_checksum;      // 0x09

    union { // size 0x2
        u16 state_flags;

        struct {                 // size 0x2
            u8 state_flags_low;  // 0x0a
            u8 state_flags_high; // 0x0b
        };

        struct {                           // size 0x2
            u16 enabled : 1;               // 0x0a:0
            u16 reserved_state_flags : 15; // 0x0a:1
        };
    }; // 0x0a

    void *data;         // 0x0c
    FLOWBOX **parents;  // 0x10
    FLOWBOX **children; // 0x14
    u8 *output_indices; // 0x18
    char *name;         // 0x1c
};

struct FLOWBOXGIZMOREF { // size 0x8
    void *gizmo;         // 0x0
    char *name;          // 0x4
};

struct FLOWBOXGIZMODATA {     // size 0xc
    i32 gizmo_count;          // 0x0
    i32 capacity;             // 0x4
    FLOWBOXGIZMOREF **gizmos; // 0x8
};

#ifdef __cplusplus
extern "C" {
#endif
    extern u8 gizflow_loop_checksum;
    void GizFlowLoadDiagnostic(char *);
    void GizFlowMissingName(char *, i32);
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

typedef struct FLOWBOXACTION {       // size 0x10
    struct FLOWBOXACTION *next;      // 0x0
    char **parameters;               // 0x4
    i32 parameter_count;             // 0x8
    struct GIZACTIONDEF *definition; // 0xc
} FLOWBOXACTION;

typedef struct FLOWBOX_CALLBACKS {                        // size 0xc
    void (*reset)(struct GIZFLOW *, FLOWBOX *);           // 0x0
    i32 (*process)(struct GIZFLOW *, FLOWBOX *, u8);      // 0x4
    i32 (*check_output)(struct GIZFLOW *, FLOWBOX *, u8); // 0x8
} FLOWBOX_CALLBACKS;

#ifdef __cplusplus
extern "C" {
#endif
    void GizFlowStoreProgress(struct GIZFLOW *system, GIZFLOWPROGRESS *progress);
    extern const FLOWBOX_CALLBACKS flowbox_callbacks[3];
    i32 CheckOutputGizmoFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
    i32 ProcessConditionFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
    i32 CheckOutputConditionFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
    i32 ProcessActionFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
    void PerformActionFlowBox(struct GIZFLOW *system, FLOWBOX *box);
    void ResetGizFlow(struct GIZFLOW *system, GIZFLOWPROGRESS *progress);
    FLOWBOX *FlowBoxFindByName(struct GIZFLOW *system, char *name);
    void ResetGizmoFlowBox(struct GIZFLOW *system, FLOWBOX *box);
    i32 CheckOutputActionFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
    void ProcessFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
    void ResetGizFlowPointers(struct GIZFLOW *system);
    void ProcessGizFlow(struct GIZFLOW *system, f32 delta_time);
    i32 ProcessGizmoFlowBox(struct GIZFLOW *system, FLOWBOX *box, u8 output);
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
