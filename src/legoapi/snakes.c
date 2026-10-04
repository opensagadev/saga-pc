#include "saga.h"

#include <string.h>

extern u8 snakebodies[0x440];
static NUSPECIALHANDLE snake_hspecials[3];

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
void __cdecl NuMtxSetRotationXYVU0(NUMTX *, NUANGVEC *);
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
