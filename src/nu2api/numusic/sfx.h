#ifndef SAGA_NU2API_NUMUSIC_SFX_H
#define SAGA_NU2API_NUMUSIC_SFX_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/render/core/gameliball.h, nu2api/numath/nuvec.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

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

#endif
