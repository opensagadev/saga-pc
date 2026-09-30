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
