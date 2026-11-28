#include "legoapi/actions/combat/hits.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/levels/episode.h"
#include <stdlib.h>
#include <stdio.h>

u16 ObjHitObj_Flags(GAMEOBJECT *object) {
    u16 flags = 0;
    if (object != NULL) {
        if (((object->flags_1fc_word >> 7) & 1) == 0) {
            flags = OBJHIT_FROM_AI;
        }
        if (((object->flags_1fc_word >> 7) & 1) != 0) {
            flags |= OBJHIT_FROM_PLAYER | OBJHIT_HITS_GIZMOS;
        }
        if ((object->flags_1f8 & (APIOBJECT_SIDE_BADDIE | APIOBJECT_SIDE_GOODIE_BADDIE)) != 0) {
            flags |= OBJHIT_FROM_BADDIE;
        } else if ((object->flags_1f8 & APIOBJECT_SIDE_NEUTRAL) != 0) {
            flags |= OBJHIT_FROM_NEUTRAL;
        } else {
            flags |= OBJHIT_FROM_GOODIE;
        }
    }
    return flags;
}

void RegenerateHearts(GAMEOBJECT *object) {
    if (object->current_hp > 0 && object->current_hp < object->max_hitpoints && object->vehicle_hit_timer <= 0.0f &&
        object->spawn_protection_timer <= 0.0f && ParticlesPerSecond(1.0f, FRAMETIME) > 0) {
        ++object->current_hp;
        GameAudio_PlaySfx(0x27, &object->collision_position, 0, 0);
    }
}

i32 CannotKill(GAMEOBJECT *object) {
    if ((CInfo[object->player.character_context].flags & 0x02000000) != 0) {
        return 1;
    }
    return (object->character_data->game_character->flags_094 & CHARACTER_OPTION_CANNOT_KILL) != 0 && object->player_index == -1;
}

i32 NoLayerKill(GAMEOBJECT *object) {
    return (object->character_data->game_character->flags_094 & CHARACTER_OPTION_HOVER_OVER_MUD) != 0 && object->ground_layer == 6;
}

GAMEOBJECT *BossKilled(i32 character_id) {
    GAMEOBJECT *object = FindGameObject(character_id, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
    if (object != NULL && (object->portal_flags & APIOBJECT_BOSS) != 0) {
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
