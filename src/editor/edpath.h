/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_EDITOR_EDPATH_H_PART
#define SAGA_EDITOR_EDPATH_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edpath_types.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_EDITOR_EDPATH_H_PART == 1 && !defined(SAGA_EDITOR_EDPATH_H_STOP)
typedef struct EDAIPATHCHECK_s EDAIPATHCHECK_s;
#undef SAGA_EDITOR_EDPATH_H_PART
#define SAGA_EDITOR_EDPATH_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_H_STOP
#endif
#endif

#if SAGA_EDITOR_EDPATH_H_PART == 2 && !defined(SAGA_EDITOR_EDPATH_H_STOP)

struct EDAIPATHCHECK_s {           // size 0x1c
    i32 on_path;                   // 0x00
    struct EDAIPATH_s *path;       // 0x04
    struct EDAIPATHNODE_s *first;  // 0x08
    struct EDAIPATHNODE_s *second; // 0x0c
    f32 fraction;                  // 0x10
    f32 width;                     // 0x14
    i32 angle;                     // 0x18
};

#undef SAGA_EDITOR_EDPATH_H_PART
#define SAGA_EDITOR_EDPATH_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_H_STOP
#endif
#endif

#if SAGA_EDITOR_EDPATH_H_PART == 3 && !defined(SAGA_EDITOR_EDPATH_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern void (*AIPathNodeDeletedFn)(struct EDAIPATHNODE_s *arg0);
#ifdef __cplusplus
}
#endif
#undef SAGA_EDITOR_EDPATH_H_PART
#define SAGA_EDITOR_EDPATH_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_H_STOP
#endif
#endif

#undef SAGA_EDITOR_EDPATH_H_STOP
