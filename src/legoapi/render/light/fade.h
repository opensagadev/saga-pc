#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"

#ifdef __cplusplus
extern "C" {
#endif
    void FadeLoop_DrawObj(f32 alpha);
    extern struct NUGSCN_s *FadeLoop_ObjScene;
    void FadeLoop_SetObj(struct NUGSCN_s *scene, char *name);
    extern nuhspecial_s FadeLoop_ObjHSpecial;
#ifdef __cplusplus
}
#endif
