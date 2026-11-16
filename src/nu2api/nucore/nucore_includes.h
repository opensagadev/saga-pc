#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufile/nufile.h"

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
    extern i32 FileDeviceFormatDefaultName(char *destination, char *path, i32 maximum_size);
    void NuStrToLower(char *text);
    i32 NuVSPrintf(char *dest, const char *format, va_list args);
    extern i32 NuFParGetLineW(struct nufpar_s *parser);
    extern i32 NuFParGetWordW(struct nufpar_s *parser);
#ifdef __cplusplus
}
#endif
