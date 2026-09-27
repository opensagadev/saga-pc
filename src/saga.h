#ifndef SAGA_H
#define SAGA_H

#include <stddef.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // Expose the Windows XP APIs used by the retail imports.
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <oleauto.h>
#include "externals.h"

typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed __int64 i64;
typedef unsigned __int64 u64;
typedef float f32;
typedef double f64;

// VC8-compatible C/C++ checks for the retail x86 data model.
typedef char saga_i8_size[(sizeof(i8) == 1) ? 1 : -1];
typedef char saga_i16_size[(sizeof(i16) == 2) ? 1 : -1];
typedef char saga_i32_size[(sizeof(i32) == 4) ? 1 : -1];
typedef char saga_i64_size[(sizeof(i64) == 8) ? 1 : -1];
typedef char saga_f32_size[(sizeof(f32) == 4) ? 1 : -1];
typedef char saga_f64_size[(sizeof(f64) == 8) ? 1 : -1];
typedef char saga_pointer_size[(sizeof(void *) == 4) ? 1 : -1];

typedef i32 NUANG;

typedef struct nunrand_s {
    i32 value;
} NURAND;

typedef char saga_nurand_size[(sizeof(NURAND) == 4) ? 1 : -1];

#ifdef __cplusplus
extern "C" {
#endif
    int __cdecl NuMain(int argc, char **argv);
    int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR commandLine, int show);

    extern u32 fseed;

    i32 __cdecl NuRand(NURAND *rand);
    f32 __cdecl NuRandFloat(void);
    f32 __cdecl NuRandFloatSeeded(u32 *seed);
    u32 __cdecl NuRandInt(void);
    u32 __cdecl NuRandIntSeeded(u32 *seed);
    void __cdecl NuRandSeed(u32 seed);
    void __cdecl NuRandSetSeed(NURAND *rand, i32 seed);
    u32 __cdecl NuRandGetSeed(void);
    f32 __cdecl NuFloatRand(NURAND *rand);

    NUANG __cdecl NuAngAdd(NUANG a, NUANG b);
    NUANG __cdecl NuAngSub(NUANG a, NUANG b);
    void __cdecl NuAng2AltSol(NUANG *out_x, NUANG *out_y, NUANG *out_z, NUANG x, NUANG y, NUANG z);
#ifdef __cplusplus
}

// These retain C++ linkage and are not exposed as C declarations.
extern i32 qseed;
i32 __cdecl qrand(void);
#endif

#endif
