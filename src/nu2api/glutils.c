#include "saga.h"
extern const f32 DebrisGlassMaterialOpacity;

typedef struct debris_material_flags_pc_s {
    u32 alpha_mode : 4;       // 0x0:0
    u32 filter_mode : 2;      // 0x0:4
    u32 unknown_0_64_128 : 2; // 0x0:6
    u32 unknown_1_1_2 : 2;    // 0x0:8
    u32 unknown_1_4_8 : 2;    // 0x0:10
    u32 cull_mode : 2;        // 0x0:12
    u32 z_mode : 2;           // 0x0:14
    u32 unknown_2_1_2 : 2;    // 0x0:16
    u32 unknown_2_4 : 1;      // 0x0:18
    u32 unknown_19 : 1;       // 0x0:19
    u32 alpha_test : 2;       // 0x0:20
    u32 unknown_22_31 : 10;   // 0x0:22
} DEBRIS_MATERIAL_FLAGS_PC;   // size 0x4

typedef struct debris_material_secondary_pc_s {
    u32 alpha_fail : 2;          // 0x0:0
    u32 disable_depth_write : 1; // 0x0:2
    u32 unknown_03_31 : 29;      // 0x0:3
} DEBRIS_MATERIAL_SECONDARY_PC;  // size 0x4

typedef struct debris_material_pc_s {
    u8 unknown_000[0x40];                   // 0x000
    DEBRIS_MATERIAL_FLAGS_PC attribs;       // 0x040
    DEBRIS_MATERIAL_SECONDARY_PC secondary; // 0x044
    u8 unknown_048[0xc];                    // 0x048
    NUVEC diffuse;                          // 0x054
    f32 glass_parameter;                    // 0x060
    u8 unknown_064[0xc];                    // 0x064
    f32 opacity;                            // 0x070
    i16 texture_id;                         // 0x074
    i16 sort_priority;                      // 0x076
    u8 unknown_078[0x21];                   // 0x078
    i8 particle_type_tag;                   // 0x099
    u8 unknown_09a[0x156];                  // 0x09a
    u32 vertex_flags;                       // 0x1f0
} DEBRIS_MATERIAL_PC;                       // size 0x1f4

NUMTL *__cdecl CreateAlphaBlendTexture(VARIPTR *buffer, VARIPTR buffer_end, char *name, i32 disable_depth_write,
                                       i32 alpha_mode, i32 sort_priority, i32 depth_mode) {
    i32 texture_id = NuTexRead(name, buffer, buffer_end);
    DEBRIS_MATERIAL_PC *material;
    buffer->addr = (buffer->addr + 15) & ~15u;
    material = (DEBRIS_MATERIAL_PC *)NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->opacity = DebrisGlassMaterialOpacity;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.unknown_2_1_2 = 2;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;

    if (disable_depth_write != 0) {
        material->secondary.disable_depth_write = 1;
    }
    material->texture_id = (i16)texture_id;
    material->sort_priority = (i16)sort_priority;
    NuMtlUpdate((NUMTL *)material);
    return (NUMTL *)material;
}
