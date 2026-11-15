/* The animation state of a scene's specials that the save and the level progress keep. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct scene_special_progress_s { // size 0x8

    union { // size 0x4
        u32 flags;

        struct {               // size 0x4
            u32 frame : 11;    // 0x0:0
            u32 visible : 1;   // 0x0:11
            u32 playing : 1;   // 0x0:12
            u32 waiting : 1;   // 0x0:13
            u32 repeating : 1; // 0x0:14
            u32 unused : 17;   // 0x0:15
        };
    }; // 0x0

    f32 tfactor; // 0x4
} SCENESPECIALPROGRESS;

typedef struct scene_progress_s {        // size 0x2800
    SCENESPECIALPROGRESS specials[1280]; // 0x0000
} SCENEPROGRESS;
