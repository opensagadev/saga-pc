// PC platform unit linked after nu3d (retail 0x5f7490-0x60df00), being rebuilt one module at a time.
// Retail compiled the nu3d\pc sources (assert paths name nugscn_dlist.c, numtl_dlist.c, nustream.c,
// nugobj.c, nutex_pc.cpp and nuqfnt_PC.cpp) together with the PS/Pc halves of the generic modules as
// one C++ unit; nu3d only ever calls into it, never inlines from it. Definitions follow retail address
// order. Modules merged so far: nugscn_pc, nuportal_pc, numtl PS/Pc, the nudlist and nupostfilter
// platform members, the render-state/stream/scene/gobj/particle backends and the geometry backend, plus
// the platform members of nulgtlaser, nuframe, nuvport, nuprim, nuhgobj, nupostdeferred, nurndr, nucamera
// and nuvec.

#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"
#pragma bss_seg(".bss$NuDebrisCurrentPacket")
static NATIVE_PARTICLE_PACKET_PC *NuDebrisCurrentPacket = 0;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentVertexCount")
static u32 NuDebrisCurrentVertexCount = 0;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisVertexBufferSize")
static u32 NuDebrisVertexBufferSize = 0;
#pragma bss_seg()

#include <float.h>
#include <stdio.h>
#include <string.h>

extern "C" {

    // @@PREAMBLE
    struct POST_SHADER_PROGRAM_PC;
    POST_SHADER_PROGRAM_PC *__cdecl PostShaderProgramCreatePc(i32, const DWORD *, const DWORD *);
    extern const DWORD PostShaderTokens7662d8[168];
    extern const DWORD PostShaderTokens766578[94];
    extern const DWORD PostShaderTokens7666f0[75];
    extern const DWORD PostShaderTokens766820[261];
    extern const DWORD PostShaderTokens766c38[168];
    extern const DWORD PostShaderTokens766ed8[94];
    extern const DWORD PostShaderTokens767050[75];
    extern const DWORD PostShaderTokens767180[261];
    extern const DWORD PostShaderTokens767598[85];
    extern const DWORD PostShaderTokens7676f0[566];
    extern const DWORD PostShaderTokens767fc8[554];
    extern const DWORD PostShaderTokens768870[155];
    extern const DWORD PostShaderTokens768ae0[143];
    extern const DWORD PostShaderTokens768d20[48];
    extern const DWORD PostShaderTokens768de0[180];
    extern const DWORD PostShaderTokens7690b0[131];
    extern const DWORD PostShaderTokens7692c0[175];
    extern const DWORD PostShaderTokens769580[100];
    PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void);
    void __cdecl NuPostEffectReleasePortsPc(void);
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;

    typedef struct SCENE_TEXTURE_PACKET_B0_PC {
        i32 kind;
        i32 texture_id;
        i32 extra_texture_ids[3];
    } SCENE_TEXTURE_PACKET_B0_PC;
    typedef struct SCENE_TEXTURE_PACKET_AE_PC {
        i32 texture_ids[3];
    } SCENE_TEXTURE_PACKET_AE_PC;
    STATIC_ASSERT(sizeof(SCENE_TEXTURE_PACKET_B0_PC) == 20);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, kind) == 0);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, texture_id) == 4);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, extra_texture_ids) == 8);
    STATIC_ASSERT(sizeof(SCENE_TEXTURE_PACKET_AE_PC) == 12);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_AE_PC, texture_ids) == 0);
    STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, type) == 0);
    STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, next) == 4);
    STATIC_ASSERT(sizeof(NUDISPLAYLISTITEM) == 16);

    /* nuportal PC backend (0x600bd0-0x600f20). */

    typedef struct PORTAL_BOX_PC {
        NUVEC first;
        f32 first_w;
        NUVEC second;
        f32 second_w;
    } PORTAL_BOX_PC;
    STATIC_ASSERT(sizeof(PORTAL_BOX_PC) == 0x20);
    STATIC_ASSERT(offsetof(PORTAL_BOX_PC, second) == 0x10);
    STATIC_ASSERT(offsetof(NUGSCN, num_instances) == 0x1c);
    STATIC_ASSERT(offsetof(NUGSCN, sphere_count) == 0xf0);
    STATIC_ASSERT(offsetof(NUGSCN, portal_instance_count) == 0xf4);
    STATIC_ASSERT(offsetof(NUGSCN, portal_spheres) == 0xf8);
    STATIC_ASSERT(offsetof(NUGSCN, portal_boxes) == 0xfc);
    STATIC_ASSERT(offsetof(NUGSCN, instance_visibility_flags) == 0x13c);
    STATIC_ASSERT(offsetof(NUINSTANCE, flags) == 0x44);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, visibility_flags) == 0x48);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, render_flags) == 0x74);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, portal_visibility_overrides) == 0x88);

    /* numtl PS half (0x601210). */

    typedef struct FRAME_MTL_ANIM_DESC_PC {
        u8 u_mode, v_mode, reserved02[2];
        f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
    } FRAME_MTL_ANIM_DESC_PC;
    STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
    typedef struct FRAME_MTL_ANIM_PC {
        u8 reserved00[0x7b];
        u8 disable_u_animation, disable_v_animation;
        u8 reserved7d[0x143];
        i32 texture_animation_data[4];
        f32 texture_animation_offsets[4][2];
        u8 reserved1f0[8];
        FRAME_MTL_ANIM_DESC_PC animation[4];
    } FRAME_MTL_ANIM_PC;
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
    typedef struct FRAME_MTL_ANIM_SET_PC {
        NUDISPLAY_LIST_PC *scene;
        i32 material_count;
        i32 *material_indices;
        struct FRAME_MTL_ANIM_SET_PC *next;
    } FRAME_MTL_ANIM_SET_PC;
    STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
    typedef struct FRAME_MTL_SCENE_PC {
        u8 reserved00[0x158];
        i32 shader_texture_animation_enabled;
    } FRAME_MTL_SCENE_PC;
    STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);

    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);

#pragma data_seg(".data$NuMainRendererControlPc")
    i32 NuMainRendererControlPc = 1;
#pragma data_seg()

#pragma bss_seg(".bss$NuRndrControlStatePc")
    i32 NuRndrControlStatePc = 0;
#pragma bss_seg()

    /* The shadow-volume pass reads the arena globals that nurndr.c defines as pointers through array
     * declarations, so retail draws from the pointer variables' own addresses (the pass has no caller). */
    extern NUVEC4 RendererArenaDataPc[];
    extern u32 RendererArenaOffsetsPc[];
    extern i32 RendererArenaTypesPc[];
    extern i32 RendererArenaVertexCountPc;
    extern u32 RendererArenaCountPc;
    extern PC_TEXTURE_RESOURCE_PC *RendererVertexDeclarationPc;
    extern PC_BUFFER_RESOURCE_PC *RendererQuadVertexBufferPc;

#pragma data_seg(".data$RendererStencilFuncPc")
    u32 RendererStencilFuncPc = 2;
#pragma data_seg()
#pragma data_seg(".data$RendererStencilPassPc")
    u32 RendererStencilPassPc = 1;
#pragma data_seg()
#pragma data_seg(".data$RendererStencilRefPc")
    u32 RendererStencilRefPc = 0x60;
#pragma data_seg()

    void __cdecl PCDeviceSetWorldIdentityPc(void);
    void __cdecl NuRndrArenaBuildShadowVolumesPc(i32 unused0, i32 unused1);
    typedef struct CAMERA_RENDER_SPECULAR_READER_PC {
        u8 unknown_0000[0xdc];
        NUVEC direction;
    } CAMERA_RENDER_SPECULAR_READER_PC;
    STATIC_ASSERT(offsetof(CAMERA_RENDER_SPECULAR_READER_PC, direction) == 0xdc);

    void __cdecl NuVpGetSize2(f32 *width, f32 *height);

    static NUMTX RndrStateTransformPc = {0};
    static NUMTX RndrStateScaledTransformPc = {0};
    static f32 RndrStateTransformScalePc = 1.0f;
    STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, counter_0020) == 0x20);
    STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_id) == 0x2e);
    STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_packet_cache) == 0x168);
    STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_state_word) == 0x16c);

    /* Native PC material-dependent transform state belongs to this original owner. */
    typedef struct PC_TRANSFORM_MATERIAL_VIEW PC_TRANSFORM_MATERIAL_VIEW;
    struct PC_TRANSFORM_MATERIAL_VIEW {
        u8 unknown_00[0x30];
        PC_TRANSFORM_MATERIAL_VIEW *next;
        u8 unknown_34[0x2c];
        f32 transform_scale;
        u8 unknown_64[0x35];
        u8 transform_kind;
    };
    STATIC_ASSERT(offsetof(PC_TRANSFORM_MATERIAL_VIEW, next) == 0x30);
    STATIC_ASSERT(offsetof(PC_TRANSFORM_MATERIAL_VIEW, transform_scale) == 0x60);
    STATIC_ASSERT(offsetof(PC_TRANSFORM_MATERIAL_VIEW, transform_kind) == 0x99);

    static VARIPTR RndrStreamFirstPc = {0};
    static VARIPTR RndrStreamSecondPc = {0};
    static i32 RndrStreamSizePc = 0;
    static i32 RndrStreamIndexPc = 0;
    static u32 RndrStreamUsedPc = 0;
    static u32 RndrStreamMaxUsedPc = 0;

    typedef struct SCENE_FILE_TEXTURE_HEADER_PC {
        i32 width;
        u32 height;
        u32 unknown_08;
        u32 unknown_0c;
        u32 flags;
        u32 size;
    } SCENE_FILE_TEXTURE_HEADER_PC;
    STATIC_ASSERT(sizeof(SCENE_FILE_TEXTURE_HEADER_PC) == 0x18);
    STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, width) == 0);
    STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, height) == 4);
    STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, flags) == 0x10);
    STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, size) == 0x14);

    typedef struct SCENE_FIXUP_GEOMETRY_PC {
        u8 unknown_00[8];
        u16 vertex_stride;
        u8 unknown_0a[0xe];
        u32 vertex_count;
        u8 unknown_1c[4];
        PC_BUFFER_RESOURCE_PC *index_buffer;
        PC_BUFFER_RESOURCE_PC *vertex_buffer;
        u32 vertex_format;
        u8 unknown_2c[4];
        PC_BUFFER_RESOURCE_PC *dynamic_vertex_buffer;
    } SCENE_FIXUP_GEOMETRY_PC;
    STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_stride) == 8);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_count) == 0x18);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, index_buffer) == 0x20);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_buffer) == 0x24);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_format) == 0x28);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, dynamic_vertex_buffer) == 0x30);
    typedef struct SCENE_FIXUP_VERTEX_STREAM_PC {
        u8 unknown_00[8];
        PC_BUFFER_RESOURCE_PC *vertex_buffer;
    } SCENE_FIXUP_VERTEX_STREAM_PC;
    STATIC_ASSERT(offsetof(SCENE_FIXUP_VERTEX_STREAM_PC, vertex_buffer) == 8);
    typedef struct SCENE_FIXUP_NATIVE_PC {
        u16 vertex_buffer_count;
        u8 unknown_02[2];
        PC_BUFFER_RESOURCE_PC **vertex_buffers;
        u16 index_buffer_count;
        u8 unknown_0a[2];
        PC_BUFFER_RESOURCE_PC **index_buffers;
        SCENE_FIXUP_GEOMETRY_PC **geometries;
        u32 geometry_count;
        SCENE_FIXUP_VERTEX_STREAM_PC **vertex_streams;
        u32 vertex_stream_count;
    } SCENE_FIXUP_NATIVE_PC;
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, vertex_buffers) == 4);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, index_buffer_count) == 8);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, index_buffers) == 0xc);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, geometries) == 0x10);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, geometry_count) == 0x14);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, vertex_streams) == 0x18);
    STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, vertex_stream_count) == 0x1c);
    void __cdecl NuMtlUpdate(NUMTL *);
    void __cdecl NuPortalMaxDepth(NUGSCN *, i32);

    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, native_word) == 0x7c);

    struct __declspec(align(16)) PC_GEOMETRY_BASIS_VECTOR_PC {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    STATIC_ASSERT(sizeof(PC_GEOMETRY_BASIS_VECTOR_PC) == 0x10);
    struct PC_GEOMETRY_BASIS_INPUT_RECORD_PC {
        PC_GEOMETRY_BASIS_VECTOR_PC rows[3];
        NUVEC weights;
        u8 bones[3];
        u8 unknown_3f;
    };
    struct PC_GEOMETRY_MORPH_DELTA_PC {
        u32 unknown_00;
        u32 unknown_04;
        PC_GEOMETRY_BASIS_VECTOR_PC *vectors;
    };
    struct PC_GEOMETRY_BASIS_SOURCE_PC {
        u32 unknown_00;
        i32 record_count;
        u32 unknown_08;
        i32 morph_count;
        u32 unknown_10;
        PC_GEOMETRY_BASIS_INPUT_RECORD_PC *records;
        PC_GEOMETRY_MORPH_DELTA_PC *morph_deltas;
    };
    struct PC_GEOMETRY_MORPH_WEIGHTS_PC {
        f32 *weight_words;
    };
    struct PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC {
        PC_GEOMETRY_BASIS_VECTOR_PC rows[3];
        u8 unwritten_30[0x10];
    };
    STATIC_ASSERT(sizeof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC) == 0x40);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC, rows) == 0);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC, weights) == 0x30);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC, bones) == 0x3c);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_MORPH_DELTA_PC) == 0xc);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_MORPH_DELTA_PC, vectors) == 8);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, record_count) == 4);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, morph_count) == 0xc);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, records) == 0x14);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, morph_deltas) == 0x18);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_MORPH_WEIGHTS_PC, weight_words) == 0);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC) == 0x40);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC, rows) == 0);
    extern "C" i32 PcGeometryMatrixCapacityPc;

    static __declspec(noinline) void PcGeometryCopyMatrixPc(NUMTX *dst, const NUMTX *src) {
        dst->m00 = src->m00;
        dst->m01 = src->m01;
        dst->m02 = src->m02;
        dst->m03 = src->m03;
        dst->m10 = src->m10;
        dst->m11 = src->m11;
        dst->m12 = src->m12;
        dst->m13 = src->m13;
        dst->m20 = src->m20;
        dst->m21 = src->m21;
        dst->m22 = src->m22;
        dst->m23 = src->m23;
        dst->m30 = src->m30;
        dst->m31 = src->m31;
        dst->m32 = src->m32;
        dst->m33 = src->m33;
    }

#define PC_GEOMETRY_SCALE(out, v, s)                                                                                   \
    {                                                                                                                  \
        f32 scale = (s);                                                                                               \
        (out).x = (v).x * scale;                                                                                       \
        (out).y = (v).y * scale;                                                                                       \
        (out).z = (v).z * scale;                                                                                       \
    }
#define PC_GEOMETRY_ADD(out, a, b)                                                                                     \
    {                                                                                                                  \
        (out).x = (a).x + (b).x;                                                                                       \
        (out).y = (a).y + (b).y;                                                                                       \
        (out).z = (a).z + (b).z;                                                                                       \
    }
#define PC_GEOMETRY_ROTATE(out, v, m)                                                                                  \
    {                                                                                                                  \
        f32 x = (v).x * (m)->m00 + (v).y * (m)->m10 + (v).z * (m)->m20;                                                \
        f32 y = (v).x * (m)->m01 + (v).y * (m)->m11 + (v).z * (m)->m21;                                                \
        (out).z = (v).x * (m)->m02 + (v).y * (m)->m12 + (v).z * (m)->m22;                                              \
        (out).x = x;                                                                                                   \
        (out).y = y;                                                                                                   \
    }

    extern "C" {
        extern NUVEC4 PcGeometryDefaultOriginPc;
        extern NUMTX PcGeometryViewMatrixPc;
        extern NUMTX PcGeometryProjectionMatrixPc;
        extern NUMTX PcGeometryViewProjectionMatrixPc;
        extern NUMTX PcGeometryInverseViewProjectionMatrixPc;
        extern NUMTX PcGeometryResetMatrixPc;
        extern NUMTX PcGeometryAdjustedProjectionMatrixPc;
        extern NUMTX PcGeometryTranslationMatrixPc;
        extern NUMTX PcGeometryScaleMatrixPc;
        void __cdecl NuMtxGetPerspectiveD3D(NUMTX *, f32 *, f32 *, f32 *, f32 *);
    }

    typedef struct PC_GEOMETRY_FORMAT_RECORD_PC {
        u8 unknown_00[0x18];
        i32 strip_segments;
        u8 unknown_1c[0x5c];
    } PC_GEOMETRY_FORMAT_RECORD_PC;
    typedef struct PC_GEOMETRY_FORMAT_PC {
        u32 unknown_00;
        u32 record_count;
        u32 unknown_08;
        PC_GEOMETRY_FORMAT_RECORD_PC *records;
    } PC_GEOMETRY_FORMAT_PC;
    typedef struct PC_DYNAMIC_GEOMETRY_PC {
        PC_GEOMETRY_FORMAT_PC *format;
        u8 unknown_04[0x10];
        PC_BUFFER_RESOURCE_PC *vertex_buffer;
        PC_BUFFER_RESOURCE_PC *strip_vertex_buffer;
        PC_BUFFER_RESOURCE_PC *strip_index_buffer;
    } PC_DYNAMIC_GEOMETRY_PC;
    typedef struct PC_GEOMETRY_STRIP_VERTEX_PC {
        f32 fraction, side;
        u8 unknown_08[8];
    } PC_GEOMETRY_STRIP_VERTEX_PC;
    typedef struct PC_GEOMETRY_CORNER_PC {
        NUVEC position;
        f32 w;
    } PC_GEOMETRY_CORNER_PC;
    typedef struct PC_GEOMETRY_OUTPUT_PC {
        PC_GEOMETRY_CORNER_PC corners[4];
        NUVEC basis_axis;
        f32 distance_squared;
    } PC_GEOMETRY_OUTPUT_PC;
    typedef struct PC_GEOMETRY_SORT_ITEM_PC {
        u32 output_index;
        f32 distance_squared;
        u32 unknown_08;
    } PC_GEOMETRY_SORT_ITEM_PC;
    STATIC_ASSERT(sizeof(PC_GEOMETRY_FORMAT_RECORD_PC) == 0x78);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_FORMAT_RECORD_PC, strip_segments) == 0x18);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_FORMAT_PC, record_count) == 4);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_FORMAT_PC, records) == 0xc);
    STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, format) == 0);
    STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, vertex_buffer) == 0x14);
    STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, strip_vertex_buffer) == 0x18);
    STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, strip_index_buffer) == 0x1c);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_STRIP_VERTEX_PC) == 0x10);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_STRIP_VERTEX_PC, fraction) == 0);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_STRIP_VERTEX_PC, side) == 4);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_OUTPUT_PC) == 0x50);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_OUTPUT_PC, corners) + 2 * sizeof(PC_GEOMETRY_CORNER_PC) == 0x20);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_OUTPUT_PC, distance_squared) == 0x4c);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_SORT_ITEM_PC) == 0xc);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_SORT_ITEM_PC, output_index) == 0);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_SORT_ITEM_PC, distance_squared) == 4);
    extern "C" NUVEC4 PcGeometryDistanceOriginPc;

    typedef struct PC_GEOMETRY_INPUT_PC {
        PC_GEOMETRY_CORNER_PC corners[4];
        NUVEC basis_weights;
        u32 unknown_4c;
        f32 output_w[4];
        i32 skin_indices[3];
        u8 skin_weights[3];
        u8 unknown_6f;
    } PC_GEOMETRY_INPUT_PC;
    typedef struct PC_GEOMETRY_GROUP_PC {
        u32 element_count;
        u32 first_element;
        u32 transform_index;
    } PC_GEOMETRY_GROUP_PC;
    typedef struct PC_GEOMETRY_MESH_PC {
        u8 unknown_00[8];
        u32 group_count;
        PC_GEOMETRY_INPUT_PC *elements;
        PC_GEOMETRY_GROUP_PC *groups;
    } PC_GEOMETRY_MESH_PC;
    typedef struct PC_GEOMETRY_SKIN_PC {
        u8 unknown_00[8];
        u32 matrix_group_count;
        u32 unknown_0c;
        NUMTX *matrices;
    } PC_GEOMETRY_SKIN_PC;
    typedef struct PC_GEOMETRY_BASIS_MATRIX_PC {
        PC_GEOMETRY_BASIS_VECTOR_PC rows[4];
    } PC_GEOMETRY_BASIS_MATRIX_PC;
    typedef struct PC_GEOMETRY_BASIS_PC {
        PC_GEOMETRY_BASIS_MATRIX_PC matrices[3];
    } PC_GEOMETRY_BASIS_PC;
    STATIC_ASSERT(sizeof(PC_GEOMETRY_INPUT_PC) == 0x70);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, corners) == 0);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, basis_weights) == 0x40);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, output_w) == 0x50);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, skin_indices) == 0x60);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, skin_weights) == 0x6c);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_GROUP_PC) == 0xc);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_GROUP_PC, element_count) == 0);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_GROUP_PC, first_element) == 4);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_GROUP_PC, transform_index) == 8);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_MESH_PC, group_count) == 8);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_MESH_PC, elements) == 0xc);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_MESH_PC, groups) == 0x10);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_SKIN_PC, matrix_group_count) == 8);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_SKIN_PC, matrices) == 0x10);
    STATIC_ASSERT(sizeof(PC_GEOMETRY_BASIS_PC) == 0xc0);
    extern "C" PC_GEOMETRY_BASIS_PC *PcGeometryTransformsPc;
    extern "C" PC_GEOMETRY_OUTPUT_PC *PcGeometryOutputPc;

    STATIC_ASSERT(sizeof(PC_GEOMETRY_CORNER_PC) == 0x10);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_CORNER_PC, position) == 0);
    STATIC_ASSERT(offsetof(PC_GEOMETRY_CORNER_PC, w) == 0xc);

    typedef struct PC_MATERIAL_UPDATE_VIEW {
        u8 unknown_00[0x3c];
        void *display;
        u8 unknown_40[4];
        u32 attribs;
        u8 unknown_48[0xc];
        NUVEC diffuse;
        u8 unknown_60[0x14];
        i16 texture_id;
        u8 unknown_76[0x3e];
        PC_MATERIAL_SHADER_DESC_VIEW shader;
        PC_TEXTURE_RESOURCE_PC *vertex_declaration;
        u16 version;
    } PC_MATERIAL_UPDATE_VIEW;
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, display) == 0x3c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, attribs) == 0x44);
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, diffuse) == 0x54);
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, texture_id) == 0x74);
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, shader) == 0xb4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, vertex_declaration) == 0x2bc);
    STATIC_ASSERT(offsetof(PC_MATERIAL_UPDATE_VIEW, version) == 0x2c0);
    typedef struct PC_SHADER_HANDLE_VIEW {
        i16 handle;
    } PC_SHADER_HANDLE_VIEW;
    STATIC_ASSERT(offsetof(PC_SHADER_HANDLE_VIEW, handle) == 0);
    struct PC_MATERIAL_DEVICE_CAPS_VIEW {
        u8 unknown_000[0x654];
        u8 shader_caps;
        i32 __thiscall ShaderVariant16();
        i32 __thiscall ShaderVariant1();
    };
    STATIC_ASSERT(offsetof(PC_MATERIAL_DEVICE_CAPS_VIEW, shader_caps) == 0x654);

    typedef struct PC_MATERIAL_CREATE_SHADER PC_MATERIAL_CREATE_SHADER;
    extern "C" void __cdecl NuShaderMtlDescInit(PC_MATERIAL_CREATE_SHADER *);

    extern "C" {
        POST_SHADER_PROGRAM_PC *__cdecl PostShaderProgramCreatePc(i32, const DWORD *, const DWORD *);
        extern const DWORD NuMaterialShaderTokens769710Pc[76] = {
            0xfffe0101u, 0x0022fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xfffe0101u, 0x00000001u, 0x0000001cu,
            0x20000120u, 0x0000004cu, 0x00000030u, 0x00000002u, 0x00020004u, 0x0000003cu, 0x00000000u, 0x77656976u,
            0x6a6f7250u, 0xababab00u, 0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u, 0x315f7376u, 0x4d00315fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u,
            0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u, 0x00000051u, 0xa00f0004u, 0x3f800000u, 0x00000000u,
            0x00000000u, 0x00000000u, 0x0000001fu, 0x80000000u, 0x900f0000u, 0x0000001fu, 0x8000000au, 0x900f0001u,
            0x0000001fu, 0x80000005u, 0x900f0002u, 0x00000009u, 0xc0010000u, 0x90e40000u, 0xa0e40000u, 0x00000009u,
            0xc0020000u, 0x90e40000u, 0xa0e40001u, 0x00000009u, 0xc0040000u, 0x90e40000u, 0xa0e40002u, 0x00000009u,
            0xc0080000u, 0x90e40000u, 0xa0e40003u, 0x00000001u, 0xd00f0000u, 0x90e40001u, 0x00000004u, 0xe00f0000u,
            0x90040002u, 0xa0500004u, 0xa0050004u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens769840Pc[44] = {
            0xffff0101u, 0x0023fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xffff0101u, 0x00000001u, 0x0000001cu,
            0x00000120u, 0x0000004cu, 0x00000030u, 0x00000003u, 0x00000001u, 0x0000003cu, 0x00000000u, 0x66666964u,
            0x5f657375u, 0x00786574u, 0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du,
            0x39207265u, 0x2e35312eu, 0x2e393737u, 0x30303030u, 0xababab00u, 0x00000042u, 0xb00f0000u, 0x00000005u,
            0x810f0000u, 0xb0e40000u, 0x90e40000u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens7698f0Pc[89] = {
            0xfffe0200u, 0x0022fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xfffe0200u, 0x00000001u, 0x0000001cu,
            0x20000120u, 0x0000004cu, 0x00000030u, 0x00000002u, 0x00020004u, 0x0000003cu, 0x00000000u, 0x77656976u,
            0x6a6f7250u, 0xababab00u, 0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u, 0x325f7376u, 0x4d00305fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u,
            0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u, 0x05000051u, 0xa00f0004u, 0x3f000000u, 0xbf000000u,
            0x00000000u, 0x00000000u, 0x0200001fu, 0x80000000u, 0x900f0000u, 0x0200001fu, 0x8000000au, 0x900f0001u,
            0x0200001fu, 0x80000005u, 0x900f0002u, 0x03000009u, 0xc0040000u, 0x90e40000u, 0xa0e40002u, 0x03000009u,
            0x80040000u, 0x90e40000u, 0xa0e40003u, 0x02000006u, 0x80080000u, 0x80aa0000u, 0x03000009u, 0x80010000u,
            0x90e40000u, 0xa0e40000u, 0x03000009u, 0x80020000u, 0x90e40000u, 0xa0e40001u, 0x03000005u, 0x80030001u,
            0x80ff0000u, 0x80e40000u, 0x02000001u, 0xc00b0000u, 0x80a40000u, 0x04000004u, 0xe0030000u, 0x80e40001u,
            0xa0e40004u, 0xa0000004u, 0x02000001u, 0xd00f0000u, 0x90e40001u, 0x02000001u, 0xe00c0000u, 0x90440002u,
            0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens769a58Pc[124] = {
            0xffff0200u, 0x002bfffeu, 0x42415443u, 0x0000001cu, 0x00000077u, 0xffff0200u, 0x00000002u, 0x0000001cu,
            0x20000120u, 0x00000070u, 0x00000044u, 0x00000003u, 0x00020001u, 0x00000054u, 0x00000000u, 0x00000064u,
            0x00010003u, 0x00060001u, 0x00000054u, 0x00000000u, 0x6b636162u, 0x66667542u, 0x745f7265u, 0xab007865u,
            0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x74736964u, 0x5f74726fu, 0x00786574u, 0x325f7370u,
            0x4d00305fu, 0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u,
            0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u, 0x05000051u, 0xa00f0000u, 0xbf000000u,
            0xbf000000u, 0x80000000u, 0x3f800000u, 0x05000051u, 0xa00f0001u, 0x3c23d70au, 0x40000000u, 0x00000000u,
            0x00000000u, 0x05000051u, 0xa00f0002u, 0x3f800000u, 0x3f800000u, 0x3f800000u, 0x40000000u, 0x0200001fu,
            0x80000000u, 0x900f0000u, 0x0200001fu, 0x80000000u, 0xb02f0000u, 0x0200001fu, 0x90000000u, 0xa00f0800u,
            0x0200001fu, 0x90000000u, 0xa00f0801u, 0x02000001u, 0x80210000u, 0xb0aa0000u, 0x02000001u, 0x80220000u,
            0xb0ff0000u, 0x03000042u, 0x802f0000u, 0x80e40000u, 0xa0e40801u, 0x03000002u, 0x80270000u, 0x80e40000u,
            0xa0e40000u, 0x04000004u, 0x80230000u, 0x80e40000u, 0xa0000001u, 0xb0e40000u, 0x03000005u, 0x80240000u,
            0x80aa0000u, 0xa0ff0000u, 0x03000042u, 0x802f0001u, 0x80e40000u, 0xa0e40800u, 0x03000005u, 0x802f0001u,
            0x80e40001u, 0x90e40000u, 0x02000001u, 0x80280000u, 0x80aa0000u, 0x02000001u, 0x80270000u, 0xa0550001u,
            0x03000005u, 0x802f0000u, 0x80e40001u, 0x80e40000u, 0x03000005u, 0x802f0000u, 0x80e40000u, 0xa0e40002u,
            0x02000001u, 0x802f0800u, 0x80e40000u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens769c48Pc[87] = {
            0xfffe0101u, 0x0022fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xfffe0101u, 0x00000001u, 0x0000001cu,
            0x20000120u, 0x0000004cu, 0x00000030u, 0x00000002u, 0x00020004u, 0x0000003cu, 0x00000000u, 0x77656976u,
            0x6a6f7250u, 0xababab00u, 0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u, 0x315f7376u, 0x4d00315fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u,
            0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u, 0x00000051u, 0xa00f0004u, 0x3c23d70au, 0x3f000000u,
            0xbf000000u, 0x3f800000u, 0x0000001fu, 0x80000000u, 0x900f0000u, 0x0000001fu, 0x8000000au, 0x900f0001u,
            0x00000009u, 0xc0040000u, 0x90e40000u, 0xa0e40002u, 0x00000009u, 0x80040000u, 0x90e40000u, 0xa0e40003u,
            0x00000006u, 0x80080000u, 0x80aa0000u, 0x00000009u, 0x80010000u, 0x90e40000u, 0xa0e40000u, 0x00000009u,
            0x80020000u, 0x90e40000u, 0xa0e40001u, 0x00000004u, 0x80030001u, 0x80e40000u, 0x80ff0000u, 0xa0000004u,
            0x00000001u, 0xc00b0000u, 0x80a40000u, 0x00000004u, 0xe0030000u, 0x80e40001u, 0xa0e90004u, 0xa0550004u,
            0x00000001u, 0xd00f0000u, 0x90e40001u, 0x00000001u, 0xe00c0000u, 0xa0ff0004u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens769da8Pc[54] = {
            0xffff0101u, 0x0024fffeu, 0x42415443u, 0x0000001cu, 0x00000057u, 0xffff0101u, 0x00000001u, 0x0000001cu,
            0x00000120u, 0x00000050u, 0x00000030u, 0x00000003u, 0x00000001u, 0x00000040u, 0x00000000u, 0x6b636162u,
            0x66667542u, 0x745f7265u, 0xab007865u, 0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x315f7370u,
            0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u, 0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u,
            0x6c69706du, 0x39207265u, 0x2e35312eu, 0x2e393737u, 0x30303030u, 0xababab00u, 0x00000051u, 0xa00f0000u,
            0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000042u, 0xb00f0000u, 0x00000005u, 0x81070000u,
            0xb0e40000u, 0x90e40000u, 0x40000001u, 0x80080000u, 0xa0ff0000u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens769e80Pc[262] = {
            0xfffe0101u, 0x005bfffeu, 0x42415443u, 0x0000001cu, 0x00000134u, 0xfffe0101u, 0x00000008u, 0x0000001cu,
            0x20000120u, 0x0000012du, 0x000000bcu, 0x00540002u, 0x01520001u, 0x000000c8u, 0x00000000u, 0x000000d8u,
            0x00500002u, 0x01420003u, 0x000000e4u, 0x00000000u, 0x000000f4u, 0x00590002u, 0x01660001u, 0x000000c8u,
            0x00000000u, 0x00000106u, 0x00300002u, 0x00c20001u, 0x000000c8u, 0x00000000u, 0x00000111u, 0x00280002u,
            0x00a20001u, 0x000000c8u, 0x00000000u, 0x00000117u, 0x000c0002u, 0x00320004u, 0x000000e4u, 0x00000000u,
            0x0000011cu, 0x00550002u, 0x01560004u, 0x000000e4u, 0x00000000u, 0x00000124u, 0x00000002u, 0x00020004u,
            0x000000e4u, 0x00000000u, 0x65636166u, 0x614d6e4fu, 0xabab0067u, 0x00030001u, 0x00040001u, 0x00000001u,
            0x00000000u, 0x65636166u, 0x744d6e4fu, 0xabab0078u, 0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u,
            0x65636166u, 0x72546e4fu, 0x6c736e61u, 0x6f697461u, 0x6f66006eu, 0x61705f67u, 0x736d6172u, 0x69546b00u,
            0x7600746eu, 0x00776569u, 0x77656976u, 0x0078744du, 0x77656976u, 0x6a6f7250u, 0x5f737600u, 0x00315f31u,
            0x7263694du, 0x666f736fu, 0x52282074u, 0x4c482029u, 0x53204c53u, 0x65646168u, 0x6f432072u, 0x6c69706du,
            0x39207265u, 0x2e39312eu, 0x2e393439u, 0x34303131u, 0xababab00u, 0x00000051u, 0xa00f0004u, 0x3f800000u,
            0x00000000u, 0x3fb8aa3bu, 0x40000000u, 0x00000051u, 0xa00f0005u, 0xbf800000u, 0x3f800000u, 0x00000000u,
            0x00000000u, 0x0000001fu, 0x80000000u, 0x900f0000u, 0x0000001fu, 0x8000000au, 0x900f0001u, 0x0000001fu,
            0x80000005u, 0x900f0002u, 0x0000001fu, 0x80010005u, 0x900f0003u, 0x00000004u, 0x800f0000u, 0x90240000u,
            0xa0400004u, 0xa0150004u, 0x00000009u, 0x80010001u, 0x80e40000u, 0xa0e40050u, 0x00000009u, 0x80020001u,
            0x80e40000u, 0xa0e40051u, 0x00000009u, 0x80040001u, 0x80e40000u, 0xa0e40052u, 0x00000002u, 0x80070000u,
            0x80e40001u, 0xa0e40059u, 0x00000005u, 0x80030001u, 0x90e40003u, 0xa0000054u, 0x00000001u, 0x80040001u,
            0xa0000004u, 0x00000008u, 0x80010002u, 0x80e40001u, 0xa0f40055u, 0x00000008u, 0x80020002u, 0x80e40001u,
            0xa0f40056u, 0x00000008u, 0x80040002u, 0x80e40001u, 0xa0f40057u, 0x00000008u, 0x80080001u, 0x80e40001u,
            0xa0f40058u, 0x00000002u, 0x80070001u, 0x80e40000u, 0x80e40002u, 0x00000009u, 0x80010000u, 0x80e40001u,
            0xa0e4000fu, 0x00000006u, 0x80010000u, 0x80000000u, 0x00000009u, 0x80020000u, 0x80e40001u, 0xa0e4000cu,
            0x00000009u, 0x80040000u, 0x80e40001u, 0xa0e4000du, 0x00000009u, 0x80080000u, 0x80e40001u, 0xa0e4000eu,
            0x00000005u, 0x80070000u, 0x80000000u, 0x80f90000u, 0x00000008u, 0x80010000u, 0x80e40000u, 0x80e40000u,
            0x00000007u, 0x80010000u, 0x80000000u, 0x00000006u, 0x80010000u, 0x80000000u, 0x00000005u, 0x80010000u,
            0x80000000u, 0xa0ff0030u, 0x00000005u, 0x80010000u, 0x80000000u, 0x80000000u, 0x00000005u, 0x80010000u,
            0x80000000u, 0xa0aa0004u, 0x0000004eu, 0x80040002u, 0x80000000u, 0x00000001u, 0x80010000u, 0x80aa0002u,
            0x00000006u, 0xe0010001u, 0x80000000u, 0x00000009u, 0xc0010000u, 0x80e40001u, 0xa0e40000u, 0x00000009u,
            0xc0020000u, 0x80e40001u, 0xa0e40001u, 0x00000009u, 0xc0040000u, 0x80e40001u, 0xa0e40002u, 0x00000009u,
            0xc0080000u, 0x80e40001u, 0xa0e40003u, 0x00000005u, 0x80080000u, 0x90ff0001u, 0xa0ff0028u, 0x00000001u,
            0x80070000u, 0x90e40001u, 0x00000001u, 0x800b0001u, 0xa0e40004u, 0x00000004u, 0x800f0001u, 0xa0240028u,
            0x80400001u, 0x80d50001u, 0x00000005u, 0xd00f0000u, 0x80e40000u, 0x80e40001u, 0x00000005u, 0xe0030000u,
            0x90e40002u, 0xa0e40005u, 0x00000001u, 0xe0060001u, 0xa0550004u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens76a3c8Pc[91] = {
            0xffff0200u, 0x002efffeu, 0x42415443u, 0x0000001cu, 0x00000083u, 0xffff0200u, 0x00000002u, 0x0000001cu,
            0x20000120u, 0x0000007cu, 0x00000044u, 0x00000003u, 0x00020001u, 0x00000050u, 0x00000000u, 0x00000060u,
            0x00000002u, 0x00020001u, 0x0000006cu, 0x00000000u, 0x66666964u, 0x5f657375u, 0x00786574u, 0x000c0004u,
            0x00010001u, 0x00000001u, 0x00000000u, 0x5f676f66u, 0x6f6c6f63u, 0xabab0072u, 0x00030001u, 0x00040001u,
            0x00000001u, 0x00000000u, 0x325f7370u, 0x4d00305fu, 0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u,
            0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u,
            0x05000051u, 0xa00f0001u, 0x3f000000u, 0x3f000000u, 0x3f000000u, 0x3f800000u, 0x0200001fu, 0x80000000u,
            0x900f0000u, 0x0200001fu, 0x80000000u, 0xb0230000u, 0x0200001fu, 0x80000000u, 0xb0210001u, 0x0200001fu,
            0x90000000u, 0xa00f0800u, 0x03000042u, 0x802f0000u, 0xb0e40000u, 0xa0e40800u, 0x04000004u, 0x80270000u,
            0x80e40000u, 0x90e40000u, 0xa1e40000u, 0x03000005u, 0x80280001u, 0x80ff0000u, 0x90ff0000u, 0x04000004u,
            0x80270001u, 0xb0000001u, 0x80e40000u, 0xa0e40000u, 0x02000001u, 0x802f0800u, 0x80e40001u, 0x02000001u,
            0x802f0801u, 0xa0e40001u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens76a298Pc[76] = {
            0xffff0101u, 0x002ffffeu, 0x42415443u, 0x0000001cu, 0x00000083u, 0xffff0101u, 0x00000002u, 0x0000001cu,
            0x00000120u, 0x0000007cu, 0x00000044u, 0x00000003u, 0x00000001u, 0x00000050u, 0x00000000u, 0x00000060u,
            0x00000002u, 0x00020001u, 0x0000006cu, 0x00000000u, 0x66666964u, 0x5f657375u, 0x00786574u, 0x000c0004u,
            0x00010001u, 0x00000001u, 0x00000000u, 0x5f676f66u, 0x6f6c6f63u, 0xabab0072u, 0x00030001u, 0x00040001u,
            0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u, 0x44334420u,
            0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du, 0x39207265u, 0x2e35312eu, 0x2e393737u, 0x30303030u,
            0xababab00u, 0x00000051u, 0xa00f0001u, 0x3f800000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000042u,
            0xb00f0000u, 0x00000040u, 0xb00f0001u, 0x00000005u, 0x800f0000u, 0xb0e40000u, 0x90e40000u, 0x00000001u,
            0x80080000u, 0x80ff0000u, 0x40000008u, 0xb0070000u, 0xa0e40001u, 0xb0e40001u, 0x00000012u, 0x80070000u,
            0xb0e40000u, 0x80e40000u, 0xa0e40000u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens76a538Pc[43] = {
            0xfffe0101u, 0x0016fffeu, 0x42415443u, 0x0000001cu, 0x00000023u, 0xfffe0101u, 0x00000000u, 0x00000000u,
            0x20000120u, 0x0000001cu, 0x315f7376u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u,
            0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u,
            0x0000001fu, 0x80000000u, 0x900f0000u, 0x0000001fu, 0x8000000au, 0x900f0001u, 0x0000001fu, 0x80000005u,
            0x900f0002u, 0x00000001u, 0xc00f0000u, 0x90e40000u, 0x00000001u, 0xd00f0000u, 0x90e40001u, 0x00000001u,
            0xe0030000u, 0x90e40002u, 0x0000ffffu,
        };

        extern const DWORD NuMaterialShaderTokens76a5e8Pc[44] = {
            0xffff0101u, 0x0023fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xffff0101u, 0x00000001u, 0x0000001cu,
            0x00000120u, 0x0000004cu, 0x00000030u, 0x00000003u, 0x00000001u, 0x0000003cu, 0x00000000u, 0x66666964u,
            0x5f657375u, 0x00786574u, 0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du,
            0x39207265u, 0x2e35312eu, 0x2e393737u, 0x30303030u, 0xababab00u, 0x00000042u, 0xb00f0000u, 0x00000005u,
            0x800f0000u, 0xb0e40000u, 0x90e40000u, 0x0000ffffu,
        };

        extern const PC_VERTEX_ELEMENT_PC NuMaterialDefaultElementsPc[4] = {
            {0, 0, 2, 0, 0, 0},
            {0, 12, 4, 0, 10, 0},
            {0, 16, 1, 0, 5, 0},
            {255, 0, 17, 0, 0, 0},
        };

        extern const PC_VERTEX_ELEMENT_PC NuMaterialDebrisElementsPc[4] = {
            {0, 0, 2, 0, 0, 0},
            {0, 12, 4, 0, 10, 0},
            {0, 16, 1, 0, 5, 0},
            {255, 0, 17, 0, 0, 0},
        };

        extern const PC_VERTEX_ELEMENT_PC NuMaterialFaceOnElementsPc[5] = {
            {0, 0, 2, 0, 0, 0}, {0, 12, 4, 0, 10, 0}, {0, 16, 1, 0, 5, 0}, {0, 24, 1, 0, 5, 1}, {255, 0, 17, 0, 0, 0},
        };

        extern const u8 NuMaterialDistortImagePc[824] = {
            0x44, 0x44, 0x53, 0x20, 0x7c, 0x00, 0x00, 0x00, 0x07, 0x10, 0x0a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00,
            0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x44, 0x58, 0x54, 0x31, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xc0, 0xb3, 0xe0, 0x9d, 0xaa, 0xaf, 0xbf, 0x35, 0xc0, 0xdb, 0x41, 0x93, 0x00, 0xa8, 0xfe, 0x55,
            0x82, 0xc3, 0xc1, 0x89, 0x58, 0x5e, 0x5e, 0xfe, 0xa0, 0x71, 0xa7, 0x53, 0xa0, 0xe8, 0x78, 0x5e, 0x00, 0x5b,
            0xea, 0x33, 0xa8, 0xae, 0xbf, 0xd5, 0x43, 0x33, 0xa2, 0x2c, 0xa0, 0xe0, 0x60, 0x68, 0x41, 0x65, 0x00, 0x2c,
            0x35, 0x35, 0xbf, 0xfe, 0x40, 0x8e, 0xc0, 0x74, 0x57, 0x57, 0xaf, 0x09, 0x61, 0x93, 0xa0, 0x8e, 0x2d, 0x2d,
            0x2d, 0x2d, 0xb0, 0xa9, 0x41, 0x7b, 0x5f, 0xff, 0x2f, 0x02, 0xd3, 0xab, 0x11, 0xaa, 0xfe, 0xaf, 0x2d, 0x25,
            0xf7, 0xab, 0x54, 0x44, 0x78, 0x78, 0xe0, 0x80, 0xdb, 0x65, 0x53, 0x44, 0x55, 0xff, 0xab, 0x03, 0x56, 0x56,
            0x4a, 0x34, 0xd5, 0xbf, 0xaa, 0x80, 0xc4, 0x43, 0x4b, 0x3d, 0xad, 0xad, 0x2d, 0x35, 0x40, 0x8f, 0xa2, 0x44,
            0x2d, 0x2d, 0xad, 0xad, 0x45, 0x7b, 0xc0, 0x75, 0x25, 0x2f, 0x2f, 0xaa, 0xd6, 0xc1, 0x71, 0x93, 0x02, 0x2f,
            0xb5, 0xb5, 0xfb, 0xc1, 0x9a, 0x93, 0x78, 0x5c, 0x5e, 0x5e, 0x3b, 0x9c, 0x9a, 0x42, 0x80, 0xa8, 0xfe, 0x57,
            0xba, 0x65, 0x1a, 0x63, 0x0b, 0xbd, 0xd5, 0x55, 0x18, 0x56, 0x5b, 0x54, 0x00, 0xaf, 0xb5, 0xb5, 0x4c, 0x54,
            0xb5, 0x4d, 0x2d, 0x35, 0xd5, 0x55, 0x66, 0x64, 0x22, 0x5e, 0x5c, 0xf8, 0x0a, 0x02, 0xc8, 0x74, 0x01, 0x6b,
            0x2b, 0x2d, 0xad, 0xff, 0x37, 0xb3, 0xd4, 0x8c, 0x09, 0xab, 0xff, 0x5f, 0x3b, 0x9c, 0x3a, 0x62, 0xfa, 0x5e,
            0x5c, 0x78, 0x59, 0x4a, 0x7d, 0x29, 0xb8, 0xfe, 0xd7, 0xd7, 0x9b, 0x4a, 0x5e, 0x41, 0x2a, 0xd7, 0xd7, 0xdf,
            0xda, 0x44, 0x7c, 0x42, 0x0b, 0x2d, 0x35, 0xb5, 0x58, 0x45, 0x54, 0x3a, 0x00, 0xa0, 0xf8, 0x5e, 0x29, 0x6c,
            0x46, 0x42, 0x80, 0xfa, 0xff, 0xf5, 0x88, 0x5b, 0x80, 0x32, 0x02, 0x0b, 0x09, 0x0d, 0x1a, 0x85, 0x94, 0x7b,
            0x09, 0x89, 0xad, 0xff, 0xfb, 0x8c, 0xdc, 0x5a, 0x78, 0x7a, 0x7e, 0x7e, 0x3e, 0x43, 0x9e, 0x31, 0x57, 0x5f,
            0x7a, 0xe0, 0xbe, 0x5a, 0xdf, 0x31, 0xd5, 0xa5, 0x0d, 0x0b, 0xbd, 0x72, 0xbb, 0x63, 0x5c, 0xde, 0xbe, 0x28,
            0x5d, 0x63, 0xf2, 0x59, 0x5c, 0x78, 0x7a, 0x7a, 0x89, 0x59, 0xa0, 0x3a, 0x5e, 0x78, 0xf8, 0xfe, 0xc0, 0x59,
            0xc6, 0x3b, 0x5e, 0x7a, 0xe0, 0x80, 0xf1, 0x6b, 0x0c, 0x4b, 0x20, 0xe0, 0x7a, 0x57, 0x59, 0x63, 0xb8, 0x5c,
            0x2a, 0xb8, 0xdc, 0x5e, 0x3d, 0x7a, 0xdc, 0x53, 0xb7, 0x95, 0xb5, 0x2d, 0xdc, 0x91, 0x3e, 0x5a, 0x55, 0xfb,
            0xe2, 0xa8, 0xdb, 0x92, 0x5a, 0x6a, 0xa9, 0x0b, 0xab, 0x55, 0xb8, 0x9a, 0x8a, 0x81, 0xe0, 0x60, 0x7a, 0x5f,
            0x23, 0x81, 0xe0, 0x41, 0x7e, 0xf8, 0xa0, 0x80, 0x40, 0x73, 0xc0, 0x71, 0xb5, 0x2d, 0x2b, 0xa8, 0x40, 0x64,
            0x29, 0x43, 0x5f, 0x5f, 0xea, 0x83, 0xf4, 0x64, 0xcf, 0x3b, 0x0d, 0x09, 0x01, 0x8b, 0x94, 0x99, 0x36, 0x74,
            0xad, 0xad, 0xad, 0xaf, 0xd9, 0xa1, 0x37, 0x7a, 0xfa, 0x62, 0x7a, 0x55, 0xb4, 0x82, 0x36, 0x62, 0x55, 0x5f,
            0xea, 0xc2, 0x83, 0x81, 0x49, 0x6a, 0x2d, 0x2d, 0x35, 0x35, 0xa0, 0x81, 0x80, 0x7b, 0x00, 0x00, 0xa8, 0x5c,
            0x40, 0xac, 0x00, 0x83, 0x5e, 0xbe, 0x2e, 0x2a, 0xc0, 0xd3, 0x41, 0x6c, 0x55, 0xff, 0xaa, 0x00, 0x61, 0x92,
            0x45, 0x84, 0xd5, 0xad, 0x09, 0x0b, 0xe9, 0x92, 0xc2, 0x69, 0xe0, 0x78, 0x5e, 0xd5, 0xe9, 0x71, 0x85, 0x63,
            0xa8, 0x78, 0x58, 0x5e, 0x2c, 0x7b, 0xe1, 0x3a, 0xe0, 0x78, 0x5e, 0x5f, 0x60, 0x54, 0x61, 0x52, 0x55, 0xf5,
            0x2f, 0x0b, 0x80, 0x84, 0x60, 0x62, 0xad, 0xab, 0xa8, 0x00, 0xa0, 0xa5, 0x86, 0xa2, 0xfa, 0xf8, 0x58, 0x5c,
            0x03, 0x92, 0x35, 0x84, 0x82, 0xe2, 0x7a, 0x5c, 0xb7, 0x55, 0xa6, 0x3b, 0x55, 0xd5, 0xbf, 0x02, 0x00, 0x7e,
            0x44, 0x3c, 0xad, 0x2d, 0x27, 0x37, 0x96, 0xaa, 0xc4, 0x6c, 0x0d, 0x2d, 0xbf, 0xff, 0x7a, 0x93, 0xfb, 0x41,
            0x02, 0xe0, 0x5c, 0x5c, 0x59, 0x55, 0xfd, 0x49, 0x02, 0x2f, 0xb5, 0xb5, 0x90, 0x55, 0xea, 0x52, 0x28, 0xa0,
            0xf8, 0x57, 0x57, 0x7c, 0xa2, 0x42, 0x0b, 0x2d, 0xad, 0xf5, 0x5a, 0x74, 0x1d, 0x3a, 0x5c, 0x78, 0xea, 0xe2,
            0xfb, 0x7a, 0xfe, 0x51, 0x25, 0x0d, 0x2f, 0xaf, 0x83, 0x69, 0xd7, 0x62, 0xed, 0xad, 0x0b, 0x03, 0x00, 0xac,
            0xe4, 0x52, 0x75, 0xff, 0xaa, 0x02, 0x68, 0x8a, 0x71, 0x64, 0x35, 0x35, 0x2b, 0x02, 0x17, 0x82, 0x23, 0x53,
            0x80, 0xa0, 0xfe, 0x57, 0xc4, 0x71, 0xe0, 0x6b, 0x00, 0xa2, 0x7a, 0x57, 0xa2, 0xa4, 0x18, 0x72, 0xf8, 0xbc,
            0xfe, 0x7a, 0xc4, 0x5d, 0xb9, 0x42, 0x2f, 0x2a, 0xab, 0xfd, 0xea, 0x7b, 0x53, 0x52, 0x62, 0xeb, 0x8b, 0xc0,
            0xb2, 0x71, 0x6a, 0x63, 0xbe, 0x2a, 0xaa, 0x7f, 0xb1, 0x72, 0x09, 0x5d, 0x7a, 0xe2, 0x0a, 0x0a, 0xd0, 0x63,
            0xd0, 0x72, 0x02, 0x56, 0x5e, 0x5e, 0x4f, 0x6b, 0x6f, 0x6b, 0xaa, 0xab, 0xab, 0xab,
        };

        extern const char NuMaterialSourcePathPc[] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\numtl_dlist.c";
    }

    extern "C" __declspec(noinline) void __cdecl NuMtlInitShaderResourcesPc(void) {
        NuDisplayMaterialDebrisProgramsPc = reinterpret_cast<PC_DL_SHADER_PROGRAM_PAIR_PC *>(
            PostShaderProgramCreatePc(2, NuMaterialShaderTokens769710Pc, NuMaterialShaderTokens769840Pc));
        if (PCDeviceHardwareVertexProcessingPC > 1)
            NuDisplayMaterialGlassProgramsPc = reinterpret_cast<PC_DL_SHADER_PROGRAM_PAIR_PC *>(
                PostShaderProgramCreatePc(2, NuMaterialShaderTokens7698f0Pc, NuMaterialShaderTokens769a58Pc));
        else
            NuDisplayMaterialGlassProgramsPc = reinterpret_cast<PC_DL_SHADER_PROGRAM_PAIR_PC *>(
                PostShaderProgramCreatePc(2, NuMaterialShaderTokens769c48Pc, NuMaterialShaderTokens769da8Pc));
        if (PCDeviceHardwareVertexProcessingPC > 1)
            NuDisplayMaterialFaceOnDecalProgramsPc = reinterpret_cast<PC_DL_SHADER_PROGRAM_PAIR_PC *>(
                PostShaderProgramCreatePc(2, NuMaterialShaderTokens769e80Pc, NuMaterialShaderTokens76a3c8Pc));
        NuDisplayMaterialFaceOnProgramsPc = reinterpret_cast<PC_DL_SHADER_PROGRAM_PAIR_PC *>(
            PostShaderProgramCreatePc(2, NuMaterialShaderTokens769e80Pc, NuMaterialShaderTokens76a298Pc));
        NuDisplayMaterialDefaultProgramsPc = reinterpret_cast<PC_DL_SHADER_PROGRAM_PAIR_PC *>(
            PostShaderProgramCreatePc(2, NuMaterialShaderTokens76a538Pc, NuMaterialShaderTokens76a5e8Pc));
        PCDeviceCreateVertexDeclaration(NuMaterialFaceOnElementsPc, &NuDisplayMaterialFaceOnDeclarationPc);
        PCDeviceCreateVertexDeclaration(NuMaterialDebrisElementsPc, &NuDisplayMaterialDebrisDeclarationPc);
        PCDeviceCreateVertexDeclaration(NuMaterialDefaultElementsPc, &NuDisplayMaterialDefaultDeclarationPc);
        NUTEXDIM_PC *texture = static_cast<NUTEXDIM_PC *>(NuAlloc(0x28, NuMaterialSourcePathPc, 0x159));
        memset(texture, 0, sizeof(*texture));
        texture->image_data = const_cast<u8 *>(NuMaterialDistortImagePc);
        texture->image_size = 0x338;
        NuDisplayMaterialDistortTexturePc = NuTexCreateNative(texture);
    }
    typedef struct PC_SPECIAL_GEOMETRY {
        u8 unknown_0000[8];
        u16 vertex_stride;
        u8 joint_indices[8];
        u8 unknown_0012[2];
        u32 base_vertex, vertex_count;
        u8 unknown_001c[8];
        PC_BUFFER_RESOURCE_PC *vertex_buffer;
        u32 morph_count;
        NUVEC **morph_deltas;
        u32 unknown_0030;
        void *dynamic_vertices;
    } PC_SPECIAL_GEOMETRY;
    STATIC_ASSERT(sizeof(PC_SPECIAL_GEOMETRY) == 0x38);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, vertex_stride) == 8);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, joint_indices) == 0xa);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, base_vertex) == 0x14);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, vertex_count) == 0x18);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, vertex_buffer) == 0x24);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, morph_count) == 0x28);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, morph_deltas) == 0x2c);
    STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, dynamic_vertices) == 0x34);
    typedef struct PC_SPECIAL_DEFORMER_WEIGHTS {
        u32 unknown_0000[2];
        f32 weights[1];
    } PC_SPECIAL_DEFORMER_WEIGHTS;
    STATIC_ASSERT(offsetof(PC_SPECIAL_DEFORMER_WEIGHTS, weights) == 8);
    typedef struct PC_SPECIAL_FACEON {
        i32 kind;
    } PC_SPECIAL_FACEON;
    typedef struct PC_SPECIAL_TRANSFORM_PACKET {
        NUMTX matrix;
    } PC_SPECIAL_TRANSFORM_PACKET;
    STATIC_ASSERT(offsetof(PC_SPECIAL_TRANSFORM_PACKET, matrix.m32) == 0x38);
    STATIC_ASSERT(offsetof(PC_SPECIAL_TRANSFORM_PACKET, matrix.m33) == 0x3c);

    typedef struct display_primitive_packet_pc_s {
        u32 type;
        u8 unknown_0004[0x14];
        u32 vertex_count;
        u8 unknown_001c[0x1c];
    } DISPLAY_PRIMITIVE_PACKET_PC;
    STATIC_ASSERT(sizeof(DISPLAY_PRIMITIVE_PACKET_PC) == 0x38);
    STATIC_ASSERT(offsetof(DISPLAY_PRIMITIVE_PACKET_PC, vertex_count) == 0x18);

    typedef struct PC_DL_SKIN_PALETTE_READER {
        i32 count;
        f32 elements[1];
    } PC_DL_SKIN_PALETTE_READER;
    STATIC_ASSERT(offsetof(PC_DL_SKIN_PALETTE_READER, count) == 0);
    STATIC_ASSERT(offsetof(PC_DL_SKIN_PALETTE_READER, elements) == 4);

    typedef struct PC_DL_LIGHTS_READER {
        NUVEC4 ambient;
        NUVEC4 intensity[3];
        NUVEC4 direction[3];
        NUMTX shine_matrix;
    } PC_DL_LIGHTS_READER;
    STATIC_ASSERT(sizeof(PC_DL_LIGHTS_READER) == 0xb0);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTS_READER, intensity) == 0x10);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTS_READER, direction) == 0x40);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTS_READER, shine_matrix) == 0x70);

    typedef union PC_DL_NORMALIZED_DIRECTION {
        NUVEC4 four;
        struct {
            NUVEC three;
            f32 w;
        } normalized;
    } PC_DL_NORMALIZED_DIRECTION;
    STATIC_ASSERT(sizeof(PC_DL_NORMALIZED_DIRECTION) == 16);
    STATIC_ASSERT(offsetof(PC_DL_NORMALIZED_DIRECTION, normalized.w) == 12);

    typedef struct PC_DL_FOG_READER {
        i32 enabled;
        u32 colour;
        f32 near_distance, far_distance, density;
    } PC_DL_FOG_READER;
    STATIC_ASSERT(sizeof(PC_DL_FOG_READER) == 0x14);
    STATIC_ASSERT(offsetof(PC_DL_FOG_READER, colour) == 4);
    STATIC_ASSERT(offsetof(PC_DL_FOG_READER, near_distance) == 8);
    STATIC_ASSERT(offsetof(PC_DL_FOG_READER, far_distance) == 0xc);
    STATIC_ASSERT(offsetof(PC_DL_FOG_READER, density) == 0x10);

    typedef struct PC_DL_LEGACY_LIGHTMAP_READER {
        i32 texture_id[3];
        f32 x_offset, y_offset;
    } PC_DL_LEGACY_LIGHTMAP_READER;
    STATIC_ASSERT(offsetof(PC_DL_LEGACY_LIGHTMAP_READER, x_offset) == 0xc);
    STATIC_ASSERT(offsetof(PC_DL_LEGACY_LIGHTMAP_READER, y_offset) == 0x10);

    typedef struct PC_DL_CAMERA_READER {
        i32 id;
        NUMTX view, projection;
        f32 viewport[4];
    } PC_DL_CAMERA_READER;
    STATIC_ASSERT(sizeof(PC_DL_CAMERA_READER) == 0x94);
    STATIC_ASSERT(offsetof(PC_DL_CAMERA_READER, view) == 4);
    STATIC_ASSERT(offsetof(PC_DL_CAMERA_READER, projection) == 0x44);
    STATIC_ASSERT(offsetof(PC_DL_CAMERA_READER, viewport) == 0x84);

    /* Source-local working declarations; backing and canonical union remain separate gates. */
    typedef struct PC_DL_MATERIAL_READER {
        u8 unknown_0000[0x40];
        u32 reflection_flags;
    } PC_DL_MATERIAL_READER;
    STATIC_ASSERT(offsetof(PC_DL_MATERIAL_READER, reflection_flags) == 0x40);
    extern NUVEC4 NuDisplayHandlerTintPc;
    extern void *NuDisplayHandlerMaterialPc;
    extern i32 NuDisplayHandlerReflectionPc;
    extern i32 NuDisplayHandlerCameraIdPc;
    extern PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC PCGraphicsFrameScratchCount;
    void __cdecl NuVec4Add(NUVEC4 *, NUVEC4 *, NUVEC4 *);

    typedef struct PC_DL_CAMERA_PROJECTION_READER {
        NUMTX projection;
        f32 viewport[4];
    } PC_DL_CAMERA_PROJECTION_READER;
    STATIC_ASSERT(offsetof(PC_DL_CAMERA_PROJECTION_READER, viewport) == 0x40);

    typedef struct PC_DL_VERTEX_GROUPS_READER {
        i32 count;
        u32 flags;
        NUVEC4 values[1];
    } PC_DL_VERTEX_GROUPS_READER;
    STATIC_ASSERT(offsetof(PC_DL_VERTEX_GROUPS_READER, count) == 0);
    STATIC_ASSERT(offsetof(PC_DL_VERTEX_GROUPS_READER, flags) == 4);
    STATIC_ASSERT(offsetof(PC_DL_VERTEX_GROUPS_READER, values) == 8);

    typedef struct PC_DL_VERTEX_OFFSETS_READER {
        i32 count;
        NUVEC4 values[1];
    } PC_DL_VERTEX_OFFSETS_READER;
    STATIC_ASSERT(offsetof(PC_DL_VERTEX_OFFSETS_READER, count) == 0);
    STATIC_ASSERT(offsetof(PC_DL_VERTEX_OFFSETS_READER, values) == 4);

    extern PC_DISPLAY_ITEM_HANDLER_PC PCDisplayShadowItemHandlersPc[49];

    /* PC-only table slot 0xad; no Android function identity is asserted. */
    typedef struct PC_DL_AD_COLOURS {
        NUVEC colour[5];
    } PC_DL_AD_COLOURS;
    STATIC_ASSERT(sizeof(PC_DL_AD_COLOURS) == 0x3c);

    typedef struct PC_DL_AD_RECORD {
        u8 unknown_0000[0x14];
        u8 enabled;
        u8 unknown_0015[3];
        i32 segment_count;
        f32 pixel_parameter, vertex_parameter_x, vertex_parameter_y, vertex_parameter_z;
        f32 pixel_parameter_x, pixel_parameter_y, pixel_parameter_z, pixel_scalar;
        PC_DL_AD_COLOURS colours;
    } PC_DL_AD_RECORD;
    STATIC_ASSERT(sizeof(PC_DL_AD_RECORD) == 0x78);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, enabled) == 0x14);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, segment_count) == 0x18);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter) == 0x1c);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, vertex_parameter_x) == 0x20);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, vertex_parameter_y) == 0x24);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, vertex_parameter_z) == 0x28);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter_x) == 0x2c);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter_y) == 0x30);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter_z) == 0x34);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_scalar) == 0x38);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, colours) == 0x3c);

    typedef struct PC_DL_AD_RECORD_LIST {
        u32 unknown_0000;
        i32 count;
        u32 unknown_0008;
        PC_DL_AD_RECORD *records;
    } PC_DL_AD_RECORD_LIST;
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD_LIST, count) == 4);
    STATIC_ASSERT(offsetof(PC_DL_AD_RECORD_LIST, records) == 0xc);

    typedef struct PC_DL_AD_PACKET {
        PC_DL_AD_RECORD_LIST *list;
        u32 instances;
        u8 unknown_0008[0xc];
        PC_BUFFER_RESOURCE_PC *instance_buffer, *vertex_buffer, *index_buffer;
    } PC_DL_AD_PACKET;
    STATIC_ASSERT(sizeof(PC_DL_AD_PACKET) == 0x20);
    STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, list) == 0);
    STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, instances) == 4);
    STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, instance_buffer) == 0x14);
    STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, vertex_buffer) == 0x18);
    STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, index_buffer) == 0x1c);

    void __cdecl PCDeviceSetStreamSourceFreqPc(u32, u32);
    void __cdecl PCDeviceSetStreamSourcePc(u32, PC_BUFFER_RESOURCE_PC *, u32, u32);
    void __cdecl PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE_PC *);
    void __cdecl PCDeviceDrawIndexedPrimitivePc(i32, i32, u32, u32, u32, u32);

    typedef struct PC_DL_GEOMETRY_READER {
        u32 kind, primitive_count;
        u16 vertex_stride;
        u8 unknown_000a[0xa];
        i32 base_vertex;
        u32 vertex_count, start_index;
        PC_BUFFER_RESOURCE_PC *index_buffer, *vertex_buffer;
        u32 dynamic_count;
        void *morph_deltas;
        PC_BUFFER_RESOURCE_PC *dynamic_buffer;
        void *dynamic_vertices;
    } PC_DL_GEOMETRY_READER;
    STATIC_ASSERT(sizeof(PC_DL_GEOMETRY_READER) == 0x38);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, kind) == 0);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, primitive_count) == 4);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, vertex_stride) == 8);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, base_vertex) == 0x14);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, vertex_count) == 0x18);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, start_index) == 0x1c);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, index_buffer) == 0x20);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, vertex_buffer) == 0x24);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, dynamic_count) == 0x28);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, dynamic_buffer) == 0x30);
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, dynamic_vertices) == 0x34);

    typedef struct PC_DL_GEOMETRY_MATERIAL_READER {
        u8 unknown_0000[0x26c];
        u32 lightmap_flags;
    } PC_DL_GEOMETRY_MATERIAL_READER;
    STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_MATERIAL_READER, lightmap_flags) == 0x26c);

    typedef struct PC_DL_FACEON_DRAW_READER {
        u32 unknown_0000, primitive_count;
        PC_BUFFER_RESOURCE_PC *vertex_buffer;
        u32 first_vertex;
    } PC_DL_FACEON_DRAW_READER;
    STATIC_ASSERT(sizeof(PC_DL_FACEON_DRAW_READER) == 0x10);
    STATIC_ASSERT(offsetof(PC_DL_FACEON_DRAW_READER, primitive_count) == 4);
    STATIC_ASSERT(offsetof(PC_DL_FACEON_DRAW_READER, vertex_buffer) == 8);
    STATIC_ASSERT(offsetof(PC_DL_FACEON_DRAW_READER, first_vertex) == 0xc);

    typedef struct PC_DL_DEBRIS_DRAW_READER {
        u8 buffer_index;
        u8 unknown_0001[3];
        u32 use_system_buffer, first_vertex, vertex_count;
    } PC_DL_DEBRIS_DRAW_READER;
    STATIC_ASSERT(sizeof(PC_DL_DEBRIS_DRAW_READER) == 0x10);
    STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, buffer_index) == 0);
    STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, use_system_buffer) == 4);
    STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, first_vertex) == 8);
    STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, vertex_count) == 0xc);

    typedef struct PC_DL_LIGHTMAP_UPDATE_READER {
        i32 mode, texture_id;
        i32 texture_ids[3];
        f32 x_offset, y_offset;
    } PC_DL_LIGHTMAP_UPDATE_READER;
    STATIC_ASSERT(sizeof(PC_DL_LIGHTMAP_UPDATE_READER) == 0x1c);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, mode) == 0);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, texture_id) == 4);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, texture_ids) == 8);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, x_offset) == 0x14);
    STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, y_offset) == 0x18);

    /* Existing canonical integer slot holds the native 32-bit packet identity. */
    extern i32 PCGraphicsBoundShader;
    void __cdecl PCDeviceSetStreamSourcePc(u32, PC_BUFFER_RESOURCE_PC *, u32, u32);
    void __cdecl PCDeviceDrawPrimitivePc(i32, u32, u32);
    void __cdecl PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE_PC *);
    void __cdecl PCDeviceDrawIndexedPrimitivePc(i32, i32, u32, u32, u32, u32);

    extern "C" void __cdecl NuStrFixExt(char *, char *, char *, i32);
    extern const char NuTextureSourcePathPC[];
    extern const char NuTextureDdsExtensionPc[];
    extern const char NuTextureEnvironmentCubePathPc[];
    extern const char NuTexturePhongCubePathPc[];
    extern PC_TEXTURE_RESOURCE_PC *NuTextureEnvironmentCubePc, *NuTexturePhongCubePc;
    void __cdecl PCDeviceCreateCubeTextureFromFile(char *path, PC_TEXTURE_RESOURCE_PC **resource);

    /* The leading pad keeps the 14-byte file header's 32-bit fields naturally aligned. */
    typedef struct nutexbmp_file_header_pc_s {
        u16 pad_00;
        u16 type;
        u32 size;
        u16 reserved_08;
        u16 reserved_0a;
        u32 data_offset;
    } NUTEXBMP_FILE_HEADER_PC;

    typedef struct nutexbmp_info_header_pc_s {
        u32 size;
        i32 width;
        i32 height;
        u16 planes;
        u16 bit_count;
        u32 compression;
        u32 image_size;
        i32 x_resolution;
        i32 y_resolution;
        u32 colours_used;
        u32 colours_important;
    } NUTEXBMP_INFO_HEADER_PC;

    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, original_mip_count) == 0xc);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_10) == 0x10);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, format) == 0x14);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, usage) == 0x18);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, pool) == 0x1c);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, flags) == 0x20);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_24) == 0x24);
    typedef struct PC_TEXTURE_LEVEL_COUNT_METHODS_PC {
        void *reserved00[13];
        u32(__stdcall *get_level_count)(PC_TEXTURE_RESOURCE_PC *);
    } PC_TEXTURE_LEVEL_COUNT_METHODS_PC;
    STATIC_ASSERT(offsetof(PC_TEXTURE_LEVEL_COUNT_METHODS_PC, get_level_count) == 0x34);

    STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) == 8);
    STATIC_ASSERT(sizeof(GRAPHICS_RENDER_TEXTURE_PC) == 0x30);

    extern "C" {
        extern u32 PCGraphicsPostEffectWidthPc;
        extern u32 PCGraphicsPostEffectHeightPc;
        extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
        extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;
    }

    STATIC_ASSERT(sizeof(GRAPHICS_RENDER_TEXTURE_PC) == 0x30);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, mip_count) == 0xa);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, flags) == 0x20);
    STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_24) == 0x24);

    /* Font atlas builder: scans a BMP glyph strip for glyph cells and builds a font around it. */
    extern "C" i32 NuStrLenW(const NUWCHAR *str);

    /* Nonzero when any texel of the 32-bit column at (x, y) has a nonzero alpha. */
    static __forceinline i32 NuQFntColumnOpaque32Inline(NUTEXBMP_PC *bitmap, i32 x, i32 y, i32 count) {
        i32 width = bitmap->width;
        u8 *alpha = bitmap->data + (width * y + x) * 4 + 3;
        i32 i;

        for (i = 0; i < count && y + i < bitmap->height; i++) {
            if (*alpha != 0) {
                return 1;
            }
            alpha += width * 4;
        }
        return 0;
    }

    /* The material fields the atlas font set-up writes. */
    typedef struct nuqfnt_atlas_material_pc_s {
        u8 unknown_000[0x40];
        u32 attributes;
        u32 attributes_high;
        u8 unknown_048[0xc];
        NUVEC diffuse;
        u8 unknown_060[0x10];
        f32 opacity;
        i16 texture_id;
    } NUQFNT_ATLAS_MATERIAL_PC;
    STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, attributes) == 0x40);
    STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, diffuse) == 0x54);
    STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, opacity) == 0x70);
    STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, texture_id) == 0x74);

#define NUQFNT_ATLAS_MAP_SIZE 808
#define NUQFNT_ATLAS_GLYPH_SIZE 750

/* Finds the next glyph cell with the scanner that suits the strip's format. */
#define NUQFNT_ATLAS_NEXT_GLYPH()                                                                                      \
    if (bitmap->type == 5) {                                                                                           \
        NuQFntNextGlyph8Pc(bitmap, cell_height, key_colour, &x, &y, &width);                                           \
    } else {                                                                                                           \
        NuQFntNextGlyph32Pc(bitmap, cell_height, &x, &y, &width);                                                      \
    }

    extern "C" {
        extern PC_EFFECT_SURFACE_PC *PcSurfaceRegistry[256];
        extern u8 PcSurfaceRegistryInitialized;
        extern PC_FRAMEBUFFER_PC PcFramebufferPool[32];
        extern u8 PcFramebufferActive[32];
        extern PC_FRAMEBUFFER_PC *PcBoundFramebuffer;
        extern PC_FRAMEBUFFER_PC *PcDefaultFramebuffer;
        extern PC_FRAMEBUFFER_PC *PcSecondaryFramebuffer;
    }

    extern "C" {
        PC_EFFECT_SURFACE_PC *PcSurfaceRegistry[256] = {NULL};
        u8 PcSurfaceRegistryInitialized = 0;
        PC_FRAMEBUFFER_PC PcFramebufferPool[32];
        u8 PcFramebufferActive[32] = {0};
        PC_FRAMEBUFFER_PC *PcBoundFramebuffer = NULL;
        PC_FRAMEBUFFER_PC *PcDefaultFramebuffer = NULL;
        PC_FRAMEBUFFER_PC *PcSecondaryFramebuffer = NULL;
        PC_BOOT_FRAMEBUFFER_VIEW_PC PcBootFramebuffersPc = {NULL, NULL};
    }

    STATIC_ASSERT(offsetof(NULGTLASER_PC, start) == 0x0);
    STATIC_ASSERT(offsetof(NULGTLASER_PC, end) == 0xc);
    STATIC_ASSERT(offsetof(NULGTLASER_PC, segment_length) == 0x1c);
    STATIC_ASSERT(offsetof(NULGTLASER_PC, width_wobble) == 0x20);
    STATIC_ASSERT(offsetof(NULGTLASER_PC, end_width_ratio) == 0x24);
    STATIC_ASSERT(offsetof(NULGTLASER_PC, length) == 0x28);
    STATIC_ASSERT(offsetof(NULGTLASER_PC, type) == 0x2c);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, start) == 0x0);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, end) == 0xc);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, bend) == 0x18);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, segment_length) == 0x28);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, wobble) == 0x2c);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, bend_amount) == 0x30);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, type) == 0x34);
    STATIC_ASSERT(offsetof(NULGTARCLASER_PC, flags) == 0x3c);
    STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, material) == 0x0);
    STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, u0) == 0x4);
    STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, u1) == 0xc);
    extern NULGTLASER_PC NuLgtLaserData[64];
    extern NULGTARCLASER_PC NuLgtArcLaserData[16];
    extern NULGTARCMATERIAL_PC NuLgtArcMtl[4];
    extern i32 NuLgtLaserCnt, NuLgtLaserOldCnt;
    extern i32 NuLgtArcLaserCnt, NuLgtArcLaserOldCnt, NuLgtArcLaserFrame;
    extern u32 NuLgtSeed;
    extern i32 PS2_REZ_W, PS2_REZ_H;
    extern i32 NuPrimCoordSystemStack[];
    void __cdecl NuVec4Scale(NUVEC4 *, NUVEC4 *, f32);
    void __cdecl NuVec4ScaleForwardPc(NUVEC4 *, NUVEC4 *, f32);
    i32 __cdecl NuRndrBeginSceneEx(i32, i32, i32);
    void __cdecl NuRndrEndSceneEx(i32);
#define NU_LGT_RAND() ((i32)(NuLgtSeed = (NuLgtSeed * 0x24cd + 1) & 0xffff))
#define NU_LGT_ARC_VERTEX(position, colour, tex_u, tex_v)                                                              \
    do {                                                                                                               \
        NuLgtSetVertexColor(colour);                                                                                   \
        vertex = (NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;                                                \
        vertex->u = (tex_u);                                                                                           \
        vertex->v = (tex_v);                                                                                           \
        NuPrim2DAddXYZ((position)->x, (position)->y, (position)->z);                                                   \
    } while (0)

    extern void __cdecl NuKeyboardRead(void);
    extern void __cdecl PCFrameCriticalSectionEnter_candidate(void);
    extern void __cdecl PCFrameCriticalSectionLeave_candidate(void);

    extern i32 FrameSleepMillisecondsPc;
    extern f32 FrameStartupDelayPc;
    extern i32 NuApiTextLinePc;
    extern f32 NuFrameAccumulatedTimePc;
    extern const char *__cdecl PcFrameDelayMessagePc(void);
    extern u32 __cdecl PcGetRefreshRatePc(void);
    extern void __cdecl bgprocFrameSleepPc(i32 milliseconds);
    extern void __cdecl NuFrameEndLeafPc(void);
    extern void __cdecl FmvPlatformUpdatePc(void);

#define NUFRAME_ELAPSED(elapsed, now, start)                                                                           \
    if ((now).low >= (start).low) {                                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high = (now).high - (start).high;                                                                    \
    } else {                                                                                                           \
        (elapsed).high = (now).high - (start).high;                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high -= 1;                                                                                           \
    }

    extern void(__cdecl *NuApiEndFrameCallbackPc)(void);

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
    extern RENDER_POST_VIEWPORT_PREFIX_PC RenderPostViewportPrefixPc;
    void __cdecl RenderPostViewportSetPc(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x,
                                         f32 dest_y, f32 dest_right, f32 dest_bottom);
    void __cdecl RenderPostViewportResetPc(void);

    STATIC_ASSERT(offsetof(NUVPCURRENT, x) == 0);
    STATIC_ASSERT(offsetof(NUVPCURRENT, y) == 4);
    STATIC_ASSERT(offsetof(NUVPCURRENT, width) == 8);
    STATIC_ASSERT(offsetof(NUVPCURRENT, height) == 0xc);

    typedef struct screengrab_tile_params_pc_s SCREENGRAB_TILE_PARAMS_PC;
    PC_BUFFER_RESOURCE_PC *NuWaterVertexBufferPc = 0;

    extern i32 NuPrimCSPos;
    extern i32 NuPrimCoordSystemStack[];
    extern i32 NuPrimVertexCount;
    extern i32 *NuPrimVertexCountPointer;
    extern VARIPTR *NuPrimStreamBufferPointer;
    extern i32 NuPrimCurrentPrimitiveType;
    extern f32 NuPrimXScale;
    extern f32 NuPrimYScale;
    extern f32 NuPrimXBias;
    extern f32 NuPrimYBias;

    extern const f32 NuPrimPS2XScale;
    extern const f32 NuPrimPS2YScale;
    extern const f32 NuPrimTwo;
    extern const f32 NuPrimMinusTwo;

    typedef struct nuprimvertex_pc_s {
        f32 x;
        f32 y;
        f32 z;
        u32 colour;
        f32 u;
        f32 v;
    } NUPRIMVERTEX_PC;
    STATIC_ASSERT(sizeof(NUPRIMVERTEX_PC) == 0x18);
    STATIC_ASSERT(offsetof(NUPRIMVERTEX_PC, colour) == 0x0c);
    STATIC_ASSERT(offsetof(NUPRIMVERTEX_PC, u) == 0x10);
    STATIC_ASSERT(offsetof(NUPRIMVERTEX_PC, v) == 0x14);

    typedef struct nuprim_packet_pc_s {
        u32 primitive_type;
        u8 unknown_0004[0x14];
        i32 vertex_count;
        u8 unknown_001c[0x1c];
    } NUPRIM_PACKET_PC;
    STATIC_ASSERT(sizeof(NUPRIM_PACKET_PC) == 0x38);
    STATIC_ASSERT(offsetof(NUPRIM_PACKET_PC, vertex_count) == 0x18);

    typedef struct nuprim_command_pc_s {
        u8 type;
        u8 id;
        u16 unknown_0002;
        void *target;
        u32 unknown_0008[2];
    } NUPRIM_COMMAND_PC;
    STATIC_ASSERT(sizeof(NUPRIM_COMMAND_PC) == 0x10);
    STATIC_ASSERT(offsetof(NUPRIM_COMMAND_PC, target) == 4);

    typedef struct nuprim_material_state_pc_s {
        u32 unknown_0000[2];
        NUMTL *material;
        u32 unknown_000c;
        u32 unknown_0010;
        i32 texture_id;
    } NUPRIM_MATERIAL_STATE_PC;
    STATIC_ASSERT(offsetof(NUPRIM_MATERIAL_STATE_PC, material) == 0x08);
    STATIC_ASSERT(offsetof(NUPRIM_MATERIAL_STATE_PC, texture_id) == 0x14);

    typedef struct nuprim_transform_packet_pc_s {
        u8 unknown_0000[0x40];
    } NUPRIM_TRANSFORM_PACKET_PC;
    STATIC_ASSERT(sizeof(NUPRIM_TRANSFORM_PACKET_PC) == 0x40);

    typedef struct nuprim_render_state_ids_pc_s {
        u8 unknown_0000[0x1e];
        i16 konst_id;
        i32 global_id;
        i16 lights_id;
        u16 unknown_0026[3];
        i16 camera_id;
        i16 fog_id;
        i16 vertex_groups_id;
        i16 reflection_id;
    } NUPRIM_RENDER_STATE_IDS_PC;
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, konst_id) == 0x1e);
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, global_id) == 0x20);
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, lights_id) == 0x24);
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, camera_id) == 0x2c);
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, fog_id) == 0x2e);
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, vertex_groups_id) == 0x30);
    STATIC_ASSERT(offsetof(NUPRIM_RENDER_STATE_IDS_PC, reflection_id) == 0x32);

    typedef struct nuprim_camera_packet_pc_s {
        i32 id;
        NUMTX view;
        NUMTX projection;
        f32 viewport[4];
    } NUPRIM_CAMERA_PACKET_PC;
    STATIC_ASSERT(sizeof(NUPRIM_CAMERA_PACKET_PC) == 0x94);
    STATIC_ASSERT(offsetof(NUPRIM_CAMERA_PACKET_PC, view) == 4);
    STATIC_ASSERT(offsetof(NUPRIM_CAMERA_PACKET_PC, projection) == 0x44);
    STATIC_ASSERT(offsetof(NUPRIM_CAMERA_PACKET_PC, viewport) == 0x84);

    typedef struct nuprim_vec4_pc_s {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    } NUPRIM_VEC4_PC;
    STATIC_ASSERT(sizeof(NUPRIM_VEC4_PC) == 0x10);

    typedef struct nuprim_light_packet_pc_s {
        NUPRIM_VEC4_PC ambient_intensity;
        NUPRIM_VEC4_PC light_intensity[3];
        NUPRIM_VEC4_PC light_direction[3];
        NUMTX specular_mtx;
        NUVEC specular_colour;
    } NUPRIM_LIGHT_PACKET_PC;
    STATIC_ASSERT(sizeof(NUPRIM_LIGHT_PACKET_PC) == 0xbc);
    STATIC_ASSERT(offsetof(NUPRIM_LIGHT_PACKET_PC, light_intensity) == 0x10);
    STATIC_ASSERT(offsetof(NUPRIM_LIGHT_PACKET_PC, light_direction) == 0x40);
    STATIC_ASSERT(offsetof(NUPRIM_LIGHT_PACKET_PC, specular_mtx) == 0x70);
    STATIC_ASSERT(offsetof(NUPRIM_LIGHT_PACKET_PC, specular_colour) == 0xb0);
#undef LINK_MTL_STATE

    /* Native selects one of the two adjacent material-use buffers by bit 15. */
    typedef struct NUPRIM_SCENE_USAGE_PC {
        u8 unknown_0000[0x58];
        u8 *material_used[2];
    } NUPRIM_SCENE_USAGE_PC;
    STATIC_ASSERT(offsetof(NUPRIM_SCENE_USAGE_PC, material_used) == 0x58);
    STATIC_ASSERT(offsetof(NUDISPLAYSCENE_PC, material_used_back) ==
                  offsetof(NUPRIM_SCENE_USAGE_PC, material_used) + sizeof(u8 *));

    typedef struct AMBIENT_SPECULAR_RENDER_VIEW_PC {
        u8 unknown_00[0x8c];
        f32 global_specular;
    } AMBIENT_SPECULAR_RENDER_VIEW_PC;
    STATIC_ASSERT(offsetof(AMBIENT_SPECULAR_RENDER_VIEW_PC, global_specular) == 0x8c);
    // @@FUNCTIONS

    /* The PC backend's transform-update leaf is a single ret instruction; retail
     * reaches it through a jump thunk, so callers in other units never inline it. */
    void __cdecl DisplayListUpdateSpecialTransformPS(NUSPECIALHANDLE *special, NUMTX *matrix) {
        (void)special;
        (void)matrix;
    }

    static __declspec(noinline) void PCSpecialTransposeMatrix(NUMTX *out, NUMTX *matrix) {
        f32 saved;
        out->m00 = matrix->m00;
        saved = matrix->m01;
        out->m01 = matrix->m10;
        out->m10 = saved;
        saved = matrix->m02;
        out->m02 = matrix->m20;
        out->m20 = saved;
        saved = matrix->m03;
        out->m03 = matrix->m30;
        out->m30 = saved;
        out->m11 = matrix->m11;
        saved = matrix->m12;
        out->m12 = matrix->m21;
        out->m21 = saved;
        saved = matrix->m13;
        out->m13 = matrix->m31;
        out->m31 = saved;
        out->m22 = matrix->m22;
        saved = matrix->m23;
        out->m23 = matrix->m32;
        out->m32 = saved;
        out->m33 = matrix->m33;
    }

    static __declspec(noinline) void NuLgtSetVertexColor(i32 colour) {
        i32 converted;
        if (NuPrimNeedsOverbrightening)
            converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
        else
            converted =
                ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
    }

    static i32 PCGraphicsFormatToD3d(i32 format) {
        switch (format) {
            case 2:
                return 21;
            case 1:
                return 22;
            case 4:
                return 75;
            case 5:
                return 83;
            case 10:
                return 114;
            default:
                return -1;
        }
    }

    static i32 PCGraphicsFormatFromD3d(i32 format) {
        switch (format) {
            case 21:
            case 22:
                return 2;
            case 75:
                return 4;
            case 83:
                return 5;
            default:
                return 0;
        }
    }

    void __cdecl NuEffectTexLockVP(VARIPTR *buffer, VARIPTR end) {
        (void)buffer;
        (void)end;
    }

    i32 __cdecl NuEffectTexUnlockVP(void) {
        return 0;
    }

    void __cdecl PCGraphicsDestroyRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture) {
        i32 i;
        i32 memory_delta;
        f32 multiplier;
        if (texture == NULL)
            return;
        for (i = 0; i != 32; ++i) {
            if (&PCGraphicsRenderTextures[i] == texture) {
                if (texture->external_id == -1) {
                    if (PCGraphicsRenderTextures[i].resource != NULL) {
                        PCGraphicsRenderTextures[i].resource->methods->release(PCGraphicsRenderTextures[i].resource);
                        PCGraphicsRenderTextures[i].resource = NULL;
                    }
                    if (PCGraphicsRenderTextures[i].surface != NULL) {
                        PCGraphicsRenderTextures[i].surface->methods->release(PCGraphicsRenderTextures[i].surface);
                        PCGraphicsRenderTextures[i].surface = NULL;
                    }
                }
                multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
                memory_delta = (i32)((0u - (u32)texture->width * (u32)texture->height) * 4u);
                PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
                memset(&PCGraphicsRenderTextures[i], 0, sizeof(GRAPHICS_RENDER_TEXTURE_PC));
                PCGraphicsRenderTextureActive[i] = 0;
                return;
            }
        }
    }

    i32 __cdecl PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE_PC *texture) {
        return texture->width;
    }

    i32 __cdecl PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE_PC *texture) {
        return texture->height;
    }

    void __cdecl PCGraphicsEffectTextureDimensions(GRAPHICS_RENDER_TEXTURE_PC *texture, u16 level, i32 *width,
                                                   i32 *height) {
        *width = (texture->width >> level) & ~1;
        *height = (texture->height >> level) & ~1;
    }

    i32 __cdecl PCGraphicsEffectTextureMipCount(GRAPHICS_RENDER_TEXTURE_PC *texture) {
        return texture->mip_count;
    }

    void __cdecl PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 low, i32 high) {
        texture->flags = (texture->flags & ~0xffu) | (((u32)high & 0xfu) << 4) | ((u32)low & 0xfu);
    }

    void __cdecl PCGraphicsEffectTextureSetField24(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 value) {
        texture->field_24 = (u32)value;
    }

    /* Sampler fields packed into GRAPHICS_RENDER_TEXTURE_PC::flags. */
    typedef union PC_SAMPLER_FLAGS_PC {
        u32 value;
        struct {
            u32 unknown_0 : 8;
            u32 address_u : 3;
            u32 address_v : 3;
            u32 unknown_14 : 3;
            u32 min_filter : 3;
            u32 mag_filter : 3;
            u32 mip_filter : 3;
            u32 unknown_26 : 6;
        } bits;
    } PC_SAMPLER_FLAGS_PC;
    STATIC_ASSERT(sizeof(PC_SAMPLER_FLAGS_PC) == 4);

    void __cdecl PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 first, i32 second,
                                                        i32 third) {
        i32 value;
        switch (first) {
            case 0:
                value = 0;
                break;
            case 1:
                value = 1;
                break;
            case 2:
                value = 2;
                break;
            case 3:
                value = 3;
                break;
            default:
                value = -1;
                break;
        }
        ((PC_SAMPLER_FLAGS_PC *)&texture->flags)->bits.min_filter = value;
        switch (second) {
            case 0:
                value = 0;
                break;
            case 1:
                value = 1;
                break;
            case 2:
                value = 2;
                break;
            case 3:
                value = 3;
                break;
            default:
                value = -1;
                break;
        }
        ((PC_SAMPLER_FLAGS_PC *)&texture->flags)->bits.mag_filter = value;
        switch (third) {
            case 0:
                value = 0;
                break;
            case 1:
                value = 1;
                break;
            case 2:
                value = 2;
                break;
            case 3:
                value = 3;
                break;
            default:
                value = -1;
                break;
        }
        ((PC_SAMPLER_FLAGS_PC *)&texture->flags)->bits.mip_filter = value;
    }

    void __cdecl PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 first, i32 second) {
        i32 value;
        switch (first) {
            case 0:
                value = 1;
                break;
            case 1:
                value = 2;
                break;
            case 2:
                value = 3;
                break;
            case 3:
                value = 4;
                break;
            default:
                value = -1;
                break;
        }
        ((PC_SAMPLER_FLAGS_PC *)&texture->flags)->bits.address_u = value;
        switch (second) {
            case 0:
                value = 1;
                break;
            case 1:
                value = 2;
                break;
            case 2:
                value = 3;
                break;
            case 3:
                value = 4;
                break;
            default:
                value = -1;
                break;
        }
        ((PC_SAMPLER_FLAGS_PC *)&texture->flags)->bits.address_v = value;
    }

    void __cdecl PCGraphicsResetRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture) {
        i32 memory_delta;
        f32 multiplier;
        multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
        memory_delta = (i32)((0u - (u32)texture->width * (u32)texture->height) * 4u);
        PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
        if (texture->external_id == -1 && texture->resource != NULL) {
            texture->resource->methods->release(texture->resource);
            texture->resource = NULL;
        }
        texture->width = 0;
        texture->height = 0;
        texture->unknown_08 = 0;
        texture->mip_count = 0;
        texture->original_mip_count = 0;
        texture->field_10 = 1;
        texture->format = 0;
        texture->usage = 0;
        texture->pool = 0;
        texture->resource = NULL;
        texture->external_id = -1;
        texture->field_24 = 0;
        texture->flags = 0x012449d0;
        if (texture->surface != NULL) {
            texture->surface->methods->release(texture->surface);
            texture->surface = NULL;
        }
    }

    /* Retail calls this from PCGraphicsCreatePostEffectTextures rather than inlining it. */
    __declspec(noinline) void __cdecl PCGraphicsInitRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 width,
                                                                  i32 height, i32 levels, i32 usage, i32 pool,
                                                                  i32 format) {
        u32 d3d_usage;
        i32 memory_delta;
        f32 multiplier;
        texture->width = (u16)width;
        texture->height = (u16)height;
        texture->mip_count = (u16)levels;
        texture->original_mip_count = (u16)levels;
        texture->unknown_08 = 0;
        texture->field_10 = 1;
        texture->format = format;
        texture->usage = usage;
        texture->pool = pool;
        texture->external_id = -1;
        texture->surface = NULL;
        texture->field_24 = 0;
        texture->flags = 0x012449d0;
        if (format == 4) {
            texture->pool = 0;
            texture->mip_count = 1;
            PCDeviceCreateDepthSurface(
                texture->width, texture->height, PCGraphicsFormatToD3d(format), PCDeviceBackend.GetMultisampleType(),
                PCDeviceBackend.quality[PCDeviceBackend.multisample_index], FALSE, &texture->surface, NULL);
            PCGraphicsRenderTextureBytes += (i32)((u32)texture->width * texture->height * 4u);
            return;
        }
        d3d_usage = 0;
        if (texture->usage == 1)
            d3d_usage = format == 5 || format == 6 || format == 7 ? 2 : 1;
        PCDeviceCreateTexture(texture->width, texture->height, texture->mip_count, d3d_usage,
                              PCGraphicsFormatToD3d(format), texture->pool, &texture->resource);
        if (texture->resource != NULL) {
            texture->mip_count = (u16)((const PC_TEXTURE_LEVEL_COUNT_METHODS_PC *)texture->resource->methods)
                                     ->get_level_count(texture->resource);
            multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
            memory_delta = (i32)((u32)texture->width * texture->height * 4u);
            PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
        }
    }

    u32 *__cdecl PCGraphicsEffectTextureSamplerPc(GRAPHICS_RENDER_TEXTURE_PC *texture) {
        return &texture->flags;
    }

    /* Retail calls this from NuDeferredFilter::ResizeTexturesPc instead of inlining it: its x87
     * byte-count conversion (an inline fnstcw/fistp for the unsigned cast) keeps it above VC8's inline limit. */
    __declspec(noinline) void __cdecl PCGraphicsResizeEffectTexture(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 width,
                                                                    i32 height, i32 levels) {
        if (texture == NULL)
            return;
        f32 multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
        i32 memory_delta = (i32)((0u - (u32)texture->width * texture->height) * 4u);
        PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
        if (texture->external_id == -1 && texture->resource != NULL) {
            texture->resource->methods->release(texture->resource);
            texture->resource = NULL;
        }
        if (texture->surface != NULL) {
            texture->surface->methods->release(texture->surface);
            texture->surface = NULL;
        }
        PCGraphicsInitRenderTexture(texture, width > 0 ? width : texture->width, height > 0 ? height : texture->height,
                                    levels > 0 ? levels : texture->original_mip_count, texture->usage, texture->pool,
                                    texture->format);
    }

    void __cdecl PCGraphicsBindEffectTexture(GRAPHICS_RENDER_TEXTURE_PC *texture, u32 stage) {
        PCDeviceSetTexture(stage, texture != NULL ? texture->resource : NULL);
    }

    void __cdecl PCGraphicsReleaseEffectTextures(void) {
        for (i32 index = 0; index != 32; ++index) {
            if (PCGraphicsRenderTextureActive[index] && PCGraphicsRenderTextures[index].external_id == -1) {
                if (PCGraphicsRenderTextures[index].resource != NULL) {
                    PCGraphicsRenderTextures[index].resource->methods->release(
                        PCGraphicsRenderTextures[index].resource);
                    PCGraphicsRenderTextures[index].resource = NULL;
                }
                if (PCGraphicsRenderTextures[index].surface != NULL) {
                    PCGraphicsRenderTextures[index].surface->methods->release(PCGraphicsRenderTextures[index].surface);
                    PCGraphicsRenderTextures[index].surface = NULL;
                }
                f32 multiplier = PCGraphicsRenderTextures[index].mip_count != 1 ? 1.5f : 1.0f;
                i32 memory_delta =
                    (i32)((0u - (u32)PCGraphicsRenderTextures[index].width * PCGraphicsRenderTextures[index].height) *
                          4u);
                PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
            }
        }
    }

    void __cdecl PCGraphicsRestoreEffectTextures(i32 unused) {
        (void)unused;
        for (i32 index = 0; index != 32; ++index) {
            GRAPHICS_RENDER_TEXTURE_PC *texture = &PCGraphicsRenderTextures[index];
            if (PCGraphicsRenderTextureActive[index]) {
                switch (texture->field_10) {
                    case 1:
                        if (texture->external_id == -1 && texture->resource == NULL && texture->surface == NULL)
                            PCGraphicsInitRenderTexture(texture, texture->width, texture->height, texture->mip_count,
                                                        texture->usage, texture->pool, texture->format);
                        break;
                }
            }
        }
    }

    /* Two additional genuine original leaves, proposed in existing effect_surface.cpp.
     * Complete native entries are6B and1B; neither replaces the held copy operation. */
    PC_FRAMEBUFFER_PC *__cdecl PCGraphicsGetBoundFramebufferPc(void) {
        return PcBoundFramebuffer;
    }

    PC_FRAMEBUFFER_PC *__cdecl NuFramebufferGetObject(i32 index) {
        return PcBootFramebuffersPc.slots[index];
    }

    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsGetFramebufferAttachmentPc(PC_FRAMEBUFFER_PC *framebuffer, i32 index,
                                                                             i32 *level, i32 *field) {
        PC_EFFECT_SURFACE_PC *target = &framebuffer->targets[index];
        GRAPHICS_RENDER_TEXTURE_PC *texture = target->texture;
        if (texture == NULL) {
            if (level != NULL)
                *level = 0;
            if (field != NULL)
                *field = 0;
            return NULL;
        }
        if (level != NULL)
            *level = target->level;
        /* Retail tests field but writes through level, including when level is null. */
        if (field != NULL)
            *level = target->field_10;
        return texture;
    }

    i32 __cdecl NuFramebufferGetWidth(PC_FRAMEBUFFER_PC *framebuffer) {
        if (framebuffer == PcDefaultFramebuffer || framebuffer == PcSecondaryFramebuffer)
            return PCDeviceBackend.parameters.width;
        return framebuffer != NULL ? framebuffer->targets[0].width : 0;
    }

    i32 __cdecl NuFramebufferGetHeight(PC_FRAMEBUFFER_PC *framebuffer) {
        if (framebuffer == PcDefaultFramebuffer || framebuffer == PcSecondaryFramebuffer)
            return PCDeviceBackend.parameters.height;
        return framebuffer != NULL ? framebuffer->targets[0].height : 0;
    }

    void __cdecl PCGraphicsFramebufferBarrierPc(i32, i32) {
    }

    void __cdecl PCGraphicsFramebufferActionPc(i32) {
    }

    void __cdecl PCGraphicsEnableFramebufferGuardsPc(PC_FRAMEBUFFER_PC *frame, i32 enabled) {
        (void)frame;
        (void)enabled;
    }

    void __cdecl PCGraphicsBindFramebuffer(PC_FRAMEBUFFER_PC *frame) {
        if (PcBoundFramebuffer == frame && (frame == NULL || frame->changed == 0))
            return;
        if (frame == PcDefaultFramebuffer)
            PCDeviceBackend.ConfigureCurrentTargets();
        else if (frame == PcSecondaryFramebuffer)
            PCDeviceBackend.ResetTargets();
        else if (frame != NULL) {
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[0], 0);
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[1], 1);
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[2], 2);
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[3], 3);
            PCDeviceBackend.SetDepthStencilSurface(&frame->targets[4]);
        }
        PcBoundFramebuffer = frame;
        if (frame != NULL)
            frame->changed = 1;
    }

    void __cdecl PCGraphicsBindDefaultFramebuffer(void) {
        PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
    }

    void __cdecl PCGraphicsClearFramebuffer(u32 mask, i32 colour) {
        u32 flags = 0;
        if (PcBoundFramebuffer->targets[0].texture != NULL && (mask & 0x100))
            flags = 1;
        if (PcBoundFramebuffer->targets[4].texture != NULL && (mask & 0x200))
            flags |= 6;
        if (flags != 0) {
            i32 converted = (((colour >> 24) * 256 + (colour & 0xff)) * 256 + ((colour >> 8) & 0xff)) * 256 +
                            ((colour >> 16) & 0xff);
            PCDeviceClear(0, NULL, flags, converted, 1.0f, 0);
        }
    }

    PC_EFFECT_SURFACE_PC *__cdecl PCGraphicsFramebufferTarget(PC_FRAMEBUFFER_PC *frame, i32 index) {
        return &frame->targets[index];
    }

    i32 __cdecl NuTexReadPc(char *name, VARIPTR *buffer, VARIPTR end) {
        char filename[1024];
        NUFILE file;
        i32 size, id;
        void *data;
        NUTEXDIM_PC *texture;
        (void)end;
        NuStrFixExt(filename, name, (char *)NuTextureDdsExtensionPc, sizeof(filename));
        file = NuFileOpen(filename, 0);
        if (file == 0)
            return 0;
        size = (i32)NuFileOpenSize(file);
        data = NuAlloc(size, NuTextureSourcePathPC, 0x2f);
        NuFileRead(file, data, size);
        NuFileClose(file);
        texture = (NUTEXDIM_PC *)buffer->addr;
        buffer->addr += sizeof(NUTEXDIM_PC);
        memset(texture, 0, sizeof(*texture));
        texture->image_data = data;
        texture->image_size = size;
        id = NuTexCreateNative(texture);
        NuFree(data, NuTextureSourcePathPC, 0x4d);
        return id;
    }

    void __cdecl NuTexCreatePS(NUTEXDIM_PC *texture) {
        PC_TEXTURE_LEVEL_DESC_PC description;
        if (texture != NULL && texture->image_data != NULL && texture->image_size != 0) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceCreateTextureFromMemory(texture->image_data, texture->image_size, &texture->resource);
            texture->resource->methods->get_level_desc(texture->resource, 0, &description);
            PCFrameCriticalSectionLeave_candidate();
            texture->width = (i32)description.width;
            texture->height = (i32)description.height;
            texture->image_data = NULL;
            texture->image_size = 0;
        }
    }

    void __cdecl NuTexDestroyPS(NUTEXDIM_PC *texture) {
        if (texture != NULL) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&texture->resource);
            PCFrameCriticalSectionLeave_candidate();
        }
    }

    i32 __cdecl NuTexGetReqSize(i32, i32) {
        return 0;
    }

    i32 __cdecl NuTexReserve(i32) {
        return -1;
    }

    void __cdecl NuTexUnReserve(void) {
    }

    void __cdecl PCGraphicsBindTextureDimensionsPc(NUTEXDIM_PC *texture, u32 stage) {
        PCDeviceSetTexture(stage, texture->resource);
    }

    void __cdecl NuTexInitExPS(VARIPTR *buffer) {
        (void)buffer;
        PCDeviceCreateCubeTextureFromFile((char *)NuTextureEnvironmentCubePathPc, &NuTextureEnvironmentCubePc);
        PCDeviceCreateCubeTextureFromFile((char *)NuTexturePhongCubePathPc, &NuTexturePhongCubePc);
    }

    /* Native PC entries 0x005f8fb0 and 0x005f8fc0 are RET-only; the D3D release and
     * restore paths call them for textures. Their historical names are unknown. */
    void __cdecl NuTexReleaseResourcesPc(void) {
    }

    void __cdecl NuTexRestoreResourcesPc(void) {
    }

    /* Creates, once, the placeholder texture used for references that fail to resolve. */
    i32 NuTexGetUnresolvedTextureTIDPS() {
        if (NuUnresolvedTextureIDPC == -1) {
            NUTEXDIM_PC *texture = (NUTEXDIM_PC *)NuAlloc(sizeof(NUTEXDIM_PC), NuTextureSourcePathPC, 0xd9);
            memset(texture, 0, sizeof(*texture));
            texture->image_data = NuUnresolvedTextureDdsPc;
            texture->image_size = sizeof(NuUnresolvedTextureDdsPc);
            NuUnresolvedTextureIDPC = NuTexCreateNative(texture);
        }
        return NuUnresolvedTextureIDPC;
    }

    /* Bits per texel of a source image type. */
    i32 __cdecl NuTexPixelBitsPc(i32 type) {
        switch (type) {
            case 0:
            case 1:
                return 16;
            case 2:
                return 24;
            case 3:
                return 32;
            case 5:
            case 20:
                return 8;
            case 4:
                return 4;
            default:
                return 0;
        }
    }

    i32 __cdecl NuTexImageSizePc(i32 type, i32 width, i32 height) {
        return NuTexPixelBitsPc(type) * width * height / 8;
    }

    i32 __cdecl NuTexImageSizeMipsPc(i32 type, i32 width, i32 height, i32 mips) {
        i32 bits = NuTexPixelBitsPc(type);
        i32 size = (bits * width * height) >> 3;
        i32 level;

        for (level = 0; level < mips;) {
            level++;
            size += ((width >> level) * (height >> level) * bits) >> 3;
        }
        return size;
    }

    /* Loads an uncompressed BMP into a bitmap record, flipping it top to bottom. */
    i32 __cdecl NuTexLoadBmpPc(const char *path, i32 mips, NUTEXBMP_PC *bitmap) {
        NUFILE file;
        NUTEXBMP_FILE_HEADER_PC file_header;
        NUTEXBMP_INFO_HEADER_PC info_header;
        i32 palette_size;
        i32 width, height, levels;
        i32 row_size, size;
        u8 *row;
        u8 *top;
        u8 *bottom;
        NUTEXBMP_COLOUR_PC swapped;
        u8 blue;
        i32 i;

        file = NuFileOpen(path, 0);
        if (file != 0) {
            if (bitmap != NULL) {
                memset(bitmap, 0, sizeof(*bitmap));
                NuFilePos(file);
                NuFileRead(file, &file_header.type, 14);
                NuFileRead(file, &info_header, sizeof(info_header));
                switch (info_header.bit_count) {
                    case 32:
                        bitmap->type = 3;
                        palette_size = 0;
                        break;
                    case 24:
                        bitmap->type = 2;
                        palette_size = 0;
                        break;
                    case 4:
                        bitmap->type = 4;
                        palette_size = 0x40;
                        NuFileRead(file, NuTexBmpPalettePc, 0x40);
                        for (i = 0; i < 0x10; i++) {
                            swapped = NuTexBmpPalettePc[i];
                            blue = swapped.blue;
                            swapped.blue = swapped.red;
                            swapped.red = blue;
                            NuTexBmpPalettePc[i] = swapped;
                        }
                        break;
                    case 8:
                        bitmap->type = 5;
                        palette_size = 0x400;
                        NuFileRead(file, NuTexBmpPalettePc, 0x400);
                        for (i = 0; i < 0x100; i++) {
                            swapped = NuTexBmpPalettePc[i];
                            blue = swapped.blue;
                            swapped.blue = swapped.red;
                            swapped.red = blue;
                            NuTexBmpPalettePc[i] = swapped;
                        }
                        break;
                    default:
                        NuTexBmpUnsupportedDepthPc(info_header.bit_count, path);
                        break;
                }
                bitmap->width = width = info_header.width;
                bitmap->height = height = info_header.height;
                bitmap->mips = mips;
                row_size = (NuTexPixelBitsPc(bitmap->type) * width + 7) / 8;
                size = 0;
                levels = mips + 1;
                do {
                    size += NuTexPixelBitsPc(bitmap->type) * height * width / 8;
                    width >>= 1;
                    height >>= 1;
                } while (--levels > 0);
                bitmap->data = (u8 *)NuAlloc(size, NuTextureSourcePathPC, 0x195);
                if (palette_size != 0) {
                    bitmap->palette = (u8 *)NuAlloc(palette_size, NuTextureSourcePathPC, 0x198);
                } else {
                    bitmap->palette = NULL;
                }
                _flushall();
                NuFileSeek(file, file_header.data_offset, 0);
                NuFileRead(file, bitmap->data, NuTexImageSizePc(bitmap->type, bitmap->height, bitmap->width));
                row = (u8 *)NuAlloc(row_size, NuTextureSourcePathPC, 0x1a0);
                top = bitmap->data;
                bottom = top + NuTexImageSizePc(bitmap->type, bitmap->height, bitmap->width) - row_size;
                while (bottom > top) {
                    memcpy(row, top, row_size);
                    memcpy(top, bottom, row_size);
                    memcpy(bottom, row, row_size);
                    top += row_size;
                    bottom -= row_size;
                }
                NuFree(row, NuTextureSourcePathPC, 0x1af);
                if (palette_size != 0) {
                    memcpy(bitmap->palette, NuTexBmpPalettePc, palette_size);
                }
            }
            NuFileClose(file);
            return 1;
        }
        sprintf(NuTexBmpErrorTextPc, "Cannot open file: %s", path);
        return 0;
    }

    NUTEXBMP_PC *__cdecl NuTexCreateBmpPc(const char *path) {
        NUTEXBMP_PC *bitmap = (NUTEXBMP_PC *)NuAlloc(sizeof(NUTEXBMP_PC), NuTextureSourcePathPC, 0x1e1);
        if (!NuTexLoadBmpPc(path, 0, bitmap)) {
            NuFree(bitmap, NuTextureSourcePathPC, 0x1e5);
            return NULL;
        }
        return bitmap;
    }

    void __cdecl NuWaterClearPc(void) {
        nu_water_flow_state = 0;
        nu_water_reset_status = 0;
    }

    __declspec(noinline) void __cdecl NuWaterReset(void) {
        nu_water_reset_state = 1;
        nu_water_reset_status = 0;
    }

    /* Releases the water vertex buffer before the D3D device is reset or destroyed. */
    void __cdecl NuWaterReleaseVertexBufferPc(void) {
        if (NuWaterVertexBufferPc != NULL) {
            NuWaterVertexBufferPc->methods->release(NuWaterVertexBufferPc);
            NuWaterVertexBufferPc = NULL;
        }
    }

    void __cdecl NuWaterSpeed(f32 speed) {
        NuShaderObjectBaseSetWaterSpeedPc(speed);
    }

    void __cdecl NuRndrShadowVolumeFinishPc(void) {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        PCDeviceSetRenderState(27, 1);
        PCDeviceSetRenderState(14, 0);
        PCDeviceSetRenderState(137, 0);
        PCDeviceSetRenderState(29, 0);
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTextureStageState(0, 1, 2);
        PCDeviceSetTextureStageState(0, 2, 0);
        PCDeviceSetTextureStageState(0, 4, 2);
        PCDeviceSetTextureStageState(0, 5, 0);
        PCDeviceSetTextureStageState(1, 1, 1);
        PCDeviceSetTextureStageState(1, 4, 1);
        PCDeviceSetRenderState(56, RendererStencilFuncPc);
        PCDeviceSetRenderState(55, RendererStencilPassPc);
        PCDeviceSetRenderState(57, RendererStencilRefPc);
        PCDeviceSetRenderState(7, 0);
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(171, 1);
        PCDeviceSetRenderState(19, 5);
        PCDeviceSetRenderState(20, 6);
        PCDeviceSetVertexDeclarationPc(PCDeviceBackend.declaration6ac);
        PCDeviceSetStreamSourcePc(0, RendererQuadVertexBufferPc, 0, 0x1c);
        PCDeviceDrawPrimitivePc(6, 0, 2);
    }

    void __cdecl NuRndrShadowVolumeDrawPc(void) {
        i32 i;
        u32 n;
        PCDeviceSetVertexDeclarationPc(RendererVertexDeclarationPc);
        PCDeviceSetRenderState(14, 0);
        PCDeviceSetRenderState(27, 1);
        PCDeviceSetRenderState(19, 1);
        PCDeviceSetRenderState(20, 2);
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(54, 1);
        PCDeviceSetRenderState(52, 1);
        PCDeviceSetRenderState(56, 8);
        PCDeviceSetRenderState(55, 4);
        PCDeviceSetRenderState(57, 1);
        PCDeviceSetRenderState(22, 3);
        for (i = 0; i != RendererArenaVertexCountPc; i += 4) {
            PCDeviceDrawPrimitiveUPPc(6, 2, &RendererArenaDataPc[i], 0x10);
        }
        for (n = 0; n < RendererArenaCountPc; n++) {
            PCDeviceDrawPrimitiveUPPc(6, RendererArenaTypesPc[n] - 2, &RendererArenaDataPc[RendererArenaOffsetsPc[n]],
                                      0x40);
        }
        PCDeviceSetRenderState(22, 2);
        PCDeviceSetRenderState(55, 5);
        for (i = 0; i != RendererArenaVertexCountPc; i += 4) {
            PCDeviceDrawPrimitiveUPPc(6, 2, &RendererArenaDataPc[i], 0x10);
        }
        for (n = 0; n < RendererArenaCountPc; n++) {
            PCDeviceDrawPrimitiveUPPc(6, RendererArenaTypesPc[n] - 2, &RendererArenaDataPc[RendererArenaOffsetsPc[n]],
                                      0x40);
        }
    }

    /* Renders the queued hierarchy shadow packets as stencil shadow volumes; retail never calls it. */
    void __cdecl NuRndrShadowVolumesPc(void) {
        if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 1 && PCShadowPacketCount != 0) {
            PCDeviceSetVertexShaderPc(NULL);
            PCDeviceSetPixelShaderPc(NULL);
            PCDeviceSetRenderState(15, 0);
            PCDeviceSetRenderState(23, 4);
            PCDeviceSetTexture(0, NULL);
            PCDeviceSetWorldIdentityPc();
            NuRndrArenaBuildShadowVolumesPc(0x80, 0);
            NuRndrShadowVolumeDrawPc();
            NuRndrShadowVolumeFinishPc();
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(52, 0);
            PCDeviceSetRenderState(14, 1);
            PCDeviceSetRenderState(27, 0);
            PCDeviceSetRenderState(22, 3);
            RendererArenaVertexCountPc = 0;
            RendererArenaCountPc = 0;
        }
        PCShadowPacketCount = 0;
    }

    void __cdecl NuRndrShadowOnOff_candidate(i32 value) {
        NuRndrControlStatePc = value;
    }

    /* Native PC entry 0x005fa220 is RET-only; the D3D release paths call it for the
     * geometry origin state. Its historical name is unknown. */
    void __cdecl PcGeometryOriginReleasePc(void) {
    }

    void __cdecl PcResetGeometryOriginStatePc(void) {
        PcGeometryViewMatrixPc = NuMtxSetIdentityTemplate;
        PcGeometryProjectionMatrixPc = NuMtxSetIdentityTemplate;
        PcGeometryViewProjectionMatrixPc = NuMtxSetIdentityTemplate;
        PcGeometryInverseViewProjectionMatrixPc = NuMtxSetIdentityTemplate;
        PcGeometryResetMatrixPc = NuMtxSetIdentityTemplate;
        PcGeometryDistanceOriginPc = PcGeometryDefaultOriginPc;
        for (i32 i = 0; i != 16; ++i) {
            PCDeviceSamplerInvalidation[i][1] = 0;
            PCDeviceSamplerInvalidation[i][0] = 0x012449d0;
        }
    }

    void __cdecl PCGraphicsSubmitViewportPc(i32 x, i32 y, i32 width, i32 height) {
        PC_VIEWPORT_PC viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.min_z = 0.0f;
        viewport.max_z = 1.0f;
        NuVpSubmitPc(&viewport);
    }

    void __cdecl PcUpdateGeometryCameraOriginPc(NUMTX *view, NUMTX *projection) {
        NUVEC scale;
        NUVEC translation;
        scale.z = 1.0f;
        translation.x = 0.0f;
        translation.y = 0.0f;
        translation.z = 0.0f;
        scale.x = PCSpecialProjectionAdjust.x_scale;
        scale.y = PCSpecialProjectionAdjust.y_scale;
        NuMtxSetScale(&PcGeometryScaleMatrixPc, &scale);
        translation.x = PCSpecialProjectionAdjust.x_offset;
        translation.y = PCSpecialProjectionAdjust.y_offset;
        NuMtxSetTranslation(&PcGeometryTranslationMatrixPc, &translation);
        NuMtxMulH(&PcGeometryAdjustedProjectionMatrixPc, projection, &PcGeometryScaleMatrixPc);
        NuMtxMulH(&PcGeometryAdjustedProjectionMatrixPc, &PcGeometryAdjustedProjectionMatrixPc,
                  &PcGeometryTranslationMatrixPc);
        PcGeometryViewMatrixPc = *view;
        PcGeometryProjectionMatrixPc = PcGeometryAdjustedProjectionMatrixPc;
        NUMTX inverse_view;
        NuMtxInvH(&inverse_view, &PcGeometryViewMatrixPc);
        PcGeometryDistanceOriginPc.x = inverse_view.m30 / inverse_view.m33;
        PcGeometryDistanceOriginPc.y = inverse_view.m31 / inverse_view.m33;
        PcGeometryDistanceOriginPc.z = inverse_view.m32 / inverse_view.m33;
        PcGeometryDistanceOriginPc.w = 1.0f;
        NuMtxMulH(&PcGeometryViewProjectionMatrixPc, &PcGeometryViewMatrixPc, &PcGeometryProjectionMatrixPc);
        NuMtxInvH(&PcGeometryInverseViewProjectionMatrixPc, &PcGeometryViewProjectionMatrixPc);
        NuShaderManagerSetfv(0x12, &PcGeometryViewMatrixPc.m00);
        NuShaderManagerSetfv(0x13, &PcGeometryViewProjectionMatrixPc.m00);
        NuShaderManagerSetfv(0x5a, &PcGeometryDistanceOriginPc.x);
        f32 fovy, aspect, z_near, z_far;
        NuMtxGetPerspectiveD3D(&PcGeometryProjectionMatrixPc, &fovy, &aspect, &z_near, &z_far);
        NUVEC4 depth = {z_near, z_far, z_far - z_near, 1.0f / PcGeometryProjectionMatrixPc.m22};
        NuShaderManagerSetfv(0x46, &depth.x);
        NUVEC4 frustum;
        NuMtxGetFrustumD3D(&PcGeometryProjectionMatrixPc, &frustum.x, &frustum.y, &frustum.z, &frustum.w, &z_near,
                           &z_far);
        frustum.y -= frustum.x;
        frustum.w -= frustum.z;
        NuShaderManagerSetfv(0x47, &frustum.x);
    }

    void __cdecl PCGraphicsSetSamplerTuplePc(u32 stage, const u32 *tuple) {
        union {
            f32 value;
            u32 bits;
        } bias;
        PCDeviceSetSamplerState(stage, 9, tuple[0] & 15);
        if ((((tuple[0] >> 4) ^ tuple[0]) & 15) == 0) {
            bias.value = -10.0f;
            PCDeviceSetSamplerState(stage, 8, bias.bits);
        } else {
            bias.value = 0.0f;
            PCDeviceSetSamplerState(stage, 8, bias.bits);
        }
        PCDeviceSetSamplerState(stage, 1, (tuple[0] >> 8) & 7);
        PCDeviceSetSamplerState(stage, 2, (tuple[0] >> 11) & 7);
        PCDeviceSetSamplerState(stage, 3, (tuple[0] >> 14) & 7);
        PCDeviceSetSamplerState(stage, 5, (tuple[0] >> 17) & 7);
        PCDeviceSetSamplerState(stage, 6, (tuple[0] >> 20) & 7);
        PCDeviceSetSamplerState(stage, 7, (tuple[0] >> 23) & 7);
        PCDeviceSetSamplerState(stage, 4, tuple[1]);
        PCDeviceSamplerInvalidation[stage][0] = tuple[0];
        PCDeviceSamplerInvalidation[stage][1] = tuple[1];
    }

    /* Converts a byte string into glyph indices of the font. */
    void __cdecl NuQFntAsciiToQCodePc(VUFNT *font, u16 *encoded, const char *text) {
        while (*text != '\0') {
            *encoded = UnicodeToIndexFast(font->unicode_map, font->unicode_count, (u16)(i16)*text);
            text++;
            encoded++;
        }
        *encoded = 0;
    }

    /* Nonzero when any texel of the 8-bit column at (x, y) differs from the key value. */
    static i32 __cdecl NuQFntColumnDiffers8Pc(NUTEXBMP_PC *bitmap, i32 x, i32 y, i32 count, i32 value) {
        i32 width = bitmap->width;
        u8 *texel = bitmap->data + width * y + x;
        i32 i;

        for (i = 0; i < count; i++) {
            if (*texel != value) {
                return 1;
            }
            texel += width;
        }
        return 0;
    }

    static i32 __cdecl NuQFntColumnOpaque32Pc(NUTEXBMP_PC *bitmap, i32 x, i32 y, i32 count) {
        return NuQFntColumnOpaque32Inline(bitmap, x, y, count);
    }

    static i32 __cdecl NuQFntFindGlyphStart8Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y, i32 value) {
        do {
            do {
                if (NuQFntColumnDiffers8Pc(bitmap, *x, *y, count, value)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    static i32 __cdecl NuQFntFindGlyphEnd8Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y, i32 value) {
        do {
            do {
                if (!NuQFntColumnDiffers8Pc(bitmap, *x, *y, count, value)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    static i32 __cdecl NuQFntFindGlyphStart32Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y) {
        do {
            do {
                if (NuQFntColumnOpaque32Pc(bitmap, *x, *y, count)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    static i32 __cdecl NuQFntFindGlyphEnd32Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y) {
        do {
            do {
                if (!NuQFntColumnOpaque32Pc(bitmap, *x, *y, count)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    void __cdecl NuQFntScanResetPc(void) {
        NuQFntScanYPc = 0;
        NuQFntScanXPc = 0;
    }

    /* Finds the next glyph cell in an 8-bit strip; returns nonzero once the strip is exhausted. */
    i32 __cdecl NuQFntNextGlyph8Pc(NUTEXBMP_PC *bitmap, i32 count, i32 value, f32 *x, f32 *y, f32 *width) {
        i32 exhausted = 0;
        i32 start;

        if (!NuQFntFindGlyphStart8Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc, value)) {
            exhausted = 1;
            NuQFntScanXPc = 0;
            NuQFntScanYPc = 0;
        }
        *x = (f32)NuQFntScanXPc;
        *y = (f32)NuQFntScanYPc;
        start = NuQFntScanXPc;
        if (!NuQFntFindGlyphEnd8Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc, value)) {
            exhausted = 1;
            NuQFntScanXPc = bitmap->width;
            NuQFntScanYPc = bitmap->height;
        }
        *width = (f32)(NuQFntScanXPc - start);
        return exhausted;
    }

    /* Finds the next glyph cell in a 32-bit strip; returns nonzero once the strip is exhausted. */
    i32 __cdecl NuQFntNextGlyph32Pc(NUTEXBMP_PC *bitmap, i32 count, f32 *x, f32 *y, f32 *width) {
        i32 exhausted = 0;
        i32 start;

        if (!NuQFntFindGlyphStart32Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc)) {
            exhausted = 1;
            NuQFntScanYPc = 0;
            NuQFntScanXPc = 0;
        }
        *x = (f32)NuQFntScanXPc;
        *y = (f32)NuQFntScanYPc;
        start = NuQFntScanXPc;
        if (!NuQFntFindGlyphEnd32Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc)) {
            exhausted = 1;
            NuQFntScanXPc = bitmap->width;
            NuQFntScanYPc = bitmap->height;
        }
        *width = (f32)(NuQFntScanXPc - start);
        return exhausted;
    }

    static u16 __cdecl NuQFntMapLookupPc(VUCHARIDX *map, i32 count, u16 unicode) {
        u16 i;

        for (i = 0; i < count; i++) {
            if (map[i].unicode == unicode) {
                return map[i].index;
            }
        }
        return 0xffff;
    }

    i32 __cdecl NuQFntMapAddPc(VUCHARIDX *map, i32 slot, u16 unicode, u16 index) {
        if (slot > 808 || slot < 0) {
            NuQFntAtlasErrorPc();
        }
        map[slot].unicode = unicode;
        map[slot].index = index;
        return slot + 1;
    }

    /* Font material set-up used by the atlas builder; mirrors NuQFntReadPS. */
    static i32 __cdecl NuQFntAtlasReadPSPc(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer,
                                           VARIPTR end) {
        NUQFNT_RENDER_DATA_PC *platform;
        NUQFNT_ATLAS_MATERIAL_PC *material;

        (void)end;
        NuTexGetWidth(texture_id);
        NuTexGetHeight(texture_id);
        buffer->addr = (buffer->addr + 15) & ~15U;
        font->platform_data = (NUQFNT_RENDER_DATA_PC *)buffer->void_ptr;
        buffer->addr += 0x58;
        platform = font->platform_data;
        font->color_abgr = &platform->colour;
        font->x_scale = (f32 *)((u8 *)platform + 0x50);
        font->y_scale = (f32 *)((u8 *)platform + 0x54);
        font->material = NuMtlCreateEx(1, plane);
        if (font->material != NULL) {
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->diffuse.x = 1.0f;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->diffuse.y = 1.0f;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->diffuse.z = 1.0f;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x1000U) | 0x2000U;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes |= 0xc000U;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->opacity = 1.0f;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~14U) | 1U;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes |= 0x40000U;
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->texture_id = (i16)texture_id;
            material = (NUQFNT_ATLAS_MATERIAL_PC *)font->material;
            if (flags & 8) {
                material->attributes_high &= ~4U;
            } else {
                material->attributes_high |= 4U;
            }
            ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x10000U) | 0x20000U;
            if (flags & 2) {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes &= ~15U;
            } else {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                    (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~14U) | 1U;
            }
            if (flags & 0x40) {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes &= ~0xc000U;
            } else if (flags & 0x20) {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                    (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x8000U) | 0x4000U;
            } else {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes |= 0xc000U;
            }
            if (flags & 4) {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                    (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x20U) | 0x10U;
            } else if (flags & 0x10) {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
                    (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x10U) | 0x20U;
            } else {
                ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes &= ~0x30U;
            }
            NuMtlUpdate(font->material);
            return 1;
        }
        return 0;
    }

    int __cdecl NuQFntMapComparePc(const void *a, const void *b) {
        u16 left = ((const VUCHARIDX *)a)->unicode;
        u16 right = ((const VUCHARIDX *)b)->unicode;

        if (left < right) {
            return -1;
        }
        return left != right;
    }

    /* Builds a font from a BMP glyph strip, one cell per character of text in strip order. */
    VUFNT *__cdecl NuQFntCreateFromBmpPc(const char *path, i32 cell_height, i32 baseline, i32 key_colour, u16 *text,
                                         i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end) {
        f32 width, y, x;
        i32 max_count;
        i32 count;
        NUTEXBMP_PC *bitmap;
        VUCHARIDX map[NUQFNT_ATLAS_MAP_SIZE];
        VUFNTCHAR glyphs[NUQFNT_ATLAS_GLYPH_SIZE];
        PC_LOCKED_RECT_PC locked;
        PC_TEXTURE_RESOURCE_PC *resource;
        NUTEXDIM_PC *texture;
        VUFNT *font;
        u8 *source, *destination;
        i32 row_size, row;
        i32 index, next, c, size;
        u16 glyph;
        i32 texture_id;

        NuStrLenW(text);
        memset(glyphs, 0, sizeof(glyphs));
        memset(map, 0xff, sizeof(map));
        bitmap = NuTexCreateBmpPc(path);
        if (bitmap == NULL) {
            return NULL;
        }
        if (bitmap->type != 5) {
            if (bitmap->type != 3) {
                NuQFntAtlasErrorPc();
            }
        } else {
            for (c = 0; c < 256; c++) {
                if (c == key_colour) {
                    bitmap->palette[c * 4 + 3] = 0;
                } else {
                    ((u32 *)bitmap->palette)[c] |= 0x7f000000;
                }
            }
        }

        NuQFntScanResetPc();
        index = 1;
        max_count = 0;
        for (; *text != 0; text++) {
            /* Skip the slots that characters with their own fixed slot occupy. */
            if (index == ' ') {
                index = '!';
            }
            if (index == '!') {
                index = '"';
            }
            if (index == '%') {
                index = '&';
            }
            if (index == '?') {
                index = '@';
            }
            if (index == '.') {
                index = '/';
            }
            if (index == ',') {
                index = '-';
            }
            if (index == '0') {
                index = ':';
            }
            if (index == 'A') {
                index = '[';
            }
            if (index == 'a') {
                index = '{';
            }
            if (index == '[') {
                index = '\\';
            }
            if (index == ']') {
                index = '^';
            }
            if (*text == ' ') {
                continue;
            }
            if ((*text >= 'a' && *text <= 'z') || (*text >= 'A' && *text <= 'Z') || (*text >= '0' && *text <= '9') ||
                *text == '!' || *text == '?' || *text == '.' || *text == ',' || *text == '%' || *text == '[' ||
                *text == ']') {
                NUQFNT_ATLAS_NEXT_GLYPH();
                glyphs[*text].x = x;
                glyphs[*text].y = y;
                glyphs[*text].width = width;
                next = NuQFntMapAddPc(map, *text, *text, *text);
            } else {
                NUQFNT_ATLAS_NEXT_GLYPH();
                glyphs[index].x = x;
                glyphs[index].y = y;
                glyphs[index].width = width;
                index = NuQFntMapAddPc(map, index, *text, (u16)index);
                next = index + 1;
            }
            if (max_count <= next) {
                max_count = next;
            }
        }

        glyphs[' '].x = 0.0f;
        glyphs[' '].y = 0.0f;
        glyphs[' '].width = (f32)(cell_height >> 2);
        count = max_count;
        map[' '].unicode = ' ';
        map[' '].index = ' ';

        /* Lower case letters without a cell borrow the upper case glyph. */
        for (c = 'a'; c <= 'z'; c++) {
            if (map[c].unicode == 0xffff) {
                glyph = NuQFntMapLookupPc(map, count, NuToUpper((u8)c));
                if (glyph != 0xffff) {
                    NuQFntMapAddPc(map, c, (u16)c, glyph);
                    if (max_count <= c) {
                        max_count = c;
                    }
                }
            }
        }
        /* Accented lower case letters fall back to their upper case forms. */
        for (c = 0xe0; c <= 0xff; c++) {
            if (NuQFntMapLookupPc(map, index, (u16)c) == 0xffff) {
                glyph = NuQFntMapLookupPc(map, count, NuToUpper((u8)c));
                if (glyph != 0xffff) {
                    index = NuQFntMapAddPc(map, index, (u16)c, glyph);
                }
            }
        }
        /* Curly quotes reuse the apostrophe. */
        if (NuQFntMapLookupPc(map, index, 0x91) == 0xffff) {
            glyph = NuQFntMapLookupPc(map, count, '\'');
            if (glyph != 0xffff) {
                index = NuQFntMapAddPc(map, index, 0x91, glyph);
            }
        }
        if (NuQFntMapLookupPc(map, index, 0x92) == 0xffff) {
            glyph = NuQFntMapLookupPc(map, count, '\'');
            if (glyph != 0xffff) {
                index = NuQFntMapAddPc(map, index, 0x92, glyph);
            }
        }
        if (index <= 'z') {
            index = '{';
        }
        qsort(map, index, sizeof(VUCHARIDX), NuQFntMapComparePc);

        texture = (NUTEXDIM_PC *)buffer->void_ptr;
        buffer->addr += sizeof(NUTEXDIM_PC);
        memset(texture, 0, sizeof(*texture));
        texture->width = bitmap->width;
        texture->height = bitmap->height;
        PCDeviceCreateTexture(bitmap->width, bitmap->height, bitmap->mips, 0, 0x15, 1, &texture->resource);
        texture_id = NuTexCreateNative(texture);
        if (texture_id == 0) {
            NuQFntAtlasErrorPc();
        }
        resource = NuTextureGetDimensions(texture_id)->resource;
        if (resource->methods->lock_rect(resource, 0, &locked, NULL, 0) >= 0) {
            source = bitmap->data;
            row_size = bitmap->width * 4;
            destination = (u8 *)locked.bits;
            for (row = 0; row < bitmap->height; row++) {
                memcpy(destination, source, row_size);
                destination += locked.pitch;
                source += row_size;
            }
            resource = NuTextureGetDimensions(texture_id)->resource;
            resource->methods->unlock_rect(resource, 0);
        }
        NuFree(bitmap->data, NuQFntSourcePathPC, 0x264);
        NuFree(bitmap, NuQFntSourcePathPC, 0x265);
        NuTexGetWidth(texture_id);
        NuTexGetHeight(texture_id);

        font = (VUFNT *)((buffer->addr + 15) & ~15U);
        buffer->addr = ((buffer->addr + 15) & ~15U) + sizeof(VUFNT);
        memset(font, 0, sizeof(VUFNT));
        font->glyph_count = count;
        font->height = (f32)cell_height;
        font->unicode_count = index;
        font->legacy_font = NULL;
        font->baseline = (f32)baseline;
        font->ic_gap = 0.0f;
        font->space_width = glyphs[' '].width;

        buffer->addr = (buffer->addr + 15) & ~15U;
        font->glyphs = (VUFNTCHAR *)buffer->void_ptr;
        buffer->addr += font->glyph_count * sizeof(VUFNTCHAR);
        memcpy(font->glyphs, glyphs, font->glyph_count * sizeof(VUFNTCHAR));
        for (size = font->glyph_count * sizeof(VUFNTCHAR); size & 15; size += 4) {
            *(u32 *)buffer->void_ptr = 0;
            buffer->addr += 4;
        }

        buffer->addr = (buffer->addr + 15) & ~15U;
        font->unicode_map = (VUCHARIDX *)buffer->void_ptr;
        buffer->addr += font->unicode_count * sizeof(VUCHARIDX);
        memcpy(font->unicode_map, map, font->unicode_count * sizeof(VUCHARIDX));
        buffer->addr = (buffer->addr + 15) & ~15U;
        font->size = buffer->addr - (u32)font;
        NuQFntAtlasReadPSPc(font, texture_id, flags, plane, buffer, end);
        return font;
    }

    void __cdecl PCGraphicsDestroyPostEffectTexturesPc(void) {
        NuPostEffectReleasePortsPc();
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectBackBufferTexturePc);
    }

    /* After a D3D reset, rebinds each live frame texture's resource and clears it to opaque black. */
    /* The colour+depth copy has no PC implementation; its only call is to the shared empty function. */
    void NuPostFilterGen::copy(GRAPHICS_RENDER_TEXTURE_PC *, GRAPHICS_RENDER_TEXTURE_PC *, PC_FRAMEBUFFER_PC *) {
        NuQFntAtlasErrorPc();
    }

    /* Non-virtual post-filter resize members; NuPostEffectResizePc calls them after a device reset. */
    void NuMainFilter::ResizeTexturesPc(i32, i32) {
    }

    void PC_EFFECT_SURFACE_PC::Reset() {
        if (texture != NULL && texture->surface != NULL)
            surface = NULL;
        if (surface != NULL) {
            surface->methods->release(surface);
            surface = NULL;
        }
        if (texture != NULL && owns_texture)
            PCGraphicsDestroyRenderTexture(texture);
        texture = NULL;
        owns_texture = 0;
        level = 0;
        field_10 = 0;
        height = width = 0;
    }

    void PC_EFFECT_SURFACE_PC::Attach(GRAPHICS_RENDER_TEXTURE_PC *new_texture, i32 new_level, bool own) {
        if (texture == new_texture && (i32)level == new_level)
            return;
        Reset();
        texture = new_texture;
        owns_texture = (u8)own;
        surface = NULL;
        level = (u16)new_level;
        field_10 = 0;
        height = width = 0;
        if (new_texture == NULL)
            return;
        if (new_texture->surface != NULL) {
            surface = new_texture->surface;
            width = new_texture->width;
            height = new_texture->height;
        } else {
            PC_TEXTURE_LEVEL_DESC_PC description;
            new_texture->resource->methods->get_surface_level(new_texture->resource, level, &surface);
            texture->resource->methods->get_level_desc(texture->resource, level, &description);
            width = (u16)description.width;
            height = (u16)description.height;
        }
    }

    void PC_EFFECT_SURFACE_PC::ReleaseResources() {
        for (u32 index = 0; index != 256; ++index) {
            PC_EFFECT_SURFACE_PC *item = PcSurfaceRegistry[index];
            if (item != NULL && item->surface != NULL) {
                if (item->texture->surface == NULL)
                    item->surface->methods->release(item->surface);
                PcSurfaceRegistry[index]->surface = NULL;
            }
        }
    }

    void PC_EFFECT_SURFACE_PC::RestoreResources() {
        for (u32 index = 0; index != 256; ++index) {
            if (PcSurfaceRegistry[index] != NULL) {
                if (PcSurfaceRegistry[index]->texture != NULL) {
                    if (PcSurfaceRegistry[index]->texture->surface != NULL) {
                        PcSurfaceRegistry[index]->surface = PcSurfaceRegistry[index]->texture->surface;
                        PcSurfaceRegistry[index]->width = PcSurfaceRegistry[index]->texture->width;
                        PcSurfaceRegistry[index]->height = PcSurfaceRegistry[index]->texture->height;
                    } else {
                        PC_TEXTURE_LEVEL_DESC_PC description;
                        PcSurfaceRegistry[index]->texture->resource->methods->get_surface_level(
                            PcSurfaceRegistry[index]->texture->resource, PcSurfaceRegistry[index]->level,
                            &PcSurfaceRegistry[index]->surface);
                        PcSurfaceRegistry[index]->texture->resource->methods->get_level_desc(
                            PcSurfaceRegistry[index]->texture->resource, PcSurfaceRegistry[index]->level, &description);
                        PcSurfaceRegistry[index]->width = (u16)description.width;
                        PcSurfaceRegistry[index]->height = (u16)description.height;
                    }
                } else
                    PcSurfaceRegistry[index]->surface = NULL;
            }
        }
    }

    void NuDeferredFilter::initTextureResources(i32 width, i32 height) {
        NuDeferredFilterGen::initTextureResources(width, height);
    }

    void NuDeferredFilter::ResizeTexturesPc(i32, i32) {
        if (textures[2] != NULL)
            PCGraphicsResizeEffectTexture(textures[2], -1, -1, -1);
        if (textures[0] != NULL)
            PCGraphicsResizeEffectTexture(textures[0], -1, -1, -1);
        for (i32 i = 0; i < 2; ++i) {
            if (textures[4 + i] != NULL)
                PCGraphicsResizeEffectTexture(textures[4 + i], textures[2]->width, textures[2]->height, -1);
        }
    }

    void NuMotionAccumFilter::initResources() {
        NuMotionAccumFilterGen::initResources();
        program = NULL;
    }

    void NuSpeedBlurFilter::initTextureResources(i32, i32) {
        texture_surface.Attach(PCGraphicsPostEffectBackBufferTexturePc, 0, false);
    }

    void NuSpeedBlurFilter::destroyTextureResources() {
        texture_surface.Reset();
    }

    void NuSpeedBlurFilter::ResizeTexturesPc(i32, i32) {
    }

    i32 __cdecl PcInitializeDynamicGeometryBuffersPc(PC_DYNAMIC_GEOMETRY_PC *geometry, i32 count) {
        geometry->vertex_buffer = NULL;
        PCDeviceCreateVertexBuffer((u32)count * 0x50, 0x208, 0, 0, &geometry->vertex_buffer);
        i32 segments =
            geometry->format ? geometry->format->records[geometry->format->record_count - 1].strip_segments : 5;
        i32 vertices = (i32)((u32)segments * 2 + 2);
        PCDeviceCreateVertexBuffer((u32)vertices * sizeof(PC_GEOMETRY_STRIP_VERTEX_PC), 0, 0, 0,
                                   &geometry->strip_vertex_buffer);
        PC_GEOMETRY_STRIP_VERTEX_PC *strip = NULL;
        PCDeviceLockVertexBuffer(0, 0, (void **)&strip, 0, geometry->strip_vertex_buffer);
        for (i32 i = 0; i < vertices; ++i) {
            strip[i].fraction = (f32)(i - 1) / (f32)(vertices - 3);
            if (strip[i].fraction < 0.0f)
                strip[i].fraction = 0.0f;
            if (strip[i].fraction > 1.0f)
                strip[i].fraction = 1.0f;
            strip[i].side = (i & 1) == 0 ? 0.5f : -0.5f;
        }
        PCDeviceUnlockVertexBuffer(geometry->strip_vertex_buffer);
        PCDeviceCreateIndexBuffer((u32)vertices * sizeof(u16), 0, 0x65, 0, &geometry->strip_index_buffer, NULL);
        u16 *indices;
        geometry->strip_index_buffer->methods->lock(geometry->strip_index_buffer, 0, 0, (void **)&indices, 0);
        for (i32 i = 0; i < vertices; ++i)
            indices[i] = (u16)i;
        geometry->strip_index_buffer->methods->unlock(geometry->strip_index_buffer);
        return 1;
    }

    u8 __cdecl PcTransformDynamicGeometryPc(PC_GEOMETRY_SKIN_PC *skin, PC_GEOMETRY_MESH_PC *mesh, u32 *output_index) {
        for (u32 group_index = 0; group_index < mesh->group_count; ++group_index) {
            PC_GEOMETRY_GROUP_PC *group = &mesh->groups[group_index];
            PC_GEOMETRY_BASIS_PC *basis = &PcGeometryTransformsPc[group->transform_index];
            for (u32 element = 0; element < group->element_count; ++element) {
                PC_GEOMETRY_INPUT_PC *input = &mesh->elements[group->first_element + element];
                PC_GEOMETRY_OUTPUT_PC *output = &PcGeometryOutputPc[*output_index];
                PC_GEOMETRY_BASIS_VECTOR_PC p00, p01, p02, partial0, row0;
                PC_GEOMETRY_SCALE(p02, basis->matrices[2].rows[0], input->basis_weights.z);
                PC_GEOMETRY_SCALE(p01, basis->matrices[1].rows[0], input->basis_weights.y);
                PC_GEOMETRY_SCALE(p00, basis->matrices[0].rows[0], input->basis_weights.x);
                PC_GEOMETRY_ADD(partial0, p00, p01);
                PC_GEOMETRY_ADD(row0, partial0, p02);
                PC_GEOMETRY_BASIS_VECTOR_PC p10, p11, p12, partial1, row1;
                PC_GEOMETRY_SCALE(p12, basis->matrices[2].rows[1], input->basis_weights.z);
                PC_GEOMETRY_SCALE(p11, basis->matrices[1].rows[1], input->basis_weights.y);
                PC_GEOMETRY_SCALE(p10, basis->matrices[0].rows[1], input->basis_weights.x);
                PC_GEOMETRY_ADD(partial1, p10, p11);
                PC_GEOMETRY_ADD(row1, partial1, p12);
                PC_GEOMETRY_BASIS_VECTOR_PC p20, p21, p22, partial2, row2;
                PC_GEOMETRY_SCALE(p22, basis->matrices[2].rows[2], input->basis_weights.z);
                PC_GEOMETRY_SCALE(p21, basis->matrices[1].rows[2], input->basis_weights.y);
                PC_GEOMETRY_SCALE(p20, basis->matrices[0].rows[2], input->basis_weights.x);
                PC_GEOMETRY_ADD(partial2, p20, p21);
                PC_GEOMETRY_ADD(row2, partial2, p22);
                PC_GEOMETRY_BASIS_VECTOR_PC cross;
                NuVecCross((NUVEC *)&cross, (NUVEC *)&row1, (NUVEC *)&row2);
                PC_GEOMETRY_CORNER_PC *corners = output->corners;
                if (input->skin_indices[0] >= 0 && skin->matrix_group_count > 1) {
                    corners[0] = input->corners[0];
                    for (i32 corner = 1; corner < 4; ++corner) {
                        NUVEC delta;
                        NuVecSub(&delta, &input->corners[corner].position, &input->corners[corner - 1].position);
                        corners[corner] = corners[corner - 1];
                        for (i32 j = 0; j < 3; ++j) {
                            i32 index = input->skin_indices[j];
                            if (index >= 0 && index < (i32)skin->matrix_group_count) {
                                NUVEC transformed;
                                f32 weight = (f32)((f64)input->skin_weights[j] * (f64)(1.0f / 255.0f));
                                NuVecMtxTransform(&transformed, &delta, &skin->matrices[index * 3 + corner - 1]);
                                NuVecScale(&transformed, &transformed, weight);
                                NuVecAdd(&corners[corner].position, &corners[corner].position, &transformed);
                            }
                        }
                    }
                } else {
                    for (i32 corner = 0; corner < 4; ++corner)
                        corners[corner] = input->corners[corner];
                }
                NUMTX matrix;
                matrix.m00 = row2.x;
                matrix.m01 = row2.y;
                matrix.m02 = row2.z;
                matrix.m03 = 0;
                matrix.m10 = cross.x;
                matrix.m11 = cross.y;
                matrix.m12 = cross.z;
                matrix.m13 = 0;
                matrix.m20 = row1.x;
                matrix.m21 = row1.y;
                matrix.m22 = row1.z;
                matrix.m23 = 0;
                matrix.m30 = row0.x;
                matrix.m31 = row0.y;
                matrix.m32 = row0.z;
                matrix.m33 = 1;
                for (i32 corner = 0; corner < 4; ++corner)
                    NuVecMtxTransform(&corners[corner].position, &corners[corner].position, &matrix);
                output->basis_axis.x = row1.x;
                output->basis_axis.y = row1.y;
                output->basis_axis.z = row1.z;
                output->distance_squared = 0;
                for (i32 corner = 0; corner < 4; ++corner)
                    corners[corner].w = input->output_w[corner];
                ++*output_index;
            }
        }
        return 1;
    }

    i32 __cdecl PcUpdateGeometryDistancesPc(PC_GEOMETRY_OUTPUT_PC *output, PC_GEOMETRY_SORT_ITEM_PC *items, i32 count) {
        PC_GEOMETRY_BASIS_VECTOR_PC origin;
        origin.x = PcGeometryDistanceOriginPc.x;
        origin.y = PcGeometryDistanceOriginPc.y;
        origin.z = PcGeometryDistanceOriginPc.z;
        for (i32 i = 0; i < count; ++i) {
            PC_GEOMETRY_BASIS_VECTOR_PC delta;
            NUVEC *position = &output[i].corners[2].position;
            delta.x = origin.x - position->x;
            delta.y = origin.y - position->y;
            delta.z = origin.z - position->z;
            output[i].distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        }
        for (i32 i = 0; i < count; ++i)
            items[i].distance_squared = output[items[i].output_index].distance_squared;
        return 1;
    }

    i32 __cdecl NuRndrSetAmbientLightPS(const NUVEC *colour) {
        render_state.ambient_intensity.x = colour->x;
        render_state.ambient_intensity.y = colour->y;
        render_state.ambient_intensity.z = colour->z;
        ++render_state.counter_0020;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
        return 1;
    }

    i32 __cdecl NuRndrSetDirectionalLightsPS(const NUVEC *d0, const NUVEC *c0, const NUVEC *d1, const NUVEC *c1,
                                             const NUVEC *d2, const NUVEC *c2) {
        NUMTX *view = NuCameraGetViewMtx();
        render_state.light_intensity[0] = *c0;
        render_state.light_intensity[1] = *c1;
        render_state.light_intensity[2] = *c2;
        NuVecMtxRotate(&render_state.light_direction[0], (NUVEC *)d0, view);
        NuVecMtxRotate(&render_state.light_direction[1], (NUVEC *)d1, view);
        NuVecMtxRotate(&render_state.light_direction[2], (NUVEC *)d2, view);
        NuVecNorm(&render_state.light_direction[0], &render_state.light_direction[0]);
        NuVecNorm(&render_state.light_direction[1], &render_state.light_direction[1]);
        NuVecNorm(&render_state.light_direction[2], &render_state.light_direction[2]);
        ++render_state.counter_0020;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
        return 1;
    }

    void __cdecl NuRndrStateSetSpecularLightEx(const NUVEC *direction, const NUMTX *matrix, const NUVEC *colour) {
        CAMERA_RENDER_SPECULAR_READER_PC *state = (CAMERA_RENDER_SPECULAR_READER_PC *)&render_state;
        render_state.specular_mtx = *matrix;
        render_state.specular_colour = *colour;
        state->direction = *direction;
        ++render_state.counter_0020;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
    }

    i32 __cdecl NuRndrStateUpdateCameraState(void) {
        NUMTX *projection = NuCameraGetProjectionMtx();
        NUMTX *view = NuCameraGetViewMtx();
        f64 horizontal, vertical;
        render_state.camera_matrix = *view;
        render_state.projection_0154 = projection->m00;
        render_state.projection_0158 = projection->m11;
        render_state.projection_015c = projection->m22;
        render_state.projection_0160 = projection->m23;
        render_state.projection_0164 = projection->m32;
        NuVpGetPosition2(&render_state.viewport_x, &render_state.viewport_y);
        NuVpGetSize2(&render_state.viewport_width, &render_state.viewport_height);
        horizontal = (f64)PS2_REZ_W / 640.0;
        render_state.viewport_x = (f32)(render_state.viewport_x * horizontal);
        render_state.viewport_width = (f32)(render_state.viewport_width * horizontal);
        vertical = (f64)PS2_REZ_H / 224.0;
        render_state.viewport_y = (f32)(render_state.viewport_y * vertical);
        render_state.viewport_height = (f32)(render_state.viewport_height * vertical);
        render_state.camera_packet_cache = NULL;
        ++render_state.counter_0020;
        ++render_state.camera_id;
        return 1;
    }

    void *__cdecl DisplayListBuildLightPacket(NUGLOBALRNDRSTATE_PC *state) {
        VARIPTR *cursor;
        NUPRIM_LIGHT_PACKET_PC *packet;
        i32 i;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = (NUPRIM_LIGHT_PACKET_PC *)cursor->void_ptr;
        cursor->void_ptr = packet + 1;
        packet->ambient_intensity.x = state->ambient_intensity.x;
        packet->ambient_intensity.y = state->ambient_intensity.y;
        packet->ambient_intensity.z = state->ambient_intensity.z;
        packet->ambient_intensity.w = 1.0f;
        for (i = 0; i < 3; ++i) {
            packet->light_intensity[i].x = state->light_intensity[i].x;
            packet->light_intensity[i].y = state->light_intensity[i].y;
            packet->light_intensity[i].z = state->light_intensity[i].z;
            packet->light_intensity[i].w = 1.0f;
            packet->light_direction[i].x = state->light_direction[i].x;
            packet->light_direction[i].y = state->light_direction[i].y;
            packet->light_direction[i].z = state->light_direction[i].z;
            packet->light_direction[i].w = 1.0f;
        }
        packet->specular_mtx = state->specular_mtx;
        packet->specular_colour = state->specular_colour;
        return packet;
    }

    void *__cdecl DisplayListBuildKonstPacket(NUGLOBALRNDRSTATE_PC *state) {
        VARIPTR *cursor;
        f32 *packet;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = (f32 *)cursor->void_ptr;
        if (state->state_00f4 != 0)
            *packet = state->state_00f8;
        else
            *packet = 1.0f;
        cursor->void_ptr = packet + 1;
        return packet;
    }

    void *__cdecl DisplayListBuildReflectionPacket(NUGLOBALRNDRSTATE_PC *state) {
        VARIPTR *cursor;
        u32 *packet;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = (u32 *)cursor->void_ptr;
        *packet = state->reflection_state;
        cursor->addr += sizeof(*packet);
        return packet;
    }

    void *__cdecl DisplayListBuildFogPacket(NUGLOBALRNDRSTATE_PC *state) {
        VARIPTR *cursor;
        void *packet;
        f32 *field;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = cursor->void_ptr;
        *(u32 *)cursor->void_ptr = state->fog_state_word;
        cursor->addr += sizeof(u32);
        if (state->fog_state_word != 0) {
            *(u32 *)cursor->void_ptr = (u32)state->fog_pointer_0178;
            cursor->addr += sizeof(u32);
            field = (f32 *)cursor->void_ptr;
            *field = state->fog_value_0170;
            cursor->void_ptr = field + 1;
            field = (f32 *)cursor->void_ptr;
            *field = state->fog_value_0174;
            cursor->void_ptr = field + 1;
            field = (f32 *)cursor->void_ptr;
            *field = state->fog_value_017c;
            cursor->void_ptr = field + 1;
        }
        return packet;
    }

    void *__cdecl DisplayListBuildVertexGroupPacket(NUGLOBALRNDRSTATE_PC *unused) {
        VARIPTR *cursor;
        u32 *packet;
        f32 *output;
        i8 *values;
        i32 i;

        if (NuPrimVertexGroupSource != NULL) {
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
            cursor = NuDisplayListStreamCursor;
            packet = (u32 *)cursor->void_ptr;
            *packet = NuPrimVertexGroupSource->count;
            cursor->addr += sizeof(u32);
            *(u32 *)cursor->void_ptr = NuPrimVertexGroupSource->flags;
            cursor->addr += sizeof(u32);
            output = (f32 *)cursor->void_ptr;
            cursor->addr += ((NuPrimVertexGroupSource->count + 3) / 4) * 0x10;
            values = NuPrimVertexGroupSource->values;
            for (i = 0; i < NuPrimVertexGroupSource->count; ++i) {
                *output++ = (f32)*values++;
            }
            return packet;
        }
        return NULL;
    }

    void __cdecl RndrStateCopyGlobalState(NUGLOBALRNDRSTATE_PC *state) {
        memcpy(state, &render_state, sizeof(*state));
        state->fog_packet_cache = NULL;
        state->camera_packet_cache = NULL;
        state->light_packet_cache = NULL;
        state->konst_packet_cache = NULL;
    }

    void __cdecl RndrStateResetGlobalState(NUGLOBALRNDRSTATE_PC *state) {
        state->state_00f4 = 0;
        state->state_0100 = 0;
        state->fog_packet_cache = NULL;
        state->light_packet_cache = NULL;
        state->camera_packet_cache = NULL;
        state->konst_packet_cache = NULL;
        state->reflection_packet_cache = NULL;

        render_state.state_0008 = 0;
        render_state.state_0014 = -1;
        render_state.counter_0020 = -1;
        memset(&render_state.lights_id, 0xff, sizeof(render_state.lights_id));
        memset(&render_state.camera_id, 0xff, sizeof(render_state.camera_id));
        memset(&render_state.fog_id, 0xff, sizeof(render_state.fog_id));
        memset(&render_state.counter_001e, 0xff, sizeof(render_state.counter_001e));
    }

    void __cdecl RndrStateResetSharedGlobalState(void) {
        render_state.state_00f4 = 0;
        render_state.state_0100 = 0;
        render_state.fog_packet_cache = NULL;
        render_state.light_packet_cache = NULL;
        render_state.camera_packet_cache = NULL;
        render_state.konst_packet_cache = NULL;
        render_state.reflection_packet_cache = NULL;
        render_state.state_0008 = 0;
        render_state.state_0014 = -1;
        render_state.counter_0020 = -1;
        memset(&render_state.lights_id, 0xff, sizeof(render_state.lights_id));
        memset(&render_state.camera_id, 0xff, sizeof(render_state.camera_id));
        memset(&render_state.fog_id, 0xff, sizeof(render_state.fog_id));
        memset(&render_state.counter_001e, 0xff, sizeof(render_state.counter_001e));
    }

    void __cdecl NuRndrStateInit(void) {
        memset(&render_state, 0, sizeof(render_state));
        NuMtxSetIdentity(&RndrStateTransformPc);
        NuMtxSetIdentity(&RndrStateScaledTransformPc);
        RndrStateTransformScalePc = 1.0f;
        ++render_state.counter_0020;
        ++render_state.fog_id;
        render_state.fog_state_word = 0;
        render_state.fog_packet_cache = NULL;
        NuRndrStateUpdateCameraState();
    }

    void __cdecl RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUVEC *tint,
                                            NUMTL *material) {
        (void)tint_enabled;
        (void)tint;
        render_state.state_00f8 = alpha;
        render_state.const_alpha_material = material;
        ++render_state.counter_0020;
        ++render_state.counter_001e;
        render_state.state_00f4 = alpha_enabled;
        render_state.konst_packet_cache = NULL;
    }

    void __cdecl NuRndrStateGetTransformPc(NUMTX *output, const NUMTL *material) {
        const PC_TRANSFORM_MATERIAL_VIEW *selected = (const PC_TRANSFORM_MATERIAL_VIEW *)material;
        f32 scale;
        NUVEC scaling;
        if (selected->next != NULL)
            selected = selected->next;
        if (selected->transform_kind != 0x81 && selected->transform_kind != 0x85) {
            *output = RndrStateTransformPc;
            return;
        }
        scale = selected->transform_scale != 0.0f ? selected->transform_scale : 1.0f;
        if (scale != RndrStateTransformScalePc) {
            RndrStateTransformScalePc = scale;
            RndrStateScaledTransformPc = RndrStateTransformPc;
            if (scale != 1.0f) {
                scaling.z = scale;
                scaling.y = scale;
                scaling.x = scale;
                NuMtxPreScale(&RndrStateScaledTransformPc, &scaling);
            }
        }
        *output = RndrStateScaledTransformPc;
    }

    void __cdecl NuRndrStateSetTransformPc(const NUMTX *matrix) {
        f32 temporary;
        RndrStateTransformScalePc = 1.0f;
        RndrStateTransformPc = *matrix;
        temporary = RndrStateTransformPc.m01;
        RndrStateTransformPc.m01 = RndrStateTransformPc.m10;
        RndrStateTransformPc.m10 = temporary;
        temporary = RndrStateTransformPc.m02;
        RndrStateTransformPc.m02 = RndrStateTransformPc.m20;
        RndrStateTransformPc.m20 = temporary;
        temporary = RndrStateTransformPc.m21;
        RndrStateTransformPc.m21 = RndrStateTransformPc.m12;
        RndrStateTransformPc.m12 = temporary;
        RndrStateScaledTransformPc = RndrStateTransformPc;
    }

    void __cdecl RndrStateSetReflection(i32 reflection) {
        render_state.reflection_state = (u32)reflection;
        ++render_state.counter_0020;
        ++render_state.reflection_id;
        render_state.reflection_packet_cache = NULL;
    }

    static void __cdecl NuRndrResetStreamBufferPc(void) {
        switch (RndrStreamIndexPc) {
            case 0:
                nudlist_stream_cursor = RndrStreamFirstPc;
                nuprim_field_ba8 = RndrStreamFirstPc.addr + RndrStreamSizePc;
                break;
            case 1:
                nudlist_stream_cursor = RndrStreamSecondPc;
                nuprim_field_ba8 = RndrStreamSecondPc.addr + RndrStreamSizePc;
                break;
        }
        if (nudlist_stream_cursor.addr & 15)
            nudlist_stream_cursor.addr = (nudlist_stream_cursor.addr + 15) & ~15u;
        SAGA_SKIN_CALL(skin_trace_stream, (unsigned)RndrStreamIndexPc, nudlist_stream_cursor.addr,
                       nuprim_field_ba8 - nudlist_stream_cursor.addr);
    }

    void __cdecl NuRndrSwapStreamBuffers(void) {
        switch (RndrStreamIndexPc) {
            case 0:
                RndrStreamUsedPc = nudlist_stream_cursor.addr - RndrStreamFirstPc.addr;
                break;
            case 1:
                RndrStreamUsedPc = nudlist_stream_cursor.addr - RndrStreamSecondPc.addr;
                break;
        }
        if (RndrStreamUsedPc > RndrStreamMaxUsedPc)
            RndrStreamMaxUsedPc = RndrStreamUsedPc;
        if (RndrStreamUsedPc > (u32)RndrStreamSizePc) {
            PCGraphicsRenderTextureAllocationError();
            DebugBreak();
        }
        RndrStreamIndexPc = (RndrStreamIndexPc + 1) % 2;
        NuRndrResetStreamBufferPc();
    }

    void __cdecl NuRndrStreamInit(i32 size, VARIPTR *buffer) {
        if (buffer == NULL) {
            RndrStreamFirstPc.void_ptr =
                NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nustream.c", 126);
            RndrStreamSecondPc.void_ptr =
                NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nustream.c", 127);
        } else {
            RndrStreamFirstPc.addr = (buffer->addr + 31) & ~31u;
            RndrStreamSecondPc.addr = (RndrStreamFirstPc.addr + size + 31) & ~31u;
            buffer->addr = RndrStreamSecondPc.addr + size;
        }
        RndrStreamSizePc = size;
        RndrStreamIndexPc = 0;
        memset(RndrStreamFirstPc.void_ptr, 0, size);
        memset(RndrStreamSecondPc.void_ptr, 0, size);
        NuRndrResetStreamBufferPc();
    }

    void __cdecl NuDisplayListDrawPrimitive2DPc(void *data) {
        DISPLAY_PRIMITIVE_PACKET_PC *packet = (DISPLAY_PRIMITIVE_PACKET_PC *)data;
        u32 count;
        const void *vertices;
        u32 point_size;
        f32 point_size_float;
        if (packet == NULL || PCShaderCurrentObjectPc() == NULL || packet->vertex_count == 0)
            return;
        count = packet->vertex_count;
        vertices = packet + 1;
        switch (packet->type) {
            case 12:
            case 16:
                if (count > 2)
                    PCDeviceDrawPrimitiveUPPc(4, count / 3, vertices, 24);
                break;
            case 15:
                if (count > 1)
                    PCDeviceDrawPrimitiveUPPc(3, count - 1, vertices, 24);
                break;
            case 13:
                if (count > 2)
                    PCDeviceDrawPrimitiveUPPc(5, count - 2, vertices, 24);
                break;
            case 14:
                if (count > 1)
                    PCDeviceDrawPrimitiveUPPc(2, count / 2, vertices, 24);
                break;
            case 18:
                point_size_float = 32.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(154, point_size);
                PCDeviceDrawPrimitiveUPPc(1, packet->vertex_count, vertices, 24);
                point_size_float = 1.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(154, point_size);
                break;
        }
    }

    void __cdecl NuDisplayListSkinPalettePc(void *arg) {
        PC_DL_SKIN_PALETTE_READER *packet = (PC_DL_SKIN_PALETTE_READER *)arg;
        i32 count = packet->count;
        void *elements = packet->elements;
        PC_SHADER_OBJECT_FULL_VIEW *shader = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
        if (shader != NULL) {
            if (count > 8)
                NuPtlDmaSlotsExhaustedPc();
            NuShaderObjectSetElementsfv(shader, 0x18, 0, count, (const f32 *)elements);
            SAGA_SKIN_CALL(skin_trace_palette, (unsigned)arg, count);
        }
    }

    void __cdecl NuDisplayListDrawFaceOnPc(void *arg) {
        PC_DL_FACEON_DRAW_READER *packet = (PC_DL_FACEON_DRAW_READER *)arg;
        if (packet == NULL)
            return;
        PCDeviceSetStreamSourcePc(0, packet->vertex_buffer, 0, 32);
        PCDeviceDrawPrimitivePc(4, packet->first_vertex, packet->primitive_count * 2u);
    }

    void __cdecl NuDisplayListLightsPc(void *arg) {
        PC_DL_LIGHTS_READER *packet = (PC_DL_LIGHTS_READER *)arg;
        if (PCDeviceHardwareVertexProcessingPC != 4) {
            NuShaderManagerSetfv(1, &packet->ambient.x);
            NuShaderManagerSetfv(6, &packet->direction[0].x);
            NuShaderManagerSetfv(7, &packet->direction[1].x);
            NuShaderManagerSetfv(8, &packet->direction[2].x);
            NuShaderManagerSetfv(3, &packet->intensity[0].x);
            NuShaderManagerSetfv(4, &packet->intensity[1].x);
            NuShaderManagerSetfv(5, &packet->intensity[2].x);
            NuShaderManagerSetfv(0x49, &packet->shine_matrix.m00);
        } else {
            NuShaderManagerSetfv(0x23, &packet->ambient.x);
        }
        if (PCDeviceHardwareVertexProcessingPC != 4) {
            PC_DL_NORMALIZED_DIRECTION direction;

            NuVec4Add(&direction.four, &packet->direction[0], &packet->direction[1]);
            NuVec4Add(&direction.four, &direction.four, &packet->direction[2]);
            NuVecNorm(&direction.normalized.three, &direction.normalized.three);
            NuShaderManagerSetfv(0x4c, &direction.four.x);
            direction.four.x =
                packet->intensity[0].x > (packet->intensity[1].x > packet->intensity[2].x ? packet->intensity[1].x
                                                                                          : packet->intensity[2].x)
                    ? packet->intensity[0].x
                    : (packet->intensity[1].x > packet->intensity[2].x ? packet->intensity[1].x
                                                                       : packet->intensity[2].x);
            direction.four.y =
                packet->intensity[0].y > (packet->intensity[1].y > packet->intensity[2].y ? packet->intensity[1].y
                                                                                          : packet->intensity[2].y)
                    ? packet->intensity[0].y
                    : (packet->intensity[1].y > packet->intensity[2].y ? packet->intensity[1].y
                                                                       : packet->intensity[2].y);
            direction.four.z =
                packet->intensity[0].z > (packet->intensity[1].z > packet->intensity[2].z ? packet->intensity[1].z
                                                                                          : packet->intensity[2].z)
                    ? packet->intensity[0].z
                    : (packet->intensity[1].z > packet->intensity[2].z ? packet->intensity[1].z
                                                                       : packet->intensity[2].z);
            direction.four.w = 1.0f;
            NuShaderManagerSetfv(0x4b, &direction.four.x);
        }
    }

    void __cdecl NuDisplayListTintPc(void *arg) {
        NUVEC4 tint = {1.0f, 1.0f, 1.0f, *(f32 *)arg};
        NuDisplayHandlerTintPc = tint;
        NuShaderManagerSetfv(0x38, &tint.x);
    }

    void __cdecl NuDisplayListFogPc(void *arg) {
        PC_DL_FOG_READER *packet = (PC_DL_FOG_READER *)arg;
        if (packet->enabled != 0) {
            i32 packed = (i32)packet->colour;
            NUVEC4 colour;
            NUVEC4 params;
            colour.x = (f32)((f32)(packed & 255) / 255.0);
            colour.y = (f32)((f32)((packed >> 8) & 255) / 255.0);
            colour.z = (f32)((f32)((packed >> 16) & 255) / 255.0);
            colour.w = (f32)((f32)((packed >> 24) & 255) / 255.0);
            params.x = packet->near_distance;
            params.y = packet->far_distance;
            params.z = packet->far_distance - packet->near_distance;
            params.w = packet->density;
            NuShaderManagerSetfv(0x44, &colour.x);
            NuShaderManagerSetfv(0x45, &params.x);
        } else {
            NUVEC4 params = {0.0f, 0.0f, 0.0f, 0.0f};
            NuShaderManagerSetfv(0x45, &params.x);
        }
    }

    void __cdecl NuDisplayListLightmapPacketPc(void *arg) {
        PCGraphicsFrameScratchCount.lightmap_packet = arg;
    }

    void __cdecl NuDisplayListUpdateLightmapPc(void) {
        PC_DL_LIGHTMAP_UPDATE_READER *packet;
        i32 stage;
        f32 offset[3];
        if (PCGraphicsFrameScratchCount.lightmap_packet == NULL ||
            (u32)PCGraphicsFrameScratchCount.lightmap_packet == (u32)PCGraphicsBoundShader)
            return;
        packet = (PC_DL_LIGHTMAP_UPDATE_READER *)PCGraphicsFrameScratchCount.lightmap_packet;
        PCGraphicsBoundShader = (i32)(u32)PCGraphicsFrameScratchCount.lightmap_packet;
        if (packet->mode == 2 && PcWindowOptions.usehires == 0)
            packet->mode = 1;
        if (packet->mode == 1) {
            NUTEXDIM_PC *texture;
            stage = 0;
            if (PCDeviceHardwareVertexProcessingPC == 1)
                stage = 1;
            if (packet->texture_id != -1)
                texture = NuTextureGetDimensions(packet->texture_id);
            else
                texture = NuTextureGetDimensions(1);
            PCDeviceSetTexture(stage, texture->resource);
            PCDeviceSetSamplerState(stage, 1, 1);
            PCDeviceSetSamplerState(stage, 2, 1);
            PCDeviceSetSamplerState(stage, 5, 2);
            PCDeviceSetSamplerState(stage, 6, 2);
            PCDeviceSetSamplerState(stage, 7, 0);
            offset[0] = packet->x_offset;
            offset[1] = -packet->y_offset;
            offset[2] = 0.0f;
            NuShaderManagerSetfv(0x60, offset);
        } else if (packet->mode == 2) {
            for (stage = 0; stage < 3; ++stage) {
                NUTEXDIM_PC *texture;
                if (packet->texture_ids[stage] != -1)
                    texture = NuTextureGetDimensions(packet->texture_ids[stage]);
                else
                    texture = NuTextureGetDimensions(1);
                PCDeviceSetTexture(stage, texture->resource);
                PCDeviceSetSamplerState(stage, 1, 1);
                PCDeviceSetSamplerState(stage, 2, 1);
                PCDeviceSetSamplerState(stage, 5, 2);
                PCDeviceSetSamplerState(stage, 6, 2);
                PCDeviceSetSamplerState(stage, 7, 0);
            }
            offset[0] = packet->x_offset;
            offset[1] = -packet->y_offset;
            offset[2] = 0.0f;
            NuShaderManagerSetfv(0x60, offset);
        }
    }

    void __cdecl NuDisplayListLegacyLightmapPc(void *arg) {
        i32 *texture_ids = (i32 *)arg;
        f32 offset[3] = {0.0f, 0.0f, 0.0f};
        i32 i;
        for (i = 0; i < 3; ++i) {
            NUTEXDIM_PC *texture;
            if (texture_ids[i] != -1)
                texture = NuTextureGetDimensions(texture_ids[i]);
            else
                texture = NuTextureGetDimensions(1);
            PCDeviceSetTexture(i, texture->resource);
            PCDeviceSetSamplerState(i, 1, 1);
            PCDeviceSetSamplerState(i, 2, 1);
        }
        NuShaderManagerSetfv(0x60, offset);
    }

    void __cdecl NuDisplayListLegacyLightmapOffsetPc(void *arg) {
        PC_DL_LEGACY_LIGHTMAP_READER *packet = (PC_DL_LEGACY_LIGHTMAP_READER *)arg;
        f32 offset[3];
        i32 i;
        for (i = 0; i < 3; ++i) {
            NUTEXDIM_PC *texture;
            if (packet->texture_id[i] != -1)
                texture = NuTextureGetDimensions(packet->texture_id[i]);
            else
                texture = NuTextureGetDimensions(1);
            PCDeviceSetTexture(i, texture->resource);
            PCDeviceSetSamplerState(i, 1, 1);
            PCDeviceSetSamplerState(i, 2, 1);
        }
        offset[0] = packet->x_offset;
        offset[1] = -packet->y_offset;
        offset[2] = 0.0f;
        NuShaderManagerSetfv(0x60, offset);
    }

    void __cdecl NuDisplayListPacketAdPc(void *arg) {
        PC_DL_AD_PACKET *packet = (PC_DL_AD_PACKET *)arg;
        PC_DL_AD_RECORD *record;
        NUVEC4 values;
        NUVEC colour0, colour1, colour2, colour3, colour4;
        i32 segments;
        u32 vertex_count;
        u32 instances = packet->instances;
        if (packet->vertex_buffer == NULL || packet->instance_buffer == NULL || packet->list == NULL)
            return;
        record = &packet->list->records[packet->list->count - 1];
        segments = record->segment_count;
        values.x = (f32)segments;
        values.y = record->enabled > 0 ? 1.0f : 0.0f;
        values.z = record->vertex_parameter_x;
        values.w = record->vertex_parameter_y;
        PCDeviceSetVertexShaderConstantsF(0xc8, &values.x, 1);
        values.x = record->vertex_parameter_z;
        values.y = 0.0f;
        values.z = 0.0f;
        values.w = 0.0f;
        PCDeviceSetVertexShaderConstantsF(0xc9, &values.x, 1);
        colour0 = record->colours.colour[0];
        colour1 = record->colours.colour[1];
        colour2 = record->colours.colour[2];
        colour3 = record->colours.colour[3];
        colour4 = record->colours.colour[4];
#define AD_COLOUR_CONSTANT(n)                                                                                          \
    values.x = colour##n.x;                                                                                            \
    values.y = colour##n.y;                                                                                            \
    values.z = colour##n.z;                                                                                            \
    values.w = 1.0f;                                                                                                   \
    PCDeviceSetPixelShaderConstantsF(0xca + (n), &values.x, 1)
        AD_COLOUR_CONSTANT(0);
        AD_COLOUR_CONSTANT(1);
        AD_COLOUR_CONSTANT(2);
        AD_COLOUR_CONSTANT(3);
        AD_COLOUR_CONSTANT(4);
#undef AD_COLOUR_CONSTANT
        values.x = record->pixel_parameter;
        values.y = record->pixel_parameter_x;
        values.z = record->pixel_parameter_y;
        values.w = record->pixel_parameter_z;
        PCDeviceSetPixelShaderConstantsF(0xd3, &values.x, 1);
        values.x = record->pixel_scalar;
        values.y = 0.0f;
        values.z = 0.0f;
        values.w = 0.0f;
        PCDeviceSetPixelShaderConstantsF(0xd4, &values.x, 1);
        PCDeviceSetStreamSourcePc(0, packet->vertex_buffer, 0, 0x10);
        PCDeviceSetStreamSourceFreqPc(0, instances | 0x40000000u);
        PCDeviceSetStreamSourcePc(1, packet->instance_buffer, 0, 0x50);
        PCDeviceSetStreamSourceFreqPc(1, 0x80000001u);
        PCDeviceSetIndicesPc(packet->index_buffer);
        vertex_count = (u32)segments * 2u + 2u;
        PCDeviceDrawIndexedPrimitivePc(5, 0, 0, vertex_count, 0, vertex_count - 2u);
        PCDeviceSetStreamSourcePc(0, NULL, 0, 0);
        PCDeviceSetStreamSourceFreqPc(0, 1);
        PCDeviceSetStreamSourceFreqPc(1, 1);
    }

    void __cdecl NuRndrScreenGrabTileInit(SCREENGRAB_TILE_PARAMS_PC *params, i32 tiles, f32 b, f32 a, f32 c) {
        (void)params;
        (void)tiles;
        (void)b;
        (void)a;
        (void)c;
    }

    void __cdecl NuRndrScreenGrabTileDeInit(SCREENGRAB_TILE_PARAMS_PC *params) {
        (void)params;
    }

    void __cdecl NuRndrScreenGrabTileBegin(void **tile) {
        *tile = NULL;
    }

    void __cdecl NuRndrScreenGrabTileEnd(void **tile) {
        (void)tile;
    }

    void __cdecl NuVpBuildScalingTemplatePc(void) {
        f32 width = (f32)PS2_REZ_W;
        f32 height = (f32)PS2_REZ_H;
        f32 min_z = vpCurrent.min_z;
        NuCameraScalingTemplate.m00 = (f32)((f64)width * NuQFntHalfScale);
        NuCameraScalingTemplate.m01 = 0.0f;
        NuCameraScalingTemplate.m02 = 0.0f;
        NuCameraScalingTemplate.m03 = 0.0f;
        NuCameraScalingTemplate.m10 = 0.0f;
        NuCameraScalingTemplate.m11 = (f32)(-height * NuQFntHalfScale);
        NuCameraScalingTemplate.m12 = 0.0f;
        NuCameraScalingTemplate.m13 = 0.0f;
        NuCameraScalingTemplate.m20 = 0.0f;
        NuCameraScalingTemplate.m21 = 0.0f;
        NuCameraScalingTemplate.m22 = (f32)(NuVpDepthRangePc - (f64)min_z);
        NuCameraScalingTemplate.m23 = 0.0f;
        NuCameraScalingTemplate.m30 = (f32)((f64)width * NuQFntHalfScale + 0.0);
        NuCameraScalingTemplate.m31 = (f32)(0.0 + (f64)height * NuQFntHalfScale);
        NuCameraScalingTemplate.m32 = min_z;
        NuCameraScalingTemplate.m33 = 1.0f;
    }

    void __cdecl NuVpUpdate(void) {
        PC_VIEWPORT_PC viewport;
        i32 x, y, width, height, right, bottom, limit_x, limit_y;
        i32 clipped_left = 0;
        i32 clipped_top = 0;
        f32 rounded_width;
        f32 rounded_height;
        f32 reciprocal;
        if (NuVpDirtyPc == 0)
            return;
        NuCameraProjectionGlobalScaleX = 1.0f;
        NuCameraProjectionGlobalOffsetX = 0.0f;
        NuCameraProjectionGlobalOffsetY = 0.0f;
        NuCameraProjectionGlobalScaleY = 1.0f;
        x = vpCurrent.x;
        y = vpCurrent.y;
        width = vpCurrent.width;
        height = vpCurrent.height;
        right = (i32)((u32)x + (u32)width);
        bottom = (i32)((u32)y + (u32)height);
        limit_x = (i32)((u32)PS2_REZ_W << 4);
        limit_y = (i32)((u32)PS2_REZ_H << 4);
        viewport.min_z = vpCurrent.min_z;
        viewport.max_z = vpCurrent.max_z;
        if (right < 0 || x >= limit_x || bottom < 0 || y >= limit_y)
            goto empty;
        if (x < 0) {
            width = right;
            x = 0;
            clipped_left = 1;
        }
        if (right > limit_x)
            width = (i32)((u32)width + (u32)limit_x - (u32)right);
        if (y < 0) {
            height = bottom;
            y = 0;
            clipped_top = 1;
        }
        if (bottom > limit_y)
            height = (i32)((u32)height + (u32)limit_y - (u32)bottom);
        if (width <= 0 || height <= 0)
            goto empty;
        rounded_width = (f32)width;
        NuCameraProjectionGlobalScaleX = (f32)((f64)vpCurrent.width / (f64)rounded_width);
        rounded_height = (f32)height;
        NuCameraProjectionGlobalScaleY = (f32)((f64)vpCurrent.height / (f64)rounded_height);
        NuCameraProjectionGlobalOffsetX = (f32)((f64)(i32)((u32)vpCurrent.width - (u32)width) / (f64)rounded_width);
        NuCameraProjectionGlobalOffsetY = (f32)(-(f64)(i32)((u32)vpCurrent.height - (u32)height) / (f64)rounded_height);
        if (clipped_left != 0)
            NuCameraProjectionGlobalOffsetX = -NuCameraProjectionGlobalOffsetX;
        if (clipped_top != 0)
            NuCameraProjectionGlobalOffsetY = -NuCameraProjectionGlobalOffsetY;
        goto submit;
    empty:
        x = y = width = height = 0;
    submit:
        viewport.x = (u32)(x >> 4);
        viewport.y = (u32)(y >> 4);
        viewport.width = (u32)(width >> 4);
        viewport.height = (u32)(height >> 4);
        NuVpDirtyPc = 0;
        NuVpSubmitPc(&viewport);
        NuMtxSetIdentity(&NuVpIdentityMatrixPc);
        NuVpBuildScalingTemplatePc();
        if (vpCurrent.clip_min_x == 0.0f && vpCurrent.clip_max_x == 1.0f) {
            vpCurrent.projection_scale_x = 1.0f;
            vpCurrent.scissor_scale_x = (f32)((f64)vpCurrent.width * NuVpFixed16ScalePc);
        } else {
            reciprocal = (f32)(1.0 / ((f64)vpCurrent.clip_max_x - (f64)vpCurrent.clip_min_x));
            vpCurrent.scissor_scale_x = reciprocal;
            vpCurrent.projection_scale_x = vpCurrent.scissor_scale_x;
        }
        if (vpCurrent.clip_min_y == 0.0f && vpCurrent.clip_max_y == 1.0f) {
            vpCurrent.projection_scale_y = 1.0f;
            vpCurrent.scissor_scale_y = (f32)((f64)vpCurrent.height * NuVpFixed16ScalePc);
        } else {
            reciprocal = (f32)(1.0 / ((f64)vpCurrent.clip_max_y - (f64)vpCurrent.clip_min_y));
            vpCurrent.scissor_scale_y = reciprocal;
            vpCurrent.projection_scale_y = vpCurrent.scissor_scale_y;
        }
    }

    void __cdecl RenderPostViewportSetPc(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x,
                                         f32 dest_y, f32 dest_right, f32 dest_bottom) {
        RenderPostViewportPrefixPc.source_x = source_x;
        RenderPostViewportPrefixPc.source_y = source_y;
        RenderPostViewportPrefixPc.source_right = source_right;
        RenderPostViewportPrefixPc.source_bottom = source_bottom;
        RenderPostViewportPrefixPc.dest_x = dest_x;
        RenderPostViewportPrefixPc.dest_y = dest_y;
        RenderPostViewportPrefixPc.dest_right = dest_right;
        RenderPostViewportPrefixPc.dest_bottom = dest_bottom;
        RenderPostViewportPrefixPc.x_offset = dest_x;
        RenderPostViewportPrefixPc.y_offset = dest_y;
        f32 width = (f32)PS2_VREZ_W;
        RenderPostViewportPrefixPc.width_scale = (dest_right - dest_x) / width;
        f32 height = (f32)PS2_VREZ_H;
        RenderPostViewportPrefixPc.height_scale = (dest_bottom - dest_y) / height;
        PCSpecialProjectionAdjust.x_scale = width / (source_right - source_x);
        PCSpecialProjectionAdjust.y_scale = height / (source_bottom - source_y);
        PCSpecialProjectionAdjust.x_offset = ((width - source_right) - source_x) * 2.0f / width;
        PCSpecialProjectionAdjust.y_offset = ((height - source_bottom) - source_y) * -2.0f / height;
    }

    i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *handle, NUMTX *matrix, f32 alpha) {
        i32 result;
        if (handle->scene == NULL)
            return 0;
        if (alpha <= 0.0f)
            return 0;
        if (alpha < 1.0f) {
            NuSpecialConstAlpha(1, alpha);
            result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
            NuSpecialConstAlpha(0, 0.0f);
        } else
            result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
        return result;
    }

    /* PC implementation of the NuInitDebrisRenderer family. */
    void __cdecl NuDebrisCreateVertexBuffersPc(void) {
        i32 index;
        NuDebrisCurrentBufferIndex = 0;
        NuDebrisUseSystemMemory = 0;
        NuDebrisCurrentVertexCount = 0;
        NuDebrisLastHeader = NULL;
        for (index = 0; index != 4; ++index)
            PCDeviceCreateVertexBuffer(NuDebrisVertexBufferSize, 0x208, 0, 0, &NuDebrisVertexBuffers[index]);
        PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, &NuDebrisVertexData, 0x2000,
                                 NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
    }

    void __cdecl NuDebrisReleaseVertexBuffersPc(void) {
        PC_BUFFER_RESOURCE_PC **buffer = NuDebrisVertexBuffers;
        do {
            PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE_PC **)buffer);
            ++buffer;
        } while (buffer != NuDebrisVertexBuffers + 4);
    }

    void __cdecl NuDebrisRendererFlushBuffers(void) {
        if (NuDebrisUseSystemMemory == 0 && NuDebrisVertexData != NULL)
            PCDeviceUnlockVertexBuffer(NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        NuDebrisVertexData = NULL;
        NuDebrisCurrentBufferIndex = 0;
        NuDebrisUseSystemMemory = 0;
        NuDebrisCurrentVertexCount = 0;
        NuDebrisLastHeader = NULL;
        NuDebrisFrameVertexCount = 0;
    }

    i32 __cdecl NuDebrisRendererNextBuffer(void) {
        if (NuDebrisUseSystemMemory == 0 && NuDebrisVertexData != NULL)
            PCDeviceUnlockVertexBuffer(NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        if (NuDebrisUseSystemMemory != 0) {
            if (NuDebrisCurrentBufferIndex + 1 >= 8) {
                PCClipScreenUnsupported();
                return 0;
            }
            NuDebrisCurrentBufferIndex = (NuDebrisCurrentBufferIndex + 1) & 7;
        } else if (NuDebrisCurrentBufferIndex + 1 >= 4) {
            NuDebrisCurrentBufferIndex = 0;
            NuDebrisUseSystemMemory = 1;
        } else {
            NuDebrisCurrentBufferIndex = (NuDebrisCurrentBufferIndex + 1) & 3;
        }
        NuDebrisCurrentVertexCount = 0;
        if (NuDebrisUseSystemMemory != 0)
            NuDebrisVertexData = NuDebrisSystemBuffers[NuDebrisCurrentBufferIndex];
        else
            PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, &NuDebrisVertexData, 0x2000,
                                     NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        return 1;
    }

    void __cdecl NuDisplayListDrawDebrisPc(void *arg) {
        PC_DL_DEBRIS_DRAW_READER *packet = (PC_DL_DEBRIS_DRAW_READER *)arg;
        if (packet->vertex_count == 0)
            return;
        if (packet->use_system_buffer != 0) {
            const void *vertices = (const u8 *)NuDebrisSystemBuffers[packet->buffer_index] + packet->first_vertex * 24u;
            PCDeviceDrawPrimitiveUPPc(4, packet->vertex_count / 3u, vertices, 24);
        } else {
            PCDeviceSetStreamSourcePc(0, NuDebrisVertexBuffers[packet->buffer_index], 0, 24);
            PCDeviceDrawPrimitivePc(4, packet->first_vertex, packet->vertex_count / 3u);
        }
    }

    DEBRIS_PARTICLE_CHUNK_PC *__cdecl CreateDmaParticleSet(void *memory, i32 *size) {
        u8 *cursor = (u8 *)memory;
        u8 *start = cursor;
        DEBRIS_PARTICLE_CHUNK_PC *chunk = (DEBRIS_PARTICLE_CHUNK_PC *)cursor;
        DEBRISPARTICLE_PC *particle;
        i32 i;
        chunk->command = 0x52;
        chunk->next = NULL;
        cursor += 0x10;
        ((u32 *)cursor)[1] = 0;
        ((u32 *)cursor)[2] = 0;
        ((u32 *)cursor)[3] = 0;
        ((u32 *)cursor)[4] = 0;
        cursor += 0x10;
        for (i = 0; i < 32; ++i) {
            particle = (DEBRISPARTICLE_PC *)cursor;
            particle->position.x = 1.0f;
            particle->position.y = 2.0f;
            particle->position.z = 3.0f;
            particle->momentum.x = 4.0f;
            particle->momentum.y = 5.0f;
            particle->momentum.z = 6.0f;
            particle->start_time = -1.0f;
            particle->inverse_lifetime = 128.0f;
            cursor += sizeof(DEBRISPARTICLE_PC);
        }
        *(u32 *)cursor = 0;
        cursor += sizeof(u32);
        *size = (i32)(cursor - start);
        return (DEBRIS_PARTICLE_CHUNK_PC *)start;
    }

    DEBRIS_PARTICLE_CHUNK_PC *__cdecl CreateDmaParticleSetGlass(void *memory, i32 *size) {
        return CreateDmaParticleSet(memory, size);
    }

    DEBRIS_PART_HEADER_PC *__cdecl CreateDmaPartEffectList(void *memory, i32 *size) {
        u8 *cursor = (u8 *)(((u32)memory + 0xf) & ~0xf);
        u8 *start = cursor;
        DEBRIS_PART_HEADER_PC *header = (DEBRIS_PART_HEADER_PC *)cursor;
        cursor = (u8 *)&header->frames[64];
        *size = (i32)(cursor - start);
        return (DEBRIS_PART_HEADER_PC *)start;
    }

    void __cdecl LinkDmaParticalSets(DEBRIS_PARTICLE_CHUNK_PC **chunks, i32 count) {
        DEBRIS_PARTICLE_CHUNK_PC *chunk = chunks[count - 1];
        i32 i;
        chunk->command = 0x52;
        chunk->next = NULL;
        for (i = count - 2; i >= 0; --i) {
            chunk = chunks[i];
            chunk->command = 0x4e;
            chunk->next = chunks[i + 1];
        }
    }

    void __cdecl NuRndrParticleSetRepeat(NUVEC *position) {
        NUVEC repeat;
        repeat.x = (position->x - NuRndrDebBase.x) / NuRndrDebRange.x;
        repeat.y = (position->y - NuRndrDebBase.y) / NuRndrDebRange.y;
        repeat.z = (position->z - NuRndrDebBase.z) / NuRndrDebRange.z;
        repeat.x -= (f32)((i32)(repeat.x + 65536.0) - 65536);
        repeat.y -= (f32)((i32)(repeat.y + 65536.0) - 65536);
        repeat.z -= (f32)((i32)(repeat.z + 65536.0) - 65536);
        position->x = NuRndrDebBase.x + repeat.x * NuRndrDebRange.x;
        position->y = NuRndrDebBase.y + repeat.y * NuRndrDebRange.y;
        position->z = NuRndrDebBase.z + repeat.z * NuRndrDebRange.z;
    }

    void __cdecl GenericDebinfoDmaTypeUpdate(DEBRIS_INFO_PC *effect) {
        if (effect->native_data == NULL) {
            if (freeDmaDebType >= EDPP_MAX_DMADEBTYPES) {
                NuPtlDmaSlotsExhaustedPc();
                return;
            }
            effect->native_data = DmaDebTypes[freeDmaDebType++];
        }

        DEBRIS_PART_HEADER_PC *header = effect->native_data;
        header->gravity = effect->gravity;
        header->texture_u0 =
            static_cast<f32>(static_cast<u32>(effect->texture_u0) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
        header->texture_v0 =
            static_cast<f32>(static_cast<u32>(effect->texture_v0) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
        header->texture_u1 =
            static_cast<f32>(static_cast<u32>(effect->texture_u1) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
        header->texture_v1 =
            static_cast<f32>(static_cast<u32>(effect->texture_v1) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);

        f32 width, height, rotation, alpha, red, green, blue;
        f32 time, duration, elapsed, wave_x, wave_y, texture_x_numerator, texture_y_numerator;
        i32 i;
        for (i32 frame_index = 0; frame_index < 64; ++frame_index) {
            width = height = rotation = 0.0f;
            red = green = blue = alpha = 0.0f;
            time = static_cast<f32>(frame_index) / 64.0f;
            for (i = 0; i < 7; ++i) {
                if (effect->width_keys[i].time <= time && time <= effect->width_keys[i + 1].time) {
                    duration = effect->width_keys[i + 1].time - effect->width_keys[i].time;
                    elapsed = time - effect->width_keys[i].time;
                    width = elapsed == 0.0f ? effect->width_keys[i].value
                                            : effect->width_keys[i].value +
                                                  (effect->width_keys[i + 1].value - effect->width_keys[i].value) *
                                                      (elapsed / duration);
                    break;
                }
            }
            for (i = 0; i < 7; ++i) {
                if (effect->height_keys[i].time <= time && time <= effect->height_keys[i + 1].time) {
                    duration = effect->height_keys[i + 1].time - effect->height_keys[i].time;
                    elapsed = time - effect->height_keys[i].time;
                    height = elapsed == 0.0f ? effect->height_keys[i].value
                                             : effect->height_keys[i].value +
                                                   (effect->height_keys[i + 1].value - effect->height_keys[i].value) *
                                                       (elapsed / duration);
                    break;
                }
            }
            for (i = 0; i < 7; ++i) {
                if (effect->rotation_keys[i].time <= time && time <= effect->rotation_keys[i + 1].time) {
                    duration = effect->rotation_keys[i + 1].time - effect->rotation_keys[i].time;
                    elapsed = time - effect->rotation_keys[i].time;
                    rotation = elapsed == 0.0f ? effect->rotation_keys[i].value
                                               : effect->rotation_keys[i].value + (effect->rotation_keys[i + 1].value -
                                                                                   effect->rotation_keys[i].value) *
                                                                                      (elapsed / duration);
                    break;
                }
            }
            wave_x = effect->jib_x_amplitude * NU_SIN_LUT(65536.0 * effect->jib_x_frequency * time);
            wave_y = effect->jib_y_amplitude * NU_SIN_LUT(65536.0 * effect->jib_y_frequency * time);
            texture_x_numerator =
                NU_SIN_LUT(rotation + 16384.0) * (width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation) + wave_x;
            texture_y_numerator =
                NU_SIN_LUT(rotation) * (-width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation + 16384.0) + wave_y;

            DEBRIS_PARTICLE_FRAME_PC &frame = header->frames[frame_index];
            frame.position.x =
                (NU_SIN_LUT(rotation + 16384.0) * (-width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation) + wave_x) /
                2048.0f;
            frame.position.y =
                (NU_SIN_LUT(rotation) * (width / 4.0f) - (height / 4.0f) * NU_SIN_LUT(rotation + 16384.0) + wave_y) /
                2048.0f;
            frame.position.z = 0.0f;
            frame.texture_offset.x = texture_x_numerator / 2048.0f;
            frame.texture_offset.y = texture_y_numerator / 2048.0f;
            frame.texture_offset.z = 0.0f;
            frame.extent.x = (texture_x_numerator + NU_SIN_LUT(rotation) * (height / 2.0f)) / 2048.0f;
            frame.extent.y = (texture_y_numerator + NU_SIN_LUT(rotation + 16384.0) * (height / 2.0f)) / 2048.0f;
            frame.extent.z = 0.0f;
            for (i = 0; i < 7; ++i) {
                if (effect->colour_keys[i].time <= time && time <= effect->colour_keys[i + 1].time) {
                    duration = effect->colour_keys[i + 1].time - effect->colour_keys[i].time;
                    elapsed = time - effect->colour_keys[i].time;
                    if (elapsed == 0.0f) {
                        red = effect->colour_keys[i].red;
                        green = effect->colour_keys[i].green;
                        blue = effect->colour_keys[i].blue;
                    } else {
                        red = effect->colour_keys[i].red +
                              static_cast<i32>(effect->colour_keys[i + 1].red - effect->colour_keys[i].red) *
                                  (elapsed / duration);
                        green = effect->colour_keys[i].green +
                                static_cast<i32>(effect->colour_keys[i + 1].green - effect->colour_keys[i].green) *
                                    (elapsed / duration);
                        blue = effect->colour_keys[i].blue +
                               static_cast<i32>(effect->colour_keys[i + 1].blue - effect->colour_keys[i].blue) *
                                   (elapsed / duration);
                    }
                    break;
                }
            }
            for (i = 0; i < 7; ++i) {
                if (effect->alpha_keys[i].time <= time && time <= effect->alpha_keys[i + 1].time) {
                    duration = effect->alpha_keys[i + 1].time - effect->alpha_keys[i].time;
                    elapsed = time - effect->alpha_keys[i].time;
                    alpha = elapsed == 0.0f ? effect->alpha_keys[i].value
                                            : effect->alpha_keys[i].value +
                                                  (effect->alpha_keys[i + 1].value - effect->alpha_keys[i].value) *
                                                      (elapsed / duration);
                    break;
                }
            }
            alpha += alpha;
            if (alpha > 255.0f)
                alpha = 255.0f;
            frame.colour = (((static_cast<u32>(alpha) << 8) | static_cast<u32>(red)) << 8 | static_cast<u32>(green))
                               << 8 |
                           static_cast<u32>(blue);
        }
    }

    void __cdecl NuPrim2DEnd(void) {
        *NuPrimVertexCountPointer = NuPrimVertexCount;
        NuPrimVertexCount = 0;
    }

    void __cdecl NuPrim3DEnd(void) {
        *NuPrimVertexCountPointer = NuPrimVertexCount;
        NuPrimVertexCount = 0;
    }

    void __cdecl NuPrim2DAddXYZ(f32 x, f32 y, f32 z) {
        ((NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->x = NuPrimXScale * x + NuPrimXBias;
        ((NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->y = NuPrimYScale * y + NuPrimYBias;
        ((NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(NUPRIMVERTEX_PC);
        NuPrimVertexCount++;

        if (NuPrimCurrentPrimitiveType == 0x10 && (NuPrimVertexCount & 1) == 0) {
            /* Expand the two sprite corners just written into two triangles. */
            NUPRIMVERTEX_PC *next = (NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
            NuPrimStreamBufferPointer->void_ptr = next + 4;
            NuPrimVertexCount += 4;
            next[0] = next[-1];
            next[1] = next[-1];
            next[2].x = next[-2].x;
            next[2].y = next[-1].y;
            next[2].z = next[-1].z;
            next[2].colour = next[-1].colour;
            next[2].u = next[-2].u;
            next[2].v = next[-1].v;
            next[-1].x = next[0].x;
            next[-1].y = next[-2].y;
            next[-1].z = next[-2].z;
            next[-1].colour = next[-2].colour;
            next[-1].u = next[0].u;
            next[-1].v = next[-2].v;
            next[3] = next[-2];
        }
    }

    void __cdecl NuPrimInit(VARIPTR *buffer, VARIPTR end) {
        (void)buffer;
        (void)end;
    }

    void __cdecl NuPrimSetCoordinateSystem(i32 scale_mode) {
        NuPrimCoordSystemStack[NuPrimCSPos] = scale_mode;

        switch (scale_mode) {
            case 0:
                NuPrimXScale = NuPrimPS2XScale;
                NuPrimYScale = NuPrimPS2YScale;
                NuPrimXBias = nu_fsign_minus_one;
                NuPrimYBias = 1.0f;
                break;
            case 1:
                NuPrimXScale = 1.0f;
                NuPrimYScale = nu_fsign_minus_one;
                NuPrimXBias = 0.0f;
                NuPrimYBias = 0.0f;
                break;
            case 2:
                NuPrimXScale = NuPrimTwo;
                NuPrimYScale = NuPrimMinusTwo;
                NuPrimXBias = nu_fsign_minus_one;
                NuPrimYBias = 1.0f;
                break;
        }
    }

    /* Native PC entry 0x00600bb0 is RET-only; the D3D release path calls it before
     * releasing the geometry buffers (0x00600bc0 is its restore counterpart). */
    void __cdecl NuPrimReleaseResourcesPc(void) {
    }

    void __cdecl NuKeyboardWindowStateLeafPc(void) {
    }

    void __cdecl PortalInitialisePsPc(NUGSCN *scene) {
        scene->instance_visibility_flags = NuPortalVisibilityBitsPc;
    }

    void __cdecl PortalFlagRoomVisiblePc(NUGSCNROOM *room, NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            for (i = 0; i < room->instance_count; ++i) {
                i32 index = room->instance_indices[i];
                NuPortalVisibilityBitsPc[index >> 3] |= (u8)(1 << (index & 7));
            }
        } else {
            for (i = 0; i < room->instance_count; ++i)
                scene->instances[room->instance_indices[i]].flags |= 4;
        }
    }

    void __cdecl PortalSetAllVisiblePc(NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            memset(NuPortalVisibilityBitsPc, 255, 625);
        } else {
            for (i = 0; i < scene->num_instances; ++i)
                scene->instances[i].flags |= 4;
        }
    }

    void __cdecl PortalSetAllHiddenPc(NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            memset(NuPortalVisibilityBitsPc, 0, 625);
        } else {
            for (i = 0; i < scene->num_instances; ++i)
                scene->instances[i].flags &= ~4U;
        }
    }

    void __cdecl PortalClipRoomPc(NUGSCN *scene, NUGSCNROOM *room, NUPORTAL_FRUSTUM_PC *frustum) {
        NUDISPLAY_LIST_PC *display = scene->display_list;
        u32 i;
        if (display != NULL && (display->render_flags & 0x4000) != 0) {
            for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
                u32 index = (u32)(i32)room->instance_indices[i];
                u32 byte_index = index >> 3;
                u32 bit = index & 7;
                if (((NuPortalVisibilityBitsPc[byte_index] >> bit) & 1) != 0)
                    continue;
                display = scene->display_list;
                if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                    NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                } else {
                    u8 flags = display->visibility_flags[index];
                    if ((flags & 1) != 0) {
                        if ((flags & 8) != 0)
                            NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                        else if (NuPortalClipTestBox(&((PORTAL_BOX_PC *)scene->portal_boxes)[index].first,
                                                     &((PORTAL_BOX_PC *)scene->portal_boxes)[index].second,
                                                     frustum) != 0)
                            NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                    }
                }
            }
        } else {
            for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
                u32 index = (u32)(i32)room->instance_indices[i];
                u32 byte_index = index >> 3;
                u32 bit = index & 7;
                u8 bits = NuPortalVisibilityBitsPc[byte_index];
                if (((bits >> bit) & 1) != 0)
                    continue;
                /* Native dereferences a null display only for an unseen instance. */
                display = scene->display_list;
                if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                    NuPortalVisibilityBitsPc[byte_index] = bits | (u8)(1 << bit);
                    continue;
                }
                if ((display->visibility_flags[index] & 8) != 0) {
                    NuPortalVisibilityBitsPc[byte_index] = bits | (u8)(1 << bit);
                    continue;
                }
                switch (clipTestSphere(&scene->portal_spheres[index], frustum)) {
                    case 1:
                        NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                        break;
                    case 2:
                        if (clipTestBox(&((PORTAL_BOX_PC *)scene->portal_boxes)[index].first,
                                        &((PORTAL_BOX_PC *)scene->portal_boxes)[index].second, frustum->planes,
                                        frustum->plane_count) != 0)
                            NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                        break;
                }
            }
        }
    }

    void __cdecl NuMtlCreatePS(NUMTL *material, i32 is_3d) {
        PC_MATERIAL_UPDATE_VIEW *mtl = reinterpret_cast<PC_MATERIAL_UPDATE_VIEW *>(material);
        NuShaderMtlDescInit(reinterpret_cast<PC_MATERIAL_CREATE_SHADER *>(&mtl->shader));
        if (mtl->texture_id > 0) {
            mtl->shader.diffuse_map_id = mtl->texture_id;
            if (mtl->shader.texture_unit_count <= 0)
                mtl->shader.texture_unit_count = 1;
            if ((mtl->shader.vertex_format & 0x3800) <= 0)
                mtl->shader.vertex_format = (mtl->shader.vertex_format & 0xffffcfff) | 0x800;
        } else {
            u32 colour = static_cast<u8>(mtl->diffuse.z * 255.0) | 0xffffff00;
            colour = (colour << 8) | static_cast<u8>(mtl->diffuse.y * 255.0);
            colour = (colour << 8) | static_cast<u8>(mtl->diffuse.x * 255.0);
            mtl->shader.packed_diffuse_colour = colour;
        }
        if (is_3d == 0)
            mtl->shader.vertex_format |= 0x40000;
    }

    void __cdecl NuMtlUpdatePS(NUMTL *material) {
        PC_MATERIAL_UPDATE_VIEW *mtl = reinterpret_cast<PC_MATERIAL_UPDATE_VIEW *>(material);
        PC_MATERIAL_DEVICE_CAPS_VIEW *device = reinterpret_cast<PC_MATERIAL_DEVICE_CAPS_VIEW *>(&PCDeviceBackend);
        PC_SHADER_HANDLE_VIEW *shader, *variant;
        PCFrameCriticalSectionEnter_candidate();
        if (mtl->texture_id > 0) {
            mtl->shader.diffuse_map_id = mtl->texture_id;
            if (mtl->shader.texture_unit_count <= 0)
                mtl->shader.texture_unit_count = 1;
            if ((mtl->shader.vertex_format & 0x3800) <= 0)
                mtl->shader.vertex_format = (mtl->shader.vertex_format & 0xffffcfff) | 0x800;
        }
        if ((mtl->attribs & 0x800000) != 0 && mtl->display == NULL)
            mtl->shader.vertex_format |= 0x40000;
        if (mtl->shader.shader_handle > 0)
            NuShaderManagerReleaseShader(PCShaderRecordFromHandle(mtl->shader.shader_handle));
        if (mtl->shader.shader_variant_handle > 0)
            NuShaderManagerReleaseShader(PCShaderRecordFromHandle(mtl->shader.shader_variant_handle));
        if ((mtl->attribs & 0x800000) != 0 && mtl->display == NULL) {
            shader = reinterpret_cast<PC_SHADER_HANDLE_VIEW *>(
                NuShaderManagerRetrieveShader(reinterpret_cast<const PC_SHADER_FILTER_DESC_VIEW *>(&mtl->shader),
                                              reinterpret_cast<const PC_SHADER_FILTER_MATERIAL_VIEW *>(material)));
            mtl->shader.shader_handle = shader->handle;
        } else {
            shader = reinterpret_cast<PC_SHADER_HANDLE_VIEW *>(NuShaderManagerRetrieveShaderVariant(
                reinterpret_cast<const PC_SHADER_FILTER_DESC_VIEW *>(&mtl->shader),
                reinterpret_cast<const PC_SHADER_FILTER_MATERIAL_VIEW *>(material), device->ShaderVariant16()));
            variant = reinterpret_cast<PC_SHADER_HANDLE_VIEW *>(NuShaderManagerRetrieveShaderVariant(
                reinterpret_cast<const PC_SHADER_FILTER_DESC_VIEW *>(&mtl->shader),
                reinterpret_cast<const PC_SHADER_FILTER_MATERIAL_VIEW *>(material), device->ShaderVariant1()));
            mtl->shader.shader_handle = shader->handle;
            mtl->shader.shader_variant_handle = variant->handle;
        }
        mtl->vertex_declaration = NuGetVertexDeclaration(mtl->shader.vertex_format);
        if (mtl->texture_id == 0) {
            u32 colour = static_cast<u8>(mtl->diffuse.z * 255.0) | 0xffffff00;
            colour <<= 8;
            colour |= static_cast<u8>(mtl->diffuse.y * 255.0);
            colour <<= 8;
            colour |= static_cast<u8>(mtl->diffuse.x * 255.0);
            mtl->shader.packed_diffuse_colour = colour;
        }
        PCFrameCriticalSectionLeave_candidate();
    }

    void NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v) {
        FRAME_MTL_ANIM_PC *view = (FRAME_MTL_ANIM_PC *)mtl;
        view->texture_animation_offsets[layer][0] = u;
        view->texture_animation_offsets[layer][1] = v;
    }

    void __cdecl NuMtlSetShaderDescPS(NUMTL *material, const PC_MATERIAL_SHADER_DESC_VIEW *description) {
        PC_MATERIAL_UPDATE_VIEW *mtl = reinterpret_cast<PC_MATERIAL_UPDATE_VIEW *>(material);
        i32 colour;
        mtl->shader = *description;
        colour = mtl->shader.packed_diffuse_colour;
        mtl->diffuse.x = (colour & 0xff) / 255.0;
        mtl->diffuse.y = ((colour >> 8) & 0xff) / 255.0;
        mtl->diffuse.z = ((colour >> 16) & 0xff) / 255.0;
    }

    void __cdecl NuDisplayListReflectionPc(void *arg) {
        i32 reflection = *(i32 *)arg;
        PC_DL_MATERIAL_READER *material;
        NuDisplayHandlerReflectionPc = reflection;
        material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
        if (material != NULL) {
            switch ((material->reflection_flags >> 12) & 3) {
                case 0:
                    if (reflection != 0)
                        PCDeviceSetRenderState(0x16, 2);
                    else
                        PCDeviceSetRenderState(0x16, 3);
                    break;
                case 1:
                    if (reflection != 0)
                        PCDeviceSetRenderState(0x16, 3);
                    else
                        PCDeviceSetRenderState(0x16, 2);
                    break;
            }
        }
    }

    void __cdecl NuMtlSetRenderPlanePc(NUMTL *material, i32 plane) {
    }

    void __cdecl NuLgtLaser(i32 type, f32 width, f32 segment_length, f32 width_wobble, NUVEC *start, NUVEC *delta,
                            u32 colour, f32 end_width, f32 length) {
        i32 i;
        if (NuLgtLaserCnt >= 64)
            return;

        NuLgtLaserData[NuLgtLaserCnt].arc = 0;
        NuLgtLaserData[NuLgtLaserCnt].type = (u8)type;
        NuLgtLaserData[NuLgtLaserCnt].width = width;
        NuLgtLaserData[NuLgtLaserCnt].width_wobble = width_wobble;
        NuLgtLaserData[NuLgtLaserCnt].segment_length = segment_length;
        NuLgtLaserData[NuLgtLaserCnt].end_width_ratio = end_width / width;
        NuLgtLaserData[NuLgtLaserCnt].length = length;
        NuLgtLaserData[NuLgtLaserCnt].start = *start;
        NuLgtLaserData[NuLgtLaserCnt].end.x = delta->x + start->x;
        NuLgtLaserData[NuLgtLaserCnt].end.y = delta->y + start->y;
        NuLgtLaserData[NuLgtLaserCnt].end.z = delta->z + start->z;
        NuLgtLaserData[NuLgtLaserCnt].colour = colour;
        // The original tests the arc cursor here, even for a straight laser.
        if ((NuLgtArcLaserFrame & 1) == 0 || NuLgtLaserData[NuLgtArcLaserCnt].seed == 0) {
            NuLgtLaserData[NuLgtLaserCnt].seed = NU_LGT_RAND();
        }
        for (i = 0; i < 6; ++i) {
            NU_LGT_RAND();
        }
        ++NuLgtLaserCnt;
    }

    void __cdecl NuLgtSetArcMatEx(i32 type, NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
        if (type >= 4)
            return;
        NuLgtArcMtl[type].material = material;
        NuLgtArcMtl[type].u0 = u0;
        NuLgtArcMtl[type].v0 = v0;
        NuLgtArcMtl[type].u1 = u1;
        NuLgtArcMtl[type].v1 = v1;
    }

    void __cdecl NuLgtArcLaserEx(i32 type, NUVEC *start, NUVEC *end, NUVEC *bend, f32 width, f32 segment_length,
                                 f32 wobble, f32 bend_amount, i32 colour, i32 flags) {
        i32 i;
        i32 index;
        u32 red;
        u32 green;
        u32 blue;
        u32 alpha;

        alpha = ((u32)colour >> 24) * 2;
        if (alpha > 255)
            alpha = 255;
        alpha <<= 24;
        blue = ((u8)((u32)colour >> 16)) * 2;
        if (blue > 255)
            blue = 255;
        green = ((u32)colour & 0xff00) * 2;
        if (green > 0xff00)
            green = 0xff00;
        red = ((u8)colour) * 2;
        if (red > 255)
            red = 255;

        index = NuLgtArcLaserCnt;
        if (index >= 16)
            return;

        NuLgtArcLaserData[index].type = (u8)type;
        NuLgtArcLaserData[index].start = *start;
        NuLgtArcLaserData[index].end = *end;
        NuLgtArcLaserData[index].bend = *bend;
        NuLgtArcLaserData[index].width = width;
        NuLgtArcLaserData[index].segment_length = segment_length;
        NuLgtArcLaserData[index].wobble = wobble;
        NuLgtArcLaserData[index].bend_amount = bend_amount;
        NuLgtArcLaserData[index].colour = (red << 16) | green | blue | alpha;
        NuLgtArcLaserData[index].flags = flags;
        if ((NuLgtArcLaserFrame & 1) == 0 || NuLgtArcLaserData[index].seed == 0)
            NuLgtArcLaserData[index].seed = NU_LGT_RAND();
        for (i = 0; i < 6; ++i)
            NU_LGT_RAND();
        ++NuLgtArcLaserCnt;
    }

    void __cdecl NuGScnRndr3(NUGSCN *scene) {
        NuDisplaySceneRndr(scene->display_list);
    }

    i32 __cdecl NuGScnUploadGfxDataFromFilePS(VARIPTR *cursor, VARIPTR end, NUFILE file) {
        i32 section_size, bytes_read, i;
        u32 size;
        void *data, *locked_data;
        u8 padding;
        i32 skip_texture, second_offset, third_offset, fourth_offset;
        SCENE_FILE_TEXTURE_HEADER_PC texture;
        (void)cursor;
        (void)end;
        if (SceneUploadBufferStartPC.addr == 0) {
            SceneUploadBufferStartPC.void_ptr = NuAlloc(SceneUploadBufferSizePC, SceneUploadSourcePathPC, 0x3f);
            SceneUploadBufferEndPC.addr = SceneUploadBufferStartPC.addr + SceneUploadBufferSizePC;
        }
        SceneUploadBufferCursorPC = SceneUploadBufferStartPC;
        memset(&SceneVideoResourcesPC, 0, sizeof(SceneVideoResourcesPC));
        bytes_read = NuFileRead(file, &section_size, sizeof(section_size));
        bytes_read +=
            NuFileRead(file, &SceneVideoResourcesPC.texture_count, sizeof(SceneVideoResourcesPC.texture_count));
        SceneVideoResourcesPC.textures = (PC_TEXTURE_RESOURCE_PC **)SceneUploadBufferCursorPC.void_ptr;
        SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.texture_count * sizeof(*SceneVideoResourcesPC.textures);
        memset(SceneVideoResourcesPC.textures, 0,
               SceneVideoResourcesPC.texture_count * sizeof(*SceneVideoResourcesPC.textures));
        second_offset = 8;
        third_offset = 12;
        fourth_offset = 16;
        for (i = 0; i != SceneVideoResourcesPC.texture_count; ++i) {
            bytes_read += NuFileRead(file, &texture, sizeof(texture));
            if (texture.size == 0) {
                SceneVideoResourcesPC.textures[i] = NULL;
                if (texture.width < 0) {
                    SceneVideoResourcesPC.textures[i + 1] = NULL;
                    SceneVideoResourcesPC.textures[second_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                    SceneVideoResourcesPC.textures[third_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                    SceneVideoResourcesPC.textures[fourth_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                    SceneVideoResourcesPC.textures[i + 5] = NULL;
                    i += 5;
                    second_offset += 20;
                    third_offset += 20;
                    fourth_offset += 20;
                }
            } else if (texture.width == 0) {
                NuPtlDmaSlotsExhaustedPc();
                SceneVideoResourcesPC.textures[i] = NULL;
            } else {
                skip_texture = 0;
                if (PcWindowOptions.usehires == 0 && texture.width > 0 &&
                    ((texture.flags & 0xc) != 0 || (texture.flags & 0x3c00) != 0))
                    skip_texture = 1;
                data = SceneUploadBufferCursorPC.void_ptr;
                SceneUploadBufferCursorPC.addr += texture.size;
                bytes_read += NuFileRead(file, data, texture.size);
                if (skip_texture == 0) {
                    PCFrameCriticalSectionEnter_candidate();
                    if (texture.width < 0) {
                        texture.width = -texture.width;
                        PCDeviceCreateCubeTextureFromMemory(data, texture.size, &SceneVideoResourcesPC.textures[i]);
                        SceneVideoResourcesPC.textures[i + 1] = NULL;
                        SceneVideoResourcesPC.textures[second_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                        SceneVideoResourcesPC.textures[third_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                        SceneVideoResourcesPC.textures[fourth_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                        SceneVideoResourcesPC.textures[i + 5] = NULL;
                        i += 5;
                        second_offset += 20;
                        third_offset += 20;
                        fourth_offset += 20;
                    } else {
                        PCDeviceCreateTextureFromMemoryEx(data, texture.size, texture.width, texture.height, -3, 0, 0,
                                                          1, -1, -1, 0, NULL, NULL, &SceneVideoResourcesPC.textures[i]);
                    }
                    PCFrameCriticalSectionLeave_candidate();
                    if (bgProcIsBgThread() != 0 && PCFrameLoadingYieldEnabled() != 0)
                        Sleep(10);
                }
                SceneUploadBufferCursorPC.addr -= texture.size;
            }
            second_offset += 4;
            third_offset += 4;
            fourth_offset += 4;
        }
        bytes_read += NuFileRead(file, &SceneVideoResourcesPC.vertex_buffer_count,
                                 sizeof(SceneVideoResourcesPC.vertex_buffer_count));
        SceneVideoResourcesPC.vertex_buffers = (PC_BUFFER_RESOURCE_PC **)SceneUploadBufferCursorPC.void_ptr;
        SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.vertex_buffer_count * sizeof(PC_BUFFER_RESOURCE_PC *);
        for (i = 0; i != SceneVideoResourcesPC.vertex_buffer_count; ++i) {
            bytes_read += NuFileRead(file, &size, sizeof(size));
            if (size == 0) {
                NuPtlDmaSlotsExhaustedPc();
                SceneVideoResourcesPC.vertex_buffers[i] = NULL;
            } else {
                data = SceneUploadBufferCursorPC.void_ptr;
                SceneUploadBufferCursorPC.addr += size;
                bytes_read += NuFileRead(file, data, size);
                PCFrameCriticalSectionEnter_candidate();
                PCDeviceCreateVertexBuffer(size, 0, 0, 1, &SceneVideoResourcesPC.vertex_buffers[i]);
                SceneVideoResourcesPC.vertex_buffers[i]->methods->lock(SceneVideoResourcesPC.vertex_buffers[i], 0, 0,
                                                                       &locked_data, 0);
                memcpy(locked_data, data, size);
                SceneVideoResourcesPC.vertex_buffers[i]->methods->unlock(SceneVideoResourcesPC.vertex_buffers[i]);
                PCFrameCriticalSectionLeave_candidate();
                SceneUploadBufferCursorPC.addr -= size;
            }
        }
        bytes_read += NuFileRead(file, &SceneVideoResourcesPC.index_buffer_count,
                                 sizeof(SceneVideoResourcesPC.index_buffer_count));
        SceneVideoResourcesPC.index_buffers = (PC_BUFFER_RESOURCE_PC **)SceneUploadBufferCursorPC.void_ptr;
        SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.index_buffer_count * sizeof(PC_BUFFER_RESOURCE_PC *);
        for (i = 0; i != SceneVideoResourcesPC.index_buffer_count; ++i) {
            bytes_read += NuFileRead(file, &size, sizeof(size));
            if (size == 0) {
                NuPtlDmaSlotsExhaustedPc();
                SceneVideoResourcesPC.index_buffers[i] = NULL;
            } else {
                data = SceneUploadBufferCursorPC.void_ptr;
                SceneUploadBufferCursorPC.addr += size;
                bytes_read += NuFileRead(file, data, size);
                PCFrameCriticalSectionEnter_candidate();
                PCDeviceCreateIndexBuffer(size, 0, 0x65, 1, &SceneVideoResourcesPC.index_buffers[i], NULL);
                SceneVideoResourcesPC.index_buffers[i]->methods->lock(SceneVideoResourcesPC.index_buffers[i], 0, 0,
                                                                      &locked_data, 0);
                memcpy(locked_data, data, size);
                SceneVideoResourcesPC.index_buffers[i]->methods->unlock(SceneVideoResourcesPC.index_buffers[i]);
                PCFrameCriticalSectionLeave_candidate();
                SceneUploadBufferCursorPC.addr -= size;
            }
        }
        section_size += 4;
        for (; bytes_read < section_size; ++bytes_read)
            NuFileRead(file, &padding, 1);
        return section_size;
    }

    void __cdecl NuGScnCreatePS(NUGSCN *scene, VARIPTR *cursor, VARIPTR *end) {
        i32 i;
        (void)cursor;
        (void)end;
        for (i = 0; i != scene->texture_count; ++i) {
            scene->textures[i]->resource = SceneVideoResourcesPC.textures[i];
            scene->textures[i]->image_data = NULL;
            scene->textures[i]->image_size = 0;
        }
    }

    void __cdecl NuGScnFixupTIDsPS(NUGSCN *scene) {
        i32 i, j;
        if (scene->display_list == NULL)
            return;
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->display_list->items)[i];
            if (item->type == 0xb0) {
                SCENE_TEXTURE_PACKET_B0_PC *packet = (SCENE_TEXTURE_PACKET_B0_PC *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j)
                        packet->extra_texture_ids[j] = NuGScnFixupTID(scene, packet->extra_texture_ids[j]);
                }
                packet->texture_id = NuGScnFixupTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE_PC *packet = (SCENE_TEXTURE_PACKET_AE_PC *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j)
                        packet->texture_ids[j] = NuGScnFixupTID(scene, packet->texture_ids[j]);
                }
            }
        }
    }

    void __cdecl NuGScnRestoreTIDsPS(NUGSCN *scene) {
        i32 i, j;
        if (scene->display_list == NULL)
            return;
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->display_list->items)[i];
            if (item->type == 0xb0) {
                SCENE_TEXTURE_PACKET_B0_PC *packet = (SCENE_TEXTURE_PACKET_B0_PC *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j)
                        packet->extra_texture_ids[j] = NuGScnRestoreTID(scene, packet->extra_texture_ids[j]);
                }
                packet->texture_id = NuGScnRestoreTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE_PC *packet = (SCENE_TEXTURE_PACKET_AE_PC *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j)
                        packet->texture_ids[j] = NuGScnRestoreTID(scene, packet->texture_ids[j]);
                }
            }
        }
    }

    void __cdecl NuGScnFixupPS(NUGSCN *scene) {
        SCENE_FIXUP_NATIVE_PC *native = (SCENE_FIXUP_NATIVE_PC *)scene->native_scene;
        SCENE_FIXUP_GEOMETRY_PC *geometry;
        SCENE_FIXUP_VERTEX_STREAM_PC *stream;
        u32 i;
        u8 dynamic = 0;
        i32 material_index, registry_index;
        for (i = 0; i != native->geometry_count; ++i) {
            geometry = native->geometries[i];
            geometry->index_buffer = SceneVideoResourcesPC.index_buffers[(u32)geometry->index_buffer];
            geometry->vertex_buffer = SceneVideoResourcesPC.vertex_buffers[(u32)geometry->vertex_buffer];
            if (geometry->vertex_format != 0) {
                dynamic = 1;
                PCFrameCriticalSectionEnter_candidate();
                PCDeviceCreateVertexBuffer(geometry->vertex_stride * geometry->vertex_count, 0x208, 0, 0,
                                           &geometry->dynamic_vertex_buffer);
                PCFrameCriticalSectionLeave_candidate();
            }
        }
        if (dynamic != 0) {
            for (registry_index = 0; registry_index != 100; ++registry_index) {
                if (PCRegisteredNativeScenes[registry_index] == NULL) {
                    PCRegisteredNativeScenes[registry_index] = scene;
                    break;
                }
            }
        }
        for (i = 0; i != native->vertex_stream_count; ++i) {
            stream = native->vertex_streams[i];
            stream->vertex_buffer = SceneVideoResourcesPC.vertex_buffers[(u32)stream->vertex_buffer];
        }
        native->vertex_buffer_count = SceneVideoResourcesPC.vertex_buffer_count;
        native->index_buffer_count = SceneVideoResourcesPC.index_buffer_count;
        memcpy(native->vertex_buffers, SceneVideoResourcesPC.vertex_buffers,
               SceneVideoResourcesPC.vertex_buffer_count * sizeof(*native->vertex_buffers));
        memcpy(native->index_buffers, SceneVideoResourcesPC.index_buffers,
               SceneVideoResourcesPC.index_buffer_count * sizeof(*native->index_buffers));
        for (material_index = 0; material_index < scene->material_count; ++material_index)
            NuMtlUpdate(scene->materials[material_index]);
        NuPortalMaxDepth(scene, scene->max_portals);
    }

    void __cdecl NuGScnDestroyPS(NUGSCN *scene) {
        PC_GSCN_NATIVE_VIEW *native = (PC_GSCN_NATIVE_VIEW *)scene->native_scene;
        PC_SCENE_MTL_VIEW *material;
        i32 i, j;
        u8 released;
        for (i = 0; i != native->vertex_buffer_count; ++i) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&native->vertex_buffers[i]);
            PCFrameCriticalSectionLeave_candidate();
        }
        for (i = 0; i != native->index_buffer_count; ++i) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&native->index_buffers[i]);
            PCFrameCriticalSectionLeave_candidate();
        }
        for (i = 0; i != scene->texture_count; ++i) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&scene->textures[i]->resource);
            PCFrameCriticalSectionLeave_candidate();
        }
        released = 0;
        for (i = 0; i != native->geometry_count; ++i) {
            PC_GSCN_GEOMETRY_VIEW *geometry = native->geometries[i];
            if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                PC_TEXTURE_RESOURCE_PC *resource;
                released = 1;
                PCFrameCriticalSectionEnter_candidate();
                resource = geometry->resource;
                resource->methods->release(resource);
                PCFrameCriticalSectionLeave_candidate();
            }
        }
        if (released) {
            for (i = 0; i != 100; ++i) {
                if (PCRegisteredNativeScenes[i] == scene) {
                    PCRegisteredNativeScenes[i] = NULL;
                    break;
                }
            }
        }
        material = (PC_SCENE_MTL_VIEW *)scene->materials[0];
        for (i = 0; i < scene->material_count; ++i, ++material) {
            for (j = 0; j < 2; ++j) {
                if (material->shader_handles[j] > 0) {
                    PC_SHADER_RECORD_VIEW *record = PCShaderRecordFromHandle(material->shader_handles[j]);
                    record->ref_count--;
                }
            }
        }
    }

    /* Releases the dynamic vertex buffers of every registered scene before a D3D reset. */
    void __cdecl NuGScnReleaseDynamicBuffersPc(void) {
        i32 i;
        u32 j;
        for (i = 0; i != 100; ++i) {
            NUGSCN *scene = PCRegisteredNativeScenes[i];
            if (scene != NULL) {
                SCENE_FIXUP_NATIVE_PC *native = (SCENE_FIXUP_NATIVE_PC *)scene->native_scene;
                for (j = 0; j != native->geometry_count; ++j) {
                    SCENE_FIXUP_GEOMETRY_PC *geometry = native->geometries[j];
                    if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                        PCFrameCriticalSectionEnter_candidate();
                        geometry->dynamic_vertex_buffer->methods->release(geometry->dynamic_vertex_buffer);
                        PCFrameCriticalSectionLeave_candidate();
                    }
                }
            }
        }
    }

    /* Recreates the dynamic vertex buffers released by NuGScnReleaseDynamicBuffersPc. */
    void __cdecl NuGScnRestoreDynamicBuffersPc(void) {
        i32 i;
        u32 j;
        for (i = 0; i != 100; ++i) {
            NUGSCN *scene = PCRegisteredNativeScenes[i];
            if (scene != NULL) {
                SCENE_FIXUP_NATIVE_PC *native = (SCENE_FIXUP_NATIVE_PC *)scene->native_scene;
                for (j = 0; j != native->geometry_count; ++j) {
                    SCENE_FIXUP_GEOMETRY_PC *geometry = native->geometries[j];
                    if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                        PCFrameCriticalSectionEnter_candidate();
                        PCDeviceCreateVertexBuffer(geometry->vertex_stride * geometry->vertex_count, 0x208, 0, 0,
                                                   &geometry->dynamic_vertex_buffer);
                        PCFrameCriticalSectionLeave_candidate();
                    }
                }
            }
        }
    }

    void __cdecl NuDisplayListDrawItemsPc(NUDISPLAYLISTITEM *items) {
        NuDisplayListExecute(items, PCDisplayItemHandlersPc);
    }

    i32 __cdecl DisplayListDebugPS(NUDISPLAYLISTITEM *item, char *text) {
        (void)item;
        (void)text;
        return 0;
    }

    void __cdecl NuDisplayListAddClut(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->next = NULL;
        item->id = 0;
    }

    void __cdecl NuDisplayListAddTexture(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->next = NULL;
        item->id = 0;
    }

    void __cdecl NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = 0x80;
        item->next = material;
        item->id = 3;
    }

    void __cdecl NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->next = NULL;
        item->id = 0;
    }

    void __cdecl NuDisplayListAddLightState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->next = NULL;
        item->id = 0;
    }

    void __cdecl DisplayListCreatePS(NUDISPLAY_LIST_PC *scene, VARIPTR *buffer, VARIPTR end) {
        (void)scene;
        (void)buffer;
        (void)end;
    }

    VARIPTR *__cdecl NuDisplayListLinkItem(NUDISPLAYLIST_PC *list, u8 type, void *target) {
        VARIPTR *cursor;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        ((NUPRIM_COMMAND_PC *)list->material_last)->target = cursor->void_ptr;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->type = type;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->id = 3;
        if (target != NULL)
            ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->target = target;
        else
            ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->target = (NUPRIM_COMMAND_PC *)cursor->void_ptr + 2;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)[1].type = 0x8d;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)[1].id = 1;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)[1].target = (NUPRIM_COMMAND_PC *)list->dynamic_geometry + 1;
        list->material_last = (NUPRIM_COMMAND_PC *)cursor->void_ptr + 1;
        cursor->addr += 2 * sizeof(NUPRIM_COMMAND_PC);
        return target != NULL ? NULL : cursor;
    }

    VARIPTR *__cdecl NuDisplayListLinkItems(NUDISPLAYLIST_PC *list, i32 count) {
        VARIPTR *cursor;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        ((NUDISPLAYLISTITEM *)list->material_last)->next = cursor->void_ptr;
        list->items = cursor->void_ptr;
        cursor->addr += count * sizeof(NUDISPLAYLISTITEM);
        ((NUDISPLAYLISTITEM *)cursor->void_ptr)->type = 0x8d;
        ((NUDISPLAYLISTITEM *)cursor->void_ptr)->id = 1;
        ((NUDISPLAYLISTITEM *)cursor->void_ptr)->next = (NUDISPLAYLISTITEM *)list->dynamic_geometry + 1;
        list->material_last = cursor->void_ptr;
        cursor->addr += sizeof(NUDISPLAYLISTITEM);
        return cursor;
    }

    void *__cdecl DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *geometry,
                                                   void *existing) {
        NUVEC translation, scale;
        NUMTX *result;
        (void)material;
        (void)geometry;
        (void)existing;
        NuCameraGetProjectionMtx();
        NuCameraGetViewMtx();
        scale.z = 1.0f;
        translation.z = translation.y = translation.x = 0.0f;
        scale.x = PCSpecialProjectionAdjust.x_scale;
        scale.y = PCSpecialProjectionAdjust.y_scale;
        NuMtxSetScale(&PCSpecialProjectionScale, &scale);
        translation.x = PCSpecialProjectionAdjust.x_offset;
        translation.y = PCSpecialProjectionAdjust.y_offset;
        NuMtxSetTranslation(&PCSpecialProjectionTranslate, &translation);
        buffer->addr = (buffer->addr + 3) & ~3u;
        result = (NUMTX *)buffer->void_ptr;
        PCSpecialTransposeMatrix(result, matrix);
        buffer->addr += sizeof(NUMTX);
        return result;
    }

    void *__cdecl DisplayListCreateFaceonTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *faceon) {
        NUMTX *result, *orientation;
        NUVEC direction;
        f32 magnitude;
        (void)material;
        buffer->addr = (buffer->addr + 3) & ~3u;
        result = (NUMTX *)buffer->void_ptr;
        *result = *matrix;
        magnitude = NuFsqrt((f32)(((f64)NuVecMagSqr((NUVEC *)&((NUMTX *)buffer->void_ptr)->m10) +
                                   NuVecMagSqr((NUVEC *)&((NUMTX *)buffer->void_ptr)->m00) +
                                   NuVecMagSqr((NUVEC *)&((NUMTX *)buffer->void_ptr)->m20)) *
                                  (f64)0.3333333432674408f));
        buffer->addr += sizeof(NUMTX);
        *(f32 *)buffer->void_ptr = magnitude;
        buffer->addr += sizeof(f32);
        orientation = (NUMTX *)buffer->void_ptr;
        if (((PC_SPECIAL_FACEON *)faceon)->kind == 0) {
            direction.x = direction.y = direction.z = 0.0f;
            buffer->addr += sizeof(NUMTX);
            NuMtxCalcCheapFaceOn(orientation, &direction);
        } else {
            direction.x = matrix->m10;
            direction.y = matrix->m11;
            direction.z = matrix->m12;
            buffer->addr += sizeof(NUMTX);
            NuMtxCalcCheapFaceY_v2(orientation, &direction);
        }
        return result;
    }

    void *__cdecl DisplayListCreateSkinTransformPS(VARIPTR *buffer, NUMTX *skin, void *weights, void *geometry_ptr,
                                                   void **render_geometry, void *unused) {
        PC_SPECIAL_GEOMETRY *geometry = (PC_SPECIAL_GEOMETRY *)geometry_ptr;
        i32 *matrix_count;
        u32 vertex, morph;
        u8 *destination;
        void *locked;
        PC_SPECIAL_GEOMETRY *copy;
        i32 i;
        (void)unused;
        if (weights != NULL && geometry->morph_count != 0) {
            *render_geometry = (void *)((buffer->addr + 31) & ~31u);
            buffer->addr = ((buffer->addr + 31) & ~31u) + sizeof(*copy);
            copy = (PC_SPECIAL_GEOMETRY *)*render_geometry;
            *copy = *geometry;
            locked = NULL;
            PCDeviceLockVertexBuffer(geometry->base_vertex * geometry->vertex_stride,
                                     geometry->vertex_count * geometry->vertex_stride, &locked, 0x10,
                                     geometry->vertex_buffer);
            copy->dynamic_vertices = (u8 *)((buffer->addr + 31) & ~31u);
            buffer->addr = (buffer->addr + 31) & ~31u;
            buffer->addr += geometry->vertex_count * geometry->vertex_stride;
            destination = (u8 *)copy->dynamic_vertices;
            PCByteCopy_candidate(destination, locked, geometry->vertex_count * geometry->vertex_stride);
            for (vertex = 0; vertex != geometry->vertex_count; ++vertex) {
                for (morph = 0; morph != geometry->morph_count; ++morph) {
                    f32 weight = ((PC_SPECIAL_DEFORMER_WEIGHTS *)weights)->weights[morph];
                    if (geometry->morph_deltas[morph] != NULL && weight != 0.0f) {
                        ((NUVEC *)destination)->x += geometry->morph_deltas[morph][vertex].x * weight;
                        ((NUVEC *)destination)->y += geometry->morph_deltas[morph][vertex].y * weight;
                        ((NUVEC *)destination)->z += geometry->morph_deltas[morph][vertex].z * weight;
                    }
                }
                locked = (u8 *)locked + geometry->vertex_stride;
                destination += geometry->vertex_stride;
            }
            PCDeviceUnlockVertexBuffer(geometry->vertex_buffer);
        } else {
            geometry->dynamic_vertices = NULL;
            *render_geometry = geometry;
        }
        matrix_count = (i32 *)buffer->void_ptr;
        buffer->addr += sizeof(*matrix_count);
        *matrix_count = 0;
        for (i = 0; geometry->joint_indices[i] != 255 && i < 8; ++i) {
            *(NUMTX *)buffer->void_ptr = skin[geometry->joint_indices[i]];
            buffer->addr += sizeof(NUMTX);
            ++*matrix_count;
        }
        return matrix_count;
    }

    void __cdecl DisplayListPrintItemPS(NUDISPLAYLISTITEM *item, i32 file_handle) {
        (void)item;
        (void)file_handle;
    }

    void __cdecl DisplayListSwapBuffersPS(void) {
    }

    /* Native603960 is an actual empty cross-TU operation, also empty in source. */
    void __cdecl DisplayGenerateTransformsNoOpPc(NUDISPLAY_LIST_PC *scene) {
        (void)scene;
    }

    void *__cdecl NuDisplayListPrepareFaceonPS(VARIPTR *buffer, void *faceon, NUMTX *matrix) {
        (void)buffer;
        (void)matrix;
        return faceon;
    }

    void __cdecl NuDisplaySceneAddPS(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        for (i = 0; i < scene->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->items)[i];
            if (item->type == 0x82)
                item->type = 0x82;
        }
    }

    void __cdecl NuDisplaySceneDestroyPS(NUDISPLAY_LIST_PC *scene) {
        (void)scene;
    }

    void __cdecl NuDisplaySceneClonePS(NUDISPLAY_LIST_PC *source_scene, NUDISPLAY_LIST_PC *clone_scene,
                                       VARIPTR *buffer) {
        u32 *word;
        clone_scene->native_word = NULL;
        if (source_scene->native_word != NULL) {
            word = (u32 *)((buffer->addr + 3) & ~3u);
            buffer->addr = ((buffer->addr + 3) & ~3u) + sizeof(*word);
            *word = *source_scene->native_word;
            clone_scene->native_word = word;
        }
    }

    void __cdecl NuDisplayListVertexGroupsPc(void *arg) {
        PC_DL_VERTEX_GROUPS_READER *packet = (PC_DL_VERTEX_GROUPS_READER *)arg;
        NUVEC4 *values = packet->values;
        i32 groups = (i32)((u32)packet->count + 3u) / 4;
        i32 i;
        for (i = 0; i < groups && i < 32; ++i, ++values)
            NuShaderManagerSetElementfv(0x4f, i, &values->x);
    }

    void __cdecl NuDisplayListVertexOffsetsPc(void *arg) {
        PC_DL_VERTEX_OFFSETS_READER *packet = (PC_DL_VERTEX_OFFSETS_READER *)arg;
        i32 count = packet->count;
        i32 i;
        NUVEC4 *values = packet->values;
        for (i = 0; i < count && i < 8; ++i, ++values)
            NuShaderManagerSetElementfv(0x4e, i, &values->x);
    }

    void __cdecl PostShadowSelectHandlersPc(i32 mode) {
        switch (mode) {
            case 0:
                PCDisplayItemHandlersPc = PCDisplayItemHandlerPrefixPc;
                break;
            case 1:
                PCDisplayItemHandlersPc = PCDisplayShadowItemHandlersPc;
                break;
            default:
                PCGraphicsRenderTextureAllocationError();
                break;
        }
    }

    void __cdecl DisplayListSetAlphaPS(NUDISPLAYLISTITEM *item, NUDISPLAYLISTITEM *geometry, f32 alpha) {
        NUMTX *matrix = (NUMTX *)item->next;
        (void)geometry;
        matrix->m33 = alpha < 0.0f ? 0.0f : (alpha > 1.0f ? 1.0f : alpha);
    }

    void __cdecl DisplayListSetShadowCasterFlagPS(NUDISPLAYLISTITEM *first, NUDISPLAYLISTITEM *last, i32 shadow) {
        PC_SPECIAL_TRANSFORM_PACKET *packet = (PC_SPECIAL_TRANSFORM_PACKET *)first->next;
        (void)last;
        packet->matrix.m32 = (f32)shadow;
    }

    void __cdecl NuDisplayListCameraPc(void *arg) {
        PC_DL_CAMERA_READER *packet = (PC_DL_CAMERA_READER *)arg;
        if (packet->id != NuDisplayHandlerCameraIdPc) {
            PC_VIEWPORT_PC viewport;
            NUMTX *view;
            PC_DL_CAMERA_PROJECTION_READER *projection;
            i32 viewport_x;
            view = &packet->view;
            projection = (PC_DL_CAMERA_PROJECTION_READER *)(view + 1);
            NuDisplayHandlerCameraIdPc = packet->id;
            PcUpdateGeometryCameraOriginPc(view, &projection->projection);
            viewport_x = (i32)projection->viewport[0];
            viewport.x = (u32)viewport_x;
            viewport.y = (u32)projection->viewport[1];
            viewport.width = (u32)projection->viewport[2];
            viewport.height = (u32)projection->viewport[3];
            viewport.min_z = 0.0f;
            viewport.max_z = 1.0f;
            NuVpSubmitPc(&viewport);
        }
    }

    void __cdecl PCGraphicsInitRenderTexturePool(VARIPTR *buffer, VARIPTR end) {
        i32 index;
        (void)buffer;
        (void)end;
        memset(PCGraphicsRenderTextures, 0, sizeof(PCGraphicsRenderTextures));
        for (index = 0; index != 32; ++index) {
            PCGraphicsRenderTextureActive[index] = 0;
            PCGraphicsResetRenderTexture(&PCGraphicsRenderTextures[index]);
        }
    }

    /* Claims a free render-texture slot; retail inlines it into each creator. */
    static GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsAllocRenderTexturePc(void) {
        for (i32 i = 0; i != 32; ++i) {
            if (PCGraphicsRenderTextureActive[i] == 0) {
                PCGraphicsRenderTextureActive[i] = 1;
                return &PCGraphicsRenderTextures[i];
            }
        }
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }

    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateEffectTexture(i32 width, i32 height, i32 levels, i32 usage,
                                                                      i32 format) {
        GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsAllocRenderTexturePc();
        usage &= ~0x10;
        switch (usage) {
            case 0:
                break;
            case 1:
                break;
            default:
                PCGraphicsRenderTextureAllocationError();
                return NULL;
        }
        PCGraphicsInitRenderTexture(texture, width, height, levels, usage, 0, format);
        return texture;
    }

    /* Returns the render texture behind a frame-copy texture id, or NULL for other textures. */
    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsFrameRenderTexturePc(i32 texture_id) {
        FRAME_NATIVE_TEXTURE_PC *texture = (FRAME_NATIVE_TEXTURE_PC *)NuTextureGetDimensions(texture_id);
        u32 index = (u32)(texture - FrameNativeTexturePoolPc.records);
        if (index > 15)
            return NULL;
        return texture->render_texture;
    }

    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateColourTarget(i32 divider, i32 levels) {
        i32 format = PCGraphicsFormatFromD3d(PCDeviceBackend.parameters.format);
        u32 height = PCDeviceBackend.parameters.height / (u32)divider;
        u32 width = PCDeviceBackend.parameters.width / (u32)divider;
        GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsAllocRenderTexturePc();
        PCGraphicsInitRenderTexture(texture, width, height, levels, 1, 0, format);
        texture->flags = (texture->flags & 0xffffdbffu) | 0x1b00;
        return texture;
    }

    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateColourTargetFormat(i32 divider, i32 levels, i32 format) {
        u32 height = PCDeviceBackend.parameters.height / (u32)divider;
        u32 width = PCDeviceBackend.parameters.width / (u32)divider;
        GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsAllocRenderTexturePc();
        PCGraphicsInitRenderTexture(texture, width, height, levels, 1, 0, format);
        texture->flags = (texture->flags & 0xffffdbffu) | 0x1b00;
        return texture;
    }

    void __cdecl PCGraphicsBindEffectTexturePc(u32 stage, GRAPHICS_RENDER_TEXTURE_PC *texture) {
        if (texture != NULL) {
            const u32 *tuple = &texture->flags;
            if ((u32)PCDeviceSamplerInvalidation[stage][0] != tuple[0] ||
                (u32)PCDeviceSamplerInvalidation[stage][1] != tuple[1])
                PCGraphicsSetSamplerTuplePc(stage, tuple);
            PCDeviceSetTexture(stage, texture->resource);
        } else {
            PCDeviceSetTexture(stage, NULL);
        }
    }

    PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void) {
        i32 index;
        for (index = 0; index != 32; ++index) {
            if (PcFramebufferActive[index] == 0) {
                PcFramebufferActive[index] = 1;
                PC_FRAMEBUFFER_PC *frame = &PcFramebufferPool[index];
                if (frame == NULL) {
                    PCGraphicsRenderTextureAllocationError();
                    return NULL;
                }
                for (index = 0; index != 5; ++index) {
                    frame->targets[index].Reset();
                    frame->clear_colour[index] = 0xffffffff;
                }
                frame->changed = 1;
                return frame;
            }
        }
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }

    void __cdecl PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER_PC *frame) {
        for (i32 index = 0; index != 5; ++index)
            frame->targets[index].Reset();
        for (i32 index = 0; index != 32; ++index)
            if (&PcFramebufferPool[index] == frame)
                PcFramebufferActive[index] = 0;
    }

    void __cdecl PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER_PC *frame, i32 index, GRAPHICS_RENDER_TEXTURE_PC *texture,
                                             i32 level) {
        frame->targets[index].Attach(texture, level, false);
        frame->changed = 1;
    }

    void __cdecl PCGraphicsAttachOwnedFramebuffer(PC_FRAMEBUFFER_PC *frame, i32 index,
                                                  GRAPHICS_RENDER_TEXTURE_PC *texture, i32 level) {
        frame->targets[index].Attach(texture, level, true);
    }

    void __cdecl PCGraphicsReleaseFramebuffers(void) {
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, NULL, 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, NULL, 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, NULL, 0);
        PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, NULL, 0);
    }

    void __cdecl PCGraphicsRestoreFramebuffers(void) {
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, PCDeviceBackend.textures[0], 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, PCDeviceBackend.textures[1], 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
        PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, PCDeviceBackend.textures[0], 0);
    }

    PC_EFFECT_SURFACE_PC::PC_EFFECT_SURFACE_PC() {
        texture = NULL;
        owns_texture = 0;
        surface = NULL;
        level = 0;
        field_10 = 0;
        height = width = 0;
        registry_index = -1;
        Reset();
        if (!PcSurfaceRegistryInitialized) {
            memset(PcSurfaceRegistry, 0, sizeof(PcSurfaceRegistry));
            PcSurfaceRegistryInitialized = 1;
        }
        for (i32 index = 0; index != 256; ++index)
            if (PcSurfaceRegistry[index] == NULL) {
                PcSurfaceRegistry[index] = this;
                registry_index = (i16)index;
                return;
            }
    }

    inline PC_EFFECT_SURFACE_PC::~PC_EFFECT_SURFACE_PC() {
        Reset();
        PcSurfaceRegistry[registry_index] = NULL;
    }

    u8 __cdecl PcInitializeGeometryBasisPc(PC_GEOMETRY_BASIS_SOURCE_PC *source, NUMTX *palette,
                                           PC_GEOMETRY_MORPH_WEIGHTS_PC *morph) {
        NUMTX identity;
        i32 morph_count;
        if (source->record_count > PcGeometryMatrixCapacityPc || PcGeometryTransformsPc == NULL) {
            PCClipScreenUnsupported();
            return 0;
        }
        PcGeometryCopyMatrixPc(&identity, &ANI3_EVAL_IDENTITY_MATRIX);
        morph_count = source->morph_count;
        if (morph == NULL)
            morph_count = 0;
        for (u32 i = 0; i < (u32)source->record_count; ++i) {
            PC_GEOMETRY_BASIS_INPUT_RECORD_PC *input = &source->records[i];
            if (morph_count == 0 && input->bones[0] == 0xff && input->bones[1] == 0xff && input->bones[2] == 0xff) {
                ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[0] = input->rows[0];
                ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[1] = input->rows[1];
                ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[2] = input->rows[2];
                continue;
            }
            PC_GEOMETRY_BASIS_VECTOR_PC point = input->rows[0];
            for (i32 j = 0; j < morph_count; ++j) {
                NUVEC delta;
                NuVecScale(&delta, (NUVEC *)&source->morph_deltas[j].vectors[i], morph->weight_words[j + 2]);
                NuVecAdd((NUVEC *)&point, (NUVEC *)&point, &delta);
            }
            NUMTX *matrices[3];
            matrices[0] = input->bones[0] < 0xff ? &palette[input->bones[0]] : &identity;
            matrices[1] = input->bones[1] < 0xff ? &palette[input->bones[1]] : &identity;
            matrices[2] = input->bones[2] < 0xff ? &palette[input->bones[2]] : &identity;
            PC_GEOMETRY_BASIS_VECTOR_PC t0, t1, t2;
            NuVecMtxTransform((NUVEC *)&t0, (NUVEC *)&point, matrices[0]);
            NuVecMtxTransform((NUVEC *)&t1, (NUVEC *)&point, matrices[1]);
            NuVecMtxTransform((NUVEC *)&t2, (NUVEC *)&point, matrices[2]);
            {
                PC_GEOMETRY_BASIS_VECTOR_PC p0, p1, p2, partial, sum;
                PC_GEOMETRY_SCALE(p2, t2, input->weights.z);
                PC_GEOMETRY_SCALE(p1, t1, input->weights.y);
                PC_GEOMETRY_SCALE(p0, t0, input->weights.x);
                PC_GEOMETRY_ADD(partial, p0, p1);
                PC_GEOMETRY_ADD(sum, partial, p2);
                ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[0] = sum;
            }
            PC_GEOMETRY_ROTATE(t0, input->rows[1], matrices[0])
            PC_GEOMETRY_ROTATE(t1, input->rows[1], matrices[1])
            PC_GEOMETRY_ROTATE(t2, input->rows[1], matrices[2]) {
                PC_GEOMETRY_BASIS_VECTOR_PC p0, p1, p2, partial, sum;
                PC_GEOMETRY_SCALE(p2, t2, input->weights.z);
                PC_GEOMETRY_SCALE(p1, t1, input->weights.y);
                PC_GEOMETRY_SCALE(p0, t0, input->weights.x);
                PC_GEOMETRY_ADD(partial, p0, p1);
                PC_GEOMETRY_ADD(sum, partial, p2);
                ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[1] = sum;
            }
            PC_GEOMETRY_ROTATE(t0, input->rows[2], matrices[0])
            PC_GEOMETRY_ROTATE(t1, input->rows[2], matrices[1])
            PC_GEOMETRY_ROTATE(t2, input->rows[2], matrices[2]) {
                PC_GEOMETRY_BASIS_VECTOR_PC p0, p1, p2, partial, sum;
                PC_GEOMETRY_SCALE(p2, t2, input->weights.z);
                PC_GEOMETRY_SCALE(p1, t1, input->weights.y);
                PC_GEOMETRY_SCALE(p0, t0, input->weights.x);
                PC_GEOMETRY_ADD(partial, p0, p1);
                PC_GEOMETRY_ADD(sum, partial, p2);
                ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[2] = sum;
            }
        }
        return 1;
    }

    i32 __cdecl NuRndrSetAmbientLightSpecular(const NUVEC4 *colour) {
        AMBIENT_SPECULAR_RENDER_VIEW_PC *state = (AMBIENT_SPECULAR_RENDER_VIEW_PC *)&render_state;
        state->global_specular = colour->w;
        PCGlobalLightingState.ambient = *(const NUVEC *)colour;
        NuRndrSetAmbientLightPS((const NUVEC *)colour);
        return 0;
    }

    void __cdecl DisplayListUpdateRenderState(NUDISPLAYLIST_PC *list, NUGLOBALRNDRSTATE_PC *global) {
        void *packet;

        if (global == NULL) {
            return;
        }
        if (((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->global_id == global->counter_0020) {
            return;
        }

        if ((u16)((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->lights_id != global->lights_id) {
            if (global->light_packet_cache == NULL) {
                global->light_packet_cache = DisplayListBuildLightPacket(global);
            }
            NuDisplayListLinkItem(list, 0x94, global->light_packet_cache);
            ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->lights_id = global->lights_id;
        }

        if ((u16)((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->camera_id != global->camera_id) {
            if (global->camera_packet_cache == NULL) {
                NUMTX projection;
                VARIPTR *cursor;
                f32 *viewport;
                memset(&projection.m01, 0, sizeof(projection) - sizeof(projection.m00));
                projection.m00 = global->projection_0154;
                projection.m11 = global->projection_0158;
                projection.m22 = global->projection_015c;
                projection.m23 = global->projection_0160;
                projection.m32 = global->projection_0164;
                NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
                cursor = NuDisplayListStreamCursor;
                global->camera_packet_cache = cursor->void_ptr;
                *(i32 *)cursor->void_ptr =
                    ((i32)(i16)global->camera_id + 5) * (global->counter_0020 + 13) + nuapi.frame_count;
                cursor->addr += sizeof(i32);
                *(NUMTX *)cursor->void_ptr = global->camera_matrix;
                cursor->addr += sizeof(NUMTX);
                *(NUMTX *)cursor->void_ptr = projection;
                cursor->addr += sizeof(NUMTX);
                viewport = (f32 *)cursor->void_ptr;
                *viewport++ = global->viewport_x;
                cursor->void_ptr = viewport;
                *viewport++ = global->viewport_y;
                cursor->void_ptr = viewport;
                *viewport++ = global->viewport_width;
                cursor->void_ptr = viewport;
                *viewport++ = global->viewport_height;
                cursor->void_ptr = viewport;
            }
            NuDisplayListLinkItem(list, 0x9a, global->camera_packet_cache);
            ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->camera_id = global->camera_id;
        }

        if ((u16)((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->fog_id != global->fog_id) {
            if (global->fog_packet_cache == NULL) {
                global->fog_packet_cache = DisplayListBuildFogPacket(global);
            }
            NuDisplayListLinkItem(list, 0xa6, global->fog_packet_cache);
            ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->fog_id = global->fog_id;
        }

        if ((u16)((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->konst_id != global->counter_001e) {
            if (global->konst_packet_cache == NULL) {
                global->konst_packet_cache = DisplayListBuildKonstPacket(global);
            }
            NuDisplayListLinkItem(list, 0xa5, global->konst_packet_cache);
            ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->konst_id = global->counter_001e;
        }

        if ((u16)((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->reflection_id != global->reflection_id) {
            if (global->reflection_packet_cache == NULL) {
                global->reflection_packet_cache = DisplayListBuildReflectionPacket(global);
            }
            NuDisplayListLinkItem(list, 0xab, global->reflection_packet_cache);
            ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->reflection_id = global->reflection_id;
        }

        if ((u16)((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->vertex_groups_id != global->vertex_groups_id) {
            packet = DisplayListBuildVertexGroupPacket(global);
            if (packet != NULL) {
                NuDisplayListLinkItem(list, 0xa9, packet);
            }
            ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->vertex_groups_id = global->vertex_groups_id;
        }

        ((NUPRIM_RENDER_STATE_IDS_PC *)list->render_state)->global_id = global->counter_0020;
    }

    void __cdecl DisplayListUpdateRenderStateShadow(NUDISPLAYLIST_PC *list, NUGLOBALRNDRSTATE_PC *state) {
        NUGLOBALRNDRSTATE_PC *previous;
        if (state == NULL)
            return;
        previous = (NUGLOBALRNDRSTATE_PC *)list->render_state;
        if (previous->counter_0020 == state->counter_0020)
            return;
        if (previous->vertex_groups_id != state->vertex_groups_id) {
            void *packet = DisplayListBuildVertexGroupPacket(state);
            if (packet != NULL)
                NuDisplayListLinkItem(list, 0xa9, packet);
            previous = (NUGLOBALRNDRSTATE_PC *)list->render_state;
            previous->vertex_groups_id = state->vertex_groups_id;
        }
        previous = (NUGLOBALRNDRSTATE_PC *)list->render_state;
        previous->counter_0020 = state->counter_0020;
    }

    void __cdecl NuDisplayListDrawGeometryPc(void *arg) {
        PC_DL_GEOMETRY_READER *packet = (PC_DL_GEOMETRY_READER *)arg;
        PC_DL_GEOMETRY_MATERIAL_READER *material = (PC_DL_GEOMETRY_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
        u32 primitive_count;
        const void *vertices;
        void *locked;
        u32 point_size;
        f32 point_size_float;
        if (packet == NULL || PCShaderCurrentObjectPc() == NULL || packet->vertex_count == 0)
            return;
        primitive_count = packet->primitive_count;
        if (material->lightmap_flags & 0x4000000)
            NuDisplayListUpdateLightmapPc();
        vertices = packet + 1;
        switch (packet->kind) {
            case 6:
                if (packet->dynamic_count != 0 && packet->dynamic_vertices != NULL) {
                    PCDeviceLockVertexBuffer(0, 0, &locked, 0x2000, packet->dynamic_buffer);
                    PCByteCopy_candidate(locked, packet->dynamic_vertices,
                                         packet->vertex_stride * packet->vertex_count);
                    PCDeviceUnlockVertexBuffer(packet->dynamic_buffer);
                    PCDeviceSetStreamSourcePc(0, packet->dynamic_buffer, 0, packet->vertex_stride);
                    PCDeviceSetIndicesPc(packet->index_buffer);
                    SAGA_SKIN_CALL(skin_trace_capture_geometry, (unsigned)packet, (unsigned)NuDisplayHandlerMaterialPc,
                                   PCShaderSlots, (unsigned)packet->dynamic_buffer, (unsigned)packet->index_buffer, 0,
                                   packet->start_index);
                    PCDeviceDrawIndexedPrimitivePc(5, 0, 0, packet->vertex_count, packet->start_index, primitive_count);
                } else {
                    PCDeviceSetStreamSourcePc(0, packet->vertex_buffer, 0, packet->vertex_stride);
                    PCDeviceSetIndicesPc(packet->index_buffer);
                    SAGA_SKIN_CALL(skin_trace_capture_geometry, (unsigned)packet, (unsigned)NuDisplayHandlerMaterialPc,
                                   PCShaderSlots, (unsigned)packet->vertex_buffer, (unsigned)packet->index_buffer,
                                   packet->base_vertex, packet->start_index);
                    PCDeviceDrawIndexedPrimitivePc(5, packet->base_vertex, 0, packet->vertex_count, packet->start_index,
                                                   primitive_count);
                }
                break;
            case 12:
                PCDeviceDrawPrimitiveUPPc(4, packet->vertex_count / 3u, vertices, 24);
                break;
            case 13:
                if (packet->vertex_count >= 3u)
                    PCDeviceDrawPrimitiveUPPc(5, packet->vertex_count - 2u, vertices, 24);
                break;
            case 14:
                PCDeviceDrawPrimitiveUPPc(2, packet->vertex_count >> 1, vertices, 24);
                break;
            case 15:
                PCDeviceDrawPrimitiveUPPc(3, packet->vertex_count - 1u, vertices, 24);
                break;
            case 17:
                if (packet->vertex_count >= 3u)
                    PCDeviceDrawPrimitiveUPPc(6, packet->vertex_count - 2u, vertices, 24);
                break;
            case 18:
                point_size_float = 32.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(0x9a, point_size);
                PCDeviceDrawPrimitiveUPPc(1, packet->vertex_count, vertices, 24);
                point_size_float = 1.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(0x9a, point_size);
                break;
        }
    }

    void __cdecl NuVpInit(void) {
        NuPs2GetViewport(&vpDevice);
        vpDevice.center_x = 0.5f;
        vpDevice.center_y = 0.5f;
        vpDevice.clip_min_x = 0.0f;
        vpDevice.clip_min_y = 0.0f;
        vpDevice.clip_max_x = 1.0f;
        vpDevice.clip_max_y = 1.0f;
        vpDevice.height *= 16;
        vpDevice.width *= 16;
        vpDevice.x *= 16;
        vpDevice.y *= 16;
        vpCurrent = vpDevice;
        NuVpDirtyPc = 1;
        NuVpUpdate();
    }

    void __cdecl NuInitDebrisRenderer(VARIPTR *buffer, VARIPTR buffer_end) {
        i32 index;
        (void)buffer_end;
        NuDebrisMaxVertexCount = NuDebrisIdealDynamicVBSizePc / 24u;
        NuDebrisVertexBufferSize = NuDebrisMaxVertexCount * 24u;
        for (index = 0; index != 8; ++index) {
            NuDebrisSystemBuffers[index] = buffer->void_ptr;
            buffer->addr += NuDebrisVertexBufferSize;
        }
        NuDebrisCreateVertexBuffersPc();
    }

    void __cdecl AddParticleGroupToDisplayList(NATIVE_PARTICLE_PACKET_PC *group) {
        NUDISPLAYLIST_PC *list = group->material->display_list;
        VARIPTR *buffer;
        list->scene->render_flags |= 2;
        (&list->scene->material_used_front)[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |=
            (u8)(1 << (list->material_id % 8));
        DisplayListUpdateRenderState(list, &render_state);
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
        buffer = NuDisplayListStreamCursor;
        ((NUDISPLAYLISTITEM *)list->material_last)->next = buffer->void_ptr;
        list->items = buffer->void_ptr;
        buffer->addr += sizeof(NUDISPLAYLISTITEM);
        ((NUDISPLAYLISTITEM *)buffer->void_ptr)->type = 0x8d;
        ((NUDISPLAYLISTITEM *)buffer->void_ptr)->id = 1;
        ((NUDISPLAYLISTITEM *)buffer->void_ptr)->next = (NUDISPLAYLISTITEM *)list->dynamic_geometry + 1;
        list->material_last = buffer->void_ptr;
        buffer->addr += sizeof(NUDISPLAYLISTITEM);
        ((NUDISPLAYLISTITEM *)list->items)->type = 0xa7;
        ((NUDISPLAYLISTITEM *)list->items)->id = 3;
        ((NUDISPLAYLISTITEM *)list->items)->next = group;
        list->items = (NUDISPLAYLISTITEM *)list->items + 1;
    }

    void __cdecl NuPrim2DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material) {
        NUDISPLAYLIST_PC *list;
        VARIPTR *primitive_buffer, *cursor;
        NUPRIM_PACKET_PC *packet;

        (void)vertex_format;
        if (material == NULL) {
            material = numtl_defaultmtl2d;
        }

        NuPrimNeedsOverbrightening = material->texture_id != 0;

        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        list = material->display_list;
        primitive_buffer = NuDisplayListStreamCursor;
        if (list != NULL) {
            list->scene->render_flags |= 2;
            ((NUPRIM_SCENE_USAGE_PC *)list->scene)
                ->material_used[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |=
                (u8)(1 << (list->material_id % 8));
        } else {
            list = (NUDISPLAYLIST_PC *)&PCDisplayList2dPc;
            NuDisplayListLinkMtl(list, material);
        }

        render_state.counter_0020++;
        render_state.counter_001e++;
        render_state.state_00f8 = 0;
        render_state.const_alpha_material = NULL;
        render_state.state_00f4 = 0;
        render_state.konst_packet_cache = NULL;
        DisplayListUpdateRenderState(list, &render_state);

        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        ((NUPRIM_COMMAND_PC *)list->material_last)->target = cursor->void_ptr;
        list->items = cursor->void_ptr;
        cursor->addr += sizeof(NUPRIM_COMMAND_PC);
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->type = 0x8d;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->id = 1;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->target = (NUPRIM_COMMAND_PC *)list->dynamic_geometry + 1;
        list->material_last = cursor->void_ptr;
        cursor->addr += sizeof(NUPRIM_COMMAND_PC);
        NuPrimStreamBufferPointer = primitive_buffer;
        packet = (NUPRIM_PACKET_PC *)primitive_buffer->void_ptr;
        primitive_buffer->void_ptr = packet + 1;
        packet->primitive_type = primitive_type;
        NuPrimVertexCount = 0;
        NuPrimVertexCountPointer = &packet->vertex_count;
        ((NUPRIM_COMMAND_PC *)list->items)->type = 0x93;
        NuPrimCurrentPrimitiveType = (i32)primitive_type;
        ((NUPRIM_COMMAND_PC *)list->items)->id = 3;
        ((NUPRIM_COMMAND_PC *)list->items)->target = packet;
        list->items = (NUPRIM_COMMAND_PC *)list->items + 1;
    }

    void __cdecl NuPrim3DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material, NUMTX *world_matrix) {
        NUDISPLAYLIST_PC *list;
        VARIPTR *cursor, *primitive_buffer;
        void *transform;
        NUPRIM_PACKET_PC *geometry;
        (void)vertex_format;
        if (material == NULL)
            material = numtl_defaultmtl3d;
        NuPrimNeedsOverbrightening = material->texture_id != 0;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        list = material->display_list;
        if (list != NULL) {
            list->scene->render_flags |= 2;
            ((NUPRIM_SCENE_USAGE_PC *)list->scene)
                ->material_used[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |=
                (u8)(1 << (list->material_id % 8));
        } else {
            list = (NUDISPLAYLIST_PC *)&PCDisplayList2dPc;
        }
        DisplayListUpdateRenderState(list, &render_state);
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        ((NUPRIM_COMMAND_PC *)list->material_last)->target = cursor->void_ptr;
        list->items = cursor->void_ptr;
        cursor->addr += 2 * sizeof(NUPRIM_COMMAND_PC);
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->type = 0x8d;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->id = 1;
        ((NUPRIM_COMMAND_PC *)cursor->void_ptr)->target = (NUPRIM_COMMAND_PC *)list->dynamic_geometry + 1;
        list->material_last = cursor->void_ptr;
        cursor->addr += sizeof(NUPRIM_COMMAND_PC);
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        primitive_buffer = NuDisplayListStreamCursor;
        if (world_matrix == NULL)
            world_matrix = &numtx_identity;
        transform = DisplayListCreateGeomTransformPS(primitive_buffer, world_matrix, NULL, NULL, NULL);
        geometry = (NUPRIM_PACKET_PC *)primitive_buffer->void_ptr;
        primitive_buffer->void_ptr = geometry + 1;
        geometry->primitive_type = primitive_type;
        NuPrimVertexCountPointer = &geometry->vertex_count;
        ((NUPRIM_COMMAND_PC *)list->items)->type = 0x8c;
        ((NUPRIM_COMMAND_PC *)list->items)->id = 3;
        ((NUPRIM_COMMAND_PC *)list->items)->target = transform;
        list->items = (NUPRIM_COMMAND_PC *)list->items + 1;
        ((NUPRIM_COMMAND_PC *)list->items)->type = 0x82;
        ((NUPRIM_COMMAND_PC *)list->items)->id = 3;
        ((NUPRIM_COMMAND_PC *)list->items)->target = geometry;
        list->items = (NUPRIM_COMMAND_PC *)list->items + 1;
    }

    void __cdecl NuLgtLaserDraw(i32 paused) {
        i32 i;
        i32 k;
        f32 tex_v[4];
        f32 tex_u[4];
        u32 saved_seed;
        NUVEC4 *vertices;
        NUMTX clip_matrix;
        f32 aspect;
        u32 colour;
        i32 red;
        i32 green;
        i32 blue;
        i32 alpha;
        i32 reds[2];
        i32 greens[2];
        i32 blues[2];
        i32 alphas[2];
        f32 dx;
        f32 dy;
        f32 dz;
        f32 remaining;
        NUVEC perpendicular;
        f32 length;
        f32 step;
        f32 width_scale;
        f32 fraction;
        f64 next_fraction;
        f64 wobble_limit;
        f32 offset_x;
        f32 offset_y;
        f32 wobble_x;
        f32 wobble_y;
        f32 neg_y;
        NULGTVERTEX_PC *vertex;
        if (paused)
            NuLgtLaserCnt = NuLgtLaserOldCnt;
        if (NuLgtArcMtl[0].material == NULL || NuLgtLaserCnt == 0) {
            NuLgtLaserOldCnt = 0;
            return;
        }

        tex_v[0] = NuLgtArcMtl[0].v0;
        tex_v[1] = NuLgtArcMtl[0].v0;
        tex_v[2] = NuLgtArcMtl[0].v1;
        tex_v[3] = NuLgtArcMtl[0].v1;
        saved_seed = NuLgtSeed;
        // Four strip corners, two projected endpoints, and two clipped world endpoints.
        vertices = (NUVEC4 *)NuScratchAlloc32(0x80);
        NuCameraGetClipMtx(&clip_matrix, NULL);
        aspect = (f32)PS2_REZ_W / PS2_REZ_H;

        for (i = 0; i < NuLgtLaserCnt; ++i) {
            vertices[4].x = NuLgtLaserData[i].start.x;
            vertices[4].y = NuLgtLaserData[i].start.y;
            vertices[4].z = NuLgtLaserData[i].start.z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
            vertices[5].x = NuLgtLaserData[i].end.x;
            vertices[5].y = NuLgtLaserData[i].end.y;
            vertices[5].z = NuLgtLaserData[i].end.z;
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
            if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f))
                continue;

            if (vertices[4].w < 0.5f) {
                if (NuLgtLaserData[i].arc == 0) {
                    colour = NuLgtLaserData[i].colour;
                    red = colour & 0xff;
                    green = (colour >> 8) & 0xff;
                    blue = (u8)(colour >> 16);
                    alpha = (u8)(colour >> 24);
                } else {
                    red = (i32)((i32)(NuLgtLaserData[i].colour & 0xff) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w));
                    green = (i32)((((i32)NuLgtLaserData[i].colour >> 8) & 0xff) * (vertices[5].w - 0.5) /
                                  (vertices[5].w - vertices[4].w));
                    blue = (i32)((((i32)NuLgtLaserData[i].colour >> 16) & 0xff) * (vertices[5].w - 0.5) /
                                 (vertices[5].w - vertices[4].w));
                    alpha = (i32)((((i32)NuLgtLaserData[i].colour >> 24) & 0xff) * (vertices[5].w - 0.5) /
                                  (vertices[5].w - vertices[4].w));
                }
                vertices[6].x = (NuLgtLaserData[i].start.x - NuLgtLaserData[i].end.x) * (vertices[5].w - 0.5) /
                                    (vertices[5].w - vertices[4].w) +
                                NuLgtLaserData[i].end.x;
                vertices[6].y = (NuLgtLaserData[i].start.y - NuLgtLaserData[i].end.y) * (vertices[5].w - 0.5) /
                                    (vertices[5].w - vertices[4].w) +
                                NuLgtLaserData[i].end.y;
                vertices[6].z = (NuLgtLaserData[i].start.z - NuLgtLaserData[i].end.z) * (vertices[5].w - 0.5) /
                                    (vertices[5].w - vertices[4].w) +
                                NuLgtLaserData[i].end.z;
            } else {
                vertices[6].x = NuLgtLaserData[i].start.x;
                vertices[6].y = NuLgtLaserData[i].start.y;
                vertices[6].z = NuLgtLaserData[i].start.z;
                colour = NuLgtLaserData[i].colour;
                red = colour & 0xff;
                green = (colour >> 8) & 0xff;
                blue = (u8)(colour >> 16);
                alpha = (u8)(colour >> 24);
            }
            vertices[6].w = 1.0f;

            remaining = (vertices[6].x - NuLgtLaserData[i].end.x) * (vertices[6].x - NuLgtLaserData[i].end.x) +
                        (vertices[6].y - NuLgtLaserData[i].end.y) * (vertices[6].y - NuLgtLaserData[i].end.y) +
                        (vertices[6].z - NuLgtLaserData[i].end.z) * (vertices[6].z - NuLgtLaserData[i].end.z);
            remaining = NuFsqrt(remaining);
            if (vertices[5].w < 0.5f) {
                vertices[7].x = (NuLgtLaserData[i].end.x - NuLgtLaserData[i].start.x) * (vertices[4].w - 0.5) /
                                    (vertices[4].w - vertices[5].w) +
                                NuLgtLaserData[i].start.x;
                vertices[7].y = (NuLgtLaserData[i].end.y - NuLgtLaserData[i].start.y) * (vertices[4].w - 0.5) /
                                    (vertices[4].w - vertices[5].w) +
                                NuLgtLaserData[i].start.y;
                vertices[7].z = (NuLgtLaserData[i].end.z - NuLgtLaserData[i].start.z) * (vertices[4].w - 0.5) /
                                    (vertices[4].w - vertices[5].w) +
                                NuLgtLaserData[i].start.z;
            } else {
                vertices[7].x = NuLgtLaserData[i].end.x;
                vertices[7].y = NuLgtLaserData[i].end.y;
                vertices[7].z = NuLgtLaserData[i].end.z;
            }
            vertices[7].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[6], &clip_matrix);
            NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
            NuVec4MtxTransformVU0(&vertices[5], &vertices[7], &clip_matrix);
            NuVec4ScaleForwardPc(&vertices[5], &vertices[5], 1.0f / vertices[5].w);
            if (vertices[4].x < -1.0f && vertices[5].x < -1.0f)
                continue;
            if (vertices[4].x > 1.0f && vertices[5].x > 1.0f)
                continue;
            if (vertices[4].y < -1.0f && vertices[5].y < -1.0f)
                continue;
            if (vertices[4].y > 1.0f && vertices[5].y > 1.0f)
                continue;

            perpendicular.x = (vertices[5].y - vertices[4].y) * aspect;
            perpendicular.y = vertices[4].x - vertices[5].x;
            perpendicular.z = 0.0f;
            NuVecNorm(&perpendicular, &perpendicular);
            perpendicular.x *= aspect * NuLgtLaserData[i].width;
            perpendicular.y = NuLgtLaserData[i].width * perpendicular.y;
            dx = vertices[7].x - vertices[6].x;
            vertices[7].x = dx;
            dy = vertices[7].y - vertices[6].y;
            vertices[7].y = dy;
            dz = vertices[7].z - vertices[6].z;
            vertices[7].z = dz;
            vertices[7].w -= vertices[6].w;
            length = dx * dx + dy * dy + dz * dz;
            length = NuFsqrt(length);
            if (!(length > 0.0f))
                continue;

            step = NuLgtLaserData[i].segment_length / length;
            // Per-laser seeds make repeated and paused rendering deterministic.
            NuLgtSeed = NuLgtLaserData[i].seed;
            if (step > 1.0f)
                step = 1.0f;

            tex_u[0] = tex_u[2] = NuLgtArcMtl[0].u0;
            tex_u[1] = tex_u[3] = NuLgtArcMtl[0].u1;
            vertices[4].x = vertices[6].x;
            vertices[4].y = vertices[6].y;
            vertices[4].z = vertices[6].z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
            width_scale = 1.0f / vertices[4].w;
            NuVec4ScaleForwardPc(&vertices[4], &vertices[4], width_scale);
            if (remaining < NuLgtLaserData[i].length) {
                if (remaining < 0.0f)
                    width_scale *= NuLgtLaserData[i].end_width_ratio;
                else
                    width_scale += (NuLgtLaserData[i].end_width_ratio * width_scale - width_scale) *
                                   (NuLgtLaserData[i].length - remaining) / NuLgtLaserData[i].length;
            }
            reds[0] = reds[1] = red;
            greens[0] = greens[1] = green;
            blues[0] = blues[1] = blue;
            alphas[0] = alphas[1] = alpha;
            offset_x = width_scale * perpendicular.x;
            offset_y = width_scale * perpendicular.y;
            vertices[1].x = vertices[4].x - offset_x;
            vertices[1].y = vertices[4].y - offset_y;
            vertices[3].x = vertices[4].x + offset_x;
            vertices[3].y = vertices[4].y + offset_y;
            vertices[1].z = vertices[4].z;
            vertices[3].z = vertices[4].z;
            remaining -= NuLgtLaserData[i].segment_length;

            fraction = 0.0f;
            wobble_limit = 1.0 - 1.5 * step;
            while (fraction < 1.0f) {
                next_fraction = fraction + step;
                if (next_fraction >= 1.0f) {
                    if (NuLgtLaserData[i].arc)
                        alphas[1] = 0;
                    vertices[5].x = vertices[7].x + vertices[6].x;
                    vertices[5].y = vertices[6].y + vertices[7].y;
                    vertices[5].z = vertices[7].z + vertices[6].z;
                    tex_u[0] = tex_u[2] =
                        NuLgtArcMtl[0].u1 + (1.0f - fraction) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
                } else {
                    if (NuLgtLaserData[i].arc)
                        alphas[1] = (i32)((1.0f - fraction - step) * alpha);
                    vertices[5].x = vertices[7].x * next_fraction + vertices[6].x;
                    vertices[5].y = vertices[7].y * next_fraction + vertices[6].y;
                    vertices[5].z = vertices[7].z * next_fraction + vertices[6].z;
                }
                vertices[5].w = 1.0f;
                NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
                width_scale = 1.0f / vertices[5].w;
                NuVec4ScaleForwardPc(&vertices[5], &vertices[5], width_scale);
                if (remaining < NuLgtLaserData[i].length) {
                    if (remaining < 0.0f)
                        width_scale *= NuLgtLaserData[i].end_width_ratio;
                    else
                        width_scale += (width_scale * NuLgtLaserData[i].end_width_ratio - width_scale) *
                                       (NuLgtLaserData[i].length - remaining) / NuLgtLaserData[i].length;
                }

                if (fraction < wobble_limit && NuLgtLaserData[i].seed != 0) {
                    wobble_y = (f32)((NU_LGT_RAND() & 0xff) - 128);
                    wobble_x = wobble_y * perpendicular.x * NuLgtLaserData[i].width_wobble * width_scale;
                    wobble_y = wobble_y * perpendicular.y * NuLgtLaserData[i].width_wobble * width_scale;
                } else {
                    wobble_x = wobble_y = 0.0f;
                }
                offset_x = width_scale * perpendicular.x;
                offset_y = width_scale * perpendicular.y;
                vertices[0].x = vertices[5].x - offset_x + wobble_x;
                vertices[0].y = vertices[5].y - offset_y + wobble_y;
                vertices[2].x = vertices[5].x + offset_x + wobble_x;
                vertices[2].y = vertices[5].y + offset_y + wobble_y;
                vertices[0].z = vertices[5].z;
                vertices[2].z = vertices[5].z;
                // NuPrimSetCoordinateSystem(1), pushed.
                NuPrimCoordSystemStack[NuPrimCSPos++] = 1;
                NuPrimXScale = 1.0f;
                NuPrimYScale = -1.0f;
                NuPrimXBias = 0.0f;
                NuPrimYBias = 0.0f;
                NuPrim2DBegin(13, 7, NuLgtArcMtl[0].material);
                for (k = 0; k < 4; ++k) {
                    NuLgtSetVertexColor((alphas[1 - (k & 1)] << 24) | ((blues[1 - (k & 1)] & 0xff) << 16) |
                                        ((greens[1 - (k & 1)] & 0xff) << 8) | (reds[1 - (k & 1)] & 0xff));
                    vertex = (NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
                    vertex->u = tex_u[k];
                    vertex->v = tex_v[k];
                    neg_y = vertices[k].y * -1.0;
                    NuPrim2DAddXYZ(vertices[k].x, neg_y, vertices[k].z);
                }
                // NuPrim2DEnd.
                *NuPrimVertexCountPointer = NuPrimVertexCount;
                NuPrimVertexCount = 0;
                NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
                vertices[1].x = vertices[0].x;
                vertices[1].y = vertices[0].y;
                vertices[1].z = vertices[0].z;
                vertices[3].x = vertices[2].x;
                vertices[3].y = vertices[2].y;
                vertices[3].z = vertices[2].z;
                alphas[0] = alphas[1];
                fraction = (f32)next_fraction;
                remaining -= NuLgtLaserData[i].segment_length;
            }
        }
        NuLgtSeed = saved_seed;
        NuLgtLaserOldCnt = NuLgtLaserCnt;
        NuLgtLaserCnt = 0;
        NuScratchRelease();
    }

    void __cdecl NuLgtInitializeStatePc(void) {
        i32 i;
        NuLgtStateUnknownPc = 0;
        NuLgtStateCapacityPc = 8;
        memset(NuLgtStateSlotsPc, 0, sizeof(NuLgtStateSlotsPc));
        for (i = 0; i < 8; ++i) {
            NuLgtStateSlotsPc[i].previous = i - 1;
            NuLgtStateSlotsPc[i].next = i + 1;
        }
        NuLgtStateSlotsPc[7].next = -1;
        NuLgtStateActiveCountPc = 0;
        NuLgtArcLaserCnt = 0;
        NuLgtArcLaserOldCnt = 0;
        NuLgtStateFreeHeadPc = -1;
        NuLgtStateEnabledPc = 1;
    }

    void __cdecl NuLgtSetArcMat(NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
        NuLgtArcMtl[0].material = material;
        NuLgtArcMtl[0].u0 = u0;
        NuLgtArcMtl[0].v0 = v0;
        NuLgtArcMtl[0].u1 = u1;
        NuLgtArcMtl[0].v1 = v1;
    }

    void __cdecl NuLgtArcLaserDraw(i32 paused) {
        i32 i;
        f32 tex_v[4];
        NULGTVERTEX_PC *vertex;
        NUVEC4 *vertices;
        NUMTX *clip;
        NUVEC perpendicular;
        f32 length;
        f32 factor;
        f32 step;
        f32 u;
        f32 u1;
        f32 progress;
        f64 next;
        f64 wobble_limit;
        i32 angle;
        f32 *trig;
        f32 reciprocal;
        f32 jitter_x;
        f32 jitter_y;
        f32 offset_x;
        f32 offset_y;
        f32 corner_x;
        f32 corner_y;
        tex_v[0] = NuLgtArcMtl[0].v0;
        tex_v[1] = NuLgtArcMtl[0].v0;
        tex_v[2] = NuLgtArcMtl[0].v1;
        tex_v[3] = NuLgtArcMtl[0].v1;
        if (paused)
            NuLgtArcLaserCnt = NuLgtArcLaserOldCnt;
        if (!NuLgtArcMtl[0].material || NuLgtArcLaserCnt == 0) {
            NuLgtArcLaserOldCnt = 0;
            return;
        }

        vertices = (NUVEC4 *)NuScratchAlloc32(0xc0);

        clip = NuCameraGetVPCSMtx();
        if (!NuRndrBeginSceneEx(1, 2, 0)) {
            NuScratchRelease();
            return;
        }
        // NuPrimSetCoordinateSystem(0), pushed.
        NuPrimXScale = NuPrimPS2XScale;
        NuPrimYScale = NuPrimPS2YScale;
        NuPrimXBias = -1.0f;
        NuPrimYBias = 1.0f;
        NuPrimCoordSystemStack[NuPrimCSPos++] = 0;
        NuPrim2DBegin(12, 5, NuLgtArcMtl[0].material);
        for (i = 0; i < NuLgtArcLaserCnt; ++i) {

            vertices[4].x = NuLgtArcLaserData[i].start.x;
            vertices[4].y = NuLgtArcLaserData[i].start.y;
            vertices[4].z = NuLgtArcLaserData[i].start.z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
            vertices[5].x = NuLgtArcLaserData[i].end.x;
            vertices[5].y = NuLgtArcLaserData[i].end.y;
            vertices[5].z = NuLgtArcLaserData[i].end.z;
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);
            if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f))
                continue;
            if (vertices[4].w < 0.5f) {
                vertices[6].x = (NuLgtArcLaserData[i].start.x - NuLgtArcLaserData[i].end.x) * (vertices[5].w - 0.5) /
                                    (vertices[5].w - vertices[4].w) +
                                NuLgtArcLaserData[i].end.x;
                vertices[6].y = (NuLgtArcLaserData[i].start.y - NuLgtArcLaserData[i].end.y) * (vertices[5].w - 0.5) /
                                    (vertices[5].w - vertices[4].w) +
                                NuLgtArcLaserData[i].end.y;
                vertices[6].z = (NuLgtArcLaserData[i].start.z - NuLgtArcLaserData[i].end.z) * (vertices[5].w - 0.5) /
                                    (vertices[5].w - vertices[4].w) +
                                NuLgtArcLaserData[i].end.z;
            } else {
                vertices[6].x = NuLgtArcLaserData[i].start.x;
                vertices[6].y = NuLgtArcLaserData[i].start.y;
                vertices[6].z = NuLgtArcLaserData[i].start.z;
            }
            vertices[6].w = 1.0f;
            if (vertices[5].w < 0.5f) {
                vertices[7].x = (NuLgtArcLaserData[i].end.x - NuLgtArcLaserData[i].start.x) * (vertices[4].w - 0.5) /
                                    (vertices[4].w - vertices[5].w) +
                                NuLgtArcLaserData[i].start.x;
                vertices[7].y = (NuLgtArcLaserData[i].end.y - NuLgtArcLaserData[i].start.y) * (vertices[4].w - 0.5) /
                                    (vertices[4].w - vertices[5].w) +
                                NuLgtArcLaserData[i].start.y;
                vertices[7].z = (NuLgtArcLaserData[i].end.z - NuLgtArcLaserData[i].start.z) * (vertices[4].w - 0.5) /
                                    (vertices[4].w - vertices[5].w) +
                                NuLgtArcLaserData[i].start.z;
            } else {
                vertices[7].x = NuLgtArcLaserData[i].end.x;
                vertices[7].y = NuLgtArcLaserData[i].end.y;
                vertices[7].z = NuLgtArcLaserData[i].end.z;
            }
            vertices[7].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[6], clip);
            NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
            NuVec4MtxTransformVU0(&vertices[5], &vertices[7], clip);
            NuVec4ScaleForwardPc(&vertices[5], &vertices[5], 1.0f / vertices[5].w);

            perpendicular.x = (vertices[5].y - vertices[4].y) * ((f32)PS2_REZ_W / 240.0f);
            perpendicular.y = vertices[4].x - vertices[5].x;
            perpendicular.z = 0.0f;
            NuVecNorm(&perpendicular, &perpendicular);
            perpendicular.x *= (((f32)PS2_REZ_W * 150.0f) / 240.0f) * NuLgtArcLaserData[i].width;
            perpendicular.y *= NuLgtArcLaserData[i].width * 150.0f;
            vertices[7].x -= vertices[6].x;
            vertices[7].y -= vertices[6].y;
            vertices[7].z -= vertices[6].z;
            vertices[7].w -= vertices[6].w;

            length =
                NuFsqrt(vertices[7].x * vertices[7].x + vertices[7].y * vertices[7].y + vertices[7].z * vertices[7].z);
            if (!(length > 0.0f))
                continue;
            vertices[10].x = NuLgtArcLaserData[i].bend.x;
            vertices[10].y = NuLgtArcLaserData[i].bend.y;
            vertices[10].z = NuLgtArcLaserData[i].bend.z;

            factor =
                (vertices[10].y * vertices[10].y) + (vertices[10].x * vertices[10].x) + vertices[10].z * vertices[10].z;
            if (factor > 0.0f)
                factor = (f64)NuLgtArcLaserData[i].bend_amount / NuFsqrt(factor);
            vertices[10].x *= factor;
            vertices[10].y *= factor;
            vertices[10].z *= factor;
            NuLgtSeed = NuLgtArcLaserData[i].seed;

            step = NuLgtArcLaserData[i].segment_length / length;
            step = step > 1.0f ? 1.0f : step;

            u = NuLgtArcMtl[0].u0;
            u1 = NuLgtArcMtl[0].u1;
            vertices[4].x = vertices[6].x;
            vertices[4].y = vertices[6].y;
            vertices[4].z = vertices[6].z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
            NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
            vertices[1].x = vertices[4].x;
            vertices[1].y = vertices[4].y;
            vertices[1].z = vertices[4].z;
            vertices[3].x = vertices[4].x;
            vertices[3].y = vertices[4].y;
            vertices[3].z = vertices[4].z;

            progress = 0.0f;
            wobble_limit = 1.0 - 1.5 * step;
            while (progress < 1.0f) {

                next = progress + step;
                if (next >= 1.0f) {
                    vertices[5].x = vertices[6].x + vertices[7].x;
                    vertices[5].y = vertices[7].y + vertices[6].y;
                    vertices[5].z = vertices[6].z + vertices[7].z;
                    u = NuLgtArcMtl[0].u1 + (1.0f - progress) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
                } else {

                    angle = ((i32)(next * 32768.0) >> 1) & 0x7fff;
                    trig = &NuTrigTable[angle];
                    vertices[5].x = vertices[7].x * next + vertices[6].x + vertices[10].x * *trig;
                    vertices[5].y = vertices[7].y * next + vertices[6].y + vertices[10].y * *trig;
                    vertices[5].z = vertices[7].z * next + vertices[6].z + *trig * vertices[10].z;
                }
                vertices[5].w = 1.0f;
                NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);

                reciprocal = 1.0f / vertices[5].w;
                NuVec4ScaleForwardPc(&vertices[5], &vertices[5], reciprocal);

                if (progress < wobble_limit && NuLgtSeed != 0) {
                    jitter_y = (f32)((NU_LGT_RAND() & 0xff) - 128);
                    jitter_x = jitter_y * perpendicular.x * NuLgtArcLaserData[i].wobble * reciprocal;
                    jitter_y = jitter_y * perpendicular.y * NuLgtArcLaserData[i].wobble * reciprocal;
                } else {
                    jitter_x = jitter_y = 0.0f;
                }
                offset_x = perpendicular.x * reciprocal;
                offset_y = perpendicular.y * reciprocal;
                corner_x = vertices[5].x - offset_x + jitter_x;
                vertices[0].x = corner_x;
                corner_y = vertices[5].y - offset_y + jitter_y;
                vertices[0].y = corner_y;
                vertices[0].z = vertices[5].z;
                vertices[2].x = vertices[5].x + offset_x + jitter_x;
                vertices[2].y = vertices[5].y + offset_y + jitter_y;
                vertices[2].z = vertices[5].z;
                if (!(corner_x < -50.0 && vertices[1].x < 50.0) && !(corner_x > 700.0f && vertices[1].x > 700.0f) &&
                    !(corner_y < -50.0 && vertices[1].y < -50.0) && !(corner_y > 530.0f && vertices[1].y > 530.0f)) {
                    NU_LGT_ARC_VERTEX(&vertices[0], NuLgtArcLaserData[i].colour, u, tex_v[0]);
                    NU_LGT_ARC_VERTEX(&vertices[1], NuLgtArcLaserData[i].colour, u1, tex_v[0]);
                    NU_LGT_ARC_VERTEX(&vertices[2], NuLgtArcLaserData[i].colour, u, tex_v[2]);
                    NU_LGT_ARC_VERTEX(&vertices[1], NuLgtArcLaserData[i].colour, u1, tex_v[0]);
                    NU_LGT_ARC_VERTEX(&vertices[3], NuLgtArcLaserData[i].colour, u1, tex_v[2]);
                    NU_LGT_ARC_VERTEX(&vertices[2], NuLgtArcLaserData[i].colour, u, tex_v[2]);
                }
                vertices[1].x = vertices[0].x;
                vertices[1].y = vertices[0].y;
                vertices[1].z = vertices[0].z;
                vertices[3].x = vertices[2].x;
                vertices[3].y = vertices[2].y;
                vertices[3].z = vertices[2].z;
                progress = (f32)next;
            }
        }
        // NuPrim2DEnd.
        *NuPrimVertexCountPointer = NuPrimVertexCount;
        NuPrimVertexCount = 0;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
        NuRndrEndSceneEx(0);
        NuLgtArcLaserOldCnt = NuLgtArcLaserCnt;
        NuLgtArcLaserCnt = 0;
        if (!paused)
            ++NuLgtArcLaserFrame;
        NuScratchRelease();
    }

    void __cdecl NuGobjReleaseChildPc(PC_GOBJ_REGISTRY_CHILD_PC *child) {
        PC_GOBJ_REGISTRY_AUX_PC *aux = child->auxiliary;
        if (child->owned_buffer != NULL) {
            NuFree(child->owned_buffer, NuGobjSourcePathPc, 605);
            child->owned_buffer = NULL;
        }
        while (aux != NULL) {
            PC_GOBJ_REGISTRY_AUX_PC *next = aux->next;
            if (aux->payload != NULL)
                aux->payload = NULL;
            aux = next;
        }
        if (child->native_resource != NULL && child->native_resource->resource != NULL)
            PCDeviceReleaseTexture(&child->native_resource->resource);
    }

    FRAME_NATIVE_TEXTURE_PC *FRAME_NATIVE_TEXTURE_POOL_PC::Allocate() {
        i32 index;
        for (index = cursor; index < 16; ++index) {
            if (((occupied[index >> 3] >> (index & 7)) & 1) == 0) {
                occupied[index >> 3] |= (u8)(1u << (index & 7));
                cursor = (index + 1) % 16;
                return &records[index];
            }
        }
        for (index = 0; index < cursor; ++index) {
            if (((occupied[index >> 3] >> (index & 7)) & 1) == 0) {
                occupied[index >> 3] |= (u8)(1u << (index & 7));
                cursor = (index + 1) % 16;
                return &records[index];
            }
        }
        return NULL;
    }

    void __cdecl PCGraphicsClearFrameTexturesPc(void) {
        PC_EFFECT_SURFACE_PC surface;
        for (i32 i = 0; i < 16; ++i) {
            FRAME_NATIVE_TEXTURE_PC *record = &FrameNativeTexturePoolPc.records[i];
            if (record->texture_id != 0 && record->render_texture != NULL) {
                record->dimensions.resource = record->render_texture->resource;
                surface.Attach(record->render_texture, 0, false);
                PCDeviceBackend.SetSurfaceRenderTarget(&surface, 0);
                PCDeviceBackend.ClearDepthStencilSurface();
                PCDeviceClear(0, NULL, 1, 0xff000000, 1.0f, 0);
            }
        }
    }

    PC_FRAMEBUFFER_PC::PC_FRAMEBUFFER_PC() {
    }

    PC_FRAMEBUFFER_PC::~PC_FRAMEBUFFER_PC() {
    }

    void __cdecl PCGraphicsInitializeFramebuffers(u8 flags, i32 unused, VARIPTR *buffer, VARIPTR end) {
        (void)unused;
        (void)buffer;
        (void)end;
        memset(PcFramebufferPool, 0, sizeof(PcFramebufferPool));
        memset(PcFramebufferActive, 0, sizeof(PcFramebufferActive));
        PcBoundFramebuffer = NULL;
        PcDefaultFramebuffer = NULL;
        PcSecondaryFramebuffer = NULL;
        bool multi = PCDeviceBackend.caps.simultaneous_render_targets > 1;
        bool second = multi && (flags & 2);
        bool third = multi && (flags & 4);
        PcDefaultFramebuffer = PCGraphicsCreateFramebuffer();
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, PCDeviceBackend.textures[0], 0);
        PcDefaultFramebuffer->clear_colour[0] = 0;
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
        PcDefaultFramebuffer->clear_colour[2] = 0xffffffff;
        if (second && PCDeviceBackend.target_count > 1) {
            PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, PCDeviceBackend.textures[1], 0);
            PcDefaultFramebuffer->clear_colour[1] = 0x808080;
        }
        if (third && PCDeviceBackend.target_count > 2) {
            PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
            PcDefaultFramebuffer->clear_colour[2] = 0x8080;
        }
        PcSecondaryFramebuffer = PCGraphicsCreateFramebuffer();
        PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, PCDeviceBackend.textures[0], 0);
        PcSecondaryFramebuffer->clear_colour[0] = 0;
        PcBootDefaultFramebuffer = PcBootSecondaryFramebuffer = NULL;
        PcBootDefaultFramebuffer = PcDefaultFramebuffer;
        PcBootSecondaryFramebuffer = PcSecondaryFramebuffer;
        PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
    }

    void __cdecl BuildDebrisVerts(DEBRIS_PART_HEADER_PC *header, DEBRIS_PARTICLE_CHUNK_PC *chunk, NUMTL *material,
                                  f32 time, NUMTX *matrix, i32 particle_type, f32 unused_a, f32 unused_b, f32 unused_c,
                                  f32 near_clip) {
        u32 emitted = 0;
        f32 u0, v0, u1, v1;
        i32 i;
        (void)unused_a;
        (void)unused_b;
        (void)unused_c;
        if (material->particle_type_tag == -105) {
            u0 = 0.0f;
            u1 = 1.0f;
            v1 = 1.0f;
            v0 = 0.0f;
        } else {
            u0 = header->texture_u0;
            u1 = header->texture_u1;
            v0 = header->texture_v0;
            v1 = header->texture_v1;
        }
        for (i = 0; i != 32; ++i) {
            DEBRISPARTICLE_PC *particle = &chunk->particles[i];
            f32 age = time - particle->start_time;
            u32 frame_index = (u32)(age * particle->inverse_lifetime);
            if (frame_index < 63) {
                f32 fraction = age * particle->inverse_lifetime - (f32)frame_index;
                f32 distance;
                DEBRIS_PARTICLE_FRAME_PC *first = &header->frames[frame_index];
                NUVEC position, rotated, corners[4];
                NUVEC *corner;
                i32 count;
                u32 colour;
                u32 index;
                position.x = particle->position.x + particle->momentum.x * age;
                position.y = (f32)(particle->position.y + particle->momentum.y * age +
                                   header->gravity * age * age * (f64)0.945f);
                position.z = particle->position.z + particle->momentum.z * age;
                rotated.x = position.x * matrix->m00 + position.y * matrix->m10 + position.z * matrix->m20;
                rotated.y = position.x * matrix->m01 + position.y * matrix->m11 + position.z * matrix->m21;
                rotated.z = position.x * matrix->m02 + position.y * matrix->m12 + position.z * matrix->m22;
                NuRndr_DebrisMtx.m30 = rotated.x + matrix->m30;
                NuRndr_DebrisMtx.m31 = rotated.y + matrix->m31;
                NuRndr_DebrisMtx.m32 = rotated.z + matrix->m32;
                if (particle_type == 6 || particle_type == 7)
                    NuRndrParticleSetRepeat((NUVEC *)&NuRndr_DebrisMtx.m30);
                distance = NuRndr_DebrisPlane.y * NuRndr_DebrisMtx.m31 + NuRndr_DebrisPlane.x * NuRndr_DebrisMtx.m30 +
                           NuRndr_DebrisPlane.z * NuRndr_DebrisMtx.m32 + NuRndr_DebrisPlane.w;
                if (distance < near_clip)
                    continue;
                if (NuDebrisCurrentVertexCount + emitted + 6 > NuDebrisMaxVertexCount) {
                    VARIPTR *buffer;
                    NATIVE_PARTICLE_PACKET_PC *packet;
                    NuDebrisCurrentPacket->vertex_count += (i32)emitted;
                    NuDebrisFrameVertexCount += emitted;
                    emitted = 0;
                    if (NuDebrisRendererNextBuffer() == 0)
                        return;
                    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
                    buffer = NuDisplayListStreamCursor;
                    packet = (NATIVE_PARTICLE_PACKET_PC *)buffer->void_ptr;
                    packet->material = NuDebrisCurrentPacket->material;
                    packet->vertex_buffer_index = (u8)NuDebrisCurrentBufferIndex;
                    packet->use_system_memory_vb = NuDebrisUseSystemMemory;
                    packet->first_vertex = (i32)NuDebrisCurrentVertexCount;
                    packet->vertex_count = 0;
                    buffer->addr += sizeof(*packet);
                    NuDebrisCurrentPacket = packet;
                    AddParticleGroupToDisplayList(packet);
                }
                corners[0].x = first[1].position.x * fraction + first->position.x * (1.0f - fraction);
                corners[0].y = first[1].position.y * fraction + first->position.y * (1.0f - fraction);
                corners[0].z = first[1].position.z * fraction + first->position.z * (1.0f - fraction);
                corners[1].x = first[1].extent.x * fraction + first->extent.x * (1.0f - fraction);
                corners[1].y = first[1].extent.y * fraction + first->extent.y * (1.0f - fraction);
                corners[1].z = first[1].extent.z * fraction + first->extent.z * (1.0f - fraction);
                corners[2].x = first[1].texture_offset.x * fraction + first->texture_offset.x * (1.0f - fraction);
                corners[2].y = first[1].texture_offset.y * fraction + first->texture_offset.y * (1.0f - fraction);
                corners[2].z = first[1].texture_offset.z * fraction + first->texture_offset.z * (1.0f - fraction);
                corners[3].x = (corners[1].x - corners[2].x) + corners[0].x;
                corners[3].y = (corners[1].y - corners[2].y) + corners[0].y;
                corners[3].z = (corners[1].z - corners[2].z) + corners[0].z;
                corner = corners;
                for (count = 4; count != 0; --count) {
                    NuVecMtxTransform(corner, corner, &NuRndr_DebrisMtx);
                    ++corner;
                }
                colour = first->colour;
                index = NuDebrisCurrentVertexCount + emitted;
                *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index].x = corners[0];
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index].colour = colour;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index].u = u0;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index].v = v1;
                *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].x = corners[2];
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].colour = colour;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].u = u1;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].v = v1;
                *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].x = corners[1];
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].colour = colour;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].u = u1;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].v = v0;
                *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].x = corners[0];
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].colour = colour;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].u = u0;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].v = v1;
                *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].x = corners[1];
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].colour = colour;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].u = u1;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].v = v0;
                *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].x = corners[3];
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].colour = colour;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].u = u0;
                ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].v = v0;
                emitted += 6;
            }
        }
        NuDebrisCurrentPacket->vertex_count += (i32)emitted;
        NuDebrisCurrentVertexCount += emitted;
        NuDebrisFrameVertexCount += emitted;
    }

    i32 __cdecl NuTexRegisterRenderTargetPc(GRAPHICS_RENDER_TEXTURE_PC *target) {
        FRAME_NATIVE_TEXTURE_PC *texture = FrameNativeTexturePoolPc.Allocate();
        texture->render_texture = target;
        texture->texture_id = NuTexRegisterDimensionsPc(&texture->dimensions);
        if (texture->texture_id == 0) {
            i32 index = (i32)((u32)texture - (u32)&FrameNativeTexturePoolPc.records[0]) / 0x30;
            FrameNativeTexturePoolPc.occupied[index >> 3] &= (u8) ~(1u << (index & 7));
            FrameNativeTexturePoolPc.cursor = index;
            return 0;
        }
        texture->dimensions.width = target->width;
        texture->dimensions.height = target->height;
        texture->dimensions.ref_count = 1;
        memset(texture->dimensions.checksum, 0, sizeof(texture->dimensions.checksum));
        texture->dimensions.image_data = NULL;
        texture->dimensions.image_size = 0;
        texture->dimensions.resource = target->resource;
        return texture->texture_id;
    }

    void NuMainFilter::render() {
        if (bloom_enabled)
            RenderBloomPc();
    }

    void __cdecl NuRndrParticleGroup(DEBRIS_PARTICLE_CHUNK_PC *chunks, DEBRIS_PART_HEADER_PC *header, NUMTL *material,
                                     f32 time, NUMTX *matrix, i32 particle_type, f32 a, f32 b, f32 c, f32 near_clip) {
        DEBRIS_PARTICLE_CHUNK_PC *chunk;
        i32 done, count;
        if (header == NULL) {
            NuDebrisLastHeader = NULL;
            return;
        }
        if (material == NULL || (material->particle_type_tag == -105 && PCDeviceHardwareVertexProcessingPC < 2))
            return;
        if (header != NuDebrisLastHeader) {
            NUCAMERA camera;
            VARIPTR *buffer;
            NATIVE_PARTICLE_PACKET_PC *packet;
            if (material->attribs.unknown_2_1_2 != 2 || material->attribs.unknown_2_4 == 0) {
                material->attribs.unknown_2_1_2 = 2;
                material->attribs.unknown_2_4 = 1;
                NuMtlUpdate(material);
            }
            if (NuRndr_DebrisRotMtxPtr == NULL)
                NuMtxCalcDebrisFaceOn(&NuRndr_DebrisMtx);
            else
                NuRndr_DebrisMtx = *NuRndr_DebrisRotMtxPtr;
            NuCameraGet(&camera);
            NuRndr_DebrisPlane.x = camera.mtx.m20;
            NuRndr_DebrisPlane.y = camera.mtx.m21;
            NuRndr_DebrisPlane.z = camera.mtx.m22;
            NuRndr_DebrisPlane.w =
                -(camera.mtx.m30 * camera.mtx.m20 + camera.mtx.m31 * camera.mtx.m21 + camera.mtx.m32 * camera.mtx.m22);
            header->last_render_time = time;
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
            buffer = NuDisplayListStreamCursor;
            packet = (NATIVE_PARTICLE_PACKET_PC *)buffer->void_ptr;
            buffer->addr = (u32)(packet + 1);
            packet->use_system_memory_vb = NuDebrisUseSystemMemory;
            packet->vertex_buffer_index = (u8)NuDebrisCurrentBufferIndex;
            packet->first_vertex = (i32)NuDebrisCurrentVertexCount;
            packet->material = material;
            packet->vertex_count = 0;
            NuDebrisCurrentPacket = packet;
            if (NuDebrisVertexData == NULL)
                PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, &NuDebrisVertexData, 0x2000,
                                         NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
            AddParticleGroupToDisplayList(NuDebrisCurrentPacket);
            NuDebrisLastHeader = header;
        }
        chunk = chunks;
        done = count = 0;
        while (!done) {
            DEBRIS_PARTICLE_CHUNK_PC *next = chunk->next;
            switch ((i8)chunk->command) {
                case 0x4e:
                    if (next != NULL) {
                        BuildDebrisVerts(header, chunk, material, time, matrix, particle_type, a, b, c, near_clip);
                        chunk = next;
                    }
                    break;
                case 0x52:
                    BuildDebrisVerts(header, chunk, material, time, matrix, particle_type, a, b, c, near_clip);
                    done = 1;
                    break;
            }
            if (++count > 0x100)
                break;
        }
    }

    void __cdecl NuGobjRegistryCleanupPc(void) {
        if (NuGobjRegistryInitializedPc != 0) {
            PC_GOBJ_REGISTRY_ENTRY_PC *entry = NuGobjRegistryFirstPc;
            while (entry != NULL) {
                PC_GOBJ_REGISTRY_ENTRY_PC *next = entry->next;
                PC_GOBJ_REGISTRY_CHILD_PC *child = entry->children;
                PC_GOBJ_REGISTRY_AUX_PC *aux = entry->auxiliary;
                while (child != NULL) {
                    PC_GOBJ_REGISTRY_CHILD_PC *next_child = child->next;
                    NuGobjReleaseChildPc(child);
                    child = next_child;
                }
                while (aux != NULL)
                    aux = aux->next;
                if (entry->previous != NULL)
                    entry->previous->next = entry->next;
                if (entry->next != NULL)
                    entry->next->previous = entry->previous;
                --NuGobjRegistryCountPc;
                entry = next;
                NuGobjRegistryFirstPc = entry;
            }
        }
    }

    void __cdecl NuGobjRegistryInitPc(void) {
        if (NuGobjRegistryInitializedPc != 0)
            NuGobjRegistryCleanupPc();
        NuGobjRegistryFirstPc = NULL;
        NuGobjRegistryInitializedPc = 1;
    }

    void NuMainFilter::initResources() {
        NuMainFilterGen::initResources();
        void *last_program;
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            programs[6] = PostShaderProgramCreatePc(2, PostShaderTokens7662d8, PostShaderTokens766578);
            programs[8] = PostShaderProgramCreatePc(2, PostShaderTokens766820, PostShaderTokens766578);
            platform_programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens7662d8, PostShaderTokens7666f0);
            last_program = PostShaderProgramCreatePc(2, PostShaderTokens766820, PostShaderTokens7666f0);
            platform_uniforms[0] = 0x82;
            platform_uniforms[1] = 0x88;
            platform_uniforms[2] = 0x89;
            platform_uniforms[3] = 0x8a;
        } else {
            programs[6] = PostShaderProgramCreatePc(2, PostShaderTokens766c38, PostShaderTokens766ed8);
            programs[8] = PostShaderProgramCreatePc(2, PostShaderTokens767180, PostShaderTokens766ed8);
            platform_programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens766c38, PostShaderTokens767050);
            last_program = PostShaderProgramCreatePc(2, PostShaderTokens767180, PostShaderTokens767050);
            platform_uniforms[0] = 0;
            platform_uniforms[1] = 1;
            platform_uniforms[2] = 2;
            platform_uniforms[3] = 3;
        }
        platform_programs[1] = last_program;
        platform_fbo = PCGraphicsCreateFramebuffer();
    }

    void NuDeferredFilter::initResources() {
        NuDeferredFilterGen::initResources();
        programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens767598, PostShaderTokens7676f0);
        programs[1] = PostShaderProgramCreatePc(2, PostShaderTokens767598, PostShaderTokens767fc8);
        programs[2] = PostShaderProgramCreatePc(2, PostShaderTokens768d20, PostShaderTokens768870);
        programs[3] = PostShaderProgramCreatePc(2, PostShaderTokens768d20, PostShaderTokens768ae0);
    }

    void NuSpeedBlurFilter::initResources() {
        NuPostFilterGen::initResources();
        void *program;
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            program = PostShaderProgramCreatePc(2, PostShaderTokens768de0, PostShaderTokens7690b0);
            platform_uniform = 0x80;
        } else {
            program = PostShaderProgramCreatePc(2, PostShaderTokens7692c0, PostShaderTokens769580);
            platform_uniform = 0;
        }
        programs[0] = program;
    }

    void __cdecl NuMtlInitExPS(VARIPTR *) {
        NuMtlInitShaderResourcesPc();
    }
    // @@END

} // extern "C"

/* nuhgobj's platform members: the POI lookup and the hierarchy shadow-group packets. */
typedef struct PC_HG_SHADOW_SHAPE48 PC_HG_SHADOW_SHAPE48;
typedef struct PC_HG_SHADOW_SHAPE64 PC_HG_SHADOW_SHAPE64;
typedef struct PC_HG_SHADOW_GROUP PC_HG_SHADOW_GROUP;
struct PC_HG_SHADOW_SHAPE48 {
    union {
        NUVEC axes[4];
        struct {
            f32 center_x;
            u32 flags;
            u8 rest[40];
        } filter;
    };
};
struct PC_HG_SHADOW_SHAPE64 {
    PC_HG_SHADOW_SHAPE48 position;
    u8 unknown_30[16];
};
struct PC_HG_SHADOW_GROUP {
    PC_HG_SHADOW_SHAPE48 *ellipses;
    PC_HG_SHADOW_SHAPE64 *cylinders;
    void *third_shape;
    u8 ellipse_count, cylinder_count, third_count, joint_index;
};
STATIC_ASSERT(sizeof(PC_HG_SHADOW_SHAPE48) == 48);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_SHAPE64) == 64);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_SHAPE48, filter.flags) == 4);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_GROUP) == 16);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, ellipses) == 0);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, cylinders) == 4);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, third_shape) == 8);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, ellipse_count) == 12);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, cylinder_count) == 13);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, third_count) == 14);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, joint_index) == 15);

extern "C" {
    NUHGOBJ_POI_PC *__cdecl NuHGobjGetPOI(NUHGOBJ_EVAL_PC *object, u8 index) {
        if ((i32)index < object->point_index_count) {
            index = object->point_index_map[index];
            if (index != 0xff)
                return &object->points[index];
        }
        return NULL;
    }

    void PCEnqueueHierarchyShadowGroup(NUMTX *matrix, void *ellipse, i32 ellipse_count, void *cylinder,
                                       i32 cylinder_count, void *third, i32 third_count) {
        if (PCShadowPacketCount >= 280)
            return;
        PCShadowPackets[PCShadowPacketCount].near_value = PCShadowNearValue;
        PCShadowPackets[PCShadowPacketCount].far_value = PCShadowFarValue;
        PCShadowPackets[PCShadowPacketCount].state0 = PCShadowState.state0;
        PCShadowPackets[PCShadowPacketCount].state1 = PCShadowState.state1;
        PCShadowPackets[PCShadowPacketCount].state2 = PCShadowState.state2;
        PCShadowPackets[PCShadowPacketCount].state_byte = PCShadowState.state_byte;
        PCShadowPackets[PCShadowPacketCount].matrix = *matrix;
        PCShadowPackets[PCShadowPacketCount].ellipse = ellipse;
        PCShadowPackets[PCShadowPacketCount].ellipse_count = (u8)ellipse_count;
        PCShadowPackets[PCShadowPacketCount].cylinder = cylinder;
        PCShadowPackets[PCShadowPacketCount].cylinder_count = (u8)cylinder_count;
        PCShadowPackets[PCShadowPacketCount].third_shape = third;
        PCShadowPackets[PCShadowPacketCount].third_count = (u8)third_count;
        PCShadowPackets[PCShadowPacketCount].sequence = (u8)PCShadowPacketSequence;
        ++PCShadowPacketCount;
        ++PCShadowPacketSequence;
    }

    void NuHGobjSubmitShadowGroupsPc(NUHGOBJ_EVAL_PC *hierarchy, NUMTX *world, NUMTX *joints) {
        PC_HG_SHADOW_GROUP *group;
        NUHGOBJ_EVAL_PC *object;

        object = hierarchy;
        if (PcWindowOptions.shadowsenabled == 0 || PcWindowOptions.shadowmethod != 1 || object->shadow_groups == NULL ||
            object->suppress_shadow_points != 0)
            return;
        for (group = object->shadow_groups; group->joint_index != 255; ++group) {
            if (PCShadowPacketCount < 280 && PCShadowSubmissionEnabled != 0) {
                NUMTX combined;

                NuMtxMulVU0(&combined, &joints[group->joint_index], world);
                PCEnqueueHierarchyShadowGroup(&combined, group->ellipses, group->ellipse_count, group->cylinders,
                                              group->cylinder_count, group->third_shape, group->third_count);
                currentScene.clear_flags = 1;
            }
        }
    }
} // extern "C"

/* nupostdeferred's platform members: the empty framebuffer resolve and the shadow display-list handlers. */
extern "C" void __cdecl PCGraphicsResolveFramebufferPc(i32) {
}
extern "C" void __cdecl NuDisplayListShadowTransformPc(void *data) {
    NUMTX *matrix = (NUMTX *)data;
    f32 saved33 = matrix->m33, saved23 = matrix->m23;
    matrix->m33 = 1.0f;
    matrix->m23 = 0.0f;
    NuShaderManagerSetfv(0x11, &matrix->m00);
    matrix->m33 = saved33;
    matrix->m23 = saved23;
}
extern "C" void __cdecl NuDisplayListShadowTransformTransposePc(void *data) {
    NUMTX *matrix = (NUMTX *)data;
    f32 saved33 = matrix->m33, saved32 = matrix->m32;
    matrix->m33 = 1.0f;
    matrix->m32 = 0.0f;
    NuShaderManagerSetElementsfv_transpose(0x11, 0, 1, &matrix->m00);
    matrix->m33 = saved33;
    matrix->m32 = saved32;
}
typedef struct PC_SHADOW_MATERIAL_READER_PC {
    union {
        NUMTL common;
        struct {
            u8 unknown_000[0x40];
            u32 attributes_word;
        };
    };
    u8 unknown_09c[0x154];
    u32 vertex_format;
    i16 shader_id, shadow_shader_id;
    u8 unknown_1f8[0xc4];
    PC_TEXTURE_RESOURCE_PC *vertex_declaration;
} PC_SHADOW_MATERIAL_READER_PC;
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, attributes_word) == 0x40);
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, vertex_format) == 0x1f0);
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, shadow_shader_id) == 0x1f6);
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, vertex_declaration) == 0x2bc);
extern "C" void __cdecl NuDisplayListShadowMaterialPc(void *data) {
    PC_SHADOW_MATERIAL_READER_PC *material = (PC_SHADOW_MATERIAL_READER_PC *)data;
    PC_SHADER_OBJECT_FULL_VIEW *shader =
        (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderRecordFromHandle((u32)(i32)material->shadow_shader_id);
    if (shader != NULL) {
        NuDisplayHandlerMaterialPc = material;
        if (!(material->vertex_format & 0x300000) && material->common.display_list != NULL) {
            PCShaderSetMaterialParametersPc(shader, &material->common);
            NuShaderManagerBindShaderPc(shader);
            PCDeviceSetVertexDeclarationPc(material->vertex_declaration);
        }
        u32 flags = material->attributes_word;
        u32 z_mode = flags & 0xc000;
        if (z_mode == 0x4000)
            material->attributes_word = (flags & 0xffff7fff) | z_mode;
        NuMtlSetRenderStatesPS(&material->common);
        PCDeviceSetRenderState(27, 0);
    }
}

/* nurndr's platform members, in nurndr order. */
extern "C" {
    extern i32 DwaWeightCapacityPc, DwaPointerCapacityPc, DwaBlockCapacityPc, DwaEnabledPc, DwaStreamCapacityPc;
    extern i32 DwaBlockCountPc;
    extern u8 DwaBlockTablePc[0x400];
    void __cdecl NuRndrGradClear(i32 clear_flags, i32 top_colour, i32 bottom_colour, f32 alpha) {
        NuRndrClear(clear_flags, bottom_colour, alpha);
    }
    __declspec(noinline) void __cdecl NuDisplayListResetStateCache(NURNDRLIBSTATE *state) {
        state->mtl = NULL;
        state->tex_id = -1;
        state->global_id = -1;
        state->lights_id = -1;
        state->camera_id = -1;
        state->fog_id = -1;
        state->konst_id = -1;
    }
    void __cdecl RndrStateUpdate(NURNDRLIBSTATE *state, NUMTL *material, NUDISPLAYLISTITEM *item) {
        (void)state;
        (void)material;
        (void)item;
    }
    void __cdecl RndrStateUpdateFx(NURNDRLIBSTATE *state, NUDISPLAYLISTITEM *item) {
        (void)state;
        (void)item;
    }
    void __cdecl NuDisplayListInit(VARIPTR *cursor, i32 arg2) {
        NuRndrStateInit();
        nudlist_field_9d8 = 0;
        nudlist_field_e80 = 0;
        sort_list = NULL;
        nudlist_field_fc8 = 0;
        PCDisplayList2dPc.first = &nudlist_2d_static;
        {
            VARIPTR end;
            end.addr = (u32)arg2;
            NuDisplayListCreateDynMtlList_candidate(cursor, end);
        }
        nudlist_flags |= 0x10;
        nudlist_field_e58 = 0;
        NuDisplayListStreamAlloc(cursor, arg2, 0x40);
        NuDisplayListStreamCursor = &nudlist_stream_cursor;
        nudlist_field_1598 = nuprim_field_ba8;
        nudlist_field_fc4 = PCThreadCriticalSectionCreate_candidate();
        NuDisplayListSwapBuffersEndFrame();
        NuDisplayListBeginFrame();
    }
    void __cdecl NuRndrDwaInitPc(void) {
        DwaWeightCapacityPc = 0x4000;
        DwaPointerCapacityPc = 0x4000;
        DwaBlockCapacityPc = 0x2000;
        DwaEnabledPc = 1;
        DwaBlockCountPc = 0;
        DwaWeightCursorPc = 0xef00;
        DwaPointerCursorPc = 0x800;
        memset(DwaBlockTablePc, 0, sizeof(DwaBlockTablePc));
        DwaStreamCapacityPc = 0x2000;
    }
    /* Native PC entry 0x005fe450 is RET-only; PcRendererCleanup calls it while the D3D
     * device is torn down. Its historical name is unknown. */
    void __cdecl NuRndrDwaReleasePc(void) {
    }
    void __cdecl NuRndrConfigureLeafPc(f32 a, f32 b, i32 c) {
        (void)a;
        (void)b;
        (void)c;
    }
    void __cdecl PermanentGraphicsResourceSetup(void) {
        NuPtlDmaSlotsExhaustedPc();
    }
    void __cdecl NuRndrShadowInit(u8 *random) {
        (void)random;
        NuPtlDmaSlotsExhaustedPc();
    }
    void __cdecl NuLightSpotFadeSet(u32 fade) {
        (void)fade;
    }
    DWA_WEIGHT_BLOCK_PC *__cdecl NuRndrCreateBlendShapeDeformerWeightsArray(i32 count) {
        i32 clear_bytes = count * 4 + 0x204;
        DWA_WEIGHT_BLOCK_PC *result;
        DwaWeightCursorPc -= clear_bytes;
        if (DwaWeightCursorPc < 0) {
            PCClipScreenUnsupported();
            return NULL;
        }
        result = &DwaWeightPoolPc[DwaWeightCursorPc];
        memset(result, 0, clear_bytes);
        return result;
    }
    DWA_WEIGHT_BLOCK_PC **__cdecl NuRndrCreateBlendShapeDWAPointers(i32 count) {
        DwaPointerCursorPc -= count;
        if (DwaWeightCursorPc < 0) {
            PCClipScreenUnsupported();
            return NULL;
        }
        return &DwaPointerPoolPc[DwaPointerCursorPc];
    }
    void __cdecl NuRndrShadowDirCol(const NUVEC *direction, u32 colour, f32 near_distance, f32 far_distance) {
        PCShadowState.direction = *direction;
        PCShadowNearValue = near_distance;
        PCShadowFarValue = far_distance;
        PCShadowState.packed_colour = ((colour >> 1) & 0x007f7f7fu) + (colour & 0xff000000u) * 2u;
    }
    void __cdecl NuRndrAddShadow(NUVEC *position, f32 radius, i32 opacity, i32 unused_x, i32 unused_y, i32 unused_z) {
        if (NuCameraClipTestPoints(position, 1, NULL) == 0 && NuRndrShadowCnt < 128) {
            NuRndrShadPolDat[NuRndrShadowCnt].position = *position;
            NuRndrShadPolDat[NuRndrShadowCnt].radius = radius;
            NuRndrShadPolDat[NuRndrShadowCnt].opacity = (u16)opacity;
            ++NuRndrShadowCnt;
        }
    }
    /* The debug 3D line batch is not drawn on PC. */
    void __cdecl NuRndrLine3dDbgFlush(void) {
    }
    void __cdecl NuRndrShadPolys(NUMTL *material) {
        i32 index;
        PC_SHADOW_QUEUE_RECORD *record;
        NUVEC position;
        f32 radius, left, right, front, back;
        u32 colour;
        if (NuRndrShadowCnt == 0)
            return;
        record = NuRndrShadPolDat;
        for (index = 0; index < NuRndrShadowCnt; ++index, ++record) {
            position = record->position;
            colour = (u32)(i16)record->opacity << 24;
            radius = record->radius;
            NuRndrShadowVerticesPc[0].colour = colour;
            NuRndrShadowVerticesPc[1].colour = colour;
            NuRndrShadowVerticesPc[2].colour = colour;
            NuRndrShadowVerticesPc[3].colour = colour;
            left = position.x - radius;
            NuRndrShadowVerticesPc[0].position.x = left;
            NuRndrShadowVerticesPc[0].position.y = position.y;
            front = position.z - radius;
            NuRndrShadowVerticesPc[0].position.z = front;
            NuRndrShadowVerticesPc[0].u = 0.0f;
            NuRndrShadowVerticesPc[0].v = 0.0f;
            right = position.x + radius;
            NuRndrShadowVerticesPc[1].position.x = right;
            NuRndrShadowVerticesPc[1].position.y = position.y;
            NuRndrShadowVerticesPc[1].position.z = front;
            NuRndrShadowVerticesPc[1].u = 1.0f;
            NuRndrShadowVerticesPc[1].v = 0.0f;
            NuRndrShadowVerticesPc[2].position.x = left;
            NuRndrShadowVerticesPc[2].position.y = position.y;
            back = position.z + radius;
            NuRndrShadowVerticesPc[2].position.z = back;
            NuRndrShadowVerticesPc[2].u = 0.0f;
            NuRndrShadowVerticesPc[2].v = 1.0f;
            NuRndrShadowVerticesPc[3].position.x = right;
            NuRndrShadowVerticesPc[3].position.y = position.y;
            NuRndrShadowVerticesPc[3].position.z = back;
            NuRndrShadowVerticesPc[3].u = 1.0f;
            NuRndrShadowVerticesPc[3].v = 1.0f;
            NuRndrStrip3d(NuRndrShadowVerticesPc, material, NULL, 4);
        }
        NuRndrShadowCnt = 0;
    }
    void __cdecl NuRndrInitWorld(void) {
    }
    /* Direct object rendering is not supported on PC: retail reports it and draws nothing. */
    i32 __cdecl NuRndrGobj(void *object, NUMTX *matrix) {
        PCClipScreenUnsupported();
        return 0;
    }
    i32 __cdecl NuRndrSetViewMtx(NUMTX *view_projection, NUMTX *viewport_clip, NUMTX *scissor_clip) {
        return 0;
    }
    void __cdecl FaceYDirStream(i32 angle) {
    }
    i32 __cdecl NuRndrSetSpecularLightPS(const NUVEC *direction, const NUVEC4 *intensity) {
        static NUVEC rndrstream_specular_dir = {0.0f, 0.0f, 1.0f};
        static NUVEC4 rndrstream_specular_intensity = {1.0f, 1.0f, 1.0f, 1.0f};
        static NUVEC camvec = {0.0f, 0.0f, -1.0f};
        NUVEC view_direction, half_direction, light_direction, eye_direction, axis;
        NUMTX matrix;
        f32 horizontal, facing, weight, remainder;
        if (direction != NULL)
            rndrstream_specular_dir = *direction;
        if (intensity != NULL)
            rndrstream_specular_intensity = *intensity;
        NuVecInvMtxRotate(&view_direction, &rndrstream_specular_dir, &global_camera.mtx);
        NuVecLerp(&half_direction, &view_direction, &camvec, 0.5f);
        NuVecMtxRotate(&half_direction, &half_direction, &global_camera.mtx);
        NuVecNorm(&half_direction, &half_direction);
        NuVecNorm(&light_direction, &rndrstream_specular_dir);
        eye_direction.x = -global_camera.mtx.m20;
        eye_direction.y = -global_camera.mtx.m21;
        eye_direction.z = -global_camera.mtx.m22;
        facing = NuVecDot(&eye_direction, &light_direction);
        axis = half_direction;
        matrix = NuMtxSetIdentityTemplate;
        horizontal = NuFsqrt(axis.x * axis.x + axis.z * axis.z);
        if (horizontal > FLT_MIN) {
            matrix.m00 = axis.z / horizontal;
            matrix.m10 = (axis.x * axis.y) / horizontal;
            matrix.m20 = -axis.x;
            matrix.m01 = 0.0f;
            matrix.m11 = horizontal;
            matrix.m21 = axis.y;
            matrix.m02 = axis.x / horizontal;
            matrix.m12 = (axis.z * -axis.y) / horizontal;
            matrix.m22 = axis.z;
        } else {
            matrix.m11 = matrix.m22 = 0.0f;
            matrix.m21 = axis.y;
            matrix.m12 = -axis.y;
        }
        matrix.m31 = matrix.m30 = 0.5f;
        if (facing < 0.0f) {
            weight = facing * facing;
            weight *= weight;
            weight *= weight;
            weight *= weight;
            weight *= weight;
            remainder = 1.0f - weight;
            if (matrix.m00 * light_direction.x + matrix.m10 * light_direction.y + matrix.m20 * light_direction.z >=
                0.0f) {
                matrix.m00 = matrix.m00 * remainder + light_direction.x * weight;
                matrix.m10 = matrix.m10 * remainder + light_direction.y * weight;
                matrix.m20 = matrix.m20 * remainder + light_direction.z * weight;
            } else {
                matrix.m00 = matrix.m00 * remainder - light_direction.x * weight;
                matrix.m10 = matrix.m10 * remainder - light_direction.y * weight;
                matrix.m20 = matrix.m20 * remainder - light_direction.z * weight;
            }
            if (matrix.m01 * light_direction.x + matrix.m11 * light_direction.y + matrix.m21 * light_direction.z >=
                0.0f) {
                matrix.m01 = matrix.m01 * remainder + light_direction.x * weight;
                matrix.m11 = matrix.m11 * remainder + light_direction.y * weight;
                matrix.m21 = matrix.m21 * remainder + light_direction.z * weight;
            } else {
                matrix.m01 = matrix.m01 * remainder - light_direction.x * weight;
                matrix.m11 = matrix.m11 * remainder - light_direction.y * weight;
                matrix.m21 = matrix.m21 * remainder - light_direction.z * weight;
            }
        }
        NuVecNeg(&view_direction, &view_direction);
        NuRndrStateSetSpecularLightEx(&view_direction, &matrix, (const NUVEC *)&rndrstream_specular_intensity);
        return 1;
    }
    extern void NuDisplayListAnimateMtls(f32);
    static f32 FrameShaderMaterialAnimationTimePc = 0;
    static i32 FrameShaderMaterialFirstCallPc = 1;
    extern f32 FrameMaterialAnimationSpeedPc;
    void NuMtlAnimateShaderMtlTextures(f32 frame_time) {
        i32 i;
        FrameShaderMaterialAnimationTimePc += frame_time;
        if (FrameShaderMaterialFirstCallPc != 0) {
            for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
                NUDISPLAY_LIST_PC *scene = PCDisplayScenes[i];
                if (((FRAME_MTL_SCENE_PC *)scene->gscene)->shader_texture_animation_enabled != 0) {
                    i32 j;
                    for (j = 0; j < (i32)scene->material_count; ++j) {
                        FRAME_MTL_ANIM_PC *material = (FRAME_MTL_ANIM_PC *)scene->display_materials[j];
                        i32 layer;
                        for (layer = 0; layer < 4; ++layer) {
                            material->texture_animation_offsets[layer][0] = 0.0f;
                            material->texture_animation_offsets[layer][1] = 0.0f;
                        }
                    }
                }
            }
            FrameShaderMaterialFirstCallPc = 0;
        }
    }
    void NuMtlAnimate(f32 frame_time) {
        NuDisplayListAnimateMtls(frame_time * FrameMaterialAnimationSpeedPc);
        NuMtlAnimateShaderMtlTextures(frame_time);
    }
    void __cdecl NuRndrInitEx(i32 size, VARIPTR *buffer, VARIPTR end) {
        NuRndrStreamInit(size, buffer);
        NuRndrInitConfigPc(buffer, end);
    }
    static i32 RendererConfigEnabledPc = 0, RendererConfigSecondEnabledPc = 0;
    static NUMTX RendererConfigMatrixPc = {0};
    static PC_BUFFER_RESOURCE_PC *RendererGeometryVertexBufferPc = 0;
    static PC_BUFFER_RESOURCE_PC *RendererGeometryIndexBufferPc = 0;
    void __cdecl NuRndrGeometryBufferReleasePc(void) {
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE_PC **)&RendererGeometryVertexBufferPc);
    }
    void __cdecl NuRndrGeometryBufferRestorePc(void) {
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE_PC **)&RendererGeometryVertexBufferPc);
        PCDeviceCreateVertexBuffer(0x40000, 0x208, 0, 0, &RendererGeometryVertexBufferPc);
        NuRndrQuadBufferInitPc();
    }
    typedef struct RENDERER_CLEAR_RECORD_PC {
        u8 unknown_00[0x20];
        u16 active;
        u8 unknown_22[6];
    } RENDERER_CLEAR_RECORD_PC;
    STATIC_ASSERT(sizeof(RENDERER_CLEAR_RECORD_PC) == 0x28);
    STATIC_ASSERT(offsetof(RENDERER_CLEAR_RECORD_PC, active) == 0x20);
    static RENDERER_CLEAR_RECORD_PC RendererClearRecordsPc[32] = {0};
    typedef struct RENDERER_SHADOW_DEFAULTS_PC {
        f32 state0, state1, state2;
        u32 packed_colour;
    } RENDERER_SHADOW_DEFAULTS_PC;
    STATIC_ASSERT(sizeof(RENDERER_SHADOW_DEFAULTS_PC) == 16);
    STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, state0) == 0);
    STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, state1) == 4);
    STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, state2) == 8);
    STATIC_ASSERT(offsetof(RENDERER_SHADOW_DEFAULTS_PC, packed_colour) == 12);
    typedef struct RENDERER_SCREEN_VERTEX_PC {
        f32 x, y, z, w;
        u32 attribute_bits;
        u8 unknown_14[8];
    } RENDERER_SCREEN_VERTEX_PC;
    STATIC_ASSERT(sizeof(RENDERER_SCREEN_VERTEX_PC) == 0x1c);
    STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, x) == 0);
    STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, y) == 4);
    STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, z) == 8);
    STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, w) == 12);
    STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, attribute_bits) == 16);
    STATIC_ASSERT(offsetof(RENDERER_SCREEN_VERTEX_PC, unknown_14) == 20);
#define QUAD_VERTEX ((RENDERER_SCREEN_VERTEX_PC *)data)
    void __cdecl NuRndrQuadBufferInitPc(void) {
        void *data;
        if (RendererQuadVertexBufferPc != NULL) {
            RendererQuadVertexBufferPc->methods->release(RendererQuadVertexBufferPc);
            RendererQuadVertexBufferPc = NULL;
        }
        PCDeviceCreateVertexBuffer(0x70, 0, 0, 1, &RendererQuadVertexBufferPc);
        if (RendererQuadVertexBufferPc->methods->lock(RendererQuadVertexBufferPc, 0, 0, &data, 0) < 0)
            return;
        QUAD_VERTEX[0].x = 0.0f;
        QUAD_VERTEX[0].y = 0.0f;
        QUAD_VERTEX[0].z = 0.5f;
        QUAD_VERTEX[1].x = (f32)PCDeviceBackend.parameters.width;
        QUAD_VERTEX[1].y = 0.0f;
        QUAD_VERTEX[1].z = 0.5f;
        QUAD_VERTEX[2].x = (f32)PCDeviceBackend.parameters.width;
        QUAD_VERTEX[2].y = (f32)PCDeviceBackend.parameters.height;
        QUAD_VERTEX[2].z = 0.5f;
        QUAD_VERTEX[3].x = 0.0f;
        QUAD_VERTEX[3].y = (f32)PCDeviceBackend.parameters.height;
        QUAD_VERTEX[3].z = 0.5f;
        QUAD_VERTEX[3].attribute_bits = 0x4b000000;
        QUAD_VERTEX[2].attribute_bits = 0x4b000000;
        QUAD_VERTEX[1].attribute_bits = 0x4b000000;
        QUAD_VERTEX[0].attribute_bits = 0x4b000000;
        QUAD_VERTEX[3].w = 1.0f;
        QUAD_VERTEX[2].w = 1.0f;
        QUAD_VERTEX[1].w = 1.0f;
        QUAD_VERTEX[0].w = 1.0f;
        RendererQuadVertexBufferPc->methods->unlock(RendererQuadVertexBufferPc);
    }
#undef QUAD_VERTEX
    void __cdecl NuRndrGeometryBuffersInitPc(void) {
        u16 *indices;
        i32 i;
        u32 base;
        if (RendererGeometryVertexBufferPc != NULL) {
            RendererGeometryVertexBufferPc->methods->release(RendererGeometryVertexBufferPc);
            RendererGeometryVertexBufferPc = NULL;
        }
        if (RendererGeometryIndexBufferPc != NULL) {
            RendererGeometryIndexBufferPc->methods->release(RendererGeometryIndexBufferPc);
            RendererGeometryIndexBufferPc = NULL;
        }
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE_PC **)&RendererGeometryVertexBufferPc);
        PCDeviceCreateVertexBuffer(0x40000, 0x208, 0, 0, &RendererGeometryVertexBufferPc);
        NuRndrQuadBufferInitPc();
        PCDeviceCreateIndexBuffer(0xc000, 0, 0x65, 1, &RendererGeometryIndexBufferPc, NULL);
        if (RendererGeometryIndexBufferPc->methods->lock(RendererGeometryIndexBufferPc, 0, 0, (void **)&indices, 0) < 0)
            return;
        base = 0;
        for (i = 0; i < 0x1000; ++i) {
            *indices++ = (u16)base;
            *indices++ = (u16)(base + 1);
            *indices++ = (u16)(base + 2);
            *indices++ = (u16)base;
            *indices++ = (u16)(base + 2);
            *indices++ = (u16)(base + 3);
            base += 4;
        }
        RendererGeometryIndexBufferPc->methods->unlock(RendererGeometryIndexBufferPc);
    }
    void __cdecl NuRndrBufferDescriptorsInitPc(VARIPTR *buffer, VARIPTR end) {
        PC_VERTEX_ELEMENT_PC elements[3];
        elements[0].stream = 0;
        elements[0].offset = 0;
        elements[0].type = 2;
        elements[0].method = 0;
        elements[0].usage = 0;
        elements[0].usage_index = 0;
        elements[1].stream = 0;
        elements[1].offset = 12;
        elements[1].type = 4;
        elements[1].method = 0;
        elements[1].usage = 10;
        elements[1].usage_index = 0;
        elements[2].stream = 0xff;
        elements[2].offset = 0;
        elements[2].type = 17;
        elements[2].method = 0;
        elements[2].usage = 0;
        elements[2].usage_index = 0;
        PCDeviceCreateVertexDeclaration(elements, &RendererVertexDeclarationPc);
        NuRndrArenaInitPc(buffer, end);
        NuRndrQuadBufferInitPc();
        NuRndrGeometryBuffersInitPc();
    }
    extern u32 PCGraphicsPostEffectWidthPc;
    extern u32 PCGraphicsPostEffectHeightPc;
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;
    void __cdecl PCGraphicsCreatePostEffectWorkTexture(i32 width, i32 height);
    /* Sizes the post-effect targets (power-of-two when the device needs it) and claims the back-buffer copy. */
    void __cdecl PCGraphicsCreatePostEffectTextures(u32 width, u32 height) {
        GRAPHICS_RENDER_TEXTURE_PC *texture;
        i32 index;
        PCGraphicsPostEffectWidthPc = width;
        if (width >= 512)
            PCGraphicsPostEffectWidthPc = 512;
        PCGraphicsPostEffectHeightPc = height;
        if (height >= 512)
            PCGraphicsPostEffectHeightPc = 512;
        if ((PCDeviceBackend.caps.texture_caps & 2) != 0 || (PCDeviceBackend.caps.texture_caps & 0x100) != 0) {
            u32 width_power = 1;
            u32 height_power = 1;
            while (width_power <= (width < 512 ? width : 512))
                width_power += width_power;
            PCGraphicsPostEffectWidthPc = width_power >> 1;
            while (height_power <= (height < 512 ? height : 512))
                height_power += height_power;
            PCGraphicsPostEffectHeightPc = height_power >> 1;
        }
        PCGraphicsCreatePostEffectWorkTexture(width, height);
        for (index = 0; index != 32; ++index) {
            if (PCGraphicsRenderTextureActive[index] == 0) {
                PCGraphicsRenderTextureActive[index] = 1;
                texture = &PCGraphicsRenderTextures[index];
                goto allocated;
            }
        }
        PCGraphicsRenderTextureAllocationError();
        texture = NULL;
    allocated:
        PCGraphicsInitRenderTexture(texture, PCGraphicsPostEffectWidthPc, PCGraphicsPostEffectHeightPc, 0, 1, 0, 2);
        PCGraphicsPostEffectBackBufferTexturePc = texture;
    }
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectAuxTexturePc;
    /* Re-sizes the post-effect targets after a mode change, with the same clamps as PCGraphicsCreatePostEffectTextures.
     */
    void __cdecl PCGraphicsResizePostEffectTexturesPc(u32 width, u32 height) {
        u32 target_width = width < 512 ? width : 512;
        u32 target_height;
        PCGraphicsPostEffectWidthPc = target_width;
        target_height = height < 512 ? height : 512;
        PCGraphicsPostEffectHeightPc = target_height;
        if ((PCDeviceBackend.caps.texture_caps & 2) != 0 || (PCDeviceBackend.caps.texture_caps & 0x100) != 0) {
            target_width = 1;
            target_height = 1;
            while (target_width <= (width < 512 ? width : 512))
                target_width += target_width;
            target_width >>= 1;
            PCGraphicsPostEffectWidthPc = target_width;
            while (target_height <= (height < 512 ? height : 512))
                target_height += target_height;
            target_height >>= 1;
            PCGraphicsPostEffectHeightPc = target_height;
        }
        PCGraphicsResizeEffectTexture(PCGraphicsPostEffectWorkTexturePc, target_width, target_height, -1);
        PCGraphicsResizeEffectTexture(PCGraphicsPostEffectBackBufferTexturePc, PCGraphicsPostEffectWidthPc,
                                      PCGraphicsPostEffectHeightPc, -1);
        if (PCGraphicsPostEffectAuxTexturePc != NULL)
            PCGraphicsResizeEffectTexture(PCGraphicsPostEffectAuxTexturePc, -1, -1, -1);
    }
    void __cdecl NuRndrInitConfigPc(VARIPTR *buffer, VARIPTR end) {
        RENDERER_SHADOW_DEFAULTS_PC *shadow = (RENDERER_SHADOW_DEFAULTS_PC *)&PCShadowState;
        NUVEC defaults;
        i32 i;
        RendererConfigEnabledPc = 1;
        RendererConfigSecondEnabledPc = 1;
        NuMtxSetIdentity(&RendererConfigMatrixPc);
        NuRndrBufferDescriptorsInitPc(buffer, end);
        defaults.x = 0.0f;
        defaults.y = -0.75f;
        defaults.z = -0.75f;
        *(NUVEC *)&shadow->state0 = defaults;
        PCShadowNearValue = 4.0f;
        PCShadowFarValue = 6.0f;
        shadow->packed_colour = 0x00101010;
        NuRndrShadowCnt = 0;
        for (i = 0; i < 32; ++i)
            RendererClearRecordsPc[i].active = 0;
        NuRndrInitGeneric();
    }
    typedef struct PC_STARTUP_MATERIAL_READER_PC {
        u8 unknown_00[0x40];
        u32 render_flags;
        u32 secondary_flags;
        u8 unknown_48[0xc];
        NUVEC diffuse;
        u8 unknown_60[0x10];
        f32 opacity;
        i16 texture_id;
        i16 sort_priority;
    } PC_STARTUP_MATERIAL_READER_PC;
    STATIC_ASSERT(sizeof(PC_STARTUP_MATERIAL_READER_PC) == 0x78);
    STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, render_flags) == 0x40);
    STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, secondary_flags) == 0x44);
    STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, diffuse) == 0x54);
    STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, opacity) == 0x70);
    STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, texture_id) == 0x74);
    STATIC_ASSERT(offsetof(PC_STARTUP_MATERIAL_READER_PC, sort_priority) == 0x76);
#define STARTUP_MATERIAL(material) ((PC_STARTUP_MATERIAL_READER_PC *)(material))
    void __cdecl NuRndrInitStartupMaterialsPc(void) {
        PCStartupMaterial1Pc = NuMtlCreate(1);
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->diffuse.z = STARTUP_MATERIAL(PCStartupMaterial1Pc)->diffuse.y =
            STARTUP_MATERIAL(PCStartupMaterial1Pc)->diffuse.x = 1.0f;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags & ~0x1000u) | 0x2000u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags & ~0x8000u) | 0x4000u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->opacity = 0.999f;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags & 0xfffffff1u) | 1u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->secondary_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->secondary_flags & ~2u) | 1u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags & ~0x200u) | 0x100u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags & ~0x800u) | 0x400u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial1Pc)->render_flags & ~0x10000u) | 0x20000u;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->texture_id = 0;
        STARTUP_MATERIAL(PCStartupMaterial1Pc)->sort_priority = 0x100;
        NuMtlUpdate(PCStartupMaterial1Pc);

        PCStartupMaterial2Pc = NuMtlCreate(1);
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->diffuse.z = STARTUP_MATERIAL(PCStartupMaterial2Pc)->diffuse.y =
            STARTUP_MATERIAL(PCStartupMaterial2Pc)->diffuse.x = 1.0f;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags & ~0x1000u) | 0x2000u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags & ~0x8000u) | 0x4000u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->opacity = 0.999f;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags & 0xfffffff2u) | 2u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->secondary_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->secondary_flags & ~2u) | 1u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags & ~0x200u) | 0x100u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags & ~0x800u) | 0x400u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags =
            (STARTUP_MATERIAL(PCStartupMaterial2Pc)->render_flags & ~0x10000u) | 0x20000u;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->texture_id = 0;
        STARTUP_MATERIAL(PCStartupMaterial2Pc)->sort_priority = 0x100;
        NuMtlUpdate(PCStartupMaterial2Pc);
    }
#undef STARTUP_MATERIAL
    void __cdecl NuRndrFx(i32 paused, void *context) {
        (void)context;
        NuLgtLaserDraw(paused);
        NuLgtArcLaserDraw(paused);
        if (NuRndrFxHeightOverridePc != 0.0f) {
            NuRndrFxHeightPc = NuRndrFxHeightOverridePc;
            NuRndrFxColourPc = 0xc04080c0;
        }
        if (paused != 0)
            NuRndrFxHeightPc = NuRndrFxPreviousHeightPc;
        else
            NuRndrFxPreviousHeightPc = NuRndrFxHeightPc;
        currentScene.clear_flags = 1;
    }
    i32 __cdecl NuRndrSetFxMtx(NUMTX *matrix) {
        return 1;
    }
} // extern "C"

/* nucamera's platform members: NuCameraSet, the extents clip test and the projection matrix. */
extern "C" {
    void __cdecl NuCameraSet(NUCAMERA *camera) {
        NuCameraSetEx(camera, 0);

        NuVpSetIdentityPc(&cmtx);
        NuMtxMulH(&vpc_vport_mtx, &vpmtx, &cmtx);
        vpc_sci_mtx = vpc_vport_mtx;

        PCDeviceSetTransform(2, &vmtx);
        PCDeviceSetTransform(3, &pmtx);
    }

#define NUCAMERA_CLIP_CODE_EX(code, v, near_enabled, horizontal, vertical)                                             \
    {                                                                                                                  \
        if ((v).z < global_camera.near_clip && (near_enabled))                                                         \
            (code) |= 0x20;                                                                                            \
        if ((v).z > far_clip)                                                                                          \
            (code) |= 0x10;                                                                                            \
        if ((v).x < -(v).z * (horizontal))                                                                             \
            (code) |= 1;                                                                                               \
        if ((v).x > (v).z * (horizontal))                                                                              \
            (code) |= 2;                                                                                               \
        if ((v).y < -(v).z * (vertical))                                                                               \
            (code) |= 8;                                                                                               \
        if ((v).y > (v).z * (vertical))                                                                                \
            (code) |= 4;                                                                                               \
    }

#define NUCAMERA_CLIP_CODE(code, v) NUCAMERA_CLIP_CODE_EX(code, v, 1, zx, zy)

    i32 __cdecl NuCameraClipTestExtents(NUVEC *min, NUVEC *max, NUMTX *world, f32 far_clip, i32 clip_screen) {
        NUVEC corners[8];
        NUVEC transformed[8];
        u8 codes[8];
        i32 i;

        if (PCClipExtentsEnabled == 0)
            return 1;
        if (clip_screen != 0) {
            PCClipScreenUnsupported();
            for (;;) {
            }
        }
        if (far_clip == 0.0f)
            far_clip = global_camera.unknown_64 != 0.0f ? global_camera.unknown_64 : global_camera.far_clip;
        if (world == NULL || world == &NuMtxSetIdentityTemplate)
            clip_test_mtx = vmtx;
        else
            NuMtxMul(&clip_test_mtx, world, &vmtx);

        ClipFarLimits[3] = far_clip;
        ClipFarLimits[2] = far_clip;
        ClipFarLimits[1] = far_clip;
        ClipFarLimits[0] = far_clip;
        if (PCClipUseSSE == 0) {
            corners[0].x = min->x;
            corners[0].y = min->y;
            corners[0].z = min->z;
            corners[1].x = max->x;
            corners[1].y = max->y;
            corners[1].z = max->z;
            corners[2].x = min->x;
            corners[2].y = min->y;
            corners[2].z = max->z;
            corners[3].x = max->x;
            corners[3].y = max->y;
            corners[3].z = min->z;
            corners[4].x = min->x;
            corners[4].y = max->y;
            corners[4].z = min->z;
            corners[5].x = min->x;
            corners[5].y = max->y;
            corners[5].z = max->z;
            corners[6].x = max->x;
            corners[6].y = min->y;
            corners[6].z = min->z;
            corners[7].x = max->x;
            corners[7].y = min->y;
            corners[7].z = max->z;
            NuVecMtxTransformBlock(transformed, corners, &clip_test_mtx, 8);
            memset(codes, 0, sizeof(codes));
            for (i = 0; i < 8; i += 4) {
                NUCAMERA_CLIP_CODE(codes[i], transformed[i]);
                NUCAMERA_CLIP_CODE(codes[i + 1], transformed[i + 1]);
                NUCAMERA_CLIP_CODE(codes[i + 2], transformed[i + 2]);
                NUCAMERA_CLIP_CODE(codes[i + 3], transformed[i + 3]);
            }
            if ((codes[0] & codes[1] & codes[2] & codes[3] & codes[4] & codes[5] & codes[6] & codes[7]) != 0)
                return 0;
            return (codes[0] | codes[1] | codes[2] | codes[3] | codes[4] | codes[5] | codes[6] | codes[7]) != 0 ? 2 : 1;
        } else {
            u32 *outcode = clip_outcode_workspace_prefix;
            ClipCornerBlocks[0].x[0] = min->x;
            ClipCornerBlocks[0].y[0] = min->y;
            ClipCornerBlocks[0].z[0] = min->z;
            ClipCornerBlocks[0].x[1] = max->x;
            ClipCornerBlocks[0].y[1] = max->y;
            ClipCornerBlocks[0].z[1] = max->z;
            ClipCornerBlocks[0].x[2] = min->x;
            ClipCornerBlocks[0].y[2] = min->y;
            ClipCornerBlocks[0].z[2] = max->z;
            ClipCornerBlocks[0].x[3] = max->x;
            ClipCornerBlocks[0].y[3] = max->y;
            ClipCornerBlocks[0].z[3] = min->z;
            ClipCornerBlocks[1].x[0] = min->x;
            ClipCornerBlocks[1].y[0] = max->y;
            ClipCornerBlocks[1].z[0] = min->z;
            ClipCornerBlocks[1].x[1] = min->x;
            ClipCornerBlocks[1].y[1] = max->y;
            ClipCornerBlocks[1].z[1] = max->z;
            ClipCornerBlocks[1].x[2] = max->x;
            ClipCornerBlocks[1].y[2] = min->y;
            ClipCornerBlocks[1].z[2] = min->z;
            ClipCornerBlocks[1].x[3] = max->x;
            ClipCornerBlocks[1].y[3] = min->y;
            ClipCornerBlocks[1].z[3] = max->z;
            PCVecMtxTransformClipBlocks(ClipTransformedBlocks, ClipCornerBlocks, &clip_test_mtx, 8, outcode);
            if ((clip_outcode_workspace_prefix[7] & clip_outcode_workspace_prefix[6] &
                 clip_outcode_workspace_prefix[5] & clip_outcode_workspace_prefix[4] &
                 clip_outcode_workspace_prefix[3] & clip_outcode_workspace_prefix[2] &
                 clip_outcode_workspace_prefix[1] & clip_outcode_workspace_prefix[0]) != 0)
                return 0;
            return (clip_outcode_workspace_prefix[7] | clip_outcode_workspace_prefix[6] |
                    clip_outcode_workspace_prefix[5] | clip_outcode_workspace_prefix[4] |
                    clip_outcode_workspace_prefix[3] | clip_outcode_workspace_prefix[2] |
                    clip_outcode_workspace_prefix[1] | clip_outcode_workspace_prefix[0]) != 0
                       ? 2
                       : 1;
        }
    }

#undef NUCAMERA_CLIP_CODE
#undef NUCAMERA_CLIP_CODE_EX

    void __cdecl NuCameraSetProjectionMtx(NUMTX *out, f32 fov, f32 aspect, f32 near_clip, f32 far_clip) {
        f32 half_fov = (f32)((f64)fov * 0.5);
        i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
        f32 cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        f32 depth_factor = far_clip / (far_clip - near_clip);
        memset(out, 0, sizeof(*out));
        out->m00 = aspect * cotangent;
        out->m11 = cotangent;
        out->m22 = depth_factor;
        out->m23 = 1.0f;
        out->m32 = -depth_factor * near_clip;
        pmtx.m00 *= NuCameraProjectionGlobalScaleX;
        pmtx.m11 *= NuCameraProjectionGlobalScaleY;
        pmtx.m20 = NuCameraProjectionGlobalOffsetX;
        pmtx.m21 = NuCameraProjectionGlobalOffsetY;
    }
} // extern "C"

/* nuvec's platform members: the block transforms. */
extern "C" {
    void __cdecl NuVecMtxTransformBlock(NUVEC *out, NUVEC *vectors, NUMTX *matrix, i32 count) {
        i32 index;

        for (index = 0; index < count; ++index) {
            out->x = vectors->x * matrix->m00 + vectors->y * matrix->m10 + vectors->z * matrix->m20 + matrix->m30;
            out->y = vectors->x * matrix->m01 + vectors->y * matrix->m11 + vectors->z * matrix->m21 + matrix->m31;
            out->z = vectors->x * matrix->m02 + vectors->y * matrix->m12 + vectors->z * matrix->m22 + matrix->m32;
            out++;
            vectors++;
        }
    }

    void __cdecl PCVecMtxTransformClipBlocks(NUVEC4BLOCK_PC *destination, NUVEC4BLOCK_PC *source, NUMTX *matrix,
                                             i32 count, u32 *outcodes) {
        count = (count + 3) >> 2;
        if (count <= 0)
            return;
        __asm {
        push esi
        push edi
        push ebx
        push ecx
        push edx
        mov edi, destination
        mov esi, source
        mov ebx, outcodes
        mov ecx, matrix
        mov edx, count
        movaps xmm4, [ecx]
        movaps xmm5, [ecx + 0x10]
        movaps xmm6, [ecx + 0x20]
        movaps xmm7, [ecx + 0x30]
    next_block:
        movaps xmm0, [esi]
        movaps xmm1, xmm4
        shufps xmm1, xmm1, 0x0
        mulps xmm1, xmm0
        movaps xmm0, [esi + 0x10]
        movaps xmm2, xmm5
        shufps xmm2, xmm2, 0x0
        mulps xmm2, xmm0
        movaps xmm0, [esi + 0x20]
        movaps xmm3, xmm6
        shufps xmm3, xmm3, 0x0
        mulps xmm3, xmm0
        movaps xmm0, xmm7
        shufps xmm0, xmm0, 0x0
        addps xmm0, xmm1
        addps xmm0, xmm2
        addps xmm0, xmm3
        movaps [edi], xmm0
        add edi, 0x10
        movaps xmm0, [esi]
        movaps xmm1, xmm4
        shufps xmm1, xmm1, 0x55
        mulps xmm1, xmm0
        movaps xmm0, [esi + 0x10]
        movaps xmm2, xmm5
        shufps xmm2, xmm2, 0x55
        mulps xmm2, xmm0
        movaps xmm0, [esi + 0x20]
        movaps xmm3, xmm6
        shufps xmm3, xmm3, 0x55
        mulps xmm3, xmm0
        movaps xmm0, xmm7
        shufps xmm0, xmm0, 0x55
        addps xmm0, xmm1
        addps xmm0, xmm2
        addps xmm0, xmm3
        movaps [edi], xmm0
        add edi, 0x10
        movaps xmm0, [esi]
        movaps xmm1, xmm4
        shufps xmm1, xmm1, 0xaa
        mulps xmm1, xmm0
        movaps xmm0, [esi + 0x10]
        movaps xmm2, xmm5
        shufps xmm2, xmm2, 0xaa
        mulps xmm2, xmm0
        movaps xmm0, [esi + 0x20]
        movaps xmm3, xmm6
        shufps xmm3, xmm3, 0xaa
        mulps xmm3, xmm0
        movaps xmm0, xmm7
        shufps xmm0, xmm0, 0xaa
        addps xmm0, xmm1
        addps xmm0, xmm2
        addps xmm0, xmm3
        movaps [edi], xmm0
        add edi, 0x10
        movaps xmm3, xmm0
        movss xmm1, global_camera.near_clip
        shufps xmm1, xmm1, 0x0
        cmpltps xmm0, xmm1
        andps xmm0, ClipOutcodeMask32
        movaps xmm2, xmm0
        movaps xmm0, xmm3
        cmpnleps xmm0, ClipFarLimits
        andps xmm0, ClipOutcodeMask16
        orps xmm2, xmm0
        movss xmm1, zx
        shufps xmm1, xmm1, 0x0
        mulps xmm1, xmm3
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x30]
        cmpltps xmm0, xmm1
        andps xmm0, ClipOutcodeMask1
        orps xmm2, xmm0
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x30]
        cmpnleps xmm0, xmm1
        andps xmm0, ClipOutcodeMask2
        orps xmm2, xmm0
        movss xmm1, zy
        shufps xmm1, xmm1, 0x0
        mulps xmm1, xmm3
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x20]
        cmpltps xmm0, xmm1
        andps xmm0, ClipOutcodeMask8
        orps xmm2, xmm0
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x20]
        cmpnleps xmm0, xmm1
        andps xmm0, ClipOutcodeMask4
        orps xmm2, xmm0
        movaps [ebx], xmm2
        add ebx, 0x10
        add esi, 0x30
        dec edx
        jne next_block
        pop edx
        pop ecx
        pop ebx
        pop edi
        pop esi
        }
    }
} // extern "C"
