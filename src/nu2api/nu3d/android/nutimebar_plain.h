#ifndef SAGA_NU2API_NU3D_ANDROID_NUTIMEBAR_PLAIN_H
#define SAGA_NU2API_NU3D_ANDROID_NUTIMEBAR_PLAIN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuTimeBarSlotReset(i32 set, i32 slot);
    void _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name);
    i32 NuTimeBarCreateSetEx(VARIPTR *buffer, VARIPTR end, i32 *colours);
    i32 NuTimeBarCreateSetEx2(VARIPTR *buffer, VARIPTR end, i32 colour);
    i32 NuTimeBarCreateSet(i32 *colours);
    void NuTimeBarDestroySet(i32 set);
    u32 _NuTimeBarSlotEnd(i32 set, i32 slot);
    void NuTimeBarSlotSet(i32 set, i32 slot, i32 value);
    void NuTimeBarSlotSetName(i32 set, i32 slot, const char *name);
    void NuTimeBarSlotSetEx(i32 set, i32 slot, i32 value, const char *name);
    i32 NuTimeBarSlotLastValueMicroseconds(i32 set, i32 slot);
    i32 NuTimeBarSlotLastValue(i32 set, i32 slot);
    void NuTimeBarResetPeaks(void);
    void NuTimeBarEnable(i32 enabled);
    void NuTimeBarIndicateGpuFrameOut(i32 enabled);
    void NuTimeBarInitEx(VARIPTR *buffer, VARIPTR end);
    void NuTimeBarInit(void);
    void NuTimeBarSetRender(i32 set);
#ifdef __cplusplus
}
#endif

#endif
