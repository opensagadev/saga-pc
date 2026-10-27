#include "saga.h"

/* Retail performs these absolute-value operations inline with x87 FABS. */
#define NUBRIDGE_ABS(value) ((value) < 0.0f ? -(value) : (value))

void __cdecl NuBrdigeDrawRope(NUMTL *material, NUVEC *first, NUVEC *second, i32 count, i32 *boundaries, i32 colour) {
    i32 i;
    (void)material;
    (void)count;
    for (i = 0; boundaries[i] < boundaries[i + 1]; ++i) {
        STUBBED_NAMED("NuBridgeDrawRope: call PC helper 0x00545030 for first edge segment");
        STUBBED_NAMED("NuBridgeDrawRope: call PC helper 0x00545030 for second edge segment");
    }
    (void)first;
    (void)second;
    (void)colour;
}

void __cdecl NuBridgeDraw(NUMTL *material) {
    i32 b, i;
    NUBRIDGE_PC *bridge;
    i32 n, group;
    NUVEC4 point;
    NUVEC first[512], second[512];
    NUMTX matrix;
    i32 boundaries[9];
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
                offsets[0].z = -bridge->field_b04;
                offsets[0].w = 1.0f;
                offsets[1].x = 0.0f;
                offsets[1].y = 0.0f;
                offsets[1].z = bridge->field_b04;
                offsets[1].w = 1.0f;
                for (i = 0; i < bridge->platform_count; ++i) {
                    if (NuSpecialExistsFn(&bridge->special)) {
                        if (NuSpecialDrawAt(&bridge->special, &bridge->platform_matrices[i]))
                            bridge->was_drawn = 1;
                    }
                    if (i % (i8)bridge->field_ae7 == 0 && NuSpecialExistsFn(&bridge->second_special)) {
                        NuVec4MtxTransformVU0(&point, &offsets[0], &bridge->platform_matrices[i]);
                        if (n < 512) {
                            first[n].x = point.x;
                            first[n].y = point.y + bridge->field_b08;
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
                            second[n].y = point.y + bridge->field_b08;
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
        if (n > 1)
            NuBrdigeDrawRope(material, first, second, n, boundaries, Bridges[0].colour);
    }
}

void NuBridgeUpdate(NUVEC *position) {
    i32 b, i;
    NUBRIDGE_PC *bridge;
    f32 t, side;
    i32 n;
    f32 left, right;
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
                                       bridge->edge_positions[bridge->platform_count - 1][1].x -
                                       bridge->edge_positions[0][0].x - bridge->edge_positions[0][1].x) *
                                      0.5f;
                            delta.z = (bridge->edge_positions[bridge->platform_count - 1][0].z +
                                       bridge->edge_positions[bridge->platform_count - 1][1].z -
                                       bridge->edge_positions[0][0].z - bridge->edge_positions[0][1].z) *
                                      0.5f;
                            t = delta.x * (position->x -
                                           (bridge->edge_positions[0][0].x + bridge->edge_positions[0][1].x) * 0.5f) +
                                delta.z * (position->z -
                                           (bridge->edge_positions[0][0].z + bridge->edge_positions[0][1].z) * 0.5f);
                            t = t * (bridge->platform_count - 1) / (delta.x * delta.x + delta.z * delta.z);
                            if (t < 0.0f)
                                t = 0.0f;
                            if (t > bridge->platform_count - 1.0f)
                                t = bridge->platform_count - 1.0f;
                            n = t;
                            t -= n;
                            side = 2.0f / (NuFsqrt(delta.x * delta.x + delta.z * delta.z) * bridge->width);
                            side =
                                side *
                                (delta.z * (position->x -
                                            (bridge->edge_positions[0][0].x + bridge->edge_positions[0][1].x) * 0.5f) +
                                 delta.x * ((bridge->edge_positions[0][0].z + bridge->edge_positions[0][1].z) * 0.5f -
                                            position->z));
                            if (side > 1.0f)
                                side = 1.0f;
                            if (side < -1.0f)
                                side = -1.0f;
                            left = bridge->gravity * (3.0f - side) * 0.25f;
                            right = bridge->gravity * (side + 3.0f) * 0.25f;
                            if (t == 0.0f) {
                                if (n > 0 && n < bridge->platform_count - 1) {
                                    bridge->edge_motion[n][0].y =
                                        bridge->edge_motion[n][0].y + (left * bridge->load_multiplier);
                                    bridge->edge_motion[n][1].y =
                                        bridge->edge_motion[n][1].y + (right * bridge->load_multiplier);
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
                        if (PlatInstGetHit(bridge->platforms[0]))
                            bridge->contact_frames = 5;
                        if (PlatInstGetHit(bridge->platforms[bridge->platform_count - 1]))
                            bridge->contact_frames = 5;
                        for (i = 1; i < bridge->platform_count - 1; ++i) {
                            if (PlatInstGetHit(bridge->platforms[i]))
                                bridge->contact_frames = 5;
                        }
                        if (bridge->contact_frames)
                            bridge->idle_frames = 0;
                        else if (bridge->idle_frames < 16384)
                            ++bridge->idle_frames;
                        if (bridge->idle_frames < 180) {
                            for (i = 1; i < bridge->platform_count - 1; ++i) {
                                bridge->edge_motion[i][0].x =
                                    bridge->edge_motion[i][0].x + (-bridge->edge_motion[i][0].x * bridge->damping);
                                bridge->edge_motion[i][0].y =
                                    bridge->edge_motion[i][0].y +
                                    (bridge->gravity - bridge->edge_motion[i][0].y * bridge->damping);
                                bridge->edge_motion[i][0].z =
                                    bridge->edge_motion[i][0].z + (-bridge->edge_motion[i][0].z * bridge->damping);
                                bridge->edge_motion[i][1].x =
                                    bridge->edge_motion[i][1].x + (-bridge->edge_motion[i][1].x * bridge->damping);
                                bridge->edge_motion[i][1].y =
                                    bridge->edge_motion[i][1].y +
                                    (bridge->gravity - bridge->edge_motion[i][1].y * bridge->damping);
                                bridge->edge_motion[i][1].z =
                                    bridge->edge_motion[i][1].z + (-bridge->edge_motion[i][1].z * bridge->damping);
                                bridge->edge_motion[i][0].x =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i][0].x)) *
                                        (bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i][0].x) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i][0].x) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][0].x;
                                bridge->edge_motion[i][0].y =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i][0].y)) *
                                        (bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i][0].y) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i][0].y) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][0].y;
                                bridge->edge_motion[i][0].z =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i][0].z)) *
                                        (bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i][0].z) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i][0].z) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][0].z;
                                bridge->edge_motion[i][0].x =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i + 1][0].x - bridge->edge_positions[i][0].x)) *
                                        (bridge->edge_positions[i + 1][0].x - bridge->edge_positions[i][0].x) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i + 1][0].x - bridge->edge_positions[i][0].x) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][0].x;
                                bridge->edge_motion[i][0].y =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i + 1][0].y - bridge->edge_positions[i][0].y)) *
                                        (bridge->edge_positions[i + 1][0].y - bridge->edge_positions[i][0].y) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i + 1][0].y - bridge->edge_positions[i][0].y) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][0].y;
                                bridge->edge_motion[i][0].z =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i + 1][0].z - bridge->edge_positions[i][0].z)) *
                                        (bridge->edge_positions[i + 1][0].z - bridge->edge_positions[i][0].z) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i + 1][0].z - bridge->edge_positions[i][0].z) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][0].z;
                                bridge->edge_motion[i][1].x =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i - 1][1].x - bridge->edge_positions[i][1].x)) *
                                        (bridge->edge_positions[i - 1][1].x - bridge->edge_positions[i][1].x) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i - 1][1].x - bridge->edge_positions[i][1].x) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][1].x;
                                bridge->edge_motion[i][1].y =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i - 1][1].y - bridge->edge_positions[i][1].y)) *
                                        (bridge->edge_positions[i - 1][1].y - bridge->edge_positions[i][1].y) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i - 1][1].y - bridge->edge_positions[i][1].y) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][1].y;
                                bridge->edge_motion[i][1].z =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i - 1][1].z - bridge->edge_positions[i][1].z)) *
                                        (bridge->edge_positions[i - 1][1].z - bridge->edge_positions[i][1].z) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i - 1][1].z - bridge->edge_positions[i][1].z) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][1].z;
                                bridge->edge_motion[i][1].x =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i + 1][1].x - bridge->edge_positions[i][1].x)) *
                                        (bridge->edge_positions[i + 1][1].x - bridge->edge_positions[i][1].x) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i + 1][1].x - bridge->edge_positions[i][1].x) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][1].x;
                                bridge->edge_motion[i][1].y =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i + 1][1].y - bridge->edge_positions[i][1].y)) *
                                        (bridge->edge_positions[i + 1][1].y - bridge->edge_positions[i][1].y) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i + 1][1].y - bridge->edge_positions[i][1].y) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][1].y;
                                bridge->edge_motion[i][1].z =
                                    NUBRIDGE_ABS(
                                        (bridge->edge_positions[i + 1][1].z - bridge->edge_positions[i][1].z)) *
                                        (bridge->edge_positions[i + 1][1].z - bridge->edge_positions[i][1].z) *
                                        bridge->spring_strength * 0.5f +
                                    (bridge->edge_positions[i + 1][1].z - bridge->edge_positions[i][1].z) *
                                        bridge->spring_strength +
                                    bridge->edge_motion[i][1].z;
                            }
                            for (i = 1; i < bridge->platform_count - 1; ++i) {
                                bridge->edge_positions[i][0].x =
                                    bridge->edge_positions[i][0].x + (bridge->edge_motion[i][0].x);
                                bridge->edge_positions[i][0].y =
                                    bridge->edge_positions[i][0].y + (bridge->edge_motion[i][0].y);
                                bridge->edge_positions[i][0].z =
                                    bridge->edge_positions[i][0].z + (bridge->edge_motion[i][0].z);
                                bridge->edge_positions[i][1].x =
                                    bridge->edge_positions[i][1].x + (bridge->edge_motion[i][1].x);
                                bridge->edge_positions[i][1].y =
                                    bridge->edge_positions[i][1].y + (bridge->edge_motion[i][1].y);
                                bridge->edge_positions[i][1].z =
                                    bridge->edge_positions[i][1].z + (bridge->edge_motion[i][1].z);
                                NuMtxSetRotationY(&bridge->platform_matrices[i], bridge->rotation_y);
                                left = NuFsqrt((bridge->edge_positions[i][1].x - bridge->edge_positions[i][0].x) *
                                                   (bridge->edge_positions[i][1].x - bridge->edge_positions[i][0].x) +
                                               (bridge->edge_positions[i][1].z - bridge->edge_positions[i][0].z) *
                                                   (bridge->edge_positions[i][1].z - bridge->edge_positions[i][0].z));
                                NuMtxPreRotateX(
                                    &bridge->platform_matrices[i],
                                    NuAtan2D((bridge->edge_positions[i][1].y - bridge->edge_positions[i][0].y), left));
                                left = NuFsqrt(
                                    (bridge->edge_positions[i + 1][0].x + bridge->edge_positions[i + 1][1].x -
                                     bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i - 1][1].x) *
                                        (bridge->edge_positions[i + 1][0].x + bridge->edge_positions[i + 1][1].x -
                                         bridge->edge_positions[i - 1][0].x - bridge->edge_positions[i - 1][1].x) +
                                    (bridge->edge_positions[i + 1][0].z + bridge->edge_positions[i + 1][1].z -
                                     bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i - 1][1].z) *
                                        (bridge->edge_positions[i + 1][0].z + bridge->edge_positions[i + 1][1].z -
                                         bridge->edge_positions[i - 1][0].z - bridge->edge_positions[i - 1][1].z));
                                NuMtxPreRotateZ(
                                    &bridge->platform_matrices[i],
                                    NuAtan2D((bridge->edge_positions[i + 1][0].y + bridge->edge_positions[i + 1][1].y -
                                              bridge->edge_positions[i - 1][0].y - bridge->edge_positions[i - 1][1].y),
                                             left));
                                bridge->platform_matrices[i].m30 =
                                    (bridge->edge_positions[i][0].x + bridge->edge_positions[i][1].x) * 0.5f;
                                bridge->platform_matrices[i].m31 =
                                    (bridge->edge_positions[i][0].y + bridge->edge_positions[i][1].y) * 0.5f;
                                bridge->platform_matrices[i].m32 =
                                    (bridge->edge_positions[i][0].z + bridge->edge_positions[i][1].z) * 0.5f;
                            }
                            NuMtxSetRotationY(&bridge->platform_matrices[0], bridge->rotation_y);
                            left = NuFsqrt((bridge->edge_positions[0][1].x - bridge->edge_positions[0][0].x) *
                                               (bridge->edge_positions[0][1].x - bridge->edge_positions[0][0].x) +
                                           (bridge->edge_positions[0][1].z - bridge->edge_positions[0][0].z) *
                                               (bridge->edge_positions[0][1].z - bridge->edge_positions[0][0].z));
                            NuMtxPreRotateX(
                                &bridge->platform_matrices[0],
                                NuAtan2D((bridge->edge_positions[0][1].y - bridge->edge_positions[0][0].y), left));
                            left = NuFsqrt((bridge->edge_positions[1][0].x + bridge->edge_positions[1][1].x -
                                            bridge->edge_positions[0][0].x - bridge->edge_positions[0][1].x) *
                                               (bridge->edge_positions[1][0].x + bridge->edge_positions[1][1].x -
                                                bridge->edge_positions[0][0].x - bridge->edge_positions[0][1].x) +
                                           (bridge->edge_positions[1][0].z + bridge->edge_positions[1][1].z -
                                            bridge->edge_positions[0][0].z - bridge->edge_positions[0][1].z) *
                                               (bridge->edge_positions[1][0].z + bridge->edge_positions[1][1].z -
                                                bridge->edge_positions[0][0].z - bridge->edge_positions[0][1].z));
                            right = (bridge->edge_positions[0][0].y + bridge->edge_positions[0][1].y +
                                     bridge->edge_positions[1][0].y + bridge->edge_positions[1][1].y) *
                                    0.25f;
                            NuMtxPreRotateZ(
                                &bridge->platform_matrices[0],
                                NuAtan2D(bridge->edge_positions[1][0].y + bridge->edge_positions[1][1].y - right * 2.0f,
                                         left));
                            bridge->platform_matrices[0].m30 =
                                (bridge->edge_positions[0][0].x + bridge->edge_positions[0][1].x) * 0.5f;
                            bridge->platform_matrices[0].m31 = right;
                            bridge->platform_matrices[0].m32 =
                                (bridge->edge_positions[0][0].z + bridge->edge_positions[0][1].z) * 0.5f;
                            NuMtxSetRotationY(&bridge->platform_matrices[bridge->platform_count - 1],
                                              bridge->rotation_y);
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
                                                         bridge->edge_positions[bridge->platform_count - 1][1].y -
                                                         right * 2.0f,
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

#undef NUBRIDGE_ABS

void __cdecl NuBridgeInit(void) {
    void *saved_terrain = TerrainGetCur();
    i32 i, j;
    for (i = 0; i < 8; ++i) {
        NUBRIDGE_PC *bridge = &Bridges[i];
        if (bridge->active) {
            TerrainSetCur(bridge->terrain);
            for (j = 0; j < bridge->platform_count; ++j) {
                if (NuSpecialExistsFn(&bridge->special))
                    DeletePlatinst(bridge->platforms[j]);
            }
            bridge->active = 0;
        }
    }
    TerrainSetCur(saved_terrain);
}

void __cdecl NuBridgeRemove(i32 index) {
    void *saved_terrain;
    NUBRIDGE_PC *bridge;
    i32 j;
    if ((u32)index > 7)
        return;
    saved_terrain = TerrainGetCur();
    bridge = &Bridges[index];
    if (bridge->active) {
        TerrainSetCur(bridge->terrain);
        for (j = 0; j < bridge->platform_count; ++j) {
            if (NuSpecialExistsFn(&bridge->special))
                DeletePlatinst(bridge->platforms[j]);
        }
        bridge->active = 0;
    }
    TerrainSetCur(saved_terrain);
}

i32 __cdecl NuBridgeCreate(NUGSCN *scene, NUSPECIALHANDLE *first, NUSPECIALHANDLE *second, NUVEC *start, NUVEC *end,
                           f32 width, i16 rotation, f32 spring_strength, f32 damping, f32 gravity, f32 load_multiplier,
                           i32 count, f32 field_b04, f32 field_b08, i32 field_ae7, u32 colour) {
    i32 index;
    i32 i;
    NUBRIDGE_PC *bridge;
    NUVEC delta;
    NUVEC side;
    f32 inverse_length;

    if (count > 24)
        STUBBED_NAMED("NuBridgeCreate: retail warning print for count > 24 (0x54407e)");

    NuBridgeProc = 1;
    for (index = 0; index < 8; ++index) {
        if (!Bridges[index].active)
            break;
    }
    if (index == 8)
        return -1;

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
    bridge->field_b04 = field_b04;
    bridge->field_b08 = field_b08;
    bridge->field_ae7 = (u8)field_ae7;
    bridge->colour = colour;

    bridge->centre.x = (end->x + start->x) * 0.5f;
    bridge->centre.y = (end->y + start->y) * 0.5f;
    bridge->centre.z = (end->z + start->z) * 0.5f;
    delta.x = end->x - start->x;
    delta.y = end->y - start->y;
    delta.z = end->z - start->z;
    bridge->cull_radius_squared = (delta.x * 0.5f) * (delta.x * 0.5f) + (delta.y * 0.5f) * (delta.y * 0.5f) +
                                  (delta.z * 0.5f) * (delta.z * 0.5f) + 1.0f;
    bridge->in_range = 0;

    inverse_length = 1.0f / NuFsqrt(delta.x * delta.x + delta.z * delta.z);
    side.x = -delta.z * inverse_length;
    side.y = 0.0f;
    side.z = delta.x * inverse_length;
    bridge->rotation_y = rotation;

    /* The retail loop is intentionally not clamped to the 24-element arrays. */
    for (i = 0; i < count; ++i) {
        if (NuSpecialExistsFn(&bridge->special)) {
            bridge->platforms[i] =
                (i16)NewPlatInst(&bridge->platform_matrices[i], NuSpecialGetInstanceix(&bridge->special));
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

    return index;
}
