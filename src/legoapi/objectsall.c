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
