#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nuwin32/d3dCore.h"

struct NuDynamicLight;
struct PC_TEXTURE_RESOURCE;

typedef struct NUANGVEC { // size 0xc
    NUANG x;              // 0x0
    NUANG y;              // 0x4
    NUANG z;              // 0x8
} NUANGVEC;

typedef struct nurnd_vertex2d_s { // size 0x1c
    f32 x;                        // 0x00
    f32 y;                        // 0x04
    f32 z;                        // 0x08
    f32 rhw;                      // 0x0c
    u32 colour;                   // 0x10
    f32 uv[2];                    // 0x14
} NURND_VERTEX2D;

#define NU_PORTAL_STACK_SIZE 0x3000

typedef struct NUGSCNROOM_s { // size 0x18
    i16 *instance_indices;    // 0x00
    NUPLANE *planes;          // 0x04
    i16 *portal_indices;      // 0x08
    i16 instance_count;       // 0x0c
    u8 plane_count;           // 0x0e
    u8 portal_count;          // 0x0f

    union { // size 0x4
        u8 visibility_flags;
        u32 flags; // NUGSCNROOM_VISITED
    }; // 0x10

    u8 priority;      // 0x14
    u8 unknown_15[3]; // 0x15
} NUGSCNROOM;

/* NUGSCNROOM.flags: set while the portal walk has reached the room this frame. */
#define NUGSCNROOM_VISITED 0x4

typedef struct pc_display_clip_object_s { // size 0xc
    i32 material_count;                   // 0x0
    i32 *material_ids;                    // 0x4
    i32 *item_indices;                    // 0x8
} PC_DISPLAY_CLIP_OBJECT;

typedef struct NUPORTAL_FRUSTUM { // size 0xa0
    f32 transposed_planes[32];    // 0x00
    NUPLANE *planes;              // 0x80
    NUVEC minimum;                // 0x84
    NUVEC maximum;                // 0x90
    i16 plane_count;              // 0x9c
    i16 room_id;                  // 0x9e
} NUPORTAL_FRUSTUM;

typedef struct nugscn_anim_lookup_s { // size 0xc
    u16 count;                        // 0x0
    u16 end_frame;                    // 0x2
    f32 *times;                       // 0x4
    u8 *values;                       // 0x8
} NUGSCN_ANIM_LOOKUP;

typedef struct NUHGOBJ_JOINT_OVERRIDE { // size 0x34
    NUVEC rotation;                     // 0x00
    NUVEC translation;                  // 0x0c
    NUVEC scale;                        // 0x18
    i16 rotation_max[3];                // 0x24
    i16 rotation_min[3];                // 0x2a
    u8 joint_index;                     // 0x30
    u8 flags;                           // 0x31
    u8 unknown_32[2];                   // 0x32
} NUHGOBJ_JOINT_OVERRIDE;

/* NUHGOBJ_JOINT_OVERRIDE.flags: what an override adds to the animated joint (the head turn of animation.c sets
 * ROTATE and the limits). */
#define NUJOINT_OVERRIDE_ROTATE 0x1    /* rotation is added */
#define NUJOINT_OVERRIDE_TRANSLATE 0x2 /* translation is added */
#define NUJOINT_OVERRIDE_SCALE 0x4     /* scale is added */
#define NUJOINT_OVERRIDE_LIMIT_X 0x8   /* the X angle is held within rotation_min[0]..rotation_max[0] */
#define NUJOINT_OVERRIDE_LIMIT_Y 0x10
#define NUJOINT_OVERRIDE_LIMIT_Z 0x20

/* The per-joint node flags of an animation (ANI3_BUFFER.joint_flags, NUANIM_LEGACY_SET.flags): which curves the
 * joint has (CurveGroupMasks: translation 2, rotation 1, scale 8) and how its matrix is built. */
#define ANI3_NODE_ROTATION 0x1
#define ANI3_NODE_TRANSLATION 0x2
#define ANI3_NODE_SCALE 0x8
#define ANI3_NODE_SCALE_COMPENSATE 0x10 /* the parent's scale is divided out */
#define ANI3_NODE_BIND_ROTATION 0x20    /* the rotation is applied on top of the joint's bind matrix */
#define ANI3_NODE_ROOT_UNSCALED 0x40    /* a root joint's accumulated scale restarts at 1 */

/* ANI3 curve key types above the constant ones (0-5 index the animation's curve constants), by how the players
 * decode a key (sizes in KeyStructSizes). */
#define ANI3_KEY_BYTE_BLEND 6     /* 8-bit values with a 2-bit blend per quarter (ANI_GetKeyParts2, ANI_GetBlend2) */
#define ANI3_KEY_1648 7           /* CalcValue1648 */
#define ANI3_KEY_CONSTANT 8       /* a constant index per quarter */
#define ANI3_KEY_WORD 9           /* a 16-bit value per quarter */
#define ANI3_KEY_CONSTANT_PAIR 10 /* two 16-bit constants make an integer */

typedef struct ANI3_SCALEMIN { // size 0x8
    f32 scale;                 // 0x0
    f32 minimum;               // 0x4
} ANI3_SCALEMIN;

typedef struct ANI3_HEADER {  // size 0x3c
    i32 magic;                // 0x00
    u16 node_count;           // 0x04
    u16 key_count;            // 0x06
    u16 key_stride;           // 0x08
    u16 frame_count;          // 0x0a
    u16 curve_count;          // 0x0c
    u16 first_frame;          // 0x0e
    u8 end_frame;             // 0x10
    u8 constant_index;        // 0x11
    u8 fixed_up;              // 0x12 (pointers fixed up after loading)
    u8 format_flags;          // 0x13
    u16 next_block;           // 0x14
    u16 declared_end_frame;   // 0x16
    u8 pad_18[4];             // 0x18
    f32 minimum;              // 0x1c
    f32 scale;                // 0x20
    ANI3_SCALEMIN *scale_min; // 0x24
    i16 *constants;           // 0x28
    u16 *curve_types;         // 0x2c
    u8 *keys;                 // 0x30
    u8 *node_flags;           // 0x34
    void *extra_data;         // 0x38
} ANI3_HEADER;

typedef union DWA_CURVE_CELL { // size 0x4
    f32 constant;
    void *encoded;
} DWA_CURVE_CELL;

typedef struct NUANIM_TIME2 { // size 0x18
    f32 time;                 // 0x00
    f32 time_offset;          // 0x04
    i32 chunk;                // 0x08
    u32 time_mask;            // 0x0c
    u32 time_byte;            // 0x10
    i32 chunk_frame;          // 0x14
} NUANIM_TIME2;

typedef struct DWA_WEIGHT_BLOCK { // size 0x208
    u32 unknown_00[2];            // 0x000
    f32 values[128];              // 0x008
} DWA_WEIGHT_BLOCK;

#ifdef __cplusplus
extern "C" {
#endif
    extern DWA_WEIGHT_BLOCK DwaWeightPool[60669];
#ifdef __cplusplus
}
#endif

typedef struct nuprim_vertex_group_source_s { // size 0x8
    u8 count;                                 // 0x0
    u8 block_count;                           // 0x1
    u8 flags;                                 // 0x2
    u8 unknown_03;                            // 0x3
    i8 *values;                               // 0x4
} NUPRIM_VERTEX_GROUP_SOURCE;

typedef struct nurndrlibstate_s { // size 0x34
    u32 unknown_00[2];            // 0x00
    struct numtl_s *mtl;          // 0x08
    u32 unknown_0c[2];            // 0x0c
    i32 tex_id;                   // 0x14
    u16 unknown_18;               // 0x18
    u16 unknown_1a;               // 0x1a
    u16 unknown_1c;               // 0x1c
    i16 konst_id;                 // 0x1e
    i32 global_id;                // 0x20
    i16 lights_id;                // 0x24
    u16 unknown_26;               // 0x26
    u16 unknown_28;               // 0x28
    u16 unknown_2a;               // 0x2a
    i16 camera_id;                // 0x2c
    i16 fog_id;                   // 0x2e
    i16 vertex_groups_id;         // 0x30
    i16 reflection_id;            // 0x32
} NURNDRLIBSTATE;

/* GRAPHICS_RENDER_TEXTURE.format, by the D3DFORMAT PCGraphicsFormatToD3d gives for it. */
#define PC_GRAPHICS_FORMAT_X8R8G8B8 1
#define PC_GRAPHICS_FORMAT_A8R8G8B8 2
#define PC_GRAPHICS_FORMAT_D24S8 4 /* a depth surface, not a texture */
#define PC_GRAPHICS_FORMAT_D24FS8 5
#define PC_GRAPHICS_FORMAT_R32F 10

typedef struct GRAPHICS_RENDER_TEXTURE GRAPHICS_RENDER_TEXTURE;
typedef struct PC_FRAMEBUFFER PC_FRAMEBUFFER;

struct PC_SPECIAL_SHADOW_LIGHT;

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
    extern i32 PCMaterialPoolCount;
    extern i32 PCShadowSurfaceSamplingEnabled;
    extern i32 PCSpecialForceMaterialCount;
    extern u8 CutSceneBoundingBoxTrackRoot;

    typedef struct FONT_LEGACY_GLYPH { // size 0x14
        f32 u0;                        // 0x00
        f32 v0;                        // 0x04
        f32 u1;                        // 0x08
        f32 v1;                        // 0x0c
        u32 unknown_10;                // 0x10
    } FONT_LEGACY_GLYPH;

    extern const char RenderSplineShopStart[11];

    void NuBackbufferCopy(i32);
    extern i32 PCPortalsEnabled;

    typedef struct TimeBarSet {  // size 0x2c
        i32 uses_buffer;         // 0x00
        i32 slot_count;          // 0x04
        i32 *accumulators[2];    // 0x08
        NuTime *start_times;     // 0x10
        u32 *peak_times;         // 0x14
        u32 *frame_peak_times;   // 0x18
        i32 *colours;            // 0x1c
        i32 *toggle_flags;       // 0x20
        const char **slot_names; // 0x24
        i32 visible;             // 0x28
    } TimeBarSet;

    extern TimeBarSet *NuTimeBar_SetList[16];
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
    extern struct PC_MATERIAL_ANIM_SET *PCMaterialAnimSets;
    extern i32 nudlist_field_fc4;
    extern u32 nudlist_field_9d0;
    extern i32 nudlist_field_9d8;
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
#define AxisClipFrustumPlanes CameraClipPlanes.frustum
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipScissorPlanes CameraClipPlanes.scissor
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipNearFarPlanes CameraClipPlanes.near_far
// NOLINTNEXTLINE(readability-identifier-naming)
#define CameraNearClipPlane CameraClipPlanes.near_plane
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipAbsFrustumPlanes CameraClipPlanes.abs_frustum
// NOLINTNEXTLINE(readability-identifier-naming)
#define AxisClipAbsScissorPlanes CameraClipPlanes.abs_scissor
    extern i32 NuTextureCapacityPC;
    extern i32 NuTextureLoadCountPC;
    extern i32 *NuTextureLoadOrderPC;
    extern i32 nuspecial_draw_state;
    extern u8 fstackmem[NU_PORTAL_STACK_SIZE];
    extern NUPORTAL_FRUSTUM **NuPortalFrusta;
    extern i32 *NuPortalFrustumCount;
    extern const NUVEC NuPortalBoundsOne;
    extern const NUVEC NuPortalBoundsMinusOne;
    extern NUVEC NuPortalWorldCamera;
    extern NUVEC *NuPortalOverrideCamera;
    extern i16 NuPortalCameraRoom;
    extern i16 NuPortalRoomsVisited[16];
    extern i32 NuPortalDebugDraw;
    extern NUPLANE NuPortalNearPlane;
    extern i32 NuPortalDebugEnabled;
    extern i32 NuPortalCameraSelect;
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
    extern u8 AnimQuatSaved[4];
    extern NUPRIM_VERTEX_GROUP_SOURCE *NuPrimVertexGroupSource;
    void NuTexAnimRemoveList(void *);
    i32 NuTexGetUnresolvedTextureTIDPS(void);
    extern i32 PCDisplayMaterialDeleteCount;
#define PCDisplayMaterialRegistry (PCDynamicDisplayScene.display_materials) // NOLINT(readability-identifier-naming)
#define PCDisplayMaterialCount (PCDynamicDisplayScene.material_count)       // NOLINT(readability-identifier-naming)
    extern u32 numtl_renderplane;
    extern const char *PCDynamicDisplaySceneNamePointer;
    extern i32 VisibilityEvaluateEnabled;
    extern i32 VisibilityOcclusionEnabled;
    extern i32 do_InstTree;
    extern i32 VisibilityCameraLock;
    extern i32 VisibilityLoadedOcclusion;
    extern i32 VisibilityUsingOcclusion;
    extern i32 VisibilityCurrentViewBox;
    extern NUPLANE VisibilityDepthPlane;
    extern NUPLANE VisibilityAbsoluteDepthPlane;
    extern u8 cacheValues[256];
    extern u8 OcclusionBitArray[0x320];
#define VISIBILITY_ALTERNATE_WIDTH zxs
#define VISIBILITY_OCCLUSION_OUTPUT OcclusionBitArray
    void NuCameraLock(i32);
    extern i32 DisplaySceneRenderDisabled;
    extern i32 noscenespecials;
    extern i32 VisibilityPortalFilterEnabled;

    typedef struct NU_POST_BLOOM_PARAMETERS NU_POST_BLOOM_PARAMETERS;

    i16 RenderPostASin(f32);
    void DrawAlphaGrid(i32, i32, NU_POST_BLOOM_PARAMETERS *);

    void NuTexAnimSetSignals(u32 signals);
    void NuTexAnimProcess(f32 frame_time);

    /* Verified game-local light views may remain source-local: light_data4d8/current61c. No new shared fields or
     * storage.
     */

    typedef struct NUANIM_LEGACY_KEY { // size 0x10
        f32 time;                      // 0x0
        f32 reciprocal_span;           // 0x4
        f32 tangent;                   // 0x8
        f32 value;                     // 0xc
    } NUANIM_LEGACY_KEY;

    typedef struct NUANIM_LEGACY_CURVE { // size 0x10
        u8 key_mask[4];                  // 0x0
        NUANIM_LEGACY_KEY *keys;         // 0x4
        i32 key_count;                   // 0x8
        u32 flags;                       // 0xc
    } NUANIM_LEGACY_CURVE;

    typedef struct NUANIM_LEGACY_SET { // size 0x10
        u32 flags;                     // 0x0
        f32 *constants;                // 0x4
        NUANIM_LEGACY_CURVE **curves;  // 0x8
        i8 curve_count;                // 0xc
        u8 unknown_0d[3];              // 0xd
    } NUANIM_LEGACY_SET;

    typedef struct NUANIM_LEGACY_CHUNK { // size 0xc
        i32 node_count;                  // 0x0
        u32 unknown_04;                  // 0x4
        NUANIM_LEGACY_SET **curve_sets;  // 0x8
    } NUANIM_LEGACY_CHUNK;

    typedef struct NUANIM_LEGACY_HEADER {    // size 0x10
        f32 duration;                        // 0x0
        u32 unknown_04;                      // 0x4
        i32 chunk_count;                     // 0x8
        struct NUANIM_LEGACY_CHUNK **chunks; // 0xc
    } NUANIM_LEGACY_HEADER;

    void NuAnimDataCalcTime(NUANIM_LEGACY_HEADER *, f32, NUANIM_TIME2 *);
    DWA_WEIGHT_BLOCK **NuHGobjEvalDwa(i32, i16 *, NUANIM_LEGACY_HEADER *, f32);
    extern f32 FrameMaterialAnimationSpeed;
    void NuDisplayListAnimateMtls(f32 frame_time);
    extern i32 nuspecial_vertex_noffsets;
    extern f32 (*nuspecial_vertex_offsets)[4];
    extern u32 NuPostEffectInitialisedFlags;
    extern char *PCDisplayCaptureCursor;
    extern char *PCDisplayCaptureFlushLimit;
    extern char PCDisplayCaptureBuffer[0x1000];
    i32 NuTexGetWidth(i32 id);
    i32 NuTexGetHeight(i32 id);
    extern i32 NuPostEffectBackBufferFrame;
#ifdef __cplusplus
}
#endif

/* Working accessed-field views only, not original allocation extents.
 * Pointer-only parameter types remain opaque until an actual read is consumed. */
struct NU_POST_BLOOM_PARAMETERS { // size 0x54
    i32 enabled;                  // 0x00
    f32 near_angle;               // 0x04
    f32 near_scale;               // 0x08
    f32 far_angle;                // 0x0c
    f32 far_scale;                // 0x10
    f32 intensity;                // 0x14
    i32 clear_screen;             // 0x18
    f32 blur_iterations;          // 0x1c
    f32 blend;                    // 0x20
    f32 threshold;                // 0x24
    i32 directional;              // 0x28
    NUVEC direction;              // 0x2c
    f32 direction_near_angle;     // 0x38
    f32 direction_far_scale;      // 0x3c
    f32 direction_far_angle;      // 0x40
    f32 direction_near_scale;     // 0x44
    f32 direction_bias;           // 0x48
    f32 unknown_4c;               // 0x4c
    f32 unknown_50;               // 0x50
};

#ifdef __cplusplus
struct PC_POST_DATA_PORT_ENTRY { // size 0x28
    void *data;                  // 0x00
    char name[32];               // 0x04
    i32 references;              // 0x24

    bool NameEquals(const char *other);
    void AssignPort(const char *other, void *value);
    void PortReleaseReference();
    i32 PortReferenceCount();
    void SetData(void *value);
    void *GetData();
};

struct PC_POST_DATA_PORT_MANAGER {        // size 0x2800
    PC_POST_DATA_PORT_ENTRY entries[256]; // 0x0000
    PC_POST_DATA_PORT_MANAGER();
    PC_POST_DATA_PORT_ENTRY *ReleaseEntry(i32 index);
    void *GetEntryData(i32 index);
    ~PC_POST_DATA_PORT_MANAGER();
    i32 Register(const char *name, void *data);
};

struct PC_POST_DATA_PORT {              // size 0x8
    i32 index;                          // 0x0
    PC_POST_DATA_PORT_MANAGER *manager; // 0x4

    ~PC_POST_DATA_PORT() {
        if (index >= 0) {
            PC_POST_DATA_PORT_ENTRY *entry = &manager->entries[index];
            --entry->references;
        }
    }
};

extern "C" PC_POST_DATA_PORT PostPortOutFramebuffer;
extern "C" PC_POST_DATA_PORT PostPortColorBuffer;
extern "C" PC_POST_DATA_PORT PostPortNormalBuffer;
extern "C" PC_POST_DATA_PORT PostPortVelocityBuffer;
extern "C" PC_POST_DATA_PORT PostPortDepthRTBuffer;
extern "C" PC_POST_DATA_PORT PostPortDepthBuffer;
#endif

typedef struct NU_POST_DOF_PARAMETERS { // size 0x18
    i32 enabled;                        // 0x00
    f32 strength;                       // 0x04
    f32 near_distance;                  // 0x08
    f32 far_distance;                   // 0x0c
    i32 mode;                           // 0x10
    i32 bias;                           // 0x14
} NU_POST_DOF_PARAMETERS;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 NuPostEffectsRan;
    extern u8 NuPostLastFrameEffect;
    extern f32 aiEditor_LocatorWidth;
    void NuLightSpeedBlur(i32 reuse_camera, f32 scale);
    void NuSpeedBlurSetMotionFactors(f32 pan, f32 pull, f32 clamp);
    extern i32 kPRIM_COORD_LOOKUP[3];
#ifdef __cplusplus
    void NuRndrEndFrameAction(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuDeferredShadingRender(void);
    extern i32 PostShadowCubeFaceIndices[6][4];
    extern f32 PostShadowPlaneExtrusion;
    extern f32 PostBlurHighInputBias;
    extern f32 PostBlurHighOutputBias;
#ifdef __cplusplus
}
#endif

typedef struct nutexdim_s {        // size 0x28
    i32 width;                     // 0x00
    i32 height;                    // 0x04
    u8 checksum[0x10];             // 0x08
    i32 ref_count;                 // 0x18
    void *image_data;              // 0x1c
    u32 image_size;                // 0x20
    PC_TEXTURE_RESOURCE *resource; // 0x24
} NUTEXDIM;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/* A grid laid over the x/z plane: its corner, cell size and cell counts (fun_005e56b0). */
typedef struct NUGRIDINFO { // size 0x20
    f32 min_x;              // 0x00
    u8 unknown_04[4];       // 0x04
    f32 min_z;              // 0x08
    u8 unknown_0c[4];       // 0x0c
    f32 cell_x;             // 0x10
    f32 cell_z;             // 0x14
    i32 count_x;            // 0x18
    i32 count_z;            // 0x1c
} NUGRIDINFO;

typedef struct nuvpcurrent_s { // size 0x40

    union { // size 0x18
        PC_VIEWPORT viewport;

        struct {        // size 0x18
            i32 x;      // 0x00
            i32 y;      // 0x04
            i32 width;  // 0x08
            i32 height; // 0x0c
            f32 min_z;  // 0x10
            f32 max_z;  // 0x14
        };
    }; // 0x00

    f32 center_x;           // 0x18
    f32 center_y;           // 0x1c
    f32 clip_min_x;         // 0x20
    f32 clip_min_y;         // 0x24
    f32 clip_max_x;         // 0x28
    f32 clip_max_y;         // 0x2c
    f32 projection_scale_x; // 0x30
    f32 projection_scale_y; // 0x34
    f32 scissor_scale_x;    // 0x38
    f32 scissor_scale_y;    // 0x3c
} NUVPCURRENT;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUVPCURRENT vpDevice;

    void NuQFntDestroy(VUFNT *);
    extern NUTEXDIM **NuTextureDimensions;
    extern NUVPCURRENT vpCurrent;
    void NuDisplayListStreamAlloc(VARIPTR *cursor, i32 end_unused, i32 count);
    extern VARIPTR PCDisplayMaterialStreamStart;
    extern VARIPTR PCDisplayMaterialStreamEnd;
    NUTEXDIM *NuTextureGetDimensions(i32);
    extern NULSTHDR *g_overrideList;
    extern VARIPTR fstack;
    extern VARIPTR fstack_top;
    extern VARIPTR fstack_limit;
    void NuDisplayListCreateDynMtlList_candidate(VARIPTR *, VARIPTR);

    void NuRndrArenaInit(VARIPTR *, VARIPTR);
    i32 NuTexRegisterDimensions(NUTEXDIM *texture);

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct POST_SHADER_PROGRAM {            // size 0x10
    PC_TEXTURE_RESOURCE *vertex_shader; // 0x0
    PC_TEXTURE_RESOURCE *pixel_shader;  // 0x4
    u32 unknown_08[2];                  // 0x8
};

#endif

typedef struct nuspecial_legacy_s { // size 0x4c
    NUMTX matrix;                   // 0x00
    struct nuinstance_s *instance;  // 0x40
    char *name;                     // 0x44
    u32 flags;                      // 0x48
} NUSPECIAL_LEGACY;

/* The legacy record behind a special handle. */
#define NUSPECIAL_LEGACY_OF(handle) ((NUSPECIAL_LEGACY *)(handle)->special)

typedef struct nudisplay_special_s {      // size 0xd0
    NUMTX instance_matrix;                // 0x00
    NUMTX draw_matrix;                    // 0x40
    NUVEC4 bounds_min;                    // 0x80
    NUVEC4 bounds_max;                    // 0x90
    NUVEC center;                         // 0xa0
    f32 radius;                           // 0xac
    PC_DISPLAY_CLIP_OBJECT *clip_objects; // 0xb0
    char *name;                           // 0xb4
    u32 flags;                            // 0xb8
    f32 *clip_range;                      // 0xbc
    u32 instance_ix;                      // 0xc0

    union { // size 0x4
        struct nuinstanim_s *instance_animation;
        NUMTX *draw_matrix_override; /* -1 or NULL: draw with draw_matrix */
    }; // 0xc4

    i16 wind_speed;     // 0xc8
    i16 wind_scale;     // 0xca
    u8 unknown_00cc[4]; // 0xcc
} NUDISPLAY_SPECIAL;

typedef struct nuinstance_s { // size 0x50
    NUMTX matrix;             // 0x00
    i16 object_index;         // 0x40
    u16 pad_42;               // 0x42

    union { // size 0x4
        u32 flags;

        struct {                        // size 0x4
            u32 visible : 1;            // 0x44:0
            u32 flags_low : 2;          // 0x44:1
            u32 no_visibility_test : 1; // 0x44:3
            u32 flags_high : 28;        // 0x44:4
        };
    }; // 0x44
    struct nuinstanim_s *animation; // 0x48
    u32 pad_4c;                     // 0x4c
} NUINSTANCE;

typedef struct ANI3_JOINT { // size 0x30
    NUVEC4 translation;     // 0x00
    NUVEC4 rotation;        // 0x10
    NUVEC4 scale;           // 0x20
} ANI3_JOINT;

typedef struct ANI3_BUFFER { // size 0x10
    i32 joint_count;         // 0x0
    i16 max_joints;          // 0x4
    u8 use_quaternions;      // 0x6
    u8 pad_07;               // 0x7

    union { // size 0x4
        ANI3_JOINT *joints;
        f32 *animation_values;
    }; // 0x8

    u8 *joint_flags; // 0xc
} ANI3_BUFFER;

typedef void (*ANI3_ROOT_CALLBACK)(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
typedef void (*ANI3_EVAL_CALLBACK)(NUMTX *, void *, NUQUAT *);

typedef struct NUHGOBJ_EVAL_JOINT { // size 0x60
    NUMTX bind_matrix;              // 0x00
    NUVEC pivot;                    // 0x40
    u8 unknown_4c_4f[4];            // 0x4c
    u8 parent_index;                // 0x50
    u8 flags;                       // 0x51
    u8 unknown_52_5f[14];           // 0x52
} NUHGOBJ_EVAL_JOINT;

typedef struct NUHGOBJ_RENDER_PART { // size 0x14
    const char *name;                // 0x00
    nuhspecial_s **rigid;            // 0x04
    nuhspecial_s *smooth;            // 0x08
    nuhspecial_s **alternate_rigid;  // 0x0c
    nuhspecial_s *alternate_smooth;  // 0x10
} NUHGOBJ_RENDER_PART;

typedef struct NUHGOBJ_POI { // size 0x50
    NUMTX matrix;            // 0x00
    u8 unknown_40[4];        // 0x40
    u8 joint_index;          // 0x44
    u8 unknown_45[11];       // 0x45
} NUHGOBJ_POI;

/* NUHGOBJ_POI.joint_index of a point fixed in model space. */
#define NUHGOBJ_POI_NO_JOINT 0xff

typedef struct nuqfnt_render_data_s { // size 0x58
    u32 colour;                       // 0x00
    f32 x;                            // 0x04
    f32 y;                            // 0x08
    f32 z;                            // 0x0c
    NUMTX matrix;                     // 0x10
    f32 x_scale;                      // 0x50
    f32 y_scale;                      // 0x54
} NUQFNT_RENDER_DATA;

#ifdef __cplusplus
class PCFrustumClipSet { // size 0x714
  public:
    PC_FRUSTUM_GROUP first_group; // 0x000
    u8 unknown_0380[0x390];       // 0x380
    i32 active_render_set_count;  // 0x710
    i32 TestBoxes(const NUVEC *, const NUVEC *);
    bool TestBox(const NUVEC *minimum, const NUVEC *maximum, i32 group_index);
    i32 TestSpecial(nuhspecial_s *special, NUMTX *matrix);
    bool TestCenterExtent(const NUVEC *center, const NUVEC *extent, i32 group_index);
    i32 TestCenterExtentBoxes(const NUVEC *center, const NUVEC *extent);
};

#endif
#ifdef __cplusplus
struct PC_OCCLUDER_RECORD { // size 0xa0
    NUVEC4 vertices[4];     // 0x00
    NUVEC4 transformed[4];  // 0x40
    f32 min_x;              // 0x80
    f32 max_x;              // 0x84
    f32 min_y;              // 0x88
    f32 max_y;              // 0x8c
    f32 min_depth;          // 0x90
    f32 depth;              // 0x94
    u8 unknown_98[8];       // 0x98
};

#endif

#ifdef __cplusplus
extern "C" {
#endif
    f32 NuQFntPrintJustifiedRSW(void *, VUFNT *, u16 *, f32, f32, f32, f32, f32, f32, f32, u32, NUMTX *);
    extern NUMTX debris_box_camera_cache;

    typedef struct CAMERA_CLIP_PLANES { // size 0x150
        NUMTX frustum;                  // 0x000
        NUMTX scissor;                  // 0x040
        NUMTX near_far;                 // 0x080
        NUVEC4 near_plane;              // 0x0c0
        NUMTX abs_frustum;              // 0x0d0
        NUMTX abs_scissor;              // 0x110
    } CAMERA_CLIP_PLANES;

    extern CAMERA_CLIP_PLANES CameraClipPlanes;
    void NuSpecialUpdate(nuhspecial_s *special);
    f32 NuSpecialGetAnimEndFrame(nuhspecial_s *special);
    extern NUMTX NuPortalLocalView;
    extern NUMTX NuPortalProjectionScratch;
    extern NUMTX NuPortalProjectionInverse;

    typedef struct nucamerareflect_s { // size 0x84
        u32 unknown_00;                // 0x00
        NUMTX mtx;                     // 0x04
        u32 unknown_44[16];            // 0x44
    } NUCAMERAREFLECT;

    extern NUCAMERAREFLECT global_reflect;
    extern NUMTX NuVpIdentityMatrix;
    extern NUMTX NuCameraScissorProjectionScratch;
    i32 NuSpecialDrawAt(nuhspecial_s *, NUMTX *);
    void NuSpecialBurstDrawAt(nuhspecial_s *special, u32 count, NUMTX *matrices, i32 clip);
    void __fastcall NuQuatMulAligned(const NUQUAT *parent, const NUQUAT *rotation, NUQUAT *out);
    void NuSpecialSetDrawMtx(nuhspecial_s *special, NUMTX *matrix);
    extern NUMTX VisibilityPrimaryPlaneMatrix;
    extern NUMTX VisibilityAlternatePlaneMatrix;
    extern NUMTX VisibilityPrimaryWorldMatrix;
    extern NUMTX VisibilityAlternateWorldMatrix;
    extern NUMTX VisibilityPrimaryAbsoluteMatrix;
    extern NUMTX VisibilityAlternateAbsoluteMatrix;
    void NuAnimBuffCreateScratch(ANI3_BUFFER *buffer);

    typedef struct CAMERA_CLIP_PACKET { // size 0x94
        NUMTX clip;                     // 0x00
        NUMTX projection;               // 0x40
        u32 flags;                      // 0x80
        f32 far_clip;                   // 0x84
        f32 near_clip;                  // 0x88
        f32 x_scale;                    // 0x8c
        f32 y_scale;                    // 0x90
    } CAMERA_CLIP_PACKET;

    void NuCameraTransformScissorClip(NUVEC *, NUVEC *, i32, NUMTX *);
#ifdef __cplusplus
}
#endif

typedef struct NU_POST_SPEED_PARAMETERS { // size 0x8c
    i32 enabled;                          // 0x00
    NUMTX previous;                       // 0x04
    NUMTX current;                        // 0x44
    f32 frame_time;                       // 0x84
    f32 scale;                            // 0x88
} NU_POST_SPEED_PARAMETERS;

typedef struct NUANIMDATA2_SKELETAL { // size 0x18
    NUANIMDATA2 prefix;               // 0x00
    i8 *node_flags;                   // 0x14
} NUANIMDATA2_SKELETAL;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUCAMERA *NuPortalCamera;
    extern NUCAMERA NuPortalDebugCamera;
    extern NUCAMERA NuPortalRestoreCamera;
    void NuAnimData2CalcTime(NUANIMDATA2 *, f32, NUANIM_TIME2 *);
    /* C extern declarations: */
    i32 StateAnimEvaluate2(STATE_ANIM *state, u8 *index, u8 *value, f32 frame);
    i32 StateAnimEvaluate(STATE_ANIM *state, u8 *index, u8 *value, f32 frame);
    STATE_ANIM *StateAnimFixPtrs(STATE_ANIM *state, i32 delta);

    typedef struct CAMERA_SAVED_STATE { // size 0x420
        NUCAMERA camera;                // 0x000
        NUMTX view;                     // 0x078
        NUMTX projection;               // 0x0b8
        NUMTX screen;                   // 0x0f8
        NUMTX view_projection;          // 0x138
        NUMTX view_projection_scissor;  // 0x178
        NUMTX view_projection_viewport; // 0x1b8
        NUMTX viewport;                 // 0x1f8
        NUMTX view_projection_screen;   // 0x238
        NUMTX projection_screen;        // 0x278
        i32 effects;                    // 0x2b8
        f32 zx;                         // 0x2bc
        f32 zy;                         // 0x2c0
        f32 zxs;                        // 0x2c4
        f32 zys;                        // 0x2c8
        u32 unknown_2cc;                // 0x2cc
        CAMERA_CLIP_PLANES planes;      // 0x2d0
    } CAMERA_SAVED_STATE;

    extern CAMERA_SAVED_STATE CameraSavedStates[16];
#ifdef __cplusplus
}
#endif

/* NUDISPLAY_LIST.render_flags bits named where they are used. */
#define NUDISPLAY_RENDER_MATERIAL_ADDED 0x2 /* a material of the scene went into a display list this frame */
#define NUDISPLAY_RENDER_SCENE_ADDED 0x4    /* the scene was added to the render this frame (it has clip entries) */
#define NUDISPLAY_RENDER_MARKED 0x8         /* set by NuDisplayListFlagScenes; nothing in this source reads it */

typedef struct nudisplay_list_s {            // size 0x90
    const char *name;                        // 0x00
    i32 item_count;                          // 0x04
    struct nudisplaylistitem_s *items;       // 0x08
    f32 *fade_ranges;                        // 0x0c
    i32 clip_count;                          // 0x10
    PC_DISPLAY_CLIP_OBJECT *clip_objects;    // 0x14
    u16 *clip_counts;                        // 0x18
    u8 unknown_001c[0xc];                    // 0x1c
    f32 *fade_levels;                        // 0x28
    f32 *far_clip_ranges;                    // 0x2c
    u8 *clip_used[2];                        // 0x30
    u8 unknown_0038[0xc];                    // 0x38
    NUVEC4 *clip_bounds;                     // 0x44
    u8 *visibility_flags;                    // 0x48
    u32 material_count;                      // 0x4c
    NUMTL **display_materials;               // 0x50
    struct nudisplaylist_s **material_lists; // 0x54
    u8 *material_used[2];                    // 0x58
    i32 sort_count;                          // 0x60
    struct nusortpri_s *sort_pris;           // 0x64
    f32 *alpha_values;                       // 0x68
    i32 special_count;                       // 0x6c
    NUDISPLAY_SPECIAL *specials;             // 0x70

    union { // size 0x4
        u32 render_flags;

        struct {                            // size 0x4
            u8 unknown_0074_76[0x2];        // 0x74
            u8 instance_visibility_enabled; // 0x76
            u8 unknown_0077[0x1];           // 0x77
        };

        struct {                         // size 0x4
            u32 : 5;                     // 0x74:0
            u32 material_layer_mask : 8; // 0x74:5
        };
    }; // 0x74
    struct NUGSCN_s *gscene;                          // 0x78
    u32 *native_word;                                 // 0x7c
    struct nuglobalrndrstate_s *local_state;          // 0x80
    u8 unknown_84[0x4];                               // 0x84
    u8 *portal_visibility_overrides;                  // 0x88
    struct PC_MATERIAL_ANIM_SET *material_animations; // 0x8c
} NUDISPLAY_LIST;

/* A material's texture flip-book in a hierarchy: the cut-scene look-at locators pick a frame from it. */
typedef struct NUHGOBJ_TEXTURE_ANIM { // size 0x20
    char *name;                       // 0x00
    u32 unknown_04;                   // 0x04
    i16 *frames;                      // 0x08
    i16 frame_count;                  // 0x0c
    i16 unknown_0e;                   // 0x0e
    struct numtl_s *material;         // 0x10
    u8 unknown_14[12];                // 0x14
} NUHGOBJ_TEXTURE_ANIM;

typedef struct NUHGOBJ_EVAL {                 // size 0x1c4
    u8 unknown_0[0xc];                        // 0x000
    struct numtl_s **materials;               // 0x00c
    i32 material_count;                       // 0x010
    u8 unknown_14[0x4c];                      // 0x014
    i32 texture_count;                        // 0x060
    NUHGOBJ_TEXTURE_ANIM *textures;           // 0x064
    u8 unknown_68[0xa8];                      // 0x068
    struct nudisplay_list_s *display_list;    // 0x110
    u8 data_0x114[84];                        // 0x114
    i32 joint_count;                          // 0x168
    NUHGOBJ_EVAL_JOINT *joints;               // 0x16c
    NUMTX *bind_matrices;                     // 0x170
    NUMTX *inverse_bind;                      // 0x174
    i32 joint_override_map_count;             // 0x178
    u8 *joint_override_map;                   // 0x17c
    u8 data_0x180[4];                         // 0x180
    NUHGOBJ_POI *points;                      // 0x184
    i32 point_index_count;                    // 0x188
    u8 *point_index_map;                      // 0x18c
    i32 render_count;                         // 0x190
    NUHGOBJ_RENDER_PART *render_parts;        // 0x194
    u8 unknown_198[4];                        // 0x198
    struct PC_HG_SHADOW_GROUP *shadow_groups; // 0x19c
    u8 suppress_shadow_points;                // 0x1a0
    u8 unknown_1a1[11];                       // 0x1a1
    NUVEC bounds_min;                         // 0x1ac
    NUVEC bounds_max;                         // 0x1b8
} NUHGOBJ_EVAL;

typedef void (*NUJOINTPROCANIMFN)(ANI3_BUFFER *, NUHGOBJ_EVAL *, i32, NUHGOBJ_JOINT_OVERRIDE *);

typedef struct nurndrlightrecord_s {  // size 0x380
    NUMTX view;                       // 0x000
    NUMTX projection;                 // 0x040
    NUMTX shadow_matrix;              // 0x080
    NUMTX texture_matrix;             // 0x0c0
    f32 parameter_100;                // 0x100
    f32 parameter_104;                // 0x104
    i32 enabled;                      // 0x108
    f32 parameter_10c;                // 0x10c
    f32 parameter_110;                // 0x110
    u8 opaque_114_120[0xc];           // 0x114
    NUVEC4 corners[8];                // 0x120
    NUVEC4 planes[12];                // 0x1a0
    i32 shadow_plane_count;           // 0x260
    u8 opaque_264_270[0xc];           // 0x264
    NUVEC4 far_plane;                 // 0x270
    NUVEC4 endpoint0;                 // 0x280
    NUVEC4 endpoint1;                 // 0x290
    f32 shadow_radius;                // 0x2a0
    NUDISPLAYLIST shadow_lists[2];    // 0x2a4
    NURNDRLIBSTATE *render_states[2]; // 0x32c
    NUDISPLAYLISTITEM *list_items[2]; // 0x334

    /* The shadow producer chains RenderSceneShadow builds per list; operator= copies them as one array. */
    union { // size 0x18
        NUDISPLAYLISTITEM *producer_items[6];

        struct {                                   // size 0x18
            NUDISPLAYLISTITEM *producer_cursor[2]; // 0x33c
            NUDISPLAYLISTITEM *producer_first[2];  // 0x344
            NUDISPLAYLISTITEM *producer_last[2];   // 0x34c
        };
    }; // 0x33c

    i32 pending[8];         // 0x354
    i32 count;              // 0x374
    u8 opaque_378_380[0x8]; // 0x378
#ifdef __cplusplus
    nurndrlightrecord_s();
    nurndrlightrecord_s &operator=(const nurndrlightrecord_s &other);
#endif
} NURNDRLIGHTRECORD;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTL **PCSpecialForcedMaterialTable;
    extern NUMTL *PCSpecialForcedMaterial;
    extern NUMTL *PCMaterialPool;
    void NuRndrRectUV2diZ(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material, i32 z);

    typedef struct NUQFNT_LEGACY_FONT { // size 0x1520
        u8 unknown_00[12];              // 0x0000
        i16 height;                     // 0x000c
        u8 unknown_0e[6];               // 0x000e
        i16 baseline;                   // 0x0014
        i16 ic_gap;                     // 0x0016
        NUMTL *material;                // 0x0018
        i32 glyph_count;                // 0x001c
        u8 character_map[256];          // 0x0020
        FONT_LEGACY_GLYPH glyphs[256];  // 0x0120
    } NUQFNT_LEGACY_FONT;

    extern NUMTL **PCDisplayMaterialAddQueue;
    NUDISPLAYLISTITEM *NuDisplayListCreateMtlDlist(NUDISPLAYLISTITEM *, NUMTL *, VARIPTR *, VARIPTR);
    i32 NuRndrLineStrip2d(NURND_VERTEX2D *vertices, NUMTL *material, i32 count);

    /* The UV-animated materials of one display-list scene, linked into PCMaterialAnimSets. */
    typedef struct PC_MATERIAL_ANIM_SET {  // size 0x10
        NUDISPLAY_LIST *scene;             // 0x0
        i32 material_count;                // 0x4
        i32 *material_indices;             // 0x8
        struct PC_MATERIAL_ANIM_SET *next; // 0xc
    } PC_MATERIAL_ANIM_SET;

    extern NUMTL **PCDisplayMaterialDeleteQueue;
    extern NUDISPLAY_LIST *PCDisplayScenes[256];
    extern NUDISPLAY_LIST PCDynamicDisplayScene;

    void DrawAlphaImage(i32 rows, i32 cols, NUMTL *material, i32 use_pixel_offsets, NU_POST_BLOOM_PARAMETERS *parameters);

    i32 NuDynamicLightIsEnabled(NURNDRLIGHTRECORD *records);

    typedef struct NU_POST_MAIN_VIEW {                // size 0x128
        void *vtable;                                 // 0x000
        u8 enabled;                                   // 0x004
        u8 unknown_005[0x63];                         // 0x005
        const struct NU_POST_BLOOM_PARAMETERS *bloom; // 0x068
        f32 blur_radius;                              // 0x06c
        f32 dof_strength;                             // 0x070
        f32 dof_near;                                 // 0x074
        f32 dof_far;                                  // 0x078
        f32 dof_blur;                                 // 0x07c
        i32 dof_mode;                                 // 0x080
        i32 dof_bias;                                 // 0x084
        u8 dof_enabled;                               // 0x088
        u8 bloom_enabled;                             // 0x089
        u8 unknown_08a[2];                            // 0x08a
        i32 active_filter_count;                      // 0x08c
        NUMTX motion_previous;                        // 0x090
        NUMTX motion_current;                         // 0x0d0
        f32 motion_scale;                             // 0x110
        f32 motion_maximum;                           // 0x114
        f32 motion_falloff;                           // 0x118
        i32 accumulation_frames;                      // 0x11c
        f32 accumulation_blend;                       // 0x120
        f32 accumulation_weight_exponent;             // 0x124
    } NU_POST_MAIN_VIEW;

    typedef struct NU_POST_MOTION_VIEW { // size 0xa0
        void *vtable;                    // 0x00
        u8 enabled;                      // 0x04
        u8 unknown_005[0xf];             // 0x05
        NUMTX previous;                  // 0x14
        NUMTX current;                   // 0x54
        f32 scale;                       // 0x94
        f32 maximum;                     // 0x98
        f32 falloff;                     // 0x9c
    } NU_POST_MOTION_VIEW;

    typedef struct NU_POST_ACCUM_VIEW { // size 0x24
        void *vtable;                   // 0x00
        u8 enabled;                     // 0x04
        u8 unknown_005[0x13];           // 0x05
        i32 frames;                     // 0x18
        f32 blend;                      // 0x1c
        f32 weight_exponent;            // 0x20
    } NU_POST_ACCUM_VIEW;

    typedef struct NU_POST_SPEED_VIEW {                    // size 0x20
        void *vtable;                                      // 0x00
        u8 enabled;                                        // 0x04
        u8 unknown_005[0x17];                              // 0x05
        const struct NU_POST_SPEED_PARAMETERS *parameters; // 0x1c
    } NU_POST_SPEED_VIEW;

    extern NU_POST_MAIN_VIEW *mainFilter;
    extern NU_POST_MOTION_VIEW *motionFilter;
    extern NU_POST_ACCUM_VIEW *motionAccumFilter;
    extern NU_POST_SPEED_VIEW *speedBlurFilter;

    void NuRndrRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material);
    void NuRndrLine2di(i32 x0, i32 y0, i32 x1, i32 y1, i32 colour, NUMTL *material);
    void DisplaySceneRndrSpecials(NUDISPLAY_LIST *, i32, i32);
#ifdef __cplusplus
}
#endif

typedef struct nurndrlibscene_s { // size 0x218
    i32 scene_id;                 // 0x000
    u32 clear_flags;              // 0x004
    u32 clear_flags_08;           // 0x008
    i32 bg_colour;                // 0x00c
    f32 clear_alpha;              // 0x010
    f32 viewport_x;               // 0x014
    f32 viewport_y;               // 0x018
    f32 calc_1c;                  // 0x01c
    f32 calc_20;                  // 0x020
    u32 render_frame_flags;       // 0x024
    u32 burnout_enabled;          // 0x028
    u8 burnout_intensity;         // 0x02c
    f32 burnout_flare;            // 0x030
    f32 burnout_threshold;        // 0x034
    i32 dyn_light_count;          // 0x038
    void *dyn_light_ptr;          // 0x03c
    i32 light_render_set;         // 0x040
    u8 opaque_44_48[0x4];         // 0x044
    u32 deferred_shading_enabled; // 0x048

    union { // size 0xc

        struct {                  // size 0xc
            f32 clear_alpha_4c;   // 0x04c
            u8 opaque_50_58[0x8]; // 0x050
        };

        f32 shadow_coefficients[3];
    }; // 0x04c

    union { // size 0x54

        struct {                   // size 0x54
            u32 bloom_enabled;     // 0x058
            u8 opaque_5c_ac[0x50]; // 0x05c
        };

        BURN_PARAMETERS burnout_parameters;
    }; // 0x058

    NU_POST_DOF_PARAMETERS dof;          // 0x0ac
    u32 texture_blend_enabled;           // 0x0c4
    i32 texture_blend_arg0;              // 0x0c8
    i32 texture_blend_arg1;              // 0x0cc
    NUVEC4 texture_blend_parameters;     // 0x0d0
    u32 motion_params_set;               // 0x0e0
    u32 motion_effect_set;               // 0x0e4
    NUMTX motion_previous;               // 0x0e8
    NUMTX motion_current;                // 0x128
    f32 motion_scale;                    // 0x168
    f32 motion_maximum;                  // 0x16c
    f32 motion_falloff;                  // 0x170
    u32 accumulation_params_set;         // 0x174
    u32 accumulation_effect_set;         // 0x178
    i32 accumulation_frames;             // 0x17c
    f32 accumulation_mode;               // 0x180
    f32 accumulation_blend;              // 0x184
    NU_POST_SPEED_PARAMETERS speed_blur; // 0x188
    i32 backbuffer_copy_texture;         // 0x214
} NURNDRLIBSCENE;

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_GLOBAL_LIGHTING_STATE PCGlobalLightingState;
    extern NURNDRLIBSCENE currentScene;
    extern NUDISPLAYLIST PCDisplayList2d;
    extern NUSORTPRI *sort_list;
    extern NUDISPLAYLISTRENDERSCENE *PCSafeRenderScenes[24];

    extern NUDISPLAYLISTRENDERSCENE *render_scene_ptrs[24];
    void SetProceduralAnimationFn(NUJOINTPROCANIMFN callback);

    /* A scene's visibility result: NuVisiEvaluate fills it, the walkers write two bits per instance into
     * clip_results (1 inside, 2 partial) and set their bit in flags (1 instance tree, 4 octree). */
    typedef struct nuvisibilityresult_s {                   // size 0x2c
        i32 instance_count;                                 // 0x00
        struct nuvisioctree_s *octree;                      // 0x04
        NUGSCN *source_scene;                               // 0x08
        struct visibility_occlusion_grid_s *occlusion_data; // 0x0c
        u8 unknown_10[4];                                   // 0x10
        struct visibility_instance_tree_s *instance_tree;   // 0x14
        void *portal_marker;                                // 0x18
        u8 *portal_bits;                                    // 0x1c
        u8 *clip_results;                                   // 0x20
        u8 *instance_tree_bits;                             // 0x24
        u32 flags;                                          // 0x28
    } NUVISIBILITYRESULT;

    extern NUVISIBILITYRESULT *myvisi;
    extern NURNDRLIBSCENE sceneParameters[16];
    void NuVisiOcclusion(NUVISIBILITYRESULT *);
    NUVISIBILITYRESULT *NuVisiEvaluate(NUGSCN *, void *);
    void NuVisiInstTree(NUVISIBILITYRESULT *, NUGSCN *);
    NUGSCN *PCGraphicsSceneReadClones(VARIPTR *, VARIPTR, char *, i32);
    i32 findrange(struct NUHGOBJ_EVAL *object, i32 first_joint);
    void NuSpecialSetDrawPos(struct nuhspecial_s *special, struct nuvec_s *pos);
#ifdef __cplusplus
}
#endif

struct GRAPHICS_RENDER_TEXTURE {   // size 0x30
    PC_TEXTURE_RESOURCE *resource; // 0x00
    u16 width;                     // 0x04
    u16 height;                    // 0x06
    u16 depth;                     // 0x08
    u16 mip_count;                 // 0x0a

    union { // size 0x1c
        u8 unknown_0c[0x1c];

        struct {                    // size 0x1c
            u16 original_mip_count; // 0x0c
            u16 reserved_0e;        // 0x0e
            u32 texture_type;       // 0x10
            i32 format;             // 0x14
            i32 usage;              // 0x18
            i32 pool;               // 0x1c

            union { // size 0x4
                u32 flags;

                struct {                        // size 0x4
                    u32 sampler_unknown_0 : 8;  // 0x20:0
                    u32 address_u : 3;          // 0x20:8
                    u32 address_v : 3;          // 0x20:11
                    u32 address_w : 3;          // 0x20:14
                    u32 min_filter : 3;         // 0x20:17
                    u32 mag_filter : 3;         // 0x20:20
                    u32 mip_filter : 3;         // 0x20:23
                    u32 sampler_unknown_26 : 6; // 0x20:26
                };
            }; // 0x20

            // Second word of the sampler tuple.
            u32 sampler_extra; // 0x24
        };
    }; // 0x0c

    PC_TEXTURE_RESOURCE *surface; // 0x28
    i32 external_id;              // 0x2c
};

#ifdef __cplusplus
extern "C" {
#endif
    void NuTexAnimResetList(TEXTURE_ANIMATION *animation);

    extern NUFILE nudlist_field_958;
    extern NUFILE NuHtmlFile;
    extern GRAPHICS_RENDER_TEXTURE *NuPostEffectBackBufferTexture;
    extern GRAPHICS_RENDER_TEXTURE *NuPostEffectBackBufferCopy;
#ifdef __cplusplus
}
#endif

/* Native 80bb38 proxy array: five 12-byte records. */
typedef struct PC_POST_BUFFER_PROXY { // size 0xc
    GRAPHICS_RENDER_TEXTURE *texture; // 0x0
    i32 kind;                         // 0x4
    u8 enabled;                       // 0x8
    u8 resolved;                      // 0x9
    u8 unknown_0a[2];                 // 0xa
#ifdef __cplusplus
    void SetProxy(GRAPHICS_RENDER_TEXTURE *texture, i32 kind, u8 enabled, u8 resolved);
    u8 IsResolved();
    void Resolve();
    GRAPHICS_RENDER_TEXTURE *GetBufferTexture();
#endif
} PC_POST_BUFFER_PROXY;

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_POST_BUFFER_PROXY PostProxyBuffers[5];
#ifdef __cplusplus
}
#endif

/* PC NuDynamicLight; its pool slots are 0x830 bytes. */
struct PC_SPECIAL_SHADOW_LIGHT {      // size 0x828
    NURNDRLIGHTRECORD render_sets[2]; // 0x000

    union { // size 0xc
        f32 split_distances[3];

        struct {                 // size 0xc
            u8 unknown_700[4];   // 0x700
            f32 shadow_distance; // 0x704
            u8 unknown_708[4];   // 0x708
        };
    }; // 0x700

    i32 render_set_count;        // 0x70c
    i32 active_render_set_count; // 0x710
    u8 unknown_714[0xc];         // 0x714
    NUVEC4 split_planes[2];      // 0x720
    NUVEC4 eye_position;         // 0x740
    NUVEC4 camera_forward;       // 0x750
    NUVEC4 light_position;       // 0x760

    union { // size 0x10
        NUVEC4 light_direction4;

        struct {                    // size 0x10
            NUVEC shadow_direction; // 0x770
            f32 light_direction_w;  // 0x77c
        };
    }; // 0x770

    NUMTX view;           // 0x780
    NUMTX projection;     // 0x7c0
    i32 parameter_4;      // 0x800
    i32 parameter_5;      // 0x804
    i32 enabled;          // 0x808
    f32 parameter_0;      // 0x80c
    f32 parameter_1;      // 0x810
    f32 parameter_2;      // 0x814
    f32 parameter_3;      // 0x818
    f32 parameter_19;     // 0x81c
    f32 parameter_20;     // 0x820
    i32 used_on_specials; // 0x824
#ifdef __cplusplus
    PC_SPECIAL_SHADOW_LIGHT();
    void LookAt(NUVEC *, NUVEC *, NUVEC *);
    void SetupCustomCameraFrustum(NUCAMERA *, const f32 *, i32);
    void RenderSceneShadow(NUGSCN *scene);
    i32 TestSphere(const NUVEC *centre, f32 radius, i32 set);
    void ComputeShadowMatrix(NURNDRLIGHTRECORD *);
    void SetupShadow(NURNDRLIGHTRECORD *);
    void RenderShadow(i32, PC_FRAMEBUFFER *);
    void RenderConstants(void *);
    void SetEnabled(i32 enabled);

    i32 IsEnabled() {
        return enabled != 0;
    }
    void BeginCapture(i32 render_set);

    void SetParameterf(i32 parameter, f32 value);
    i32 GetUsedOnSpecials();
    void SetUsedOnSpecials(i32 used);
    NUDISPLAYLIST *GetShadowList(i32 set, NUMTL *material);
    void SetPostShadowMatrices(NUMTX *view, NUMTX *projection);
    i32 GetActiveRenderSetCount();
    NUMTX *GetView();
    NUMTX *GetProjection();
    f32 GetParameterf(i32 parameter);
    void SetParameteri(i32 parameter, i32 value);
    i32 GetParameteri(i32 parameter);
    void QueueScene(i32 set, i32 scene);
    PC_SPECIAL_SHADOW_LIGHT *Clone(VARIPTR *arena, VARIPTR end);
    PC_SPECIAL_SHADOW_LIGHT &operator=(const PC_SPECIAL_SHADOW_LIGHT &other);
    void SetOrthoProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
#endif
};

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

struct PC_FRAMEBUFFER {           // size 0xa4
    PC_EFFECT_SURFACE targets[5]; // 0x00
    u32 clear_colour[5];          // 0x8c
    i32 changed;                  // 0xa0
#ifdef __cplusplus
    PC_FRAMEBUFFER();
    ~PC_FRAMEBUFFER();
#endif
};

#ifdef __cplusplus
extern "C" {
#endif
    i32 PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE *);
    i32 PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE *);
    void PCDeviceClear(u32, const RECT *, u32, u32, f32, u32);
#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern struct numtl_s *pAlphaMask;
    extern struct numtl_s *pZClearMaterial;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
#endif

typedef struct NU_POST_DEFERRED_VIEW {         // size 0xdc
    void *vtable;                              // 0x00
    u8 enabled;                                // 0x04
    u8 unknown_005[0x3f];                      // 0x05
    f32 parameters[4];                         // 0x44
    i32 dynamic_light_count;                   // 0x54
    struct NuDynamicLight *dynamic_lights[32]; // 0x58
    i32 deferred_geometry_count;               // 0xd8
} NU_POST_DEFERRED_VIEW;

#ifdef __cplusplus
extern "C" {
#endif
    extern NU_POST_DEFERRED_VIEW *deferredFilter;
    void NuRndrGradRect2di(i32 x, i32 y, i32 width, i32 height, u32 *colours, NUMTL *material);
    void NuRndrLineStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);
    void NuRndrTriStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);
    extern NUMTL *NuTimeBar_FrameOutMtl;
    extern NUVEC4 PostShadowWhite;
    extern PC_FRAMEBUFFER *PostBlurFramebuffer;
    extern PC_FRAMEBUFFER *PostCopyFramebuffer;
    extern PC_TEXTURE_RESOURCE *RendererVertexDeclaration;
    extern const WCHAR PostDeferredLabel[17];
    extern const WCHAR PostEffectRenderLabel[];
    extern const WCHAR PostShadowRenderLabel[14];
    extern i32 NuCameraScissorHeight;
    extern i32 NuCameraScissorWidth;
    extern i32 RendererArenaVertexCount;
    extern i32 dat_00806b84;
    extern i32 dat_00806b88;
    extern i32 dat_00806b8c;
    extern i32 dat_00806b90;
    extern i32 NuDebugGeometrySortOutput;
    extern i32 dat_0080b6e4;
    extern i32 NuDebugDisabled;
    extern i32 motionBlurAccumActiveThisFrame;
    extern i32 mtl_animation_mask;
    extern i32 nuqfnt_initialised;
    extern u8 PostBloomAttachPending;
    extern u8 dat_009737ff;
    extern void *NuTexScratchBlock;
    void NuDisplayListHtmlEnd(void);
    void NuRndrArenaBuildShadowVolumes(i32 unused0, i32 unused1);
    void NuRndrInitLeaf(VARIPTR *buffer, VARIPTR end);
    void NuRndrRectUV2dNoScale(f32 x, f32 y, f32 z, f32 width, f32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material);
    void NuRndrSphereMtx(NUMTX *matrix, i32 colour, i32 segments, NUMTL *material);
    void PostFilterInitSharedResources(void);
    void NuTexDestroyId(i32 texture_id);
    NUVPCURRENT *NuVpGetCurrentViewport(void);
    extern NUMTX PostShadowProjection;
    extern NUMTX PostShadowView;
    extern NUVEC ShadowLightUp;
    extern f32 ShadowLightBlendPower;
    extern f32 ShadowLightEyeDistance;
    extern f32 ShadowLightFallbackHeight;
    extern f32 ShadowLightFocusHeight;
    extern f32 ShadowLightNearOffset;
    extern f32 ShadowLightSplitScale;
    extern i32 ShadowLightSplitCount;
    void NuPortalSetActive(NUGSCN *scene, i32 portal_id, i32 active);
    i32 NuSpecialClipTestExtents(nuhspecial_s *special, NUMTX *matrix);
    u8 NuAnimSetForceEulerToQuat(u8 force);
    extern GRAPHICS_RENDER_TEXTURE *PCGraphicsPostEffectAuxTexture;
    extern GRAPHICS_RENDER_TEXTURE *PCGraphicsPostEffectWorkTexture;
    f32 NuQFntGetLengthScale(void);
    f32 NuQFntGetHeightScale(void);
    void NuMtlAnimateSetMask(i32 mask);
    void NuMtlAnimateSetSpeedScale(f32 scale);
    void SetSpotLightMode(void);

    typedef struct screengrab_tile_params_s { // size 0x6
        i8 channels[4];                       // 0x0
        u16 width;                            // 0x4
    } SCREENGRAB_TILE_PARAMS;

    void NuRndrScreenGrabTileInit(SCREENGRAB_TILE_PARAMS *params, i32 tiles, f32 b, f32 a, f32 c);
    void NuRndrScreenGrabTileDeInit(SCREENGRAB_TILE_PARAMS *params);
    i32 NuRndrSetViewMtx(NUMTX *view_projection, NUMTX *viewport_clip, NUMTX *scissor_clip);
    /* An empty hook the callers pass their arguments to; C callers see it unprototyped. */
#ifdef __cplusplus
    void PCSpecialEmptyHandler(...);
#else
void PCSpecialEmptyHandler();
#endif
#ifdef __cplusplus
}
#endif
