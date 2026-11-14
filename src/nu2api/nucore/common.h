/* The prelude every header may include first (docs/header-plan.md, step 2): the platform headers, externals.h, the
 * scaffolding macros (STATIC_ASSERT, STUBBED, U64_PAIR), the fixed-width types and the engine's basic types. */

#pragma once

#include <stddef.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // NOLINT(readability-identifier-naming): SDK-required Windows XP API selector.
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <oleauto.h>
#include <math.h>
// The engine's SSE vector header (retail keeps its VuVec SSE helper copies in nu2api units) brought <xmmintrin.h>
// into every engine unit. VC8 compiles that header's __inline __m128 wrappers, whose aligned frames keep EBX out of the
// register arguments of every static function in the unit. The bundled Ogg Vorbis sources never included it.
#ifndef SAGA_VENDORED_LIBRARY
#include <xmmintrin.h>
#endif

#include "externals.h"

#define SAGA_JOIN_IMPL(a, b) a##b
#define SAGA_JOIN(a, b) SAGA_JOIN_IMPL(a, b)
#define STATIC_ASSERT(expression) typedef char SAGA_JOIN(saga_static_assert_, __COUNTER__)[(expression) ? 1 : -1]
// src/debug/diagnostics.h defines the Debug versions when a unit includes it first.
#ifndef STUBBED
#define STUBBED() ((void)0)
#endif
#ifndef STUBBED_NAMED
#define STUBBED_NAMED(name) ((void)0)
#endif
// Retail reads several adjacent u32 pairs (bit masks, CInfo flags) as one 64-bit value.  The structs keep their u32
// members so that their alignment stays at 4; this accesses such a pair in place as a u64.
#define U64_PAIR(pair) (*(u64 *)(pair))

#include "nu2api/nucore/fixed_width.h"

// VC8-compatible C/C++ checks for the retail x86 data model.
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(i8) == 1);
STATIC_ASSERT(sizeof(i16) == 2);
STATIC_ASSERT(sizeof(i32) == 4);
STATIC_ASSERT(sizeof(i64) == 8);

// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.
typedef float f32;
typedef double f64;
// NOLINTEND(google-runtime-int)
STATIC_ASSERT(sizeof(f32) == 4);
STATIC_ASSERT(sizeof(f64) == 8);
union variptr_u;

typedef u32 usize;

/* Shared macros and callback types. */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

typedef union variptr_u { // size 0x4
    void *void_ptr;
    char *char_ptr;
    u32 addr;
} VARIPTR;
