#include "saga.h"

void __cdecl Buck_Start(GAMEOBJECT *object, GAMEOBJECT *rider) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (LEGOCONTEXT_BUCK == -1 || LEGOACT_BUCK == -1)
        return;
    model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    if (model->model_data_b[LEGOACT_BUCK] == NULL)
        return;
    if (rider != NULL)
        NewRumble(rider->pad->bound_pad, 0.6f, 0);
    object->character_context = (i8)LEGOCONTEXT_BUCK;
    object->field_7aa = LEGOACT_BUCK;
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
    rider->airborne_collision_target = mount;
    rider->tag_flags |= 1;
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

/* Do not split a "~~" pair, and keep a single '~' with the text after it. */
#define KEEP_TILDE_PAIR(text, index)                                                                                   \
    if ((text)[index] == '~') {                                                                                        \
        if ((index) > 0 && (text)[(index) - 1] == '~')                                                                 \
            --(index);                                                                                                 \
    } else if ((index) > 0 && (text)[(index) - 1] == '~' && (index) > 1 && (text)[(index) - 2] != '~')                 \
    --(index)

#define IS_BREAK_POINT(text, index)                                                                                    \
    (((text)[index] == ' ' && (text)[(index) + 1] != '!' && (text)[(index) + 1] != '?' &&                              \
      (text)[(index) + 1] != ';' && (text)[(index) + 1] != ':') ||                                                     \
     ((text)[index] == '.' && (text)[(index) + 1] != '.') || (text)[index] == '-' || (text)[index] == ',')

i32 __cdecl FindNextBreak(u8 *text, i32 index) {
    i32 japanese = 0, ascii_start, length, count;
    if (NuLanguageGet() == 0)
        japanese = 1;
    count = 0;
    length = NuStrLen((char *)text);
    while (text[index] >= 0x80 && text[index] <= 0xbf)
        --index;
    if (IS_BREAK_POINT(text, index))
        return index;
    ascii_start = text[index] <= 0x7f;
    for (;;) {
        ++count;
        if ((japanese && count > 10 && !ascii_start) || count > 20)
            break;
        if (++index >= length)
            return length;
        while (text[index] >= 0x80 && text[index] <= 0xbf)
            ++index;
        if (IS_BREAK_POINT(text, index))
            return index;
        if (japanese && ((text[index] <= 0x7f && !ascii_start) || (text[index] >= 0x80 && ascii_start)))
            break;
    }
    KEEP_TILDE_PAIR(text, index);
    return index;
}

i32 __cdecl FindNearestBreak(u8 *text, i32 index) {
    i32 japanese = 0, ascii_start, length, forward, backward, count;
    if (NuLanguageGet() == 0)
        japanese = 1;
    while (text[index] >= 0x80 && text[index] <= 0xbf)
        --index;
    if (IS_BREAK_POINT(text, index))
        return index;
    ascii_start = text[index] <= 0x7f;
    forward = backward = index;
    count = 0;
    length = NuStrLen((char *)text);
    for (;;) {
        ++count;
        if ((japanese && count > 10 && !ascii_start) || count > 20)
            break;
        if (forward < length - 1)
            ++forward;
        while (text[forward] >= 0x80 && text[forward] <= 0xbf)
            ++forward;
        if (backward > 0)
            --backward;
        while (text[backward] >= 0x80 && text[backward] <= 0xbf)
            --backward;
        if (IS_BREAK_POINT(text, forward))
            return forward;
        if (japanese && ((text[forward] <= 0x7f && !ascii_start) || (text[forward] >= 0x80 && ascii_start))) {
            KEEP_TILDE_PAIR(text, forward);
            return forward;
        }
        if (IS_BREAK_POINT(text, backward))
            return backward;
        if (japanese && ((text[backward] <= 0x7f && !ascii_start) || (text[backward] >= 0x80 && ascii_start))) {
            ++backward;
            KEEP_TILDE_PAIR(text, backward);
            return backward;
        }
    }
    KEEP_TILDE_PAIR(text, index);
    return index;
}

typedef struct EQUIVALENT_GROUP_PC {
    i16 object_count;
    i16 byte_size;
    NUSPECIALHANDLE objects[1];
} EQUIVALENT_GROUP_PC;
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, object_count) == 0);
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, byte_size) == 2);
STATIC_ASSERT(offsetof(EQUIVALENT_GROUP_PC, objects) == 4);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_group_count) == 0x46b4);

void __cdecl EquivalentObjects_Configure(WORLDINFO *world, char *config) {
    NUFPAR_PC *parser;
    EQUIVALENT_GROUP_PC *cursor;
    world->equivalent_groups = NULL;
    world->equivalent_group_count = 0;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("equivalentobjects", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    cursor = (EQUIVALENT_GROUP_PC *)world->giz_buffer.addr;
    world->equivalent_groups = cursor;
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
            cursor = (EQUIVALENT_GROUP_PC *)((u8 *)cursor + cursor->byte_size);
            ++world->equivalent_group_count;
        }
    }
    NuFParDestroy(parser);
    if (world->equivalent_group_count > 0)
        world->giz_buffer.addr = ((u32)cursor + 15U) & ~15U;
    else
        world->equivalent_groups = NULL;
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
extern i32 PushBlockHintIdPc;
void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
char *__cdecl NuIToA(i32, char *, i32);
extern char *PushBlocksNamePc;

STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
