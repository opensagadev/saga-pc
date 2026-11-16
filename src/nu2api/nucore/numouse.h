#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuMouseRead(void);
    u32 NuMouseReadButtons(void);
    f32 NuMouseReadXVel(void);
    f32 NuMouseReadYVel(void);
    f32 NuMouseReadZVel(void);
    f32 NuMouseReadXRel(void);
    f32 NuMouseReadYRel(void);
    f32 NuMouseReadZRel(void);
    f32 NuMouseReadX(void);
    f32 NuMouseReadY(void);
    void PcEditorCursorEnable(i32 enabled);
    void PcGetMousePosition(f32 *x, f32 *y);
    void PcSetMousePosition(f32 x, f32 y);
#ifdef __cplusplus
}
#endif
