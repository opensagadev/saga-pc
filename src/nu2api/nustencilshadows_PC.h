/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART
#define SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

/* 7 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUSTENCILSHADOWS_PC_H_PART == 1 && !defined(SAGA_NU2API_NUSTENCILSHADOWS_PC_H_STOP)

struct NUGSCN_s;
struct PC_TEXTURE_RESOURCE;
struct PC_VERTEX_ELEMENT;
struct numtl_s;

typedef struct __declspec(align(16)) NUVEC4BLOCK { // size 0x30
    f32 x[4];                                      // 0x00
    f32 y[4];                                      // 0x10
    f32 z[4];                                      // 0x20
} NUVEC4BLOCK;

typedef struct PC_TEXTURE_LEVEL_DESC { // size 0x20
    u32 format;                        // 0x00
    u32 type;                          // 0x04
    u32 usage;                         // 0x08
    u32 pool;                          // 0x0c
    u32 unknown_10[2];                 // 0x10
    u32 width;                         // 0x18
    u32 height;                        // 0x1c
} PC_TEXTURE_LEVEL_DESC;

typedef struct nutexbmp_colour_s {
    u8 blue;
    u8 green;
    u8 red;
    u8 alpha;
} NUTEXBMP_COLOUR_PC;

typedef struct nutexbmp_s { // size 0x24
    i32 type;               // 0x00
    i32 width;              // 0x04
    i32 height;             // 0x08
    i32 mips;               // 0x0c
    u8 *data;               // 0x10
    u8 *palette;            // 0x14
    u32 unknown_18[3];      // 0x18
} NUTEXBMP_PC;

typedef struct PC_VIEWPORT { // size 0x18
    u32 x;                   // 0x00
    u32 y;                   // 0x04
    u32 width;               // 0x08
    u32 height;              // 0x0c
    f32 min_z;               // 0x10
    f32 max_z;               // 0x14
} PC_VIEWPORT;

typedef struct nulgtvertex_s { // size 0x18
    f32 x;                     // 0x00
    f32 y;                     // 0x04
    f32 z;                     // 0x08
    u32 colour;                // 0x0c
    f32 u;                     // 0x10
    f32 v;                     // 0x14
} NULGTVERTEX;

typedef struct PC_BUFFER_RESOURCE PC_BUFFER_RESOURCE;

typedef struct PC_BUFFER_RESOURCE_METHODS {                                 // size 0x34
    void *unknown_0000[2];                                                  // 0x00
    ULONG(__stdcall *release)(PC_BUFFER_RESOURCE *);                        // 0x08
    void *unknown_000c[8];                                                  // 0x0c
    HRESULT(__stdcall *lock)(PC_BUFFER_RESOURCE *, u32, u32, void **, u32); // 0x2c
    HRESULT(__stdcall *unlock)(PC_BUFFER_RESOURCE *);                       // 0x30
} PC_BUFFER_RESOURCE_METHODS;

struct PC_BUFFER_RESOURCE {                    // size 0x4
    const PC_BUFFER_RESOURCE_METHODS *methods; // 0x0
};

typedef struct PC_EFFECT_SURFACE PC_EFFECT_SURFACE;

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

    typedef struct PC_SHADOW_QUEUE_RECORD { // size 0x18
        NUVEC position;                     // 0x00
        f32 radius;                         // 0x0c
        u16 opacity;                        // 0x10
        u8 unknown_12[6];                   // 0x12
    } PC_SHADOW_QUEUE_RECORD;

    extern i32 NuPrimVertexCount;
    extern i32 *NuPrimVertexCountPointer;
    extern i32 NuPrimCurrentPrimitiveType;
    extern f32 NuPrimXScale;
    extern f32 NuPrimYScale;
    extern f32 NuPrimXBias;
    extern f32 NuPrimYBias;
    extern const f32 NuPrimPS2XScale;
    extern const f32 NuPrimPS2YScale;
    extern u32 nuprim_field_ba8;
    extern f32 NuRndrFxHeightOverride;
    extern f32 NuRndrFxHeight;
    extern f32 NuRndrFxPreviousHeight;
    extern u32 NuRndrFxColour;
    extern PC_BUFFER_RESOURCE *NuDebrisVertexBuffers[4];
    extern void *NuDebrisSystemBuffers[8];
    extern i32 NuDebrisUseSystemMemory;
    extern u32 NuDebrisMaxVertexCount;
    extern void *NuDebrisVertexData;
    extern u32 NuDebrisFrameVertexCount;

    typedef struct debris_particle_chunk_s DEBRIS_PARTICLE_CHUNK_PC;
    void PCDeviceCreateVertexBuffer(u32, u32, u32, i32, PC_BUFFER_RESOURCE **);
    void PCDeviceCreateIndexBuffer(u32, u32, i32, i32, PC_BUFFER_RESOURCE **, HANDLE *);
    extern i32 nu_water_reset_state;
    extern i32 nu_water_reset_status;
    extern i32 nu_water_flow_state;
    extern u8 NuPortalVisibilityBits[625];
    extern const f64 NuVpFixed16Scale;
    extern const f64 NuVpDepthRange;
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
    extern NUVEC4BLOCK ClipCornerBlocks[2];
    extern NUVEC4BLOCK ClipTransformedBlocks[2];
    extern i32 PCDeviceSamplerInvalidation[16][2];
    extern DWA_WEIGHT_BLOCK DwaWeightPool[60669];
    extern DWA_WEIGHT_BLOCK *DwaPointerPool[2048];
    extern u8 NuPrimNeedsOverbrightening;
    extern i32 NuUnresolvedTextureIDPC;
    extern u8 NuUnresolvedTextureDds[0x2080];
    extern const u8 NuUnresolvedTextureDDS[8320];
    extern char NuTexBmpErrorText[0x80];
    extern NUTEXBMP_COLOUR_PC NuTexBmpPalette[0x100];
    extern i32 NuQFntScanX;
    extern i32 NuQFntScanY;
    extern i32 SceneUploadBufferSizePC;
    extern const char SceneUploadSourcePathPC[57];
#define nudlist_flags (PCDynamicDisplayScene.render_flags)    // NOLINT(readability-identifier-naming)
#define nudlist_field_e58 (PCDynamicDisplayScene.native_word) // NOLINT(readability-identifier-naming)
    void PCDeviceDrawPrimitiveUPPc(i32, u32, const void *, u32);
    extern const char NuTexturePhongCubePath[];
    extern RENDER_POST_VIEWPORT_PREFIX_PC RenderPostViewportPrefix;
    extern u32 NuDebrisIdealDynamicVBSize;
    extern u8 PCGraphicsRenderTextureActive[32];
    extern i32 PCGraphicsRenderTextureBytes;
    void NuRndrQuadBufferInit(void);
#ifdef __cplusplus
}
#endif

struct PC_GEOMETRY_BASIS;
struct PC_GEOMETRY_OUTPUT;

/* Public interfaces for connected motion, title callbacks and geometry readers.
 * Private PC field cohorts and their offset assertions remain in the owners. */
struct PC_DYNAMIC_GEOMETRY;
struct PC_GEOMETRY_SORT_ITEM;
struct PC_GEOMETRY_SKIN;
struct PC_GEOMETRY_MESH;
typedef struct PC_GEOMETRY_BASIS_SOURCE PC_GEOMETRY_BASIS_SOURCE;
typedef struct PC_GEOMETRY_MORPH_WEIGHTS PC_GEOMETRY_MORPH_WEIGHTS;
#ifdef __cplusplus
extern "C" {
#endif
    void PcResetGeometryOriginState(void);
    extern u32 RenderFrameSceneFlags;
#ifdef __cplusplus
}
#endif

// NOLINTNEXTLINE(readability-identifier-naming)
#define PcBootDefaultFramebuffer (PcBootFramebuffers.fields.default_framebuffer)
#ifdef __cplusplus
extern "C" {
#endif
    extern void *NuDisplayHandlerMaterial;
    extern i32 NuDisplayHandlerCameraId;
    extern f32 NuDisplayHandlerShadow;
    extern i32 PCGraphicsBoundShader;
    void PCDeviceDrawPrimitivePc(i32, u32, u32);
    void PCDeviceDrawIndexedPrimitivePc(i32, i32, u32, u32, u32, u32);
    extern i32 NuDisplayMaterialDistortTexture;
    extern PC_DISPLAY_ITEM_HANDLER_PC PCDisplayItemHandlerPrefix[49];
    extern PC_DISPLAY_ITEM_HANDLER_PC *CurrentItemTable;
#ifdef __cplusplus
}
#endif

typedef struct PC_LIGHT_STATE_SLOT { // size 0x9c
    u8 unknown_00[0x94];             // 0x00
    i32 next;                        // 0x94
    i32 previous;                    // 0x98
} PC_LIGHT_STATE_SLOT;

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_LIGHT_STATE_SLOT NuLgtStateSlots[8];
    extern i32 NuLgtStateCapacity;
    extern i32 NuLgtStateActiveCount;
    extern i32 NuLgtStateFreeHead;
    extern i32 NuLgtStateEnabled;
    extern i32 NuLgtStateUnknown;

    typedef struct PC_GOBJ_REGISTRY_AUX_PC {  // size 0x10
        struct PC_GOBJ_REGISTRY_AUX_PC *next; // 0x0
        u8 unknown_04[8];                     // 0x4
        void *payload;                        // 0xc
    } PC_GOBJ_REGISTRY_AUX_PC;

    extern i32 NuGobjRegistryInitializedPc;
    extern i32 NuGobjRegistryCountPc;
#ifdef __cplusplus
    void PCGraphicsSubmitViewport(i32, i32, i32, i32);
    void PCGraphicsSetSamplerTuple(u32, const u32 *);
    void PCGraphicsDrawPostEffectQuad(void);
    void PCGraphicsDrawPostEffectIndexed(void);
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

typedef struct debris_part_header_s { // size 0xa4c
    u8 fields_000[16];                // 0x000
    f32 gravity;                      // 0x010
    f32 last_render_time;             // 0x014
    u8 fields_018[16];                // 0x018
    f32 texture_u0;                   // 0x028
    f32 texture_v0;                   // 0x02c
    f32 texture_u1;                   // 0x030
    f32 texture_v1;                   // 0x034
    u8 fields_038[20];                // 0x038
    DEBRIS_PARTICLE_FRAME frames[64]; // 0x04c
} DEBRIS_PART_HEADER_PC;

typedef struct SCENE_VIDEO_RESOURCE_HEADER { // size 0x18
    u16 vertex_buffer_count;                 // 0x00
    u16 unknown_02;                          // 0x02
    PC_BUFFER_RESOURCE **vertex_buffers;     // 0x04
    u16 index_buffer_count;                  // 0x08
    u16 unknown_0a;                          // 0x0a
    PC_BUFFER_RESOURCE **index_buffers;      // 0x0c
    u16 texture_count;                       // 0x10
    u16 unknown_12;                          // 0x12
    struct PC_TEXTURE_RESOURCE **textures;   // 0x14
} SCENE_VIDEO_RESOURCE_HEADER;

#ifdef __cplusplus
extern "C" {
#endif
    extern VARIPTR *NuPrimStreamBufferPointer;
    extern NUVPCURRENT vpDevice;
    extern NUDISPLAYLISTITEM nudlist_2d_static;
    extern VARIPTR nudlist_stream_cursor;
    void PCDeviceCreateVertexDeclaration(const struct PC_VERTEX_ELEMENT *, struct PC_TEXTURE_RESOURCE **);
    extern DEBRIS_PART_HEADER_PC *NuDebrisLastHeader;
    extern SCENE_VIDEO_RESOURCE_HEADER SceneVideoResourcesPC;
    extern VARIPTR SceneUploadBufferStartPC;
    extern VARIPTR SceneUploadBufferCursorPC;
    extern VARIPTR SceneUploadBufferEndPC;
    extern struct PC_TEXTURE_RESOURCE *NuTextureEnvironmentCube;
    extern struct PC_TEXTURE_RESOURCE *NuTexturePhongCube;
    void PCDeviceCreateDepthSurface(u32, u32, i32, i32, u32, BOOL, struct PC_TEXTURE_RESOURCE **, HANDLE *);
    void PCDeviceCreateTexture(u32, u32, u32, u32, i32, i32, struct PC_TEXTURE_RESOURCE **);
    extern struct PC_TEXTURE_RESOURCE *NuDisplayHandlerVertexShaderCachePc;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayHandlerPixelShaderCachePc;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayMaterialDebrisDeclaration;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayMaterialFaceOnDeclaration;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayMaterialDefaultDeclaration;

    typedef struct PC_GOBJ_NATIVE_RESOURCE {  // size 0x14
        u8 unknown_00[0x10];                  // 0x00
        struct PC_TEXTURE_RESOURCE *resource; // 0x10
    } PC_GOBJ_NATIVE_RESOURCE;

    typedef struct PC_GOBJ_REGISTRY_CHILD_PC {    // size 0x3c
        struct PC_GOBJ_REGISTRY_CHILD_PC *next;   // 0x00
        u8 unknown_04[0x1c];                      // 0x04
        void *owned_buffer;                       // 0x20
        u8 unknown_24[8];                         // 0x24
        PC_GOBJ_REGISTRY_AUX_PC *auxiliary;       // 0x2c
        u8 unknown_30[8];                         // 0x30
        PC_GOBJ_NATIVE_RESOURCE *native_resource; // 0x38
    } PC_GOBJ_REGISTRY_CHILD_PC;

    typedef struct PC_GOBJ_REGISTRY_ENTRY_PC {      // size 0x14
        struct PC_GOBJ_REGISTRY_ENTRY_PC *next;     // 0x00
        struct PC_GOBJ_REGISTRY_ENTRY_PC *previous; // 0x04
        u8 unknown_08[4];                           // 0x08
        PC_GOBJ_REGISTRY_CHILD_PC *children;        // 0x0c
        PC_GOBJ_REGISTRY_AUX_PC *auxiliary;         // 0x10
    } PC_GOBJ_REGISTRY_ENTRY_PC;

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
typedef struct NU_CAMERA_STACK_ENTRY { // size 0x7c
    NUCAMERA camera;                   // 0x00
    i32 camera_id;                     // 0x78
} NU_CAMERA_STACK_ENTRY;
#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX PCSpecialProjectionTranslate;
    extern NUMTX PCSpecialProjectionScale;
    extern struct PC_HG_SHADOW_PACKET PCShadowPackets[280];
    extern NU_CAMERA_STACK_ENTRY dat_0280a598[64];
    extern NU_CAMERA_STACK_ENTRY *dat_0080e864;
    void PCEnqueueHierarchyShadowGroup(NUMTX *, void *, i32, void *, i32, void *, i32);
    extern NUMTX NuRndr_DebrisMtx;
    extern NUVEC4 NuRndr_DebrisPlane;
    extern NUMTX *NuRndr_DebrisRotMtxPtr;
    extern NUMTX NuCameraScalingTemplate;
    extern NUVEC4 PcGeometryDistanceOrigin;
    void PcUpdateGeometryCameraOrigin(NUMTX *, NUMTX *);
    extern NUVEC4 PcGeometryDefaultOrigin;
    extern NUMTX PcGeometryViewMatrix;
    extern NUMTX PcGeometryViewProjectionMatrix;
    extern NUMTX PcGeometryProjectionMatrix;
    extern NUMTX PcGeometryInverseViewProjectionMatrix;
    extern NUMTX PcGeometryResetMatrix;
    extern NUMTX PcGeometryAdjustedProjectionMatrix;
    extern NUMTX PcGeometryTranslationMatrix;
    extern NUMTX PcGeometryScaleMatrix;
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
    extern struct numtl_s *dat_02854d00;
    extern struct PC_HG_SHADOW_STATE_VIEW PCShadowState;
    void NuHGobjSubmitShadowGroups(NUHGOBJ_EVAL *, NUMTX *, NUMTX *);
    extern NURND_VERTEX3D_PC NuRndrShadowVertices[4];

    typedef struct NATIVE_PARTICLE_PACKET { // size 0x14
        u8 vertex_buffer_index;             // 0x00
        u8 fields_001[3];                   // 0x01
        i32 use_system_memory_vb;           // 0x04
        i32 first_vertex;                   // 0x08
        i32 vertex_count;                   // 0x0c
        struct numtl_s *material;           // 0x10
    } NATIVE_PARTICLE_PACKET;

    extern struct NUGSCN_s *PCRegisteredNativeScenes[100];
    extern struct numtl_s *PCStartupMaterial1;
    extern struct numtl_s *PCStartupMaterial2;
    extern PC_GRAPHICS_FRAME_SCRATCH_VIEW PCGraphicsFrameScratchCount;
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

    struct PC_EFFECT_SURFACE {                                                                             // size 0x1c
        GRAPHICS_RENDER_TEXTURE *texture;                                                                  // 0x00
        u8 owns_texture; /* 0x05..0x07 alignment padding; no char arrays, so locals carry no /GS cookie */ // 0x04
        struct PC_TEXTURE_RESOURCE *surface;                                                               // 0x08
        u16 level;                                                                                         // 0x0c
        u16 unknown_0e;                                                                                    // 0x0e
        u32 field_10;                                                                                      // 0x10
        u16 width;                                                                                         // 0x14
        u16 height;                                                                                        // 0x16
        i16 registry_index; /* 0x1a..0x1b alignment padding */                                             // 0x18
#ifdef __cplusplus
        PC_EFFECT_SURFACE();
        ~PC_EFFECT_SURFACE();
        void Reset();
        void Attach(GRAPHICS_RENDER_TEXTURE *, i32, bool);
        static void ReleaseResources();
        static void RestoreResources();
        GRAPHICS_RENDER_TEXTURE *fun_005f7af0();
        u16 fun_005f7b00();
        u32 fun_005f7b10();
        u16 fun_005f7b20();
        u16 fun_005f7b30();
        void fun_005fb920(PC_EFFECT_SURFACE *source);
#endif
    };

    extern GRAPHICS_RENDER_TEXTURE PCGraphicsRenderTextures[32];

#ifdef __cplusplus
}
#endif

/* Minimum selected native producer/reader view; original allocation unknown. */
typedef struct FRAME_NATIVE_TEXTURE {        // size 0x30
    NUTEXDIM_PC dimensions;                  // 0x00
    GRAPHICS_RENDER_TEXTURE *render_texture; // 0x28
    i32 texture_id;                          // 0x2c
} FRAME_NATIVE_TEXTURE;

#ifdef __cplusplus
extern "C" {
#endif
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialDebrisPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialGlassPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialFaceOnDecalPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialFaceOnPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialDefaultPrograms;
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
    extern FRAME_NATIVE_TEXTURE_POOL FrameNativeTexturePool;
    extern PC_FRAMEBUFFER *PcDefaultFramebuffer;
    extern PC_BOOT_FRAMEBUFFER_VIEW PcBootFramebuffers;
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

    struct debris_particle_chunk_s {          // size 0x424
        u8 command;                           // 0x000
        u8 fields_001[3];                     // 0x001
        struct debris_particle_chunk_s *next; // 0x004
        u8 fields_008[12];                    // 0x008
        u32 field_014;                        // 0x014
        u32 field_018;                        // 0x018
        u32 field_01c;                        // 0x01c
        DEBRISPARTICLE_PC particles[32];      // 0x020
        u32 end_command;                      // 0x420
    };

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
