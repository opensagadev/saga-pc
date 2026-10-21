#include "saga.h"

STATIC_ASSERT(sizeof(REGISTER_GIZMO_CONFIG_TYPE_FN) == 4);
extern "C" {
    void __cdecl RegisterGizmoTypes(VARIPTR *, VARIPTR *, REGISTER_GIZMO_CONFIG_TYPE_FN *, i32);
    ADDGIZMOTYPE_PC *__cdecl GizObstacles_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizBuildIts_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl NewBlowup_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizmoPickups_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Shards_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Signals_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Grapples_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl TightRopes_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Ledges_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Levers_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Spinner_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Technos_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl SecurityDoors_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Attractos_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl MiniCut_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GuideLines_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Tubes_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl ZipUps_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizTurrets_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl AI_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizTimer_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizRandom_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizSpecial_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Door_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizAIMessage_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl EdGizShadowMachine_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Portal_RegisterGizmo(i32);
}
ADDGIZMOTYPE_PC *__cdecl Push_RegisterGizmo(i32);

extern "C" void __cdecl RegisterAlternateGizmoTypesPc(VARIPTR *buffer, VARIPTR *end) {
    REGISTER_GIZMO_CONFIG_TYPE_FN functions[29];
    functions[0] = GizObstacles_RegisterGizmo;
    functions[1] = GizBuildIts_RegisterGizmo;
    functions[2] = NewBlowup_RegisterGizmo;
    functions[3] = GizmoPickups_RegisterGizmo;
    functions[4] = Shards_RegisterGizmo;
    functions[5] = Signals_RegisterGizmo;
    functions[6] = Grapples_RegisterGizmo;
    functions[7] = TightRopes_RegisterGizmo;
    functions[8] = Ledges_RegisterGizmo;
    functions[9] = Levers_RegisterGizmo;
    functions[10] = Spinner_RegisterGizmo;
    functions[11] = Technos_RegisterGizmo;
    functions[12] = SecurityDoors_RegisterGizmo;
    functions[13] = Attractos_RegisterGizmo;
    functions[14] = MiniCut_RegisterGizmo;
    functions[15] = GuideLines_RegisterGizmo;
    functions[16] = Tubes_RegisterGizmo;
    functions[17] = ZipUps_RegisterGizmo;
    functions[18] = GizTurrets_RegisterGizmo;
    functions[19] = AI_RegisterGizmo;
    functions[20] = GizTimer_RegisterGizmo;
    functions[21] = GizRandom_RegisterGizmo;
    functions[22] = GizSpecial_RegisterGizmo;
    functions[23] = Door_RegisterGizmo;
    functions[24] = GizAIMessage_RegisterGizmo;
    functions[25] = Push_RegisterGizmo;
    functions[26] = EdGizShadowMachine_RegisterGizmo;
    functions[27] = Portal_RegisterGizmo;
    functions[28] = NULL;
    RegisterGizmoTypes(buffer, end, functions, 12);
}
