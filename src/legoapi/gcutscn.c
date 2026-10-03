#include "saga.h"

typedef struct cut_scene_pre_end_pc_s {
    u8 unknown_00[8];
    f32 duration;
} CUT_SCENE_PRE_END_PC;
STATIC_ASSERT(offsetof(CUT_SCENE_PRE_END_PC, duration) == 8);
typedef struct cut_camera_pre_end_pc_s {
    u8 unknown_00[9];
    i8 camera_index;
} CUT_CAMERA_PRE_END_PC;
STATIC_ASSERT(offsetof(CUT_CAMERA_PRE_END_PC, camera_index) == 9);
typedef struct cut_instance_pre_end_pc_s {
    u8 unknown_00[0x58];
    CUT_SCENE_PRE_END_PC *scene;
    u8 unknown_5c[0x2c];
    u32 flags_88;
    u32 flags_8c;
    f32 current_frame;
    f32 render_frame;
    u8 unknown_98[4];
    CUT_CAMERA_PRE_END_PC *camera_instance;
} CUT_INSTANCE_PRE_END_PC;
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, scene) == 0x58);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, flags_8c) == 0x8c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, current_frame) == 0x90);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, render_frame) == 0x94);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, camera_instance) == 0x9c);

void __cdecl instNuGCutSceneEndButNotSystems(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_PRE_END_PC *instance = (CUT_INSTANCE_PRE_END_PC *)owner;
    instance->flags_88 = (instance->flags_88 & ~2U) | 0x1000;
    instance->current_frame = instance->scene->duration;
    instance->render_frame = instance->scene->duration;
    instance->flags_8c &= ~0x40U;
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}
