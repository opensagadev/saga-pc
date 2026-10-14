#include "saga.h"

i32 __cdecl PartDraw_Torp(PART_PC *part) {
    i32 draw = PartDraw_Flickerer(part);
    if (draw != 0 && WORLD->level_objects[0x7b].active != 0) {
        NUMTX matrix = part->transform;
        NuMtxPreScaleU(&matrix, (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0));
        NuSpecialDrawAt(&WORLD->level_objects[0x7b].special, &matrix);
    }
    return draw;
}

typedef struct torpedo_machine_query_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    f32 activation_time;
    u16 y_rotation;
    u8 unknown_22[9];
    u8 flags;
} TORPEDO_MACHINE_QUERY_PC;
typedef struct torpedo_machine_query_sys_pc_s {
    TORPEDO_MACHINE_QUERY_PC *machines;
    i32 count;
    f32 scale;
} TORPEDO_MACHINE_QUERY_SYS_PC;
STATIC_ASSERT(sizeof(TORPEDO_MACHINE_QUERY_PC) == 0x2c);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_PC, position) == 0x10);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_PC, activation_time) == 0x1c);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_PC, y_rotation) == 0x20);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_PC, flags) == 0x2b);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_SYS_PC, machines) == 0);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_QUERY_SYS_PC, count) == 4);
STATIC_ASSERT(sizeof(TORPEDO_MACHINE_QUERY_SYS_PC) == 0xc);
void *__cdecl GizTorpMachine_FindNearest(WORLDINFO *world, NUVEC *position, f32 *distance_squared) {
    TORPEDO_MACHINE_QUERY_PC *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    i32 index;
    if (world == NULL || world->torpedo_machine_sys == NULL)
        return NULL;
    for (index = 0; index < ((TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys)->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine =
            &((TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys)->machines[index];
        f32 distance;
        if ((machine->flags & 3) != 3)
            continue;
        distance = NuVecDistSqr(position, &machine->position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = machine;
        }
    }
    if (distance_squared != NULL)
        *distance_squared = nearest_distance;
    return nearest;
}

static char *GizTorp_GetOutputName(GIZMO_PC *unused0, i32 output_index) {
    return output_index == 0 ? ((char *)("Ready")) : NULL;
}

void __cdecl Draw3DObject(WORLDINFO *, i32, NUVEC *, u16, u16, u16, f32, f32, f32, i32);

void __cdecl GizTorp_Update(void *world_ptr, void *unused, f32 elapsed) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TORPEDO_MACHINE_QUERY_SYS_PC *system = (TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys;
    i32 index;
    (void)unused;
    (void)elapsed;
    if (system == NULL)
        return;
    for (index = 0; index < system->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine = &system->machines[index];
        if (machine->activation_time < 0.5f) {
            f32 time = machine->activation_time + FRAMETIME;
            machine->activation_time = time;
            if (time > 0.5f)
                machine->activation_time = 0.5f;
        }
    }
}

void __cdecl GizTorp_Draw(void *world_ptr, void *unused, f32 elapsed) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TORPEDO_MACHINE_QUERY_SYS_PC *system;
    i32 index;
    (void)unused;
    (void)elapsed;
    if (world == NULL)
        return;
    system = (TORPEDO_MACHINE_QUERY_SYS_PC *)world->torpedo_machine_sys;
    if (system == NULL || system->count == 0 || world->level_objects[0x122].active == 0 ||
        world->level_objects[0x123].active == 0)
        return;
    EnableShadowMapRendering(0);
    for (index = 0; index < system->count; ++index) {
        TORPEDO_MACHINE_QUERY_PC *machine = &system->machines[index];
        NUMTX matrix;
        i32 alternate;
        if ((machine->flags & 2) == 0 && editor_active == 0)
            continue;
        NuMtxSetRotationY(&matrix, machine->y_rotation);
        NuMtxTranslate(&matrix, &machine->position);
        NuMtxPreScaleU(&matrix, system->scale);
        alternate = (machine->flags >> 2) & 1;
        if (alternate == 0 && world->level_objects[0x122].active != 0)
            NuSpecialDrawAt(&world->level_objects[0x122].special, &matrix);
        else if (alternate == 1 && world->level_objects[0x123].active != 0)
            NuSpecialDrawAt(&world->level_objects[0x123].special, &matrix);
        if ((machine->flags & 1) != 0 && machine->activation_time > 0.0f) {
            f32 phase = machine->activation_time + machine->activation_time;
            if (phase >= 0.3f) {
                i32 angle = ((i32)(((f64)phase - (f64)0.3f) / (f64)0.7f * 16384.0) >> 1) & 0x7fff;
                f32 scale = (f32)(NuTrigTable[angle] * ((f64)WORLD->torpedo_machine_sys->scale * 4.0));
                angle = ((i32)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) / (f64)0.2f * 65536.0) >> 1) & 0x7fff;
                scale = (f32)((NuTrigTable[angle] * (f64)0.1f + 1.0) * scale);
                Draw3DObject(world, 0x79, &machine->position, 0, 0, 0, scale, scale, scale, 2);
                Draw3DObject(world, 0x7b, &machine->position, 0, 0, 0, scale, scale, scale, 2);
            }
        }
    }
    ResetShadowMapRendering();
}

i32 __cdecl GizTorp_Load(void *world_pointer, void *unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZTORPMACHINESYS_s *system;
    i32 version, index, length;
    i16 rotation;
    u8 value;
    (void)unused;
    if (world == NULL)
        return 0;
    system = (GIZTORPMACHINESYS_s *)world->torpedo_machine_sys;
    if (system == NULL || system->count != 0)
        return 0;
    version = EdFileReadInt();
    system->count = EdFileReadInt();
    system->scale = version >= 3 ? EdFileReadFloat() : 1.0f;
    for (index = 0; index < system->count; ++index) {
        length = EdFileReadInt();
        EdFileRead(system->machines[index].name, length);
        EdFileReadNuVec(&system->machines[index].position);
        rotation = EdFileReadShort();
        system->machines[index].y_rotation = (u16)rotation;
        if (version >= 2) {
            value = EdFileReadUnsignedChar();
            system->machines[index].flags = (u8)((system->machines[index].flags & ~4u) | (value != 0 ? 4 : 0));
        } else {
            system->machines[index].flags &= ~4u;
        }
    }
    return 1;
}
