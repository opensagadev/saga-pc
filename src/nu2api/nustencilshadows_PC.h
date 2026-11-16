#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"

struct DWA_WEIGHT_BLOCK;
struct GRAPHICS_RENDER_TEXTURE;
struct NUHGOBJ_EVAL;

struct NUGSCN_s;
struct PC_TEXTURE_RESOURCE;
struct PC_VERTEX_ELEMENT;
struct numtl_s;

typedef struct __declspec(align(16)) NUVEC4BLOCK { // size 0x30
    f32 x[4];                                      // 0x00
    f32 y[4];                                      // 0x10
    f32 z[4];                                      // 0x20
} NUVEC4BLOCK;

typedef struct nutexbmp_colour_s {
    u8 blue;
    u8 green;
    u8 red;
    u8 alpha;
} NUTEXBMP_COLOUR;

typedef struct nutexbmp_s { // size 0x24
    i32 type;               // 0x00
    i32 width;              // 0x04
    i32 height;             // 0x08
    i32 mips;               // 0x0c
    u8 *data;               // 0x10
    u8 *palette;            // 0x14
    u32 unknown_18[3];      // 0x18
} NUTEXBMP;

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

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct PC_SPECIAL_PROJECTION_ADJUST { // size 0x10
        f32 x_scale;                              // 0x0
        f32 y_scale;                              // 0x4
        f32 x_offset;                             // 0x8
        f32 y_offset;                             // 0xc
    } PC_SPECIAL_PROJECTION_ADJUST;

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
    extern NULGTVERTEX *NuDebrisSystemBuffers[8];
    extern i32 NuDebrisUseSystemMemory;
    extern u32 NuDebrisMaxVertexCount;
    extern NULGTVERTEX *NuDebrisVertexData;
    extern u32 NuDebrisFrameVertexCount;

    typedef struct debris_particle_chunk_s DEBRIS_PARTICLE_CHUNK;
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
    extern struct DWA_WEIGHT_BLOCK *DwaPointerPool[2048];
    extern u8 NuPrimNeedsOverbrightening;
    extern i32 NuUnresolvedTextureIDPC;
    extern u8 NuUnresolvedTextureDds[0x2080];
    extern const u8 NuUnresolvedTextureDDS[8320];
    extern char NuTexBmpErrorText[0x80];
    extern NUTEXBMP_COLOUR NuTexBmpPalette[0x100];
    extern i32 NuQFntScanX;
    extern i32 NuQFntScanY;
    extern i32 SceneUploadBufferSizePC;
    extern const char SceneUploadSourcePathPC[57];
#define nudlist_flags (PCDynamicDisplayScene.render_flags)    // NOLINT(readability-identifier-naming)
#define nudlist_field_e58 (PCDynamicDisplayScene.native_word) // NOLINT(readability-identifier-naming)
    void PCDeviceDrawPrimitiveUPPc(i32, u32, const void *, u32);
    extern const char NuTexturePhongCubePath[];

    typedef struct render_post_viewport_prefix_s { // size 0x30
        f32 source_x;                              // 0x00
        f32 source_y;                              // 0x04
        f32 source_right;                          // 0x08
        f32 source_bottom;                         // 0x0c
        f32 dest_x;                                // 0x10
        f32 dest_y;                                // 0x14
        f32 dest_right;                            // 0x18
        f32 dest_bottom;                           // 0x1c
        f32 width_scale;                           // 0x20
        f32 height_scale;                          // 0x24
        f32 x_offset;                              // 0x28
        f32 y_offset;                              // 0x2c
    } RENDER_POST_VIEWPORT_PREFIX;

    extern RENDER_POST_VIEWPORT_PREFIX RenderPostViewportPrefix;
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
    extern struct numtl_s *NuDisplayHandlerMaterial;
    extern i32 NuDisplayHandlerCameraId;
    extern f32 NuDisplayHandlerShadow;
    extern i32 PCGraphicsBoundShader;
    void PCDeviceDrawPrimitive(i32, u32, u32);
    void PCDeviceDrawIndexedPrimitive(i32, i32, u32, u32, u32, u32);
    extern i32 NuDisplayMaterialDistortTexture;
    typedef void (*PC_DISPLAY_ITEM_HANDLER)(void *);

    extern PC_DISPLAY_ITEM_HANDLER PCDisplayItemHandlerPrefix[49];
    extern PC_DISPLAY_ITEM_HANDLER *CurrentItemTable;
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

    typedef struct PC_GOBJ_REGISTRY_AUX {  // size 0x10
        struct PC_GOBJ_REGISTRY_AUX *next; // 0x0
        u8 unknown_04[8];                  // 0x4
        void *payload;                     // 0xc
    } PC_GOBJ_REGISTRY_AUX;

    extern i32 NuGobjRegistryInitialized;
    extern i32 NuGobjRegistryCount;
#ifdef __cplusplus
    void PCGraphicsSubmitViewport(i32, i32, i32, i32);
    void PCGraphicsSetSamplerTuple(u32, const u32 *);
    void PCGraphicsDrawPostEffectQuad(void);
    void PCGraphicsDrawPostEffectIndexed(void);
#ifdef __cplusplus
}
#endif
#endif

typedef struct debris_particle_frame_s { // size 0x28
    NUVEC position;                      // 0x00
    NUVEC texture_offset;                // 0x0c
    NUVEC extent;                        // 0x18
    u32 colour;                          // 0x24
} DEBRIS_PARTICLE_FRAME;

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
} DEBRIS_PART_HEADER;

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
    extern NUDISPLAYLISTITEM nudlist_2d_static;
    extern VARIPTR nudlist_stream_cursor;
    void PCDeviceCreateVertexDeclaration(const struct PC_VERTEX_ELEMENT *, struct PC_TEXTURE_RESOURCE **);
    extern DEBRIS_PART_HEADER *NuDebrisLastHeader;
    extern SCENE_VIDEO_RESOURCE_HEADER SceneVideoResourcesPC;
    extern VARIPTR SceneUploadBufferStartPC;
    extern VARIPTR SceneUploadBufferCursorPC;
    extern VARIPTR SceneUploadBufferEndPC;
    extern struct PC_TEXTURE_RESOURCE *NuTextureEnvironmentCube;
    extern struct PC_TEXTURE_RESOURCE *NuTexturePhongCube;
    void PCDeviceCreateDepthSurface(u32, u32, i32, i32, u32, BOOL, struct PC_TEXTURE_RESOURCE **, HANDLE *);
    void PCDeviceCreateTexture(u32, u32, u32, u32, i32, i32, struct PC_TEXTURE_RESOURCE **);
    extern struct PC_TEXTURE_RESOURCE *NuDisplayHandlerVertexShaderCache;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayHandlerPixelShaderCache;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayMaterialDebrisDeclaration;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayMaterialFaceOnDeclaration;
    extern struct PC_TEXTURE_RESOURCE *NuDisplayMaterialDefaultDeclaration;

    typedef struct PC_GOBJ_NATIVE_RESOURCE {  // size 0x14
        u8 unknown_00[0x10];                  // 0x00
        struct PC_TEXTURE_RESOURCE *resource; // 0x10
    } PC_GOBJ_NATIVE_RESOURCE;

    typedef struct PC_GOBJ_REGISTRY_CHILD {       // size 0x3c
        struct PC_GOBJ_REGISTRY_CHILD *next;      // 0x00
        u8 unknown_04[0x1c];                      // 0x04
        void *owned_buffer;                       // 0x20
        u8 unknown_24[8];                         // 0x24
        PC_GOBJ_REGISTRY_AUX *auxiliary;          // 0x2c
        u8 unknown_30[8];                         // 0x30
        PC_GOBJ_NATIVE_RESOURCE *native_resource; // 0x38
    } PC_GOBJ_REGISTRY_CHILD;

    typedef struct PC_GOBJ_REGISTRY_ENTRY {      // size 0x14
        struct PC_GOBJ_REGISTRY_ENTRY *next;     // 0x00
        struct PC_GOBJ_REGISTRY_ENTRY *previous; // 0x04
        u8 unknown_08[4];                        // 0x08
        PC_GOBJ_REGISTRY_CHILD *children;        // 0x0c
        PC_GOBJ_REGISTRY_AUX *auxiliary;         // 0x10
    } PC_GOBJ_REGISTRY_ENTRY;

    extern PC_GOBJ_REGISTRY_ENTRY *NuGobjRegistryFirst;
#ifdef __cplusplus
}
#endif

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

    typedef struct PC_HG_SHADOW_SHAPE48 PC_HG_SHADOW_SHAPE48;
    typedef struct PC_HG_SHADOW_SHAPE64 PC_HG_SHADOW_SHAPE64;
    typedef struct PC_HG_SHADOW_GROUP PC_HG_SHADOW_GROUP;

    struct PC_HG_SHADOW_SHAPE48 { // size 0x30

        union { // size 0x30
            NUVEC axes[4];

            struct {          // size 0x30
                f32 center_x; // 0x00
                u32 flags;    // 0x04
                u8 rest[40];  // 0x08
            } filter;
        }; // 0x00
    };

    struct PC_HG_SHADOW_SHAPE64 {      // size 0x40
        PC_HG_SHADOW_SHAPE48 position; // 0x00
        u8 unknown_30[16];             // 0x30
    };

    struct PC_HG_SHADOW_GROUP {          // size 0x10
        PC_HG_SHADOW_SHAPE48 *ellipses;  // 0x0
        PC_HG_SHADOW_SHAPE64 *cylinders; // 0x4
        void *third_shape;               // 0x8
        u8 ellipse_count;                // 0xc
        u8 cylinder_count;               // 0xd
        u8 third_count;                  // 0xe
        u8 joint_index;                  // 0xf
    };

    struct PC_HG_SHADOW_PACKET { // size 0x68
        NUMTX matrix;            // 0x00
        void *ellipse;           // 0x40
        void *cylinder;          // 0x44
        void *third_shape;       // 0x48
        i32 state0;              // 0x4c
        i32 state1;              // 0x50
        i32 state2;              // 0x54
        i32 state_byte;          // 0x58
        f32 near_value;          // 0x5c
        f32 far_value;           // 0x60
        u8 ellipse_count;        // 0x64
        u8 cylinder_count;       // 0x65
        u8 third_count;          // 0x66
        u8 sequence;             // 0x67
    };

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

struct PC_HG_SHADOW_STATE_VIEW { // size 0x10

    union { // size 0xc

        struct {        // size 0xc
            i32 state0; // 0x0
            i32 state1; // 0x4
            i32 state2; // 0x8
        };

        NUVEC direction;
    }; // 0x0

    union { // size 0x4

        struct {              // size 0x4
            u8 unknown_0c[3]; // 0xc
            i8 state_byte;    // 0xf
        };

        u32 packed_colour;
    }; // 0xc
};

#ifdef __cplusplus
extern "C" {
#endif
    extern struct numtl_s *dat_02854d00;
    extern struct PC_HG_SHADOW_STATE_VIEW PCShadowState;
    void NuHGobjSubmitShadowGroups(struct NUHGOBJ_EVAL *, NUMTX *, NUMTX *);

    typedef struct NATIVE_PARTICLE_PACKET { // size 0x14
        u8 vertex_buffer_index;             // 0x00
        u8 fields_001[3];                   // 0x01
        i32 use_system_memory_vb;           // 0x04
        i32 first_vertex;                   // 0x08
        i32 vertex_count;                   // 0x0c
        struct numtl_s *material;           // 0x10
    } NATIVE_PARTICLE_PACKET;

    typedef union PC_GRAPHICS_FRAME_SCRATCH_VIEW { // size 0x4
        i32 reset_word;
        void *lightmap_packet;
    } PC_GRAPHICS_FRAME_SCRATCH_VIEW;

    extern struct NUGSCN_s *PCRegisteredNativeScenes[100];
    extern struct numtl_s *PCStartupMaterial1;
    extern struct numtl_s *PCStartupMaterial2;
    extern PC_GRAPHICS_FRAME_SCRATCH_VIEW PCGraphicsFrameScratchCount;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    extern GRAPHICS_RENDER_TEXTURE PCGraphicsRenderTextures[32];

    extern struct PC_DL_SHADER_PROGRAM_PAIR *NuDisplayMaterialDebrisPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR *NuDisplayMaterialGlassPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR *NuDisplayMaterialFaceOnDecalPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR *NuDisplayMaterialFaceOnPrograms;
    extern struct PC_DL_SHADER_PROGRAM_PAIR *NuDisplayMaterialDefaultPrograms;
#ifdef __cplusplus
}
#endif

typedef union PC_BOOT_FRAMEBUFFER_VIEW { // size 0x8

    struct {                                          // size 0x8
        struct PC_FRAMEBUFFER *default_framebuffer;   // 0x0
        struct PC_FRAMEBUFFER *secondary_framebuffer; // 0x4
    } fields;

    struct PC_FRAMEBUFFER *slots[2];
} PC_BOOT_FRAMEBUFFER_VIEW;

#ifdef __cplusplus
extern "C" {
#endif

    /* Minimum selected native producer/reader view; original allocation unknown. */
    typedef struct FRAME_NATIVE_TEXTURE {               // size 0x30
        NUTEXDIM dimensions;                            // 0x00
        struct GRAPHICS_RENDER_TEXTURE *render_texture; // 0x28
        i32 texture_id;                                 // 0x2c
    } FRAME_NATIVE_TEXTURE;

    typedef struct FRAME_NATIVE_TEXTURE_POOL { // size 0x308
        FRAME_NATIVE_TEXTURE records[16];      // 0x000
        i8 occupied[2];                        // 0x300
        u8 unknown_302[2];                     // 0x302
        i32 cursor;                            // 0x304
#ifdef __cplusplus
        FRAME_NATIVE_TEXTURE *Allocate();
        FRAME_NATIVE_TEXTURE_POOL *fun_00605150();
        i32 fun_00605170(FRAME_NATIVE_TEXTURE *record);
        void fun_00605300(i32 index, bool set);
        i32 fun_00605460(i32 index);
        void fun_0060a010(FRAME_NATIVE_TEXTURE *record);
#endif
    } FRAME_NATIVE_TEXTURE_POOL;

    extern FRAME_NATIVE_TEXTURE_POOL FrameNativeTexturePool;
    extern PC_FRAMEBUFFER *PcDefaultFramebuffer;
    extern PC_BOOT_FRAMEBUFFER_VIEW PcBootFramebuffers;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    struct debris_particle_chunk_s {          // size 0x424
        u8 command;                           // 0x000
        u8 fields_001[3];                     // 0x001
        struct debris_particle_chunk_s *next; // 0x004
        u8 fields_008[12];                    // 0x008
        u32 unknown_014;                      // 0x014
        u32 unknown_018;                      // 0x018
        u32 unknown_01c;                      // 0x01c
        DEBRISPARTICLE particles[32];         // 0x020
        u32 end_command;                      // 0x420
    };

    extern GRAPHICS_RENDER_TEXTURE *PCGraphicsPostEffectBackBufferTexture;
    extern PC_EFFECT_SURFACE *PcSurfaceRegistry[256];
    extern PC_FRAMEBUFFER *PcBoundFramebuffer;
    extern PC_FRAMEBUFFER *PcSecondaryFramebuffer;
    extern PC_FRAMEBUFFER PcFramebufferPool[32];
    extern u32 PCGraphicsPostEffectHeight;
    extern u32 PCGraphicsPostEffectWidth;
    extern u8 DwaBlockTable[0x400];
    extern u8 PcFramebufferActive[32];
    extern u8 PcSurfaceRegistryInitialized;
    void NuRndrGeometryBufferRelease(void);
    void NuRndrGeometryBufferRestore(void);
    void PCGraphicsDestroyPostEffectTextures(void);
    void PCGraphicsResizePostEffectTextures(u32, u32);
    void PcGeometryOriginRelease(void);
#ifdef __cplusplus
}
#endif
