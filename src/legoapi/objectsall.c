#include "saga.h"

void __cdecl Buck_Start(GAMEOBJECT *object, GAMEOBJECT *rider) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (LEGOCONTEXT_BUCK == -1 || LEGOACT_BUCK == -1)
        return;
    model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    if (model->model_data_b[LEGOACT_BUCK] == NULL)
        return;
    if (rider != NULL)
        STUBBED_NAMED("Buck_Start: NewRumble PC54a850 rider pad, 0.6f, 0");
    object->field_7aa = LEGOACT_BUCK;
    object->character_context = (i8)LEGOCONTEXT_BUCK;
    object->context_timer = AnimDuration((i16)object->id, LEGOACT_BUCK, 0.0f, 0.0f, 1);
    if (BuckStartExtraFn != NULL)
        BuckStartExtraFn(object);
}

void __cdecl Buck_MoveCode(GAMEOBJECT *object, i32 start) {
    if (LEGOCONTEXT_BUCK != -1 && object->character_context == LEGOCONTEXT_BUCK) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
    } else if (object->character_context == -1 && start != 0) {
        Buck_Start(object, object->takeover_source);
    }
}

void __cdecl Buck_StartRiderJump(GAMEOBJECT *rider, GAMEOBJECT *mount) {
    u16 angle = (u16)qrand();
    f32 speed = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.09f + (f64)0.01f);
    f32 clearance;
    PLAYERCHARACTERCONFIG_PC *character;
    rider->velocity.x = NuTrigTable[angle >> 1] * speed;
    rider->velocity.z = speed * NuTrigTable[(((i32)angle + 0x4000) >> 1) & 0x7fff];
    StartJump(rider, 0);
    rider->tag_flags |= 1;
    rider->airborne_collision_target = mount;
    rider->velocity.x = 0.0f;
    rider->velocity.z = 0.0f;
    rider->pad->buttons_held = 0;
    rider->pad->buttons_pressed = 0;
    clearance = mount->upper_position.y;
    if ((mount->character_data->game_character->flags_094 & 0x20000) != 0)
        clearance += BUCK_RIDERJUMPCLEARANCE;
    clearance -= rider->lower_position.y;
    clearance = (f32)((f64)clearance + (f64)0.1f);
    character = rider->character_data->game_character;
    if (clearance > character->jump_height)
        rider->velocity.y = NuFsqrt((f32)(-2.0 * (f64)character->gravity * clearance));
}

i32 __cdecl FindNextBreak(u8 *text, i32 index) {
    i32 japanese = NuLanguageGet() == 0, count = 0, ascii_start, length;
    u8 ch, next;
    length = NuStrLen((char *)text);
    while ((u8)(text[index] - 0x80) <= 0x3f)
        --index;
    ch = text[index];
    next = text[index + 1];
    if ((ch == ' ' && next != '!' && next != '?' && next != ';' && next != ':') || (ch == '.' && next != '.') ||
        ch == '-' || ch == ',')
        return index;
    ascii_start = ch <= 0x7f;
    for (;;) {
        ++count;
        if ((japanese && count > 10 && !ascii_start) || count > 20)
            break;
        if (++index >= length)
            return length;
        while ((u8)(text[index] - 0x80) <= 0x3f)
            ++index;
        ch = text[index];
        next = text[index + 1];
        if ((ch == ' ' && next != '!' && next != '?' && next != ';' && next != ':') || (ch == '.' && next != '.') ||
            ch == '-' || ch == ',')
            return index;
        if (japanese && ((ch <= 0x7f && !ascii_start) || (ch >= 0x80 && ascii_start)))
            break;
    }
    if (text[index] == '~') {
        if (index > 0 && text[index - 1] == '~')
            --index;
    } else if (index > 1 && text[index - 1] == '~' && text[index - 2] != '~')
        --index;
    return index;
}
i32 __cdecl FindNearestBreak(u8 *text, i32 index) {
    i32 japanese = NuLanguageGet() == 0, ascii_start, length, forward, backward, count = 0, result;
    u8 ch, next;
    while ((u8)(text[index] - 0x80) <= 0x3f)
        --index;
    ch = text[index];
    next = text[index + 1];
    if ((ch == ' ' && next != '!' && next != '?' && next != ';' && next != ':') || (ch == '.' && next != '.') ||
        ch == '-' || ch == ',')
        return index;
    ascii_start = ch <= 0x7f;
    length = NuStrLen((char *)text);
    forward = backward = index;
    for (;;) {
        ++count;
        if ((japanese && count > 10 && !ascii_start) || count > 20) {
            result = index;
            break;
        }
        if (forward < length - 1)
            ++forward;
        while ((u8)(text[forward] - 0x80) <= 0x3f)
            ++forward;
        if (backward > 0)
            --backward;
        while ((u8)(text[backward] - 0x80) <= 0x3f)
            --backward;
        ch = text[forward];
        next = text[forward + 1];
        if ((ch == ' ' && next != '!' && next != '?' && next != ';' && next != ':') || (ch == '.' && next != '.') ||
            ch == '-' || ch == ',')
            return forward;
        if (japanese && ((ch <= 0x7f && !ascii_start) || (ch >= 0x80 && ascii_start))) {
            result = forward;
            break;
        }
        ch = text[backward];
        next = text[backward + 1];
        if ((ch == ' ' && next != '!' && next != '?' && next != ';' && next != ':') || (ch == '.' && next != '.') ||
            ch == '-' || ch == ',')
            return backward;
        if (japanese && ((ch <= 0x7f && !ascii_start) || (ch >= 0x80 && ascii_start))) {
            result = backward + 1;
            break;
        }
    }
    if (text[result] == '~') {
        if (result > 0 && text[result - 1] == '~')
            --result;
    } else if (result > 1 && text[result - 1] == '~' && text[result - 2] != '~')
        --result;
    return result;
}

typedef struct EQUIVALENT_GROUP_PC {
    i16 object_count;
    i16 byte_size;
    NUSPECIALHANDLE objects[1];
} EQUIVALENT_GROUP_PC;
typedef struct EQUIVALENT_WORLD_VIEW_PC {
    u8 unknown_0[0x46b0];
    EQUIVALENT_GROUP_PC *equivalent_groups;
    i32 equivalent_group_count;
} EQUIVALENT_WORLD_VIEW_PC;
typedef struct CHAR_PLATFORM_PC {
    NUSPECIALHANDLE special;
    i16 object_id;
    i16 platform_id;
    GAMEOBJECT *object;
} CHAR_PLATFORM_PC;
typedef struct CHAR_PLATFORM_SYSTEM_PC {
    NUGSCN *scene;
    i32 platform_count;
    CHAR_PLATFORM_PC platforms[1];
} CHAR_PLATFORM_SYSTEM_PC;
typedef struct CHAR_PLATFORM_WORLD_PC {
    u8 unknown_0[0x2adc];
    CHAR_PLATFORM_SYSTEM_PC *char_platform_sys;
} CHAR_PLATFORM_WORLD_PC;
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, object_count) == 0);
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, byte_size) == 2);
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, objects) == 4);
STATIC_ASSERT(offsetof(EQUIVALENT_WORLD_VIEW_PC, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(EQUIVALENT_WORLD_VIEW_PC, equivalent_group_count) == 0x46b4);
STATIC_ASSERT(sizeof(CHAR_PLATFORM_PC) == 0x14);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, special) == 0);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, object_id) == 0xc);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, platform_id) == 0xe);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_PC, object) == 0x10);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_SYSTEM_PC, scene) == 0);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_SYSTEM_PC, platform_count) == 4);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_SYSTEM_PC, platforms) == 8);
STATIC_ASSERT(offsetof(CHAR_PLATFORM_WORLD_PC, char_platform_sys) == 0x2adc);

void __cdecl EquivalentObjects_Configure(WORLDINFO *world, char *config) {
    EQUIVALENT_WORLD_VIEW_PC *view = (EQUIVALENT_WORLD_VIEW_PC *)world;
    NUFPAR_PC *parser;
    EQUIVALENT_GROUP_PC *cursor;
    view->equivalent_groups = NULL;
    view->equivalent_group_count = 0;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("equivalentobjects", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    cursor = (EQUIVALENT_GROUP_PC *)world->giz_buffer.addr;
    view->equivalent_groups = cursor;
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, "equivalentobjects") != 0)
            continue;
        cursor->object_count = 0;
        cursor->byte_size = 4;
        while (NuFParGetWord(parser) != 0) {
            if (NuSpecialFind(world->current_gscn, &cursor->objects[cursor->object_count], parser->word_buffer_ptr,
                              1) != 0) {
                ++cursor->object_count;
                cursor->byte_size += sizeof(NUSPECIALHANDLE);
            }
        }
        if (cursor->object_count > 0) {
            ++view->equivalent_group_count;
            cursor = (EQUIVALENT_GROUP_PC *)((u8 *)cursor + cursor->byte_size);
        }
    }
    NuFParDestroy(parser);
    if (view->equivalent_group_count > 0)
        world->giz_buffer.addr = ((u32)cursor + 15U) & ~15U;
    else
        view->equivalent_groups = NULL;
}

PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl UpdatePushBlocks(void *, void *, f32);
extern f32 PushVelocitySeekRatePc;
extern f32 PushFallingSpeedPc;
extern f32 PushCornerInsetPc;
extern f32 PushGroundOffsetPc;
extern f32 PushRumblePeriodPc;
extern f32 PushRumbleWindowPc;
extern f32 PushBlockOverlapShrinkPc;
extern i32 PushBlockHintIdPc;
void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
char *__cdecl NuIToA(i32, char *, i32);
extern char *PushBlocksNamePc;

typedef struct PUSH_RUNTIME_BLOCK_VIEW_PC {
    f32 ground_offset, ground_height, snap_timer, settled_height;
    i8 terrain_info[4];
    union {
        i8 bytes[4];
        i32 word;
    } extra_terrain_info;
    i32 previous_extra_terrain_info;
    PUSH_BLOCK_PC *supporting_block, *previous_supporting_block;
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    i32 platform_id;
    u8 unknown_038[8];
    f32 snap_distance;
    char name[16];
    NUVEC *snap_positions, *position;
    NUSPECIALHANDLE end_position_specials[2];
    NUVEC end_position_origins[2];
    NUVEC velocity, target_velocity, snap_origin, bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions, end_position_count, output_count, unknown_0d3;
} PUSH_RUNTIME_BLOCK_VIEW_PC;
typedef struct PUSH_RUNTIME_LEVEL_VIEW_PC {
    u8 unknown_000[0xff];
    u8 max_push_blocks;
} PUSH_RUNTIME_LEVEL_VIEW_PC;
typedef struct PUSH_RUNTIME_WORLD_VIEW_PC {
    u8 unknown_000[0x104];
    VARIPTR giz_buffer;
    u8 unknown_108[0x128 - 0x108];
    PUSH_RUNTIME_LEVEL_VIEW_PC *current_level;
    u8 unknown_12c[0x13c - 0x12c];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2ac8 - 0x140];
    void *gizmo_sys;
    u8 unknown_2acc[0x46bc - 0x2acc];
    PUSH_RUNTIME_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_RUNTIME_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_RUNTIME_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_LEVEL_VIEW_PC, max_push_blocks) == 0xff);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, count) == 0x46c0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_offset) == 0x0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_height) == 0x4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_timer) == 0x8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, settled_height) == 0xc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, terrain_info) == 0x10);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, extra_terrain_info) == 0x14);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_extra_terrain_info) == 0x18);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, supporting_block) == 0x1c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_supporting_block) == 0x20);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, platform_id) == 0x34);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_distance) == 0x40);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, name) == 0x44);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_positions) == 0x54);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_specials) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_origins) == 0x74);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, velocity) == 0x8c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, target_velocity) == 0x98);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_origin) == 0xa4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);

PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, i32 excluded,
                                    PUSH_BLOCK_PC **support) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = (PUSH_RUNTIME_WORLD_VIEW_PC *)opaque_world;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = (PUSH_RUNTIME_BLOCK_VIEW_PC *)opaque_block;
    NUVEC *position = block->position;
    i32 i;
    if (support != NULL)
        *support = NULL;
    for (i = 0; i < world->count; ++i) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *other;
        f32 x, y, z;
        NUVEC minimum, maximum;
        i32 overlap_x, overlap_y, overlap_z;
        f32 shrink, epsilon;
        if (i == excluded)
            continue;
        other = &world->blocks[i];
        if ((other->packed_state_flags & 0x02000000u) != 0)
            continue;
        if (NuSpecialGetVisibilityFn(&other->special) == 0)
            continue;
        x = position->x - other->position->x;
        y = position->y - other->position->y;
        z = position->z - other->position->z;
        minimum.x = other->bounds_min.x - block->bounds_max.x;
        minimum.y = other->bounds_min.y - block->bounds_max.y;
        minimum.z = other->bounds_min.z - block->bounds_max.z;
        maximum.x = other->bounds_max.x - block->bounds_min.x;
        maximum.y = other->bounds_max.y - block->bounds_min.y;
        maximum.z = other->bounds_max.z - block->bounds_min.z;
        shrink = PushBlockOverlapShrinkPc;
        if (shrink > 0.0f) {
            minimum.x += shrink;
            minimum.y += shrink;
            minimum.z += shrink;
            maximum.x -= shrink;
            maximum.y -= shrink;
            maximum.z -= shrink;
        }
        epsilon = PushBlockGapEpsilonPc;
        overlap_x = x > minimum.x + epsilon && x < maximum.x - epsilon;
        overlap_y = y > minimum.y && y < maximum.y;
        overlap_z = z > minimum.z + epsilon && z < maximum.z - epsilon;
        if (support != NULL && overlap_x && overlap_z) {
            f32 gap = y - maximum.y;
            if (gap > 0.0f) {
                if (*support == NULL) {
                    *support = (PUSH_BLOCK_PC *)other;
                } else {
                    PUSH_RUNTIME_BLOCK_VIEW_PC *previous = (PUSH_RUNTIME_BLOCK_VIEW_PC *)*support;
                    f32 previous_y = position->y - previous->position->y;
                    f32 previous_maximum = previous->bounds_max.y - block->bounds_min.y;
                    f32 previous_gap;
                    if (shrink > 0.0f)
                        previous_maximum -= shrink;
                    previous_gap = previous_y - previous_maximum;
                    if (previous_gap > gap) {
                        other->supporting_block = (PUSH_BLOCK_PC *)previous;
                        *support = (PUSH_BLOCK_PC *)other;
                    }
                }
            }
        }
        if (overlap_x && overlap_y && overlap_z)
            return (PUSH_BLOCK_PC *)other;
    }
    return NULL;
}
