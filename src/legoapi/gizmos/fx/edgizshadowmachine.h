/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct addgizmotype_pc_s;

typedef struct edgizshadow_s { // size 0x38
    NUVEC direction;           // 0x00
    f32 field_0c;              // 0x0c
    f32 field_10;              // 0x10
    f32 field_14;              // 0x14
    f32 field_18;              // 0x18
    f32 field_1c;              // 0x1c
    f32 field_20;              // 0x20
    f32 field_24;              // 0x24
    f32 field_28;              // 0x28
    f32 field_2c;              // 0x2c
    f32 field_30;              // 0x30
    i32 preset;                // 0x34
} EDGIZSHADOW_PC;

#ifdef __cplusplus
extern "C" {
#endif
    struct addgizmotype_pc_s *EdGizShadowMachine_RegisterGizmo(i32 type_id);
#ifdef __cplusplus
}
#endif
