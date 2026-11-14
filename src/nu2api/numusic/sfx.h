/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 MASTERVOLUME;
    extern f32 AUDIOFADELEVEL;
    extern u16 SfxBits[100];
    extern u16 GlobalSfxBits[100];
    i32 GetSfxId(char *name);
    void PlaySfxByIdEx(i32, NUVEC *, f32, f32);
    extern SFX_RECORD_PC *SfxInfo;
    void InitSfx(VARIPTR *cursor, VARIPTR end, const char *file);
#ifdef __cplusplus
}
#endif
