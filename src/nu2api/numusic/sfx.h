#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 MASTERVOLUME;
    extern f32 AUDIOFADELEVEL;
    extern u16 SfxBits[100];
    extern u16 GlobalSfxBits[100];
    i32 GetSfxId(char *name);
    void PlaySfxByIdEx(i32, NUVEC *, f32, f32);
    extern SFX_RECORD *SfxInfo;
    void InitSfx(VARIPTR *cursor, VARIPTR end, const char *file);
#ifdef __cplusplus
}
#endif
