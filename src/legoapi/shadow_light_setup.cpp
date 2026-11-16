#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nudynamiclight_renderset_gen.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec4.h"

void PC_SPECIAL_SHADOW_LIGHT::SetupCustomCameraFrustum(NUCAMERA *camera, const f32 *splits, i32 split_count) {
    f32 aspect = 1.0f / camera->aspect;
    f32 fov = ((f64)camera->fov * 180.0) / 3.1415927410125732;
    NuMtxSetPerspectiveD3D(&PostShadowProjection, fov, aspect, camera->near_clip, camera->far_clip);
    NuMtxInv(&PostShadowView, &camera->mtx);
    // MATCH-EXCEPTION: the original reads each row into temporaries before storing, as an inlined
    // vec4 setter would (it also keeps that setter's redundant w stores, which a helper here
    // would reproduce but only with an out-of-line copy the original does not have).
    {
        f32 x = camera->mtx.m30;
        f32 y = camera->mtx.m31;
        f32 z = camera->mtx.m32;
        eye_position.x = x;
        eye_position.y = y;
        eye_position.z = z;
        eye_position.w = 1.0f;
    }
    {
        f32 x = camera->mtx.m20;
        f32 y = camera->mtx.m21;
        f32 z = camera->mtx.m22;
        camera_forward.x = x;
        camera_forward.y = y;
        camera_forward.z = z;
    }
    i32 set_count = split_count - 1;
    active_render_set_count = set_count;
    camera_forward.w = 0.0f;
    for (i32 i = 0; i < split_count; ++i) {
        split_distances[i] = splits[i];
    }
    f32 near_clip = camera->near_clip;
    f32 far_clip = camera->far_clip;
    for (i32 i = 0; i < set_count; ++i) {
        NURNDRLIGHTRECORD &set = render_sets[i];
        camera->near_clip = splits[i];
        camera->far_clip = splits[i + 1];
        PostShadowComputeFrustumCube(camera, set.corners, &set.far_plane);
        set.shadow_plane_count = PostShadowComputePlanes(&light_direction4, set.corners, set.planes);
        set.view = view;
        set.projection = projection;
        PostShadowComputeCapsule(&light_direction4, set.corners, &set.endpoint0, &set.endpoint1, &set.shadow_radius);
    }
    for (i32 i = 0; i < set_count; ++i) {
        split_planes[i].x = 0.0f;
        split_planes[i].y = 0.0f;
        split_planes[i].z = 1.0f / splits[i + 1];
        split_planes[i].w = -1.0f;
    }
    camera->near_clip = near_clip;
    camera->far_clip = far_clip;
}
