#include "saga.h"
extern const f32 DebrisGlassMaterialOpacity;
typedef struct debris_material_flags_pc_s {
    u32 alpha_mode : 4, filter_mode : 2, unknown_0_64_128 : 2, unknown_1_1_2 : 2, unknown_1_4_8 : 2, cull_mode : 2,
        z_mode : 2, unknown_2_1_2 : 2, unknown_2_4 : 1, unknown_19 : 1, alpha_test : 2, unknown_22_31 : 10;
} DEBRIS_MATERIAL_FLAGS_PC;
typedef struct debris_material_secondary_pc_s {
    u32 alpha_fail : 2, disable_depth_write : 1, unknown_03_31 : 29;
} DEBRIS_MATERIAL_SECONDARY_PC;
typedef struct debris_material_pc_s {
    u8 unknown_000[0x40];
    DEBRIS_MATERIAL_FLAGS_PC attribs;
    DEBRIS_MATERIAL_SECONDARY_PC secondary;
    u8 unknown_048[0xc];
    NUVEC diffuse;
    f32 glass_parameter;
    u8 unknown_064[0xc];
    f32 opacity;
    i16 texture_id, sort_priority;
    u8 unknown_078[0x21];
    i8 particle_type_tag;
    u8 unknown_09a[0x156];
    u32 vertex_flags;
} DEBRIS_MATERIAL_PC;
STATIC_ASSERT(sizeof(DEBRIS_MATERIAL_FLAGS_PC) == 4);
STATIC_ASSERT(sizeof(DEBRIS_MATERIAL_SECONDARY_PC) == 4);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, attribs) == 0x40);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, secondary) == 0x44);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, glass_parameter) == 0x60);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, sort_priority) == 0x76);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, particle_type_tag) == 0x99);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, vertex_flags) == 0x1f0);
i32 __cdecl NuTexReadPc(char *, VARIPTR *, VARIPTR);
NUMTL *__cdecl CreateAlphaBlendTexture(VARIPTR *, VARIPTR, char *, i32, i32, i32, i32);
NUMTL *__cdecl CreateCopyMat(NUMTL *, i32, i32, i32, i32);
NUMTL *__cdecl CreateAlphaBlendTexture(VARIPTR *buffer, VARIPTR buffer_end, char *name, i32 disable_depth_write,
                                       i32 alpha_mode, i32 sort_priority, i32 depth_mode) {
    i32 texture_id = NuTexReadPc(name, buffer, buffer_end);
    DEBRIS_MATERIAL_PC *material;
    buffer->addr = (buffer->addr + 15) & ~15u;
    material = (DEBRIS_MATERIAL_PC *)NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.unknown_2_1_2 = 2;
    material->opacity = DebrisGlassMaterialOpacity;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;
    if (disable_depth_write != 0)
        material->secondary.disable_depth_write = 1;
    material->texture_id = (i16)texture_id;
    material->sort_priority = (i16)sort_priority;
    NuMtlUpdate((NUMTL *)material);
    return (NUMTL *)material;
}
