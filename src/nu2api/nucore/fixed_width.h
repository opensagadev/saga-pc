#ifndef SAGA_NU2API_NUCORE_FIXED_WIDTH_H
#define SAGA_NU2API_NUCORE_FIXED_WIDTH_H

/* The fixed-width integer types; nu2api/nucore/common.h, the prelude, includes this header and checks their sizes. */

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

#endif
