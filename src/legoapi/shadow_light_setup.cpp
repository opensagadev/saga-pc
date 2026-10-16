#include "saga.h"

extern "C" {
    extern NUMTX PostShadowProjectionPc, PostShadowViewPc;
    extern NUVEC APIObjectZeroMomentumPC;
    extern f32 ShadowLightSplitScalePc, ShadowLightWarpPc, ShadowLightNearOffsetPc;
    extern f32 ShadowLightBlendPowerPc, ShadowLightFallbackHeightPc, ShadowLightEyeDistancePc;
    extern f32 ShadowLightFocusHeightPc;
    extern NUVEC ShadowLightUpPc;
    extern i32 ShadowLightSplitCountPc;
    extern NUCAMERA *EdMainCurrentCameraPc;
}
struct SHADOW_PLAYER_BOUNDS_PC {
    u8 unknown_000[0x5fc];
    NUVEC center;
    f32 radius;
};
STATIC_ASSERT(offsetof(SHADOW_PLAYER_BOUNDS_PC, center) == 0x5fc);
STATIC_ASSERT(offsetof(SHADOW_PLAYER_BOUNDS_PC, radius) == 0x608);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) + offsetof(NUMTX, m20) == 0xd8);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) + offsetof(NUMTX, m30) == 0xe8);
STATIC_ASSERT(offsetof(GAMECAMERA, target) == 0x128);

extern "C" NUCAMERA *__cdecl edmainGetCameraPc(void) {
    return EdMainCurrentCameraPc;
}
/* Native private EDI=center, stack=radius pointer. */
extern "C" i32 __cdecl ShadowAveragePlayerBoundsPc(NUVEC *center, f32 *radius) {
    *center = APIObjectZeroMomentumPC;
    *radius = -1.0f;
    i32 count = 0;
    for (i32 i = 0; i < 2; ++i) {
        GAMEOBJECT *player = static_cast<GAMEOBJECT *>(Player[i]);
        if (player != NULL && (player->flags_1fc & 0x80) != 0) {
            SHADOW_PLAYER_BOUNDS_PC *bounds = reinterpret_cast<SHADOW_PLAYER_BOUNDS_PC *>(player);
            NuVecAdd(center, center, &bounds->center);
            volatile f32 maximum = bounds->radius < *radius ? *radius : bounds->radius;
            ++count;
            *radius = maximum;
        }
    }
    if (count > 0) {
        volatile f32 scale = 1.0f / count;
        NuVecScale(center, center, scale);
    }
    return count;
}
void PC_SPECIAL_SHADOW_LIGHT::LookAtPc(NUVEC *eye, NUVEC *target, NUVEC *up) {
    volatile f32 direction_y = target->y - eye->y;
    volatile f32 direction_z = target->z - eye->z;
    shadow_direction_pc.x = target->x - eye->x;
    shadow_direction_pc.y = direction_y;
    shadow_direction_pc.z = direction_z;
    light_direction_w_pc = 0.0f;
    NuVecNorm(&shadow_direction_pc, &shadow_direction_pc);
    light_direction_w_pc = 1.0f;
    f32 position_y = eye->y;
    f32 position_z = eye->z;
    light_position_pc.x = eye->x;
    light_position_pc.y = position_y;
    light_position_pc.z = position_z;
    light_position_pc.w = 1.0f;
    NuMtxLookAtD3D(&view, eye, target, up);
}
void PC_SPECIAL_SHADOW_LIGHT::SetupCustomCameraFrustumPc(NUCAMERA *camera, const f32 *splits, i32 split_count) {
    volatile f32 aspect = 1.0f / camera->aspect;
    volatile f32 fov = ((f64)camera->fov * 180.0) / 3.1415927410125732;
    NuMtxSetPerspectiveD3D(&PostShadowProjectionPc, fov, aspect, camera->near_clip, camera->far_clip);
    NuMtxInv(&PostShadowViewPc, &camera->mtx);
    camera_position_pc.x = camera->mtx.m30;
    camera_position_pc.y = camera->mtx.m31;
    camera_position_pc.z = camera->mtx.m32;
    camera_position_pc.w = 1.0f;
    camera_forward_pc.x = camera->mtx.m20;
    camera_forward_pc.y = camera->mtx.m21;
    camera_forward_pc.z = camera->mtx.m22;
    camera_forward_pc.w = 1.0f;
    active_render_set_count = (i32)((u32)split_count - 1u);
    camera_forward_pc.w = 0.0f;
    for (i32 i = 0; i < split_count; ++i)
        split_distances_pc[i] = splits[i];
    f32 near_clip = camera->near_clip;
    f32 far_clip = camera->far_clip;
    for (i32 i = 0; i < active_render_set_count; ++i) {
        NURNDRLIGHTRECORD &set = render_sets[i];
        camera->near_clip = splits[i];
        camera->far_clip = splits[i + 1];
        PostShadowComputeFrustumCubePc(camera, set.corners_pc, &set.far_plane_pc);
        set.shadow_plane_count = PostShadowComputePlanesPc(&light_direction4_pc, set.corners_pc, set.planes_pc);
        set.view_pc = view;
        set.projection_pc = projection;
        PostShadowComputeCapsulePc(&light_direction4_pc, set.corners_pc, &set.endpoint0_pc, &set.endpoint1_pc,
                                   &set.shadow_radius_pc);
    }
    for (i32 i = 0; i < active_render_set_count; ++i) {
        split_planes_pc[i].x = 0.0f;
        split_planes_pc[i].y = 0.0f;
        split_planes_pc[i].z = 1.0f / splits[i + 1];
        split_planes_pc[i].w = -1.0f;
    }
    camera->near_clip = near_clip;
    camera->far_clip = far_clip;
}

extern "C" void __cdecl SetShadowLight(void) {
    NUVEC center, target, eye, secondary_eye, direction, camera_position;
    NUCAMERA *camera;
    f32 secondary_strength;
    if (CutStopInfo != NULL) {
        NUCAMERA *current = NuCameraGetCam();
        center.x = (f64)current->mtx.m20 * 2.0 + current->mtx.m30;
        center.y = (f64)current->mtx.m21 * 2.0 + current->mtx.m31;
        center.z = (f64)current->mtx.m22 * 2.0 + current->mtx.m32;
        ShadowLightFocusHeightPc = -2.0f;
    } else {
        if (Players_AveragePos(&center, NULL) == 0)
            return;
        ShadowLightFocusHeightPc = WORLD->shadow_editor->field_30;
    }
    if (WORLD->shadow_editor == NULL)
        return;
    target.x = center.x;
    target.y = ShadowLightFocusHeightPc + center.y;
    target.z = center.z;
    bool alternate =
        WORLD->area != NULL && (WORLD->area->flags & 0x100) != 0 && (WORLD->current_level->flags & 0x400) == 0;
    if (CutStopInfo != NULL) {
        direction = WORLD->shadow_editor->direction;
        secondary_strength = WORLD->shadow_editor->field_0c;
        eye.x = center.x - ShadowLightEyeDistancePc * direction.x;
        eye.y = center.y - ShadowLightEyeDistancePc * direction.y;
        eye.z = center.z - ShadowLightEyeDistancePc * direction.z;
        secondary_eye.x = center.x + (f64)ShadowLightEyeDistancePc * 2.0;
        secondary_eye.y = center.y + (f64)ShadowLightEyeDistancePc * 10.0;
        secondary_eye.z = center.z + (f64)ShadowLightEyeDistancePc * 2.0;
    } else if (ShadowAveragePlayerBoundsPc(&direction, &secondary_strength) != 0 && secondary_strength > 0.0f) {
        NuVecAdd(&direction, &direction, &WORLD->shadow_editor->direction);
        NuVecNorm(&direction, &direction);
        eye.x = center.x - ShadowLightEyeDistancePc * direction.x;
        eye.y = center.y - ShadowLightEyeDistancePc * direction.y;
        eye.z = center.z - ShadowLightEyeDistancePc * direction.z;
        secondary_eye.x = center.x + (ShadowLightEyeDistancePc + ShadowLightEyeDistancePc);
        secondary_eye.y = center.y + (f64)ShadowLightEyeDistancePc * 10.0;
        secondary_eye.z = center.z + (ShadowLightEyeDistancePc + ShadowLightEyeDistancePc);
    } else {
        eye.x = (f64)GameCam->position.x - 1.2000000476837158;
        eye.y = ShadowLightFallbackHeightPc;
        eye.z = (f64)GameCam->position.z - 1.5;
        secondary_eye = eye;
    }
    if (alternate)
        secondary_strength = 0.5f;
    if (CutStopInfo != NULL) {
        direction.x = GameCam->render_mtx.m20;
        direction.y = GameCam->render_mtx.m21;
        direction.z = GameCam->render_mtx.m22;
        camera_position.x = GameCam->render_mtx.m30;
        camera_position.y = GameCam->render_mtx.m31;
        camera_position.z = GameCam->render_mtx.m32;
        camera = pNuCam;
    } else if (editor_active != 0) {
        NUCAMERA *current = NuCameraGetCam();
        direction.x = current->mtx.m20;
        direction.y = current->mtx.m21;
        direction.z = current->mtx.m22;
        current = NuCameraGetCam();
        camera_position.x = current->mtx.m30;
        camera_position.y = current->mtx.m31;
        camera_position.z = current->mtx.m32;
        camera = edmainGetCameraPc();
    } else {
        NuVecSub(&direction, &GameCam->target, &GameCam->position);
        camera_position = GameCam->position;
        camera = pNuCam;
    }
    NuVecNorm(&direction, &direction);
    f64 weight = 1.0 + (f64)direction.y;
    weight = 1.0 < weight ? 1.0 : weight;
    volatile f32 distance = NuVecDist(&camera_position, &target, NULL);
    volatile f32 range = WORLD->shadow_editor->field_20;
    volatile f32 weight32 = weight;
    volatile f32 interpolation = NuPow(weight32, ShadowLightBlendPowerPc);
    if (alternate)
        range = 16.0f;
    f32 splits[3];
    i32 split_count;
    if (CutStopInfo != NULL) {
        splits[0] = pNuCam->near_clip;
        split_count = 3;
        ShadowLightSplitCountPc = split_count;
        splits[1] = pNuCam->near_clip + ShadowLightNearOffsetPc;
        splits[2] = distance + (range - distance) * interpolation;
    } else {
        splits[0] = pNuCam->near_clip;
        split_count = 2;
        ShadowLightSplitCountPc = 1;
        splits[1] = distance + (range - distance) * interpolation;
    }
    volatile f32 parameter = WORLD->shadow_editor->field_0c;
    if (alternate)
        parameter = secondary_strength;
    PCShadowCurrentLight->parameter_0 = parameter;
    parameter = WORLD->shadow_editor->field_10;
    PCShadowCurrentLight->parameter_1 = parameter;
    parameter = WORLD->shadow_editor->field_14;
    PCShadowCurrentLight->parameter_2 = parameter;
    parameter = WORLD->shadow_editor->field_1c;
    PCShadowCurrentLight->render_sets[0].parameter_100 = parameter;
    parameter = WORLD->shadow_editor->field_18;
    PCShadowCurrentLight->render_sets[0].parameter_104 = parameter;
    PCShadowCurrentLight->render_sets[0].parameter_110 = ShadowLightWarpPc;
    parameter = WORLD->shadow_editor->field_1c;
    PCShadowCurrentLight->render_sets[1].parameter_100 = parameter;
    parameter = WORLD->shadow_editor->field_18;
    PCShadowCurrentLight->render_sets[1].parameter_104 = parameter;
    PCShadowCurrentLight->render_sets[1].parameter_110 = ShadowLightWarpPc;
    parameter = splits[split_count - 1] * ShadowLightSplitScalePc;
    PCShadowCurrentLight->parameter_19 = parameter;
    parameter = splits[split_count - 1];
    PCShadowCurrentLight->parameter_20 = parameter;
    PCShadowCurrentLight->LookAtPc(&eye, &center, &ShadowLightUpPc);
    PCShadowCurrentLight->SetupCustomCameraFrustumPc(camera, splits, split_count);
    PCShadowSecondaryLight->parameter_0 = secondary_strength;
    PCShadowSecondaryLight->LookAtPc(&secondary_eye, &center, &ShadowLightUpPc);
    PCShadowSecondaryLight->SetupCustomCameraFrustumPc(camera, splits, split_count);
}

extern "C" {
#pragma data_seg(".data$ShadowLightParametersPc")
    f32 ShadowLightSplitScalePc = 0.9f, ShadowLightWarpPc = 5.0f;
    NUVEC ShadowLightUpPc = {0.0f, 1.0f, 0.0f};
    f32 ShadowLightNearOffsetPc = 1.5f, ShadowLightBlendPowerPc = 3.0f, ShadowLightFallbackHeightPc = 5.0f;
    f32 ShadowLightEyeDistancePc = 20.0f, ShadowLightFocusHeightPc = -2.0f;
#pragma data_seg()
#pragma bss_seg(".bss$ShadowLightSplitCountPc")
    i32 ShadowLightSplitCountPc = 0;
#pragma bss_seg(".bss$EdMainCurrentCameraPc")
    NUCAMERA *EdMainCurrentCameraPc = 0;
#pragma bss_seg()
}
