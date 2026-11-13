#include "saga.h"
#include <stdlib.h>
#include <stdio.h>

/* Retail unit 0x58b720-0x58b8aa (legoapi/hits.c): definitions in retail address order. */

u16 __cdecl ObjHitObj_Flags(GAMEOBJECT *object) {
    u16 flags = 0;
    if (object != NULL) {
        if (((object->flags_1fc_word >> 7) & 1) == 0) {
            flags = 2;
        }
        if (((object->flags_1fc_word >> 7) & 1) != 0) {
            flags |= 0x804;
        }
        if ((object->flags_1f8 & 0x10001) != 0) {
            flags |= 0x10;
        } else if ((object->flags_1f8 & 4) != 0) {
            flags |= 0x20;
        } else {
            flags |= 8;
        }
    }
    return flags;
}

void __cdecl RegenerateHearts(GAMEOBJECT *object) {
    if (object->current_hp > 0 && object->current_hp < object->field_107e && object->vehicle_hit_timer <= 0.0f &&
        object->spawn_protection_timer <= 0.0f && ParticlesPerSecond(1.0f, FRAMETIME) > 0) {
        ++object->current_hp;
        GameAudio_PlaySfx(0x27, &object->collision_position, 0, 0);
    }
}

i32 __cdecl CannotKill(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x02000000) != 0) {
        return 1;
    }
    return (object->character_data->game_character->flags_094 & 0x800) != 0 && object->field_280 == 0xff;
}

i32 __cdecl NoLayerKill(GAMEOBJECT *object) {
    return (object->character_data->game_character->flags_094 & 0x800000) != 0 && object->field_283 == 6;
}

GAMEOBJECT *__cdecl BossKilled(i32 character_id) {
    GAMEOBJECT *object = FindGameObject(character_id, 1, 1, 1, 0);
    if (object != NULL && (object->portal_flags & 0x08000000) != 0) {
        if (object->dead != 0 || object->current_hp == 0) {
            return object;
        }
    }
    return NULL;
}

typedef struct combat_coinpacket_pc_s {
    u32 coins;
} COMBAT_COINPACKET_PC;

STATIC_ASSERT(offsetof(COMBAT_COINPACKET_PC, coins) == 0);

typedef struct shield_state_pc_s {
    u8 unknown_000[0xd30];
    f32 shield_enabled;
    f32 shield_timer;
} SHIELD_STATE_PC;

STATIC_ASSERT(offsetof(SHIELD_STATE_PC, shield_enabled) == 0xd30);
STATIC_ASSERT(offsetof(SHIELD_STATE_PC, shield_timer) == 0xd34);

typedef struct reaction_fields_pc_s {
    u8 unknown_00[0x6c4];
    u8 reset_contexts;
    u8 unknown_6c5[0x750 - 0x6c5];
    f32 animation_speed;
    u8 unknown_754[0x778 - 0x754];
    f32 reaction_duration;
} REACTION_FIELDS_PC;

STATIC_ASSERT(offsetof(REACTION_FIELDS_PC, reset_contexts) == 0x6c4);
STATIC_ASSERT(offsetof(REACTION_FIELDS_PC, animation_speed) == 0x750);
STATIC_ASSERT(offsetof(REACTION_FIELDS_PC, reaction_duration) == 0x778);

/* Full PC ObjHitObj source contract: complete 5764-byte original interval,
   physical calling convention and finite reader cohorts independently reviewed.
   PC-only reaction and adaptive branches and PC DWORD masks are preserved.
   Existing partial part/object providers retain their explicit runtime gaps. */
typedef struct combat_reaction_owner_pc_s {
    u8 unknown_000[0x7bb];
    i8 attacker_slot;
} COMBAT_REACTION_OWNER_PC;

STATIC_ASSERT(offsetof(COMBAT_REACTION_OWNER_PC, attacker_slot) == 0x7bb);

typedef struct combat_attack_counter_pc_s {
    u8 unknown_000[0x1083];
    u8 attack_count;
} COMBAT_ATTACK_COUNTER_PC;

STATIC_ASSERT(offsetof(COMBAT_ATTACK_COUNTER_PC, attack_count) == 0x1083);

typedef struct collision_exclusion_view_pc_s {
    u8 unknown_0000[0x298];
    GAMEOBJECT *excluded_object;
    u8 unknown_029c[4];
    u64 exclusion_mask;
} COLLISION_EXCLUSION_VIEW_PC;

STATIC_ASSERT(offsetof(COLLISION_EXCLUSION_VIEW_PC, excluded_object) == 0x298);
STATIC_ASSERT(offsetof(COLLISION_EXCLUSION_VIEW_PC, exclusion_mask) == 0x2a0);

typedef struct collision_platform_push_view_pc_s {
    u8 unknown_0000[0xdc4];
    f32 platform_push_timer;
} COLLISION_PLATFORM_PUSH_VIEW_PC;

STATIC_ASSERT(offsetof(COLLISION_PLATFORM_PUSH_VIEW_PC, platform_push_timer) == 0xdc4);

typedef struct collision_techno_view_pc_s {
    u8 unknown_0000[0x68];
    u8 flags;
} COLLISION_TECHNO_VIEW_PC;

STATIC_ASSERT(offsetof(COLLISION_TECHNO_VIEW_PC, flags) == 0x68);

typedef struct collision_masks_view_pc_s {
    u8 unknown_0000[0x1e8];
    u64 collision_identity_mask;
    u64 collision_contact_mask;
} COLLISION_MASKS_VIEW_PC;

STATIC_ASSERT(offsetof(COLLISION_MASKS_VIEW_PC, collision_identity_mask) == 0x1e8);
STATIC_ASSERT(offsetof(COLLISION_MASKS_VIEW_PC, collision_contact_mask) == 0x1f0);
