#include "saga.h"

void NuPlnEqn(NUPLANE *out, NUVEC *pnt0, NUVEC *pnt1, NUVEC *pnt2) {
    NUVEC *pln;
    NUVEC v1_minus_0;
    NUVEC v2_minus_0;
    NUVEC normal;

    pln = &out->normal;

    NuVecSub(&v1_minus_0, pnt1, pnt0);
    NuVecSub(&v2_minus_0, pnt2, pnt0);

    NuVecCross(&normal, &v1_minus_0, &v2_minus_0);

    NuVecNorm(pln, &normal);

    out->d = -(out->a * pnt0->x + out->b * pnt0->y + out->c * pnt0->z);
}

i32 NuPlnLine(NUPLANE *plane, NUVEC *s, NUVEC *e, NUVEC *out) {
    f32 dot_sp;
    f32 dot_ep;
    NUVEC dir;
    f32 inv_magnitude;
    i32 is_acute;

    dot_sp = NuVecDot(s, &plane->normal) + plane->d;
    dot_ep = NuVecDot(e, &plane->normal) + plane->d;

    if (NuFsign(dot_sp) != NuFsign(dot_ep)) {
        NuVecSub(&dir, e, s);

        inv_magnitude = -dot_sp / (dot_ep - dot_sp);
        NuVecScale(out, &dir, inv_magnitude);

        NuVecAdd(out, out, s);

        return 1;
    }

    return 0;
}

f32 NuPlnLine3(NUPLANE *plane, NUVEC *pnt, NUVEC *v, NUVEC *out) {
    NUVEC *pln;
    f32 dist;
    f32 dot;

    NuVecNorm(v, v);

    pln = &plane->normal;
    dot = v->x * pln->x + v->y * pln->y + v->z * pln->z;

    dist = ((plane->a * -plane->d - pnt->x) * pln->x + (plane->b * -plane->d - pnt->y) * pln->y +
            (plane->c * -plane->d - pnt->z) * pln->z) /
           dot;

    out->x = pnt->x + v->x * dist;
    out->y = pnt->y + v->y * dist;
    out->z = pnt->z + v->z * dist;

    return dist;
}

i32 NuPtInPolyXY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->x - t0->x) * (pnt->y - t1->y) - (pnt->x - t1->x) * (t1->y - t0->y) <= 0.0f &&
        (t2->x - t1->x) * (pnt->y - t2->y) - (pnt->x - t2->x) * (t2->y - t1->y) <= 0.0f &&
        (t0->x - t2->x) * (pnt->y - t0->y) - (pnt->x - t0->x) * (t0->y - t2->y) <= 0.0f) {
        if ((t0->x != t1->x || t0->y != t1->y) && (t0->x != t2->x || t0->y != t2->y) &&
            (t1->x != t2->x || t1->y != t2->y)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyYX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->y - t0->y) * (pnt->x - t1->x) - (pnt->y - t1->y) * (t1->x - t0->x) <= 0.0f &&
        (t2->y - t1->y) * (pnt->x - t2->x) - (pnt->y - t2->y) * (t2->x - t1->x) <= 0.0f &&
        (t0->y - t2->y) * (pnt->x - t0->x) - (pnt->y - t0->y) * (t0->x - t2->x) <= 0.0f) {
        if ((t0->y != t1->y || t0->x != t1->x) && (t0->y != t2->y || t0->x != t2->x) &&
            (t1->y != t2->y || t1->x != t2->x)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyXZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->x - t0->x) * (pnt->z - t1->z) - (pnt->x - t1->x) * (t1->z - t0->z) <= 0.0f &&
        (t2->x - t1->x) * (pnt->z - t2->z) - (pnt->x - t2->x) * (t2->z - t1->z) <= 0.0f &&
        (t0->x - t2->x) * (pnt->z - t0->z) - (pnt->x - t0->x) * (t0->z - t2->z) <= 0.0f) {
        if ((t0->x != t1->x || t0->z != t1->z) && (t0->x != t2->x || t0->z != t2->z) &&
            (t1->x != t2->x || t1->z != t2->z)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyZX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->z - t0->z) * (pnt->x - t1->x) - (pnt->z - t1->z) * (t1->x - t0->x) <= 0.0f &&
        (t2->z - t1->z) * (pnt->x - t2->x) - (pnt->z - t2->z) * (t2->x - t1->x) <= 0.0f &&
        (t0->z - t2->z) * (pnt->x - t0->x) - (pnt->z - t0->z) * (t0->x - t2->x) <= 0.0f) {
        if ((t0->z != t1->z || t0->x != t1->x) && (t0->z != t2->z || t0->x != t2->x) &&
            (t1->z != t2->z || t1->x != t2->x)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyYZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->y - t0->y) * (pnt->z - t1->z) - (pnt->y - t1->y) * (t1->z - t0->z) <= 0.0f &&
        (t2->y - t1->y) * (pnt->z - t2->z) - (pnt->y - t2->y) * (t2->z - t1->z) <= 0.0f &&
        (t0->y - t2->y) * (pnt->z - t0->z) - (pnt->y - t0->y) * (t0->z - t2->z) <= 0.0f) {
        if ((t0->y != t1->y || t0->z != t1->z) && (t0->y != t2->y || t0->z != t2->z) &&
            (t1->y != t2->y || t1->z != t2->z)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyZY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->z - t0->z) * (pnt->y - t1->y) - (pnt->z - t1->z) * (t1->y - t0->y) <= 0.0f &&
        (t2->z - t1->z) * (pnt->y - t2->y) - (pnt->z - t2->z) * (t2->y - t1->y) <= 0.0f &&
        (t0->z - t2->z) * (pnt->y - t0->y) - (pnt->z - t0->z) * (t0->y - t2->y) <= 0.0f) {
        if ((t0->z != t1->z || t0->y != t1->y) && (t0->z != t2->z || t0->y != t2->y) &&
            (t1->z != t2->z || t1->y != t2->y)) {
            return 1;
        }
    }

    return 0;
}

f32 NuPlnDist2(NUPLANE *plane, NUVEC *a, NUVEC *b) {
    f32 da = NuVecDot(a, &plane->normal) + plane->d;
    f32 db = NuVecDot(b, &plane->normal) + plane->d;
    if (da < 0.0f && db < 0.0f)
        return da > db ? da : db;
    if (da > 0.0f && db > 0.0f)
        return da < db ? da : db;
    return 0.0f;
}

i32 NuPlnPlnIntersect(NUPLANE *first, NUPLANE *second, NUVEC *point, NUVEC *direction) {
    f32 cross_y = first->c * second->a - first->a * second->c;
    f32 cross_z = first->a * second->b - first->b * second->a;
    NUVEC normal;
    union {
        f32 value;
        u32 bits;
    } magnitude;
    normal.x = first->b * second->c - first->c * second->b;
    normal.y = cross_y;
    normal.z = cross_z;
    NuVecNorm(&normal, &normal);
    *direction = normal;
    magnitude.value = normal.x;
    magnitude.bits &= 0x7fffffff;
    normal.x = magnitude.value;
    magnitude.value = normal.y;
    magnitude.bits &= 0x7fffffff;
    normal.y = magnitude.value;
    magnitude.value = normal.z;
    magnitude.bits &= 0x7fffffff;
    normal.z = magnitude.value;
    if (normal.z >= normal.x && normal.z >= normal.y) {
        magnitude.value = second->a;
        magnitude.bits &= 0x7fffffff;
        if (magnitude.value < 0.00009999999747378752) {
            point->y = (first->d * second->a / first->a - second->d) / (-second->a * first->b / first->a + second->b);
            point->x = (-first->d - first->b * point->y) / first->a;
            point->z = 0.0f;
        } else {
            point->y = (second->d * first->a / second->a - first->d) / (-first->a * second->b / second->a + first->b);
            point->x = (-second->d - second->b * point->y) / second->a;
            point->z = 0.0f;
        }
    } else if (normal.y >= normal.x && normal.y >= normal.z) {
        magnitude.value = second->a;
        magnitude.bits &= 0x7fffffff;
        if (magnitude.value < 0.00009999999747378752) {
            point->z = (first->d * second->a / first->a - second->d) / (-second->a * first->c / first->a + second->c);
            point->x = (-first->d - first->c * point->z) / first->a;
            point->y = 0.0f;
        } else {
            point->z = (second->d * first->a / second->a - first->d) / (-first->a * second->c / second->a + first->c);
            point->x = (-second->d - second->c * point->z) / second->a;
            point->y = 0.0f;
        }
    } else {
        magnitude.value = second->b;
        magnitude.bits &= 0x7fffffff;
        if (magnitude.value < 0.00009999999747378752) {
            point->z = (first->d * second->b / first->b - second->d) / (-second->b * first->c / first->b + second->c);
            point->y = (-first->d - first->c * point->z) / first->b;
            point->x = 0.0f;
        } else {
            point->z = (second->d * first->b / second->b - first->d) / (-first->b * second->c / second->b + first->c);
            point->y = (-second->d - second->c * point->z) / second->b;
            point->x = 0.0f;
        }
    }
    return 0;
}

f32 __cdecl NuLineToPointDistSqrEx(NUVEC *start, NUVEC *end, NUVEC *point, NUVEC *out) {
    NUVEC direction;
    NUVEC offset;
    NUVEC nearest;
    NUVEC difference;
    f32 length;
    f32 inverse_length;
    f32 along;
    f32 distance;
    direction.x = end->x - start->x;
    direction.y = end->y - start->y;
    direction.z = end->z - start->z;
    length = NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    inverse_length = 1.0f / length;
    direction.x *= inverse_length;
    direction.y *= inverse_length;
    direction.z *= inverse_length;
    offset.x = point->x - start->x;
    offset.y = point->y - start->y;
    offset.z = point->z - start->z;
    along = direction.x * offset.x + direction.y * offset.y + direction.z * offset.z;
    if (along <= 0.0f) {
        nearest = *start;
    } else if (along >= length) {
        nearest = *end;
    } else {
        nearest.x = direction.x * along + start->x;
        nearest.y = direction.y * along + start->y;
        nearest.z = direction.z * along + start->z;
    }
    distance = NuVecDistSqr(&nearest, point, &difference);
    if (out != NULL)
        *out = nearest;
    return distance;
}

__declspec(noinline) f32 __cdecl NuInfiniteLineToPointDistSqrEx(NUVEC *start, NUVEC *end, NUVEC *point,
                                                                NUVEC *closest) {
    NUVEC direction;
    NUVEC relative;
    NUVEC projected;
    NUVEC delta;
    f32 inverse_length;
    f32 projection;
    f32 distance;
    direction.x = end->x - start->x;
    direction.y = end->y - start->y;
    direction.z = end->z - start->z;
    inverse_length = 1.0f / NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    direction.x *= inverse_length;
    direction.y *= inverse_length;
    direction.z *= inverse_length;
    relative.x = point->x - start->x;
    relative.y = point->y - start->y;
    relative.z = point->z - start->z;
    projection = direction.x * relative.x + direction.y * relative.y + direction.z * relative.z;
    projected.x = direction.x * projection;
    projected.y = direction.y * projection;
    projected.z = direction.z * projection;
    projected.x += start->x;
    projected.y += start->y;
    projected.z += start->z;
    distance = NuVecDistSqr(&projected, point, &delta);
    if (closest != NULL)
        *closest = projected;
    return distance;
}

f32 __cdecl NuInfiniteLineToPointDistSqr(NUVEC *start, NUVEC *end, NUVEC *point) {
    return NuInfiniteLineToPointDistSqrEx(start, end, point, NULL);
}
