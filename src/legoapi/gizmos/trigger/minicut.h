#pragma once

#include "nu2api/nucore/common.h"

/* GizMiniCut_GetOutput outputs, named after GizMiniCut_GetOutputName. */
#define MINICUT_OUTPUT_PLAYED 0
#define MINICUT_OUTPUT_PLAYING 1

#ifdef __cplusplus
extern "C" {
#endif
    struct addgizmotype_s *MiniCut_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
