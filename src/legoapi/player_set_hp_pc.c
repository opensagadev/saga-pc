#include "saga.h"

/* Original source name unknown. Retail places this with the game object code
   at 0x414d10, and PlayersDropInOut calls it rather than inlining it, so the
   definition lives in its own unit. */
void __cdecl PlayerSetCurrentHpPc(GAMEOBJECT *object, i8 hitpoints) {
    object->current_hp = hitpoints;
    if ((i32)hitpoints > (i32)object->field_107e)
        object->current_hp = object->field_107e;
}
