#include "saga.h"

extern "C" {
    void __cdecl RegisterGizmoTypes(VARIPTR *, VARIPTR *, REGISTER_GIZMO_CONFIG_TYPE_FN *, i32);
    ADDGIZMOTYPE_PC *__cdecl GizObstacles_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizBuildIts_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizForce_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl NewBlowup_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizmoPickups_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Levers_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Spinner_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl MiniCut_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Tubes_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl ZipUps_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizTurrets_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizBombGen_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl AI_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizSpecial_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizAIMessage_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizTimer_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizRandom_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizPanel_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl HatMachine_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Door_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Teleport_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl GizTorpMachine_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl EdGizShadowMachine_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Portal_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Grapples_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Plugs_RegisterGizmo(i32);
    ADDGIZMOTYPE_PC *__cdecl Technos_RegisterGizmo(i32);
}
ADDGIZMOTYPE_PC *__cdecl Push_RegisterGizmo(i32);

extern "C" void __cdecl RegisterLswGizmoTypesPc(VARIPTR *buffer, VARIPTR *end) {
    REGISTER_GIZMO_CONFIG_TYPE_FN functions[29];
    functions[0] = GizObstacles_RegisterGizmo;
    functions[1] = GizBuildIts_RegisterGizmo;
    functions[2] = GizForce_RegisterGizmo;
    functions[3] = NewBlowup_RegisterGizmo;
    functions[4] = GizmoPickups_RegisterGizmo;
    functions[5] = Levers_RegisterGizmo;
    functions[6] = Spinner_RegisterGizmo;
    functions[7] = MiniCut_RegisterGizmo;
    functions[8] = Tubes_RegisterGizmo;
    functions[9] = ZipUps_RegisterGizmo;
    functions[10] = GizTurrets_RegisterGizmo;
    functions[11] = GizBombGen_RegisterGizmo;
    functions[12] = AI_RegisterGizmo;
    functions[13] = GizSpecial_RegisterGizmo;
    functions[14] = GizAIMessage_RegisterGizmo;
    functions[15] = GizTimer_RegisterGizmo;
    functions[16] = GizRandom_RegisterGizmo;
    functions[17] = GizPanel_RegisterGizmo;
    functions[18] = HatMachine_RegisterGizmo;
    functions[19] = Push_RegisterGizmo;
    functions[20] = Door_RegisterGizmo;
    functions[21] = Teleport_RegisterGizmo;
    functions[22] = GizTorpMachine_RegisterGizmo;
    functions[23] = EdGizShadowMachine_RegisterGizmo;
    functions[24] = Portal_RegisterGizmo;
    functions[25] = Grapples_RegisterGizmo;
    functions[26] = Plugs_RegisterGizmo;
    functions[27] = Technos_RegisterGizmo;
    functions[28] = NULL;
    RegisterGizmoTypes(buffer, end, functions, 12);
}
