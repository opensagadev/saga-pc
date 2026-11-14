/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuSPrintf(char *buffer, const char *format, ...);
    i32 NuHasError(void);
    void NuClearError(void);
    void NuSevereWarning(const char *format, ...);
    extern i32 file_criticalsection;
    extern i32 nufile_buffering_enabled;
    NUFILE_DEVICE *NuFileAddDevice(NUFILE_DEVICE *device);
    u16 *NuStringBadSubString(const u16 *, const u16 *, i32 *, i32);
    void NuDatFileDecodeNext(void);
    void NuFileExtInit(i32);
    void NuRegisterEndFrameCallBackFn(void (*)(void));
    char *NuGetErrN(i32 entry);

    char *NuStrStr(char *text, const char *needle);
#ifdef __cplusplus
}
#endif
