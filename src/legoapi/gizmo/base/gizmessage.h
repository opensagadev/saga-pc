#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/nucore/nulist.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct gizaimessage_s { // size 0x38
        NULISTLNK links;            // 0x00
        char name[32];              // 0x08
        f32 value;                  // 0x28
        i8 output_values[8];        // 0x2c
        i8 output_count;            // 0x34
        u8 reserved_35;              // 0x35
        u16 flags;                  // 0x36
    } GIZAIMESSAGE;

    typedef struct gizaimessagesys_s { // size 0x18
        i32 count;                     // 0x00
        GIZAIMESSAGE *messages;        // 0x04
        NULISTHDR free_list;           // 0x08
        NULISTHDR active_list;         // 0x10
    } GIZAIMESSAGESYS;

    void ResetGizAIMessageSys(GIZAIMESSAGESYS *sys);
    void ClearGizAIMessageSys(GIZAIMESSAGESYS *sys);
    GIZAIMESSAGE *CheckGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message);
    GIZAIMESSAGE *SetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, f32 value, GIZAIMESSAGE *message);
    char *GizAIMessage_GetName(GIZAIMESSAGE *message);
    f32 GetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message);
    GIZAIMESSAGE *QueryGizAIMessage(GIZAIMESSAGESYS *sys, GIZAIMESSAGE *message);
    extern char gizaimessage_prefix[5];
    extern const char GizAIMessage_PrefixedNameFormat[5];
    GIZAIMESSAGESYS *CreateGizAIMessageSys(VARIPTR *cursor, VARIPTR *end, i32 count);
    void GizAIMessage_AddGizmos(struct gizmosys_s *gizmo_sys, i32 arg1, void *arg2, void *arg3);
    char *GizAIMessage_GetGizmoName(GIZMO *gizmo);
    i32 GizAIMessage_GetMaxGizmos(void *arg0);
    i32 GizAIMessage_GetNumOutputs(GIZMO *gizmo);
    i32 GizAIMessage_GetOutput(GIZMO *gizmo, i32 output_index, i32 arg2);
    char *GizAIMessage_GetOutputName(GIZMO *gizmo, i32 output_index);
    extern i32 gizaimessage_gizmotype_id;
    ADDGIZMOTYPE *GizAIMessage_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
