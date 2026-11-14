#ifndef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: decomp.h, legoapi/legoapi_types.h, legoapi/render/core/gameliball.h,
 * nu2api/nu3d/nu3d_includes.h, nu2api/nu3d/nucamera.h, nu2api/nu3d/nudlist.h, nu2api/nu3d/nugscn.h,
 * nu2api/nu3d/numtl.h, nu2api/nu3d/nurndr.h, nu2api/nucamera_dlist.h, nu2api/nuhgo_dlist.h, nu2api/numath/numtx.h,
 * nu2api/numath/nuvec.h, nu2api/numath/nuvec4.h, nu2api/nuwin32/d3dCore.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nuwin32/d3dCore.h"
#endif
/* 7 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 1 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)

typedef struct __declspec(align(16)) NUVEC4BLOCK_PC {
    f32 x[4];
    f32 y[4];
    f32 z[4];
} NUVEC4BLOCK_PC;

STATIC_ASSERT(sizeof(NUVEC4BLOCK_PC) == 0x30);
STATIC_ASSERT(offsetof(NUVEC4BLOCK_PC, x) == 0);
STATIC_ASSERT(offsetof(NUVEC4BLOCK_PC, y) == 0x10);
STATIC_ASSERT(offsetof(NUVEC4BLOCK_PC, z) == 0x20);

typedef struct PC_TEXTURE_LEVEL_DESC_PC {
    u32 format;
    u32 type;
    u32 usage;
    u32 pool;
    u32 unknown_10[2];
    u32 width;
    u32 height;
} PC_TEXTURE_LEVEL_DESC_PC;

STATIC_ASSERT(sizeof(PC_TEXTURE_LEVEL_DESC_PC) == 0x20);
STATIC_ASSERT(offsetof(PC_TEXTURE_LEVEL_DESC_PC, format) == 0);
STATIC_ASSERT(offsetof(PC_TEXTURE_LEVEL_DESC_PC, width) == 0x18);
STATIC_ASSERT(offsetof(PC_TEXTURE_LEVEL_DESC_PC, height) == 0x1c);

typedef struct nutexbmp_colour_pc_s {
    u8 blue;
    u8 green;
    u8 red;
    u8 alpha;
} NUTEXBMP_COLOUR_PC;

typedef struct nutexbmp_pc_s {
    i32 type;
    i32 width;
    i32 height;
    i32 mips;
    u8 *data;
    u8 *palette;
    u32 unknown_18[3];
} NUTEXBMP_PC;

STATIC_ASSERT(offsetof(NUTEXBMP_PC, data) == 0x10);
STATIC_ASSERT(offsetof(NUTEXBMP_PC, palette) == 0x14);
STATIC_ASSERT(sizeof(NUTEXBMP_PC) == 0x24);

typedef struct PC_VIEWPORT_PC {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
    f32 min_z;
    f32 max_z;
} PC_VIEWPORT_PC;

typedef struct nulgtvertex_pc_s {
    f32 x;
    f32 y;
    f32 z;
    u32 colour;
    f32 u;
    f32 v;
} NULGTVERTEX_PC;

STATIC_ASSERT(offsetof(NULGTVERTEX_PC, colour) == 0xc);
STATIC_ASSERT(offsetof(NULGTVERTEX_PC, u) == 0x10);
STATIC_ASSERT(offsetof(NULGTVERTEX_PC, v) == 0x14);

typedef struct PC_BUFFER_RESOURCE_PC PC_BUFFER_RESOURCE_PC;

typedef struct PC_BUFFER_RESOURCE_METHODS_PC {
    void *unknown_0000[2];
    ULONG(__stdcall *release)(PC_BUFFER_RESOURCE_PC *);
    void *unknown_000c[8];
    HRESULT(__stdcall *lock)(PC_BUFFER_RESOURCE_PC *, u32, u32, void **, u32);
    HRESULT(__stdcall *unlock)(PC_BUFFER_RESOURCE_PC *);
} PC_BUFFER_RESOURCE_METHODS_PC;

struct PC_BUFFER_RESOURCE_PC {
    const PC_BUFFER_RESOURCE_METHODS_PC *methods;
};

STATIC_ASSERT(offsetof(PC_BUFFER_RESOURCE_PC, methods) == 0);
STATIC_ASSERT(offsetof(PC_BUFFER_RESOURCE_METHODS_PC, release) == 8);
STATIC_ASSERT(offsetof(PC_BUFFER_RESOURCE_METHODS_PC, lock) == 0x2c);
STATIC_ASSERT(offsetof(PC_BUFFER_RESOURCE_METHODS_PC, unlock) == 0x30);

typedef struct PC_EFFECT_SURFACE_PC PC_EFFECT_SURFACE_PC;

STATIC_ASSERT(sizeof(PC_VIEWPORT_PC) == 24);
STATIC_ASSERT(offsetof(PC_VIEWPORT_PC, x) == 0);
STATIC_ASSERT(offsetof(PC_VIEWPORT_PC, y) == 4);
STATIC_ASSERT(offsetof(PC_VIEWPORT_PC, width) == 8);
STATIC_ASSERT(offsetof(PC_VIEWPORT_PC, height) == 12);
STATIC_ASSERT(offsetof(PC_VIEWPORT_PC, min_z) == 0x10);
STATIC_ASSERT(offsetof(PC_VIEWPORT_PC, max_z) == 0x14);
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_SPECIAL_PROJECTION_ADJUST PCSpecialProjectionAdjust;

    extern i32 PCShadowSubmissionEnabled;
    extern i32 dat_0097391c;
    extern u32 dat_0280d838;
    extern i32 dat_02854cb0;
    extern i32 dat_02854cac;
    extern i32 dat_0080e21c;
    extern i32 dat_02854c90;
    extern i32 dat_02854c94;
    extern i32 dat_0080e6a4;
    extern f32 dat_0097f990;
    extern f32 dat_0280d3a0;
    extern i32 dat_02854f54;
    extern i32 PCShadowPacketSequence;
    extern f32 PCShadowNearValue;
    extern f32 PCShadowFarValue;
    extern i32 PCShadowPacketCount;

    typedef struct PC_SHADOW_QUEUE_RECORD {
        NUVEC position;
        f32 radius;
        u16 opacity;
        u8 unknown_12[6];
    } PC_SHADOW_QUEUE_RECORD;

    STATIC_ASSERT(sizeof(PC_SHADOW_QUEUE_RECORD) == 24);
    STATIC_ASSERT(offsetof(PC_SHADOW_QUEUE_RECORD, radius) == 0xc);
    STATIC_ASSERT(offsetof(PC_SHADOW_QUEUE_RECORD, opacity) == 0x10);
    extern i32 NuPrimVertexCount;
    extern i32 *NuPrimVertexCountPointer;
    extern i32 NuPrimCurrentPrimitiveType;
    extern f32 NuPrimXScale;
    extern f32 NuPrimYScale;
    extern f32 NuPrimXBias;
    extern f32 NuPrimYBias;
    extern const f32 NuPrimPS2XScale;
    extern const f32 NuPrimPS2YScale;
    extern const f32 nu_fsign_minus_one;
    extern u32 nuprim_field_ba8;
    extern f32 NuRndrFxHeightOverridePc;
    extern f32 NuRndrFxHeightPc;
    extern f32 NuRndrFxPreviousHeightPc;
    extern u32 NuRndrFxColourPc;
    extern PC_BUFFER_RESOURCE_PC *NuDebrisVertexBuffers[4];
    extern void *NuDebrisSystemBuffers[8];
    extern i32 NuDebrisUseSystemMemory;
    extern u32 NuDebrisMaxVertexCount;
    extern void *NuDebrisVertexData;
    extern u32 NuDebrisFrameVertexCount;

    typedef struct debris_particle_chunk_pc_s DEBRIS_PARTICLE_CHUNK_PC;
    void PCDeviceCreateVertexBuffer(u32, u32, u32, i32, PC_BUFFER_RESOURCE_PC **);
    void PCDeviceCreateIndexBuffer(u32, u32, i32, i32, PC_BUFFER_RESOURCE_PC **, HANDLE *);
    extern i32 nu_water_reset_state;
    extern i32 nu_water_reset_status;
    extern i32 nu_water_flow_state;
    extern u8 NuPortalVisibilityBitsPc[625];
    extern const f64 NuVpFixed16ScalePc;
    extern const f64 NuVpDepthRangePc;
    extern f32 NuCameraProjectionGlobalScaleX;
    extern f32 NuCameraProjectionGlobalScaleY;
    extern f32 NuCameraProjectionGlobalOffsetX;
    extern f32 NuCameraProjectionGlobalOffsetY;
    extern i32 PCClipExtentsEnabled;
    extern const u32 ClipOutcodeMask32[4];
    extern const u32 ClipOutcodeMask16[4];
    extern const u32 ClipOutcodeMask1[4];
    extern const u32 ClipOutcodeMask2[4];
    extern const u32 ClipOutcodeMask8[4];
    extern const u32 ClipOutcodeMask4[4];
    extern const f32 ClipOutcodeMinusOne[4];
    extern f32 ClipFarLimits[4];
    extern u32 clip_outcode_workspace_prefix[8];
    extern NUVEC4BLOCK_PC ClipCornerBlocks[2];
    extern NUVEC4BLOCK_PC ClipTransformedBlocks[2];
    extern i32 PCDeviceSamplerInvalidation[16][2];
    extern DWA_WEIGHT_BLOCK_PC DwaWeightPoolPc[60669];
    extern DWA_WEIGHT_BLOCK_PC *DwaPointerPoolPc[2048];
    extern u8 NuPrimNeedsOverbrightening;
    extern i32 NuUnresolvedTextureIDPC;
    extern u8 NuUnresolvedTextureDdsPc[0x2080];
    extern const u8 NuUnresolvedTextureDDS[8320];
    extern char NuTexBmpErrorTextPc[0x80];
    extern NUTEXBMP_COLOUR_PC NuTexBmpPalettePc[0x100];
    extern i32 NuQFntScanXPc;
    extern i32 NuQFntScanYPc;
    extern const char NuTextureSourcePathPC[55];
    extern const char NuQFntSourcePathPC[56];
    extern i32 SceneUploadBufferSizePC;
    extern const char SceneUploadSourcePathPC[57];
#define nudlist_flags (PCDynamicDisplayScene.render_flags)    // NOLINT(readability-identifier-naming)
#define nudlist_field_e58 (PCDynamicDisplayScene.native_word) // NOLINT(readability-identifier-naming)
    void PCDeviceDrawPrimitiveUPPc(i32, u32, const void *, u32);
    extern const char NuTexturePhongCubePathPc[];
    extern RENDER_POST_VIEWPORT_PREFIX_PC RenderPostViewportPrefixPc;
    extern u32 NuDebrisIdealDynamicVBSizePc;
    extern u8 PCGraphicsRenderTextureActive[32];
    extern i32 PCGraphicsRenderTextureBytes;
    void NuRndrQuadBufferInitPc(void);
#ifdef __cplusplus
}
#endif

struct PC_GEOMETRY_BASIS_PC;
struct PC_GEOMETRY_OUTPUT_PC;

/* Public interfaces for connected motion, title callbacks and geometry readers.
 * Private PC field cohorts and their offset assertions remain in the owners. */
struct PC_DYNAMIC_GEOMETRY_PC;
struct PC_GEOMETRY_SORT_ITEM_PC;
struct PC_GEOMETRY_SKIN_PC;
struct PC_GEOMETRY_MESH_PC;
typedef struct PC_GEOMETRY_BASIS_SOURCE_PC PC_GEOMETRY_BASIS_SOURCE_PC;
typedef struct PC_GEOMETRY_MORPH_WEIGHTS_PC PC_GEOMETRY_MORPH_WEIGHTS_PC;
#ifdef __cplusplus
extern "C" {
#endif
    void PcResetGeometryOriginStatePc(void);
    extern u32 RenderFrameSceneFlagsPc;
#ifdef __cplusplus
}
#endif

// NOLINTNEXTLINE(readability-identifier-naming)
#define PcBootDefaultFramebuffer (PcBootFramebuffersPc.fields.default_framebuffer)
#ifdef __cplusplus
extern "C" {
#endif
    extern void *NuDisplayHandlerMaterialPc;
    extern i32 NuDisplayHandlerCameraIdPc;
    extern f32 NuDisplayHandlerShadowPc;
    extern i32 PCGraphicsBoundShader;
    void PCDeviceDrawPrimitivePc(i32, u32, u32);
    void PCDeviceDrawIndexedPrimitivePc(i32, i32, u32, u32, u32, u32);
    extern i32 NuDisplayMaterialDistortTexturePc;
    extern PC_DISPLAY_ITEM_HANDLER_PC PCDisplayItemHandlerPrefixPc[49];
    extern PC_DISPLAY_ITEM_HANDLER_PC *CurrentItemTable;
#ifdef __cplusplus
}
#endif

typedef struct PC_LIGHT_STATE_SLOT_PC {
    u8 unknown_00[0x94];
    i32 next;
    i32 previous;
} PC_LIGHT_STATE_SLOT_PC;

STATIC_ASSERT(sizeof(PC_LIGHT_STATE_SLOT_PC) == 0x9c);
STATIC_ASSERT(offsetof(PC_LIGHT_STATE_SLOT_PC, next) == 0x94);
STATIC_ASSERT(offsetof(PC_LIGHT_STATE_SLOT_PC, previous) == 0x98);
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_LIGHT_STATE_SLOT_PC NuLgtStateSlotsPc[8];
    extern i32 NuLgtStateCapacityPc;
    extern i32 NuLgtStateActiveCountPc;
    extern i32 NuLgtStateFreeHeadPc;
    extern i32 NuLgtStateEnabledPc;
    extern i32 NuLgtStateUnknownPc;

    typedef struct PC_GOBJ_REGISTRY_AUX_PC {
        struct PC_GOBJ_REGISTRY_AUX_PC *next;
        u8 unknown_04[8];
        void *payload;
    } PC_GOBJ_REGISTRY_AUX_PC;

    STATIC_ASSERT(offsetof(PC_GOBJ_REGISTRY_AUX_PC, payload) == 0xc);
    extern i32 NuGobjRegistryInitializedPc;
    extern i32 NuGobjRegistryCountPc;
    extern const char NuGobjSourcePathPc[51];
#ifdef __cplusplus
    void PCGraphicsSubmitViewportPc(i32, i32, i32, i32);
    void PCGraphicsSetSamplerTuplePc(u32, const u32 *);
    void PCGraphicsDrawPostEffectQuadPc(void);
    void PCGraphicsDrawPostEffectIndexedPc(void);
#ifdef __cplusplus
}
#endif
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 2 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)

typedef struct debris_part_header_pc_s {
    u8 fields_000[16];
    f32 gravity;
    f32 last_render_time;
    u8 fields_018[16];
    f32 texture_u0;
    f32 texture_v0;
    f32 texture_u1;
    f32 texture_v1;
    u8 fields_038[20];
    DEBRIS_PARTICLE_FRAME_PC frames[64];
} DEBRIS_PART_HEADER_PC;

STATIC_ASSERT(sizeof(DEBRIS_PART_HEADER_PC) == 0xa4c);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, gravity) == 0x10);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, last_render_time) == 0x14);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, texture_u0) == 0x28);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, texture_v0) == 0x2c);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, texture_u1) == 0x30);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, texture_v1) == 0x34);
STATIC_ASSERT(offsetof(DEBRIS_PART_HEADER_PC, frames) == 0x4c);

typedef struct SCENE_VIDEO_RESOURCE_HEADER_PC {
    u16 vertex_buffer_count;
    u16 unknown_02;
    PC_BUFFER_RESOURCE_PC **vertex_buffers;
    u16 index_buffer_count;
    u16 unknown_0a;
    PC_BUFFER_RESOURCE_PC **index_buffers;
    u16 texture_count;
    u16 unknown_12;
    PC_TEXTURE_RESOURCE_PC **textures;
} SCENE_VIDEO_RESOURCE_HEADER_PC;

STATIC_ASSERT(sizeof(SCENE_VIDEO_RESOURCE_HEADER_PC) == 0x18);
STATIC_ASSERT(offsetof(SCENE_VIDEO_RESOURCE_HEADER_PC, vertex_buffer_count) == 0x0);
STATIC_ASSERT(offsetof(SCENE_VIDEO_RESOURCE_HEADER_PC, vertex_buffers) == 0x4);
STATIC_ASSERT(offsetof(SCENE_VIDEO_RESOURCE_HEADER_PC, index_buffer_count) == 0x8);
STATIC_ASSERT(offsetof(SCENE_VIDEO_RESOURCE_HEADER_PC, index_buffers) == 0xc);
STATIC_ASSERT(offsetof(SCENE_VIDEO_RESOURCE_HEADER_PC, texture_count) == 0x10);
STATIC_ASSERT(offsetof(SCENE_VIDEO_RESOURCE_HEADER_PC, textures) == 0x14);
#ifdef __cplusplus
extern "C" {
#endif
    extern VARIPTR *NuPrimStreamBufferPointer;
    extern NUVPCURRENT vpDevice;
    extern NUDISPLAYLISTITEM nudlist_2d_static;
    extern VARIPTR nudlist_stream_cursor;
    void PCDeviceCreateVertexDeclaration(const PC_VERTEX_ELEMENT_PC *, PC_TEXTURE_RESOURCE_PC **);
    extern DEBRIS_PART_HEADER_PC *NuDebrisLastHeader;
    extern SCENE_VIDEO_RESOURCE_HEADER_PC SceneVideoResourcesPC;
    extern VARIPTR SceneUploadBufferStartPC;
    extern VARIPTR SceneUploadBufferCursorPC;
    extern VARIPTR SceneUploadBufferEndPC;
    extern PC_TEXTURE_RESOURCE_PC *NuTextureEnvironmentCubePc;
    extern PC_TEXTURE_RESOURCE_PC *NuTexturePhongCubePc;
    void PCDeviceCreateDepthSurface(u32, u32, i32, i32, u32, BOOL, PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    void PCDeviceCreateTexture(u32, u32, u32, u32, i32, i32, PC_TEXTURE_RESOURCE_PC **);
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayHandlerVertexShaderCachePc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayHandlerPixelShaderCachePc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayMaterialDebrisDeclarationPc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayMaterialFaceOnDeclarationPc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayMaterialDefaultDeclarationPc;

    typedef struct PC_GOBJ_NATIVE_RESOURCE_PC {
        u8 unknown_00[0x10];
        PC_TEXTURE_RESOURCE_PC *resource;
    } PC_GOBJ_NATIVE_RESOURCE_PC;

    STATIC_ASSERT(offsetof(PC_GOBJ_NATIVE_RESOURCE_PC, resource) == 0x10);

    typedef struct PC_GOBJ_REGISTRY_CHILD_PC {
        struct PC_GOBJ_REGISTRY_CHILD_PC *next;
        u8 unknown_04[0x1c];
        void *owned_buffer;
        u8 unknown_24[8];
        PC_GOBJ_REGISTRY_AUX_PC *auxiliary;
        u8 unknown_30[8];
        PC_GOBJ_NATIVE_RESOURCE_PC *native_resource;
    } PC_GOBJ_REGISTRY_CHILD_PC;

    STATIC_ASSERT(offsetof(PC_GOBJ_REGISTRY_CHILD_PC, owned_buffer) == 0x20);
    STATIC_ASSERT(offsetof(PC_GOBJ_REGISTRY_CHILD_PC, auxiliary) == 0x2c);
    STATIC_ASSERT(offsetof(PC_GOBJ_REGISTRY_CHILD_PC, native_resource) == 0x38);

    typedef struct PC_GOBJ_REGISTRY_ENTRY_PC {
        struct PC_GOBJ_REGISTRY_ENTRY_PC *next;
        struct PC_GOBJ_REGISTRY_ENTRY_PC *previous;
        u8 unknown_08[4];
        PC_GOBJ_REGISTRY_CHILD_PC *children;
        PC_GOBJ_REGISTRY_AUX_PC *auxiliary;
    } PC_GOBJ_REGISTRY_ENTRY_PC;

    STATIC_ASSERT(offsetof(PC_GOBJ_REGISTRY_ENTRY_PC, children) == 0xc);
    STATIC_ASSERT(offsetof(PC_GOBJ_REGISTRY_ENTRY_PC, auxiliary) == 0x10);
    extern PC_GOBJ_REGISTRY_ENTRY_PC *NuGobjRegistryFirstPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 3 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)

/* One saved camera of the PC camera stack (0x280a598, 64 entries) with the render-state camera id. */
typedef struct NU_CAMERA_STACK_ENTRY_PC {
    NUCAMERA camera;        // 0x00
    i32 camera_id;          // 0x78
} NU_CAMERA_STACK_ENTRY_PC; // size 0x7c
#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX PCSpecialProjectionTranslate;
    extern NUMTX PCSpecialProjectionScale;
    extern struct PC_HG_SHADOW_PACKET PCShadowPackets[280];
    extern NU_CAMERA_STACK_ENTRY_PC dat_0280a598[64];
    extern NU_CAMERA_STACK_ENTRY_PC *dat_0080e864;
    void PCEnqueueHierarchyShadowGroup(NUMTX *, void *, i32, void *, i32, void *, i32);
    extern NUMTX NuRndr_DebrisMtx;
    extern NUVEC4 NuRndr_DebrisPlane;
    extern NUMTX *NuRndr_DebrisRotMtxPtr;
    extern NUMTX NuCameraScalingTemplate;
    extern NUVEC4 PcGeometryDistanceOriginPc;
    void PcUpdateGeometryCameraOriginPc(NUMTX *, NUMTX *);
    extern NUVEC4 PcGeometryDefaultOriginPc;
    extern NUMTX PcGeometryViewMatrixPc;
    extern NUMTX PcGeometryViewProjectionMatrixPc;
    extern NUMTX PcGeometryProjectionMatrixPc;
    extern NUMTX PcGeometryInverseViewProjectionMatrixPc;
    extern NUMTX PcGeometryResetMatrixPc;
    extern NUMTX PcGeometryAdjustedProjectionMatrixPc;
    extern NUMTX PcGeometryTranslationMatrixPc;
    extern NUMTX PcGeometryScaleMatrixPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 4 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTL *dat_02854d00;
    extern struct PC_HG_SHADOW_STATE_VIEW PCShadowState;
    void NuHGobjSubmitShadowGroupsPc(NUHGOBJ_EVAL_PC *, NUMTX *, NUMTX *);
    extern NURND_VERTEX3D_PC NuRndrShadowVerticesPc[4];

    typedef struct NATIVE_PARTICLE_PACKET_PC {
        u8 vertex_buffer_index;
        u8 fields_001[3];
        i32 use_system_memory_vb;
        i32 first_vertex;
        i32 vertex_count;
        NUMTL *material;
    } NATIVE_PARTICLE_PACKET_PC;

    STATIC_ASSERT(sizeof(NATIVE_PARTICLE_PACKET_PC) == 0x14);
    STATIC_ASSERT(offsetof(NATIVE_PARTICLE_PACKET_PC, vertex_buffer_index) == 0);
    STATIC_ASSERT(offsetof(NATIVE_PARTICLE_PACKET_PC, use_system_memory_vb) == 4);
    STATIC_ASSERT(offsetof(NATIVE_PARTICLE_PACKET_PC, first_vertex) == 8);
    STATIC_ASSERT(offsetof(NATIVE_PARTICLE_PACKET_PC, vertex_count) == 0xc);
    STATIC_ASSERT(offsetof(NATIVE_PARTICLE_PACKET_PC, material) == 0x10);
    extern NUGSCN *PCRegisteredNativeScenes[100];
    extern NUMTL *PCStartupMaterial1Pc;
    extern NUMTL *PCStartupMaterial2Pc;
    extern PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC PCGraphicsFrameScratchCount;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 5 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    struct PC_EFFECT_SURFACE_PC {
        GRAPHICS_RENDER_TEXTURE_PC *texture;
        u8 owns_texture; /* 0x05..0x07 alignment padding; no char arrays, so locals carry no /GS cookie */
        PC_TEXTURE_RESOURCE_PC *surface;
        u16 level;
        u16 unknown_0e;
        u32 field_10;
        u16 width;
        u16 height;
        i16 registry_index; /* 0x1a..0x1b alignment padding */
#ifdef __cplusplus
        PC_EFFECT_SURFACE_PC();
        ~PC_EFFECT_SURFACE_PC();
        void Reset();
        void Attach(GRAPHICS_RENDER_TEXTURE_PC *, i32, bool);
        static void ReleaseResources();
        static void RestoreResources();
        GRAPHICS_RENDER_TEXTURE_PC *fun_005f7af0();
        u16 fun_005f7b00();
        u32 fun_005f7b10();
        u16 fun_005f7b20();
        u16 fun_005f7b30();
        void fun_005fb920(PC_EFFECT_SURFACE_PC *source);
#endif
    };

    STATIC_ASSERT(sizeof(PC_EFFECT_SURFACE_PC) == 0x1c);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, texture) == 0);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, owns_texture) == 4);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, surface) == 8);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, level) == 0xc);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, field_10) == 0x10);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, width) == 0x14);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, height) == 0x16);
    STATIC_ASSERT(offsetof(PC_EFFECT_SURFACE_PC, registry_index) == 0x18);

    extern GRAPHICS_RENDER_TEXTURE_PC PCGraphicsRenderTextures[32];

#ifdef __cplusplus
}
#endif

/* Minimum selected native producer/reader view; original allocation unknown. */
typedef struct FRAME_NATIVE_TEXTURE_PC {
    NUTEXDIM_PC dimensions;
    GRAPHICS_RENDER_TEXTURE_PC *render_texture;
    i32 texture_id;
} FRAME_NATIVE_TEXTURE_PC;

STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_PC, dimensions) == 0);
STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_PC, render_texture) == 0x28);
STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_PC, texture_id) == 0x2c);
STATIC_ASSERT(sizeof(FRAME_NATIVE_TEXTURE_PC) == 0x30);
#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialDebrisProgramsPc;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialGlassProgramsPc;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialFaceOnDecalProgramsPc;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialFaceOnProgramsPc;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialDefaultProgramsPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 6 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern FRAME_NATIVE_TEXTURE_POOL_PC FrameNativeTexturePoolPc;
    extern PC_FRAMEBUFFER_PC *PcDefaultFramebuffer;
    extern PC_BOOT_FRAMEBUFFER_VIEW_PC PcBootFramebuffersPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 7 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    struct debris_particle_chunk_pc_s {
        u8 command;
        u8 fields_001[3];
        struct debris_particle_chunk_pc_s *next;
        u8 fields_008[12];
        u32 field_014;
        u32 field_018;
        u32 field_01c;
        DEBRISPARTICLE_PC particles[32];
        u32 end_command;
    };

    STATIC_ASSERT(sizeof(DEBRIS_PARTICLE_CHUNK_PC) == 0x424);
    STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_CHUNK_PC, command) == 0);
    STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_CHUNK_PC, next) == 4);
    STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_CHUNK_PC, field_014) == 0x14);
    STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_CHUNK_PC, particles) == 0x20);
    STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_CHUNK_PC, end_command) == 0x420);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP
