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
extern char TextInputKeyNameTextPc7516c0[1];
extern char *BlowupOutputNamesPc[3];
extern char *PickupOutputNamePc;
extern char SpinnerOutputBufferPc[14];
extern char AIProcessingOutputTextPc[11];
extern const char SpinnerCompleteSuffixPc[12];
extern const char SpinnerAllCompletePc[15];

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
