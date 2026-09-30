#include "saga.h"

void __cdecl GizmoSetVisibility(void *system, void *gizmo_ptr, i32 visible, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;

    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return;

    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return;

    type = &registry->types[gizmo->type_id];
    if (type->set_visibility == NULL)
        return;

    if (visible == 0 && type->activate != NULL)
        type->activate(gizmo, 1);

    type->set_visibility(gizmo, visible);
    (void)mode;
}

void __cdecl GizmoActivate(void *system, void *gizmo_ptr, i32 active, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;

    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return;

    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return;

    type = &registry->types[gizmo->type_id];
    if (type->activate == NULL)
        return;

    if (active == 0 && type->set_visibility != NULL) {
        type->set_visibility(gizmo, 0);
        registry = (GIZMOTYPES_PC *)gizmotypes;
        type = &registry->types[gizmo->type_id];
    }

    type->activate(gizmo, active);
    (void)mode;
}

i32 __cdecl GizmoGetOutput(void *system, void *gizmo_ptr, i32 output, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return 0;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return 0;
    type = &registry->types[gizmo->type_id];
    if (type->get_output == NULL)
        return 0;
    return type->get_output(gizmo, output, mode);
}

char *__cdecl GizmoGetOutputName(void *system, void *gizmo_ptr, i32 output) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    if (gizmotypes == NULL || system == NULL || gizmo == NULL)
        return NULL;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    if (gizmo->type_id >= registry->count)
        return (char *)GizmoDefaultOutputName;
    type = &registry->types[gizmo->type_id];
    if (type->get_output_name == NULL)
        return (char *)GizmoDefaultOutputName;
    return type->get_output_name(gizmo, output);
}

void __cdecl GizmoActivateReverse(void *system, void *gizmo_ptr, i32 reverse, i32 visibility, i32 mode) {
    GIZMOTYPES_PC *registry;
    GIZMOTYPE_PC *type;
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_ptr;
    i32 command = visibility == 0 ? 2 : 6;
    if (gizmo == NULL || gizmotypes == NULL)
        return;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    type = &registry->types[gizmo->type_id];
    if (type->activate_reverse == NULL)
        return;
    if (type->activate_reverse(gizmo, reverse, command | 1) == 0)
        return;
    registry = (GIZMOTYPES_PC *)gizmotypes;
    type = &registry->types[gizmo->type_id];
    type->activate_reverse(gizmo, reverse, command);
    if (visibility != 0)
        GizmoActivate(system, gizmo, reverse == 0, 1);
    (void)mode;
}
