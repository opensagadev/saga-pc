#include "saga.h"

void __cdecl NuRndrSetParticleRotation(NUMTX *rotation) {
    NuRndr_DebrisRotMtxPtr = rotation;
}

void __cdecl NuRndrSetDebBox(NUVEC *range) {
    NUVEC size = *range;
    NUVEC corner, minimum, maximum;
    f64 right_x, right_y, right_z, up_x, up_y, up_z, forward_x, forward_y, forward_z;
    i32 i;
    if (NuRndrStopUpdate == 0)
        debris_box_camera_cache = global_camera.mtx;
    right_x = (f64)size.x * debris_box_camera_cache.m00;
    right_y = (f64)size.x * debris_box_camera_cache.m01;
    right_z = (f64)size.x * debris_box_camera_cache.m02;
    up_x = (f64)size.y * debris_box_camera_cache.m10;
    up_y = (f64)size.y * debris_box_camera_cache.m11;
    up_z = (f64)size.y * debris_box_camera_cache.m12;
    forward_x = (f64)size.z * debris_box_camera_cache.m20;
    forward_y = (f64)size.z * debris_box_camera_cache.m21;
    forward_z = (f64)size.z * debris_box_camera_cache.m22;
    corner.x =
        (f32)(((debris_box_corners[0].x * right_x + debris_box_camera_cache.m30) + debris_box_corners[0].y * up_x) +
              debris_box_corners[0].z * forward_x);
    corner.y =
        (f32)(((debris_box_corners[0].x * right_y + debris_box_camera_cache.m31) + debris_box_corners[0].y * up_y) +
              debris_box_corners[0].z * forward_y);
    corner.z =
        (f32)(((debris_box_corners[0].x * right_z + debris_box_camera_cache.m32) + debris_box_corners[0].y * up_z) +
              debris_box_corners[0].z * forward_z);
    minimum = maximum = corner;
    for (i = 1; i < 8; ++i) {
        corner.x =
            (f32)(((debris_box_corners[i].x * right_x + debris_box_camera_cache.m30) + debris_box_corners[i].y * up_x) +
                  debris_box_corners[i].z * forward_x);
        corner.y =
            (f32)(((debris_box_corners[i].x * right_y + debris_box_camera_cache.m31) + debris_box_corners[i].y * up_y) +
                  debris_box_corners[i].z * forward_y);
        corner.z =
            (f32)(((debris_box_corners[i].x * right_z + debris_box_camera_cache.m32) + debris_box_corners[i].y * up_z) +
                  debris_box_corners[i].z * forward_z);
        if (corner.x < minimum.x)
            minimum.x = corner.x;
        if (corner.y < minimum.y)
            minimum.y = corner.y;
        if (corner.z < minimum.z)
            minimum.z = corner.z;
        if (corner.x > maximum.x)
            maximum.x = corner.x;
        if (corner.y > maximum.y)
            maximum.y = corner.y;
        if (corner.z > maximum.z)
            maximum.z = corner.z;
    }
    NuRndrDebBase.x = (f32)((maximum.x + minimum.x) * 0.5 - size.x * 0.5);
    NuRndrDebBase.y = (f32)((maximum.y + minimum.y) * 0.5 - size.y * 0.5);
    NuRndrDebBase.z = (f32)((maximum.z + minimum.z) * 0.5 - size.z * 0.5);
    NuRndrDebRange = size;
    NuRndrDebRangeInv.x = 1.0f / size.x;
    NuRndrDebRangeInv.y = 1.0f / size.y;
    NuRndrDebRangeInv.z = 1.0f / size.z;
}
