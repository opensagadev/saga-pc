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

extern i32 LEGOCONTEXT_GRAPPLE;
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) +
                  offsetof(AIPATHINFO_PC, connection) ==
              0x424);
GAMEOBJECT *__cdecl Grapple_Occupied(void *grapple, GAMEOBJECT *excluded, AIPATHCNX_PC *connection) {
    GAMEOBJECT *object;
    i32 index, count, jump_context;
    if (LEGOCONTEXT_GRAPPLE == -1)
        return NULL;
    object = Obj;
    count = HIGHGAMEOBJECT;
    jump_context = LEGOCONTEXT_JUMP;
    for (index = 0; index < count; ++index, ++object) {
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object == excluded)
            continue;
        if (object->character_context == LEGOCONTEXT_GRAPPLE && object->grapple == grapple)
            return object;
        if (connection != NULL && object->ai_packet.path_info.connection == connection && jump_context != -1 &&
            object->character_context == jump_context)
            return object;
    }
    return NULL;
}
