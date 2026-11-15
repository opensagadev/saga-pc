#include "legoapi/actions/character/snake.h"
#include "legoapi/characters/motion.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

extern u8 snakebodies[0x440];
static nuhspecial_s snake_hspecials[3] = {0};
u8 snakebodies[0x440] = {0};

void InitSnakes(WORLDINFO *world) {
    memset(snakebodies, 0, sizeof(snakebodies));
    NuSpecialFind(world->current_gscn, &snake_hspecials[0], "Snake_bit_1", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[1], "Snake_bit_2", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[2], "Snake_bit_3", 1);
}

typedef struct snake_segment_s { // size 0x18
    NUVEC position;              // 0x00
    f32 ground_height;           // 0x0c
    NUANG yaw;                   // 0x10
    NUANG pitch;                 // 0x14
} SNAKE_SEGMENT;

typedef struct snake_body_s {   // size 0x110
    SNAKE_SEGMENT segments[11]; // 0x000
    f32 scale;                  // 0x108
    u16 segment_count;          // 0x10c
    u16 flags;                  // 0x10e
} SNAKE_BODY;

typedef struct snake_object_reader_s { // size 0x10b0
    u8 unknown_000[0x4d8];             // 0x0000
    PART_LIGHTING_DATA lighting;       // 0x04d8
    u8 unknown_5a4[0x10ac - 0x5a4];    // 0x05a4
    SNAKE_BODY *snake_body;            // 0x10ac
} SNAKE_OBJECT_READER;

extern f32 SnakeSegmentSpacing;
extern f32 SnakeHeadHeight;
extern f32 SnakeYawSeekRate;

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 SnakeFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

/* Unit-local copy of the NuASin polynomial (the snake unit carries its own instance). */
i32 SnakeASin(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    abs = SnakeFabs(sin);
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = -1.0f < unknown_c ? unknown_c : -1.0f;

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}

f32 SnakeSegmentSpacing = 0.15f;
f32 SnakeHeadHeight = 0.15f;
f32 SnakeYawSeekRate = 5.0f;
f32 SnakeSegmentHeight = 0.02f;
f32 SnakeDebrisUpwardSpeed = 1.0f;
static f32 SnakeBodyDefaultScale = 1.0f;

SNAKE_BODY *CreateSnakeBody(GAMEOBJECT *object, i32 segment_count) {
    SNAKE_BODY *body;
    i32 slot;
    i32 index;

    for (slot = 0; slot < 4; slot++) {
        body = &((SNAKE_BODY *)snakebodies)[slot];
        if ((body->flags & 1) == 0) {
            ((SNAKE_OBJECT_READER *)object)->snake_body = body;
            body->scale = SnakeBodyDefaultScale;
            body->flags |= 1;
            body->segment_count = (u16)segment_count;
            for (index = 0; index < body->segment_count; index++) {
                body->segments[index].yaw = object->unknown_276;
                body->segments[index].pitch = 0;
                body->segments[index].ground_height = 1000000000.0f;
            }
            return body;
        }
    }
    return NULL;
}

void UpdateSnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER *reader = (SNAKE_OBJECT_READER *)object;
    NUVEC position;
    NUVEC offset;
    i32 index;
    f32 ground;
    f32 length;
    if (object == NULL || reader->snake_body == NULL) {
        return;
    }
    object->collision_scale = reader->snake_body->scale;
    position = object->lower_position;
    position.y += reader->snake_body->scale * SnakeHeadHeight;
    for (index = 0; index < reader->snake_body->segment_count; ++index) {
        reader->snake_body->segments[index].position = position;
        reader->snake_body->segments[index].yaw =
            SeekRot((u16)reader->snake_body->segments[index].yaw,
                    index != 0 ? reader->snake_body->segments[index - 1].yaw : object->unknown_276, SnakeYawSeekRate);
        reader->snake_body->segments[index].pitch = 0;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = -(reader->snake_body->scale * SnakeSegmentSpacing);
        NuVecRotateY(&offset, &offset, reader->snake_body->segments[index].yaw);
        NuVecAdd(&position, &reader->snake_body->segments[index].position, &offset);
        ground = GameShadow(NULL, &position, 5.0f, 0x1f);
        if (ground == 2000000.0f) {
            ground = reader->snake_body->segments[index].position.y;
        }
        if (reader->snake_body->segments[index].ground_height == 1000000000.0f) {
            reader->snake_body->segments[index].ground_height = ground;
        } else {
            reader->snake_body->segments[index].ground_height = SeekValF(reader->snake_body->segments[index].ground_height, ground, 10.0f);
        }
        if (reader->snake_body->segments[index].ground_height != 2000000.0f) {
            position.y = reader->snake_body->segments[index].ground_height;
            length = NuVecDist(&position, &reader->snake_body->segments[index].position, &offset);
            NuVecScale(&offset, &offset, reader->snake_body->scale * SnakeSegmentSpacing / length);
            NuVecAdd(&position, &reader->snake_body->segments[index].position, &offset);
            reader->snake_body->segments[index].pitch = (i16)SnakeASin(-offset.y / (reader->snake_body->scale * SnakeSegmentSpacing));
        }
    }
}

void SnakeBeenHit(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER *reader = (SNAKE_OBJECT_READER *)object;
    i32 i;
    i32 index;
    i32 special_index;
    NUMTX matrix;
    NUANGVEC angles;
    NUVEC scale;
    NUVEC momentum;
    ADDPART params;
    nuhspecial_s *special;
    if (object == NULL || reader->snake_body == NULL || reader->snake_body->segment_count <= 2) {
        return;
    }
    for (i = 0; i < 2; ++i) {
        index = reader->snake_body->segment_count - i - 1;
        angles.y = NuAngAdd(reader->snake_body->segments[index].yaw, 0x8000);
        angles.x = reader->snake_body->segments[index].pitch;
        NuMtxSetRotationXYVU0(&matrix, &angles);
        if (reader->snake_body->scale != 1.0f) {
            scale.x = scale.y = scale.z = reader->snake_body->scale;
            NuMtxPreScale(&matrix, &scale);
        }
        matrix.m30 += reader->snake_body->segments[index].position.x;
        matrix.m31 += reader->snake_body->segments[index].position.y + reader->snake_body->scale * SnakeSegmentHeight;
        matrix.m32 += reader->snake_body->segments[index].position.z;
        special_index = index == reader->snake_body->segment_count - 1 ? 2 : index % 2;
        special = &snake_hspecials[special_index];
        if (NuSpecialExistsFn(special)) {
            SetKillPartMom(&momentum);
            momentum.y += SnakeDebrisUpwardSpeed;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.unknown_14 = 0.1f;
            params.unknown_18 = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.time_step = FRAMETIME;
            params.lighting_data = &reader->lighting;
            params.flags = PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_ROTATE_RANDOMLY;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            AddPart(&params);
        }
    }
    reader->snake_body->segment_count -= 2;
}

void DestroySnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER *reader = (SNAKE_OBJECT_READER *)object;
    if (object != NULL && reader->snake_body != NULL) {
        memset(reader->snake_body, 0, sizeof(*reader->snake_body));
        reader->snake_body = NULL;
    }
}

void BlowUpSnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER *reader = (SNAKE_OBJECT_READER *)object;
    i32 index;
    i32 special_index;
    NUMTX matrix;
    NUANGVEC angles;
    NUVEC scale;
    NUVEC momentum;
    ADDPART params;
    nuhspecial_s *special;
    if (object == NULL || reader->snake_body == NULL) {
        return;
    }
    for (index = 0; index < reader->snake_body->segment_count; ++index) {
        angles.y = NuAngAdd(reader->snake_body->segments[index].yaw, 0x8000);
        angles.x = reader->snake_body->segments[index].pitch;
        NuMtxSetRotationXYVU0(&matrix, &angles);
        if (reader->snake_body->scale != 1.0f) {
            scale.x = scale.y = scale.z = reader->snake_body->scale;
            NuMtxPreScale(&matrix, &scale);
        }
        matrix.m30 = matrix.m30 + reader->snake_body->segments[index].position.x;
        matrix.m31 = reader->snake_body->scale * SnakeSegmentHeight + reader->snake_body->segments[index].position.y + matrix.m31;
        matrix.m32 = reader->snake_body->segments[index].position.z + matrix.m32;
        special_index = index == reader->snake_body->segment_count - 1 ? 2 : index % 2;
        special = &snake_hspecials[special_index];
        if (NuSpecialExistsFn(special)) {
            SetKillPartMom(&momentum);
            momentum.y += SnakeDebrisUpwardSpeed;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.unknown_14 = 0.1f;
            params.unknown_18 = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.time_step = FRAMETIME;
            params.lighting_data = &reader->lighting;
            params.flags = PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_ROTATE_RANDOMLY;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            AddPart(&params);
        }
    }
    if (reader->snake_body != NULL) {
        memset(reader->snake_body, 0, sizeof(*reader->snake_body));
        reader->snake_body = NULL;
    }
}

void DrawSnakeBody(GAMEOBJECT *object) {
    SNAKE_OBJECT_READER *reader = (SNAKE_OBJECT_READER *)object;
    i32 segment_index;
    i32 special_index;
    NUVEC scale;
    NUMTX matrix;
    NUANGVEC angles;
    if (object == NULL || reader->snake_body == NULL) {
        return;
    }
    for (segment_index = 0; segment_index < reader->snake_body->segment_count; ++segment_index) {
        object->collision_scale = reader->snake_body->scale;
        angles.y = NuAngAdd(reader->snake_body->segments[segment_index].yaw, 0x8000);
        angles.x = reader->snake_body->segments[segment_index].pitch;
        NuMtxSetRotationXYVU0(&matrix, &angles);
        if (reader->snake_body->scale != 1.0f) {
            scale.z = reader->snake_body->scale;
            scale.y = scale.z;
            scale.x = scale.y;
            NuMtxPreScale(&matrix, &scale);
        }
        matrix.m30 = reader->snake_body->segments[segment_index].position.x + matrix.m30;
        matrix.m31 = reader->snake_body->scale * SnakeSegmentHeight + reader->snake_body->segments[segment_index].position.y + matrix.m31;
        matrix.m32 = reader->snake_body->segments[segment_index].position.z + matrix.m32;
        special_index = segment_index == reader->snake_body->segment_count - 1 ? 2 : segment_index % 2;
        if (NuSpecialExistsFn(&snake_hspecials[special_index])) {
            NuSpecialDrawAt(&snake_hspecials[special_index], &matrix);
        }
    }
}
