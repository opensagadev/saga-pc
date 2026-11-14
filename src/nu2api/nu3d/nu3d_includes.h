/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"
#endif

/* 9 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

typedef struct NUANGVEC {
    NUANG x;
    NUANG y;
    NUANG z;
} NUANGVEC;

STATIC_ASSERT(sizeof(NUANGVEC) == 12);
STATIC_ASSERT(offsetof(NUANGVEC, x) == 0);
STATIC_ASSERT(offsetof(NUANGVEC, y) == 4);
STATIC_ASSERT(offsetof(NUANGVEC, z) == 8);

typedef struct nurnd_vertex2d_pc_s {
    f32 x;
    f32 y;
    f32 z;
    f32 rhw;
    u32 colour;
    f32 uv[2];
} NURND_VERTEX2D_PC;

STATIC_ASSERT(sizeof(NURND_VERTEX2D_PC) == 0x1c);
STATIC_ASSERT(offsetof(NURND_VERTEX2D_PC, x) == 0);
STATIC_ASSERT(offsetof(NURND_VERTEX2D_PC, y) == 4);
STATIC_ASSERT(offsetof(NURND_VERTEX2D_PC, z) == 8);
STATIC_ASSERT(offsetof(NURND_VERTEX2D_PC, colour) == 0x10);

#define NU_PORTAL_STACK_SIZE 0x3000

typedef struct NUGSCNROOM_s {
    i16 *instance_indices;
    NUPLANE *planes;
    i16 *portal_indices;
    i16 instance_count;
    u8 plane_count;
    u8 portal_count;

    union {
        u8 visibility_flags;
        u8 flags;
    };

    u8 unknown_11[3];
    u8 priority;
    u8 unknown_15[3];
} NUGSCNROOM;

STATIC_ASSERT(sizeof(NUGSCNROOM) == 0x18);
STATIC_ASSERT(offsetof(NUGSCNROOM, planes) == 4);
STATIC_ASSERT(offsetof(NUGSCNROOM, portal_indices) == 8);
STATIC_ASSERT(offsetof(NUGSCNROOM, instance_count) == 0xc);
STATIC_ASSERT(offsetof(NUGSCNROOM, plane_count) == 0xe);
STATIC_ASSERT(offsetof(NUGSCNROOM, portal_count) == 0xf);
STATIC_ASSERT(offsetof(NUGSCNROOM, visibility_flags) == 0x10);
STATIC_ASSERT(offsetof(NUGSCNROOM, priority) == 0x14);

typedef struct pc_display_clip_object_pc_s {
    i32 material_count;
    i32 *material_ids;
    i32 *item_indices;
} PC_DISPLAY_CLIP_OBJECT_PC;

STATIC_ASSERT(sizeof(PC_DISPLAY_CLIP_OBJECT_PC) == 0xc);
STATIC_ASSERT(offsetof(PC_DISPLAY_CLIP_OBJECT_PC, material_ids) == 4);
STATIC_ASSERT(offsetof(PC_DISPLAY_CLIP_OBJECT_PC, item_indices) == 8);

typedef struct NUPORTAL_FRUSTUM_PC {
    f32 transposed_planes[32];
    NUPLANE *planes;
    NUVEC minimum;
    NUVEC maximum;
    i16 plane_count;
    i16 room_id;
} NUPORTAL_FRUSTUM_PC;

STATIC_ASSERT(offsetof(NUPORTAL_FRUSTUM_PC, transposed_planes) == 0);
STATIC_ASSERT(offsetof(NUPORTAL_FRUSTUM_PC, minimum) == 0x84);
STATIC_ASSERT(offsetof(NUPORTAL_FRUSTUM_PC, maximum) == 0x90);

STATIC_ASSERT(sizeof(NUPORTAL_FRUSTUM_PC) == 0xa0);
STATIC_ASSERT(offsetof(NUPORTAL_FRUSTUM_PC, planes) == 0x80);
STATIC_ASSERT(offsetof(NUPORTAL_FRUSTUM_PC, plane_count) == 0x9c);
STATIC_ASSERT(offsetof(NUPORTAL_FRUSTUM_PC, room_id) == 0x9e);

typedef struct nugscn_anim_lookup_pc_s {
    u16 count;
    u16 end_frame;
    f32 *times;
    u8 *values;
} NUGSCN_ANIM_LOOKUP_PC;

typedef struct NUHGOBJ_JOINT_OVERRIDE_PC {
    NUVEC rotation;
    NUVEC translation;
    NUVEC scale_pc;
    i16 rotation_max_pc[3];
    i16 rotation_min_pc[3];
    u8 joint_index;
    u8 flags_pc;
    u8 unknown_32[2];
} NUHGOBJ_JOINT_OVERRIDE_PC;

typedef struct ANI3_SCALEMIN_PC {
    f32 scale;
    f32 minimum;
} ANI3_SCALEMIN_PC;

STATIC_ASSERT(sizeof(ANI3_SCALEMIN_PC) == 0x8);

typedef struct ANI3_HEADER_PC {
    u32 magic;
    u16 node_count;
    u16 key_count;
    u16 key_stride;
    u16 frame_count;
    u16 curve_count;
    u16 first_frame;
    u8 end_frame;
    u8 constant_index;
    u8 field_12_low;
    u8 format_flags;
    u16 next_block;
    u16 declared_end_frame;
    u8 pad_18[4];
    f32 minimum;
    f32 scale;
    ANI3_SCALEMIN_PC *scale_min;
    i16 *constants;
    u16 *curve_types;
    u8 *keys;
    u8 *node_flags;
    void *field_38;
} ANI3_HEADER_PC;

STATIC_ASSERT(offsetof(ANI3_HEADER_PC, node_count) == 0x04);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, key_count) == 0x06);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, key_stride) == 0x08);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, frame_count) == 0x0a);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, curve_count) == 0x0c);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, first_frame) == 0x0e);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, end_frame) == 0x10);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, format_flags) == 0x13);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, next_block) == 0x14);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, declared_end_frame) == 0x16);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, minimum) == 0x1c);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, scale) == 0x20);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, scale_min) == 0x24);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, constants) == 0x28);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, curve_types) == 0x2c);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, keys) == 0x30);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, node_flags) == 0x34);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, field_38) == 0x38);
STATIC_ASSERT(sizeof(ANI3_HEADER_PC) == 0x3c);

typedef union DWA_CURVE_CELL_PC {
    f32 constant;
    void *encoded;
} DWA_CURVE_CELL_PC;

typedef struct NUANIM_TIME2_PC {
    f32 time;
    f32 time_offset;
    i32 chunk;
    u32 time_mask;
    u32 time_byte;
    i32 chunk_frame;
} NUANIM_TIME2_PC;

typedef struct DWA_WEIGHT_BLOCK_PC {
    u32 unknown_00[2];
    f32 values[128];
} DWA_WEIGHT_BLOCK_PC;

STATIC_ASSERT(sizeof(DWA_CURVE_CELL_PC) == 4);
STATIC_ASSERT(sizeof(NUANIM_TIME2_PC) == 24);
STATIC_ASSERT(offsetof(NUANIM_TIME2_PC, time_mask) == 0x0c);
STATIC_ASSERT(offsetof(DWA_WEIGHT_BLOCK_PC, values) == 8);
STATIC_ASSERT(sizeof(DWA_WEIGHT_BLOCK_PC) == 0x208);
STATIC_ASSERT(offsetof(NUANIM_TIME2_PC, time) == 0);
STATIC_ASSERT(offsetof(NUANIM_TIME2_PC, time_offset) == 4);
STATIC_ASSERT(offsetof(NUANIM_TIME2_PC, chunk) == 8);
STATIC_ASSERT(offsetof(NUANIM_TIME2_PC, time_byte) == 0x10);
STATIC_ASSERT(offsetof(NUANIM_TIME2_PC, chunk_frame) == 0x14);

STATIC_ASSERT(sizeof(NUHGOBJ_JOINT_OVERRIDE_PC) == 0x34);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation) == 0);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, translation) == 0xc);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, joint_index) == 0x30);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, scale_pc) == 0x18);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation_max_pc) == 0x24);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation_min_pc) == 0x2a);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, flags_pc) == 0x31);

typedef struct nuprim_vertex_group_source_pc_s {
    u8 count;
    u8 block_count;
    u8 flags;
    u8 unknown_03;
    i8 *values;
} NUPRIM_VERTEX_GROUP_SOURCE_PC;

STATIC_ASSERT(sizeof(NUPRIM_VERTEX_GROUP_SOURCE_PC) == 8);
STATIC_ASSERT(offsetof(NUPRIM_VERTEX_GROUP_SOURCE_PC, flags) == 2);
STATIC_ASSERT(offsetof(NUPRIM_VERTEX_GROUP_SOURCE_PC, values) == 4);

typedef struct nurndrlibstate_s {
    u8 opaque_00_08[0x8];
    void *mtl;
    u8 opaque_0c_14[0x8];
    i32 tex_id;
    u8 opaque_18_1e[0x6];
    i16 konst_id;
    i32 global_id;
    i16 lights_id;
    u8 opaque_26_2c[0x6];
    i16 camera_id;
    i16 fog_id;
    u8 opaque_30_34[0x4];
} NURNDRLIBSTATE;

STATIC_ASSERT(sizeof(NURNDRLIBSTATE) == 0x34);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, mtl) == 0x08);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, tex_id) == 0x14);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, konst_id) == 0x1e);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, global_id) == 0x20);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, lights_id) == 0x24);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, camera_id) == 0x2c);
STATIC_ASSERT(offsetof(NURNDRLIBSTATE, fog_id) == 0x2e);
typedef struct GRAPHICS_RENDER_TEXTURE_PC GRAPHICS_RENDER_TEXTURE_PC;
typedef struct PC_FRAMEBUFFER_PC PC_FRAMEBUFFER_PC;

struct PC_SPECIAL_SHADOW_LIGHT;

typedef struct PC_SPECIAL_PROJECTION_ADJUST {
    f32 x_scale;
    f32 y_scale;
    f32 x_offset;
    f32 y_offset;
} PC_SPECIAL_PROJECTION_ADJUST;

STATIC_ASSERT(sizeof(PC_SPECIAL_PROJECTION_ADJUST) == 0x10);
STATIC_ASSERT(offsetof(PC_SPECIAL_PROJECTION_ADJUST, x_scale) == 0);
STATIC_ASSERT(offsetof(PC_SPECIAL_PROJECTION_ADJUST, y_scale) == 4);
STATIC_ASSERT(offsetof(PC_SPECIAL_PROJECTION_ADJUST, x_offset) == 8);
STATIC_ASSERT(offsetof(PC_SPECIAL_PROJECTION_ADJUST, y_offset) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCSpecialClipState;
    extern i32 PCSpecialActiveShadowLights;
    extern i32 PCSpecialShadowClipResultsPresent;
    extern i32 PCSpecialForcedLOD;
    extern i32 PCSpecialReflectionPass;
    extern struct PC_SPECIAL_SHADOW_LIGHT *PCSpecialShadowLights[4];
    extern u32 PCSpecialShadowClipResults[4];
    extern u32 PCWindFrameCount;

    i32 NuSpecialClipTestShadowLights(NUVEC *, NUVEC *, i32);
    /* Forward view remains local to the original material group. */
    typedef struct PC_MATERIAL_CREATE_SHADER PC_MATERIAL_CREATE_SHADER;
    extern i32 PCMaterialPoolCountPc;
    extern i32 PCShadowSurfaceSamplingEnabled;
    extern i32 PCSpecialForceMaterialCount;
    extern u8 CutSceneBoundingBoxTrackRoot;

    typedef struct FONT_LEGACY_GLYPH_PC {
        f32 u0;
        f32 v0;
        f32 u1;
        f32 v1;
        u32 unknown10;
    } FONT_LEGACY_GLYPH_PC;

    STATIC_ASSERT(sizeof(FONT_LEGACY_GLYPH_PC) == 0x14);

    extern const char RenderSplineShopStartPc[11];

    void NuBackbufferCopy(i32);
    extern i32 PCPortalsEnabled;
    extern void *NuTimeBar_SetList[16];
    extern i32 NuTimeBar_Initialised;
    extern i32 NuTimeBar_PeakReset;
    extern i32 NuTimeBar_EngineEnabled;
    extern i32 NuTimeBar_GpuFrameOutEnabled;
    extern const f64 NuTimeBarScanlineMicroseconds;
    extern const f64 NuTimeBarScanlinesPerFrame;
    extern const u32 NuTimeBar_EngineColours[7];
    extern const u32 NuTimeBar_DefaultColours[16];
    extern const f32 NuTimeBarQFontSize;
    extern const f32 NuTimeBarQFontScale;
    extern const f64 NuTimeBarSlotWidth;
    extern const f64 NuTimeBarHorizontalTop;
    extern const f32 NuTimeBarFontScale;
    extern const f64 NuTimeBarVerticalScale;
    extern const f64 NuTimeBarNegativeSlotWidth;
    extern const f64 NuTimeBarRight;
    extern const f64 NuTimeBarNameMargin;
    extern const f32 NuTimeBarHalfFontScale;
    extern const f64 NuTimeBarLinesPerSecond;
    extern i32 NuTimeBarPeakResetInterval;
    extern i32 NuTimeBarRenderPeakReset;
    extern i32 NuTimeBarHorizontalPeakReset;
    extern const f64 NuTime_RefreshRate;
    extern f32 qfnt_justify_squash;
    extern f32 qfnt_justify_stretch;
    extern i32 nuqfnt_cs_mode;
    extern const f32 NuPrimMinusTwo;
    extern i32 nused_sort_pris;
    extern i32 render_scene_index;
    void NuDisplayListBeginFrame(void);
    extern i32 nudlist_alloc_count;
    extern u32 nudlist_alloc_base;
    extern u32 nudlist_alloc_end;
    extern u32 nudlist_item_base;
    extern i32 nudlist_frame_active;
    extern u32 nudlist_field_e80;
    extern u32 nudlist_field_fc8;
    extern i32 nudlist_field_fc4;
    extern u32 nudlist_field_9d0;
    extern u32 nudlist_field_9d8;
    extern u32 nudlist_field_fe4;
    extern u32 nudlist_field_1598;
    extern i32 sceneParametersCount;
    extern i32 PCDisplayMaterialAddCount;
    extern u8 *PCDisplayMaterialReferenceCounts;
    extern u8 *PCDisplayMaterialStreamUsed;
    extern i32 PCMaterialGeometryItemIndex;
    extern i32 PCMaterialLinkItemIndex;
    extern void *PCDisplayClipSet;
    extern i32 PCDisplayClipGroupIndex;
    extern NUPLANE NuPortalCameraPlane;
    extern const char GHGJointFormatWarningPC[95];
    extern const char GHGSceneFormatWarningPC[87];
    extern const NUVEC debris_box_corners[8];
    void NuFree(void *user_data, const char *file, i32 line);
    extern u8 NuAnimDataBitCount[256];
    extern i32 NuAnimDataBitCountInitialized;
// Native SaveState copies this complete 0x150-byte working view.
// The original allocation extent remains unknown.
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipFrustumPlanesPc CameraClipPlanesPc.frustum
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipScissorPlanesPc CameraClipPlanesPc.scissor
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipNearFarPlanesPc CameraClipPlanesPc.near_far
// NOLINTNEXTLINE(readability-identifier-naming)
#define CameraNearClipPlanePc CameraClipPlanesPc.near_plane
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipAbsFrustumPlanesPc CameraClipPlanesPc.abs_frustum
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipAbsScissorPlanesPc CameraClipPlanesPc.abs_scissor
    extern i32 NuTextureCapacityPC;
    extern i32 NuTextureLoadCountPC;
    extern i32 *NuTextureLoadOrderPC;
    extern i32 nuspecial_draw_state;
    extern u8 fstackmem[NU_PORTAL_STACK_SIZE];
    extern NUPORTAL_FRUSTUM_PC **NuPortalFrustaPc;
    extern i32 *NuPortalFrustumCountPc;
    extern const NUVEC NuPortalBoundsOnePc;
    extern const NUVEC NuPortalBoundsMinusOnePc;
    extern NUVEC NuPortalWorldCameraPc;
    extern NUVEC *NuPortalOverrideCameraPc;
    extern i16 NuPortalCameraRoomPc;
    extern i16 NuPortalRoomsVisitedPc[16];
    extern i32 NuPortalDebugDrawPc;
    extern NUPLANE NuPortalNearPlanePc;
    extern i32 NuPortalDebugEnabledPc;
    extern i32 NuPortalCameraSelectPc;
    void NuPause(i32 paused);
    extern i32 NuCameraMode;
    extern i32 NuCameraClipScissorToViewport;
    extern i32 NuCameraPreviousClipScissorToViewport;
    extern i32 NuCameraOrthographic;
    extern i32 vport_inval;
    extern f32 NuCameraProjectionFarClipBias;
    extern const u8 CurveGroupMasks[4];
    extern const u8 KeyStructSizes[16];
    extern i32 cubeEdgeIndices[12][2];
    extern i32 cubeEdgeTopologies[12][2];
    void NuRndrSphere(NUVEC *centre, f32 radius, i32 colour, f32 vertical_scale);
    void NuRndrSphereEx(NUVEC *centre, f32 radius, i32 colour, f32 vertical_scale, i32 angle_step);
    void NuTexAnimProgSysInit(void);
    extern NUVEC ANI3_EVAL_SCALE_ARRAY[256];
    extern i32 ddmaxjoints;
    extern i32 NumQuatPushes;
    extern u8 AnimQuatSavedPc[4];
    extern NUPRIM_VERTEX_GROUP_SOURCE_PC *NuPrimVertexGroupSource;
    void NuTexAnimRemoveList(void *);
    i32 NuTexGetUnresolvedTextureTIDPS(void);
    extern i32 PCDisplayMaterialDeleteCount;
#define PCDisplayMaterialRegistry (PCDynamicDisplayScene.display_materials) // NOLINT(readability-identifier-naming)
#define PCDisplayMaterialCount (PCDynamicDisplayScene.material_count)       // NOLINT(readability-identifier-naming)
    extern u32 numtl_renderplane;
    extern const char *PCDynamicDisplaySceneNamePointer;
    extern i32 VisibilityEvaluateEnabledPc;
    extern i32 VisibilityOcclusionEnabledPc;
    extern i32 do_InstTree;
    extern i32 VisibilityCameraLockPc;
    extern i32 VisibilityLoadedOcclusionPc;
    extern i32 VisibilityUsingOcclusionPc;
    extern i32 VisibilityCurrentViewBoxPc;
    extern NUPLANE VisibilityDepthPlanePc;
    extern NUPLANE VisibilityAbsoluteDepthPlanePc;
    extern u8 cacheValues[256];
    extern u8 OcclusionBitArray[0x320];
#define VISIBILITY_ALTERNATE_WIDTH_PC zxs
#define VISIBILITY_OCCLUSION_OUTPUT_PC OcclusionBitArray
    void NuCameraLock(i32);
    extern i32 DisplaySceneRenderDisabledPc;
    extern i32 noscenespecials;
    extern i32 VisibilityPortalFilterEnabledPc;

    typedef struct render_post_bloom_reader_pc_s {
        u8 unknown_00[4];
        f32 near_angle;
        f32 near_scale;
        f32 far_angle;
        f32 far_scale;
        f32 intensity;
        i32 clear_screen;
        f32 blur_iterations;
        f32 blend;
        f32 threshold;
        i32 directional;
        NUVEC direction;
        f32 direction_near_angle;
        f32 direction_far_scale;
        f32 direction_far_angle;
        f32 direction_near_scale;
        f32 direction_bias;
    } RENDER_POST_BLOOM_READER_PC;

    STATIC_ASSERT(sizeof(RENDER_POST_BLOOM_READER_PC) == 0x4c);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, near_angle) == 4);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, near_scale) == 8);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, far_angle) == 0xc);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, far_scale) == 0x10);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, intensity) == 0x14);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, clear_screen) == 0x18);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, directional) == 0x28);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, direction) == 0x2c);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, direction_near_angle) == 0x38);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, direction_far_scale) == 0x3c);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, direction_far_angle) == 0x40);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, direction_near_scale) == 0x44);
    STATIC_ASSERT(offsetof(RENDER_POST_BLOOM_READER_PC, direction_bias) == 0x48);
    i16 RenderPostASinPc(f32);
    void DrawAlphaGrid(i32, i32, RENDER_POST_BLOOM_READER_PC *);

    void NuTexAnimSetSignals(u32 signals);
    void NuTexAnimProcess(f32 frame_time);

    typedef struct render_post_viewport_prefix_pc_s {
        f32 source_x;
        f32 source_y;
        f32 source_right;
        f32 source_bottom;
        f32 dest_x;
        f32 dest_y;
        f32 dest_right;
        f32 dest_bottom;
        f32 width_scale;
        f32 height_scale;
        f32 x_offset;
        f32 y_offset;
    } RENDER_POST_VIEWPORT_PREFIX_PC;

    STATIC_ASSERT(sizeof(RENDER_POST_VIEWPORT_PREFIX_PC) == 0x30);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_x) == 0);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_y) == 4);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_right) == 8);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_bottom) == 0xc);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_x) == 0x10);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_y) == 0x14);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_right) == 0x18);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_bottom) == 0x1c);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, width_scale) == 0x20);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, height_scale) == 0x24);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, x_offset) == 0x28);
    STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, y_offset) == 0x2c);

#define RENDER_POST_ALPHA_CACHE_PC cacheValues

    /* Verified game-local light views may remain source-local: light_data4d8/current61c. No new shared fields or
     * storage.
     */

    typedef struct NUANIM_LEGACY_KEY_PC {
        f32 time;
        f32 reciprocal_span;
        f32 tangent;
        f32 value;
    } NUANIM_LEGACY_KEY_PC;

    typedef struct NUANIM_LEGACY_CURVE_PC {
        u8 key_mask[4];
        NUANIM_LEGACY_KEY_PC *keys;
        i32 key_count;
        u32 flags;
    } NUANIM_LEGACY_CURVE_PC;

    typedef struct NUANIM_LEGACY_SET_PC {
        u32 flags;
        f32 *constants;
        NUANIM_LEGACY_CURVE_PC **curves;
        i8 curve_count;
        u8 unknown_0d[3];
    } NUANIM_LEGACY_SET_PC;

    typedef struct NUANIM_LEGACY_HEADER_PC {
        f32 duration;
        u32 unknown_04;
        i32 chunk_count;
        void **chunks;
    } NUANIM_LEGACY_HEADER_PC;

    STATIC_ASSERT(sizeof(NUANIM_LEGACY_KEY_PC) == 0x10);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_KEY_PC, reciprocal_span) == 4);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_KEY_PC, tangent) == 8);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_KEY_PC, value) == 12);
    STATIC_ASSERT(sizeof(NUANIM_LEGACY_CURVE_PC) == 0x10);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_CURVE_PC, keys) == 4);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_CURVE_PC, key_count) == 8);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_CURVE_PC, flags) == 12);
    STATIC_ASSERT(sizeof(NUANIM_LEGACY_SET_PC) == 0x10);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_SET_PC, constants) == 4);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_SET_PC, curves) == 8);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_SET_PC, curve_count) == 12);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_HEADER_PC, duration) == 0);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_HEADER_PC, chunk_count) == 8);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_HEADER_PC, chunks) == 12);

    STATIC_ASSERT(offsetof(NUANIM_LEGACY_KEY_PC, time) == 0);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_CURVE_PC, key_mask) == 0);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_SET_PC, flags) == 0);
    STATIC_ASSERT(sizeof(NUANIM_LEGACY_HEADER_PC) == 0x10);

    typedef struct NUANIM_LEGACY_CHUNK_PC {
        i32 node_count;
        u32 unknown_04;
        NUANIM_LEGACY_SET_PC **curve_sets;
    } NUANIM_LEGACY_CHUNK_PC;

    STATIC_ASSERT(offsetof(NUANIM_LEGACY_CHUNK_PC, node_count) == 0);
    STATIC_ASSERT(offsetof(NUANIM_LEGACY_CHUNK_PC, curve_sets) == 8);

    STATIC_ASSERT(sizeof(NUANIM_LEGACY_CHUNK_PC) == 0xc);
    void NuAnimDataCalcTime(NUANIM_LEGACY_HEADER_PC *, f32, NUANIM_TIME2_PC *);
    DWA_WEIGHT_BLOCK_PC **NuHGobjEvalDwa(i32, i16 *, NUANIM_LEGACY_HEADER_PC *, f32);
    extern f32 FrameMaterialAnimationSpeedPc;
    void NuDisplayListAnimateMtls(f32 frame_time);
    extern i32 nuspecial_vertex_noffsets;
    extern f32 (*nuspecial_vertex_offsets)[4];
    extern u32 NuPostEffectInitialisedFlagsPc;
    extern char *PCDisplayCaptureCursorPc;
    extern char *nudlist_field_18b0;
    extern char PCDisplayCaptureBufferPc[0x1000];
    i32 NuTexGetWidth(i32 id);
    i32 NuTexGetHeight(i32 id);
    extern i32 NuPostEffectBackBufferFramePc;
#ifdef __cplusplus
}
#endif

typedef void (*PC_DISPLAY_ITEM_HANDLER_PC)(void *);
STATIC_ASSERT(sizeof(PC_DISPLAY_ITEM_HANDLER_PC) == 4);
/* Working accessed-field views only, not original allocation extents.
 * Pointer-only parameter types remain opaque until an actual read is consumed. */
typedef struct NU_POST_BLOOM_PARAMETERS_PC NU_POST_BLOOM_PARAMETERS_PC;

struct NU_POST_BLOOM_PARAMETERS_PC {
    i32 enabled;
    f32 near_angle;
    f32 near_scale;
    f32 far_angle;
    f32 far_scale;
    f32 intensity;
    f32 unknown_18;
    f32 blur_iterations;
    f32 blend;
    f32 threshold;
    i32 directional;
    NUVEC direction;
    f32 direction_near_angle;
    f32 direction_far_scale;
    f32 direction_far_angle;
    f32 direction_near_scale;
    f32 direction_bias;
    f32 unknown_4c;
    f32 unknown_50;
};

STATIC_ASSERT(sizeof(NU_POST_BLOOM_PARAMETERS_PC) == 0x54);
STATIC_ASSERT(offsetof(NU_POST_BLOOM_PARAMETERS_PC, intensity) == 0x14);
STATIC_ASSERT(offsetof(NU_POST_BLOOM_PARAMETERS_PC, directional) == 0x28);
STATIC_ASSERT(offsetof(NU_POST_BLOOM_PARAMETERS_PC, direction) == 0x2c);
STATIC_ASSERT(offsetof(NU_POST_BLOOM_PARAMETERS_PC, direction_bias) == 0x48);

#ifdef __cplusplus
struct PC_POST_DATA_PORT_ENTRY {
    void *data;
    char name[32];
    i32 references;

    bool fun_005c2090(const char *other);
    void fun_005c20b0(const char *other, void *value);
    void fun_005c20e0();
    i32 fun_005c20f0();
    void fun_005c2100(void *value);
    void *fun_005c2110();
};

struct PC_POST_DATA_PORT_MANAGER {
    PC_POST_DATA_PORT_ENTRY entries[256];
    PC_POST_DATA_PORT_MANAGER();
    PC_POST_DATA_PORT_ENTRY *fun_005c2120(i32 index);
    void *fun_005c2160(i32 index);
    ~PC_POST_DATA_PORT_MANAGER();
    i32 Register(const char *name, void *data);
};

struct PC_POST_DATA_PORT {
    i32 index;
    PC_POST_DATA_PORT_MANAGER *manager;

    ~PC_POST_DATA_PORT() {
        if (index >= 0) {
            PC_POST_DATA_PORT_ENTRY *entry = &manager->entries[index];
            --entry->references;
        }
    }
};

STATIC_ASSERT(sizeof(PC_POST_DATA_PORT_ENTRY) == 40);
STATIC_ASSERT(offsetof(PC_POST_DATA_PORT_ENTRY, name) == 4);
STATIC_ASSERT(offsetof(PC_POST_DATA_PORT_ENTRY, references) == 36);
STATIC_ASSERT(sizeof(PC_POST_DATA_PORT_MANAGER) == 0x2800);
STATIC_ASSERT(sizeof(PC_POST_DATA_PORT) == 8);
STATIC_ASSERT(offsetof(PC_POST_DATA_PORT, manager) == 4);
extern "C" PC_POST_DATA_PORT PostPortOutFramebufferPc;
extern "C" PC_POST_DATA_PORT PostPortColorBufferPc;
extern "C" PC_POST_DATA_PORT PostPortNormalBufferPc;
extern "C" PC_POST_DATA_PORT PostPortVelocityBufferPc;
extern "C" PC_POST_DATA_PORT PostPortDepthRTBufferPc;
extern "C" PC_POST_DATA_PORT PostPortDepthBufferPc;
#endif

typedef struct NU_POST_DOF_PARAMETERS_PC {
    i32 enabled;
    f32 strength;
    f32 near_distance;
    f32 far_distance;
    i32 mode;
    i32 bias;
} NU_POST_DOF_PARAMETERS_PC;

STATIC_ASSERT(offsetof(NU_POST_DOF_PARAMETERS_PC, strength) == 4);
STATIC_ASSERT(offsetof(NU_POST_DOF_PARAMETERS_PC, bias) == 0x14);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 NuPostEffectsRanPc;
    extern u8 NuPostLastFrameEffectPc;
    extern f32 aiEditor_LocatorWidth;
    void NuLightSpeedBlur(i32 reuse_camera, f32 scale);
    void NuSpeedBlurSetMotionFactors(f32 pan, f32 pull, f32 clamp);
    extern i32 kPRIM_COORD_LOOKUP[3];
#ifdef __cplusplus
    void NuRndrEndFrameActionPc(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuDeferredShadingRender(void);
    extern i32 PostShadowCubeFaceIndicesPc[6][4];
    extern f32 PostShadowPlaneExtrusionPc;
    extern f32 PostBlurHighInputBiasPc;
    extern f32 PostBlurHighOutputBiasPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

typedef struct nutexdim_pc_s {
    i32 width;
    i32 height;
    u8 checksum[0x10];
    i32 ref_count;
    void *image_data;
    u32 image_size;
    PC_TEXTURE_RESOURCE_PC *resource;
} NUTEXDIM_PC;

STATIC_ASSERT(offsetof(NUTEXDIM_PC, width) == 0);
STATIC_ASSERT(offsetof(NUTEXDIM_PC, height) == 4);
STATIC_ASSERT(offsetof(NUTEXDIM_PC, ref_count) == 0x18);
STATIC_ASSERT(offsetof(NUTEXDIM_PC, resource) == 0x24);
STATIC_ASSERT(offsetof(NUTEXDIM_PC, image_data) == 0x1c);
STATIC_ASSERT(offsetof(NUTEXDIM_PC, image_size) == 0x20);
STATIC_ASSERT(sizeof(NUTEXDIM_PC) == 0x28);

typedef struct nuvpcurrent_s {
    union {
        PC_VIEWPORT_PC viewport;

        struct {
            i32 x;
            i32 y;
            i32 width;
            i32 height;
            f32 min_z;
            f32 max_z;
        };
    };

    f32 center_x;
    f32 center_y;
    f32 clip_min_x;
    f32 clip_min_y;
    f32 clip_max_x;
    f32 clip_max_y;
    f32 projection_scale_x;
    f32 projection_scale_y;
    f32 scissor_scale_x;
    f32 scissor_scale_y;
} NUVPCURRENT;

STATIC_ASSERT(sizeof(NUVPCURRENT) == 0x40);
STATIC_ASSERT(offsetof(NUVPCURRENT, x) == 0x0);
STATIC_ASSERT(offsetof(NUVPCURRENT, y) == 0x4);
STATIC_ASSERT(offsetof(NUVPCURRENT, width) == 0x8);
STATIC_ASSERT(offsetof(NUVPCURRENT, height) == 0xc);
STATIC_ASSERT(offsetof(NUVPCURRENT, min_z) == 0x10);
STATIC_ASSERT(offsetof(NUVPCURRENT, max_z) == 0x14);
STATIC_ASSERT(offsetof(NUVPCURRENT, center_x) == 0x18);
STATIC_ASSERT(offsetof(NUVPCURRENT, center_y) == 0x1c);
STATIC_ASSERT(offsetof(NUVPCURRENT, clip_min_x) == 0x20);
STATIC_ASSERT(offsetof(NUVPCURRENT, clip_min_y) == 0x24);
STATIC_ASSERT(offsetof(NUVPCURRENT, clip_max_x) == 0x28);
STATIC_ASSERT(offsetof(NUVPCURRENT, clip_max_y) == 0x2c);
STATIC_ASSERT(offsetof(NUVPCURRENT, projection_scale_x) == 0x30);
STATIC_ASSERT(offsetof(NUVPCURRENT, projection_scale_y) == 0x34);
STATIC_ASSERT(offsetof(NUVPCURRENT, scissor_scale_x) == 0x38);
STATIC_ASSERT(offsetof(NUVPCURRENT, scissor_scale_y) == 0x3c);
#ifdef __cplusplus
extern "C" {
#endif
    void NuQFntDestroy(VUFNT *);
    extern NUTEXDIM_PC **NuTextureDimensions;
    extern NUVPCURRENT vpCurrent;
    void NuDisplayListStreamAlloc(VARIPTR *cursor, i32 end_unused, i32 count);
    extern VARIPTR PCDisplayMaterialStreamStart;
    extern VARIPTR PCDisplayMaterialStreamEnd;
    NUTEXDIM_PC *NuTextureGetDimensions(i32);
    extern NULSTHDR *g_overrideList;
    extern VARIPTR fstack;
    extern VARIPTR fstack_top;
    extern VARIPTR fstack_limit;
    void NuDisplayListCreateDynMtlList_candidate(VARIPTR *, VARIPTR);

    void NuRndrArenaInitPc(VARIPTR *, VARIPTR);
    i32 NuTexRegisterDimensionsPc(NUTEXDIM_PC *texture);

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct POST_SHADER_PROGRAM_PC {
    PC_TEXTURE_RESOURCE_PC *vertex_shader;
    PC_TEXTURE_RESOURCE_PC *pixel_shader;
    u32 unknown_08[2];
};

STATIC_ASSERT(sizeof(POST_SHADER_PROGRAM_PC) == 0x10);
#endif
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 3 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

typedef struct nuspecial_handle_s {
    void *scene;
    NUMTX *special;
    NUMTX *display_special;
} NUSPECIALHANDLE;

STATIC_ASSERT(sizeof(NUSPECIALHANDLE) == 0xc);
STATIC_ASSERT(offsetof(NUSPECIALHANDLE, special) == 4);
STATIC_ASSERT(offsetof(NUSPECIALHANDLE, display_special) == 8);

typedef struct nuspecial_legacy_pc_s {
    NUMTX matrix;
    struct nuinstance_s *instance;
    char *name;
    u32 flags;
} NUSPECIAL_LEGACY_PC;

STATIC_ASSERT(sizeof(NUSPECIAL_LEGACY_PC) == 0x4c);
STATIC_ASSERT(offsetof(NUSPECIAL_LEGACY_PC, name) == 0x44);

typedef struct nudisplay_special_pc_s {
    NUMTX instance_matrix;
    NUMTX draw_matrix;
    NUVEC4 bounds_min;
    NUVEC4 bounds_max;
    NUVEC center;
    f32 radius;
    void *clip_objects;
    char *name;
    u32 flags;
    f32 *clip_range;
    u32 instance_ix;
    struct nuinstanim_s *instance_animation;
    u8 unknown_00c8[8];
} NUDISPLAY_SPECIAL_PC;

STATIC_ASSERT(sizeof(NUDISPLAY_SPECIAL_PC) == 0xd0);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, bounds_min) == 0x80);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, bounds_max) == 0x90);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, name) == 0xb4);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, flags) == 0xb8);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, instance_ix) == 0xc0);

typedef struct nuinstance_s {
    NUMTX matrix;
    i16 object_index;
    u16 pad_42;

    union {
        u32 flags;

        struct {
            u32 visible : 1;
            u32 flags_low : 2;
            u32 no_visibility_test : 1;
            u32 flags_high : 28;
        };
    };
    struct nuinstanim_s *animation;
    u32 pad_4c;
} NUINSTANCE;

typedef struct ANI3_JOINT_PC {
    NUVEC4 translation;
    NUVEC4 rotation;
    NUVEC4 scale;
} ANI3_JOINT_PC;

STATIC_ASSERT(offsetof(ANI3_JOINT_PC, rotation) == 0x10);
STATIC_ASSERT(offsetof(ANI3_JOINT_PC, scale) == 0x20);
STATIC_ASSERT(sizeof(ANI3_JOINT_PC) == 0x30);

typedef struct ANI3_BUFFER_PC {
    i32 joint_count;
    i16 max_joints;
    u8 use_quaternions;
    u8 pad_07;
    ANI3_JOINT_PC *joints;
    u8 *joint_flags;
} ANI3_BUFFER_PC;

STATIC_ASSERT(offsetof(ANI3_BUFFER_PC, use_quaternions) == 0x06);
STATIC_ASSERT(offsetof(ANI3_BUFFER_PC, joints) == 0x08);
STATIC_ASSERT(offsetof(ANI3_BUFFER_PC, joint_flags) == 0x0c);
STATIC_ASSERT(sizeof(ANI3_BUFFER_PC) == 0x10);

typedef struct ANI3_GLOBALBUFFER_PC {
    union {
        u8 unknown_00[4];
        i32 joint_count;
    };

    i16 scratch_joint_capacity;
    u8 unknown_06[2];

    union {
        f32 *animation_values;
        ANI3_JOINT_PC *joints;
    };

    u8 *joint_flags;
} ANI3_GLOBALBUFFER_PC;

STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, joint_count) == 0);
STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, scratch_joint_capacity) == 4);
STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, unknown_06) == 6);
STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, animation_values) == 8);
STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, joints) == 8);
STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, joint_flags) == 0xc);
STATIC_ASSERT(sizeof(ANI3_GLOBALBUFFER_PC) == 0x10);

typedef void (*ANI3_ROOT_CALLBACK_PC)(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
typedef void (*ANI3_EVAL_CALLBACK_PC)(NUMTX *, void *, NUQUAT *);

typedef struct NUHGOBJ_EVAL_JOINT_PC {
    NUMTX bind_matrix;
    NUVEC pivot_pc;
    u8 unknown_4c_4f[4];
    u8 parent_index;
    u8 flags_pc;
    u8 unknown_52_5f[14];
} NUHGOBJ_EVAL_JOINT_PC;

STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_JOINT_PC, bind_matrix) == 0);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_JOINT_PC, parent_index) == 0x50);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_JOINT_PC, pivot_pc) == 0x40);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_JOINT_PC, flags_pc) == 0x51);
STATIC_ASSERT(sizeof(NUHGOBJ_EVAL_JOINT_PC) == 0x60);

typedef struct NUHGOBJ_RENDER_PART_PC {
    const char *name;
    NUSPECIALHANDLE **rigid;
    NUSPECIALHANDLE *smooth;
    NUSPECIALHANDLE **alternate_rigid;
    NUSPECIALHANDLE *alternate_smooth;
} NUHGOBJ_RENDER_PART_PC;

STATIC_ASSERT(sizeof(NUHGOBJ_RENDER_PART_PC) == 0x14);
STATIC_ASSERT(offsetof(NUHGOBJ_RENDER_PART_PC, name) == 0);

typedef struct NUHGOBJ_POI_PC {
    NUMTX matrix;
    u8 unknown_40[4];
    u8 joint_index;
    u8 field_45[11];
} NUHGOBJ_POI_PC;

STATIC_ASSERT(sizeof(NUHGOBJ_POI_PC) == 0x50);
STATIC_ASSERT(offsetof(NUHGOBJ_POI_PC, matrix) == 0);
STATIC_ASSERT(offsetof(NUHGOBJ_POI_PC, joint_index) == 0x44);
STATIC_ASSERT(offsetof(NUHGOBJ_RENDER_PART_PC, rigid) == 4);
STATIC_ASSERT(offsetof(NUHGOBJ_RENDER_PART_PC, alternate_smooth) == 16);

typedef struct nuqfnt_render_data_pc_s {
    u32 colour;
    f32 x;
    f32 y;
    f32 z;
    NUMTX matrix;
} NUQFNT_RENDER_DATA_PC;

STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, colour) == 0);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, x) == 4);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, y) == 8);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, z) == 0xc);
STATIC_ASSERT(offsetof(NUQFNT_RENDER_DATA_PC, matrix) == 0x10);
#ifdef __cplusplus
class PCFrustumClipSet {
  public:
    PC_FRUSTUM_GROUP_PC first_group;
    u8 unknown_0380[0x390];
    i32 active_render_set_count;
    i32 TestBoxes(const NUVEC *, const NUVEC *);
    bool TestBox(const NUVEC *minimum, const NUVEC *maximum, i32 group_index);
    i32 fun_005e66b0(NUSPECIALHANDLE *special, NUMTX *matrix);
    bool fun_005e6710(const NUVEC *center, const NUVEC *extent, i32 group_index);
    i32 fun_005e6770(const NUVEC *center, const NUVEC *extent);
};

STATIC_ASSERT(offsetof(PCFrustumClipSet, first_group) == 0);
STATIC_ASSERT(offsetof(PCFrustumClipSet, active_render_set_count) == 0x710);

#endif
#ifdef __cplusplus
struct PC_OCCLUDER_RECORD {
    NUVEC4 vertices[4];
    NUVEC4 transformed[4];
    f32 min_x;
    f32 max_x;
    f32 min_y;
    f32 max_y;
    f32 min_depth;
    f32 depth;
    u8 unknown_98[8];
};

STATIC_ASSERT(sizeof(PC_OCCLUDER_RECORD) == 0xa0);
STATIC_ASSERT(offsetof(PC_OCCLUDER_RECORD, transformed) == 0x40);
STATIC_ASSERT(offsetof(PC_OCCLUDER_RECORD, min_x) == 0x80);
STATIC_ASSERT(offsetof(PC_OCCLUDER_RECORD, depth) == 0x94);
#endif

struct PC_HG_SHADOW_PACKET {
    NUMTX matrix;
    void *ellipse;
    void *cylinder;
    void *third_shape;
    i32 state0;
    i32 state1;
    i32 state2;
    i32 state_byte;
    f32 near_value;
    f32 far_value;
    u8 ellipse_count;
    u8 cylinder_count;
    u8 third_count;
    u8 sequence;
};

STATIC_ASSERT(sizeof(struct PC_HG_SHADOW_PACKET) == 104);
STATIC_ASSERT(offsetof(struct PC_HG_SHADOW_PACKET, sequence) == 103);
#ifdef __cplusplus
extern "C" {
#endif
    f32 NuQFntPrintJustifiedRSW(void *, VUFNT *, u16 *, f32, f32, f32, f32, f32, f32, f32, u32, NUMTX *);
    extern NUMTX debris_box_camera_cache;

    typedef struct CAMERA_CLIP_PLANES_PC {
        NUMTX frustum;
        NUMTX scissor;
        NUMTX near_far;
        NUVEC4 near_plane;
        NUMTX abs_frustum;
        NUMTX abs_scissor;
    } CAMERA_CLIP_PLANES_PC;

    STATIC_ASSERT(sizeof(CAMERA_CLIP_PLANES_PC) == 0x150);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PLANES_PC, scissor) == 0x40);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PLANES_PC, near_far) == 0x80);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PLANES_PC, near_plane) == 0xc0);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PLANES_PC, abs_frustum) == 0xd0);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PLANES_PC, abs_scissor) == 0x110);
    extern CAMERA_CLIP_PLANES_PC CameraClipPlanesPc;
    void NuSpecialUpdate(NUSPECIALHANDLE *special);
    f32 NuSpecialGetAnimEndFrame(NUSPECIALHANDLE *special);
    extern NUMTX NuPortalLocalViewPc;
    extern NUMTX NuPortalProjectionScratchPc;
    extern NUMTX NuPortalProjectionInversePc;

    typedef struct nucamerareflect_s {
        u32 unknown_00;
        NUMTX mtx;
        u32 unknown_44[16];
    } NUCAMERAREFLECT;

    STATIC_ASSERT(offsetof(NUCAMERAREFLECT, mtx) == 0x04);
    STATIC_ASSERT(sizeof(NUCAMERAREFLECT) == 0x84);
    extern NUCAMERAREFLECT global_reflect;
    extern NUMTX NuVpIdentityMatrixPc;
    extern NUMTX NuCameraScissorProjectionScratch;
    i32 NuSpecialDrawAt(NUSPECIALHANDLE *, NUMTX *);
    void __fastcall NuQuatMulAlignedPc(const NUQUAT *parent, const NUQUAT *rotation, NUQUAT *out);
    void NuSpecialSetDrawMtx(NUSPECIALHANDLE *special, NUMTX *matrix);
    extern NUMTX VisibilityPrimaryPlaneMatrixPc;
    extern NUMTX VisibilityAlternatePlaneMatrixPc;
    extern NUMTX VisibilityPrimaryWorldMatrixPc;
    extern NUMTX VisibilityAlternateWorldMatrixPc;
    extern NUMTX VisibilityPrimaryAbsoluteMatrixPc;
    extern NUMTX VisibilityAlternateAbsoluteMatrixPc;
    void NuAnimBuffCreateScratch(ANI3_BUFFER_PC *buffer);

    typedef struct CAMERA_CLIP_PACKET_PC {
        NUMTX clip;
        NUMTX projection;
        u32 flags;
        f32 far_clip;
        f32 near_clip;
        f32 x_scale;
        f32 y_scale;
    } CAMERA_CLIP_PACKET_PC;

    STATIC_ASSERT(sizeof(CAMERA_CLIP_PACKET_PC) == 0x94);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PACKET_PC, projection) == 0x40);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PACKET_PC, flags) == 0x80);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PACKET_PC, far_clip) == 0x84);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PACKET_PC, near_clip) == 0x88);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PACKET_PC, x_scale) == 0x8c);
    STATIC_ASSERT(offsetof(CAMERA_CLIP_PACKET_PC, y_scale) == 0x90);
    void NuCameraTransformScissorClip(NUVEC *, NUVEC *, i32, NUMTX *);
#ifdef __cplusplus
}
#endif

typedef struct NU_POST_SPEED_PARAMETERS_PC {
    i32 enabled;
    NUMTX previous;
    NUMTX current;
    f32 frame_time;
    f32 scale;
} NU_POST_SPEED_PARAMETERS_PC;

STATIC_ASSERT(sizeof(NU_POST_SPEED_PARAMETERS_PC) == 0x8c);
STATIC_ASSERT(offsetof(NU_POST_SPEED_PARAMETERS_PC, previous) == 4);
STATIC_ASSERT(offsetof(NU_POST_SPEED_PARAMETERS_PC, current) == 0x44);
STATIC_ASSERT(offsetof(NU_POST_SPEED_PARAMETERS_PC, scale) == 0x88);
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 4 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

typedef struct NUANIMDATA2_SKELETAL_PC {
    NUANIMDATA2_PC prefix;
    i8 *node_flags;
} NUANIMDATA2_SKELETAL_PC;

STATIC_ASSERT(offsetof(NUANIMDATA2_SKELETAL_PC, prefix) == 0);
STATIC_ASSERT(offsetof(NUANIMDATA2_SKELETAL_PC, node_flags) == 0x14);
STATIC_ASSERT(sizeof(NUANIMDATA2_SKELETAL_PC) == 0x18);
#ifdef __cplusplus
extern "C" {
#endif
    extern NUCAMERA *NuPortalCameraPc;
    extern NUCAMERA NuPortalDebugCameraPc;
    extern NUCAMERA NuPortalRestoreCameraPc;
    void NuAnimData2CalcTime(NUANIMDATA2_PC *, f32, NUANIM_TIME2_PC *);
    /* C extern declarations: */
    i32 StateAnimEvaluate2(STATE_ANIM_PC *state, u8 *index, u8 *value, f32 frame);
    i32 StateAnimEvaluate(STATE_ANIM_PC *state, u8 *index, u8 *value, f32 frame);
    STATE_ANIM_PC *StateAnimFixPtrs(STATE_ANIM_PC *state, i32 delta);

    typedef struct CAMERA_SAVED_STATE_PC {
        NUCAMERA camera;
        NUMTX view;
        NUMTX projection;
        NUMTX screen;
        NUMTX view_projection;
        NUMTX view_projection_scissor;
        NUMTX view_projection_viewport;
        NUMTX viewport;
        NUMTX view_projection_screen;
        NUMTX projection_screen;
        i32 effects;
        f32 zx;
        f32 zy;
        f32 zxs;
        f32 zys;
        u32 unknown_2cc;
        CAMERA_CLIP_PLANES_PC planes;
    } CAMERA_SAVED_STATE_PC;

    STATIC_ASSERT(sizeof(CAMERA_SAVED_STATE_PC) == 0x420);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, view) == 0x78);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, projection) == 0xb8);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, screen) == 0xf8);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, view_projection) == 0x138);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, view_projection_scissor) == 0x178);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, view_projection_viewport) == 0x1b8);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, viewport) == 0x1f8);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, view_projection_screen) == 0x238);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, projection_screen) == 0x278);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, effects) == 0x2b8);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, zx) == 0x2bc);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, zy) == 0x2c0);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, zxs) == 0x2c4);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, zys) == 0x2c8);
    STATIC_ASSERT(offsetof(CAMERA_SAVED_STATE_PC, planes) == 0x2d0);
    extern CAMERA_SAVED_STATE_PC CameraSavedStatesPc[16];
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 5 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

typedef struct nudisplay_list_pc_s {
    void *allocation;
    i32 item_count;
    void *items;
    f32 *fade_ranges;
    i32 clip_count;
    PC_DISPLAY_CLIP_OBJECT_PC *clip_objects;
    u16 *clip_counts;
    u8 unknown_001c[0xc];
    f32 *fade_levels;
    f32 *far_clip_ranges;
    u8 *clip_used[2];
    u8 unknown_0038[0xc];
    NUVEC4 *clip_bounds;
    u8 *visibility_flags;
    u32 material_count;
    NUMTL **display_materials;
    struct nudisplaylist_s **material_lists;
    u8 *material_used[2];
    i32 sort_count;
    struct nusortpri_s *sort_pris;
    f32 *alpha_values;
    i32 special_count;
    NUDISPLAY_SPECIAL_PC *specials;

    union {
        u32 render_flags;

        struct {
            u8 unknown_0074_76[0x2];
            u8 instance_visibility_enabled;
            u8 unknown_0077[0x1];
        };

        struct {
            u32 : 5;
            u32 material_layer_mask : 8;
        };
    };
    struct NUGSCN_s *gscene;
    u32 *native_word;
    struct nuglobalrndrstate_pc_s *local_state;
    u8 unknown_84[0x4];
    u8 *portal_visibility_overrides;
    struct PC_MATERIAL_ANIM_SET *material_animations;
} NUDISPLAY_LIST_PC;

STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, clip_count) == 16);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, clip_objects) == 0x14);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, clip_counts) == 0x18);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, fade_levels) == 0x28);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, local_state) == 0x80);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, clip_used) == 48);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, material_count) == 76);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, display_materials) == 80);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, material_lists) == 84);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, material_used) == 88);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, gscene) == 120);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, native_word) == 124);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, material_animations) == 0x8c);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, allocation) == 0);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, item_count) == 4);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, items) == 8);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, sort_count) == 0x60);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, sort_pris) == 0x64);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, alpha_values) == 0x68);

STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, clip_bounds) == 0x44);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, render_flags) == 0x74);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, visibility_flags) == 0x48);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, instance_visibility_enabled) == 0x76);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, special_count) == 0x6c);
STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, specials) == 0x70);

typedef struct NUHGOBJ_EVAL_PC {
    u8 unknown_0[0x110];
    struct nudisplay_list_pc_s *display_list;
    u8 data_0x114[84];
    i32 joint_count;
    NUHGOBJ_EVAL_JOINT_PC *joints;
    NUMTX *bind_matrices;
    NUMTX *inverse_bind;
    i32 joint_override_map_count;
    u8 *joint_override_map;
    u8 data_0x180[4];
    NUHGOBJ_POI_PC *points;
    i32 point_index_count;
    u8 *point_index_map;
    i32 render_count;
    NUHGOBJ_RENDER_PART_PC *render_parts;
    u8 unknown_198[4];
    struct PC_HG_SHADOW_GROUP *shadow_groups;
    u8 suppress_shadow_points;
    u8 unknown_1a1[11];
    NUVEC bounds_min;
    NUVEC bounds_max;
} NUHGOBJ_EVAL_PC;

STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_count) == 0x168);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joints) == 0x16c);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, bind_matrices) == 0x170);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_override_map_count) == 0x178);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_override_map) == 0x17c);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, points) == 0x184);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, point_index_count) == 0x188);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, point_index_map) == 0x18c);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, inverse_bind) == 0x174);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, shadow_groups) == 0x19c);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, suppress_shadow_points) == 0x1a0);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, bounds_min) == 0x1ac);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, bounds_max) == 0x1b8);

/* PC NuDynamicLight::RenderSet. */
typedef struct nurndrlightrecord_s {
    NUMTX view_pc;
    NUMTX projection_pc;
    NUMTX shadow_matrix_pc;
    NUMTX texture_matrix_pc;
    f32 parameter_100;
    f32 parameter_104;
    i32 enabled;
    f32 parameter_10c;
    f32 parameter_110;
    u8 opaque_114_120[0xc];
    NUVEC4 corners_pc[8];
    NUVEC4 planes_pc[12];
    i32 shadow_plane_count;
    u8 opaque_264_270[0xc];
    NUVEC4 far_plane_pc;
    NUVEC4 endpoint0_pc;
    NUVEC4 endpoint1_pc;
    f32 shadow_radius_pc;
    NU_LIGHT_SHADOW_LIST_VIEW_PC shadow_lists_pc[2];
    NURNDRLIBSTATE *render_states[2];
    NUDISPLAYLISTITEM *list_items[2];
    u32 unknown_33c[6];
    i32 pending[8];
    i32 count;
    u8 opaque_378_380[0x8];
#ifdef __cplusplus
    nurndrlightrecord_s();
    nurndrlightrecord_s &operator=(const nurndrlightrecord_s &other);
#endif
} NURNDRLIGHTRECORD;

STATIC_ASSERT(sizeof(NURNDRLIGHTRECORD) == 0x380);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, parameter_110) == 0x110);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, shadow_plane_count) == 0x260);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, render_states) == 0x32c);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, enabled) == 0x108);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, pending) == 0x354);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, count) == 0x374);
#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTL **PCSpecialForcedMaterialTable;
    extern NUMTL *PCSpecialForcedMaterial;
    extern NUMTL *PCMaterialPoolPc;
    void NuRndrRectUV2diZ(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour,
                          NUMTL *material, i32 z);

    typedef struct FONT_LEGACY_READER_PC {
        u8 unknown00[12];
        i16 height;
        u8 unknown0e[6];
        i16 baseline;
        i16 ic_gap;
        NUMTL *material;
        i32 glyph_count;
        u8 character_map[256];
        FONT_LEGACY_GLYPH_PC glyphs[256];
    } FONT_LEGACY_READER_PC;

    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, height) == 0xc);
    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, baseline) == 0x14);
    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, ic_gap) == 0x16);
    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, material) == 0x18);
    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, glyph_count) == 0x1c);
    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, character_map) == 0x20);
    STATIC_ASSERT(offsetof(FONT_LEGACY_READER_PC, glyphs) == 0x120);
    extern NUMTL **PCDisplayMaterialAddQueue;
    NUDISPLAYLISTITEM *NuDisplayListCreateMtlDlist(NUDISPLAYLISTITEM *, NUMTL *, VARIPTR *, VARIPTR);
    STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, render_count) == 0x190);
    STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, render_parts) == 0x194);
    i32 NuRndrLineStrip2d(NURND_VERTEX2D_PC *vertices, NUMTL *material, i32 count);

    typedef struct PC_MATERIAL_ANIM_SET {
        u8 unknown_0[12];
        struct PC_MATERIAL_ANIM_SET *next;
    } PC_MATERIAL_ANIM_SET;

    STATIC_ASSERT(offsetof(PC_MATERIAL_ANIM_SET, next) == 12);

    extern NUMTL **PCDisplayMaterialDeleteQueue;
    extern NUDISPLAY_LIST_PC *PCDisplayScenes[256];
    extern NUDISPLAY_LIST_PC PCDynamicDisplayScene;

    void DrawAlphaImage(i32 rows, i32 cols, NUMTL *material, i32 use_pixel_offsets,
                        RENDER_POST_BLOOM_READER_PC *parameters);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, view_pc) == 0);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, projection_pc) == 0x40);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, shadow_matrix_pc) == 0x80);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, texture_matrix_pc) == 0xc0);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, corners_pc) == 0x120);
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuDynamicLightIsEnabled(NURNDRLIGHTRECORD *records);

    extern NU_POST_MAIN_VIEW_PC *mainFilter;
    extern NU_POST_MOTION_VIEW_PC *motionFilter;
    extern NU_POST_ACCUM_VIEW_PC *motionAccumFilter;
    extern NU_POST_SPEED_VIEW_PC *speedBlurFilter;
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, shadow_lists_pc) == 0x2a4);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, shadow_lists_pc) + sizeof(NU_LIGHT_SHADOW_LIST_VIEW_PC) == 0x2e8);
#ifdef __cplusplus
extern "C" {
#endif
    void NuRndrRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material);
    void NuRndrLine2di(i32 x0, i32 y0, i32 x1, i32 y1, i32 colour, NUMTL *material);
    void DisplaySceneRndrSpecials(NUDISPLAY_LIST_PC *, i32, i32);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, planes_pc) == 0x1a0);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, far_plane_pc) == 0x270);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, endpoint0_pc) == 0x280);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, endpoint1_pc) == 0x290);
STATIC_ASSERT(offsetof(NURNDRLIGHTRECORD, shadow_radius_pc) == 0x2a0);
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 6 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

typedef struct nurndrlibscene_s {
    i32 scene_id;
    u32 clear_flags;
    u32 clear_flags_08;
    i32 bg_colour;
    f32 clear_alpha;
    f32 unknown_14;
    f32 unknown_18;
    f32 calc_1c;
    f32 calc_20;
    u32 unknown_24;
    u32 unknown_28;
    u8 burnout_intensity;
    f32 burnout_flare;
    f32 burnout_threshold;
    i32 dyn_light_count;
    void *dyn_light_ptr;
    i32 unknown_40;
    u8 opaque_44_48[0x4];
    u32 unknown_48;

    union {
        struct {
            f32 clear_alpha_4c;
            u8 opaque_50_58[0x8];
        };

        f32 shadow_coefficients_pc[3];
    };

    union {
        struct {
            u32 unknown_58;
            u8 opaque_5c_ac[0x50];
        };

        BURN_PARAMETERS_PC burnout_parameters_pc;
    };

    u32 unknown_ac;
    u8 opaque_b0_c4[0x14];
    u32 unknown_c4;
    u8 opaque_c8_e0[0x18];
    u32 unknown_e0;
    u32 unknown_e4;
    u8 opaque_e8_174[0x8c];
    u32 unknown_174;
    u32 unknown_178;
    u8 opaque_17c_188[0xc];
    u32 unknown_188;
    u8 opaque_18c_214[0x88];
    u32 unknown_214;
} NURNDRLIBSCENE;

STATIC_ASSERT(sizeof(NURNDRLIBSCENE) == 0x218);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, scene_id) == 0x00);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, clear_flags) == 0x04);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, clear_flags_08) == 0x08);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, bg_colour) == 0x0c);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, clear_alpha) == 0x10);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, calc_1c) == 0x1c);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, calc_20) == 0x20);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, dyn_light_count) == 0x38);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, dyn_light_ptr) == 0x3c);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_40) == 0x40);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_48) == 0x48);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, clear_alpha_4c) == 0x4c);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_58) == 0x58);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_ac) == 0xac);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_c4) == 0xc4);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_e0) == 0xe0);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_174) == 0x174);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_178) == 0x178);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_188) == 0x188);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, unknown_214) == 0x214);
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_GLOBAL_LIGHTING_STATE PCGlobalLightingState;
    extern NURNDRLIBSCENE currentScene;
    extern NUDISPLAYLIST PCDisplayList2dPc;
    extern NUSORTPRI *sort_list;
    extern NUDISPLAYLISTRENDERSCENE *PCSafeRenderScenes[24];

    extern NUDISPLAYLISTRENDERSCENE *render_scene_ptrs[24];
    void SetProceduralAnimationFn(NUJOINTPROCANIMFN_PC callback);

    /* Qualified CPU visibility/tree views; original owning extents remain unknown. */
    typedef struct visibility_result_reader_pc_s {
        i32 instance_count;
        u8 unknown_04[4];
        NUGSCN *source_scene;
        void *occlusion_data;
        u8 unknown_10[4];
        void *instance_tree;
        void *portal_marker;
        u8 *portal_bits;
        void *context;
        u8 *instance_tree_bits;
        u32 flags;
    } VISIBILITY_RESULT_READER_PC;

    STATIC_ASSERT(sizeof(VISIBILITY_RESULT_READER_PC) == 0x2c);
    STATIC_ASSERT(offsetof(VISIBILITY_RESULT_READER_PC, source_scene) == 8);
    STATIC_ASSERT(offsetof(VISIBILITY_RESULT_READER_PC, occlusion_data) == 0xc);
    STATIC_ASSERT(offsetof(VISIBILITY_RESULT_READER_PC, instance_tree) == 0x14);
    STATIC_ASSERT(offsetof(VISIBILITY_RESULT_READER_PC, context) == 0x20);
    STATIC_ASSERT(offsetof(VISIBILITY_RESULT_READER_PC, instance_tree_bits) == 0x24);
    STATIC_ASSERT(offsetof(VISIBILITY_RESULT_READER_PC, flags) == 0x28);
    extern VISIBILITY_RESULT_READER_PC *myvisi;
    extern NURNDRLIBSCENE sceneParameters[16];
    void NuVisiOcclusion(VISIBILITY_RESULT_READER_PC *);
    VISIBILITY_RESULT_READER_PC *NuVisiEvaluate(NUGSCN *, void *);
    void NuVisiInstTree(VISIBILITY_RESULT_READER_PC *, NUGSCN *);
    NUGSCN *PCGraphicsSceneReadClones(VARIPTR *, VARIPTR, char *, i32);
    i32 findrange(struct NUGSCN_s *arg0, i32 arg1);
    void NuSpecialSetDrawPos(struct nuhspecial_s *special, struct nuvec_s *pos);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, shadow_coefficients_pc) == 0x4c);
STATIC_ASSERT(offsetof(NURNDRLIBSCENE, burnout_parameters_pc) == 0x58);
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 7 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

struct GRAPHICS_RENDER_TEXTURE_PC {
    PC_TEXTURE_RESOURCE_PC *resource;
    u16 width;
    u16 height;
    u16 unknown_08;
    u16 mip_count;

    union {
        u8 unknown_0c[0x1c];

        struct {
            u16 original_mip_count;
            u16 reserved_0e;
            u32 field_10;
            i32 format;
            i32 usage;
            i32 pool;

            union {
                u32 flags;

                struct {
                    u32 sampler_unknown_0 : 8;
                    u32 address_u : 3;
                    u32 address_v : 3;
                    u32 address_w : 3;
                    u32 min_filter : 3;
                    u32 mag_filter : 3;
                    u32 mip_filter : 3;
                    u32 sampler_unknown_26 : 6;
                };
            };

            u32 field_24;
        };
    };

    PC_TEXTURE_RESOURCE_PC *surface;
    i32 external_id;
};

STATIC_ASSERT(sizeof(GRAPHICS_RENDER_TEXTURE_PC) == 0x30);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, resource) == 0);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, width) == 4);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, height) == 6);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, mip_count) == 0xa);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, surface) == 0x28);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, external_id) == 0x2c);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, original_mip_count) == 0xc);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, format) == 0x14);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, usage) == 0x18);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, pool) == 0x1c);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, flags) == 0x20);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_24) == 0x24);
#ifdef __cplusplus
extern "C" {
#endif
    void NuTexAnimResetList(TEXTURE_ANIMATION_PC *animation);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, unknown_08) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern NUFILE nudlist_field_958;
    extern NUFILE nudlist_field_988;
    extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferTexturePc;
    extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferCopyPc;
#ifdef __cplusplus
}
#endif

/* Native 80bb38 proxy array: five 12-byte records. */
typedef struct PC_POST_BUFFER_PROXY_PC {
    GRAPHICS_RENDER_TEXTURE_PC *texture;
    i32 kind;
    u8 enabled;
    u8 resolved;
    u8 unknown_0a[2];
#ifdef __cplusplus
    void fun_005c21c0(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 kind, u8 enabled, u8 resolved);
    u8 fun_005c21e0();
    void fun_005c21f0();
    GRAPHICS_RENDER_TEXTURE_PC *fun_005c2210();
#endif
} PC_POST_BUFFER_PROXY_PC;

STATIC_ASSERT(sizeof(PC_POST_BUFFER_PROXY_PC) == 12);
STATIC_ASSERT(offsetof(PC_POST_BUFFER_PROXY_PC, texture) == 0);
STATIC_ASSERT(offsetof(PC_POST_BUFFER_PROXY_PC, kind) == 4);
STATIC_ASSERT(offsetof(PC_POST_BUFFER_PROXY_PC, enabled) == 8);
STATIC_ASSERT(offsetof(PC_POST_BUFFER_PROXY_PC, resolved) == 9);
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_POST_BUFFER_PROXY_PC PostProxyBuffersPc[5];
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 8 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)

/* PC NuDynamicLight; its pool slots are 0x830 bytes. */
struct PC_SPECIAL_SHADOW_LIGHT {
    NURNDRLIGHTRECORD render_sets[2];

    union {
        f32 split_distances_pc[3];

        struct {
            u8 unknown_700[4];
            f32 shadow_distance_pc;
            u8 unknown_708[4];
        };
    };

    i32 render_set_count;
    i32 active_render_set_count;
    u8 unknown_714[0xc];
    NUVEC4 split_planes_pc[2];
    NUVEC4 camera_position_pc;
    NUVEC4 camera_forward_pc;
    NUVEC4 light_position_pc;

    union {
        NUVEC4 light_direction4_pc;

        struct {
            NUVEC shadow_direction_pc;
            f32 light_direction_w_pc;
        };
    };

    NUMTX view;
    NUMTX projection;
    i32 parameter_4;
    i32 parameter_5;
    i32 enabled;
    f32 parameter_0;
    f32 parameter_1;
    f32 parameter_2;
    f32 parameter_3;
    f32 parameter_19;
    f32 parameter_20;
    i32 used_on_specials;
#ifdef __cplusplus
    PC_SPECIAL_SHADOW_LIGHT();
    void LookAtPc(NUVEC *, NUVEC *, NUVEC *);
    void SetupCustomCameraFrustumPc(NUCAMERA *, const f32 *, i32);
    void RenderSceneShadowPc(NUGSCN *scene);
    void ComputeShadowMatrixPc(NURNDRLIGHTRECORD *);
    void SetupShadowPc(NURNDRLIGHTRECORD *);
    void RenderShadowPc(i32, PC_FRAMEBUFFER_PC *);
    void RenderConstantsPc(void *);
    void SetEnabledPc(i32 enabled);
    void SetParameterfPc(i32 parameter, f32 value);
    i32 fun_005c2eb0();
    void fun_005c2ec0(i32 used);
    NU_LIGHT_SHADOW_LIST_VIEW_PC *fun_005c2ee0(i32 set, NUMTL *material);
    void fun_005c6d90(NUMTX *view, NUMTX *projection);
    i32 fun_005c2f20();
    NUMTX *fun_005c2f30();
    NUMTX *fun_005c2f40();
    f32 GetParameterfPc(i32 parameter);
    void SetParameteriPc(i32 parameter, i32 value);
    i32 GetParameteriPc(i32 parameter);
    void fun_005c3070(i32 set, i32 scene);
    PC_SPECIAL_SHADOW_LIGHT *Clone(VARIPTR *arena, VARIPTR end);
    PC_SPECIAL_SHADOW_LIGHT &operator=(const PC_SPECIAL_SHADOW_LIGHT &other);
    void fun_00595360(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
#endif
};

STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, shadow_distance_pc) == 0x704);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, active_render_set_count) == 0x710);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, shadow_direction_pc) == 0x770);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, view) == 0x780);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, parameter_4) == 0x800);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, parameter_5) == 0x804);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, parameter_0) == 0x80c);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, render_set_count) == 0x70c);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, projection) == 0x7c0);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, enabled) == 0x808);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, used_on_specials) == 0x824);
STATIC_ASSERT(sizeof(struct PC_SPECIAL_SHADOW_LIGHT) == 0x828);
#ifdef __cplusplus
extern "C" {
#endif
    void NuDynamicLightSetParameterf(struct PC_SPECIAL_SHADOW_LIGHT *light, i32 parameter, f32 value);
    void NuDynamicLightSetParameteri(struct PC_SPECIAL_SHADOW_LIGHT *light, i32 parameter, i32 value);
#ifdef __cplusplus
}
#endif

#ifndef SAGA_HARDWARE_LIFECYCLE_PROPOSAL
#define SAGA_HARDWARE_LIFECYCLE_PROPOSAL

/* Proposed working fields only. Original PC class name/TU visibility stays qualified.
   Keep the pool and its real static constructor/destructor in one C++ owner. */

struct PC_FRAMEBUFFER_PC {
    PC_EFFECT_SURFACE_PC targets[5];
    u32 clear_colour[5];
    i32 changed;
#ifdef __cplusplus
    PC_FRAMEBUFFER_PC();
    ~PC_FRAMEBUFFER_PC();
#endif
};

STATIC_ASSERT(sizeof(PC_FRAMEBUFFER_PC) == 0xa4);
STATIC_ASSERT(offsetof(PC_FRAMEBUFFER_PC, targets) == 0);
STATIC_ASSERT(offsetof(PC_FRAMEBUFFER_PC, clear_colour) == 0x8c);
STATIC_ASSERT(offsetof(PC_FRAMEBUFFER_PC, changed) == 0xa0);

STATIC_ASSERT(offsetof(PC_TEXTURE_RESOURCE_METHODS_PC, get_surface_level) == 0x48);

#ifdef __cplusplus
extern "C" {
#endif
    i32 PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE_PC *);
    i32 PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE_PC *);
    void PCDeviceClear(u32, const RECT *, u32, u32, f32, u32);
#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_SCENE_MTL_VIEW *pAlphaMask;
    extern struct PC_SCENE_MTL_VIEW *pZClearMaterial;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, split_distances_pc) == 0x700);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, split_planes_pc) == 0x720);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, camera_position_pc) == 0x740);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, camera_forward_pc) == 0x750);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, light_position_pc) == 0x760);
STATIC_ASSERT(offsetof(struct PC_SPECIAL_SHADOW_LIGHT, light_direction4_pc) == 0x770);
#endif
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART == 9 && !defined(SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern NU_POST_DEFERRED_VIEW_PC *deferredFilter;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_PART 10
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NU3D_INCLUDES_H_STOP
