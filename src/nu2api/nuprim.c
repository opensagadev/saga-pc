#include "saga.h"

extern i32 NuPrimCSPos;
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

void __cdecl NuDisplayListLinkMtl(NUDISPLAYLIST_PC *list, NUMTL *material) {
    NUPRIM_MATERIAL_STATE_PC *state = (NUPRIM_MATERIAL_STATE_PC *)list->render_state;
    NUPRIM_COMMAND_PC *commands;
    i32 texture_id = material->texture_id;

    if (state->material == material) {
        return;
    }

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
    ((NUPRIM_COMMAND_PC *)list->material_last)->target = NuDisplayListStreamCursor->void_ptr;

    if (texture_id > 0 && state->texture_id != texture_id) {
        commands = (NUPRIM_COMMAND_PC *)NuDisplayListStreamCursor->void_ptr;
        NuDisplayListStreamCursor->addr += 0x60;

        commands[0].type = 0x87;
        commands[0].id = 0;
        commands[0].target = NULL;
        commands[1].type = 0x87;
        commands[1].id = 0;
        commands[1].target = NULL;

        state->texture_id = texture_id;
        commands += 2;
    } else {
        NuDisplayListStreamCursor->addr += 0x40;
        commands = (NUPRIM_COMMAND_PC *)NuDisplayListStreamCursor->void_ptr;
    }

    commands[0].type = 0x80;
    commands[0].id = 3;
    commands[0].target = material;
    commands[1].type = 0x87;
    commands[1].id = 0;
    commands[1].target = NULL;
    commands[2].type = 0x87;
    commands[2].id = 0;
    commands[2].target = NULL;
    commands[3].type = 0x8d;
    commands[3].id = 1;
    commands[3].target = NULL;

    list->material_last = &commands[3];
    state->material = material;
}

void __cdecl NuDisplayListLinkItem(NUDISPLAYLIST_PC *list, u8 type, void *target) {
    NUPRIM_COMMAND_PC *item;
    NUPRIM_COMMAND_PC *geometry_command;

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
    item = (NUPRIM_COMMAND_PC *)NuDisplayListStreamCursor->void_ptr;
    ((NUPRIM_COMMAND_PC *)list->material_last)->target = item;

    item->type = type;
    item->id = 3;
    item->target = target != NULL ? target : item + 2;

    geometry_command = item + 1;
    geometry_command->type = 0x8d;
    geometry_command->id = 1;
    geometry_command->target = (NUPRIM_COMMAND_PC *)list->dynamic_geometry + 1;
    list->material_last = geometry_command;
    NuDisplayListStreamCursor->addr += 2 * sizeof(NUPRIM_COMMAND_PC);
}

void __cdecl NuPrim2DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material) {
    NUDISPLAYLIST_PC *list;
    NUPRIM_COMMAND_PC *first_command;
    NUPRIM_COMMAND_PC *geometry_command;
    NUPRIM_PACKET_PC *packet;
    u8 *material_used;
    i32 material_id;

    (void)vertex_format;
    if (material == NULL) {
        material = numtl_defaultmtl2d;
    }

    NuPrimNeedsOverbrightening = material->texture_id != 0;

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
    list = material->display_list;
    if (list == NULL) {
        STUBBED_NAMED(0x00973e90); /* Need the fallback list object before linking its material. */
        return;
    }

    list->scene->flags |= 2;
    material_used =
        (list->scene->flags & 0x8000) != 0 ? list->scene->material_used_back : list->scene->material_used_front;
    material_id = list->material_id;
    material_used[material_id >> 3] |= (u8)(1 << (material_id & 7));

    render_state.counter_001e++;
    render_state.counter_0020++;
    render_state.state_00f8 = 0;
    render_state.state_00f4 = 0;
    render_state.state_00fc = 0;
    render_state.state_0180 = 0;
    STUBBED_NAMED(0x00607590); /* DisplayListUpdateRenderState(list, &render_state). */

    first_command = (NUPRIM_COMMAND_PC *)NuDisplayListStreamCursor->void_ptr;
    ((NUPRIM_COMMAND_PC *)list->material_last)->target = first_command;
    list->items = first_command;

    NuDisplayListStreamCursor->addr += sizeof(NUPRIM_COMMAND_PC);
    geometry_command = (NUPRIM_COMMAND_PC *)NuDisplayListStreamCursor->void_ptr;
    geometry_command->type = 0x8d;
    geometry_command->id = 1;
    geometry_command->target = (NUPRIM_COMMAND_PC *)list->dynamic_geometry + 1;
    list->material_last = geometry_command;

    NuDisplayListStreamCursor->addr += sizeof(NUPRIM_COMMAND_PC);
    packet = (NUPRIM_PACKET_PC *)NuDisplayListStreamCursor->void_ptr;
    packet->primitive_type = primitive_type;
    packet->vertex_count = 0;

    first_command->type = 0x93;
    first_command->id = 3;
    first_command->target = packet;
    list->items = geometry_command;

    NuPrimStreamBufferPointer = NuDisplayListStreamCursor;
    NuPrimVertexCount = 0;
    NuPrimVertexCountPointer = &packet->vertex_count;
    NuPrimCurrentPrimitiveType = (i32)primitive_type;
    NuDisplayListStreamCursor->addr += sizeof(NUPRIM_PACKET_PC);
}

void __cdecl NuPrim3DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material, NUMTX *world_matrix) {
    NUDISPLAYLIST_PC *list;
    NUPRIM_COMMAND_PC *command;
    NUPRIM_TRANSFORM_PACKET_PC *transform;
    NUPRIM_PACKET_PC *geometry;

    (void)vertex_format;
    if (material == NULL) {
        material = numtl_defaultmtl3d;
    }

    NuPrimNeedsOverbrightening = material->texture_id != 0;

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
    list = material->display_list;
    if (list == NULL) {
        STUBBED_NAMED(0x00973e90); /* The fallback list object's full extent is unknown. */
        return;
    }

    list->scene->flags |= 2;
    {
        u8 *material_used =
            (list->scene->flags & 0x8000) != 0 ? list->scene->material_used_back : list->scene->material_used_front;
        i32 material_id = list->material_id;
        material_used[material_id >> 3] |= (u8)(1 << (material_id & 7));
    }

    STUBBED_NAMED(0x00607590); /* DisplayListUpdateRenderState(list, &render_state). */

    transform = (NUPRIM_TRANSFORM_PACKET_PC *)NuDisplayListStreamCursor->void_ptr;
    STUBBED_NAMED(0x00603580); /* DisplayListCreateGeomTransformPS fills this 0x40-byte packet. */
    NuDisplayListStreamCursor->addr += sizeof(NUPRIM_TRANSFORM_PACKET_PC);

    geometry = (NUPRIM_PACKET_PC *)NuDisplayListStreamCursor->void_ptr;
    geometry->primitive_type = primitive_type;
    NuPrimVertexCountPointer = &geometry->vertex_count;
    NuDisplayListStreamCursor->addr += sizeof(NUPRIM_PACKET_PC);

    command = (NUPRIM_COMMAND_PC *)list->items;
    command->type = 0x8c;
    command->id = 3;
    command->target = transform;
    list->items = command + 1;
    command = (NUPRIM_COMMAND_PC *)list->items;
    command->type = 0x82;
    command->id = 3;
    command->target = geometry;
    list->items = command + 1;
}

void __cdecl NuPrimSetCoordinateSystem(i32 scale_mode) {
    STUBBED_NAMED(0x00981c70); /* PC coordinate-system stack write; extent unknown. */

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

void __cdecl NuPrim2DEnd(void) {
    *NuPrimVertexCountPointer = NuPrimVertexCount;
    NuPrimVertexCount = 0;
}

void __cdecl NuPrim3DEnd(void) {
    *NuPrimVertexCountPointer = NuPrimVertexCount;
    NuPrimVertexCount = 0;
}

void __cdecl NuPrim2DAddXYZ(f32 x, f32 y, f32 z) {
    NUPRIMVERTEX_PC *vertices = (NUPRIMVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;

    vertices->x = NuPrimXScale * x + NuPrimXBias;
    vertices->y = NuPrimYScale * y + NuPrimYBias;
    vertices->z = z;
    NuPrimStreamBufferPointer->addr += sizeof(NUPRIMVERTEX_PC);
    NuPrimVertexCount++;

    if (NuPrimCurrentPrimitiveType == 0x10 && (NuPrimVertexCount & 1) == 0) {
        NUPRIMVERTEX_PC first = vertices[-1];
        NUPRIMVERTEX_PC second = vertices[0];

        vertices[0].x = second.x;
        vertices[0].y = first.y;
        vertices[0].z = first.z;
        vertices[0].colour = first.colour;
        vertices[0].u = second.u;
        vertices[0].v = first.v;

        vertices[1] = second;
        vertices[2] = second;

        vertices[3].x = first.x;
        vertices[3].y = second.y;
        vertices[3].z = second.z;
        vertices[3].colour = second.colour;
        vertices[3].u = first.u;
        vertices[3].v = second.v;
        vertices[4] = first;

        NuPrimStreamBufferPointer->addr += 4 * sizeof(NUPRIMVERTEX_PC);
        NuPrimVertexCount += 4;
    }
}
