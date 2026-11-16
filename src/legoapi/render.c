#include <stdio.h>
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nushadermtldesc_gen.h"
#include "nu2api/nustencilshadows_PC.h"

void DrawAlphaGrid(i32 rows, i32 cols, RENDER_POST_BLOOM_READER *parameters) {
    f32 inv_col = 1.0f / (cols - 1);
    f32 inv_row = 1.0f / (rows - 1);
    f32 x0 = 0.0f * inv_row * 2.0f - 1.0f;
    f32 y_start = 2.0f * (0.0f * inv_col) - 1.0f;
    f32 x1 = inv_row * 2.0f - 1.0f;
    f32 y_next = inv_col * 2.0f - 1.0f;
    f32 step_x = 2.0f * inv_row;
    f32 step_y = 2.0f * inv_col;
    f32 near_scale = 128.0f * parameters->near_scale;
    f32 far_scale = parameters->far_scale * 128.0f;
    f32 near_angle = parameters->near_angle / 180.0f;
    f32 far_angle = parameters->far_angle / 180.0f;
    f32 angle_delta = far_angle - near_angle;
    __declspec(align(16)) NUVEC4 direction;
    f32 scale_delta = far_scale - near_scale;
    NUCAMERA camera;
    f32 brightness;
    static i32 first = 1;
    static f32 saved_fov = 0;
    static f32 adjacent = 0;
    static i32 row = 0;
    static i32 col = 0;
    camera = global_camera;
    if (first) {
        f32 half_fov = 0.5f * camera.fov;
        saved_fov = camera.fov;
        first = 0;
        adjacent = 1.0f / (NuTrigTable[(i32)(half_fov * 10430.378f) >> 1 & 0x7fff] /
                           NuTrigTable[(16384 - (i32)(half_fov * -10430.378f)) >> 1 & 0x7fff]);
    }
    if (saved_fov != camera.fov) {
        f32 half_fov = camera.fov * 0.5f;
        adjacent = 1.0f / (NuTrigTable[(i32)(half_fov * 10430.378f) >> 1 & 0x7fff] /
                           NuTrigTable[(16384 - (i32)(half_fov * -10430.378f)) >> 1 & 0x7fff]);
        // The original writes the saved FOV back into this camera copy.
        camera.fov = saved_fov;
    }
    camera.mtx.m30 = 0.0f;
    camera.mtx.m31 = 0.0f;
    camera.mtx.m32 = 0.0f;
    camera.mtx.m33 = 1.0f;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    for (row = 0; row < rows - 1; ++row) {
        f32 y0;
        f32 y1;
        NuPrim2DBegin(14, 5, NULL);
        y0 = y_start;
        y1 = y_next;
        for (col = 0; col < cols; ++col) {
            {
                i32 colour;
                u32 converted;
                direction.x = x0;
                direction.y = -y0 * camera.aspect;
                direction.z = adjacent;
                NuVecNorm(&direction.xyz, &direction.xyz);
                direction.w = 1.0f;
                NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                NuVecNorm(&direction.xyz, &direction.xyz);
                direction.w = 1.0f;
                brightness = direction.y * 0.5f + 0.5f;
                if (brightness <= near_angle) {
                    brightness = near_scale * parameters->intensity;
                } else if (brightness >= far_angle) {
                    brightness = far_scale * parameters->intensity;
                } else {
                    brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                }
                if (parameters->directional) {
                    f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                    if (angle < parameters->direction_near_angle) {
                        brightness += 128.0f * parameters->direction_far_scale;
                    } else if (!(angle > parameters->direction_far_angle)) {
                        brightness += 128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                      (parameters->direction_far_angle - parameters->direction_near_angle),
                                                                  parameters->direction_bias)) *
                                                    (parameters->direction_far_scale - parameters->direction_near_scale) +
                                                parameters->direction_near_scale);
                    }
                }
                colour = ((u32)(u8)MIN(255.0f, MAX(0.0f, brightness)) | ((u32)(u8)MIN(255.0f, MAX(0.0f, brightness)) << 8) |
                          ((u32)(u8)MIN(255.0f, MAX(0.0f, brightness)) << 16) | ((u32)128 << 24));
                if (NuPrimNeedsOverbrightening != 0) {
                    converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                } else {
                    converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                }
                NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                NuPrim2DAddXYZ(x0, y0, 0.0f);
            }
            {
                i32 colour;
                u32 converted;
                direction.x = x1;
                direction.y = -y0 * camera.aspect;
                direction.z = adjacent;
                NuVecNorm(&direction.xyz, &direction.xyz);
                direction.w = 1.0f;
                NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                NuVecNorm(&direction.xyz, &direction.xyz);
                direction.w = 1.0f;
                brightness = direction.y * 0.5f + 0.5f;
                if (brightness <= near_angle) {
                    brightness = near_scale * parameters->intensity;
                } else if (brightness >= far_angle) {
                    brightness = far_scale * parameters->intensity;
                } else {
                    brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                }
                if (parameters->directional) {
                    f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                    if (angle < parameters->direction_near_angle) {
                        brightness += 128.0f * parameters->direction_far_scale;
                    } else if (!(angle > parameters->direction_far_angle)) {
                        brightness += 128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                      (parameters->direction_far_angle - parameters->direction_near_angle),
                                                                  parameters->direction_bias)) *
                                                    (parameters->direction_far_scale - parameters->direction_near_scale) +
                                                parameters->direction_near_scale);
                    }
                }
                colour = ((u32)(u8)MIN(255.0f, MAX(0.0f, brightness)) | ((u32)(u8)MIN(255.0f, MAX(0.0f, brightness)) << 8) |
                          ((u32)(u8)MIN(255.0f, MAX(0.0f, brightness)) << 16) | ((u32)128 << 24));
                if (NuPrimNeedsOverbrightening != 0) {
                    converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                } else {
                    converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                }
                NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                NuPrim2DAddXYZ(x1, y0, 0.0f);
            }
            if ((cols - 1) != col) {
                {
                    i32 colour;
                    u32 converted;
                    direction.x = x0;
                    direction.y = -y0 * camera.aspect;
                    direction.z = adjacent;
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    brightness = direction.y * 0.5f + 0.5f;
                    if (brightness <= near_angle) {
                        brightness = near_scale * parameters->intensity;
                    } else if (brightness >= far_angle) {
                        brightness = far_scale * parameters->intensity;
                    } else {
                        brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                    }
                    if (parameters->directional) {
                        f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                        if (angle < parameters->direction_near_angle) {
                            brightness += 128.0f * parameters->direction_far_scale;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                (parameters->direction_far_angle - parameters->direction_near_angle),
                                                            parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        }
                    }
                    do {
                        const u32 component = NuPrim2DAddCLR(0, 255, brightness);
                        colour = ((u32)(u8)component | ((u32)(u8)component << 8) | ((u32)(u8)component << 16) | ((u32)128 << 24));
                    } while (0);
                    if (NuPrimNeedsOverbrightening != 0) {
                        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                    } else {
                        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                    }
                    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                    NuPrim2DAddXYZ(x0, y0, 0.0f);
                }
                {
                    i32 colour;
                    u32 converted;
                    direction.x = x0;
                    direction.y = -y1 * camera.aspect;
                    direction.z = adjacent;
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    brightness = direction.y * 0.5f + 0.5f;
                    if (brightness <= near_angle) {
                        brightness = near_scale * parameters->intensity;
                    } else if (brightness >= far_angle) {
                        brightness = far_scale * parameters->intensity;
                    } else {
                        brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                    }
                    if (parameters->directional) {
                        f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                        if (angle < parameters->direction_near_angle) {
                            brightness += 128.0f * parameters->direction_far_scale;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                (parameters->direction_far_angle - parameters->direction_near_angle),
                                                            parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        }
                    }
                    do {
                        const u32 component = NuPrim2DAddCLR(0, 255, brightness);
                        colour = ((u32)(u8)component | ((u32)(u8)component << 8) | ((u32)(u8)component << 16) | ((u32)128 << 24));
                    } while (0);
                    if (NuPrimNeedsOverbrightening != 0) {
                        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                    } else {
                        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                    }
                    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                    NuPrim2DAddXYZ(x0, y1, 0.0f);
                }
                {
                    i32 colour;
                    u32 converted;
                    direction.x = x1;
                    direction.y = -y0 * camera.aspect;
                    direction.z = adjacent;
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    brightness = direction.y * 0.5f + 0.5f;
                    if (brightness <= near_angle) {
                        brightness = near_scale * parameters->intensity;
                    } else if (brightness >= far_angle) {
                        brightness = far_scale * parameters->intensity;
                    } else {
                        brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                    }
                    if (parameters->directional) {
                        f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                        if (angle < parameters->direction_near_angle) {
                            brightness += 128.0f * parameters->direction_far_scale;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                (parameters->direction_far_angle - parameters->direction_near_angle),
                                                            parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        }
                    }
                    do {
                        const u32 component = NuPrim2DAddCLR(0, 255, brightness);
                        colour = ((u32)(u8)component | ((u32)(u8)component << 8) | ((u32)(u8)component << 16) | ((u32)128 << 24));
                    } while (0);
                    if (NuPrimNeedsOverbrightening != 0) {
                        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                    } else {
                        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                    }
                    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                    NuPrim2DAddXYZ(x1, y0, 0.0f);
                }
                {
                    i32 colour;
                    u32 converted;
                    direction.x = x1;
                    direction.y = -y1 * camera.aspect;
                    direction.z = adjacent;
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    brightness = direction.y * 0.5f + 0.5f;
                    if (brightness <= near_angle) {
                        brightness = near_scale * parameters->intensity;
                    } else if (brightness >= far_angle) {
                        brightness = far_scale * parameters->intensity;
                    } else {
                        brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                    }
                    if (parameters->directional) {
                        f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                        if (angle < parameters->direction_near_angle) {
                            brightness += 128.0f * parameters->direction_far_scale;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                (parameters->direction_far_angle - parameters->direction_near_angle),
                                                            parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        }
                    }
                    do {
                        const u32 component = NuPrim2DAddCLR(0, 255, brightness);
                        colour = ((u32)(u8)component | ((u32)(u8)component << 8) | ((u32)(u8)component << 16) | ((u32)128 << 24));
                    } while (0);
                    if (NuPrimNeedsOverbrightening != 0) {
                        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                    } else {
                        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                    }
                    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                    NuPrim2DAddXYZ(x1, y1, 0.0f);
                }
                {
                    i32 colour;
                    u32 converted;
                    direction.x = x1;
                    direction.y = -y0 * camera.aspect;
                    direction.z = adjacent;
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    brightness = direction.y * 0.5f + 0.5f;
                    if (brightness <= near_angle) {
                        brightness = near_scale * parameters->intensity;
                    } else if (brightness >= far_angle) {
                        brightness = far_scale * parameters->intensity;
                    } else {
                        brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                    }
                    if (parameters->directional) {
                        f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                        if (angle < parameters->direction_near_angle) {
                            brightness += 128.0f * parameters->direction_far_scale;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                (parameters->direction_far_angle - parameters->direction_near_angle),
                                                            parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        }
                    }
                    do {
                        const u32 component = NuPrim2DAddCLR(0, 255, brightness);
                        colour = ((u32)(u8)component | ((u32)(u8)component << 8) | ((u32)(u8)component << 16) | ((u32)128 << 24));
                    } while (0);
                    if (NuPrimNeedsOverbrightening != 0) {
                        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                    } else {
                        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                    }
                    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                    NuPrim2DAddXYZ(x1, y0, 0.0f);
                }
                {
                    i32 colour;
                    u32 converted;
                    direction.x = x0;
                    direction.y = -y1 * camera.aspect;
                    direction.z = adjacent;
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    NuVecMtxTransform(&direction.xyz, &direction.xyz, &camera.mtx);
                    NuVecNorm(&direction.xyz, &direction.xyz);
                    direction.w = 1.0f;
                    brightness = direction.y * 0.5f + 0.5f;
                    if (brightness <= near_angle) {
                        brightness = near_scale * parameters->intensity;
                    } else if (brightness >= far_angle) {
                        brightness = far_scale * parameters->intensity;
                    } else {
                        brightness = ((brightness - near_angle) * scale_delta / angle_delta + near_scale) * parameters->intensity;
                    }
                    if (parameters->directional) {
                        f32 angle = (i16)(0x4000 - RenderPostASin(NuVecDot(&parameters->direction, &direction.xyz))) * 0.005493164f;
                        if (angle < parameters->direction_near_angle) {
                            brightness += 128.0f * parameters->direction_far_scale;
                        } else if (!(angle > parameters->direction_far_angle)) {
                            brightness +=
                                128.0f * ((1.0f - NuPowFast((angle - parameters->direction_near_angle) /
                                                                (parameters->direction_far_angle - parameters->direction_near_angle),
                                                            parameters->direction_bias)) *
                                              (parameters->direction_far_scale - parameters->direction_near_scale) +
                                          parameters->direction_near_scale);
                        }
                    }
                    do {
                        const u32 component = NuPrim2DAddCLR(0, 255, brightness);
                        colour = ((u32)(u8)component | ((u32)(u8)component << 8) | ((u32)(u8)component << 16) | ((u32)128 << 24));
                    } while (0);
                    if (NuPrimNeedsOverbrightening != 0) {
                        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
                    } else {
                        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
                    }
                    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
                    NuPrim2DAddXYZ(x0, y1, 0.0f);
                }
            }
            y0 += step_y;
            y1 += step_y;
        }
        NuPrim2DEnd();
        x0 += step_x;
        x1 += step_x;
    }
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

void CollectCharactersOff_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
}

void BonusTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
}

void BonusWin_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
}

/* Original source name unknown. The native entry is exactly RET; ReleaseTakeOver passes it the rider and
 * vehicle character names. */
void TakeOverReleaseDiagnostic(char *rider_name, char *object_name) {
}

/* Original source name unknown. The native entry is exactly RET; GameAnimSet_AddObject passes it the name of a
 * special whose animation has no frames. */
void GameAnimMissingAnimDiagnostic(char *name) {
}

/* Original source name unknown. The native entry is exactly RET; LoadGizmoSys passes it each gizmo type name it
 * reads, loads or skips. */
void GizmoNameDiagnostic(char *name) {
}
