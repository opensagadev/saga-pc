#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"

// Native PC Bink ABI: opaque decoder records and the verified 32-bit DLL exports.
typedef struct PC_FMV_BINK PC_FMV_BINK;
typedef void *(__stdcall *FMV_SOUND_SYSTEM_OPEN)(IDirectSound8 *);
#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllimport) i32 __stdcall BinkSetSoundSystem(FMV_SOUND_SYSTEM_OPEN, IDirectSound8 *);
    __declspec(dllimport) void *__stdcall BinkOpenDirectSound(IDirectSound8 *);
    __declspec(dllimport) void __stdcall BinkSetSoundTrack(u32, u32 *);
    __declspec(dllimport) PC_FMV_BINK *__stdcall BinkOpen(const char *, u32);
    __declspec(dllimport) void __stdcall BinkClose(PC_FMV_BINK *);
    __declspec(dllimport) void __stdcall BinkSetVolume(PC_FMV_BINK *, u32, i32);
    __declspec(dllimport) i32 __stdcall BinkDoFrame(PC_FMV_BINK *);
    __declspec(dllimport) i32 __stdcall BinkCopyToBufferRect(PC_FMV_BINK *, void *, i32, u32, u32, u32, u32, u32, u32, u32, u32);
    __declspec(dllimport) i32 __stdcall BinkWait(PC_FMV_BINK *);
    __declspec(dllimport) void __stdcall BinkNextFrame(PC_FMV_BINK *);
    __declspec(dllimport) void __stdcall BinkGetRealtime(PC_FMV_BINK *, PC_FMV_REALTIME *, u32);
    __declspec(dllimport) void __stdcall BinkGoto(PC_FMV_BINK *, u32, u32);
#ifdef __cplusplus
}
#endif
