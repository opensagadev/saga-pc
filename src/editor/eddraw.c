#include "saga.h"

extern NUMTL *EdDrawMaterialsPc[2];
extern i32 EdDrawPrimStatePc;
extern i32 EdDrawMaterialIndexPc;
extern NUMTX *EdDrawMatrixPc;

#define EDDRAW_DARKEN_CHANNEL(colour, shift) ((u8)((((colour) >> (shift)) & 0xff) * 0xdc / 256))
#define EDDRAW_DARKEN(colour)                                                                                          \
    ((((EDDRAW_DARKEN_CHANNEL(colour, 16) << 8) | EDDRAW_DARKEN_CHANNEL(colour, 8)) << 8) |                            \
     ((((colour) & 0xff) * 0xdc / 256) & 0xff) | ((colour) & 0xff000000))

static __declspec(noinline) void EdDrawSetVertexColour(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
    } else {
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
    }
    ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

void __cdecl EdDrawSetMaterial(i32 index) {
    if (EdDrawMaterialsPc[0] == NULL) {
        EdDrawMaterialsPc[0] = NuMtlCreate3D(1);
        EdDrawMaterialsPc[0]->attribs.cull_mode = 2;
        EdDrawMaterialsPc[0]->attribs.alpha_mode = 1;
        NuMtlUpdate(EdDrawMaterialsPc[0]);
        EdDrawMaterialsPc[1] = NuMtlCreate3D(1);
        EdDrawMaterialsPc[1]->attribs.cull_mode = 2;
        EdDrawMaterialsPc[1]->attribs.z_mode = 3;
        EdDrawMaterialsPc[1]->attribs.alpha_mode = 1;
        NuMtlUpdate(EdDrawMaterialsPc[1]);
    }
    EdDrawMaterialIndexPc = index;
    EdDrawPrimStatePc = 1;
}

void __cdecl EdDrawFlush(void) {
    if (EdDrawPrimStatePc != 0) {
        if (EdDrawPrimStatePc == 2 || EdDrawPrimStatePc == 3)
            NuPrim3DEnd();
        EdDrawPrimStatePc = 0;
    }
}

#define EDDRAW_ADD_XYZ(px, py, pz)                                                                                     \
    do {                                                                                                               \
        f32 x_ = (px);                                                                                                 \
        f32 y_ = (py);                                                                                                 \
        f32 z_ = (pz);                                                                                                 \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->x = x_;                                               \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->y = y_;                                               \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = z_;                                               \
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX_PC);                                                     \
        NuPrimVertexCount++;                                                                                           \
    } while (0)

void __cdecl EdDrawPolyTri(NUVEC *a, NUVEC *b, NUVEC *c, i32 colour) {
    if (EdDrawPrimStatePc == 1) {
        NuPrim3DBegin(0xc, 5, EdDrawMaterialsPc[EdDrawMaterialIndexPc], EdDrawMatrixPc);
        EdDrawPrimStatePc = 3;
    }
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(a->x, a->y, a->z);
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(b->x, b->y, b->z);
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(c->x, c->y, c->z);
}

#define EDDRAW_SET_VEC4(v, px, py, pz)                                                                                 \
    do {                                                                                                               \
        (v)->x = (px);                                                                                                 \
        (v)->y = (py);                                                                                                 \
        (v)->z = (pz);                                                                                                 \
        (v)->w = 1.0f;                                                                                                 \
    } while (0)

void __cdecl EdDrawPolyCylinder(NUMTX *matrix, f32 half_height, f32 radius, f32 top_radius, i32 segments, i32 colour,
                                i32 cap_bottom, i32 cap_top) {
    __declspec(align(16)) NUVEC4 a;
    __declspec(align(16)) NUVEC4 b;
    __declspec(align(16)) NUVEC4 c;
    f32 scale = top_radius / radius;
    i32 dark = EDDRAW_DARKEN(colour);
    i32 tri_colour = 0;
    f32 s0, c0, s1, c1, s2, c2, top_s, top_c;
    i32 i;
    i32 count;

    EdDrawFlush();
    EdDrawSetMaterial(0);
    EdDrawMatrixPc = matrix;
    s0 = NU_SIN_LUT(0) * radius;
    c0 = NU_COS_LUT(0) * radius;
    for (i = 0; i < segments; i++) {
        i32 angle = ((i + 1) << 16) / segments;
        tri_colour = (i & 1) ? dark : colour;
        s1 = NU_SIN_LUT(angle) * radius;
        c1 = NU_COS_LUT(angle) * radius;
        top_s = s0 * scale;
        top_c = c0 * scale;
        EDDRAW_SET_VEC4(&a, top_s, top_c, half_height);
        EDDRAW_SET_VEC4(&b, s1 * scale, c1 * scale, half_height);
        EDDRAW_SET_VEC4(&c, s1, c1, -half_height);
        EdDrawPolyTri((NUVEC *)&a, (NUVEC *)&b, (NUVEC *)&c, tri_colour);
        EDDRAW_SET_VEC4(&a, top_s, top_c, half_height);
        EDDRAW_SET_VEC4(&b, s1, c1, -half_height);
        EDDRAW_SET_VEC4(&c, s0, c0, -half_height);
        EdDrawPolyTri((NUVEC *)&a, (NUVEC *)&b, (NUVEC *)&c, tri_colour);
        s0 = s1;
        c0 = c1;
    }
    if (cap_bottom != 0 || cap_top != 0) {
        i32 angle = 0x10000 / segments;
        s0 = NU_SIN_LUT(0) * radius;
        c0 = NU_COS_LUT(0) * radius;
        s1 = NU_SIN_LUT(angle) * radius;
        c1 = NU_COS_LUT(angle) * radius;
        for (count = segments - 2, i = 0x20000; count > 0; count--, i += 0x10000) {
            angle = i / segments;
            s2 = NU_SIN_LUT(angle) * radius;
            c2 = NU_COS_LUT(angle) * radius;
            if (cap_top != 0) {
                EDDRAW_SET_VEC4(&a, s0 * scale, c0 * scale, half_height);
                EDDRAW_SET_VEC4(&b, s2 * scale, c2 * scale, half_height);
                EDDRAW_SET_VEC4(&c, s1 * scale, c1 * scale, half_height);
                EdDrawPolyTri((NUVEC *)&a, (NUVEC *)&b, (NUVEC *)&c, tri_colour);
            }
            if (cap_bottom != 0) {
                EDDRAW_SET_VEC4(&a, s0, c0, -half_height);
                EDDRAW_SET_VEC4(&b, s1, c1, -half_height);
                EDDRAW_SET_VEC4(&c, s2, c2, -half_height);
                EdDrawPolyTri((NUVEC *)&a, (NUVEC *)&b, (NUVEC *)&c, tri_colour);
            }
            s1 = s2;
            c1 = c2;
        }
    }
    EdDrawFlush();
    EdDrawSetMaterial(0);
    EdDrawMatrixPc = NULL;
}

void __cdecl EdDrawPolyArrow(NUVEC *from, NUVEC *to, i32 segments, i32 colour, f32 min_radius, f32 max_radius,
                             f32 radius_scale, f32 radius_bias) {
    __declspec(align(16)) NUMTX matrix;
    NUVEC4 delta;
    NUVEC4 dir;
    NUVEC4 rotated;
    NUVEC4 offset;
    NUVEC4 position;
    NUANGVEC angles;
    f32 length;
    f32 inverse;
    f32 half_length;
    f32 radius;

    delta.x = to->x - from->x;
    delta.y = to->y - from->y;
    delta.z = to->z - from->z;
    length = NuFsqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    if (length > 0.0f) {
        inverse = 1.0f / length;
        dir.x = delta.x * inverse;
        dir.y = delta.y * inverse;
        dir.z = delta.z * inverse;
        rotated.x = dir.x;
        rotated.y = dir.y;
        rotated.z = dir.z;
        rotated.w = dir.w;
        half_length = length * (f64)0.4f;
        radius = half_length * radius_scale + radius_bias;
        if (rotated.x == 0.0f && rotated.z == 0.0f) {
            angles.x = -0x4000;
            angles.y = 0;
        } else {
            angles.y = NuAtan2D(rotated.x, rotated.z);
            NuVecRotateY((NUVEC *)&rotated, (NUVEC *)&rotated, -angles.y);
            angles.x = -NuAtan2D(rotated.y, rotated.z);
        }
        angles.z = 0;
        if (radius < min_radius)
            radius = min_radius;
        if (radius > max_radius)
            radius = max_radius;
        offset.x = delta.x * (f64)0.4f;
        offset.y = delta.y * (f64)0.4f;
        offset.z = delta.z * (f64)0.4f;
        dir.x = offset.x + from->x;
        dir.y = offset.y + from->y;
        dir.z = from->z + offset.z;
        position.x = dir.x;
        position.y = dir.y;
        position.z = dir.z;
        position.w = dir.w;
        NuMtxSetRotateXYZVU0(&matrix, &angles);
        NuMtxTranslate(&matrix, (NUVEC *)&position);
        EdDrawPolyCylinder(&matrix, half_length, radius, radius, segments, colour, 1, 0);
        NuMtxTranslateNeg(&matrix, (NUVEC *)&position);
        offset.x = delta.x * (f64)0.1f;
        offset.y = delta.y * (f64)0.1f;
        offset.z = delta.z * (f64)0.1f;
        dir.x = to->x - offset.x;
        dir.y = to->y - offset.y;
        dir.z = to->z - offset.z;
        position.x = dir.x;
        position.y = dir.y;
        position.z = dir.z;
        position.w = dir.w;
        NuMtxTranslate(&matrix, (NUVEC *)&position);
        radius *= (f64)1.6f;
        half_length *= 0.25f;
        EdDrawPolyCylinder(&matrix, half_length, radius, 0.0f, segments, colour, 1, 0);
    }
}
