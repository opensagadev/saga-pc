#include "saga.h"
#include <stdlib.h>
#include <stdio.h>

u16 ObjHitObj_Flags(GAMEOBJECT *object) {
    u16 flags = 0;
    if (object != NULL) {
        if (((object->flags_1fc_word >> 7) & 1) == 0) {
            flags = 2;
        }
        if (((object->flags_1fc_word >> 7) & 1) != 0) {
            flags |= 0x804;
        }
        if ((object->flags_1f8 & (APIOBJECT_SIDE_BADDIE | APIOBJECT_SIDE_GOODIE_BADDIE)) != 0) {
            flags |= 0x10;
        } else if ((object->flags_1f8 & APIOBJECT_SIDE_NEUTRAL) != 0) {
            flags |= 0x20;
        } else {
            flags |= 8;
        }
    }
    return flags;
}

void RegenerateHearts(GAMEOBJECT *object) {
    if (object->current_hp > 0 && object->current_hp < object->unknown_107e && object->vehicle_hit_timer <= 0.0f &&
        object->spawn_protection_timer <= 0.0f && ParticlesPerSecond(1.0f, FRAMETIME) > 0) {
        ++object->current_hp;
        GameAudio_PlaySfx(0x27, &object->collision_position, 0, 0);
    }
}

i32 CannotKill(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x02000000) != 0) {
        return 1;
    }
    return (object->character_data->game_character->flags_094 & 0x800) != 0 && object->player_index == -1;
}

i32 NoLayerKill(GAMEOBJECT *object) {
    return (object->character_data->game_character->flags_094 & 0x800000) != 0 && object->unknown_283 == 6;
}

GAMEOBJECT *BossKilled(i32 character_id) {
    GAMEOBJECT *object = FindGameObject(character_id, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
    if (object != NULL && (object->portal_flags & 0x08000000) != 0) {
        if (object->dead != 0 || object->current_hp == 0) {
            return object;
        }
    }
    return NULL;
}

/* Full PC ObjHitObj source contract: complete 5764-byte original interval,
   physical calling convention and finite reader cohorts independently reviewed.
   PC-only reaction and adaptive branches and PC DWORD masks are preserved.
   Existing partial part/object providers retain their explicit runtime gaps. */

typedef struct collision_exclusion_view_s { // size 0x2a8
    u8 unknown_0000[0x298];                 // 0x000
    GAMEOBJECT *excluded_object;            // 0x298
    u8 unknown_029c[4];                     // 0x29c
    u64 exclusion_mask;                     // 0x2a0
} COLLISION_EXCLUSION_VIEW;
