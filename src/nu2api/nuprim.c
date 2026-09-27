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
