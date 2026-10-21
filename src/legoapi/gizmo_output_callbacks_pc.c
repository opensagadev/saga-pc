#include "saga.h"

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZMO_PC, type_id) == 6);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(GIZSPINNER_s, output_count) == 0xa4);
STATIC_ASSERT(offsetof(GIZSPINNER_s, flags) == 0xac);
STATIC_ASSERT(offsetof(GIZSPINNER_s, anim_set) == 0x68);
STATIC_ASSERT(sizeof(GIZSPINNER_s) == 0x304);
STATIC_ASSERT(offsetof(WORLDINFO, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinners) == 0xf2);
STATIC_ASSERT(offsetof(GAMEANIMSET, objects) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, next) == 0);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, special) == 4);
STATIC_ASSERT(offsetof(GIZTURRET_s, field_0x131) == 0x131);
extern char AIRegistryTextPc007575bc[7];
extern char TextInputKeyNameTextPc7516c0[1];
extern char *BlowupOutputNamesPc[3];
extern char *PickupOutputNamePc;
extern char SpinnerOutputBufferPc[14];
extern char TurretOutputBufferPc[32];
extern char AIProcessingOutputTextPc[11];
extern const char GrappleOccupiedOutputTextPc[9];
extern const char GrappleOccupiedByTwoOutputTextPc[14];
extern const char SpinnerCompleteSuffixPc[12];
extern const char SpinnerAllCompletePc[15];
extern const char TurretDestroyedOutputTextPc[10];
extern const char TurretFiredOutputTextPc[6];
extern const char TurretFiredShotsFormatPc[15];

char *__cdecl GizmoBuildit_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return "Finished";
}
i32 __cdecl GizmoBuildit_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
char *__cdecl Blowup_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    if ((u32)output <= 2)
        return BlowupOutputNamesPc[output];
    return NULL;
}
i32 __cdecl Blowup_GetVisibility(GIZMO_PC *gizmo) {
    return gizmo != NULL && (((GIZMOBLOWUP_PC *)gizmo->object)->status_flags & 0x4000) != 0;
}
char *__cdecl GizmoPickup_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return PickupOutputNamePc;
}
char *__cdecl Grapple_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    if (output == 1)
        return (char *)GrappleOccupiedOutputTextPc;
    if (output == 2)
        return (char *)GrappleOccupiedByTwoOutputTextPc;
    return AIRegistryTextPc007575bc;
}
char *__cdecl GizSpinner_GetOutputName(void *gizmo, i32 output) {
    GIZSPINNER_s *spinner = (GIZSPINNER_s *)((GIZMO_PC *)gizmo)->object;
    if (output < 0)
        return NULL;
    if (output > spinner->output_count)
        return NULL;
    if (spinner->output_count >= 2) {
        f32 fraction = (f32)output / (spinner->output_count - 1);
        f32 percentage = (f64)fraction * 100.0;
        NuIToA((i32)percentage, SpinnerOutputBufferPc, 10);
        NuStrCat(SpinnerOutputBufferPc, (char *)SpinnerCompleteSuffixPc);
    } else {
        char *name = (char *)SpinnerAllCompletePc;
        if (spinner->output_count == 0)
            name = TextInputKeyNameTextPc7516c0;
        NuStrCpy(SpinnerOutputBufferPc, name);
    }
    return SpinnerOutputBufferPc;
}
i32 __cdecl GizSpinner_GetNumOutputs(void *gizmo) {
    return ((GIZSPINNER_s *)((GIZMO_PC *)gizmo)->object)->output_count;
}
i32 __cdecl GizSpinner_UsingSpecial(void *results_ptr, void *world_ptr, i32 capacity, char *name) {
    GIZMO_PC **results = (GIZMO_PC **)results_ptr;
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 count = 0;
    i32 type_id = -1;
    i32 index;
    NUSPECIALHANDLE special;
    GIZSPINNER_s *cached_spinner;
    if (results == NULL || world == NULL || world->spinners == NULL)
        return 0;
    NuSpecialFind(world->current_gscn, &special, name, 0);
    if (NuSpecialExistsFn(&special) == 0)
        return 0;
    cached_spinner = world->spinners;
    for (index = 0; index < world->current_level->max_spinners; ++index, ++cached_spinner) {
        GAMEANIMOBJ *object;
        i32 searching;
        if (count >= capacity)
            break;
        if ((world->spinners[index].flags & 1) == 0)
            continue;
        object = cached_spinner->anim_set->objects;
        searching = 1;
        for (; object != NULL && searching != 0; object = object->next) {
            if (NuSpecialCompare(&special, &object->special) != 0) {
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, type_id, name);
                if (gizmo != NULL) {
                    type_id = (u8)gizmo->type_id;
                    results[count++] = gizmo;
                    searching = 0;
                }
            }
        }
    }
    return count;
}
char *__cdecl GizmoTurret_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    switch (output) {
        case 0:
            return (char *)TurretDestroyedOutputTextPc;
        case 1:
            return (char *)TurretFiredOutputTextPc;
        case 2: {
            GIZTURRET_s *turret = (GIZTURRET_s *)gizmo->object;
            if (turret == NULL)
                return NULL;
            sprintf(TurretOutputBufferPc, TurretFiredShotsFormatPc, turret->field_0x131);
            return TurretOutputBufferPc;
        }
        default:
            return NULL;
    }
}
i32 __cdecl GizmoTurret_GetNumOutputs(GIZMO_PC *gizmo) {
    return 3;
}
char *__cdecl AI_GetOutputName(void *gizmo, i32 output) {
    return AIProcessingOutputTextPc;
}
i32 __cdecl AI_GetNumOutputs(void *gizmo) {
    return 1;
}

#pragma const_seg(".rdata$BlowupBlownupOutputTextPc")
const char BlowupBlownupOutputTextPc[8] = "Blownup";
#pragma const_seg()

#pragma const_seg(".rdata$BlowupPunchedOutputTextPc")
const char BlowupPunchedOutputTextPc[8] = "Punched";
#pragma const_seg()

#pragma const_seg(".rdata$BlowupPluggingOutputTextPc")
const char BlowupPluggingOutputTextPc[9] = "Plugging";
#pragma const_seg()

#pragma const_seg(".rdata$PickupCollectedOutputTextPc")
const char PickupCollectedOutputTextPc[10] = "Collected";
#pragma const_seg()

#pragma const_seg(".rdata$GrappleOccupiedOutputTextPc")
const char GrappleOccupiedOutputTextPc[9] = "Occupied";
#pragma const_seg()

#pragma const_seg(".rdata$GrappleOccupiedByTwoOutputTextPc")
const char GrappleOccupiedByTwoOutputTextPc[14] = "Occupied By 2";
#pragma const_seg()

#pragma const_seg(".rdata$SpinnerCompleteSuffixPc")
const char SpinnerCompleteSuffixPc[12] = "%% Complete";
#pragma const_seg()

#pragma const_seg(".rdata$SpinnerAllCompletePc")
const char SpinnerAllCompletePc[15] = "100%% Complete";
#pragma const_seg()

#pragma const_seg(".rdata$TurretDestroyedOutputTextPc")
const char TurretDestroyedOutputTextPc[10] = "destroyed";
#pragma const_seg()

#pragma const_seg(".rdata$TurretFiredOutputTextPc")
const char TurretFiredOutputTextPc[6] = "fired";
#pragma const_seg()

#pragma const_seg(".rdata$TurretFiredShotsFormatPc")
const char TurretFiredShotsFormatPc[15] = "Fired %d Shots";
#pragma const_seg()

#pragma data_seg(".data$BlowupOutputNamesPc")
char *BlowupOutputNamesPc[3] = {(char *)BlowupBlownupOutputTextPc, (char *)BlowupPunchedOutputTextPc,
                                (char *)BlowupPluggingOutputTextPc};
#pragma data_seg()

#pragma data_seg(".data$PickupOutputNamePc")
char *PickupOutputNamePc = (char *)PickupCollectedOutputTextPc;
#pragma data_seg()

#pragma data_seg(".data$SpinnerOutputBufferPc")
char SpinnerOutputBufferPc[14] = "100% Complete";
#pragma data_seg()

#pragma data_seg(".data$TurretOutputBufferPc")
char TurretOutputBufferPc[32] = {0};
#pragma data_seg()

#pragma data_seg(".data$AIProcessingOutputTextPc")
char AIProcessingOutputTextPc[11] = "Processing";
#pragma data_seg()
