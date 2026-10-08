#include "saga.h"
struct HATMACHINE_PC {
    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 yaw;
    u8 configured_hat, displayed_hat;
    i8 animation_state;
    u8 model_letter, model_special_index, flags;
    NUVEC player_position, target_offset;
    u16 terrain_pitch, terrain_roll;
    i16 platform_id;
    u16 reserved_82;
    f32 hat_delay, state_duration, state_elapsed, scale, animation_time, reserved_98, blink_timer;
};
struct HATMACHINE_SYS_PC {
    i32 count, reserved_04, reserved_08;
    HATMACHINE_PC *machines;
};
struct HATMACHINE_WORLD_PC {
    u8 unknown_000[0x5074];
    HATMACHINE_SYS_PC *hat_machine_sys;
};
STATIC_ASSERT(sizeof(HATMACHINE_PC) == 0xa0);
STATIC_ASSERT(sizeof(HATMACHINE_SYS_PC) == 0x10);
STATIC_ASSERT(offsetof(HATMACHINE_SYS_PC, count) == 0);
STATIC_ASSERT(offsetof(HATMACHINE_SYS_PC, machines) == 0xc);
STATIC_ASSERT(offsetof(HATMACHINE_WORLD_PC, hat_machine_sys) == 0x5074);
STATIC_ASSERT(offsetof(HATMACHINE_PC, position) == 0x50);
STATIC_ASSERT(offsetof(HATMACHINE_PC, yaw) == 0x5c);
STATIC_ASSERT(offsetof(HATMACHINE_PC, flags) == 0x63);
STATIC_ASSERT(offsetof(HATMACHINE_PC, player_position) == 0x64);
STATIC_ASSERT(offsetof(HATMACHINE_PC, target_offset) == 0x70);
STATIC_ASSERT(offsetof(HATMACHINE_PC, platform_id) == 0x80);
extern "C" void __cdecl Hat_GetAbsTargetPos(HATMACHINE_PC *machine, NUVEC *position) {
    if (machine != NULL && position != NULL) {
        NUVEC offset = machine->target_offset;
        NuVecRotateY(&offset, &offset, machine->yaw);
        offset.x += machine->position.x;
        offset.z += machine->position.z;
        *position = offset;
    }
}
extern "C" void __cdecl HatMachines_InitTerrain(WORLDINFO *world) {
    if (((HATMACHINE_WORLD_PC *)world)->hat_machine_sys != NULL) {
        for (i32 index = 0; index < ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->count; ++index) {
            HATMACHINE_PC *machine = &((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index];
            i16 platform = NewPlatPickupInst(machine, 2);
            ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index].platform_id = platform;
            PlatInstRotate(((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index].platform_id, 1);
        }
    }
}
extern "C" HATMACHINE_PC *__cdecl HatMachine_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object,
                                                         f32 *distance) {
    f32 nearest_distance = 1.0e9f;
    HATMACHINE_PC *nearest = NULL;
    if (world == NULL || ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys == NULL)
        return NULL;
    for (i32 index = 0; index < ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->count; ++index) {
        HATMACHINE_PC *machine = &((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index];
        f32 candidate_distance;
        if (object != NULL) {
            if ((machine->flags & 8) == 0 || (machine->flags & 4) == 0 || (machine->flags & 3) != 0 ||
                machine->player_position.y == 2000000.0f)
                continue;
            NUVEC target;
            Hat_GetAbsTargetPos(machine, &target);
            candidate_distance = NuVecDistSqr(position, &target, NULL);
        } else {
            candidate_distance = NuVecDistSqr(position, &machine->position, NULL);
        }
        if (candidate_distance < nearest_distance) {
            nearest_distance = candidate_distance;
            nearest = machine;
        }
    }
    if (distance != NULL)
        *distance = nearest_distance;
    return nearest;
}
extern "C" i32 __cdecl HatMachine_BeingUsed(HATMACHINE_PC *machine) {
    return machine->flags & 1;
}
