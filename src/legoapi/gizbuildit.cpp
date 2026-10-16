#include "saga.h"

typedef struct gizbuildit_record_reader_pc_s {
    char name[16];
    u8 unknown_10[0x6c];
} GIZBUILDIT_RECORD_READER_PC;
STATIC_ASSERT(sizeof(GIZBUILDIT_RECORD_READER_PC) == 0x7c);
STATIC_ASSERT(offsetof(GIZBUILDIT_RECORD_READER_PC, name) == 0);

typedef struct gizbuildit_system_reader_pc_s {
    GIZBUILDIT_RECORD_READER_PC *buildits;
    u16 count;
} GIZBUILDIT_SYSTEM_READER_PC;
STATIC_ASSERT(offsetof(GIZBUILDIT_SYSTEM_READER_PC, buildits) == 0);
STATIC_ASSERT(offsetof(GIZBUILDIT_SYSTEM_READER_PC, count) == 4);

typedef struct gizbuildit_world_reader_pc_s {
    u8 unknown_00[0x46a8];
    GIZBUILDIT_SYSTEM_READER_PC *system;
} GIZBUILDIT_WORLD_READER_PC;
STATIC_ASSERT(offsetof(GIZBUILDIT_WORLD_READER_PC, system) == 0x46a8);

extern "C" void *__cdecl GizBuildIt_Find(WORLDINFO *world, char *name) {
    GIZBUILDIT_WORLD_READER_PC *w = (GIZBUILDIT_WORLD_READER_PC *)world;
    GIZBUILDIT_SYSTEM_READER_PC *system = w->system;
    if (system != NULL && system->count != 0) {
        GIZBUILDIT_RECORD_READER_PC *buildit = system->buildits;
        for (i32 i = 0; i < w->system->count; ++i, ++buildit) {
            if (NuStrICmp(buildit->name, name) == 0)
                return buildit;
        }
    }
    return NULL;
}

extern "C" void GizBuildIt_SetHeadTarget(GIZBUILDIT_s *buildit_pointer, GAMEOBJECT *player) {
    GIZBUILDIT_PC *buildit = reinterpret_cast<GIZBUILDIT_PC *>(buildit_pointer);
    if (buildit->anim_object_count != 0) {
        i32 index = buildit->built_object_count;
        if (buildit->anim_object_count <= index)
            index = buildit->anim_object_count - 1;
        GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(buildit->anim_objects[index]->object_data);
        SetHeadTarget(player, reinterpret_cast<NUVEC *>(&data->start_mtx.m30), 0, 2.0f, 1.0f, 2.0f);
    }
}

typedef struct buildit_attracto_reader_pc_s {
    u8 unknown_00[0x18];
    void *linked_buildit;
    u8 unknown_1c[0x28];
    f32 step_timer, step_duration;
    u8 unknown_4c[0x30];
} BUILDIT_ATTRACTO_READER_PC;
STATIC_ASSERT(sizeof(BUILDIT_ATTRACTO_READER_PC) == 0x7c);
STATIC_ASSERT(offsetof(BUILDIT_ATTRACTO_READER_PC, linked_buildit) == 0x18);
STATIC_ASSERT(offsetof(BUILDIT_ATTRACTO_READER_PC, step_timer) == 0x44);
STATIC_ASSERT(offsetof(BUILDIT_ATTRACTO_READER_PC, step_duration) == 0x48);

extern "C" void GizMoveAttractoBuildItPiece(GIZBUILDIT_s *buildit_pointer, GAMEANIMOBJ *piece) {
    BUILDIT_ATTRACTO_READER_PC *buildit = (BUILDIT_ATTRACTO_READER_PC *)buildit_pointer;
    volatile f32 progress = 1.0 - (f64)buildit->step_timer / buildit->step_duration;
    GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(piece->object_data);
    NUMTX *draw = NuSpecialGetDrawMtx(&piece->special);
    if (progress > 0.0f) {
        NUMTX result;
        NUMTX from = *draw;
        from.m30 = from.m31 = from.m32 = 0.0f;
        NUMTX to = data->end_mtx;
        to.m30 = to.m31 = to.m32 = 0.0f;
        NUQUAT rotation, from_rotation, to_rotation;
        NuMtxToQuat(&from, &from_rotation);
        NuMtxToQuat(&to, &to_rotation);
        NuQuatSlerp(&rotation, &from_rotation, &to_rotation, progress);
        NuQuatToMtx(&rotation, &result);
        result.m30 = ((f64)data->end_mtx.m30 - draw->m30) * progress + draw->m30;
        result.m31 = ((f64)data->end_mtx.m31 - draw->m31) * progress + draw->m31 +
                     (f64)NU_SIN_LUT((f64)progress * 32768.0) * (f64)0.1f;
        result.m32 = ((f64)data->end_mtx.m32 - draw->m32) * progress + draw->m32;
        *draw = result;
    }
    if (buildit->linked_buildit != NULL)
        data->draw_mtx = *draw;
    else
        NuSpecialSetDrawMtx(&piece->special, draw);
}

STATIC_ASSERT(offsetof(GIZBUILDIT_PC, step_timer) == 0x44);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, step_duration) == 0x48);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, anim_object_count) == 0x6c);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, builders_active) == 0x6e);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, built_object_count) == 0x6f);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, end_mtx) == 0x40);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);

extern "C" void __cdecl GizDrawBuildItPiece(GAMEOBJECT *object, i32 draw_reflection) {
    GIZBUILDIT_PC *buildit = static_cast<GIZBUILDIT_PC *>(object->grapple);
    if (buildit == NULL || buildit->builders_active == 0 || buildit->build_state != 0 ||
        buildit->anim_object_count == 0)
        return;
    volatile f32 progress = 1.0 - (f64)buildit->step_timer / (f64)buildit->step_duration;
    GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
    GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(piece->object_data);
    NUMTX matrix = data->start_mtx;
    if (progress > 0.0f) {
        NUMTX from = matrix;
        NUMTX to = data->end_mtx;
        NUMTX result;
        NUQUAT from_rotation, to_rotation, rotation;
        from.m30 = from.m31 = from.m32 = 0.0f;
        to.m30 = to.m31 = to.m32 = 0.0f;
        NuMtxToQuat(&from, &from_rotation);
        NuMtxToQuat(&to, &to_rotation);
        NuQuatSlerp(&rotation, &from_rotation, &to_rotation, progress);
        NuQuatToMtx(&rotation, &result);
        result.m30 = ((f64)data->end_mtx.m30 - matrix.m30) * (f64)progress + matrix.m30;
        result.m31 = ((f64)data->end_mtx.m31 - matrix.m31) * (f64)progress + matrix.m31 +
                     (f64)NU_SIN_LUT((f64)progress * 32768.0) * (f64)0.1f;
        result.m32 = ((f64)data->end_mtx.m32 - matrix.m32) * (f64)progress + matrix.m32;
        matrix = result;
    }
    if (buildit->linked_buildit != NULL) {
        data->draw_mtx = matrix;
        return;
    }
    NuSpecialDrawAt(&piece->special, &matrix);
    if (draw_reflection != 0) {
        NUMTX reflected;
        if (MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                             WORLD->current_level->reflection_y, &reflected) != 0) {
            NuRndrStartReflectionRender(0);
            NuSpecialDrawAt(&piece->special, &reflected);
            NuRndrEndReflectionRender();
        }
    }
}
