#include "saga.h"
#include <math.h>
#include <string.h>

STATIC_ASSERT(offsetof(NULGTLASER_PC, start) == 0x0);
STATIC_ASSERT(offsetof(NULGTLASER_PC, end) == 0xc);
STATIC_ASSERT(offsetof(NULGTLASER_PC, segment_length) == 0x1c);
STATIC_ASSERT(offsetof(NULGTLASER_PC, width_wobble) == 0x20);
STATIC_ASSERT(offsetof(NULGTLASER_PC, end_width_ratio) == 0x24);
STATIC_ASSERT(offsetof(NULGTLASER_PC, length) == 0x28);
STATIC_ASSERT(offsetof(NULGTLASER_PC, type) == 0x2c);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, start) == 0x0);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, end) == 0xc);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, bend) == 0x18);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, segment_length) == 0x28);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, wobble) == 0x2c);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, bend_amount) == 0x30);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, type) == 0x34);
STATIC_ASSERT(offsetof(NULGTARCLASER_PC, flags) == 0x3c);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, material) == 0x0);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, u0) == 0x4);
STATIC_ASSERT(offsetof(NULGTARCMATERIAL_PC, u1) == 0xc);
extern NULGTLASER_PC NuLgtLaserData[64];
extern NULGTARCLASER_PC NuLgtArcLaserData[16];
extern NULGTARCMATERIAL_PC NuLgtArcMtl[4];
extern i32 NuLgtLaserCnt, NuLgtLaserOldCnt;
extern i32 NuLgtArcLaserCnt, NuLgtArcLaserOldCnt, NuLgtArcLaserFrame;
extern u32 NuLgtSeed;
extern i32 PS2_REZ_W, PS2_REZ_H;
extern i32 NuPrimCoordSystemStack[];
void __cdecl NuVec4ScaleForwardPc(NUVEC4 *, NUVEC4 *, f32);
i32 __cdecl NuRndrBeginSceneEx(i32, i32, i32);
void __cdecl NuRndrEndSceneEx(i32);

__declspec(noinline) void __cdecl NuVec4Scale(NUVEC4 *out, NUVEC4 *input, f32 scale) {
    out->x = input->x * scale;
    out->y = input->y * scale;
    out->z = input->z * scale;
    out->w = input->w * scale;
}

__declspec(noinline) void __cdecl NuVec4ScaleForwardPc(NUVEC4 *out, NUVEC4 *input, f32 scale) {
    NuVec4Scale(out, input, scale);
}

__declspec(noinline) i32 __cdecl NuRndrBeginSceneEx(i32 flags, i32 layer, i32 mode) {
    return NuRndrBeginScene(flags);
}

__declspec(noinline) void __cdecl NuRndrEndSceneEx(i32 flags) {
    NuRndrEndScene();
}

static __declspec(noinline) void NuLgtSetVertexColor(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening)
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff000000) + (colour & 0xff00);
    else
        converted =
            ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}
#define NU_LGT_RAND() ((i32)(NuLgtSeed = (NuLgtSeed * 0x24cd + 1) & 0xffff))
#define NU_LGT_ARC_VERTEX(position, colour, tex_u, tex_v)                                                              \
    do {                                                                                                               \
        NuLgtSetVertexColor(colour);                                                                                   \
        vertex = (NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;                                                \
        vertex->u = (tex_u);                                                                                           \
        vertex->v = (tex_v);                                                                                           \
        NuPrim2DAddXYZ((position)->x, (position)->y, (position)->z);                                                   \
    } while (0)

void __cdecl NuLgtLaserDraw(i32 paused) {
    i32 i;
    i32 k;
    f32 tex_v[4];
    f32 tex_u[4];
    u32 saved_seed;
    NUVEC4 *vertices;
    NUMTX clip_matrix;
    f32 aspect;
    u32 colour;
    i32 red;
    i32 green;
    i32 blue;
    i32 alpha;
    i32 reds[2];
    i32 greens[2];
    i32 blues[2];
    i32 alphas[2];
    f32 dx;
    f32 dy;
    f32 dz;
    f32 remaining;
    NUVEC perpendicular;
    f32 length;
    f32 step;
    f32 width_scale;
    f32 fraction;
    f64 next_fraction;
    f64 wobble_limit;
    f32 offset_x;
    f32 offset_y;
    f32 wobble_x;
    f32 wobble_y;
    f32 neg_y;
    NULGTVERTEX_PC *vertex;
    if (paused)
        NuLgtLaserCnt = NuLgtLaserOldCnt;
    if (NuLgtArcMtl[0].material == NULL || NuLgtLaserCnt == 0) {
        NuLgtLaserOldCnt = 0;
        return;
    }

    tex_v[0] = NuLgtArcMtl[0].v0;
    tex_v[1] = NuLgtArcMtl[0].v0;
    tex_v[2] = NuLgtArcMtl[0].v1;
    tex_v[3] = NuLgtArcMtl[0].v1;
    saved_seed = NuLgtSeed;
    // Four strip corners, two projected endpoints, and two clipped world endpoints.
    vertices = (NUVEC4 *)NuScratchAlloc32(0x80);
    NuCameraGetClipMtx(&clip_matrix, NULL);
    aspect = (f32)PS2_REZ_W / PS2_REZ_H;

    for (i = 0; i < NuLgtLaserCnt; ++i) {
        vertices[4].x = NuLgtLaserData[i].start.x;
        vertices[4].y = NuLgtLaserData[i].start.y;
        vertices[4].z = NuLgtLaserData[i].start.z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
        vertices[5].x = NuLgtLaserData[i].end.x;
        vertices[5].y = NuLgtLaserData[i].end.y;
        vertices[5].z = NuLgtLaserData[i].end.z;
        vertices[5].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
        if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f))
            continue;

        if (vertices[4].w < 0.5f) {
            if (NuLgtLaserData[i].arc == 0) {
                colour = NuLgtLaserData[i].colour;
                red = colour & 0xff;
                green = (colour >> 8) & 0xff;
                blue = (u8)(colour >> 16);
                alpha = (u8)(colour >> 24);
            } else {
                red = (i32)((i32)(NuLgtLaserData[i].colour & 0xff) * (vertices[5].w - 0.5) /
                            (vertices[5].w - vertices[4].w));
                green = (i32)((((i32)NuLgtLaserData[i].colour >> 8) & 0xff) * (vertices[5].w - 0.5) /
                              (vertices[5].w - vertices[4].w));
                blue = (i32)((((i32)NuLgtLaserData[i].colour >> 16) & 0xff) * (vertices[5].w - 0.5) /
                             (vertices[5].w - vertices[4].w));
                alpha = (i32)((((i32)NuLgtLaserData[i].colour >> 24) & 0xff) * (vertices[5].w - 0.5) /
                              (vertices[5].w - vertices[4].w));
            }
            vertices[6].x = (NuLgtLaserData[i].start.x - NuLgtLaserData[i].end.x) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w) +
                            NuLgtLaserData[i].end.x;
            vertices[6].y = (NuLgtLaserData[i].start.y - NuLgtLaserData[i].end.y) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w) +
                            NuLgtLaserData[i].end.y;
            vertices[6].z = (NuLgtLaserData[i].start.z - NuLgtLaserData[i].end.z) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w) +
                            NuLgtLaserData[i].end.z;
        } else {
            vertices[6].x = NuLgtLaserData[i].start.x;
            vertices[6].y = NuLgtLaserData[i].start.y;
            vertices[6].z = NuLgtLaserData[i].start.z;
            colour = NuLgtLaserData[i].colour;
            red = colour & 0xff;
            green = (colour >> 8) & 0xff;
            blue = (u8)(colour >> 16);
            alpha = (u8)(colour >> 24);
        }
        vertices[6].w = 1.0f;

        remaining = (vertices[6].x - NuLgtLaserData[i].end.x) * (vertices[6].x - NuLgtLaserData[i].end.x) +
                    (vertices[6].y - NuLgtLaserData[i].end.y) * (vertices[6].y - NuLgtLaserData[i].end.y) +
                    (vertices[6].z - NuLgtLaserData[i].end.z) * (vertices[6].z - NuLgtLaserData[i].end.z);
        remaining = NuFsqrt(remaining);
        if (vertices[5].w < 0.5f) {
            vertices[7].x = (NuLgtLaserData[i].end.x - NuLgtLaserData[i].start.x) * (vertices[4].w - 0.5) /
                                (vertices[4].w - vertices[5].w) +
                            NuLgtLaserData[i].start.x;
            vertices[7].y = (NuLgtLaserData[i].end.y - NuLgtLaserData[i].start.y) * (vertices[4].w - 0.5) /
                                (vertices[4].w - vertices[5].w) +
                            NuLgtLaserData[i].start.y;
            vertices[7].z = (NuLgtLaserData[i].end.z - NuLgtLaserData[i].start.z) * (vertices[4].w - 0.5) /
                                (vertices[4].w - vertices[5].w) +
                            NuLgtLaserData[i].start.z;
        } else {
            vertices[7].x = NuLgtLaserData[i].end.x;
            vertices[7].y = NuLgtLaserData[i].end.y;
            vertices[7].z = NuLgtLaserData[i].end.z;
        }
        vertices[7].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[6], &clip_matrix);
        NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
        NuVec4MtxTransformVU0(&vertices[5], &vertices[7], &clip_matrix);
        NuVec4ScaleForwardPc(&vertices[5], &vertices[5], 1.0f / vertices[5].w);
        if (vertices[4].x < -1.0f && vertices[5].x < -1.0f)
            continue;
        if (vertices[4].x > 1.0f && vertices[5].x > 1.0f)
            continue;
        if (vertices[4].y < -1.0f && vertices[5].y < -1.0f)
            continue;
        if (vertices[4].y > 1.0f && vertices[5].y > 1.0f)
            continue;

        perpendicular.x = (vertices[5].y - vertices[4].y) * aspect;
        perpendicular.y = vertices[4].x - vertices[5].x;
        perpendicular.z = 0.0f;
        NuVecNorm(&perpendicular, &perpendicular);
        perpendicular.x *= aspect * NuLgtLaserData[i].width;
        perpendicular.y = NuLgtLaserData[i].width * perpendicular.y;
        dx = vertices[7].x - vertices[6].x;
        vertices[7].x = dx;
        dy = vertices[7].y - vertices[6].y;
        vertices[7].y = dy;
        dz = vertices[7].z - vertices[6].z;
        vertices[7].z = dz;
        vertices[7].w -= vertices[6].w;
        length = dx * dx + dy * dy + dz * dz;
        length = NuFsqrt(length);
        if (!(length > 0.0f))
            continue;

        step = NuLgtLaserData[i].segment_length / length;
        // Per-laser seeds make repeated and paused rendering deterministic.
        NuLgtSeed = NuLgtLaserData[i].seed;
        if (step > 1.0f)
            step = 1.0f;

        tex_u[0] = tex_u[2] = NuLgtArcMtl[0].u0;
        tex_u[1] = tex_u[3] = NuLgtArcMtl[0].u1;
        vertices[4].x = vertices[6].x;
        vertices[4].y = vertices[6].y;
        vertices[4].z = vertices[6].z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
        width_scale = 1.0f / vertices[4].w;
        NuVec4ScaleForwardPc(&vertices[4], &vertices[4], width_scale);
        if (remaining < NuLgtLaserData[i].length) {
            if (remaining < 0.0f)
                width_scale *= NuLgtLaserData[i].end_width_ratio;
            else
                width_scale += (NuLgtLaserData[i].end_width_ratio * width_scale - width_scale) *
                               (NuLgtLaserData[i].length - remaining) / NuLgtLaserData[i].length;
        }
        reds[0] = reds[1] = red;
        greens[0] = greens[1] = green;
        blues[0] = blues[1] = blue;
        alphas[0] = alphas[1] = alpha;
        offset_x = width_scale * perpendicular.x;
        offset_y = width_scale * perpendicular.y;
        vertices[1].x = vertices[4].x - offset_x;
        vertices[1].y = vertices[4].y - offset_y;
        vertices[3].x = vertices[4].x + offset_x;
        vertices[3].y = vertices[4].y + offset_y;
        vertices[1].z = vertices[4].z;
        vertices[3].z = vertices[4].z;
        remaining -= NuLgtLaserData[i].segment_length;

        fraction = 0.0f;
        wobble_limit = 1.0 - 1.5 * step;
        while (fraction < 1.0f) {
            next_fraction = fraction + step;
            if (next_fraction >= 1.0f) {
                if (NuLgtLaserData[i].arc)
                    alphas[1] = 0;
                vertices[5].x = vertices[7].x + vertices[6].x;
                vertices[5].y = vertices[6].y + vertices[7].y;
                vertices[5].z = vertices[7].z + vertices[6].z;
                tex_u[0] = tex_u[2] =
                    NuLgtArcMtl[0].u1 + (1.0f - fraction) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
            } else {
                if (NuLgtLaserData[i].arc)
                    alphas[1] = (i32)((1.0f - fraction - step) * alpha);
                vertices[5].x = vertices[7].x * next_fraction + vertices[6].x;
                vertices[5].y = vertices[7].y * next_fraction + vertices[6].y;
                vertices[5].z = vertices[7].z * next_fraction + vertices[6].z;
            }
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
            width_scale = 1.0f / vertices[5].w;
            NuVec4ScaleForwardPc(&vertices[5], &vertices[5], width_scale);
            if (remaining < NuLgtLaserData[i].length) {
                if (remaining < 0.0f)
                    width_scale *= NuLgtLaserData[i].end_width_ratio;
                else
                    width_scale += (width_scale * NuLgtLaserData[i].end_width_ratio - width_scale) *
                                   (NuLgtLaserData[i].length - remaining) / NuLgtLaserData[i].length;
            }

            if (fraction < wobble_limit && NuLgtLaserData[i].seed != 0) {
                wobble_y = (f32)((NU_LGT_RAND() & 0xff) - 128);
                wobble_x = wobble_y * perpendicular.x * NuLgtLaserData[i].width_wobble * width_scale;
                wobble_y = wobble_y * perpendicular.y * NuLgtLaserData[i].width_wobble * width_scale;
            } else {
                wobble_x = wobble_y = 0.0f;
            }
            offset_x = width_scale * perpendicular.x;
            offset_y = width_scale * perpendicular.y;
            vertices[0].x = vertices[5].x - offset_x + wobble_x;
            vertices[0].y = vertices[5].y - offset_y + wobble_y;
            vertices[2].x = vertices[5].x + offset_x + wobble_x;
            vertices[2].y = vertices[5].y + offset_y + wobble_y;
            vertices[0].z = vertices[5].z;
            vertices[2].z = vertices[5].z;
            // NuPrimSetCoordinateSystem(1), pushed.
            NuPrimCoordSystemStack[NuPrimCSPos++] = 1;
            NuPrimXScale = 1.0f;
            NuPrimYScale = -1.0f;
            NuPrimXBias = 0.0f;
            NuPrimYBias = 0.0f;
            NuPrim2DBegin(13, 7, NuLgtArcMtl[0].material);
            for (k = 0; k < 4; ++k) {
                NuLgtSetVertexColor((alphas[1 - (k & 1)] << 24) | ((blues[1 - (k & 1)] & 0xff) << 16) |
                                    ((greens[1 - (k & 1)] & 0xff) << 8) | (reds[1 - (k & 1)] & 0xff));
                vertex = (NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
                vertex->u = tex_u[k];
                vertex->v = tex_v[k];
                neg_y = vertices[k].y * -1.0;
                NuPrim2DAddXYZ(vertices[k].x, neg_y, vertices[k].z);
            }
            // NuPrim2DEnd.
            *NuPrimVertexCountPointer = NuPrimVertexCount;
            NuPrimVertexCount = 0;
            NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
            vertices[1].x = vertices[0].x;
            vertices[1].y = vertices[0].y;
            vertices[1].z = vertices[0].z;
            vertices[3].x = vertices[2].x;
            vertices[3].y = vertices[2].y;
            vertices[3].z = vertices[2].z;
            alphas[0] = alphas[1];
            fraction = (f32)next_fraction;
            remaining -= NuLgtLaserData[i].segment_length;
        }
    }
    NuLgtSeed = saved_seed;
    NuLgtLaserOldCnt = NuLgtLaserCnt;
    NuLgtLaserCnt = 0;
    NuScratchRelease();
}

void __cdecl NuLgtArcLaserDraw(i32 paused) {
    i32 i;
    f32 tex_v[4];
    NULGTVERTEX_PC *vertex;
    NUVEC4 *vertices;
    NUMTX *clip;
    NUVEC perpendicular;
    f32 length;
    f32 factor;
    f32 step;
    f32 u;
    f32 u1;
    f32 progress;
    f64 next;
    f64 wobble_limit;
    i32 angle;
    f32 *trig;
    f32 reciprocal;
    f32 jitter_x;
    f32 jitter_y;
    f32 offset_x;
    f32 offset_y;
    f32 corner_x;
    f32 corner_y;
    tex_v[0] = NuLgtArcMtl[0].v0;
    tex_v[1] = NuLgtArcMtl[0].v0;
    tex_v[2] = NuLgtArcMtl[0].v1;
    tex_v[3] = NuLgtArcMtl[0].v1;
    if (paused)
        NuLgtArcLaserCnt = NuLgtArcLaserOldCnt;
    if (!NuLgtArcMtl[0].material || NuLgtArcLaserCnt == 0) {
        NuLgtArcLaserOldCnt = 0;
        return;
    }

    vertices = (NUVEC4 *)NuScratchAlloc32(0xc0);

    clip = NuCameraGetVPCSMtx();
    if (!NuRndrBeginSceneEx(1, 2, 0)) {
        NuScratchRelease();
        return;
    }
    // NuPrimSetCoordinateSystem(0), pushed.
    NuPrimXScale = NuPrimPS2XScale;
    NuPrimYScale = NuPrimPS2YScale;
    NuPrimXBias = -1.0f;
    NuPrimYBias = 1.0f;
    NuPrimCoordSystemStack[NuPrimCSPos++] = 0;
    NuPrim2DBegin(12, 5, NuLgtArcMtl[0].material);
    for (i = 0; i < NuLgtArcLaserCnt; ++i) {

        vertices[4].x = NuLgtArcLaserData[i].start.x;
        vertices[4].y = NuLgtArcLaserData[i].start.y;
        vertices[4].z = NuLgtArcLaserData[i].start.z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
        vertices[5].x = NuLgtArcLaserData[i].end.x;
        vertices[5].y = NuLgtArcLaserData[i].end.y;
        vertices[5].z = NuLgtArcLaserData[i].end.z;
        vertices[5].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);
        if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f))
            continue;
        if (vertices[4].w < 0.5f) {
            vertices[6].x = (NuLgtArcLaserData[i].start.x - NuLgtArcLaserData[i].end.x) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w) +
                            NuLgtArcLaserData[i].end.x;
            vertices[6].y = (NuLgtArcLaserData[i].start.y - NuLgtArcLaserData[i].end.y) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w) +
                            NuLgtArcLaserData[i].end.y;
            vertices[6].z = (NuLgtArcLaserData[i].start.z - NuLgtArcLaserData[i].end.z) * (vertices[5].w - 0.5) /
                                (vertices[5].w - vertices[4].w) +
                            NuLgtArcLaserData[i].end.z;
        } else {
            vertices[6].x = NuLgtArcLaserData[i].start.x;
            vertices[6].y = NuLgtArcLaserData[i].start.y;
            vertices[6].z = NuLgtArcLaserData[i].start.z;
        }
        vertices[6].w = 1.0f;
        if (vertices[5].w < 0.5f) {
            vertices[7].x = (NuLgtArcLaserData[i].end.x - NuLgtArcLaserData[i].start.x) * (vertices[4].w - 0.5) /
                                (vertices[4].w - vertices[5].w) +
                            NuLgtArcLaserData[i].start.x;
            vertices[7].y = (NuLgtArcLaserData[i].end.y - NuLgtArcLaserData[i].start.y) * (vertices[4].w - 0.5) /
                                (vertices[4].w - vertices[5].w) +
                            NuLgtArcLaserData[i].start.y;
            vertices[7].z = (NuLgtArcLaserData[i].end.z - NuLgtArcLaserData[i].start.z) * (vertices[4].w - 0.5) /
                                (vertices[4].w - vertices[5].w) +
                            NuLgtArcLaserData[i].start.z;
        } else {
            vertices[7].x = NuLgtArcLaserData[i].end.x;
            vertices[7].y = NuLgtArcLaserData[i].end.y;
            vertices[7].z = NuLgtArcLaserData[i].end.z;
        }
        vertices[7].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[6], clip);
        NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
        NuVec4MtxTransformVU0(&vertices[5], &vertices[7], clip);
        NuVec4ScaleForwardPc(&vertices[5], &vertices[5], 1.0f / vertices[5].w);

        perpendicular.x = (vertices[5].y - vertices[4].y) * ((f32)PS2_REZ_W / 240.0f);
        perpendicular.y = vertices[4].x - vertices[5].x;
        perpendicular.z = 0.0f;
        NuVecNorm(&perpendicular, &perpendicular);
        perpendicular.x *= (((f32)PS2_REZ_W * 150.0f) / 240.0f) * NuLgtArcLaserData[i].width;
        perpendicular.y *= NuLgtArcLaserData[i].width * 150.0f;
        vertices[7].x -= vertices[6].x;
        vertices[7].y -= vertices[6].y;
        vertices[7].z -= vertices[6].z;
        vertices[7].w -= vertices[6].w;

        length = NuFsqrt(vertices[7].x * vertices[7].x + vertices[7].y * vertices[7].y + vertices[7].z * vertices[7].z);
        if (!(length > 0.0f))
            continue;
        vertices[10].x = NuLgtArcLaserData[i].bend.x;
        vertices[10].y = NuLgtArcLaserData[i].bend.y;
        vertices[10].z = NuLgtArcLaserData[i].bend.z;

        factor =
            (vertices[10].y * vertices[10].y) + (vertices[10].x * vertices[10].x) + vertices[10].z * vertices[10].z;
        if (factor > 0.0f)
            factor = (f64)NuLgtArcLaserData[i].bend_amount / NuFsqrt(factor);
        vertices[10].x *= factor;
        vertices[10].y *= factor;
        vertices[10].z *= factor;
        NuLgtSeed = NuLgtArcLaserData[i].seed;

        step = NuLgtArcLaserData[i].segment_length / length;
        step = step > 1.0f ? 1.0f : step;

        u = NuLgtArcMtl[0].u0;
        u1 = NuLgtArcMtl[0].u1;
        vertices[4].x = vertices[6].x;
        vertices[4].y = vertices[6].y;
        vertices[4].z = vertices[6].z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
        NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
        vertices[1].x = vertices[4].x;
        vertices[1].y = vertices[4].y;
        vertices[1].z = vertices[4].z;
        vertices[3].x = vertices[4].x;
        vertices[3].y = vertices[4].y;
        vertices[3].z = vertices[4].z;

        progress = 0.0f;
        wobble_limit = 1.0 - 1.5 * step;
        while (progress < 1.0f) {

            next = progress + step;
            if (next >= 1.0f) {
                vertices[5].x = vertices[6].x + vertices[7].x;
                vertices[5].y = vertices[7].y + vertices[6].y;
                vertices[5].z = vertices[6].z + vertices[7].z;
                u = NuLgtArcMtl[0].u1 + (1.0f - progress) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
            } else {

                angle = ((i32)(next * 32768.0) >> 1) & 0x7fff;
                trig = &NuTrigTable[angle];
                vertices[5].x = vertices[7].x * next + vertices[6].x + vertices[10].x * *trig;
                vertices[5].y = vertices[7].y * next + vertices[6].y + vertices[10].y * *trig;
                vertices[5].z = vertices[7].z * next + vertices[6].z + *trig * vertices[10].z;
            }
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);

            reciprocal = 1.0f / vertices[5].w;
            NuVec4ScaleForwardPc(&vertices[5], &vertices[5], reciprocal);

            if (progress < wobble_limit && NuLgtSeed != 0) {
                jitter_y = (f32)((NU_LGT_RAND() & 0xff) - 128);
                jitter_x = jitter_y * perpendicular.x * NuLgtArcLaserData[i].wobble * reciprocal;
                jitter_y = jitter_y * perpendicular.y * NuLgtArcLaserData[i].wobble * reciprocal;
            } else {
                jitter_x = jitter_y = 0.0f;
            }
            offset_x = perpendicular.x * reciprocal;
            offset_y = perpendicular.y * reciprocal;
            corner_x = vertices[5].x - offset_x + jitter_x;
            vertices[0].x = corner_x;
            corner_y = vertices[5].y - offset_y + jitter_y;
            vertices[0].y = corner_y;
            vertices[0].z = vertices[5].z;
            vertices[2].x = vertices[5].x + offset_x + jitter_x;
            vertices[2].y = vertices[5].y + offset_y + jitter_y;
            vertices[2].z = vertices[5].z;
            if (!(corner_x < -50.0 && vertices[1].x < 50.0) && !(corner_x > 700.0f && vertices[1].x > 700.0f) &&
                !(corner_y < -50.0 && vertices[1].y < -50.0) && !(corner_y > 530.0f && vertices[1].y > 530.0f)) {
                NU_LGT_ARC_VERTEX(&vertices[0], NuLgtArcLaserData[i].colour, u, tex_v[0]);
                NU_LGT_ARC_VERTEX(&vertices[1], NuLgtArcLaserData[i].colour, u1, tex_v[0]);
                NU_LGT_ARC_VERTEX(&vertices[2], NuLgtArcLaserData[i].colour, u, tex_v[2]);
                NU_LGT_ARC_VERTEX(&vertices[1], NuLgtArcLaserData[i].colour, u1, tex_v[0]);
                NU_LGT_ARC_VERTEX(&vertices[3], NuLgtArcLaserData[i].colour, u1, tex_v[2]);
                NU_LGT_ARC_VERTEX(&vertices[2], NuLgtArcLaserData[i].colour, u, tex_v[2]);
            }
            vertices[1].x = vertices[0].x;
            vertices[1].y = vertices[0].y;
            vertices[1].z = vertices[0].z;
            vertices[3].x = vertices[2].x;
            vertices[3].y = vertices[2].y;
            vertices[3].z = vertices[2].z;
            progress = (f32)next;
        }
    }
    // NuPrim2DEnd.
    *NuPrimVertexCountPointer = NuPrimVertexCount;
    NuPrimVertexCount = 0;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
    NuRndrEndSceneEx(0);
    NuLgtArcLaserOldCnt = NuLgtArcLaserCnt;
    NuLgtArcLaserCnt = 0;
    if (!paused)
        ++NuLgtArcLaserFrame;
    NuScratchRelease();
}

void __cdecl NuLgtLaser(i32 type, f32 width, f32 segment_length, f32 width_wobble, NUVEC *start, NUVEC *delta,
                        u32 colour, f32 end_width, f32 length) {
    NULGTLASER_PC *laser;
    if (NuLgtLaserCnt > 63)
        return;

    laser = &NuLgtLaserData[NuLgtLaserCnt];
    laser->width_wobble = width_wobble;
    laser->segment_length = segment_length;
    laser->arc = 0;
    laser->width = width;
    laser->type = (u8)type;
    laser->start = *start;
    laser->length = length;
    laser->end.x = delta->x + start->x;
    laser->end.y = delta->y + start->y;
    laser->end.z = delta->z + start->z;
    laser->colour = colour;
    laser->end_width_ratio = end_width / width;
    // The original tests the arc cursor here, even for a straight laser.
    if ((NuLgtArcLaserFrame & 1) == 0 || NuLgtLaserData[NuLgtArcLaserCnt].seed == 0) {
        NuLgtLaserData[NuLgtLaserCnt].seed = NU_LGT_RAND();
    }
    NU_LGT_RAND();
    NU_LGT_RAND();
    NU_LGT_RAND();
    NU_LGT_RAND();
    NU_LGT_RAND();
    NU_LGT_RAND();
    ++NuLgtLaserCnt;
}

void __cdecl NuLgtArcLaserEx(i32 type, NUVEC *start, NUVEC *end, NUVEC *bend, f32 width, f32 segment_length, f32 wobble,
                             f32 bend_amount, i32 colour, i32 flags) {
    i32 i;
    i32 index;
    NULGTARCLASER_PC *laser;
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
    index = NuLgtArcLaserCnt;
    if (index >= 16)
        return;

    laser = &NuLgtArcLaserData[index];
    laser->type = (u8)type;
    laser->start = *start;
    laser->end = *end;
    laser->bend = *bend;
    laser->width = width;
    laser->segment_length = segment_length;
    laser->wobble = wobble;
    laser->bend_amount = bend_amount;

    red = colour & 0xff;

    green = (colour >> 8) & 0xff;

    blue = (u8)(colour >> 16);

    alpha = (u8)(colour >> 24);
    if (red > 255)
        red = 255;
    if (green > 0xff00)
        green = 0xff00;
    if (blue > 255)
        blue = 255;
    if (alpha > 255)
        alpha = 255;
    laser->colour = blue | (red << 16) | green | (alpha << 24);
    laser->flags = flags;
    if ((NuLgtArcLaserFrame & 1) == 0 || laser->seed == 0)
        laser->seed = NU_LGT_RAND();
    for (i = 0; i < 6; ++i)
        NU_LGT_RAND();
    ++NuLgtArcLaserCnt;
}

void __cdecl NuLgtSetArcMat(NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
    NuLgtArcMtl[0].material = material;
    NuLgtArcMtl[0].u0 = u0;
    NuLgtArcMtl[0].v0 = v0;
    NuLgtArcMtl[0].u1 = u1;
    NuLgtArcMtl[0].v1 = v1;
}

void __cdecl NuLgtSetArcMatEx(i32 type, NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
    if (type > 3)
        return;
    NuLgtArcMtl[type].material = material;
    NuLgtArcMtl[type].u0 = u0;
    NuLgtArcMtl[type].v0 = v0;
    NuLgtArcMtl[type].u1 = u1;
    NuLgtArcMtl[type].v1 = v1;
}

void __cdecl NuLgtInitializeStatePc(void) {
    i32 i;
    NuLgtStateUnknownPc = 0;
    NuLgtStateCapacityPc = 8;
    memset(NuLgtStateSlotsPc, 0, sizeof(NuLgtStateSlotsPc));
    for (i = 0; i < 8; ++i) {
        NuLgtStateSlotsPc[i].next = i == 7 ? -1 : i + 1;
        NuLgtStateSlotsPc[i].previous = i - 1;
    }
    NuLgtStateActiveCountPc = 0;
    NuLgtArcLaserCnt = 0;
    NuLgtArcLaserOldCnt = 0;
    NuLgtStateFreeHeadPc = -1;
    NuLgtStateEnabledPc = 1;
}
