#include "saga.h"

#include <string.h>

extern u8 snakebodies[0x440];
static NUSPECIALHANDLE snake_hspecials[3] = {0};

void __cdecl InitSnakes(WORLDINFO *world) {
    memset(snakebodies, 0, sizeof(snakebodies));
    NuSpecialFind(world->current_gscn, &snake_hspecials[0], "Snake_bit_1", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[1], "Snake_bit_2", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[2], "Snake_bit_3", 1);
}

typedef struct snake_segment_pc_s {
    NUVEC position;
    f32 ground_height;
    NUANG yaw, pitch;
} SNAKE_SEGMENT_PC;
typedef struct snake_body_pc_s {
    SNAKE_SEGMENT_PC segments[11];
    f32 scale;
    u16 segment_count;
    u16 flags;
} SNAKE_BODY_PC;
typedef struct snake_object_reader_pc_s {
    u8 unknown_000[0x4d8];
    PART_LIGHTING_DATA_PC lighting;
    u8 unknown_5a4[0x10ac - 0x5a4];
    SNAKE_BODY_PC *snake_body;
} SNAKE_OBJECT_READER_PC;
STATIC_ASSERT(sizeof(SNAKE_SEGMENT_PC) == 0x18);
STATIC_ASSERT(offsetof(SNAKE_SEGMENT_PC, yaw) == 0x10);
STATIC_ASSERT(offsetof(SNAKE_SEGMENT_PC, pitch) == 0x14);
STATIC_ASSERT(offsetof(SNAKE_BODY_PC, scale) == 0x108);
STATIC_ASSERT(offsetof(SNAKE_BODY_PC, segment_count) == 0x10c);
STATIC_ASSERT(sizeof(SNAKE_BODY_PC) == 0x110);
STATIC_ASSERT(offsetof(SNAKE_OBJECT_READER_PC, lighting) == 0x4d8);
STATIC_ASSERT(offsetof(SNAKE_OBJECT_READER_PC, snake_body) == 0x10ac);
extern f32 SnakeSegmentHeightPc, SnakeDebrisUpwardSpeedPc;
extern f32 SnakeSegmentSpacingPc, SnakeHeadHeightPc, SnakeYawSeekRatePc;
void __cdecl NuMtxSetRotationXYVU0(NUMTX *, NUANGVEC *);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 PCPathArcSineCoefficient3
#define NEG_3_OVER_40 PCPathArcSineCoefficient5
#define NEG_5_OVER_112 PCPathArcSineCoefficient7
#define NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define MAX_SHORT_OVER_PI 10430.4f
#define POW2(x) ((x) * (x))
#define POW3(x) ((x) * POW2(x))
#define POW4(x) (POW2(x) * POW2(x))
#define POW5(x) (POW2(x) * POW3(x))
/* Unit-local copy of the NuASin polynomial (the snake unit carries its own instance). */
i32 __cdecl SnakeASinPc(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude;
    magnitude.value = sin;
    magnitude.bits &= 0x7fffffff;
    abs = magnitude.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}
#undef MIN
#undef MAX
#undef PI_OVER_4
#undef NEG_1_OVER_6
#undef NEG_3_OVER_40
#undef NEG_5_OVER_112
#undef NEG_35_OVER_1152
#undef MAX_SHORT_OVER_PI
#undef POW2
#undef POW3
#undef POW4
#undef POW5

void __cdecl UpdateSnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER_PC *reader = (SNAKE_OBJECT_READER_PC *)object;
    SNAKE_BODY_PC *body;
    NUVEC position, offset;
    i32 index;
    f32 ground, length;
    if (object == NULL || reader->snake_body == NULL)
        return;
    object->collision_scale = reader->snake_body->scale;
    position = object->lower_position;
    position.y += reader->snake_body->scale * SnakeHeadHeightPc;
    for (index = 0; index < reader->snake_body->segment_count; ++index) {
        body = reader->snake_body;
        body->segments[index].position = position;
        body = reader->snake_body;
        body->segments[index].yaw =
            SeekRot((u16)body->segments[index].yaw,
                    index == 0 ? object->field_0x276 : reader->snake_body->segments[index - 1].yaw, SnakeYawSeekRatePc);
        reader->snake_body->segments[index].pitch = 0;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = -(reader->snake_body->scale * SnakeSegmentSpacingPc);
        NuVecRotateY(&offset, &offset, (NUANG)reader->snake_body->segments[index].yaw);
        NuVecAdd(&position, &reader->snake_body->segments[index].position, &offset);
        ground = GameShadow(NULL, &position, 5.0f, 0x1f);
        if (ground == 2000000.0f)
            ground = reader->snake_body->segments[index].position.y;
        body = reader->snake_body;
        if (body->segments[index].ground_height == 1000000000.0f)
            body->segments[index].ground_height = ground;
        else
            body->segments[index].ground_height = SeekValF(body->segments[index].ground_height, ground, 10.0f);
        body = reader->snake_body;
        if (body->segments[index].ground_height != 2000000.0f) {
            position.y = body->segments[index].ground_height;
            length = NuVecDist(&position, &body->segments[index].position, &offset);
            NuVecScale(&offset, &offset, reader->snake_body->scale * SnakeSegmentSpacingPc / length);
            NuVecAdd(&position, &reader->snake_body->segments[index].position, &offset);
            reader->snake_body->segments[index].pitch =
                (i16)SnakeASinPc(-offset.y / (reader->snake_body->scale * SnakeSegmentSpacingPc));
        }
    }
}
void __cdecl SnakeBeenHit(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER_PC *reader = (SNAKE_OBJECT_READER_PC *)object;
    SNAKE_BODY_PC *body;
    i32 i, index, special_index;
    NUMTX matrix;
    NUANGVEC angles;
    NUVEC scale, momentum;
    ADDPART_PC params;
    NUSPECIALHANDLE *special;
    if (object == NULL || reader->snake_body == NULL || reader->snake_body->segment_count <= 2)
        return;
    for (i = 0; i < 2; ++i) {
        body = reader->snake_body;
        index = body->segment_count - i - 1;
        angles.y = NuAngAdd(body->segments[index].yaw, 0x8000);
        angles.x = body->segments[index].pitch;
        NuMtxSetRotationXYVU0(&matrix, &angles);
        body = reader->snake_body;
        if (body->scale != 1.0f) {
            scale.x = scale.y = scale.z = body->scale;
            NuMtxPreScale(&matrix, &scale);
        }
        body = reader->snake_body;
        matrix.m30 += body->segments[index].position.x;
        matrix.m31 += body->segments[index].position.y + body->scale * SnakeSegmentHeightPc;
        matrix.m32 += body->segments[index].position.z;
        special_index = index == body->segment_count - 1 ? 2 : index % 2;
        special = &snake_hspecials[special_index];
        if (NuSpecialExistsFn(special)) {
            SetKillPartMom(&momentum);
            momentum.y += SnakeDebrisUpwardSpeedPc;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.field_14 = params.field_18 = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.flags = 0x90;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            params.time_step = FRAMETIME;
            params.lighting_data = &reader->lighting;
            AddPart(&params);
        }
    }
    reader->snake_body->segment_count -= 2;
}

void __cdecl DestroySnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER_PC *reader = (SNAKE_OBJECT_READER_PC *)object;
    if (object != NULL && reader->snake_body != NULL) {
        memset(reader->snake_body, 0, sizeof(*reader->snake_body));
        reader->snake_body = NULL;
    }
}

void __cdecl BlowUpSnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER_PC *reader = (SNAKE_OBJECT_READER_PC *)object;
    SNAKE_BODY_PC *body;
    i32 index, special_index;
    NUMTX matrix;
    NUANGVEC angles;
    NUVEC scale, momentum;
    ADDPART_PC params;
    NUSPECIALHANDLE *special;
    if (object == NULL || reader->snake_body == NULL)
        return;
    for (index = 0; index < reader->snake_body->segment_count; ++index) {
        angles.y = NuAngAdd(reader->snake_body->segments[index].yaw, 0x8000);
        angles.x = reader->snake_body->segments[index].pitch;
        NuMtxSetRotationXYVU0(&matrix, &angles);
        body = reader->snake_body;
        if (body->scale != 1.0f) {
            scale.x = scale.y = scale.z = body->scale;
            NuMtxPreScale(&matrix, &scale);
        }
        body = reader->snake_body;
        matrix.m30 += body->segments[index].position.x;
        matrix.m31 = (body->scale * SnakeSegmentHeightPc + body->segments[index].position.y) + matrix.m31;
        matrix.m32 = body->segments[index].position.z + matrix.m32;
        special_index = index == body->segment_count - 1 ? 2 : index % 2;
        special = &snake_hspecials[special_index];
        if (NuSpecialExistsFn(special)) {
            SetKillPartMom(&momentum);
            momentum.y += SnakeDebrisUpwardSpeedPc;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.field_14 = params.field_18 = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.flags = 0x90;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            params.time_step = FRAMETIME;
            params.lighting_data = &reader->lighting;
            AddPart(&params);
        }
    }
    if (reader->snake_body != NULL) {
        memset(reader->snake_body, 0, sizeof(*reader->snake_body));
        reader->snake_body = NULL;
    }
}
