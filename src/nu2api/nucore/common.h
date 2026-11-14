#ifndef SAGA_NU2API_NUCORE_COMMON_H_PART
#define SAGA_NU2API_NUCORE_COMMON_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/fixed_width.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nucore/fixed_width.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUCORE_COMMON_H_PART == 1 && !defined(SAGA_NU2API_NUCORE_COMMON_H_STOP)
// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.
typedef float f32;
typedef double f64;
// NOLINTEND(google-runtime-int)
STATIC_ASSERT(sizeof(f32) == 4);
STATIC_ASSERT(sizeof(f64) == 8);
union variptr_u;

typedef u32 usize;

/* Android macros and callback types that ported code uses. */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#undef SAGA_NU2API_NUCORE_COMMON_H_PART
#define SAGA_NU2API_NUCORE_COMMON_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_COMMON_H_STOP
#endif
#endif

#if SAGA_NU2API_NUCORE_COMMON_H_PART == 2 && !defined(SAGA_NU2API_NUCORE_COMMON_H_STOP)

typedef union variptr_u {
    void *void_ptr; // 0x0
    char *char_ptr; // 0x0
    u32 addr;       // 0x0
} VARIPTR;          // size 0x4

#undef SAGA_NU2API_NUCORE_COMMON_H_PART
#define SAGA_NU2API_NUCORE_COMMON_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_COMMON_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUCORE_COMMON_H_STOP
