// PC platform unit linked after nu3d (retail 0x5f7490-0x60df00), being rebuilt one module at a time.
// Retail compiled the nu3d\pc sources (assert paths name nugscn_dlist.c, numtl_dlist.c, nustream.c,
// nugobj.c, nutex_pc.cpp and nuqfnt_PC.cpp) together with the PS/Pc halves of the generic modules as
// one C++ unit; nu3d only ever calls into it, never inlines from it. Definitions follow retail address
// order. Modules merged so far: nugscn_pc, nuportal_pc, numtl PS/Pc, the nudlist and nupostfilter
// platform members, the render-state/stream/scene/gobj/particle backends and the geometry backend.

#include "saga.h"

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
    typedef struct PC_GEOMETRY_BASIS_PC {
        NUMTX matrices[3];
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
    // @@FUNCTIONS

    /* The PC backend's transform-update leaf is a single ret instruction; retail
     * reaches it through a jump thunk, so callers in other units never inline it. */
    void __cdecl DisplayListUpdateSpecialTransformPS(NUSPECIALHANDLE *special, NUMTX *matrix) {
        (void)special;
        (void)matrix;
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
        for (i32 i = 0; i < 16; ++i) {
            PCDeviceSamplerInvalidation[i][0] = 0x012449d0;
            PCDeviceSamplerInvalidation[i][1] = 0;
        }
    }

    void __cdecl PcUpdateGeometryCameraOriginPc(NUMTX *view, NUMTX *projection) {
        NUVEC scale = {PCSpecialProjectionAdjust.x_scale, PCSpecialProjectionAdjust.y_scale, 1.0f};
        NUVEC translation = {PCSpecialProjectionAdjust.x_offset, PCSpecialProjectionAdjust.y_offset, 0.0f};
        NuMtxSetScale(&PcGeometryScaleMatrixPc, &scale);
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
        f32 left, right, bottom, top;
        NuMtxGetFrustumD3D(&PcGeometryProjectionMatrixPc, &left, &right, &bottom, &top, &z_near, &z_far);
        NUVEC4 frustum = {left, right - left, bottom, top - bottom};
        NuShaderManagerSetfv(0x47, &frustum.x);
    }

    void __cdecl PCGraphicsDestroyPostEffectTexturesPc(void) {
        NuPostEffectReleasePortsPc();
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectBackBufferTexturePc);
    }

    void NuDeferredFilter::initTextureResources(i32 width, i32 height) {
        NuDeferredFilterGen::initTextureResources(width, height);
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
        f32 denominator = (f32)(vertices - 3);
        for (i32 i = 0; i < vertices; ++i) {
            strip[i].fraction = (f32)(i - 1) / denominator;
            if (0.0f > strip[i].fraction)
                strip[i].fraction = 0.0f;
            if (1.0f < strip[i].fraction)
                strip[i].fraction = 1.0f;
            strip[i].side = (i & 1) ? -0.5f : 0.5f;
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
                NUVEC products[3][3], combined[3], axes[3];
                for (i32 axis = 0; axis < 3; ++axis) {
                    NUMTX *m = &basis->matrices[axis];
                    f32 weight = axis == 0   ? input->basis_weights.x
                                 : axis == 1 ? input->basis_weights.y
                                             : input->basis_weights.z;
                    products[axis][0].x = weight * m->m00;
                    products[axis][0].y = weight * m->m01;
                    products[axis][0].z = weight * m->m02;
                    products[axis][1].x = weight * m->m10;
                    products[axis][1].y = weight * m->m11;
                    products[axis][1].z = weight * m->m12;
                    products[axis][2].x = weight * m->m20;
                    products[axis][2].y = weight * m->m21;
                    products[axis][2].z = weight * m->m22;
                }
                for (i32 row = 0; row < 3; ++row) {
                    combined[row].x = products[0][row].x + products[1][row].x;
                    combined[row].y = products[0][row].y + products[1][row].y;
                    combined[row].z = products[0][row].z + products[1][row].z;
                    axes[row].x = combined[row].x + products[2][row].x;
                    axes[row].y = combined[row].y + products[2][row].y;
                    axes[row].z = combined[row].z + products[2][row].z;
                }
                NUVEC cross;
                NuVecCross(&cross, &axes[1], &axes[2]);
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
                matrix.m00 = axes[2].x;
                matrix.m01 = axes[2].y;
                matrix.m02 = axes[2].z;
                matrix.m03 = 0;
                matrix.m10 = cross.x;
                matrix.m11 = cross.y;
                matrix.m12 = cross.z;
                matrix.m13 = 0;
                matrix.m20 = axes[1].x;
                matrix.m21 = axes[1].y;
                matrix.m22 = axes[1].z;
                matrix.m23 = 0;
                matrix.m30 = axes[0].x;
                matrix.m31 = axes[0].y;
                matrix.m32 = axes[0].z;
                matrix.m33 = 1;
                for (i32 corner = 0; corner < 4; ++corner)
                    NuVecMtxTransform(&corners[corner].position, &corners[corner].position, &matrix);
                output->basis_axis = axes[1];
                output->distance_squared = 0;
                for (i32 corner = 0; corner < 4; ++corner)
                    corners[corner].w = input->output_w[corner];
                ++*output_index;
            }
        }
        return 1;
    }

    i32 __cdecl PcUpdateGeometryDistancesPc(PC_GEOMETRY_OUTPUT_PC *output, PC_GEOMETRY_SORT_ITEM_PC *items, i32 count) {
        NUVEC4 origin = PcGeometryDistanceOriginPc;
        for (i32 i = 0; i < count; ++i) {
            f32 dx = origin.x - output[i].corners[2].position.x;
            f32 dy = origin.y - output[i].corners[2].position.y;
            f32 dz = origin.z - output[i].corners[2].position.z;
            output[i].distance_squared = (f32)(((f64)dx * dx + (f64)dy * dy) + (f64)dz * dz);
        }
        for (i32 i = 0; i < count; ++i)
            items[i].distance_squared = output[items[i].output_index].distance_squared;
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
        vertical = (f64)PS2_REZ_H / 224.0;
        render_state.viewport_x = (f32)(render_state.viewport_x * horizontal);
        render_state.viewport_width = (f32)(render_state.viewport_width * horizontal);
        render_state.viewport_y = (f32)(render_state.viewport_y * vertical);
        render_state.viewport_height = (f32)(render_state.viewport_height * vertical);
        render_state.camera_packet_cache = NULL;
        ++render_state.counter_0020;
        ++render_state.camera_id;
        return 1;
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

    static void __cdecl NuRndrResetStreamBufferPc(void) {
        if (RndrStreamIndexPc == 0) {
            nudlist_stream_cursor = RndrStreamFirstPc;
            nuprim_field_ba8 = RndrStreamFirstPc.addr + RndrStreamSizePc;
        } else if (RndrStreamIndexPc == 1) {
            nudlist_stream_cursor = RndrStreamSecondPc;
            nuprim_field_ba8 = RndrStreamSecondPc.addr + RndrStreamSizePc;
        }
        if (nudlist_stream_cursor.addr & 15)
            nudlist_stream_cursor.addr = (nudlist_stream_cursor.addr + 15) & ~15u;
        SAGA_SKIN_CALL(skin_trace_stream, (unsigned)RndrStreamIndexPc, nudlist_stream_cursor.addr,
                       nuprim_field_ba8 - nudlist_stream_cursor.addr);
    }

    void __cdecl NuRndrSwapStreamBuffers(void) {
        if (RndrStreamIndexPc == 0)
            RndrStreamUsedPc = nudlist_stream_cursor.addr - RndrStreamFirstPc.addr;
        else if (RndrStreamIndexPc == 1)
            RndrStreamUsedPc = nudlist_stream_cursor.addr - RndrStreamSecondPc.addr;
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

    i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *handle, NUMTX *matrix, f32 alpha) {
        i32 result;
        if (handle->scene == NULL || alpha <= 0.0f)
            return 0;
        if (alpha < 1.0f) {
            NuSpecialConstAlpha(1, alpha);
            result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
            NuSpecialConstAlpha(0, 0.0f);
            return result;
        }
        return NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
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

    DEBRIS_PARTICLE_CHUNK_PC *__cdecl CreateDmaParticleSet(void *memory, i32 *size) {
        DEBRIS_PARTICLE_CHUNK_PC *chunk = (DEBRIS_PARTICLE_CHUNK_PC *)memory;
        i32 i;
        chunk->command = 0x52;
        chunk->next = NULL;
        chunk->field_014 = 0;
        chunk->field_018 = 0;
        chunk->field_01c = 0;
        chunk->particles[0].position.x = 0.0f;
        for (i = 0; i < 32; ++i) {
            DEBRISPARTICLE_PC *particle = &chunk->particles[i];
            particle->position.x = 1.0f;
            particle->position.y = 2.0f;
            particle->position.z = 3.0f;
            particle->momentum.x = 4.0f;
            particle->momentum.y = 5.0f;
            particle->momentum.z = 6.0f;
            particle->start_time = -1.0f;
            particle->inverse_lifetime = 128.0f;
        }
        chunk->end_command = 0;
        *size = sizeof(*chunk);
        return chunk;
    }

    DEBRIS_PARTICLE_CHUNK_PC *__cdecl CreateDmaParticleSetGlass(void *memory, i32 *size) {
        return CreateDmaParticleSet(memory, size);
    }

    DEBRIS_PART_HEADER_PC *__cdecl CreateDmaPartEffectList(void *memory, i32 *size) {
        u32 address = ((u32)memory + 0xf) & ~0xf;
        *size = sizeof(DEBRIS_PART_HEADER_PC);
        return (DEBRIS_PART_HEADER_PC *)address;
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
            if (mtl->shader.texture_unit_count == 0)
                mtl->shader.texture_unit_count = 1;
            if ((mtl->shader.vertex_format & 0x3800) == 0)
                mtl->shader.vertex_format = (mtl->shader.vertex_format & 0xffffcfff) | 0x800;
        } else {
            u32 colour = (static_cast<i32>(mtl->diffuse.z * 255.0) & 0xff) | 0xffffff00;
            colour = (colour << 8) | (static_cast<i32>(mtl->diffuse.y * 255.0) & 0xff);
            colour = (colour << 8) | (static_cast<i32>(mtl->diffuse.x * 255.0) & 0xff);
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
            if (mtl->shader.texture_unit_count == 0)
                mtl->shader.texture_unit_count = 1;
            if ((mtl->shader.vertex_format & 0x3800) == 0)
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
            u32 colour = (static_cast<i32>(mtl->diffuse.z * 255.0) & 0xff) | 0xffffff00;
            colour <<= 8;
            colour |= static_cast<i32>(mtl->diffuse.y * 255.0) & 0xff;
            colour <<= 8;
            colour |= static_cast<i32>(mtl->diffuse.x * 255.0) & 0xff;
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
        mtl->shader = *description;
        mtl->diffuse.x = (mtl->shader.packed_diffuse_colour & 0xff) / 255.0;
        mtl->diffuse.y = ((mtl->shader.packed_diffuse_colour >> 8) & 0xff) / 255.0;
        mtl->diffuse.z = ((mtl->shader.packed_diffuse_colour >> 16) & 0xff) / 255.0;
    }

    void __cdecl NuMtlSetRenderPlanePc(NUMTL *material, i32 plane) {
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
        u8 released = 0;
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
            for (i = 0; i < 100; ++i) {
                if (PCRegisteredNativeScenes[i] == scene) {
                    PCRegisteredNativeScenes[i] = NULL;
                    break;
                }
            }
        }
        material = (PC_SCENE_MTL_VIEW *)scene->materials[0];
        for (i = 0; i < scene->material_count; ++i, ++material) {
            for (j = 0; j < 2; ++j) {
                if (material->shader_handles[j] > 0)
                    --PCShaderRecordFromHandle(material->shader_handles[j])->ref_count;
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
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddTexture(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = 0x80;
        item->id = 3;
        item->next = material;
    }

    void __cdecl NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddLightState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl DisplayListPrintItemPS(NUDISPLAYLISTITEM *item, i32 file_handle) {
        (void)item;
        (void)file_handle;
    }

    void __cdecl DisplayListSwapBuffersPS(void) {
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

    void __cdecl DisplayListSetAlphaPS(NUDISPLAYLISTITEM *item, NUDISPLAYLISTITEM *geometry, f32 alpha) {
        NUMTX *matrix = (NUMTX *)item->next;
        (void)geometry;
        if (alpha < 0.0f)
            alpha = 0.0f;
        if (alpha > 1.0f)
            alpha = 1.0f;
        matrix->m33 = alpha;
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
        NuGobjRegistryInitializedPc = 1;
        NuGobjRegistryFirstPc = NULL;
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
