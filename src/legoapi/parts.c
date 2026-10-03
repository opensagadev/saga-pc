#include "saga.h"

void __cdecl InitParts(i32 count, VARIPTR *buffer, VARIPTR end) {
    i32 i, j;
    MAXPARTS = count;
    buffer->addr = (buffer->addr + 15) & ~(u32)15;
    Part = (PART_PC *)buffer->void_ptr;
    buffer->addr += MAXPARTS * sizeof(PART_PC);
    if (Part != NULL)
        memset(Part, 0, MAXPARTS * sizeof(PART_PC));
    part_slot_cursor = 0;
    memset(part_types, 0, sizeof(part_types));
    for (i = 0; i < 128; ++i) {
        for (j = 0; j < 8; ++j) {
            part_types[i].effect_ids[j] = -1;
            part_types[i].effect_pages[j] = -1;
        }
    }
    part_types_used = 0;
    memset(part_emits, 0, sizeof(part_emits));
    for (i = 0; i < 40; ++i)
        part_emits[i].effect_id = -1;
    part_emits_used = 0;
    memset(part_page_used, 0, sizeof(part_page_used));
    memset(part_page_on, 0, sizeof(part_page_on));
    memset(part_scene, 0, sizeof(part_scene));
    memset(part_scene_pageid, -1, sizeof(part_scene_pageid));
    part_platimpactcnt = 0;
}

__declspec(noinline) void __cdecl KillPartsByScene(NUGSCN *scene) {
    PART_PC *part = Part;
    i32 index;
    for (index = 0; index < MAXPARTS; ++index, ++part) {
        if ((part->active & 1) != 0 && part->source_special != NULL && part->special.scene == scene) {
            part->active &= ~1;
            memset(&part->special, 0, sizeof(part->special));
            part->source_special = NULL;
        }
    }
}

void __cdecl edpartClearPage(i8 page) {
    i32 index;
    NuThreadDisableThreadSwap();
    if (part_page_on[page] != 0)
        part_page_on[page] = 0;
    for (index = 0; index < 128; ++index) {
        if (part_types[index].page == page && part_types[index].name[0] != 0) {
            part_types[index].name[0] = 0;
            part_types[index].effect_ids[0] = -1;
            --part_types_used;
        }
    }
    for (index = 0; index < 40; ++index) {
        if (part_emits[index].page == page && part_emits[index].effect_id != -1) {
            part_emits[index].effect_id = -1;
            --part_emits_used;
        }
    }
    for (index = 0; index < 32; ++index) {
        if (part_scene_pageid[index] == page) {
            KillPartsByScene(part_scene[index]);
            part_scene_pageid[index] = -1;
            part_scene[index] = NULL;
        }
    }
    NuThreadEnableThreadSwap();
    part_page_used[page] = 0;
}

__declspec(noinline) i32 __cdecl PARTLookupTypePageOnly(char *name, i32 page) {
    i32 i;
    if (name == NULL || name[0] == 0)
        return -1;
    if ((u32)(page - 1) <= 6) {
        for (i = 0; i < 128; ++i) {
            if (part_types[i].page == page && NuStrNICmp(name, part_types[i].name, 16) == 0)
                return i;
        }
    }
    for (i = 0; i < 128; ++i) {
        if (part_types[i].page == 0 && NuStrNCmp(name, part_types[i].name, 16) == 0)
            return i;
    }
    return -1;
}

#include <string.h>

void __cdecl KillPart(PART_PC *part, i32 reason) {
    if ((part->active & 1) == 0) {
        return;
    }
    part->active &= ~1u;
    if ((part->flags & 0x20000) != 0 && part->debris_handle != NULL) {
        STUBBED_NAMED("KillPart: DebFreeInstantly");
    }
    if (part->lighting_handle != NULL) {
        STUBBED_NAMED("KillPart: rtlDynamicFree");
        *part->lighting_handle = -1;
    }
    if (part->kill_fn != NULL) {
        part->kill_fn(part, reason);
    }
    if (reason != 6 && part->effect_id_1f4 != -1) {
        STUBBED_NAMED("KillPart: AddScaledFiniteShotDebrisEffect");
    }
}

__declspec(noinline) void __cdecl NewPartRotation(PART_PC *part) {
    i32 index;
    for (index = 0; index < 2; ++index) {
        do {
            part->rotation_axes[index] = (u8)((NuRandIntSeeded(&partseed) % 65535u) / 21845u + 1);
        } while (index == 1 && part->rotation_axes[0] == part->rotation_axes[1]);
        part->field_124[index + 3] = (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * 65536.0);
    }
}

__declspec(noinline) void __cdecl NewPartOrderedRotation(PART_PC *part) {
    i32 index;
    for (index = 0; index < 3; ++index) {
        part->field_124[index] =
            part->rotation_parameters[index] +
            (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[index + 3]);
    }
}

PART_PC *__cdecl AddPart(ADDPART_PC *parameters) {
    NUVEC forward = {0.0f, 0.0f, -1.0f};
    PART_PC *part;
    i32 scanned = 0;
    i32 index;

    if (parameters->special != NULL) {
        if (NuSpecialExistsFn(parameters->special) == 0) {
            return NULL;
        }
    } else if (parameters->part_id != -1 && parameters->part_id != 9999) {
        return NULL;
    }
    part = &Part[part_slot_cursor];
    while ((part->active & 1) != 0 && (part->flags & 0x200) != 0 && scanned < MAXPARTS) {
        if (++part_slot_cursor == MAXPARTS) {
            part_slot_cursor = 0;
        }
        part = &Part[part_slot_cursor];
        ++scanned;
    }
    if (scanned == MAXPARTS) {
        return NULL;
    }
    if ((part->active & 1) != 0) {
        if (part->destroy_fn != NULL) {
            part->destroy_fn(part);
        }
        KillPart(part, 0);
    }
    if (parameters->matrix != NULL) {
        NUMTX matrix = *parameters->matrix;
        part->transform = *parameters->matrix;
        NuVecMtxRotate(&forward, &forward, &matrix);
        part->rotation_y = (u16)NuAtan2D(forward.x, forward.z);
        NuVecRotateY(&forward, &forward, -(i32)part->rotation_y);
        part->rotation_x = (u16)-NuAtan2D(forward.y, forward.z);
    } else {
        if (parameters->position == NULL) {
            return NULL;
        }
        NuMtxSetTranslation(&part->transform, parameters->position);
    }
    part->previous_position = part->position;
    part->owner = parameters->owner;
    part->recipient = parameters->recipient;
    part->velocity = *parameters->velocity;
    part->field_206 = -1;
    part->field_20b = -1;
    part->time_remaining = part->initial_time = parameters->field_a4;
    part->active = (part->active & ~0x60u) | 0x21;
    part->time_step = parameters->time_step;
    part->scale_time = 0.0f;
    part->radius = parameters->field_14;
    part->field_e0 = (parameters->time_step / PARTSCALEUPTIME) * parameters->field_14;
    part->field_e4 = parameters->field_18;
    part->field_214 = parameters->field_c0;
    part->gravity = parameters->gravity;
    part->field_ec = parameters->field_20;
    part->active &= ~2u;
    part->part_id = (i16)parameters->part_id;
    part->field_f0 = parameters->field_88;
    part->flags = parameters->flags;
    part->flicker_flags = parameters->field_c4;
    part->active &= ~0x4000u;
    if (parameters->lighting_data != NULL) {
        for (index = 0; index < 7; ++index) {
            part->lighting[index] = parameters->lighting_data->values[index];
        }
        part->active |= 8;
    } else {
        part->active &= ~8u;
        if (parameters->lighting_scale_x != 1.0f || parameters->lighting_scale_y != 1.0f ||
            parameters->lighting_scale_z != 1.0f) {
            part->lighting[6].x = parameters->lighting_scale_x;
            part->lighting[6].y = parameters->lighting_scale_y;
            part->lighting[6].z = parameters->lighting_scale_z;
            part->active |= 0x2000;
        } else {
            part->active &= ~0x2000u;
        }
    }
    part->source_special = parameters->special;
    if (parameters->special != NULL) {
        part->special = *parameters->special;
    } else {
        memset(&part->special, 0, sizeof(part->special));
    }
    part->active &= ~0x14u;
    part->field_f4 = 0.0f;
    part->callback_1ac = parameters->callback_30;
    part->move_fn = parameters->move_fn;
    part->deflect_fn = parameters->deflect_fn;
    part->impact_fn = parameters->impact_fn;
    part->collision_fn = parameters->collision_fn;
    part->kill_fn = parameters->kill_fn;
    part->update_fn = parameters->update_fn;
    part->stop_fn = parameters->stop_fn;
    part->destroy_fn = parameters->destroy_fn;
    part->draw_fn = parameters->draw_fn;
    part->collision_min.x = part->position.x - part->radius;
    part->collision_min.y = part->position.y - part->radius;
    part->collision_min.z = part->position.z - part->radius;
    part->collision_max.x = part->position.x + part->radius;
    part->collision_max.y = part->position.y + part->radius;
    part->collision_max.z = part->position.z + part->radius;
    for (index = 0; index < 6; ++index) {
        part->field_10c[index] = parameters->field_a8[index];
    }
    for (index = 0; index < 5; ++index) {
        part->field_124[index] = 0;
    }
    if ((part->flags & 0x80) != 0) {
        NewPartRotation(part);
    }
    if ((part->flags & 0x100000) != 0) {
        NewPartOrderedRotation(part);
    }
    if (parameters->effect_id_58 != -1) {
        part->inline_debris_handle = -1;
        part->debris_handle = &part->inline_debris_handle;
        STUBBED_NAMED("AddPart: scaled debris creation and instance population");
    } else {
        part->debris_handle = parameters->debris_handle;
    }
    part->effect_id_1e0 = parameters->effect_id_60;
    part->effect_id_1e4 = parameters->effect_id_64;
    part->field_1e8 = parameters->field_68;
    part->field_1ec = parameters->field_6c;
    part->effect_id_1f0 = parameters->effect_id_70;
    part->effect_id_1f8 = parameters->effect_id_74;
    part->effect_id_1f4 = parameters->effect_id_78;
    part->effect_scale = parameters->effect_scale;
    part->previous_transform = part->transform;
    part->field_208 = 1;
    part->force_flags = 0;
    part->field_21c = 0;
    part->active |= 0x1800;
    part->field_210 = 0;
    part->hostility_group = (u8)parameters->hostility_group;
    part->field_204 = 1;
    if ((part->flags & 0x2000) != 0 && parameters->lighting_handle != NULL && PartRTL != NULL) {
        part->lighting_handle = parameters->lighting_handle;
        STUBBED_NAMED("AddPart: dynamic lighting template allocation and handle assignment");
    }
    if (++part_slot_cursor == MAXPARTS) {
        part_slot_cursor = 0;
    }
    return part;
}

PART_PC *__cdecl HitParts(GAMEOBJECT *excluded_owner, NUVEC *points, i32 point_count, f32 radius, NUVEC *minimum,
                          NUVEC *maximum, u32 flags) {
    PART_PC *part = Part;
    i32 index;

    for (index = 0; index < MAXPARTS; ++index, ++part) {
        i32 point;
        if ((part->active & 1) == 0 || part->owner == excluded_owner || (part->flags & flags) == 0) {
            continue;
        }
        if (part->collision_min.x > maximum->x || minimum->x > part->collision_max.x ||
            part->collision_min.z > maximum->z || minimum->z > part->collision_max.z ||
            part->collision_min.y > maximum->y || minimum->y > part->collision_max.y) {
            continue;
        }
        for (point = point_count - 1; point >= 0; --point) {
            NUVEC difference;
            f32 distance;
            f32 combined_radius;
            NuVecSub(&difference, &points[point], &part->position);
            distance = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
            combined_radius = radius + part->radius;
            if (distance <= combined_radius * combined_radius) {
                return part;
            }
        }
    }
    return NULL;
}

__declspec(noinline) void edpartStartPage(u8 page) {
    i32 i;
    i8 signed_page = (i8)page;

    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1 && part_emits[i].page == signed_page) {
            part_emits[i].camera_distance = 9999.9f;
        }
    }
    part_page_on[signed_page] = 1;
}

__declspec(noinline) void edpartStopPage(u8 page) {
    part_page_on[(i8)page] = 0;
}

void Parts_Start(WORLDINFO *world) {
    if (world->page_part != -1) {
        edpartStartPage((u8)world->page_part);
    }
}

void Parts_Stop(WORLDINFO *world) {
    if (world->page_part != -1) {
        edpartStopPage((u8)world->page_part);
    }
}

void SetPartRTLSet(RTLSET_PC *rtl_set) {
    PartRTL = rtl_set;
}

void NuPartSetSeed(i32 seed) {
    partseed = (u32)seed;
}

u32 NuPartGetSeed(void) {
    return partseed;
}

void __cdecl AddPartDebris(PARTDEBRISSYS *sys, i32 index, NUVEC *position) {
    PARTDEBRISENTRY *entry;

    if (sys == NULL || index < 0 || index >= part_debris_count) {
        return;
    }
    entry = &sys->entries[index];
    if (entry->id == -1) {
        return;
    }
    AddFiniteShotPART(entry->id, position, 1);
}

void __cdecl PartStop_Flickerer(PART_PC *part) {
    part->time_remaining = 5.0f;
    if ((part->flicker_flags & 1) != 0)
        part->time_remaining = 2.5f;
}
i32 __cdecl PartDraw_Flickerer(PART_PC *part) {
    if (part->time_remaining > 0.0f && (u32)GameTimer.update_count % 6 >= 3) {
        f32 threshold = 2.0f;
        if ((part->flicker_flags & 1) != 0)
            threshold = 1.0f;
        if (part->time_remaining <= threshold)
            return 0;
    }
    return 1;
}
void __cdecl PartImpact_Basketball(PART_PC *part) {
    f32 volume = NuVecMag(&part->velocity);
    if (volume >= 2.5f)
        volume = 1.0f;
    else if (volume >= (f64)0.1f)
        volume = (f32)(((f64)volume - (f64)0.1f) / (f64)2.4f);
    else
        return;
    if (volume > 0.0f)
        PlaySfxAndSetVolume("BBounce", &part->position, volume);
}

void __cdecl PartUpdate_Basketball(PART_PC *part) {
    GIZMOBLOWUP_PC *blowup;
    if (LevGizmo[0] == NULL)
        return;
    blowup = (GIZMOBLOWUP_PC *)LevGizmo[0]->object;
    if ((blowup->status_flags & 0x800001) == 0 &&
        NuVecDistSqr(&blowup->position, &part->position, NULL) < 0.1f * 0.1f) {
        GizmoActivate(WORLD->gizmo_sys, LevGizmo[0], 1, 0);
        STUBBED_NAMED("PartUpdate_Basketball: GizmoBlowupBlowup PC567800(blowup,1,-1,-1,NULL,1)");
    }
}
void __cdecl PartCollide_3D(PART_PC *part) {
    (void)part;
    STUBBED_NAMED(
        "PartCollide_3D: entire private PartCollide PC42c740 collision action, PART in EDI and mode1 on stack");
}

typedef struct DEBRIS_KEY_EMISSION_PC {
    u8 unknown_000[0x190];
    NUVEC emitter_position;
    u8 unknown_19c[0xc];
    NUVEC particle_momentum;
    NUVEC emitter_momentum;
    u8 unknown_1c0[0x140];
    f32 position_reset_300;
    u8 unknown_304[0x2c];
} DEBRIS_KEY_EMISSION_PC;
STATIC_ASSERT(sizeof(DEBRIS_KEY_EMISSION_PC) == 0x330);
STATIC_ASSERT(offsetof(DEBRIS_KEY_EMISSION_PC, emitter_position) == 0x190);
STATIC_ASSERT(offsetof(DEBRIS_KEY_EMISSION_PC, particle_momentum) == 0x1a8);
STATIC_ASSERT(offsetof(DEBRIS_KEY_EMISSION_PC, emitter_momentum) == 0x1b4);
STATIC_ASSERT(offsetof(DEBRIS_KEY_EMISSION_PC, position_reset_300) == 0x300);

extern void *debkeydata;
#define DEBRIS_EMISSION_KEYS_PC ((DEBRIS_KEY_EMISSION_PC *)debkeydata)

void __cdecl DebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        DEBRIS_EMISSION_KEYS_PC[handle].emitter_position.x = x;
        DEBRIS_EMISSION_KEYS_PC[handle].emitter_position.y = y;
        DEBRIS_EMISSION_KEYS_PC[handle].emitter_position.z = z;
        DEBRIS_EMISSION_KEYS_PC[handle].position_reset_300 = 0.0f;
    }
}

void __cdecl DebrisEmitterMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        DEBRIS_EMISSION_KEYS_PC[handle].emitter_momentum.x = x;
        DEBRIS_EMISSION_KEYS_PC[handle].emitter_momentum.y = y;
        DEBRIS_EMISSION_KEYS_PC[handle].emitter_momentum.z = z;
    }
}

void __cdecl DebrisParticleMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        DEBRIS_EMISSION_KEYS_PC[handle].particle_momentum.x = x;
        DEBRIS_EMISSION_KEYS_PC[handle].particle_momentum.y = y;
        DEBRIS_EMISSION_KEYS_PC[handle].particle_momentum.z = z;
    }
}

#undef DEBRIS_EMISSION_KEYS_PC

i32 __cdecl PARTLookupType(char *name) {
    i32 index;
    if (name != NULL && name[0] != 0) {
        for (index = 0; index < 128; ++index) {
            if (NuStrNICmp(name, part_types[index].name, 16) == 0)
                return index;
        }
    }
    return -1;
}

#include "saga.h"
extern void *debkeydata;
#include <string.h>
#define DEBRIS_KEYS_PC ((DEBRISKEY_PC *)debkeydata)
static __declspec(noinline) void DebrisMtxCopyPrivate(NUMTX *destination, const NUMTX *source) {
    *destination = *source;
}

static __declspec(noinline) void DebrisMtxRotateXPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m01;
    f32 m11;
    f32 m21;
    f32 m31;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m01 = m->m01;
    m11 = m->m11;
    m21 = m->m21;
    m31 = m->m31;

    m->m01 = m01 * cosx - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
}

static __declspec(noinline) void DebrisMtxRotateYPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static __declspec(noinline) void DebrisMtxRotateZPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = m00 * cosx - m->m01 * sinx;
    m->m01 = m00 * sinx + m->m01 * cosx;
    m->m10 = m10 * cosx - m->m11 * sinx;
    m->m11 = m10 * sinx + m->m11 * cosx;
    m->m20 = m20 * cosx - m->m21 * sinx;
    m->m21 = m20 * sinx + m->m21 * cosx;
    m->m30 = m30 * cosx - m->m31 * sinx;
    m->m31 = m30 * sinx + m->m31 * cosx;
}

void __cdecl DebrisOrientation(i32 handle, i16 z, i16 y) {
    if (handle == -1)
        return;
    DebrisMtxCopyPrivate(&DEBRIS_KEYS_PC[handle].effect_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&DEBRIS_KEYS_PC[handle].effect_orientation, z);
    DebrisMtxRotateYPrivate(&DEBRIS_KEYS_PC[handle].effect_orientation, y);
    DEBRIS_KEYS_PC[handle].orientation_dirty = 0.0f;
}
void __cdecl DebrisEmitterOrientation(i32 handle, i16 z, i16 y, i16 x) {
    if (handle == -1)
        return;
    DebrisMtxCopyPrivate(&DEBRIS_KEYS_PC[handle].emitter_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&DEBRIS_KEYS_PC[handle].emitter_orientation, z);
    DebrisMtxRotateYPrivate(&DEBRIS_KEYS_PC[handle].emitter_orientation, y);
    DebrisMtxRotateXPrivate(&DEBRIS_KEYS_PC[handle].emitter_orientation, x);
    DEBRIS_KEYS_PC[handle].orientation_dirty = 0.0f;
}

void DebrisReflectionOrientation(i32 handle, i16 x, i16 y, f32 plane, f32 scale) {
    DEBRISKEY_PC *key;
    if (handle != -1) {
        key = &DEBRIS_KEYS_PC[handle];
        key->collision_plane = plane;
        key->reflection_x = x;
        key->reflection_y = y;
        key->reflection_scale = scale;
    }
}

void DebrisSetTrigger(i32 handle, i32 first, i32 second, f32 third) {
    if (handle != -1) {
        DEBRISKEY_PC *key = &DEBRIS_KEYS_PC[handle];
        key->switch_variable = third;
        key->trigger_first = first;
        key->trigger_second = second;
    }
}

void DebrisStartOffset(i32 handle, f32 offset) {

    DebrisStartOffsetEx(DEBRIS_KEYS_PC + handle, offset);
}

void AddDebrisEffect(i32 *handle, i32 effect_index, f32 x, f32 y, f32 z) {
    i32 i;
    DEBRIS_INFO_PC *effect;
    i32 newly_allocated;
    i32 key_index;
    f32 now;
    DEBRISKEY_PC *key;
    if (effect_index == -1 || effect_index == 0 || effect_index >= debinf_count) {
        return;
    }
    effect = debtab[effect_index];
    if (effect == NULL)
        return;
    if (effect->disabled != 0) {
        return;
    }

    newly_allocated = 0;
    key_index = *handle;
    if (key_index == -1) {
        key_index = DebAlloc();
        *handle = key_index;
        if (key_index == -1) {
            return;
        }
        newly_allocated = 1;
    }

    now = effect->time_group == 4 ? panelglobaltime : globaltime;
    DEBRIS_KEYS_PC[key_index].finite = 0;
    DEBRIS_KEYS_PC[key_index].effect_index = (i16)(effect_index);
    DebrisStartOffset(key_index, effect->emission_start_offset);
    DEBRIS_KEYS_PC[key_index].generator = gensorttab[(i8)(effect->generator_type)];
    DEBRIS_KEYS_PC[key_index].momentum_adjuster = gencodetab[(i8)(effect->momentum_adjustment_type)];
    DEBRIS_KEYS_PC[key_index].finite_count = 0;
    DEBRIS_KEYS_PC[key_index].sphere_next_time = 0.0f;
    DEBRIS_KEYS_PC[key_index].emitter_rotation_x = 0;
    DEBRIS_KEYS_PC[key_index].emitter_rotation_y = 0;
    DEBRIS_KEYS_PC[key_index].sphere_index = 0;
    for (i = 0; i < effect->process_spheres; ++i) {
        DEBRIS_KEYS_PC[key_index].process_spheres[i].time = -1.0f;
    }
    memset(&DEBRIS_KEYS_PC[key_index].emission_position, 0, sizeof(DEBRIS_KEYS_PC[key_index].emission_position));
    memset(&DEBRIS_KEYS_PC[key_index].momentum, 0, sizeof(DEBRIS_KEYS_PC[key_index].momentum));
    memset(&DEBRIS_KEYS_PC[key_index].emitter_momentum, 0, sizeof(DEBRIS_KEYS_PC[key_index].emitter_momentum));
    DEBRIS_KEYS_PC[key_index].orientation_dirty = 0.0f;
    DEBRIS_KEYS_PC[key_index].cutoff_distance = 999999.0f;
    DEBRIS_KEYS_PC[key_index].field_2f2 = -1;
    DEBRIS_KEYS_PC[key_index].emission_disabled = 0;
    DEBRIS_KEYS_PC[key_index].orientation_flags = 7;
    DEBRIS_KEYS_PC[key_index].gscene = NULL;
    DEBRIS_KEYS_PC[key_index].process_collision_sound = 0;
    DEBRIS_KEYS_PC[key_index].last_update_time = now;
    DEBRIS_KEYS_PC[key_index].field_2f9 = 1;
    DEBRIS_KEYS_PC[key_index].user_data = NULL;
    DEBRIS_KEYS_PC[key_index].field_2fa = 0;
    DEBRIS_KEYS_PC[key_index].emission_epoch =
        DEBRIS_KEYS_PC[key_index].emission_end_time < now ? DEBRIS_KEYS_PC[key_index].emission_end_time : now;

    switch (effect->particle_type) {
        case 3:
            DEBRIS_KEYS_PC[key_index].render_priority = (i16)(40000);
            break;
        case 7:
            DEBRIS_KEYS_PC[key_index].render_priority = (i16)(20000);
            break;
        case 2:
            DEBRIS_KEYS_PC[key_index].render_priority = (i16)(50000);
            break;
        default:
            DEBRIS_KEYS_PC[key_index].render_priority = (i16)(30000);
            break;
    }
    for (i = 0; i < 4; ++i) {
        DEBRIS_KEYS_PC[key_index].collision_timers[i] = 9999;
        if (effect->sound_data[i * 3] != -1) {
            if (effect->sound_data[i * 3 + 1] == 3 || effect->sound_data[i * 3 + 1] == 4)
                DEBRIS_KEYS_PC[key_index].collision_timers[i] = 1;
            DEBRIS_KEYS_PC[key_index].process_collision_sound = 1;
        }
    }
    DebrisEmitterPos(key_index, x, y, z);
    DebrisEmitterOrientation(key_index, 0, 0, 0);
    DebrisOrientation(key_index, 0, 0);
    DebrisReflectionOrientation(key_index, 0, 0, 0, 0.9f);
    DebrisSetTrigger(key_index, 0, -1, 0);
    if (newly_allocated) {
        key = DEBRIS_KEYS_PC + key_index;
        if (debris_keystack != NULL)
            debris_keystack->next = key;
        key->previous = debris_keystack;
        debris_keystack = key;
    }
}

void AddFiniteShotDebrisEffect2(i32 *handle, i32 effect, NUVEC *position, NUVEC *emitter_momentum,
                                NUVEC *particle_momentum, i32 count) {
    i32 sound;
    f32 now;
    f32 offset;
    f32 range;
    f32 volume;
    if (debris_suspended != 0 || update_debris_enabled == 0)
        return;
    AddDebrisEffect(handle, effect, position->x, position->y, position->z);
    if (*handle == -1)
        return;
    DEBRIS_KEYS_PC[*handle].finite_count = count;
    now = debtab[effect]->time_group == 4 ? panelglobaltime : globaltime;
    DEBRIS_KEYS_PC[*handle].finite = 1;
    DEBRIS_KEYS_PC[*handle].emission_disabled = 0;
    offset = DEBRIS_KEYS_PC[*handle].emission_time - now;
    DEBRIS_KEYS_PC[*handle].emission_time -= offset;
    DEBRIS_KEYS_PC[*handle].emission_end_time -= offset;
    DEBRIS_KEYS_PC[*handle].emission_epoch = DEBRIS_KEYS_PC[*handle].last_update_time;
    DEBRIS_KEYS_PC[*handle].cutoff_distance =
        (debris_cutoff_camera_vec != NULL ? NuVecDist(position, (NUVEC *)debris_cutoff_camera_vec, NULL) : 0.0f);
    if (DEBRIS_KEYS_PC[*handle].process_collision_sound != 0) {
        range = debtab[effect]->sound_range_override;
        if (range == 0.0f)
            range = debtab[effect]->sound_range;
        if (range == 0.0f)
            range = debtab[effect]->clip_extent;
        volume = 0.0f;
        if (DEBRIS_KEYS_PC[*handle].cutoff_distance < range) {
            for (sound = 0; sound < 4; ++sound) {
                if (debtab[effect]->sound_data[sound * 3] != -1)
                    SetSfxBit_On(debtab[effect]->sound_data[sound * 3]);
            }
            volume = (range - DEBRIS_KEYS_PC[*handle].cutoff_distance) / range;
        }
        if (DEBRIS_KEYS_PC[*handle].cutoff_distance < range) {
            for (sound = 0; sound < 4; ++sound) {
                if (debtab[effect]->sound_data[sound * 3] != -1 && debtab[effect]->sound_data[sound * 3 + 1] == 1)
                    PlaySfxByIdEx(debtab[effect]->sound_data[sound * 3], position, volume, 1.0f);
            }
        }
    }
    if (emitter_momentum != NULL) {
        DebrisEmitterMomentum(*handle, emitter_momentum->x, emitter_momentum->y, emitter_momentum->z);
    }
    if (particle_momentum != NULL) {
        DebrisParticleMomentum(*handle, particle_momentum->x, particle_momentum->y, particle_momentum->z);
    }
}

void AddFiniteShotDebrisEffect(i32 *handle, i32 effect, NUVEC *position, i32 count) {
    AddFiniteShotDebrisEffect2(handle, effect, position, NULL, NULL, count);
}

#undef DEBRIS_KEYS_PC

#define DEBRIS_KEYS_PC ((DEBRISKEY_PC *)debkeydata)
i32 __cdecl DebrisNuASin(f32);
DEBRISPARTICLE_PC *GenDebIndexSpheroid(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
    i32 trail;
    i16 particle_index;
    DEBRISPARTICLE_PC *particle;
    NUVEC vector;
    i32 angle_y;
    f32 random;
    f32 sine;
    i16 angle_z;
    NUMTX scale;
    i16 trail_index;
    DEBRISPARTICLE_PC *trail_particle;
    particle_index = key->particle_cursor;
    if (particle_index < key->particle_count) {
        ++key->particle_cursor;
    } else {
        particle_index = 0;
        key->particle_cursor = 1;
    }

    particle = &key->particle_chunks[particle_index / (effect->particle_type == 7 ? 12 : 32)]
                    ->particles[particle_index % (effect->particle_type == 7 ? 12 : 32)];
    particle->start_time = time;
    key->emission_epoch = time;
    particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

    vector.x = NuFsqrt(NuRandFloatSeeded(&debrisseed));
    vector.y = 0.0f;
    vector.z = 0.0f;
    angle_y = (i32)(NuRandFloatSeeded(&debrisseed) * 65536.0f);
    random = NuRandFloatSeeded(&debrisseed);
    sine = random + random - 1.0f;
    angle_z = (i16)(DebrisNuASin(sine));
    NuVecRotateZ(&vector, &vector, angle_z);
    NuVecRotateY(&vector, &vector, angle_y);

    DebrisMtxCopyPrivate(&scale, &ANI3_EVAL_IDENTITY_MATRIX);
    scale.m00 = effect->emitter_random_extent.x;
    scale.m11 = effect->emitter_random_extent.y;
    scale.m22 = effect->emitter_random_extent.z;
    NuVecMtxTransformVU0(&vector, &vector, &scale);
    NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
    particle->position = vector;
    random = NuRandFloatSeeded(&debrisseed);
    vector.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x;
    random = NuRandFloatSeeded(&debrisseed);
    vector.y = (random + random) * effect->rotor_speed_x - effect->rotor_speed_x + effect->momentum_base;
    random = NuRandFloatSeeded(&debrisseed);
    vector.z = (random + random) * effect->rotor_speed_y - effect->rotor_speed_y;
    NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
    particle->momentum = vector;
    if (key->momentum_adjuster != NULL) {
        key->momentum_adjuster(key, effect, particle);
    }
    NuVecAdd(&particle->position, &particle->position, &key->emission_position);
    NuVecAdd(&particle->momentum, &particle->momentum, &key->momentum);

    for (trail = 0; trail < (i8)(effect->trail_count); ++trail) {
        trail_index = key->particle_cursor;
        if (trail_index < key->particle_count) {
            ++key->particle_cursor;
        } else {
            trail_index = 0;
            key->particle_cursor = 1;
        }
        trail_particle = &key->particle_chunks[trail_index / (effect->particle_type == 7 ? 12 : 32)]
                              ->particles[trail_index % (effect->particle_type == 7 ? 12 : 32)];
        *trail_particle = *particle;
        trail_particle->start_time += (f32)(trail + 1) * effect->trail_time;
    }

    if (effect->native_data == NULL) {
        GenericDebinfoDmaTypeUpdate(effect);
    }
    return particle;
}

#undef DEBRIS_KEYS_PC

extern LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
extern LEVELDATA_PC *MAULA_LDATA;
extern LEVELDATA_PC *MAULF_LDATA;
extern LEVELDATA_PC *DOOKUC_LDATA;
extern const char ForceThrowExplodeNamePc[9];
i32 __cdecl AddGameDebris(APIDEBRISSYS_PC *, i32, NUVEC *);
void PartKill_ForceThrow(PART_PC *part, i32 reason) {
    LEVELDATA_PC *level = WorldInfo_CurrentlyActive()->current_level;
    i32 debris = -1;
    i32 part_debris = -1;
    i32 type;
    if (level == CLOUDCITYTRAPB_LDATA) {
        debris = 0x75;
        part_debris = 8;
    } else if (level == MAULA_LDATA) {
        debris = 0x2d;
        part_debris = 9;
    } else if (level == MAULF_LDATA) {
        type = 11;
        if (NuSpecialExistsFn(&LevHSpecial[0]) && NuSpecialCompare(&LevHSpecial[0], &part->special))
            type = 11;
        else if (NuSpecialExistsFn(&LevHSpecial[1]) && NuSpecialCompare(&LevHSpecial[1], &part->special))
            type = 12;
        else if (NuSpecialExistsFn(&LevHSpecial[2]) && NuSpecialCompare(&LevHSpecial[2], &part->special))
            type = 13;
        part_debris = type;
    } else if (level == DOOKUC_LDATA) {
        debris = 0x34;
        part_debris = 10;
    }
    if (debris != -1)
        AddGameDebris(WORLD->game_debris_sys, debris, &part->position);
    if (part_debris != -1)
        AddPartDebris(WORLD->part_debris_sys, part_debris, &part->position);
    NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, qrand() > 0x7fff ? -0.4f : 0.4f, 2, NULL);
    GameCam_NewShake(GameCam, 0.6f, 0.6f, 1.0f);
    PlaySfx((char *)ForceThrowExplodeNamePc, &part->position);
}

#define DEBRIS_KEYS_PC ((DEBRISKEY_PC *)debkeydata)
static __declspec(noinline) f32 DebrisDivide(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    return a / b;
}

void __cdecl DebrisEmitterOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        DEBRISKEY_PC *key = &DEBRIS_KEYS_PC[handle];
        key->emitter_orientation = *source;
        key->emitter_orientation.m30 = 0.0f;
        key->emitter_orientation.m31 = 0.0f;
        key->emitter_orientation.m32 = 0.0f;
        key->orientation_dirty = 0.0f;
    }
}

void __cdecl AddVariableShotDebrisEffectTimed5(i32 effect_index, NUVEC *position, NUVEC *momentum,
                                               NUVEC *position_delta, i32 count, f32 duration,
                                               NUMTX *emitter_orientation, NUMTX *particle_orientation,
                                               u16 render_priority, i8 timed_flags) {
    DEBRIS_INFO_PC *effect;
    NUVEC clipped_position;
    NUVEC extent;
    f32 thinning;
    f32 emission_interval;
    f32 rate;
    f32 now;
    f32 end_time;
    f32 elapsed_intervals;
    f32 emission_epoch;
    f32 emission_time;
    f32 next_emission_time;
    i32 panel_time;
    i32 emission_count;
    i32 i;
    i32 j;
    i32 particle;
    i32 particle_count;
    i32 particles_per_chunk;
    i32 maximum_particles;
    i32 particle_key_slot;
    i32 new_key_index;
    DEBRISKEY_PC *key;
    i32 required_particles;
    i32 required_chunks;
    i32 allocated_chunks;
    i32 new_chunk_count;
    i32 *free_chunk_count;
    i32 available_chunk_count;
    DEBRIS_PARTICLE_CHUNK_PC **free_chunks;
    DEBRIS_PARTICLE_CHUNK_PC *chunk;
    i32 total_chunk_count;
    DEBRIS_RENDER_CHUNK_PC *render_chunk;
    DEBRISPARTICLE_PC *generated;
    i32 chunk_index;
    DEBRIS_CHUNK_CONTROL_PC *control;
    DEBRIS_CHUNK_CONTROL_PC **stack;
    DEBRIS_PARTICLE_CHUNK_PC *last_chunk;

    if (debris_suspended != 0 || update_debris_enabled == 0 || effect_index == -1 || effect_index == 0 ||
        effect_index >= debinf_count)
        return;
    effect = debtab[effect_index];
    if (effect == NULL || effect->disabled != 0 || count < 1)
        return;
    if (effect->time_group != 4) {
        clipped_position = *position;
        extent.x = 1.0f;
        extent.y = 1.0f;
        extent.z = 1.0f;
        if (NuCameraClipTestExtentsAxisAligned(&clipped_position, &extent, effect->clip_extent) == 0)
            return;
    }
    thinning = forced_debris_thinning != 0
                   ? debris_thinning_level
                   : (debris_thinning_level > effect->thinning ? effect->thinning : debris_thinning_level);
    rate = DebrisDivide((f32)count, thinning);
    emission_interval = DebrisDivide(1.0f, rate);
    if (emitter_orientation == NULL)
        emitter_orientation = &NuMtxSetIdentityTemplate;
    if (particle_orientation == NULL)
        particle_orientation = &NuMtxSetIdentityTemplate;
    panel_time = effect->time_group == 4;
    now = panel_time ? panelglobaltime : globaltime;
    end_time = now + duration;
    elapsed_intervals = (f32)(i32)DebrisDivide(now, emission_interval);
    emission_epoch = elapsed_intervals * emission_interval;
    emission_time = emission_interval + emission_epoch;
    if (end_time < emission_time)
        return;
    emission_count = 0;
    next_emission_time = emission_time;
    for (i = 1; i < 100 && next_emission_time <= end_time; ++i) {
        ++emission_count;
        next_emission_time += emission_interval;
    }
    if (emission_count == 0)
        return;
    particle_count = emission_count * ((i32)effect->trail_count + 1);
    particles_per_chunk = effect->particle_type == 7 ? 12 : 32;
    maximum_particles = effect->particle_type == 7 ? 0x180 : 0x400;
    particle_key_slot = -1;
    key = NULL;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] != -1) {
            key = &DEBRIS_KEYS_PC[effect->particle_keys[i]];
            if (key->particle_count + particle_count <= maximum_particles) {
                particle_key_slot = i;
                break;
            }
        } else if (freedebkeyptr < maxdebkeys) {
            new_key_index = DebAlloc();
            effect->particle_keys[i] = (i16)new_key_index;
            key = &DEBRIS_KEYS_PC[(i16)new_key_index];
            key->effect_index = (i16)effect_index;
            key->finite_count = 0;
            key->generator = gensorttab[effect->generator_type];
            key->momentum_adjuster = gencodetab[effect->momentum_adjustment_type];
            key->effect_orientation = *particle_orientation;
            key->effect_orientation.m30 = 0.0f;
            key->effect_orientation.m31 = 0.0f;
            key->effect_orientation.m32 = 0.0f;
            DebrisEmitterPos(effect->particle_keys[i], 0.0f, 0.0f, 0.0f);
            key->timed_flags = timed_flags;
            key->render_priority = render_priority;
            particle_key_slot = i;
            break;
        }
    }
    if (key == NULL)
        return;
    required_particles = key->particle_count + particle_count;
    allocated_chunks = key->allocated_chunk_count;
    if (required_particles > allocated_chunks * particles_per_chunk) {
        if (allocated_chunks < 0) {
            key->allocated_chunk_count = 0;
            allocated_chunks = 0;
        }
        required_chunks = (required_particles + particles_per_chunk - 1) / particles_per_chunk;
        new_chunk_count = required_chunks - allocated_chunks;
        free_chunk_count = effect->particle_type == 7 ? &freedebchkptrg : &freedebchkptr;
        available_chunk_count = effect->particle_type == 7 ? debrischunksglass : debrischunks;
        free_chunks = effect->particle_type == 7 ? freedebchunksglass : freedebchunks;
        if (*free_chunk_count + new_chunk_count >= available_chunk_count || required_chunks > 32)
            return;
        for (i = 0; i < new_chunk_count; ++i) {
            chunk = free_chunks[(*free_chunk_count)++];
            key->particle_chunks[key->allocated_chunk_count] = chunk;
            for (particle = 0; particle < particles_per_chunk; ++particle) {
                chunk->particles[particle].start_time = 0.0f;
                chunk->particles[particle].inverse_lifetime = 32768.0f;
            }
            ++key->allocated_chunk_count;
        }
        key->allocated_chunk_count = (i16)required_chunks;
        LinkDmaParticalSets(key->particle_chunks, required_chunks);
        if (required_chunks == new_chunk_count) {
            total_chunk_count = debrischunks + debrischunksglass;
            render_chunk = NULL;
            for (j = 0; j < total_chunk_count; ++j) {
                if (ParticleChunkToRender[j].particle_chunk == NULL) {
                    render_chunk = &ParticleChunkToRender[j];
                    break;
                }
            }
            if (render_chunk != NULL) {
                render_chunk->particle_chunk = key->particle_chunks[0];
                render_chunk->effect = effect;
                render_chunk->key = key;
                render_chunk->render_priority = render_priority;
                AddChunkToRenderStack(render_chunk, &ParticleChunkRenderStack[effect->time_group]);
            }
        }
    }
    key->particle_count += (i16)particle_count;
    if (momentum == NULL) {
        key->momentum.x = 0.0f;
        key->momentum.y = 0.0f;
        key->momentum.z = 0.0f;
    } else
        key->momentum = *momentum;
    DebrisEmitterOrientationMtx(effect->particle_keys[particle_key_slot], emitter_orientation);
    key->emission_epoch = emission_epoch;
    for (i = 1; i < 100 && emission_time <= end_time; ++i) {
        if (position_delta == NULL)
            key->emission_position = *position;
        else
            NuVecAddScale(&key->emission_position, position, position_delta, emission_time - end_time);
        key->emission_time = emission_time;
        generated = key->generator(key, effect, emission_time);
        if (effect->process_spheres != 0 && generated != NULL && i == 1)
            DebrisProcessSpheres(generated, emission_time, effect, key, 1);
        emission_time = key->emission_epoch + emission_interval;
    }
    DebrisGetControlStackLock();
    for (chunk_index = key->controlled_chunk_count; chunk_index < key->allocated_chunk_count; ++chunk_index) {
        if (freechunkcontrolsptr < (debrischunks + debrischunksglass) * 2) {
            control = freechunkcontrols[freechunkcontrolsptr];
            control->particle_chunk = key->particle_chunks[chunk_index];
            control->active = 1;
            control->owner = key;
            control->expiry_time = now + effect->particle_lifetime + (f32)effect->trail_count * effect->trail_time;
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time ? 1 : 0]);
            ++freechunkcontrolsptr;
            ++key->controlled_chunk_count;
        }
    }
    if (key->controlled_chunk_count == key->allocated_chunk_count) {
        stack = &debris_chunk_control_stack[panel_time ? 1 : 0];
        last_chunk = key->particle_chunks[key->controlled_chunk_count - 1];
        for (control = *stack; control != NULL; control = control->next) {
            if (control->particle_chunk == last_chunk) {
                RemoveChunkControlFromStack(control, stack);
                control->expiry_time = now + effect->particle_lifetime + (f32)effect->trail_count * effect->trail_time;
                AddChunkControlToStack(control, stack);
                break;
            }
        }
    }
    DebrisReleaseControlStackLock();
    key->previous_particle_count = key->particle_count;
    key->previous_allocated_chunk_count = key->allocated_chunk_count;
}

void __cdecl AddVariableShotDebrisEffectMtx3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count,
                                             NUMTX *emitter_orientation, NUMTX *particle_orientation) {
    u16 priority = 20000;
    if (effect < 0 || debtab[effect] == NULL)
        return;
    switch (debtab[effect]->particle_type) {
        case 2:
            priority = 40000;
            break;
        case 3:
            priority = 30000;
            break;
        case 7:
            priority = 10000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count * 30, debris_time_increment,
                                      emitter_orientation, particle_orientation, priority, 0);
}

void __cdecl AddVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectMtx3(effect, position, &APIObjectZeroMomentumPC, count, &orientation, NULL);
}

#undef DEBRIS_KEYS_PC

void __cdecl DebrisDrawCalculateClipBoxes(DEBRIS_INFO_PC *effect, DEBRISKEY_PC *key) {
    NUMTX matrix;
    f32 emission_time, lifetime, padding, horizontal, radius;
    NUVEC extent, minimum;
    union {
        f32 value;
        u32 bits;
    } velocity_x, velocity_y, velocity_z, momentum;
    DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
    NuMtxTranslate(&matrix, &key->position);
    emission_time = effect->emission_period + effect->emission_period_random;
    lifetime = effect->particle_lifetime;
    velocity_x.value = effect->emitter_velocity.x;
    velocity_y.value = effect->emitter_velocity.y;
    velocity_z.value = effect->emitter_velocity.z;
    momentum.value = effect->momentum_base;
    velocity_x.bits &= 0x7fffffff;
    velocity_y.bits &= 0x7fffffff;
    velocity_z.bits &= 0x7fffffff;
    momentum.bits &= 0x7fffffff;
    extent.x =
        (velocity_x.value * emission_time + effect->momentum_random_x * lifetime) + effect->emitter_random_extent.x;
    extent.y = (velocity_y.value * emission_time + (momentum.value + effect->rotor_speed_x) * lifetime) +
               effect->emitter_random_extent.y;
    extent.z = (velocity_z.value * emission_time + effect->rotor_speed_y * lifetime) + effect->emitter_random_extent.z;
    NuVecMtxRotate(&extent, &extent, &key->emitter_orientation);
    if (extent.x < 0.0f)
        extent.x = -extent.x;
    if (extent.y < 0.0f)
        extent.y = -extent.y;
    if (extent.z < 0.0f)
        extent.z = -extent.z;
    padding = (f32)((f64)0.0001f * effect->max_size);
    extent.x = (f32)((extent.x + padding) + (f64)0.2f);
    extent.y = (f32)((extent.y + padding) + (f64)0.2f);
    extent.z = (f32)((extent.z + padding) + (f64)0.2f);
    minimum.x = -extent.x;
    minimum.y = -extent.y;
    minimum.z = -extent.z;
    if (effect->gravity > 0.0f)
        extent.y += (effect->gravity * lifetime) * lifetime;
    else if (effect->gravity < 0.0f)
        minimum.y = (effect->gravity * lifetime) * lifetime - extent.y;
    horizontal = extent.x > extent.z ? extent.x : extent.z;
    if (extent.y > -minimum.y)
        radius = horizontal > extent.y ? horizontal : extent.y;
    else if (horizontal > -minimum.y)
        radius = horizontal;
    else
        radius = extent.y > -minimum.y ? extent.y : -minimum.y;
    key->clip_max = extent;
    key->clip_radius = radius;
    key->clip_min = minimum;
    DebrisMtxCopyPrivate(&key->clip_matrix, &matrix);
}

void __cdecl DebrisDraw(i32 unused, i32 pass) {
    DEBRIS_RENDER_CHUNK_PC *chunk;
    i32 drawn = 0;
    (void)unused;
    if (debris_suspended != 0)
        return;
    _NuTimeBarSlotBegin(0, 12, DebrisTimeBarName);
    DebMat[7]->texture_id = (i16)debris_rt;
    for (chunk = ParticleChunkRenderStack[pass]; chunk != NULL; chunk = chunk->next) {
        DEBRIS_INFO_PC *effect;
        DEBRISKEY_PC *key;
        NUMTX matrix;
        NUVEC position;
        f32 render_time;
        i32 mode;
        if (chunk->particle_chunk == NULL)
            continue;
        effect = chunk->effect;
        key = chunk->key;
        if (DebrisCutSceneMode != 0 && effect->cutscene_only != 0)
            continue;
        if (DebrisSuspendDrawObjectSwitch != -1 && key != NULL && key->trigger_second == DebrisSuspendDrawObjectSwitch)
            continue;
        if (effect->native_data == NULL) {
            if (freeDmaDebType == EDPP_MAX_DMADEBTYPES)
                DebrisFreeOldestDmaDebTypeTable();
            GenericDebinfoDmaTypeUpdate(effect);
            if (effect->native_data == NULL)
                continue;
        }
        if (key != NULL) {
            i32 assigned = 0;
            i32 visible = 1;
            i32 i;
            for (i = 0; i < 8; ++i) {
                if (effect->particle_keys[i] != -1 && &((DEBRISKEY_PC *)debkeydata)[effect->particle_keys[i]] == key) {
                    assigned = 1;
                    break;
                }
            }
            if (!assigned && effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                key->cutoff_distance > effect->sound_range)
                visible = 0;
            if (key->field_2f7 == 0)
                visible = 0;
            if (pass != 4 && visible && effect->generator_type == 0 && effect->use_explicit_clip_box == 0 &&
                !assigned) {
                NUVEC minimum, maximum;
                if (key->clip_radius == 0.0f || *edbits_editor_enabled != 0)
                    DebrisDrawCalculateClipBoxes(effect, key);
                if (edbits_editmode == 0 && key->gscene != NULL && key->field_2f2 != -1)
                    visible = NuPortalClipTest(key->gscene, &key->position, key->clip_radius, key->field_2f2) != 0;
                minimum = key->clip_min;
                maximum = key->clip_max;
                if (NuCameraClipTestExtents(&minimum, &maximum, &key->clip_matrix, 0.0f, 0) == 0)
                    continue;
            }
            if (!visible)
                continue;
            DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
            position = key->position;
        } else {
            if (effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                CameraEmitterDistance(&chunk->position) > effect->sound_range)
                continue;
            matrix = chunk->effect_orientation;
            position = chunk->position;
        }
        NuMtxTranslate(&matrix, &position);
        if (pass == 4) {
            NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
            render_time = debris_render_panel_time;
            effect->last_render_time = panelglobaltime;
        } else {
            render_time = debris_render_global_time;
            effect->last_render_time = globaltime;
        }
        if (key != NULL && key->field_2fa != 0)
            NuRndrSetParticleRotation(&key->particle_orientation);
        else if (effect->camera_facing != 0)
            NuRndrSetParticleRotation(&xzfacingmtx);
        else
            NuRndrSetParticleRotation(NULL);
        NuMtxPreScaleX(&matrix, debris_render_scale);
        if (effect->particle_type == 7)
            mode = 4;
        else if (effect->use_explicit_clip_box != 0) {
            NuRndrSetDebBox(&effect->particle_box);
            mode = 6;
        } else
            mode = 0;
        NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[(i8)chunk->effect->particle_type],
                            render_time, &matrix, mode, effect->particle_parameter_a, effect->particle_parameter_b,
                            effect->clip_extent, effect->field_044);
        drawn = 1;
    }
    if (drawn != 0)
        g_renderingDebris = g_renderingDebris == 0;
    _NuTimeBarSlotEnd(0, 12);
}
