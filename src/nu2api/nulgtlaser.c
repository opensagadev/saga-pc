#include "saga.h"
#include <math.h>

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
#define NU_LGT_VERTEX(position, colour, tex_u, tex_v)                                                                  \
    do {                                                                                                               \
        NuLgtSetVertexColor(colour);                                                                                   \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = (tex_u);                                          \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = (tex_v);                                          \
        NuPrim2DAddXYZ((position)->x, -(position)->y, (position)->z);                                                  \
    } while (0)
#define NU_LGT_ARC_VERTEX(position, colour, tex_u, tex_v)                                                              \
    do {                                                                                                               \
        NuLgtSetVertexColor(colour);                                                                                   \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->u = (tex_u);                                          \
        ((NULGTVERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->v = (tex_v);                                          \
        NuPrim2DAddXYZ((position)->x, (position)->y, (position)->z);                                                   \
    } while (0)

void __cdecl NuLgtLaserDraw(i32 paused) {
    i32 i;
    f32 v0;
    f32 v1;
    u32 saved_seed;
    NUVEC4 *vertices;
    NUMTX clip_matrix;
    f32 aspect;
    NULGTLASER_PC *laser;
    i32 red;
    i32 green;
    i32 blue;
    i32 alpha;
    f32 numerator;
    f32 denominator;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 remaining;
    NUVEC perpendicular;
    f32 length;
    f32 step;
    f32 u0;
    f32 u1;
    f32 width_scale;
    f32 fraction;
    i32 start_alpha;
    f32 next_fraction;
    i32 end_alpha;
    f32 wobble_x;
    f32 wobble_y;
    f32 random;
    if (paused)
        NuLgtLaserCnt = NuLgtLaserOldCnt;
    if (NuLgtArcMtl[0].material == NULL || NuLgtLaserCnt == 0) {
        NuLgtLaserOldCnt = 0;
        return;
    }

    v0 = NuLgtArcMtl[0].v0;

    v1 = NuLgtArcMtl[0].v1;

    saved_seed = NuLgtSeed;
    // Four strip corners, two projected endpoints, and two clipped world endpoints.

    vertices = (NUVEC4 *)NuScratchAlloc32(0x80);

    NuCameraGetClipMtx(&clip_matrix, NULL);

    aspect = (f32)PS2_REZ_W / (f32)PS2_REZ_H;

    laser = NuLgtLaserData;
    for (i = 0; i < NuLgtLaserCnt; ++i, ++laser) {
        vertices[4].x = laser->start.x;
        vertices[4].y = laser->start.y;
        vertices[4].z = laser->start.z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
        vertices[5].x = laser->end.x;
        vertices[5].y = laser->end.y;
        vertices[5].z = laser->end.z;
        vertices[5].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
        if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f))
            continue;

        red = laser->colour & 0xff;

        green = (laser->colour >> 8) & 0xff;

        blue = (laser->colour >> 16) & 0xff;

        alpha = laser->colour >> 24;
        if (vertices[4].w < 0.5f) {

            numerator = vertices[5].w - 0.5f;

            denominator = vertices[5].w - vertices[4].w;
            if (laser->arc) {
                red = (i32)((f32)red * numerator / denominator);
                green = (i32)((f32)green * numerator / denominator);
                blue = (i32)((f32)blue * numerator / denominator);
                alpha = (i32)((f32)alpha * numerator / denominator);
            }
            vertices[6].x = (laser->start.x - laser->end.x) * numerator / denominator + laser->end.x;
            vertices[6].y = (laser->start.y - laser->end.y) * numerator / denominator + laser->end.y;
            vertices[6].z = (laser->start.z - laser->end.z) * numerator / denominator + laser->end.z;
        } else {
            vertices[6].x = laser->start.x;
            vertices[6].y = laser->start.y;
            vertices[6].z = laser->start.z;
        }
        vertices[6].w = 1.0f;

        dx = vertices[6].x - laser->end.x;

        dy = vertices[6].y - laser->end.y;

        dz = vertices[6].z - laser->end.z;

        remaining = NuFsqrt(dx * dx + dy * dy + dz * dz);
        if (vertices[5].w < 0.5f) {

            numerator = vertices[4].w - 0.5f;

            denominator = vertices[4].w - vertices[5].w;
            vertices[7].x = (laser->end.x - laser->start.x) * numerator / denominator + laser->start.x;
            vertices[7].y = (laser->end.y - laser->start.y) * numerator / denominator + laser->start.y;
            vertices[7].z = (laser->end.z - laser->start.z) * numerator / denominator + laser->start.z;
        } else {
            vertices[7].x = laser->end.x;
            vertices[7].y = laser->end.y;
            vertices[7].z = laser->end.z;
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
        perpendicular.x *= laser->width * aspect;
        perpendicular.y *= laser->width;
        vertices[7].x -= vertices[6].x;
        vertices[7].y -= vertices[6].y;
        vertices[7].z -= vertices[6].z;
        vertices[7].w -= vertices[6].w;

        length = NuFsqrt(vertices[7].x * vertices[7].x + vertices[7].y * vertices[7].y + vertices[7].z * vertices[7].z);
        if (!(length > 0.0f))
            continue;

        step = laser->segment_length / length;
        if (step > 1.0f)
            step = 1.0f;
        // Per-laser seeds make repeated and paused rendering deterministic.
        NuLgtSeed = laser->seed;

        u0 = NuLgtArcMtl[0].u0;

        u1 = NuLgtArcMtl[0].u1;
        vertices[4].x = vertices[6].x;
        vertices[4].y = vertices[6].y;
        vertices[4].z = vertices[6].z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);

        width_scale = 1.0f / vertices[4].w;
        NuVec4ScaleForwardPc(&vertices[4], &vertices[4], width_scale);
        if (laser->length > remaining) {
            if (remaining < 0.0f)
                width_scale *= laser->end_width_ratio;
            else
                width_scale +=
                    (laser->end_width_ratio * width_scale - width_scale) * (laser->length - remaining) / laser->length;
        }
        vertices[1].x = vertices[4].x - perpendicular.x * width_scale;
        vertices[1].y = vertices[4].y - perpendicular.y * width_scale;
        vertices[1].z = vertices[4].z;
        vertices[3].x = vertices[4].x + perpendicular.x * width_scale;
        vertices[3].y = vertices[4].y + perpendicular.y * width_scale;
        vertices[3].z = vertices[4].z;
        remaining -= laser->segment_length;

        fraction = 0.0f;

        start_alpha = alpha;
        for (;;) {

            next_fraction = fraction + step;

            end_alpha = start_alpha;
            if (next_fraction < 1.0f) {
                if (laser->arc)
                    end_alpha = (i32)((1.0f - fraction - step) * (f32)alpha);
                vertices[5].x = next_fraction * vertices[7].x + vertices[6].x;
                vertices[5].y = next_fraction * vertices[7].y + vertices[6].y;
                vertices[5].z = next_fraction * vertices[7].z + vertices[6].z;
            } else {
                if (laser->arc)
                    end_alpha = 0;
                vertices[5].x = vertices[6].x + vertices[7].x;
                vertices[5].y = vertices[6].y + vertices[7].y;
                vertices[5].z = vertices[6].z + vertices[7].z;
                u0 = NuLgtArcMtl[0].u1 + (1.0f - fraction) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
            }
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
            width_scale = 1.0f / vertices[5].w;
            NuVec4ScaleForwardPc(&vertices[5], &vertices[5], width_scale);
            if (laser->length > remaining) {
                if (remaining < 0.0f)
                    width_scale *= laser->end_width_ratio;
                else
                    width_scale += (width_scale * laser->end_width_ratio - width_scale) * (laser->length - remaining) /
                                   laser->length;
            }

            wobble_x = 0.0f;
            wobble_y = 0.0f;
            if (1.0f - 1.5f * step > fraction && laser->seed != 0) {

                random = (f32)((NU_LGT_RAND() & 0xff) - 128);
                wobble_x = perpendicular.x * random * laser->width_wobble * width_scale;
                wobble_y = perpendicular.y * random * laser->width_wobble * width_scale;
            }
            vertices[0].x = vertices[5].x - perpendicular.x * width_scale + wobble_x;
            vertices[0].y = vertices[5].y - perpendicular.y * width_scale + wobble_y;
            vertices[0].z = vertices[5].z;
            vertices[2].x = vertices[5].x + perpendicular.x * width_scale + wobble_x;
            vertices[2].y = vertices[5].y + perpendicular.y * width_scale + wobble_y;
            vertices[2].z = vertices[5].z;
            NuPrimSetCoordinateSystem(1);
            ++NuPrimCSPos;
            NuPrim2DBegin(13, 7, NuLgtArcMtl[0].material);
            NU_LGT_VERTEX(&vertices[0],
                          ((red & 255) | ((green & 255) << 8) | ((blue & 255) << 16)) | ((u32)end_alpha << 24), u0, v0);
            NU_LGT_VERTEX(&vertices[1],
                          ((red & 255) | ((green & 255) << 8) | ((blue & 255) << 16)) | ((u32)start_alpha << 24), u1,
                          v0);
            NU_LGT_VERTEX(&vertices[2],
                          ((red & 255) | ((green & 255) << 8) | ((blue & 255) << 16)) | ((u32)end_alpha << 24), u0, v1);
            NU_LGT_VERTEX(&vertices[3],
                          ((red & 255) | ((green & 255) << 8) | ((blue & 255) << 16)) | ((u32)start_alpha << 24), u1,
                          v1);
            NuPrim2DEnd();
            NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
            vertices[1].x = vertices[0].x;
            vertices[1].y = vertices[0].y;
            vertices[1].z = vertices[0].z;
            vertices[3].x = vertices[2].x;
            vertices[3].y = vertices[2].y;
            vertices[3].z = vertices[2].z;
            remaining -= laser->segment_length;
            if (!(1.0f > next_fraction))
                break;
            fraction = next_fraction;
            start_alpha = end_alpha;
        }
    }
    NuLgtSeed = saved_seed;
    NuLgtLaserOldCnt = NuLgtLaserCnt;
    NuLgtLaserCnt = 0;
    NuScratchRelease();
}

void __cdecl NuLgtArcLaserDraw(i32 paused) {
    i32 i;
    f32 v0;
    f32 v1;
    NUVEC4 *vertices;
    NUMTX *clip;
    NULGTARCLASER_PC *laser;
    f32 numerator;
    f32 denominator;
    f32 dx;
    f32 dy;
    f32 dz;
    NUVEC perpendicular;
    f32 length;
    f32 factor;
    f32 step;
    f32 u;
    f32 u1;
    f32 progress;
    f32 next;
    i32 angle;
    f32 reciprocal;
    f32 jitter_x;
    f32 jitter_y;
    f32 random;
    v0 = NuLgtArcMtl[0].v0;
    v1 = NuLgtArcMtl[0].v1;
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
    NuPrimSetCoordinateSystem(0);
    ++NuPrimCSPos;
    NuPrim2DBegin(12, 5, NuLgtArcMtl[0].material);
    for (i = 0; i < NuLgtArcLaserCnt; ++i) {

        laser = &NuLgtArcLaserData[i];
        vertices[4].x = laser->start.x;
        vertices[4].y = laser->start.y;
        vertices[4].z = laser->start.z;
        vertices[4].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
        vertices[5].x = laser->end.x;
        vertices[5].y = laser->end.y;
        vertices[5].z = laser->end.z;
        vertices[5].w = 1.0f;
        NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);
        if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f))
            continue;
        if (vertices[4].w < 0.5f) {

            numerator = vertices[5].w - 0.5f;

            denominator = vertices[5].w - vertices[4].w;
            vertices[6].x = (laser->start.x - laser->end.x) * numerator / denominator + laser->end.x;
            vertices[6].y = (laser->start.y - laser->end.y) * numerator / denominator + laser->end.y;
            vertices[6].z = (laser->start.z - laser->end.z) * numerator / denominator + laser->end.z;
        } else {
            vertices[6].x = laser->start.x;
            vertices[6].y = laser->start.y;
            vertices[6].z = laser->start.z;
        }
        vertices[6].w = 1.0f;

        dx = vertices[6].x - laser->end.x;

        dy = vertices[6].y - laser->end.y;

        dz = vertices[6].z - laser->end.z;
        sqrt((double)(dx * dx + dy * dy + dz * dz));
        if (vertices[5].w < 0.5f) {

            numerator = vertices[4].w - 0.5f;

            denominator = vertices[4].w - vertices[5].w;
            vertices[7].x = (laser->end.x - laser->start.x) * numerator / denominator + laser->start.x;
            vertices[7].y = (laser->end.y - laser->start.y) * numerator / denominator + laser->start.y;
            vertices[7].z = (laser->end.z - laser->start.z) * numerator / denominator + laser->start.z;
        } else {
            vertices[7].x = laser->end.x;
            vertices[7].y = laser->end.y;
            vertices[7].z = laser->end.z;
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
        perpendicular.x *= (((f32)PS2_REZ_W * 150.0f) / 240.0f) * laser->width;
        perpendicular.y *= laser->width * 150.0f;
        vertices[7].x -= vertices[6].x;
        vertices[7].y -= vertices[6].y;
        vertices[7].z -= vertices[6].z;
        vertices[7].w -= vertices[6].w;

        length = NuFsqrt(vertices[7].x * vertices[7].x + vertices[7].y * vertices[7].y + vertices[7].z * vertices[7].z);
        if (!(length > 0.0f))
            continue;
        vertices[10].x = laser->bend.x;
        vertices[10].y = laser->bend.y;
        vertices[10].z = laser->bend.z;

        factor = vertices[10].x * vertices[10].x + vertices[10].y * vertices[10].y + vertices[10].z * vertices[10].z;
        if (factor > 0.0f)
            factor = laser->bend_amount / NuFsqrt(factor);
        vertices[10].x *= factor;
        vertices[10].y *= factor;
        vertices[10].z *= factor;
        NuLgtSeed = laser->seed;

        step = laser->segment_length / length;
        step = 1.0f < step ? 1.0f : step;

        u = NuLgtArcMtl[0].u0;
        u1 = NuLgtArcMtl[0].u1;
        vertices[4].w = 1.0f;
        vertices[4].x = vertices[6].x;
        vertices[4].y = vertices[6].y;
        vertices[4].z = vertices[6].z;
        NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
        NuVec4ScaleForwardPc(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
        vertices[1].x = vertices[3].x = vertices[4].x;
        vertices[1].y = vertices[3].y = vertices[4].y;
        vertices[1].z = vertices[3].z = vertices[4].z;

        progress = 0.0f;
        do {

            next = progress + step;
            if (next >= 1.0f) {
                vertices[5].x = vertices[6].x + vertices[7].x;
                vertices[5].y = vertices[6].y + vertices[7].y;
                vertices[5].z = vertices[6].z + vertices[7].z;
                u = NuLgtArcMtl[0].u1 + (1.0f - progress) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
            } else {

                angle = ((i32)(32768.0f * next) >> 1) & 0x7fff;
                vertices[5].x = next * vertices[7].x + vertices[6].x + vertices[10].x * NuTrigTable[angle];
                vertices[5].y = next * vertices[7].y + vertices[6].y + vertices[10].y * NuTrigTable[angle];
                vertices[5].z = next * vertices[7].z + vertices[6].z + vertices[10].z * NuTrigTable[angle];
            }
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);

            reciprocal = 1.0f / vertices[5].w;
            NuVec4ScaleForwardPc(&vertices[5], &vertices[5], reciprocal);

            jitter_x = 0.0f;
            jitter_y = 0.0f;
            if (1.0f - 1.5f * step > progress && NuLgtSeed != 0) {

                random = (f32)((NU_LGT_RAND() & 0xff) - 128);
                jitter_x = perpendicular.x * random * laser->wobble * reciprocal;
                jitter_y = perpendicular.y * random * laser->wobble * reciprocal;
            }
            vertices[0].x = vertices[5].x - perpendicular.x * reciprocal + jitter_x;
            vertices[0].y = vertices[5].y - perpendicular.y * reciprocal + jitter_y;
            vertices[0].z = vertices[5].z;
            vertices[2].x = perpendicular.x * reciprocal + vertices[5].x + jitter_x;
            vertices[2].y = perpendicular.y * reciprocal + vertices[5].y + jitter_y;
            vertices[2].z = vertices[5].z;
            if (!(vertices[0].x < -50.0f && vertices[1].x < 50.0f) &&
                !(vertices[0].x > 700.0f && vertices[1].x > 700.0f) &&
                !(vertices[0].y < -50.0f && vertices[1].y < -50.0f) &&
                !(vertices[0].y > 530.0f && vertices[1].y > 530.0f)) {
                NU_LGT_ARC_VERTEX(&vertices[0], laser->colour, u, v0);
                NU_LGT_ARC_VERTEX(&vertices[1], laser->colour, u1, v0);
                NU_LGT_ARC_VERTEX(&vertices[2], laser->colour, u, v1);
                NU_LGT_ARC_VERTEX(&vertices[1], laser->colour, u1, v0);
                NU_LGT_ARC_VERTEX(&vertices[3], laser->colour, u1, v1);
                NU_LGT_ARC_VERTEX(&vertices[2], laser->colour, u, v1);
            }
            vertices[1].x = vertices[0].x;
            vertices[1].y = vertices[0].y;
            vertices[1].z = vertices[0].z;
            vertices[3].x = vertices[2].x;
            vertices[3].y = vertices[2].y;
            vertices[3].z = vertices[2].z;
            progress = next;
        } while (1.0f > progress);
    }
    NuPrim2DEnd();
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

    red = ((u32)colour & 0xff) * 2;

    green = ((u32)colour & 0xff00) * 2;

    blue = ((u32)colour & 0xff0000) >> 15;

    alpha = ((u32)colour >> 24) * 2;
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

void __cdecl NuLgtSetArcMatEx(i32 type, NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
    if (type > 3)
        return;
    NuLgtArcMtl[type].material = material;
    NuLgtArcMtl[type].u0 = u0;
    NuLgtArcMtl[type].v0 = v0;
    NuLgtArcMtl[type].u1 = u1;
    NuLgtArcMtl[type].v1 = v1;
}
