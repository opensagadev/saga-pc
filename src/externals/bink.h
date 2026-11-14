#ifndef SAGA_EXTERNALS_BINK_H
#define SAGA_EXTERNALS_BINK_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nufmv/pc/Bink/dsplay_bink.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
#endif

// Native PC Bink ABI: opaque decoder records and the verified 32-bit DLL exports.
typedef struct PC_FMV_BINK_PC PC_FMV_BINK_PC;
typedef void *(__stdcall *FMV_SOUND_SYSTEM_OPEN_PC)(IDirectSound8 *);
#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllimport) i32 __stdcall BinkSetSoundSystem(FMV_SOUND_SYSTEM_OPEN_PC, IDirectSound8 *);
    __declspec(dllimport) void *__stdcall BinkOpenDirectSound(IDirectSound8 *);
    __declspec(dllimport) void __stdcall BinkSetSoundTrack(u32, u32 *);
    __declspec(dllimport) PC_FMV_BINK_PC *__stdcall BinkOpen(const char *, u32);
    __declspec(dllimport) void __stdcall BinkClose(PC_FMV_BINK_PC *);
    __declspec(dllimport) void __stdcall BinkSetVolume(PC_FMV_BINK_PC *, u32, i32);
    __declspec(dllimport) i32 __stdcall BinkDoFrame(PC_FMV_BINK_PC *);
    __declspec(dllimport) i32 __stdcall BinkCopyToBufferRect(PC_FMV_BINK_PC *, void *, i32, u32, u32, u32, u32, u32,
                                                             u32, u32, u32);
    __declspec(dllimport) i32 __stdcall BinkWait(PC_FMV_BINK_PC *);
    __declspec(dllimport) void __stdcall BinkNextFrame(PC_FMV_BINK_PC *);
    __declspec(dllimport) void __stdcall BinkGetRealtime(PC_FMV_BINK_PC *, PC_FMV_REALTIME_PC *, u32);
    __declspec(dllimport) void __stdcall BinkGoto(PC_FMV_BINK_PC *, u32, u32);
#ifdef __cplusplus
}
#endif

#endif
