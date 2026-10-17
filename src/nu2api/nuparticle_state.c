#include "saga.h"

void __cdecl NuRndrSetParticleRotation(NUMTX *rotation) {
    NuRndr_DebrisRotMtxPtr = rotation;
}

void __cdecl NuRndrSetDebBox(NUVEC *range) {
    NUVEC size;
    NUVEC corners[8], minimum, maximum, base;
    i32 i;
    if (NuRndrStopUpdate == 0)
        debris_box_camera_cache = global_camera.mtx;
    size = *range;
    minimum.x =
        (f32)(((debris_box_corners[0].x * ((f64)size.x * debris_box_camera_cache.m00) + debris_box_camera_cache.m30) +
               debris_box_corners[0].y * ((f64)size.y * debris_box_camera_cache.m10)) +
              debris_box_corners[0].z * ((f64)size.z * debris_box_camera_cache.m20));
    maximum.x = minimum.x;
    minimum.y =
        (f32)(((debris_box_corners[0].x * ((f64)size.x * debris_box_camera_cache.m01) + debris_box_camera_cache.m31) +
               debris_box_corners[0].y * ((f64)size.y * debris_box_camera_cache.m11)) +
              debris_box_corners[0].z * ((f64)size.z * debris_box_camera_cache.m21));
    maximum.y = minimum.y;
    minimum.z =
        (f32)(((debris_box_corners[0].x * ((f64)size.x * debris_box_camera_cache.m02) + debris_box_camera_cache.m32) +
               debris_box_corners[0].y * ((f64)size.y * debris_box_camera_cache.m12)) +
              debris_box_corners[0].z * ((f64)size.z * debris_box_camera_cache.m22));
    maximum.z = minimum.z;
    for (i = 1; i < 8; ++i) {
        corners[i].x = (f32)(((debris_box_corners[i].x * ((f64)size.x * debris_box_camera_cache.m00) +
                               debris_box_camera_cache.m30) +
                              debris_box_corners[i].y * ((f64)size.y * debris_box_camera_cache.m10)) +
                             debris_box_corners[i].z * ((f64)size.z * debris_box_camera_cache.m20));
        corners[i].y = (f32)(((debris_box_corners[i].x * ((f64)size.x * debris_box_camera_cache.m01) +
                               debris_box_camera_cache.m31) +
                              debris_box_corners[i].y * ((f64)size.y * debris_box_camera_cache.m11)) +
                             debris_box_corners[i].z * ((f64)size.z * debris_box_camera_cache.m21));
        corners[i].z = (f32)(((debris_box_corners[i].x * ((f64)size.x * debris_box_camera_cache.m02) +
                               debris_box_camera_cache.m32) +
                              debris_box_corners[i].y * ((f64)size.y * debris_box_camera_cache.m12)) +
                             debris_box_corners[i].z * ((f64)size.z * debris_box_camera_cache.m22));
        if (corners[i].x < minimum.x)
            minimum.x = corners[i].x;
        if (corners[i].y < minimum.y)
            minimum.y = corners[i].y;
        if (corners[i].z < minimum.z)
            minimum.z = corners[i].z;
        if (corners[i].x > maximum.x)
            maximum.x = corners[i].x;
        if (corners[i].y > maximum.y)
            maximum.y = corners[i].y;
        if (corners[i].z > maximum.z)
            maximum.z = corners[i].z;
    }
    base.x = (f32)((maximum.x + minimum.x) * 0.5 - size.x * 0.5);
    base.y = (f32)((maximum.y + minimum.y) * 0.5 - size.y * 0.5);
    base.z = (f32)((maximum.z + minimum.z) * 0.5 - size.z * 0.5);
    NuRndrDebBase = base;
    NuRndrDebRange = size;
    NuRndrDebRangeInv.x = 1.0f / size.x;
    NuRndrDebRangeInv.y = 1.0f / size.y;
    NuRndrDebRangeInv.z = 1.0f / size.z;
}
