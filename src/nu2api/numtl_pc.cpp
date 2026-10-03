#include "saga.h"
typedef struct PC_MATERIAL_SHADER_DESC_VIEW {
    u8 unknown_00[4];
    i32 diffuse_map_id;
    u8 unknown_08[0xc];
    u32 packed_diffuse_colour;
    u8 unknown_18[0x90];
    u8 texture_unit_count;
    u8 unknown_a9[0x93];
    u32 vertex_format;
    i16 shader_handle;
    i16 shader_variant_handle;
    u8 unknown_144[0xc4];
} PC_MATERIAL_SHADER_DESC_VIEW;
STATIC_ASSERT(sizeof(PC_MATERIAL_SHADER_DESC_VIEW) == 0x208);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, diffuse_map_id) == 4);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, packed_diffuse_colour) == 0x14);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, texture_unit_count) == 0xa8);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, vertex_format) == 0x13c);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, shader_handle) == 0x140);
STATIC_ASSERT(offsetof(PC_MATERIAL_SHADER_DESC_VIEW, shader_variant_handle) == 0x142);
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
extern "C" void __cdecl NuMtlUpdatePS(NUMTL *material) {
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

typedef struct PC_MATERIAL_CREATE_SHADER PC_MATERIAL_CREATE_SHADER;
extern "C" void __cdecl NuShaderMtlDescInit(PC_MATERIAL_CREATE_SHADER *);
extern "C" void __cdecl NuMtlCreatePS(NUMTL *material, i32 is_3d) {
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
