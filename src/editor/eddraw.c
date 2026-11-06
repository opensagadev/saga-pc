#include "saga.h"

#define EDDRAW_SET_VEC4(v, px, py, pz)                                                                                 \
    do {                                                                                                               \
        (v)->x = (px);                                                                                                 \
        (v)->y = (py);                                                                                                 \
        (v)->z = (pz);                                                                                                 \
        (v)->w = 1.0f;                                                                                                 \
    } while (0)

extern NUMTL *EdDrawMtl[2];
extern i32 NewPrim;
extern i32 NewMtl;
extern NUMTX *NewMtx;

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

void __cdecl EdDrawBegin(i32 index) {
    if (EdDrawMtl[0] == NULL) {
        EdDrawMtl[0] = NuMtlCreate3D(1);
        EdDrawMtl[0]->attribs.cull_mode = 2;
        EdDrawMtl[0]->attribs.alpha_mode = 1;
        NuMtlUpdate(EdDrawMtl[0]);
        EdDrawMtl[1] = NuMtlCreate3D(1);
        EdDrawMtl[1]->attribs.cull_mode = 2;
        EdDrawMtl[1]->attribs.z_mode = 3;
        EdDrawMtl[1]->attribs.alpha_mode = 1;
        NuMtlUpdate(EdDrawMtl[1]);
    }
    NewMtl = index;
    NewPrim = 1;
}

void __cdecl EdDrawEnd(void) {
    if (NewPrim != 0) {
        if (NewPrim == 2 || NewPrim == 3)
            NuPrim3DEnd();
        NewPrim = 0;
    }
}

void __cdecl EdDrawMtx(NUMTX *matrix) {
    EdDrawEnd();
    EdDrawBegin(0);
    NewMtx = matrix;
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

void __cdecl EdDrawLineSegment(NUVEC *a, NUVEC *b, i32 colour) {
    if (NewPrim == 1) {
        NuPrim3DBegin(0xe, 5, EdDrawMtl[NewMtl], NewMtx);
        NewPrim = 2;
    }
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(a->x, a->y, a->z);
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(b->x, b->y, b->z);
}

void __cdecl EdDrawLineCross(NUVEC *position, f32 size, i32 colour) {
    __declspec(align(16)) NUVEC4 start;
    __declspec(align(16)) NUVEC4 end;
    start.y = end.y = position->y;
    start.z = end.z = position->z;
    start.x = position->x - size;
    end.x = position->x + size;
    EdDrawLineSegment((NUVEC *)&start, (NUVEC *)&end, colour);
    start.x = end.x = position->x;
    start.y = position->y - size;
    end.y = position->y + size;
    EdDrawLineSegment((NUVEC *)&start, (NUVEC *)&end, colour);
    start.y = end.y = position->y;
    start.z = position->z - size;
    end.z = position->z + size;
    EdDrawLineSegment((NUVEC *)&start, (NUVEC *)&end, colour);
}

#define EDDRAW_LINE_CIRCLE(rotate, base_y, base_z)                                                                     \
    do {                                                                                                               \
        __declspec(align(16)) NUVEC4 base;                                                                             \
        __declspec(align(16)) NUVEC4 point;                                                                            \
        __declspec(align(16)) NUVEC4 previous;                                                                         \
        i32 i;                                                                                                         \
        base.x = 0.0f;                                                                                                 \
        base.y = (base_y);                                                                                             \
        base.z = (base_z);                                                                                             \
        point.x = base.x + centre->x;                                                                                  \
        point.y = base.y + centre->y;                                                                                  \
        point.z = base.z + centre->z;                                                                                  \
        point.w = 0.0f;                                                                                                \
        for (i = 1; i <= segments; i++) {                                                                              \
            previous.x = point.x;                                                                                      \
            previous.y = point.y;                                                                                      \
            previous.z = point.z;                                                                                      \
            previous.w = point.w;                                                                                      \
            rotate((NUVEC *)&point, (NUVEC *)&base, i * (0x10000 / segments));                                         \
            point.x = point.x + centre->x;                                                                             \
            point.y = point.y + centre->y;                                                                             \
            point.z = point.z + centre->z;                                                                             \
            point.w = 0.0f;                                                                                            \
            EdDrawLineSegment((NUVEC *)&previous, (NUVEC *)&point, colour);                                            \
        }                                                                                                              \
    } while (0)

void __cdecl EdDrawLineCircleX(NUVEC *centre, f32 radius, i32 colour, i32 segments) {
    EDDRAW_LINE_CIRCLE(NuVecRotateX, 0.0f, radius);
}

void __cdecl EdDrawLineCircleY(NUVEC *centre, f32 radius, i32 colour, i32 segments) {
    EDDRAW_LINE_CIRCLE(NuVecRotateY, 0.0f, radius);
}

void __cdecl EdDrawLineCircleZ(NUVEC *centre, f32 radius, i32 colour, i32 segments) {
    EDDRAW_LINE_CIRCLE(NuVecRotateZ, radius, 0.0f);
}

#define EDDRAW_SET_POINT(v, px, py, pz)                                                                                \
    do {                                                                                                               \
        (v).x = (px);                                                                                                  \
        (v).y = (py);                                                                                                  \
        (v).z = (pz);                                                                                                  \
        (v).w = 0.0f;                                                                                                  \
    } while (0)

void __cdecl EdDrawLineArrow(NUMTX *transform, f32 size, i32 colour) {
    __declspec(align(16)) NUVEC4 points[4];
    i32 i;
    EDDRAW_SET_POINT(points[0], 0.0f, 0.0f, -size);
    EDDRAW_SET_POINT(points[1], 0.0f, 0.0f, size);
    EDDRAW_SET_POINT(points[2], size * 0.5f, 0.0f, 0.0f);
    EDDRAW_SET_POINT(points[3], -size * 0.5f, 0.0f, 0.0f);
    for (i = 0; i < 4; i++)
        NuVecMtxTransform((NUVEC *)&points[i], (NUVEC *)&points[i], transform);
    EdDrawLineSegment((NUVEC *)&points[0], (NUVEC *)&points[1], colour);
    EdDrawLineSegment((NUVEC *)&points[1], (NUVEC *)&points[2], colour);
    EdDrawLineSegment((NUVEC *)&points[1], (NUVEC *)&points[3], colour);
    EdDrawLineSegment((NUVEC *)&points[2], (NUVEC *)&points[3], colour);
}

void __cdecl EdDrawLineCube(NUMTX *transform, f32 size, i32 colour) {
    __declspec(align(16)) NUVEC4 points[9];
    i32 i;
    EDDRAW_SET_POINT(points[0], size, size, -size);
    EDDRAW_SET_POINT(points[1], size, size, size);
    EDDRAW_SET_POINT(points[2], -size, size, size);
    EDDRAW_SET_POINT(points[3], -size, size, -size);
    EDDRAW_SET_POINT(points[4], size, -size, -size);
    EDDRAW_SET_POINT(points[5], size, -size, size);
    EDDRAW_SET_POINT(points[6], -size, -size, size);
    EDDRAW_SET_POINT(points[7], -size, -size, -size);
    EDDRAW_SET_POINT(points[8], 0.0f, 0.0f, size);
    for (i = 0; i < 9; i++)
        NuVecMtxTransform((NUVEC *)&points[i], (NUVEC *)&points[i], transform);
    EdDrawLineSegment((NUVEC *)&points[0], (NUVEC *)&points[1], colour);
    EdDrawLineSegment((NUVEC *)&points[1], (NUVEC *)&points[2], colour);
    EdDrawLineSegment((NUVEC *)&points[2], (NUVEC *)&points[3], colour);
    EdDrawLineSegment((NUVEC *)&points[3], (NUVEC *)&points[0], colour);
    EdDrawLineSegment((NUVEC *)&points[0], (NUVEC *)&points[2], colour);
    EdDrawLineSegment((NUVEC *)&points[1], (NUVEC *)&points[3], colour);
    EdDrawLineSegment((NUVEC *)&points[4], (NUVEC *)&points[5], colour);
    EdDrawLineSegment((NUVEC *)&points[5], (NUVEC *)&points[6], colour);
    EdDrawLineSegment((NUVEC *)&points[6], (NUVEC *)&points[7], colour);
    EdDrawLineSegment((NUVEC *)&points[7], (NUVEC *)&points[4], colour);
    EdDrawLineSegment((NUVEC *)&points[0], (NUVEC *)&points[4], colour);
    EdDrawLineSegment((NUVEC *)&points[1], (NUVEC *)&points[5], colour);
    EdDrawLineSegment((NUVEC *)&points[2], (NUVEC *)&points[6], colour);
    EdDrawLineSegment((NUVEC *)&points[3], (NUVEC *)&points[7], colour);
    EdDrawLineSegment((NUVEC *)&points[5], (NUVEC *)&points[8], colour);
    EdDrawLineSegment((NUVEC *)&points[6], (NUVEC *)&points[8], colour);
}

void __cdecl EdDrawLineSphere(NUVEC *center, f32 radius, f32 scale, i32 colour) {
    __declspec(align(16)) NUVEC4 first;
    __declspec(align(16)) NUVEC4 second;
    f64 radius_squared = radius * radius;
    f32 first_radius;
    f32 second_radius;
    f32 first_height;
    f32 second_height;
    i32 latitude;
    i32 angle;
    for (latitude = 0; latitude < 8; latitude++) {
        if (latitude < 4)
            first_radius = radius * NU_SIN_LUT(latitude * 0x1000);
        else
            first_radius = radius * NU_SIN_LUT((8 - latitude) * 0x1000);
        if (latitude < 3)
            second_radius = radius * NU_SIN_LUT((latitude + 1) * 0x1000);
        else
            second_radius = radius * NU_SIN_LUT((7 - latitude) * 0x1000);
        first_height = NuFsqrt(radius_squared - first_radius * first_radius) * scale;
        second_height = NuFsqrt(radius_squared - second_radius * second_radius) * scale;
        if (latitude > 4)
            first_height = -first_height;
        if (latitude > 3)
            second_height = -second_height;
        first_height += center->y;
        second_height += center->y;
        for (angle = 0; angle < 0x10000; angle += 0x2000) {
            EDDRAW_SET_VEC4(&first, center->x + first_radius * NU_COS_LUT(angle), first_height,
                            center->z + first_radius * NU_SIN_LUT(angle));
            EDDRAW_SET_VEC4(&second, center->x + second_radius * NU_COS_LUT(angle), second_height,
                            center->z + second_radius * NU_SIN_LUT(angle));
            EdDrawLineSegment((NUVEC *)&first, (NUVEC *)&second, colour);
            if (latitude != 0) {
                EDDRAW_SET_VEC4(&second, center->x + first_radius * NU_COS_LUT(angle + 0x2000), first_height,
                                center->z + first_radius * NU_SIN_LUT(angle + 0x2000));
                EdDrawLineSegment((NUVEC *)&first, (NUVEC *)&second, colour);
            }
        }
    }
}

void __cdecl EdDrawPolyTri(NUVEC *a, NUVEC *b, NUVEC *c, i32 colour) {
    if (NewPrim == 1) {
        NuPrim3DBegin(0xc, 5, EdDrawMtl[NewMtl], NewMtx);
        NewPrim = 3;
    }
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(a->x, a->y, a->z);
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(b->x, b->y, b->z);
    EdDrawSetVertexColour(colour);
    EDDRAW_ADD_XYZ(c->x, c->y, c->z);
}

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

    EdDrawMtx(matrix);
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
    EdDrawMtx(NULL);
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

void __cdecl EdDrawPolySector(NUVEC *center, f32 radius, i32 axis, i32 first_angle, i32 last_angle, i32 colour,
                              i32 segments) {
    __declspec(align(16)) NUVEC4 first;
    __declspec(align(16)) NUVEC4 second;
    i32 step;
    i32 angle;
    i32 remaining;
    i32 previous_angle;
    f32 negative_radius;
    if (segments == 0)
        segments = 16;
    step = 0x10000 / segments;
    if (first_angle < last_angle) {
        remaining = (last_angle - first_angle) & 0xffff;
        if (remaining >= 0x8000)
            remaining -= 0x10000;
        angle = first_angle;
    } else {
        remaining = (first_angle - last_angle) & 0xffff;
        if (remaining >= 0x8000)
            remaining -= 0x10000;
        angle = last_angle;
    }
    if (remaining <= 0)
        return;
    negative_radius = -radius;
    do {
        previous_angle = angle;
        if (remaining > step) {
            angle += step;
            remaining -= step;
        } else {
            angle += remaining;
            remaining = 0;
        }
        switch (axis) {
            case 1:
                first.w = 1.0f;
                first.x = negative_radius * 0.0f;
                first.y = negative_radius * 0.0f;
                first.z = negative_radius;
                NuVecRotateY((NUVEC *)&first, (NUVEC *)&first, -previous_angle);
                second.w = 1.0f;
                second.y = negative_radius * 0.0f;
                second.x = negative_radius * 0.0f;
                second.z = negative_radius;
                NuVecRotateY((NUVEC *)&second, (NUVEC *)&second, -angle);
                break;
            case 2:
                first.w = 1.0f;
                first.x = 0.0f * radius;
                first.y = radius;
                first.z = 0.0f * radius;
                NuVecRotateZ((NUVEC *)&first, (NUVEC *)&first, -previous_angle);
                second.w = 1.0f;
                second.x = 0.0f * radius;
                second.y = radius;
                second.z = 0.0f * radius;
                NuVecRotateZ((NUVEC *)&second, (NUVEC *)&second, -angle);
                break;
            case 0:
                first.w = 1.0f;
                first.x = 0.0f * radius;
                first.y = 0.0f * radius;
                first.z = radius;
                NuVecRotateX((NUVEC *)&first, (NUVEC *)&first, -previous_angle);
                second.w = 1.0f;
                second.y = 0.0f * radius;
                second.x = 0.0f * radius;
                second.z = radius;
                NuVecRotateX((NUVEC *)&second, (NUVEC *)&second, -angle);
                break;
        }
        first.x += center->x;
        first.y += center->y;
        first.z += center->z;
        second.x += center->x;
        second.y += center->y;
        second.z += center->z;
        EdDrawPolyTri(center, (NUVEC *)&first, (NUVEC *)&second, colour);
    } while (remaining > 0);
}

void __cdecl EdDrawPolyCylinderEx(NUVEC *start, NUVEC *end, i32 sides, i32 colour, i32 cap_colour, f32 radius,
                                  f32 limit, f32 offset) {
    __declspec(align(16)) NUMTX transform;
    __declspec(align(16)) NUVEC4 direction;
    __declspec(align(16)) NUVEC4 center;
    NUANGVEC angles;
    f32 length;
    f32 inverse_length;
    f32 half_length;
    f32 minimum_radius;
    f32 first_radius;
    f32 second_radius;
    direction.x = end->x - start->x;
    direction.y = end->y - start->y;
    direction.z = end->z - start->z;
    direction.w = 0.0f;
    length = NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length > 0.0f) {
        inverse_length = 1.0f / length;
        direction.x *= inverse_length;
        direction.y *= inverse_length;
        direction.z *= inverse_length;
        if (direction.x == 0.0f && direction.z == 0.0f) {
            angles.x = 0x2000;
            angles.y = 0;
        } else {
            angles.y = NuAtan2D(direction.x, direction.z);
            NuVecRotateY((NUVEC *)&direction, (NUVEC *)&direction, -angles.y);
            angles.x = -NuAtan2D(direction.y, direction.z);
        }
        angles.z = 0;
        center.x = (end->x + start->x) * 0.5f;
        center.y = (end->y + start->y) * 0.5f;
        center.z = (end->z + start->z) * 0.5f;
        center.w = 0.0f;
        NuMtxSetRotateXYZVU0(&transform, &angles);
        NuMtxTranslate(&transform, (NUVEC *)&center);
        half_length = length * 0.5f;
        minimum_radius = radius * half_length;
        first_radius = limit > minimum_radius ? limit : minimum_radius;
        if (first_radius > offset)
            first_radius = offset;
        second_radius = limit > minimum_radius * 0.1f ? limit : minimum_radius * (f64)0.1f;
        if (second_radius > offset)
            second_radius = offset;
        EdDrawPolyCylinder(&transform, half_length, first_radius, second_radius, sides, colour, 1, 1);
    }
}

#define EDDRAW_AXIS_ARROW(row_x, row_y, row_z, colour)                                                                 \
    do {                                                                                                               \
        tip.x = (row_x) * size;                                                                                        \
        tip.y = (row_y) * size;                                                                                        \
        tip.z = (row_z) * size;                                                                                        \
        sum.x = origin->x + tip.x;                                                                                     \
        sum.y = origin->y + tip.y;                                                                                     \
        sum.z = tip.z + origin->z;                                                                                     \
        tip.x = sum.x;                                                                                                 \
        tip.y = sum.y;                                                                                                 \
        tip.z = sum.z;                                                                                                 \
        tip.w = sum.w;                                                                                                 \
        width = size * (f64)0.02f;                                                                                     \
        EdDrawPolyArrow(origin, (NUVEC *)&tip, 8, (colour), width, width, 0.02f, 0.0f);                                \
    } while (0)

void __cdecl EdDrawPolyAxis(NUMTX *transform, f32 size, i32 opacity) {
    __declspec(align(16)) NUVEC4 tip;
    __declspec(align(16)) NUVEC4 sum;
    NUVEC *origin = &transform->translation;
    f32 width;
    EDDRAW_AXIS_ARROW(transform->m00, transform->m01, transform->m02, (opacity & 0xff) | 0xff000000);
    EDDRAW_AXIS_ARROW(transform->m10, transform->m11, transform->m12, ((opacity & 0xff) << 8) | 0xff000000);
    EDDRAW_AXIS_ARROW(transform->m20, transform->m21, transform->m22, ((opacity & 0xff) << 16) | 0xff000000);
}
