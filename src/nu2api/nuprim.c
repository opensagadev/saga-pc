#include "saga.h"

#include <string.h>

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

void __cdecl NuDisplayListLinkMtl(NUDISPLAYLIST_PC *list, NUMTL *material) {
#define LINK_MTL_STATE ((NUPRIM_MATERIAL_STATE_PC *)list->render_state)
    NUPRIM_COMMAND_PC *command;
    VARIPTR *cursor;

    if (material == LINK_MTL_STATE->material) {
        return;
    }

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
    cursor = NuDisplayListStreamCursor;
    ((NUPRIM_COMMAND_PC *)list->material_last)->target = cursor->void_ptr;

    if (material->texture_id > 0 && material->texture_id != LINK_MTL_STATE->texture_id) {
        command = (NUPRIM_COMMAND_PC *)cursor->void_ptr;
        cursor->void_ptr = command + 6;
        command->type = 0x87;
        command->target = NULL;
        command->id = 0;
        command++;
        command->type = 0x87;
        command->target = NULL;
        command->id = 0;
        command++;
        LINK_MTL_STATE->texture_id = material->texture_id;
    } else {
        command = (NUPRIM_COMMAND_PC *)cursor->void_ptr;
        cursor->void_ptr = command + 4;
    }

    command->type = 0x80;
    command->target = material;
    command->id = 3;
    command++;
    command->type = 0x87;
    command->target = NULL;
    command->id = 0;
    command++;
    command->type = 0x87;
    command->target = NULL;
    command->id = 0;
    command++;
    command->target = NULL;
    command->type = 0x8d;
    command->id = 1;

    list->material_last = command;
    LINK_MTL_STATE->material = material;
}
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

void __cdecl NuPrim2DSetColor(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
    } else {
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
    }
    ((NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

void __cdecl NuPrim2DSetUV(f32 u, f32 v) {
    NUPRIMVERTEX_PC *vertex = (NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
    vertex->u = u;
    vertex->v = v;
}
