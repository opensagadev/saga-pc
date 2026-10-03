#include "saga.h"
typedef struct whip_basis_pc_s {
    u8 unknown_000[0x748];
    NUVEC basis[3];
} WHIP_BASIS_PC;
STATIC_ASSERT(offsetof(WHIP_BASIS_PC, basis) == 0x748);
void __cdecl SuperCarry_Throw(GAMEOBJECT *, i32);
void __cdecl Whip_Release(GAMEOBJECT *object) {
    WHIP_BASIS_PC *view = (WHIP_BASIS_PC *)object;
    if (LEGOCONTEXT_WHIP != -1 && object->character_context == LEGOCONTEXT_WHIP && object->grapple != NULL) {
        view->basis[0] = v100;
        view->basis[1] = v010;
        view->basis[2] = v001;
        SuperCarry_Throw(object, 1);
    }
}
