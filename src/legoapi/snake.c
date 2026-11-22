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

static nuhspecial_s snake_hspecials[3] = {0};
SNAKE_BODY snakebodies[4] = {0};

void InitSnakes(WORLDINFO *world) {
    memset(snakebodies, 0, sizeof(snakebodies));
    NuSpecialFind(world->current_gscn, &snake_hspecials[0], "Snake_bit_1", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[1], "Snake_bit_2", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[2], "Snake_bit_3", 1);
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static void SnakeMtxSetRotationXY(NUMTX *matrix, NUANG x, NUANG y) {
    NUANGVEC angles;
    angles.x = x;
    angles.y = y;
    NuMtxSetRotationXYVU0(matrix, &angles);
}

static f32 SnakeFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

/* Unit-local copy of the NuASin polynomial (the snake unit carries its own instance). */
i32 SnakeASin(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 magnitude;
    f32 range_sign;
    f32 value_sign;
    f32 octant;

    abs = SnakeFabs(sin);
    sqrt = NuFsqrt(1.0f - sin * sin);

    magnitude = MIN(sqrt, abs);

    range_sign = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    value_sign = MIN(sin * 3.40282e+38f, 1.0f);
    value_sign = -1.0f < value_sign ? value_sign : -1.0f;

    octant = range_sign * value_sign + value_sign;

    return (NEG_3_OVER_40 * POW2(range_sign * value_sign * magnitude) * POW3(range_sign * value_sign * magnitude) +
            (octant * PI_OVER_4 - (range_sign * value_sign * magnitude) +
             NEG_1_OVER_6 * (range_sign * value_sign * magnitude) * POW2(range_sign * value_sign * magnitude)) +
            NEG_5_OVER_112 * POW3(range_sign * value_sign * magnitude) * POW4(range_sign * value_sign * magnitude) +
            NEG_35_OVER_1152 * POW4(range_sign * value_sign * magnitude) * POW5(range_sign * value_sign * magnitude)) *
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
        body = &snakebodies[slot];
        if ((body->flags & 1) == 0) {
            object->snake_body = body;
            body->scale = SnakeBodyDefaultScale;
            body->flags |= 1;
            body->segment_count = (u16)segment_count;
            for (index = 0; index < body->segment_count; index++) {
                body->segments[index].yaw = object->movement_angle;
                body->segments[index].pitch = 0;
                body->segments[index].ground_height = 1000000000.0f;
            }
            return body;
        }
    }
    return NULL;
}

void UpdateSnakeBody(GAMEOBJECT *object) {
    NUVEC position;
    NUVEC offset;
    i32 index;
    f32 ground;
    f32 length;
    if (object == NULL || object->snake_body == NULL) {
        return;
    }
    object->collision_scale = object->snake_body->scale;
    position = object->lower_position;
    position.y += object->snake_body->scale * SnakeHeadHeight;
    for (index = 0; index < object->snake_body->segment_count; ++index) {
        object->snake_body->segments[index].position = position;
        object->snake_body->segments[index].yaw =
            SeekRot((u16)object->snake_body->segments[index].yaw,
                    index != 0 ? object->snake_body->segments[index - 1].yaw : object->movement_angle, SnakeYawSeekRate);
        object->snake_body->segments[index].pitch = 0;
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = -(object->snake_body->scale * SnakeSegmentSpacing);
        NuVecRotateY(&offset, &offset, object->snake_body->segments[index].yaw);
        NuVecAdd(&position, &object->snake_body->segments[index].position, &offset);
        ground = GameShadow(NULL, &position, 5.0f, 0x1f);
        if (ground == 2000000.0f) {
            ground = object->snake_body->segments[index].position.y;
        }
        if (object->snake_body->segments[index].ground_height == 1000000000.0f) {
            object->snake_body->segments[index].ground_height = ground;
        } else {
            object->snake_body->segments[index].ground_height = SeekValF(object->snake_body->segments[index].ground_height, ground, 10.0f);
        }
        if (object->snake_body->segments[index].ground_height != 2000000.0f) {
            position.y = object->snake_body->segments[index].ground_height;
            length = NuVecDist(&position, &object->snake_body->segments[index].position, &offset);
            NuVecScale(&offset, &offset, object->snake_body->scale * SnakeSegmentSpacing / length);
            NuVecAdd(&position, &object->snake_body->segments[index].position, &offset);
            object->snake_body->segments[index].pitch = (i16)SnakeASin(-offset.y / (object->snake_body->scale * SnakeSegmentSpacing));
        }
    }
}

void SnakeBeenHit(GAMEOBJECT *object) {
    i32 i;
    i32 index;
    i32 special_index;
    NUMTX matrix;
    NUVEC scale;
    NUVEC momentum;
    ADDPART params;
    nuhspecial_s *special;
    if (object == NULL || object->snake_body == NULL || object->snake_body->segment_count <= 2) {
        return;
    }
    for (i = 0; i < 2; ++i) {
        index = object->snake_body->segment_count - i - 1;
        SnakeMtxSetRotationXY(&matrix, object->snake_body->segments[index].pitch,
                              NuAngAdd(object->snake_body->segments[index].yaw, 0x8000));
        if (object->snake_body->scale != 1.0f) {
            scale.x = scale.y = scale.z = object->snake_body->scale;
            NuMtxPreScale(&matrix, &scale);
        }
        matrix.m30 += object->snake_body->segments[index].position.x;
        matrix.m31 += object->snake_body->segments[index].position.y + object->snake_body->scale * SnakeSegmentHeight;
        matrix.m32 += object->snake_body->segments[index].position.z;
        special_index = index == object->snake_body->segment_count - 1 ? 2 : index % 2;
        special = &snake_hspecials[special_index];
        if (NuSpecialExistsFn(special)) {
            SetKillPartMom(&momentum);
            momentum.y += SnakeDebrisUpwardSpeed;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.start_radius = 0.1f;
            params.end_radius = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.time_step = FRAMETIME;
            params.lighting_data = &object->lighting_packet;
            params.flags = PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_ROTATE_RANDOMLY;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            AddPart(&params);
        }
    }
    object->snake_body->segment_count -= 2;
}

void DestroySnakeBody(GAMEOBJECT *object) {
    if (object != NULL && object->snake_body != NULL) {
        memset(object->snake_body, 0, sizeof(*object->snake_body));
        object->snake_body = NULL;
    }
}

void BlowUpSnakeBody(GAMEOBJECT *object) {
    i32 index;
    i32 special_index;
    NUMTX matrix;
    NUVEC scale;
    NUVEC momentum;
    ADDPART params;
    nuhspecial_s *special;
    if (object == NULL || object->snake_body == NULL) {
        return;
    }
    for (index = 0; index < object->snake_body->segment_count; ++index) {
        SnakeMtxSetRotationXY(&matrix, object->snake_body->segments[index].pitch,
                              NuAngAdd(object->snake_body->segments[index].yaw, 0x8000));
        if (object->snake_body->scale != 1.0f) {
            scale.x = scale.y = scale.z = object->snake_body->scale;
            NuMtxPreScale(&matrix, &scale);
        }
        matrix.m30 = matrix.m30 + object->snake_body->segments[index].position.x;
        matrix.m31 = (object->snake_body->scale * SnakeSegmentHeight) + object->snake_body->segments[index].position.y + matrix.m31;
        matrix.m32 = object->snake_body->segments[index].position.z + matrix.m32;
        special_index = index == object->snake_body->segment_count - 1 ? 2 : index % 2;
        special = &snake_hspecials[special_index];
        if (NuSpecialExistsFn(special)) {
            SetKillPartMom(&momentum);
            momentum.y += SnakeDebrisUpwardSpeed;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.start_radius = 0.1f;
            params.end_radius = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.time_step = FRAMETIME;
            params.lighting_data = &object->lighting_packet;
            params.flags = PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_ROTATE_RANDOMLY;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            AddPart(&params);
        }
    }
    if (object->snake_body != NULL) {
        memset(object->snake_body, 0, sizeof(*object->snake_body));
        object->snake_body = NULL;
    }
}

void DrawSnakeBody(GAMEOBJECT *object) {
    i32 segment_index;
    i32 special_index;
    NUVEC scale;
    NUMTX matrix;
    if (object == NULL || object->snake_body == NULL) {
        return;
    }
    for (segment_index = 0; segment_index < object->snake_body->segment_count; ++segment_index) {
        object->collision_scale = object->snake_body->scale;
        SnakeMtxSetRotationXY(&matrix, object->snake_body->segments[segment_index].pitch,
                              NuAngAdd(object->snake_body->segments[segment_index].yaw, 0x8000));
        if (object->snake_body->scale != 1.0f) {
            scale.z = object->snake_body->scale;
            scale.y = scale.z;
            scale.x = scale.y;
            NuMtxPreScale(&matrix, &scale);
        }
        matrix.m30 = object->snake_body->segments[segment_index].position.x + matrix.m30;
        matrix.m31 = object->snake_body->scale * SnakeSegmentHeight + object->snake_body->segments[segment_index].position.y + matrix.m31;
        matrix.m32 = object->snake_body->segments[segment_index].position.z + matrix.m32;
        special_index = segment_index == object->snake_body->segment_count - 1 ? 2 : segment_index % 2;
        if (NuSpecialExistsFn(&snake_hspecials[special_index])) {
            NuSpecialDrawAt(&snake_hspecials[special_index], &matrix);
        }
    }
}
