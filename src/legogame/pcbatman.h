/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/level.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern void *Game_NuPad_Store[2];
    i32 StartupParseArgument(i32 count, char **arguments);
    extern const char *StartupDatNames[7];
    extern i32 SUPERBUFFERSIZE;
    extern VARIPTR original_permbuffer_base;
    extern const char InitOnceAllocSource[15];
    extern char InitOnceCurrentDirectory[1];
    extern const char *SaveIconName;
    extern char SaveUnicodeName[20];
    extern i32 CHARACTERBUFFERSIZE;
#ifdef __cplusplus
    void PcControlPlaySound(i32);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern const char InitOnceCompany[10];
    extern const char InitOnceProduct[35];
    extern i32 InitOnceFileMode;
    extern i32 InitOnceSelector34;
    extern i32 InitOnceDvd;
    extern i32 InitOnceHostFs;
    extern i32 InitOnceEject;

    extern LEVELFIXUP_PC LevelFixupStartup[2];
    extern char HubButtonStringBuffer[];
#ifdef __cplusplus
}
#endif
