#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nubridge.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nubridge.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include <stdio.h>

void fun_00543ed0(void) {
}

void NubridgeNuError(void) {
}

/* NuFabs: copies the value with the sign bit cleared into a new float. */
static __inline f32 NuBridgeFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    f32 result = *(f32 *)&bits;
    return result;
}

/* Builds a scaled frame for each rope segment; the PC build has the draw calls compiled out. */
void NuBridgeDrawRopeSegment(NUMTL *material, NUVEC *points, i32 count, i32 colour) {
    i32 i;
    NUVEC *next;
    NUVEC side;
    NUVEC up;
    NUVEC edge;
    NUVEC corner;
    NUVEC direction;

    next = points + 1;
    for (i = 0; i < count; ++i, ++points, ++next) {
        if (i == count - 1) {
            continue;
        }
        direction.x = next->x - points->x;
        direction.y = next->y - points->y;
        direction.z = next->z - points->z;
        NuVecNorm(&direction, &direction);
        if (direction.x != direction.z) {
            side.x = -direction.z;
            side.y = direction.y;
            side.z = direction.x;
        } else {
            side.x = -direction.y;
            side.y = direction.x;
            side.z = direction.z;
        }
        NuVecCross(&up, &direction, &side);
        NuVecNorm(&up, &up);
        edge.x = side.x + (up.x - side.x) * 0.5f;
        edge.y = side.y + (up.y - side.y) * 0.5f;
        edge.z = side.z + (up.z - side.z) * 0.5f;
        NuVecNorm(&edge, &edge);
        corner.x = up.x + (-side.x - up.x) * 0.5f;
        corner.y = up.y + (-side.y - up.y) * 0.5f;
        corner.z = up.z + (-side.z - up.z) * 0.5f;
        NuVecNorm(&corner, &corner);
        side.x *= 0.08f;
        side.y *= 0.08f;
        side.z *= 0.08f;
        up.x *= 0.08f;
        up.y *= 0.08f;
        up.z *= 0.08f;
        edge.x *= 0.08f;
        edge.y *= 0.08f;
        edge.z *= 0.08f;
        corner.x *= 0.08f;
        corner.y *= 0.08f;
        corner.z *= 0.08f;
    }
}

void NuBrdigeDrawRope(NUMTL *material, NUVEC *first, NUVEC *second, i32 count, i32 *boundaries, i32 colour) {
    i32 i;
    for (i = 0; boundaries[i] < boundaries[i + 1]; ++i) {
        NuBridgeDrawRopeSegment(material, &first[boundaries[i]], boundaries[i + 1] - boundaries[i], colour);
        NuBridgeDrawRopeSegment(material, &second[boundaries[i]], boundaries[i + 1] - boundaries[i], colour);
    }
}

NUBRIDGE Bridges[8] = {0};
i32 NuBridgeProc = 0;

void NuBridgeDraw(NUMTL *material) {
    i32 b;
    i32 i;
    NUBRIDGE *bridge;
    i32 n;
    i32 group;
    NUVEC4 point;
    NUVEC first[512];
    NUVEC second[512];
    NUMTX matrix;
    i32 boundaries[8]; /* boundaries[group + 1] can write one past the end with all 8 bridges */
    NUVEC4 offsets[2];
    if (NuBridgeProc) {
        bridge = Bridges;
        n = 0;
        group = 0;
        boundaries[group] = 0;
        for (b = 0; b < 8; ++b, ++bridge) {
            if (bridge->active && bridge->in_range) {
                bridge->was_drawn = 0;
                NuMtxSetRotationY(&matrix, bridge->rotation_y);
                offsets[0].x = 0.0f;
                offsets[0].y = 0.0f;
                offsets[0].z = -bridge->half_width;
                offsets[0].w = 1.0f;
                offsets[1].x = 0.0f;
                offsets[1].y = 0.0f;
                offsets[1].z = bridge->half_width;
                offsets[1].w = 1.0f;
                for (i = 0; i < bridge->platform_count; ++i) {
                    if (NuSpecialExistsFn(&bridge->special)) {
                        if (NuSpecialDrawAt(&bridge->special, &bridge->platform_matrices[i])) {
                            bridge->was_drawn = 1;
                        }
                    }
                    if (i % (i8)bridge->special_spacing == 0 && NuSpecialExistsFn(&bridge->second_special)) {
                        NuVec4MtxTransformVU0(&point, &offsets[0], &bridge->platform_matrices[i]);
                        if (n < 512) {
                            first[n].x = point.x;
                            first[n].y = point.y + bridge->height_offset;
                            first[n].z = point.z;
                        }
                        matrix.m30 = point.x;
                        matrix.m31 = point.y;
                        matrix.m32 = point.z;
                        NuSpecialDrawAt(&bridge->second_special, &matrix);
                        point.x = 0.0f;
                        point.y = 0.0f;
                        point.z = 1.0f;
                        point.w = 1.0f;
                        NuVec4MtxTransformVU0(&point, &offsets[1], &bridge->platform_matrices[i]);
                        if (n < 512) {
                            second[n].x = point.x;
                            second[n].y = point.y + bridge->height_offset;
                            second[n].z = point.z;
                            ++n;
                        }
                        matrix.m30 = point.x;
                        matrix.m31 = point.y;
                        matrix.m32 = point.z;
                        NuSpecialDrawAt(&bridge->second_special, &matrix);
                    }
                }
                if (boundaries[group] != n) {
                    ++group;
                    boundaries[group] = n;
                }
            }
        }
        boundaries[group + 1] = n;
        if (n > 1) {
            NuBrdigeDrawRope(material, first, second, n, boundaries, Bridges[0].colour);
        }
    }
}

void fun_00545530(void) {
}

void NuBridgeUpdate(NUVEC *position) {
    i32 b;
    i32 i;
    NUBRIDGE *bridge;
    f32 t;
    f32 side;
    i32 n;
    f32 left;
    f32 right;
    NUVEC delta;
    if (NuBridgeProc) {
        bridge = Bridges;
        for (b = 0; b < 8; ++b, ++bridge) {
            if (bridge->active) {
                if ((bridge->centre.x - global_camera.mtx.m30) * (bridge->centre.x - global_camera.mtx.m30) +
                        (bridge->centre.y - global_camera.mtx.m31) * (bridge->centre.y - global_camera.mtx.m31) +
                        (bridge->centre.z - global_camera.mtx.m32) * (bridge->centre.z - global_camera.mtx.m32) <
                    global_camera.far_clip * global_camera.far_clip + bridge->cull_radius_squared) {
                    bridge->in_range = 1;
                    if (bridge->was_drawn) {
                        if (bridge->contact_frames) {
                            delta.x = (bridge->edge_positions[bridge->platform_count - 1][0].x +
                                       bridge->edge_positions[bridge->platform_count - 1][1].x - bridge->edge_positions[0][0].x -
                                       bridge->edge_positions[0][1].x) *
                                      0.5f;
                            delta.z = (bridge->edge_positions[bridge->platform_count - 1][0].z +
                                       bridge->edge_positions[bridge->platform_count - 1][1].z - bridge->edge_positions[0][0].z -
                                       bridge->edge_positions[0][1].z) *
                                      0.5f;
                            t = delta.x * (position->x - (bridge->edge_positions[0][0].x + bridge->edge_positions[0][1].x) * 0.5f) +
                                delta.z * (position->z - (bridge->edge_positions[0][0].z + bridge->edge_positions[0][1].z) * 0.5f);
                            t = t * (bridge->platform_count - 1) / (delta.x * delta.x + delta.z * delta.z);
                            if (t < 0.0f) {
                                t = 0.0f;
                            }
                            if (t > bridge->platform_count - 1.0f) {
                                t = bridge->platform_count - 1.0f;
                            }
                            n = t;
                            t -= n;
                            side = 2.0f / (NuFsqrt(delta.x * delta.x + delta.z * delta.z) * bridge->width);
                            side =
                                side * (delta.z * (position->x - (bridge->edge_positions[0][0].x + bridge->edge_positions[0][1].x) * 0.5f) +
                                        delta.x * ((bridge->edge_positions[0][0].z + bridge->edge_positions[0][1].z) * 0.5f - position->z));
                            if (side > 1.0f) {
                                side = 1.0f;
                            }
                            if (side < -1.0f) {
                                side = -1.0f;
                            }
                            left = bridge->gravity * (3.0f - side) * 0.25f;
                            right = bridge->gravity * (side + 3.0f) * 0.25f;
                            if (t == 0.0f) {
                                if (n > 0 && n < bridge->platform_count - 1) {
                                    bridge->edge_motion[n][0].y = bridge->edge_motion[n][0].y + (left * bridge->load_multiplier);
                                    bridge->edge_motion[n][1].y = right * bridge->load_multiplier + bridge->edge_motion[n][1].y;
                                }
                            } else {
                                if (n > 0 && n < bridge->platform_count - 1) {
                                    bridge->edge_motion[n][0].y =
                                        bridge->edge_motion[n][0].y + (left * bridge->load_multiplier * (1.0f - t));
                                    bridge->edge_motion[n][1].y =
                                        bridge->edge_motion[n][1].y + (right * bridge->load_multiplier * (1.0f - t));
                                }
                                if (n + 1 < bridge->platform_count - 1) {
                                    bridge->edge_motion[n + 1][0].y =
                                        bridge->edge_motion[n + 1][0].y + (left * bridge->load_multiplier * t);
                                    bridge->edge_motion[n + 1][1].y =
                                        bridge->edge_motion[n + 1][1].y + (right * bridge->load_multiplier * t);
                                }
                            }
                            --bridge->contact_frames;
                        } else {
                            left = 0.0f;
                        }
                        if (PlatInstGetHit(bridge->platforms[0])) {
                            bridge->contact_frames = 5;
                        }
                        if (PlatInstGetHit(bridge->platforms[bridge->platform_count - 1])) {
                            bridge->contact_frames = 5;
                        }
                        for (i = 1; i < bridge->platform_count - 1; ++i) {
                            if (PlatInstGetHit(bridge->platforms[i])) {
                                bridge->contact_frames = 5;
                            }
                        }
                        if (bridge->contact_frames) {
                            bridge->idle_frames = 0;
                        } else if (bridge->idle_frames < 16384) {
                            ++bridge->idle_frames;
                        }
                        if (bridge->idle_frames < 180) {
                            for (i = 1; i < bridge->platform_count - 1; ++i) {
                                bridge->edge_motion[i][0].x = bridge->edge_motion[i][0].x - bridge->edge_motion[i][0].x * bridge->damping;
                                bridge->edge_motion[i][0].y =
                                    bridge->edge_motion[i][0].y + (bridge->gravity - bridge->edge_motion[i][0].y * bridge->damping);
                                bridge->edge_motion[i][0].z = bridge->edge_motion[i][0].z - bridge->edge_motion[i][0].z * bridge->damping;
                                bridge->edge_motion[i][1].x = bridge->edge_motion[i][1].x - bridge->edge_motion[i][1].x * bridge->damping;
                                bridge->edge_motion[i][1].y =
                                    bridge->edge_motion[i][1].y + (bridge->gravity - bridge->edge_motion[i][1].y * bridge->damping);
                                bridge->edge_motion[i][1].z = bridge->edge_motion[i][1].z - bridge->edge_motion[i][1].z * bridge->damping;
                                bridge->edge_motion[i][0].x =
                                    NuBridgeFabs((bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i][0].x)) *
                                        (bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i][0].x) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i][0].x) * bridge->spring_strength +
                                    bridge->edge_motion[i][0].x;
                                bridge->edge_motion[i][0].y =
                                    NuBridgeFabs((bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i][0].y)) *
                                        (bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i][0].y) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i][0].y) * bridge->spring_strength +
                                    bridge->edge_motion[i][0].y;
                                bridge->edge_motion[i][0].z =
                                    NuBridgeFabs((bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i][0].z)) *
                                        (bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i][0].z) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i][0].z) * bridge->spring_strength +
                                    bridge->edge_motion[i][0].z;
                                bridge->edge_motion[i][0].x =
                                    NuBridgeFabs((bridge->edge_positions[i + 1][0].x - bridge->edge_positions[i][0].x)) *
                                        (bridge->edge_positions[i + 1][0].x - bridge->edge_positions[i][0].x) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i + 1][0].x - bridge->edge_positions[i][0].x) * bridge->spring_strength +
                                    bridge->edge_motion[i][0].x;
                                bridge->edge_motion[i][0].y =
                                    NuBridgeFabs((bridge->edge_positions[i + 1][0].y - bridge->edge_positions[i][0].y)) *
                                        (bridge->edge_positions[i + 1][0].y - bridge->edge_positions[i][0].y) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i + 1][0].y - bridge->edge_positions[i][0].y) * bridge->spring_strength +
                                    bridge->edge_motion[i][0].y;
                                bridge->edge_motion[i][0].z =
                                    NuBridgeFabs((bridge->edge_positions[i + 1][0].z - bridge->edge_positions[i][0].z)) *
                                        (bridge->edge_positions[i + 1][0].z - bridge->edge_positions[i][0].z) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i + 1][0].z - bridge->edge_positions[i][0].z) * bridge->spring_strength +
                                    bridge->edge_motion[i][0].z;
                                bridge->edge_motion[i][1].x =
                                    NuBridgeFabs((bridge->edge_positions[i - 1][1].x - bridge->edge_positions[i][1].x)) *
                                        (bridge->edge_positions[i - 1][1].x - bridge->edge_positions[i][1].x) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i - 1][1].x - bridge->edge_positions[i][1].x) * bridge->spring_strength +
                                    bridge->edge_motion[i][1].x;
                                bridge->edge_motion[i][1].y =
                                    NuBridgeFabs((bridge->edge_positions[i - 1][1].y - bridge->edge_positions[i][1].y)) *
                                        (bridge->edge_positions[i - 1][1].y - bridge->edge_positions[i][1].y) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i - 1][1].y - bridge->edge_positions[i][1].y) * bridge->spring_strength +
                                    bridge->edge_motion[i][1].y;
                                bridge->edge_motion[i][1].z =
                                    NuBridgeFabs((bridge->edge_positions[i - 1][1].z - bridge->edge_positions[i][1].z)) *
                                        (bridge->edge_positions[i - 1][1].z - bridge->edge_positions[i][1].z) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i - 1][1].z - bridge->edge_positions[i][1].z) * bridge->spring_strength +
                                    bridge->edge_motion[i][1].z;
                                bridge->edge_motion[i][1].x =
                                    NuBridgeFabs((bridge->edge_positions[i + 1][1].x - bridge->edge_positions[i][1].x)) *
                                        (bridge->edge_positions[i + 1][1].x - bridge->edge_positions[i][1].x) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i + 1][1].x - bridge->edge_positions[i][1].x) * bridge->spring_strength +
                                    bridge->edge_motion[i][1].x;
                                bridge->edge_motion[i][1].y =
                                    NuBridgeFabs((bridge->edge_positions[i + 1][1].y - bridge->edge_positions[i][1].y)) *
                                        (bridge->edge_positions[i + 1][1].y - bridge->edge_positions[i][1].y) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i + 1][1].y - bridge->edge_positions[i][1].y) * bridge->spring_strength +
                                    bridge->edge_motion[i][1].y;
                                bridge->edge_motion[i][1].z =
                                    NuBridgeFabs((bridge->edge_positions[i + 1][1].z - bridge->edge_positions[i][1].z)) *
                                        (bridge->edge_positions[i + 1][1].z - bridge->edge_positions[i][1].z) * bridge->spring_strength *
                                        0.5f +
                                    (bridge->edge_positions[i + 1][1].z - bridge->edge_positions[i][1].z) * bridge->spring_strength +
                                    bridge->edge_motion[i][1].z;
                            }
                            for (i = 1; i < bridge->platform_count - 1; ++i) {
                                bridge->edge_positions[i][0].x = bridge->edge_positions[i][0].x + (bridge->edge_motion[i][0].x);
                                bridge->edge_positions[i][0].y = bridge->edge_positions[i][0].y + (bridge->edge_motion[i][0].y);
                                bridge->edge_positions[i][0].z = bridge->edge_positions[i][0].z + (bridge->edge_motion[i][0].z);
                                bridge->edge_positions[i][1].x = bridge->edge_positions[i][1].x + (bridge->edge_motion[i][1].x);
                                bridge->edge_positions[i][1].y = bridge->edge_positions[i][1].y + (bridge->edge_motion[i][1].y);
                                bridge->edge_positions[i][1].z = bridge->edge_positions[i][1].z + (bridge->edge_motion[i][1].z);
                                NuMtxSetRotationY(&bridge->platform_matrices[i], bridge->rotation_y);
                                left = NuFsqrt((bridge->edge_positions[i][1].x - bridge->edge_positions[i][0].x) *
                                                   (bridge->edge_positions[i][1].x - bridge->edge_positions[i][0].x) +
                                               (bridge->edge_positions[i][1].z - bridge->edge_positions[i][0].z) *
                                                   (bridge->edge_positions[i][1].z - bridge->edge_positions[i][0].z));
                                NuMtxPreRotateX(&bridge->platform_matrices[i],
                                                NuAtan2D((bridge->edge_positions[i][1].y - bridge->edge_positions[i][0].y), left));
                                left = NuFsqrt((bridge->edge_positions[i + 1][0].x + bridge->edge_positions[i + 1][1].x -
                                                bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i - 1][1].x) *
                                                   (bridge->edge_positions[i + 1][0].x + bridge->edge_positions[i + 1][1].x -
                                                    bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i - 1][1].x) +
                                               (bridge->edge_positions[i + 1][0].z + bridge->edge_positions[i + 1][1].z -
                                                bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i - 1][1].z) *
                                                   (bridge->edge_positions[i + 1][0].z + bridge->edge_positions[i + 1][1].z -
                                                    bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i - 1][1].z));
                                NuMtxPreRotateZ(&bridge->platform_matrices[i],
                                                NuAtan2D((bridge->edge_positions[i + 1][0].y + bridge->edge_positions[i + 1][1].y -
                                                          bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i - 1][1].y),
                                                         left));
                                bridge->platform_matrices[i].m30 = (bridge->edge_positions[i][0].x + bridge->edge_positions[i][1].x) * 0.5f;
                                bridge->platform_matrices[i].m31 = (bridge->edge_positions[i][0].y + bridge->edge_positions[i][1].y) * 0.5f;
                                bridge->platform_matrices[i].m32 = (bridge->edge_positions[i][0].z + bridge->edge_positions[i][1].z) * 0.5f;
                            }
                            NuMtxSetRotationY(&bridge->platform_matrices[0], bridge->rotation_y);
                            left = NuFsqrt((bridge->edge_positions[0][1].x - bridge->edge_positions[0][0].x) *
                                               (bridge->edge_positions[0][1].x - bridge->edge_positions[0][0].x) +
                                           (bridge->edge_positions[0][1].z - bridge->edge_positions[0][0].z) *
                                               (bridge->edge_positions[0][1].z - bridge->edge_positions[0][0].z));
                            NuMtxPreRotateX(&bridge->platform_matrices[0],
                                            NuAtan2D((bridge->edge_positions[0][1].y - bridge->edge_positions[0][0].y), left));
                            left = NuFsqrt((bridge->edge_positions[1][0].x + bridge->edge_positions[1][1].x -
                                            bridge->edge_positions[0][0].x - bridge->edge_positions[0][1].x) *
                                               (bridge->edge_positions[1][0].x + bridge->edge_positions[1][1].x -
                                                bridge->edge_positions[0][0].x - bridge->edge_positions[0][1].x) +
                                           (bridge->edge_positions[1][0].z + bridge->edge_positions[1][1].z -
                                            bridge->edge_positions[0][0].z - bridge->edge_positions[0][1].z) *
                                               (bridge->edge_positions[1][0].z + bridge->edge_positions[1][1].z -
                                                bridge->edge_positions[0][0].z - bridge->edge_positions[0][1].z));
                            right = (bridge->edge_positions[0][0].y + bridge->edge_positions[0][1].y + bridge->edge_positions[1][0].y +
                                     bridge->edge_positions[1][1].y) *
                                    0.25f;
                            NuMtxPreRotateZ(&bridge->platform_matrices[0],
                                            NuAtan2D(bridge->edge_positions[1][0].y + bridge->edge_positions[1][1].y - right * 2.0f, left));
                            bridge->platform_matrices[0].m30 = (bridge->edge_positions[0][0].x + bridge->edge_positions[0][1].x) * 0.5f;
                            bridge->platform_matrices[0].m31 = right;
                            bridge->platform_matrices[0].m32 = (bridge->edge_positions[0][0].z + bridge->edge_positions[0][1].z) * 0.5f;
                            NuMtxSetRotationY(&bridge->platform_matrices[bridge->platform_count - 1], bridge->rotation_y);
                            left = NuFsqrt((bridge->edge_positions[bridge->platform_count - 2][1].x -
                                            bridge->edge_positions[bridge->platform_count - 2][0].x) *
                                               (bridge->edge_positions[bridge->platform_count - 2][1].x -
                                                bridge->edge_positions[bridge->platform_count - 2][0].x) +
                                           (bridge->edge_positions[bridge->platform_count - 2][1].z -
                                            bridge->edge_positions[bridge->platform_count - 2][0].z) *
                                               (bridge->edge_positions[bridge->platform_count - 2][1].z -
                                                bridge->edge_positions[bridge->platform_count - 2][0].z));
                            // The original applies the last endpoint tilt to matrix zero.
                            NuMtxPreRotateX(&bridge->platform_matrices[0],
                                            NuAtan2D((bridge->edge_positions[bridge->platform_count - 2][1].y -
                                                      bridge->edge_positions[bridge->platform_count - 2][0].y),
                                                     left));
                            left = NuFsqrt((bridge->edge_positions[bridge->platform_count - 1][0].x +
                                            bridge->edge_positions[bridge->platform_count - 1][1].x -
                                            bridge->edge_positions[bridge->platform_count - 2][0].x -
                                            bridge->edge_positions[bridge->platform_count - 2][1].x) *
                                               (bridge->edge_positions[bridge->platform_count - 1][0].x +
                                                bridge->edge_positions[bridge->platform_count - 1][1].x -
                                                bridge->edge_positions[bridge->platform_count - 2][0].x -
                                                bridge->edge_positions[bridge->platform_count - 2][1].x) +
                                           (bridge->edge_positions[bridge->platform_count - 1][0].z +
                                            bridge->edge_positions[bridge->platform_count - 1][1].z -
                                            bridge->edge_positions[bridge->platform_count - 2][0].z -
                                            bridge->edge_positions[bridge->platform_count - 2][1].z) *
                                               (bridge->edge_positions[bridge->platform_count - 1][0].z +
                                                bridge->edge_positions[bridge->platform_count - 1][1].z -
                                                bridge->edge_positions[bridge->platform_count - 2][0].z -
                                                bridge->edge_positions[bridge->platform_count - 2][1].z));
                            right = (bridge->edge_positions[bridge->platform_count - 1][0].y +
                                     bridge->edge_positions[bridge->platform_count - 1][1].y +
                                     bridge->edge_positions[bridge->platform_count - 2][0].y +
                                     bridge->edge_positions[bridge->platform_count - 2][1].y) *
                                    0.25f;
                            NuMtxPreRotateZ(&bridge->platform_matrices[bridge->platform_count - 1],
                                            NuAtan2D(bridge->edge_positions[bridge->platform_count - 1][0].y +
                                                         bridge->edge_positions[bridge->platform_count - 1][1].y - right * 2.0f,
                                                     left));
                            bridge->platform_matrices[bridge->platform_count - 1].m30 =
                                (bridge->edge_positions[bridge->platform_count - 1][0].x +
                                 bridge->edge_positions[bridge->platform_count - 1][1].x) *
                                0.5f;
                            bridge->platform_matrices[bridge->platform_count - 1].m31 = right;
                            bridge->platform_matrices[bridge->platform_count - 1].m32 =
                                (bridge->edge_positions[bridge->platform_count - 1][0].z +
                                 bridge->edge_positions[bridge->platform_count - 1][1].z) *
                                0.5f;
                        }
                    }
                } else {
                    bridge->in_range = 0;
                }
            }
        }
    }
}

i32 NuBridgeAlloc(void) {
    i32 index;
    NUBRIDGE *bridge = Bridges;
    for (index = 0; index < 8; ++index, ++bridge) {
        if (!bridge->active) {
            return index;
        }
    }
    return -1;
}

void NuBridgeInit(void) {
    void *saved_terrain = TerrainGetCur();
    NUBRIDGE *bridge = Bridges;
    i32 i;
    i32 j;
    for (i = 0; i < 8; ++i, ++bridge) {
        if (bridge->active) {
            TerrainSetCur(bridge->terrain);
            for (j = 0; j < bridge->platform_count; ++j) {
                if (NuSpecialExistsFn(&bridge->special)) {
                    DeletePlatinst(bridge->platforms[j]);
                }
            }
            bridge->active = 0;
        }
    }
    TerrainSetCur(saved_terrain);
}

void NuBridgeRemove(i32 index) {
    void *saved_terrain;
    NUBRIDGE *bridge;
    i32 j;
    if ((u32)index > 7) {
        return;
    }
    saved_terrain = TerrainGetCur();
    bridge = &Bridges[index];
    if (bridge->active) {
        TerrainSetCur(bridge->terrain);
        for (j = 0; j < bridge->platform_count; ++j) {
            if (NuSpecialExistsFn(&bridge->special)) {
                DeletePlatinst(bridge->platforms[j]);
            }
        }
        bridge->active = 0;
    }
    TerrainSetCur(saved_terrain);
}

void NuBridgeOn(i32 enabled) {
    NuBridgeProc = enabled;
}

i32 NuBridgeCreate(NUGSCN *scene, nuhspecial_s *first, nuhspecial_s *second, NUVEC *start, NUVEC *end, f32 width, i16 rotation,
                   f32 spring_strength, f32 damping, f32 gravity, f32 load_multiplier, i32 count, f32 unknown_b04, f32 unknown_b08,
                   i32 unknown_ae7, u32 colour) {
    i32 index;
    i32 i;
    NUBRIDGE *bridge;
    NUVEC delta;
    NUVEC side;
    f32 inverse_length;

    if (count > 24) {
        printf("to many sections/n");
    }

    NuBridgeOn(1);
    index = NuBridgeAlloc();
    if (index != -1) {
        bridge = &Bridges[index];
        bridge->special = *first;
        bridge->second_special = *second;
        bridge->scene = scene;
        bridge->active = 1;
        bridge->idle_frames = 0;
        bridge->terrain = TerrainGetCur();
        bridge->platform_count = (i8)count;
        bridge->spring_strength = spring_strength;
        bridge->damping = damping;
        bridge->width = width;
        bridge->gravity = gravity;
        bridge->load_multiplier = load_multiplier;
        bridge->half_width = unknown_b04;
        bridge->height_offset = unknown_b08;
        bridge->special_spacing = (u8)unknown_ae7;
        bridge->colour = colour;

        bridge->centre.x = (end->x + start->x) * 0.5f;
        bridge->centre.y = (end->y + start->y) * 0.5f;
        bridge->centre.z = (end->z + start->z) * 0.5f;
        delta.x = end->x - start->x;
        delta.y = end->y - start->y;
        delta.z = end->z - start->z;
        bridge->cull_radius_squared =
            (delta.x * 0.5f) * (delta.x * 0.5f) + (delta.y * 0.5f) * (delta.y * 0.5f) + (delta.z * 0.5f) * (delta.z * 0.5f) + 1.0f;
        bridge->in_range = 0;

        inverse_length = 1.0f / NuFsqrt(delta.x * delta.x + delta.z * delta.z);
        side.x = -delta.z * inverse_length;
        side.y = 0.0f;
        side.z = delta.x * inverse_length;
        bridge->rotation_y = rotation;

        /* The loop is intentionally not clamped to the 24-element arrays. */
        for (i = 0; i < count; ++i) {
            if (NuSpecialExistsFn(&bridge->special)) {
                bridge->platforms[i] = (i16)NewPlatInst(&bridge->platform_matrices[i], NuSpecialGetInstanceix(&bridge->special));
            } else {
                bridge->platforms[i] = -1;
            }

            NuMtxSetIdentity(&bridge->platform_matrices[i]);
            NuMtxPreRotateY(&bridge->platform_matrices[i], bridge->rotation_y);
            bridge->platform_matrices[i].m30 = delta.x * i / (count - 1) + start->x;
            bridge->platform_matrices[i].m31 = delta.y * i / (count - 1) + start->y;
            bridge->platform_matrices[i].m32 = delta.z * i / (count - 1) + start->z;
            bridge->edge_positions[i][0].x = delta.x * i / (count - 1) + start->x - side.x * width * 0.5f;
            bridge->edge_positions[i][0].y = delta.y * i / (count - 1) + start->y;
            bridge->edge_positions[i][0].z = delta.z * i / (count - 1) + start->z - side.z * width * 0.5f;
            bridge->edge_positions[i][1].x = delta.x * i / (count - 1) + start->x + side.x * width * 0.5f;
            bridge->edge_positions[i][1].y = delta.y * i / (count - 1) + start->y;
            bridge->edge_positions[i][1].z = delta.z * i / (count - 1) + start->z + side.z * width * 0.5f;
            bridge->edge_motion[i][0].x = 0.0f;
            bridge->edge_motion[i][0].y = 0.0f;
            bridge->edge_motion[i][0].z = 0.0f;
            bridge->edge_motion[i][1].x = 0.0f;
            bridge->edge_motion[i][1].y = 0.0f;
            bridge->edge_motion[i][1].z = 0.0f;
            bridge->contact_frames = 0;
        }
    }

    return index;
}
