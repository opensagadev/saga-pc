#ifndef SAGA_NU2API_NUCORE_FIXED_WIDTH_H
#define SAGA_NU2API_NUCORE_FIXED_WIDTH_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.

typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed __int64 i64;
typedef unsigned __int64 u64;
// NOLINTEND(google-runtime-int)

// VC8-compatible C/C++ checks for the retail x86 data model.
STATIC_ASSERT(sizeof(i8) == 1);
STATIC_ASSERT(sizeof(i16) == 2);
STATIC_ASSERT(sizeof(i32) == 4);
STATIC_ASSERT(sizeof(i64) == 8);

#endif
