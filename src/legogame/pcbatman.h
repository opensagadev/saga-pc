#ifndef SAGA_LEGOGAME_PCBATMAN_H
#define SAGA_LEGOGAME_PCBATMAN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/level.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/level.h"
#endif

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
    extern const char *SaveIconNamePc;
    extern char SaveUnicodeNamePc[20];
    extern i32 CHARACTERBUFFERSIZE;
#ifdef __cplusplus
    void PcControlPlaySoundPc(i32);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern const char InitOnceCompanyPc[10];
    extern const char InitOnceProductPc[35];
    extern const char InitOnceEmptyConfigPc[27];
    extern i32 InitOnceFileModePc;
    extern i32 InitOnceSelector34Pc;
    extern i32 InitOnceDvdPc;
    extern i32 InitOnceHostFsPc;
    extern i32 InitOnceEjectPc;
    extern const char InitOnceWindowTitle[36];

    extern LEVELFIXUP_PC LevelFixupStartupPc[2];
    extern char HubButtonStringBufferPc[];
#ifdef __cplusplus
}
#endif

#endif
