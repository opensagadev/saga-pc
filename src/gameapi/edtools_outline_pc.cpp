#include "saga.h"

/* Camera-facing outline helpers from the retail edtools unit, used by the
 * light editor. */

/* Draws one line between two vertices, optionally transformed by a matrix
 * (copied to aligned storage when it is not 16-byte aligned). */
static __declspec(noinline) void RndrVtxLine3d(NUMTX *matrix, NURND_VERTEX3D_PC *vertices) {
    __declspec(align(16)) NUVEC first = vertices[0].position;
    __declspec(align(16)) NUVEC second = vertices[1].position;
    if (matrix != NULL) {
        __declspec(align(16)) NUMTX matrix_copy;
        NUMTX *transform = matrix;
        if (((size_t)(transform) & 15) != 0) {
            matrix_copy = *transform;
            transform = &matrix_copy;
        }
        NuVecMtxTransform(&first, &first, transform);
        NuVecMtxTransform(&second, &second, transform);
    }
    NuRndrLine3dDbg(first.x, first.y, first.z, second.x, second.y, second.z, vertices[0].colour);
}

/* Camera-facing circle outline around centre. */
void __cdecl RndrOSphere(NUVEC *centre, f32 radius, i32 colour, i32 segments, NUMTL *) {
    NUMTX matrix = global_camera.mtx;
    NURND_VERTEX3D_PC vertices[2];
    i32 step;
    i32 i;
    matrix.m30 = centre->x;
    matrix.m31 = centre->y;
    matrix.m32 = centre->z;
    vertices[0].position.x = radius;
    vertices[0].position.y = 0.0f;
    vertices[0].position.z = 0.0f;
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    step = 0x10000 / segments;
    NuVecRotateZ(&vertices[1].position, &vertices[0].position, step);
    for (i = 0; i < segments; i++) {
        RndrVtxLine3d(&matrix, vertices);
        vertices[0].position = vertices[1].position;
        NuVecRotateZ(&vertices[1].position, &vertices[1].position, step);
    }
}

/* Camera-facing square outline of half-size radius around centre. */
void __cdecl RndrOSquare(NUVEC *centre, f32 radius, i32 colour, NUMTL *) {
    f32 corners[8] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
    NUMTX matrix = global_camera.mtx;
    NURND_VERTEX3D_PC vertices[2];
    i32 i;
    matrix.m30 = centre->x;
    matrix.m31 = centre->y;
    matrix.m32 = centre->z;
    vertices[0].position.z = 0.0f;
    vertices[0].colour = colour;
    vertices[1].position.z = 0.0f;
    vertices[1].colour = colour;
    for (i = 0; i < 4; i++) {
        vertices[0].position.x = corners[(i & 3) * 2] * radius;
        vertices[0].position.y = corners[(i * 2 + 1) & 7] * radius;
        vertices[1].position.x = corners[(i * 2 + 2) & 7] * radius;
        vertices[1].position.y = corners[(i * 2 + 3) & 7] * radius;
        RndrVtxLine3d(&matrix, vertices);
    }
}
