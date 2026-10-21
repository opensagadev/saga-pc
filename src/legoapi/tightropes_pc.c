#include "saga.h"

typedef struct tightrope_progress_recovered_pc_s {
    u32 active_mask;
    u32 visible_mask;
} TIGHTROPE_PROGRESS_PC;
STATIC_ASSERT(sizeof(TIGHTROPE_s) == 0x48);
STATIC_ASSERT(offsetof(TIGHTROPE_s, start_position) == 0x10);
STATIC_ASSERT(offsetof(TIGHTROPE_s, end_position) == 0x1c);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x28) == 0x28);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x2a) == 0x2a);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x2c) == 0x2c);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x2e) == 0x2e);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x30) == 0x30);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x31) == 0x31);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x32) == 0x32);
STATIC_ASSERT(offsetof(TIGHTROPE_s, direction) == 0x34);
STATIC_ASSERT(offsetof(TIGHTROPE_s, length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_s, y_rotation) == 0x44);
STATIC_ASSERT(offsetof(TIGHTROPE_s, visible) == 0x46);
STATIC_ASSERT(offsetof(TIGHTROPE_s, active) == 0x47);
STATIC_ASSERT(offsetof(WORLDINFO, tightropes) == 0x5058);
STATIC_ASSERT(offsetof(WORLDINFO, tightrope_count) == 0x505c);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_tightropes) == 0xe8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
STATIC_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(sizeof(NUMTX) == 0x40);
STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC) == 0x24);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, colour) == 0x18);
extern char AIRegistryTextPc007575bc[7];
extern char TextInputKeyNameTextPc7516c0[1];

void *__cdecl TightRopes_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->tightropes = NULL;
    world->tightrope_count = 0;
    if (level->max_tightropes != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->tightropes = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_tightropes * sizeof(TIGHTROPE_s);
    }
    return world->tightropes;
}

i32 __cdecl TightRopes_Load(WORLDINFO *world, void *unused) {
    i32 version, index;
    if (world->tightrope_count != 0)
        return 0;
    version = EdFileReadInt();
    world->tightrope_count = EdFileReadInt();
    for (index = 0; index < world->tightrope_count; ++index) {
        EdFileRead(world->tightropes[index].name, 16);
        EdFileReadNuVec(&world->tightropes[index].start_position);
        EdFileReadNuVec(&world->tightropes[index].end_position);
        if (version >= 2) {
            u16 angle = EdFileReadUnsignedShort();
            world->tightropes[index].field_0x28 = angle;
            angle = EdFileReadUnsignedShort();
            world->tightropes[index].field_0x2a = angle;
            angle = EdFileReadUnsignedShort();
            world->tightropes[index].field_0x2c = angle;
            angle = EdFileReadUnsignedShort();
            world->tightropes[index].field_0x2e = angle;
            {
                u8 endpoint = EdFileReadUnsignedChar();
                world->tightropes[index].field_0x30 = endpoint;
                endpoint = EdFileReadUnsignedChar();
                world->tightropes[index].field_0x31 = endpoint;
            }
            if (version >= 3) {
                i8 show = EdFileReadChar();
                world->tightropes[index].field_0x32 = show;
            }
        }
    }
    return 1;
}

static void TightRopeInitRecordPc(TIGHTROPE_s *rope) {
    f32 inverse;
    NuVecSub(&rope->direction, &rope->end_position, &rope->start_position);
    rope->y_rotation = NuAtan2D(rope->direction.x, rope->direction.z);
    rope->length = NuVecMag(&rope->direction);
    inverse = 1.0f / rope->length;
    NuVecScale(&rope->direction, &rope->direction, inverse);
    rope->visible = 1;
    rope->active = 1;
}

void __cdecl TightRopes_Reset(WORLDINFO *world, void *unused, TIGHTROPE_PROGRESS_PC *progress) {
    TIGHTROPE_s *rope;
    i32 index;
    if (world == NULL || world->tightropes == NULL)
        return;
    rope = world->tightropes;
    for (index = 0; index < world->tightrope_count; ++index, ++rope) {
        TightRopeInitRecordPc(rope);
        if (progress != NULL && index < 32) {
            u32 mask = 1u << index;
            rope->visible = (progress->visible_mask & mask) != 0;
            rope->active = (progress->active_mask & mask) != 0;
        }
    }
}

void __cdecl TightRopes_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    TIGHTROPE_s *rope = world->tightropes;
    NUMTX matrix;
    NUVEC *positions[64];
    f32 distances[64];
    NURND_VERTEX3D_PC vertices[66];
    i32 index, endpoint;
    if (rope == NULL)
        return;
    for (index = 0; index < 66; ++index)
        vertices[index].colour = 0xffffffffu;
    for (index = 0; index < world->tightrope_count; ++index, ++rope) {
        i32 count, object_index, pass, point;
        GAMEOBJECT *object;
        for (endpoint = 0; endpoint < 2; ++endpoint) {
            if (rope->visible != 0 || (rope->field_0x32 != 0 && endpoint < 1)) {
                i32 model;
                u16 angle_x;
                NUVEC *position;
                if (endpoint == 0) {
                    model = rope->field_0x30 != 0 ? 64 : 63;
                    angle_x = rope->field_0x28;
                    position = &rope->start_position;
                } else {
                    model = rope->field_0x31 != 0 ? 64 : 63;
                    angle_x = rope->field_0x2a;
                    position = &rope->end_position;
                }
                if (WORLD->level_objects[model].active != 0) {
                    NuMtxSetRotationX(&matrix, angle_x);
                    NuMtxRotateY(&matrix, endpoint == 0 ? rope->field_0x2c : rope->field_0x2e);
                    NuMtxTranslate(&matrix, position);
                    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
                }
            }
        }
        if (rope->active == 0 || rope->visible == 0)
            continue;
        count = 0;
        object = Obj;
        for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
            if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->character_context != 0x44 ||
                object->field_7aa == 6 || object->field_7aa == 5 || object->grapple != rope)
                continue;
            positions[count] = (object->character_data->game_character->flags_090 & 0x80000) != 0
                                   ? &object->lower_position
                                   : &object->upper_position;
            distances[count] = NuVecDist(positions[count], &rope->start_position, NULL);
            ++count;
            if (count == 64)
                break;
        }
        for (pass = 0; pass < count - 1; ++pass) {
            for (point = 0; point < count - 1; ++point) {
                if (distances[point] > distances[point + 1]) {
                    NUVEC *position = positions[point];
                    f32 distance = distances[point];
                    positions[point] = positions[point + 1];
                    positions[point + 1] = position;
                    distances[point] = distances[point + 1];
                    distances[point + 1] = distance;
                }
            }
        }
        vertices[0].position = rope->start_position;
        for (point = 0; point < count; ++point)
            vertices[point + 1].position = *positions[point];
        vertices[count + 1].position = rope->end_position;
        for (point = 0; point <= count; ++point)
            NuRndrLine3d(&vertices[point], ZipUpLineMaterialPc, NULL);
    }
}

void __cdecl TightRope_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL)
        ((TIGHTROPE_s *)gizmo->object)->active = active != 0;
}
void __cdecl TightRope_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL)
        ((TIGHTROPE_s *)gizmo->object)->visible = visible != 0;
}
i32 __cdecl TightRopes_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_tightropes : 0;
}
char *__cdecl TightRope_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}
i32 __cdecl TightRope_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    TIGHTROPE_s *rope = (TIGHTROPE_s *)gizmo->object;
    return rope->visible != 0 && rope->active != 0;
}
char *__cdecl TightRope_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return AIRegistryTextPc007575bc;
}
i32 __cdecl TightRope_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
void *__cdecl TightRopes_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(TIGHTROPE_PROGRESS_PC));
}
void __cdecl TightRopes_ClearProgress(void *world, TIGHTROPE_PROGRESS_PC *progress) {
    if (progress != NULL) {
        progress->active_mask = 0xffffffffu;
        progress->visible_mask = 0xffffffffu;
    }
}
void __cdecl TightRopes_StoreProgress(WORLDINFO *world, void *unused, TIGHTROPE_PROGRESS_PC *progress) {
    TIGHTROPE_s *rope;
    i32 index;
    if (progress == NULL)
        return;
    progress->active_mask = 0xffffffffu;
    progress->visible_mask = 0xffffffffu;
    if (world == NULL || world->tightropes == NULL)
        return;
    rope = world->tightropes;
    for (index = 0; index < world->tightrope_count && index < 32; ++index, ++rope) {
        u32 mask = 1u << index;
        if (rope->visible == 0)
            progress->visible_mask &= ~mask;
        if (rope->active == 0)
            progress->active_mask &= ~mask;
    }
}
void __cdecl TightRopes_AddGizmos(GIZMOSYS_PC *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->tightrope_count; ++index) {
        if (NuStrLen(world->tightropes[index].name) != 0)
            AddGizmo(system, type_id, NULL, &world->tightropes[index]);
    }
}

#pragma const_seg(".rdata$PCContextName_TightRope")
const char PCContextName_TightRope[10] = "TightRope";
#pragma const_seg()
#pragma data_seg(".data$TightRopeRegistryNamePc")
char *TightRopeRegistryNamePc = (char *)PCContextName_TightRope;
#pragma data_seg()
#pragma data_seg(".data$TightRopeAddTypePc")
ADDGIZMOTYPE_PC TightRopeAddTypePc = {0};
#pragma data_seg()

ADDGIZMOTYPE_PC *__cdecl TightRopes_RegisterGizmo(i32 unused_type_id) {
    char *name = TightRopeRegistryNamePc;
    TightRopeAddTypePc = Default_ADDGIZMOTYPE;
    TightRopeAddTypePc.name = name;
    TightRopeAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    TightRopeAddTypePc.fns.options = 8;
    TightRopeAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))TightRopes_GetMaxGizmos;
    TightRopeAddTypePc.fns.add_gizmos_fn = (void *)TightRopes_AddGizmos;
    TightRopeAddTypePc.fns.early_update_fn = NULL;
    TightRopeAddTypePc.fns.late_update_fn = NULL;
    TightRopeAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))TightRopes_Draw;
    TightRopeAddTypePc.fns.panel_draw_fn = NULL;
    TightRopeAddTypePc.fns.get_gizmo_name_fn = TightRope_GetGizmoName;
    TightRopeAddTypePc.fns.get_output_fn = TightRope_GetOutput;
    TightRopeAddTypePc.fns.get_output_name_fn = TightRope_GetOutputName;
    TightRopeAddTypePc.fns.get_num_outputs_fn = (void *)TightRope_GetNumOutputs;
    TightRopeAddTypePc.fns.activate_fn = TightRope_Activate;
    TightRopeAddTypePc.fns.activate_rev_fn = NULL;
    TightRopeAddTypePc.fns.set_visibility_fn = TightRope_SetVisibility;
    TightRopeAddTypePc.fns.get_visibility_fn = NULL;
    TightRopeAddTypePc.fns.get_pos_fn = NULL;
    TightRopeAddTypePc.fns.using_special_fn = NULL;
    TightRopeAddTypePc.fns.bolt_hit_plat_fn = NULL;
    TightRopeAddTypePc.fns.get_best_bolt_target_fn = NULL;
    TightRopeAddTypePc.fns.bolt_hit_fn = NULL;
    TightRopeAddTypePc.fns.allocate_progress_data_fn = TightRopes_AllocateProgressData;
    TightRopeAddTypePc.fns.clear_progress_fn = (void(__cdecl *)(void *, void *))TightRopes_ClearProgress;
    TightRopeAddTypePc.fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))TightRopes_StoreProgress;
    TightRopeAddTypePc.fns.reset_fn = (void(__cdecl *)(void *, void *, void *))TightRopes_Reset;
    TightRopeAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))TightRopes_ReserveBufferSpace;
    TightRopeAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))TightRopes_Load;
    TightRopeAddTypePc.fns.post_load_fn = NULL;
    TightRopeAddTypePc.fns.add_level_sfx_fn = NULL;
    return &TightRopeAddTypePc;
}
