/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern NUDATHDR *StartupDatList[7];
    extern i32 StartupDatCount;
#ifdef __cplusplus
}
#endif

typedef struct HOTH_WORLD_SCRIPT {
    u8 reserved[0x2b00];
    AISCRIPTPROCESS processor;
} HOTH_WORLD_SCRIPT;

typedef struct area_gizmosys_prefix_s { // size 0x4
    AREA_GIZMOSET *sets;                // 0x0
} AREA_GIZMOSYS_PREFIX;

typedef struct area_gizmotypes_s { // size 0xc
    i32 count;                     // 0x0
    i32 unknown_0004;              // 0x4
    AREA_GIZMOTYPE *types;         // 0x8
} AREA_GIZMOTYPES;

typedef struct {
    BUILD_PROGRESS_RECORD *records;
    u16 count;
} BUILD_PROGRESS_SYSTEM;
