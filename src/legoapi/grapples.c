#include "saga.h"

GRAPPLE_DYNAMIC_PC *__cdecl Grapple_AddDynamic(void *attached_object, i32 retain_attachment) {
    i32 index;
    GRAPPLE_DYNAMIC_PC *grapple;
    for (index = 0; index < 4; ++index) {
        if (DynamicGrapplePc[index].attached_object == attached_object)
            return &DynamicGrapplePc[index];
    }
    for (index = 0; index < 4; ++index) {
        grapple = &DynamicGrapplePc[index];
        if (grapple->attached_object == NULL) {
            grapple->attached_object = (GAMEOBJECT *)attached_object;
            grapple->retain_attachment = (u8)retain_attachment;
            grapple->flags |= 2;
            return grapple;
        }
    }
    return NULL;
}
GRAPPLE_DYNAMIC_PC *__cdecl Grapple_RemoveDynamic(void *attached_object) {
    i32 index;
    GRAPPLE_DYNAMIC_PC *grapple;
    for (index = 0; index < 4; ++index) {
        grapple = &DynamicGrapplePc[index];
        if (grapple->attached_object == attached_object) {
            grapple->attached_object = NULL;
            grapple->flags &= ~2;
            return grapple;
        }
    }
    return NULL;
}
