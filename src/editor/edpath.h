#ifndef SAGA_EDITOR_EDPATH_H_PART
#define SAGA_EDITOR_EDPATH_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

struct EDAIPATHCHECK_s {
    i32 on_path;
    struct EDAIPATH_s *path;
    struct EDAIPATHNODE_s *first;
    struct EDAIPATHNODE_s *second;
    f32 fraction;
    f32 width;
    i32 angle;
};

STATIC_ASSERT(sizeof(struct EDAIPATHCHECK_s) == 28);
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
