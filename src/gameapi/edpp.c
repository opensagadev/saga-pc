#include "saga.h"

extern void *debkeydata;
#define EDPP_DEBRIS_KEYS_PC ((DEBRISKEY_PC *)debkeydata)

__declspec(noinline) void __cdecl edppPtlDestroy(i32 index) {
    if (edpp_ptls[index].instance_id != -1) {
        if (edpp_ptls[index].instance_id != 99999)
            DebFreeInstantly(&edpp_ptls[index].instance_id);
        --edpp_instances_used;
        edpp_ptls[index].instance_id = -1;
    }
}

void __cdecl edppPtlShelve(i32 index) {
    EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
    if (particle->instance_id != -1 && particle->instance_id != 99999) {
        DebFreeInstantly(&particle->instance_id);
        particle->instance_id = 99999;
    }
}

void __cdecl edppStopPage(i8 page) {
    i32 index, key, handle;
    EDPP_PARTICLE_PC *particle;
    edpp_page_on[page] = 0;
    for (index = 0; index < 512; ++index) {
        particle = &edpp_ptls[index];
        if (particle->instance_id == 99999 || particle->instance_id == -1)
            continue;
        if (particle->page == page) {
            DebFreeInstantly(&particle->instance_id);
            particle->instance_id = 99999;
        } else if (particle->effect_index > 0 && debtab[particle->effect_index] != NULL &&
                   debtab[particle->effect_index]->page == (u8)page) {
            edppPtlDestroy(index);
        }
    }
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL || debtab[index]->page != (u8)page)
            continue;
        for (key = 0; key < maxdebkeys; ++key) {
            if (EDPP_DEBRIS_KEYS_PC[key].effect_index == index) {
                handle = key;
                DebFreeInstantly(&handle);
            }
        }
        DebFreeOrphansInstantly(debtab[index]);
    }
}

void __cdecl edppClearPage(i8 page) {
    i32 index, key, handle;
    edpp_page_on[page] = 0;
    edpp_page_used[page] = 0;
    for (index = 0; index < 512; ++index) {
        if (edpp_ptls[index].page == page && edpp_ptls[index].instance_id != -1) {
            if (edpp_ptls[index].instance_id != 99999)
                DebFreeInstantly(&edpp_ptls[index].instance_id);
            --edpp_instances_used;
            edpp_ptls[index].instance_id = -1;
        }
    }
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL || debtab[index]->page != (u8)page)
            continue;
        debtab[index]->disabled = 1;
        for (key = 0; key < maxdebkeys; ++key) {
            if (EDPP_DEBRIS_KEYS_PC[key].effect_index == index) {
                handle = key;
                DebFreeInstantly(&handle);
            }
        }
        DebFreeOrphansInstantly(debtab[index]);
        if (debtab[index]->native_data != NULL) {
            DmaDebTypes[--freeDmaDebType] = debtab[index]->native_data;
            debtab[index]->native_data = NULL;
        }
        debtab[index] = NULL;
        --edpp_types_used;
    }
}

extern NUGSCN *EdppPageScenesPc[8];
void __cdecl DebrisSetFacing(i32, u8, i16, i16);
void __cdecl DebrisSetRoomID(i32, NUGSCN *);
void __cdecl edppStartSingleEffect(i32 index) {
    EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
    if (particle->instance_id != -1)
        return;
    AddDebrisEffect(&particle->instance_id, particle->effect_index, particle->position.x, particle->position.y,
                    particle->position.z);
    if (particle->instance_id == -1)
        return;
    EDPP_DEBRIS_KEYS_PC[particle->instance_id].field_2f9 = 0;
    DebrisOrientation(particle->instance_id, particle->rotation_z, particle->rotation_y);
    DebrisEmitterOrientation(particle->instance_id, particle->emitter_rotation_z, particle->emitter_rotation_y,
                             particle->emitter_rotation_x);
    DebrisStartOffsetEx(&EDPP_DEBRIS_KEYS_PC[particle->instance_id], particle->start_offset);
    DebrisSetTrigger(particle->instance_id, particle->switch_type, particle->switch_id, particle->switch_variable);
    DebrisReflectionOrientation(particle->instance_id, particle->reflection_rotation_z, particle->reflection_rotation_y,
                                particle->reflection_offset, particle->reflection_bounce);
    DebrisSetFacing(particle->instance_id, particle->facing_mode, particle->facing_rotation_x,
                    particle->facing_rotation_y);
    if (particle->instance_id != -1)
        EDPP_DEBRIS_KEYS_PC[particle->instance_id].render_group = particle->render_group;
    DebrisSetRoomID(particle->instance_id, EdppPageScenesPc[particle->page]);
    EDPP_DEBRIS_KEYS_PC[particle->instance_id].orientation_flags = (u8)particle->detail_levels;
}
void __cdecl edppStartPage(i32 page) {
    i32 index;
    for (index = 0; index < 512; ++index) {
        EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
        if (particle->page == (i8)page && particle->instance_id == 99999) {
            particle->instance_id = -1;
            particle->effect_index = LookupDebrisEffectPageIgnore(particle->name, (i8)page, 0);
            edppStartSingleEffect(index);
            if (particle->instance_id == -1)
                particle->instance_id = 99999;
        }
    }
    edpp_page_on[(i8)page] = 1;
}
extern i32 DebrisProcessingEnabledPc, globalframes;
extern i32 debris_sort_delay, debris_sort_this_stack, debris_update_delay, debris_update_this_stack;
extern i32 DebrisDmaCleanupDelayPc, DebrisDmaCleanupIndexPc;
void __cdecl edppDeleteEffect(i32);
extern i32 edpp_nearest;
extern NUVEC edpp_cam_pos;
void __cdecl edppDetermineNearest(f32);
void __cdecl edppDetermineNearest(f32 max_distance_squared) {
    NUVEC delta;
    i32 i;
    f32 distance;
    if (edpp_nearest != -1) {
        EDPP_PARTICLE_PC *particle = &edpp_ptls[edpp_nearest];
        if (particle->instance_id != 99999 && particle->instance_id != -1) {
            NuVecSub(&delta, &edpp_cam_pos, &particle->position);
            distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (distance == 0.0f)
                return;
        }
    }
    edpp_nearest = -1;
    for (i = 0; i < 512; ++i) {
        if (edpp_ptls[i].instance_id == -1 || edpp_ptls[i].instance_id == 99999)
            continue;
        NuVecSub(&delta, &edpp_cam_pos, &edpp_ptls[i].position);
        distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (max_distance_squared < 0.0f || max_distance_squared > distance) {
            max_distance_squared = distance;
            edpp_nearest = i;
        }
    }
}
void __cdecl edppDeleteEffect(i32 index) {
    i32 replacement, i;
    if (edpp_ptls[edpp_nearest].effect_index == index)
        edpp_nearest = -1;
    DebFreeOrphansInstantly(debtab[index]);
    replacement = LookupDebrisEffectPageIgnore(debtab[index]->name, 1, index);
    for (i = 0; i < 512; ++i) {
        EDPP_PARTICLE_PC *particle = &edpp_ptls[i];
        if (particle->effect_index != index)
            continue;
        if (replacement == -1) {
            if (particle->instance_id == -1)
                continue;
            if (particle->instance_id != 99999)
                DebFreeInstantly(&particle->instance_id);
            --edpp_instances_used;
            particle->instance_id = -1;
        } else {
            i32 handle = particle->instance_id;
            if (handle != 99999 && handle != -1)
                EDPP_DEBRIS_KEYS_PC[handle].effect_index = (i16)replacement;
            particle->effect_index = replacement;
        }
    }
    for (i = 0; i < maxdebkeys; ++i) {
        if (EDPP_DEBRIS_KEYS_PC[i].effect_index == index) {
            if (replacement == -1) {
                i32 handle = i;
                DebFreeInstantly(&handle);
            } else
                EDPP_DEBRIS_KEYS_PC[i].effect_index = (i16)replacement;
        }
    }
    debtab[index] = NULL;
    --edpp_types_used;
    edppDetermineNearest(1.0f);
}
