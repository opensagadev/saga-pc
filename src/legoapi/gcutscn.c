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

typedef struct CUT_PLAYER_ACTIVE_PC {
    void *clips;
    void *active;
} CUT_PLAYER_ACTIVE_PC;
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, active) == 4);
extern CUT_PLAYER_ACTIVE_PC *CutScenePlayerPc;
void *__cdecl CutScenePlayer_Active(void) {
    return CutScenePlayerPc != NULL ? CutScenePlayerPc->active : NULL;
}

typedef struct CUT_PLAYER_STATE_PC {
    NUSPECIALHANDLE special;
    u8 flags;
    u8 unknown_0d[3];
} CUT_PLAYER_STATE_PC;
STATIC_ASSERT(sizeof(CUT_PLAYER_STATE_PC) == 0x10);
STATIC_ASSERT(offsetof(CUT_PLAYER_STATE_PC, flags) == 0xc);
typedef struct CUT_PLAYER_RECORD_PC {
    u8 unknown_00[0x48];
    CUT_PLAYER_STATE_PC *states;
    u8 state_count;
} CUT_PLAYER_RECORD_PC;
STATIC_ASSERT(offsetof(CUT_PLAYER_RECORD_PC, states) == 0x48);
STATIC_ASSERT(offsetof(CUT_PLAYER_RECORD_PC, state_count) == 0x4c);
typedef struct CUT_PLAYER_INST_ANIM_PC {
    u8 unknown_00[0x4c];
    f32 time;
    u32 flags;
} CUT_PLAYER_INST_ANIM_PC;
STATIC_ASSERT(offsetof(CUT_PLAYER_INST_ANIM_PC, time) == 0x4c);
STATIC_ASSERT(offsetof(CUT_PLAYER_INST_ANIM_PC, flags) == 0x50);
void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *, i32);
void __cdecl CutScenePlayer_SetObjects(CUTINFO *owner) {
    CUT_PLAYER_RECORD_PC *cut = (CUT_PLAYER_RECORD_PC *)owner;
    i32 i;
    if (CutScenePlayerPc == NULL || CutScenePlayerPc->active == NULL)
        return;
    if (cut->states == NULL)
        return;
    for (i = 0; i < cut->state_count; ++i) {
        u8 flags = cut->states[i].flags;
        if ((flags & 1) != 0)
            NuSpecialSetVisibility(&cut->states[i].special, 1);
        else if ((flags & 2) != 0)
            NuSpecialSetVisibility(&cut->states[i].special, 0);
        else if ((flags & 4) != 0) {
            f32 time = NuSpecialGetAnimEndFrame(&cut->states[i].special);
            CUT_PLAYER_INST_ANIM_PC *animation =
                (CUT_PLAYER_INST_ANIM_PC *)NuSpecialGetInstAnim(&cut->states[i].special);
            if (animation != NULL) {
                animation->flags |= 1;
                animation->time = time;
            }
        }
    }
}
