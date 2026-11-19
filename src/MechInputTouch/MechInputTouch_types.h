#pragma once

#include "nu2api/nucore/common.h"

struct FRAME_PAD;

#ifdef __cplusplus
struct ThingProcessData;
struct ThingRenderData;
struct ThingResetData;
#endif

#ifdef __cplusplus

/* The packet ProcessThings hands every thing (NuMainUpdateFrame, Movie_CallBack). */
struct ThingProcessData {
    f32 frame_time;
    i32 paused;
    struct FRAME_PAD **pads;
    i32 controller_count;
};
#endif
