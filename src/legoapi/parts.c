#include "saga.h"

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
    PartDebrisCreatePart_candidate(entry->id, position, 1);
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
