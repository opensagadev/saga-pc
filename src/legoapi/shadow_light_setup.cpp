#include "saga.h"

extern "C" {
    extern NUMTX PostShadowProjectionPc, PostShadowViewPc;
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

void PC_SPECIAL_SHADOW_LIGHT::SetupCustomCameraFrustumPc(NUCAMERA *camera, const f32 *splits, i32 split_count) {
    f32 aspect = 1.0f / camera->aspect;
    f32 fov = ((f64)camera->fov * 180.0) / 3.1415927410125732;
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
    i32 set_count = split_count - 1;
    active_render_set_count = set_count;
    camera_forward_pc.w = 0.0f;
    for (i32 i = 0; i < split_count; ++i)
        split_distances_pc[i] = splits[i];
    f32 near_clip = camera->near_clip;
    f32 far_clip = camera->far_clip;
    for (i32 i = 0; i < set_count; ++i) {
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
    for (i32 i = 0; i < set_count; ++i) {
        split_planes_pc[i].x = 0.0f;
        split_planes_pc[i].y = 0.0f;
        split_planes_pc[i].z = 1.0f / splits[i + 1];
        split_planes_pc[i].w = -1.0f;
    }
    camera->near_clip = near_clip;
    camera->far_clip = far_clip;
}
